// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ML.CPP -
//
// Marshaling engine.

#include "common.h"
#include "vars.hpp"
#include "ml.h"
#include "excep.h"
#include "frames.h"
#include "interoputil.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "comvariant.h"
#include "comdelegate.h"
#include "comvarargs.h"
#include "invokeutil.h"
#include "fieldmarshaler.h"
#include "marshalnative.h"
#include "comvariant.h"
#include "marshaler.h"
#include "perfcounters.h"


#ifdef _DEBUG

VOID DisassembleMLStream(const MLCode *pMLCode)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMLCode));
    }
    CONTRACTL_END;

    MLCode mlcode;
    while (ML_END != (mlcode = *(pMLCode++)))
    {
        UINT numOperands = gMLInfo[mlcode].m_numOperandBytes;
        LOG((LF_INTEROP, LL_INFO10000, "  %-20s ", gMLInfo[mlcode].m_szDebugName));
        for (UINT i = 0; i < numOperands; i++)
        {
            LOG((LF_INTEROP, LL_INFO10000, "%lxh ", (ULONG)*(pMLCode++)));
        }
        LOG((LF_INTEROP, LL_INFO10000, "\n"));
    }
    LOG((LF_INTEROP, LL_INFO10000, "  ML_END\n"));
}
#endif


UINT SizeOfML_OBJECT_C2N_SR()
{
    LEAF_CONTRACT;
    return sizeof(ML_OBJECT_C2N_SR);
}


//----------------------------------------------------------------------
// Generate a database of MLCode information.
//----------------------------------------------------------------------
const MLInfo gMLInfo[] =
{
#undef DEFINE_ML
#ifdef _DEBUG
#define DEFINE_ML(name,operandbytes,frequiredCleanup,cblocals, hndl) {operandbytes,frequiredCleanup, ((cblocals)+3) & ~3, hndl, #name},
#else
#define DEFINE_ML(name,operandbytes,frequiredCleanup,cblocals, hndl) {operandbytes,frequiredCleanup, ((cblocals)+3) & ~3, hndl},
#endif

#include "mlopdef.h"
};


//----------------------------------------------------------------------
// struct to compute the summary of a series of ML codes
//----------------------------------------------------------------------

VOID MLSummary::ComputeMLSummary(const MLCode *pMLCode)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMLCode));
    }
    CONTRACTL_END;
    
    const MLCode *pml = pMLCode;
    MLCode code;
    while (ML_END != (code = *(pml++))) 
    {
        m_fRequiresCleanup = m_fRequiresCleanup || gMLInfo[code].m_frequiresCleanup; 
        m_cbTotalHandles += (gMLInfo[code].m_frequiresHandle ? 1 : 0);
        pml += gMLInfo[code].m_numOperandBytes; 
        m_cbTotalLocals+= gMLInfo[code].m_cbLocal;
    }
    
    m_cbMLSize = (unsigned)(pml - pMLCode);
}


//----------------------------------------------------------------------
// Computes the length of an MLCode stream in bytes, including
// the terminating ML_END opcode.
//----------------------------------------------------------------------
UINT MLStreamLength(const MLCode * const pMLCode)
{
    WRAPPER_CONTRACT;
    
    MLSummary summary;
    summary.ComputeMLSummary(pMLCode);
    return summary.m_cbMLSize;
}


//----------------------------------------------------------------------
// checks if MLCode stream requires cleanup
//----------------------------------------------------------------------

BOOL MLStreamRequiresCleanup(const MLCode  *pMLCode)
{
    WRAPPER_CONTRACT;
    
    MLSummary summary;
    summary.ComputeMLSummary(pMLCode);
    return summary.m_fRequiresCleanup;
}


//----------------------------------------------------------------------
// Executes MLCode up to the next ML_END or ML_INTERRUPT opcode.
//
// Inputs:
//    psrc             - sets initial value of SRC register
//    pdst             - sets initial value of DST register
//    plocals          - pointer to ML local var array
//    dstinc           - -1 or +1 depending on direction of DST movement.
//                       (the SRC always moves in the +1 direction)
//    pCleanupWorkList - (optional) pointer to initialized
//                       CleanupWorkList. this pointer may be NULL if none
//                       of the opcodes in the MLCode stream uses it.
//----------------------------------------------------------------------

VOID DisassembleMLStream(const MLCode *pMLCode);

const MLCode *
RunML(const MLCode    *       pMLCode,
      const VOID      *       psrc,
            VOID      *       pdst,
            UINT8     * const plocals,
      CleanupWorkList * const pCleanupWorkList
#if defined(TRACK_FLOATING_POINT_REGISTERS)
             , DOUBLE *       pFloatRegs /* = NULL */
#endif // TRACK_FLOATING_POINT_REGISTERS
     )
{
    CONTRACT (const MLCode*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        
        PRECONDITION(CheckPointer(pMLCode));
        PRECONDITION(CheckPointer(psrc));
        PRECONDITION(CheckPointer(pdst));
        PRECONDITION(CheckPointer(plocals));
        PRECONDITION(CheckPointer(pCleanupWorkList, NULL_OK));
        
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("RunML");

#if BIGENDIAN
#define ENDIANFIX(size)     (size < 3 ? sizeof(void *) - size : 0)
#else
#define ENDIANFIX(size)     0
#endif

#ifdef _X86_
    #define STDSTSIZE(type,val,cb)  do { (BYTE*&)pdst -= (cb); *(          type*)pdst = (val);                       } while (0)
#else
    #define STDSTSIZE(type,val,cb)  do {                       *(UNALIGNED type*)pdst = (val); (BYTE*&)pdst += (cb); } while (0)
#endif
    #define STDST(type,val)         STDSTSIZE(type, val, sizeof(type))


    #define STPTRDST(type, val)     STDST(type, val)

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
    #define _RAWPTRSRC(ptr,size)    ((size) > ENREGISTERED_PARAMTYPE_MAXSIZE ? *(void**)(ptr) : (void*)(ptr))
    #define _SRCSIZE(size)          StackElemSize((size) > ENREGISTERED_PARAMTYPE_MAXSIZE ? sizeof(void*) : (size))
#else
    #define _RAWPTRSRC(ptr,size)    ((void*)(ptr))
    #define _SRCSIZE(size)          StackElemSize(size)
#endif

    #define LDSRC(type)             (INCSRC(type), *(type*)_RAWPTRSRC((BYTE*)psrc-_SRCSIZE(sizeof(type)), sizeof(type)))
    #define PTRSRC(type)            ((type*)_RAWPTRSRC(psrc, sizeof(type)))
    #define INCSRC(type)            ((BYTE*&)psrc)+=_SRCSIZE(sizeof(type))

    #define INCLOCAL(size)          plocalwalk += ((size)+3) & ~3; 

    #define STLOCAL(type, val)      *((type*&)plocalwalk) = (val)
    #define LDLOCAL(type)           *((type*&)plocalwalk)
    #define PTRLOCAL(type)          (type)plocalwalk

    #define LDCODE(object, type)    { memcpy((void*)object, (void*)pMLCode, sizeof(type)); \
                                        ((BYTE*&)pMLCode) += sizeof(type); }

    #define LDCODE8()               *(((BYTE*&)pMLCode)++)
    #define LDCODE16()              GET_UNALIGNED_16(((UINT16*&)pMLCode)++)    
    #define LDCODE32()              GET_UNALIGNED_32(((UINT32*&)pMLCode)++)
    #define LDCODEPTR()             (UINT_PTR)GET_UNALIGNED_PTR(((UINT_PTR*&)pMLCode)++)

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
    #define MARSHALOP(op, cbDst, cbSrc) (BYTE*&)pdst -= (cbDst); op; (BYTE*&)psrc+= (cbSrc);
#else
    #define MARSHALOP(op, cbDst, cbSrc) op; (BYTE*&)pdst += (cbDst); (BYTE*&)psrc += (cbSrc);
#endif


    enum
    {
        kSpecialError_InvalidRedim = 1,
    };
    
#ifdef _DEBUG
    LOG((LF_INTEROP, LL_INFO10000, "------------RunML------------\n"));
    VOID DisassembleMLStream(const MLCode *pMLCode);
    DisassembleMLStream(pMLCode);
#endif //_DEBUG

    // Perf Counter "%Time in marshalling" support
    // Implementation note: Pentium counter implementation 
    // is too expensive for marshalling. So, we implement
    // it as a counter.
    COUNTER_ONLY(GetPrivatePerfCounters().m_Interop.cMarshalling++);

    UINT8*          plocalwalk          = plocals;
    BOOL            fDeferredException  = FALSE;
    HRESULT         deferredExceptionHR = E_OUTOFMEMORY;
    UINT            specialErrorCode    = 0;
    Marshaler*      pMarshaler          = NULL;
    RetValBuffer*   ptempRetValBuffer;
    STRINGREF       tempStr;
    LPVOID          pv;
    BYTE            inout;
    UINT16          elemsize;
    UINT32          tempU32;
    MethodTable*    pMT;
    OBJECTREF       tempOR;
    MethodDesc*     tempMD;
    BYTE            BestFitMapping;
    BYTE            ThrowOnUnmappableChar;
    UINT32          paramSize;
    VOID*           pParam;
#ifdef _PPC_
    UINT            nFloatRegs          = 0;
#endif

    for(;;)
    {
#ifdef _DEBUG
        const MLCode *pMLCodeSave = pMLCode;
        UINT8 *poldlocalwalk = plocalwalk;
#endif
        switch (*(pMLCode++))
        {
            //
            // these ops will sign/zero extend to the stack element size
            //
            case ML_COPYI1: STDST( INT_PTR,  (INT8  )LDSRC(INT_PTR)  ); break;
            case ML_COPYU1: STDST( UINT_PTR, (UINT8 )LDSRC(UINT_PTR) ); break;
            case ML_COPYI2: STDST( INT_PTR,  (INT16 )LDSRC(INT_PTR)  ); break;
            case ML_COPYU2: STDST( UINT_PTR, (UINT16)LDSRC(UINT_PTR) ); break;
            case ML_COPYI4: STDST( INT_PTR,  (INT32 )LDSRC(INT_PTR)  ); break;
            case ML_COPYU4: STDST( UINT_PTR, (UINT32)LDSRC(UINT_PTR) ); break;

            //
            // these ops will just copy the specified number of bytes.  Note
            // that we can't use STDST here, because it will only bump pdst by
            // sizeof(dest type).
            //
            case ML_COPY1:  STDSTSIZE(UINT8,  (UINT8 )LDSRC(UINT8 ), sizeof(UINT_PTR)); break;
            case ML_COPY2:  STDSTSIZE(UINT16, (UINT16)LDSRC(UINT16), sizeof(UINT_PTR)); break;
            case ML_COPY4:  STDSTSIZE(UINT32, (UINT32)LDSRC(UINT32), sizeof(UINT_PTR)); break;

            case ML_COPY8:
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
                // the deregistered arguments are only 4-byte aligned
                STDST( UINT32, LDSRC(UINT32) );
                STDST( UINT32, LDSRC(UINT32) );
#else
                STDST( UINT64, LDSRC(UINT64) );
#endif
                break;

#if defined(TRACK_FLOATING_POINT_REGISTERS)

#if defined(_PPC_)
            case ML_COPYR4_N2C: // copy 4 byte float val
            {
                FLOAT value;
                value = LDSRC(FLOAT);
                if (nFloatRegs < NUM_FLOAT_ARGUMENT_REGISTERS)
                {
                    value = *pFloatRegs++;
                    nFloatRegs++;
                }
                STDST(FLOAT, value);
            }
            break;

            case ML_COPYR4_C2N:
            {
                FLOAT value;
                value = LDSRC(FLOAT);
                if (nFloatRegs < NUM_FLOAT_ARGUMENT_REGISTERS)
                {
                    *pFloatRegs++ = value;
                    nFloatRegs++;
                }
                STDST(FLOAT, value);
            }
            break;

            case ML_COPYR8_N2C: // copy 8 byte float val
            {
                DOUBLE value;
                value = LDSRC(DOUBLE);
                if (nFloatRegs < NUM_FLOAT_ARGUMENT_REGISTERS)
                {
                    value = *pFloatRegs++;
                    nFloatRegs++;
                }
                STDST(DOUBLE, value);
            }
            break;

            case ML_COPYR8_C2N:
            {
                DOUBLE value;
                value = LDSRC(DOUBLE);
                if (nFloatRegs < NUM_FLOAT_ARGUMENT_REGISTERS)
                {
                    *pFloatRegs++ = value;
                    nFloatRegs++;
                }
                STDST(DOUBLE, value);
            }
            break;

#else  // !_PPC_ && !_WIN64
            PORTABILITY_ASSERT("NYI - keeping track of floating point registers in RunML()");
#endif // !_PPC_ && !_WIN64

#endif // TRACK_FLOATING_POINT_REGISTERS

            case ML_END:  // intentional fallthru
            case ML_INTERRUPT:
                if (fDeferredException)
                {
                    switch (specialErrorCode)
                    {
                        case kSpecialError_InvalidRedim:
                            COMPlusThrow(kInvalidOperationException, IDS_INVALID_REDIM);

                        default:
                            // An ML opcode encountered an error but chose to defer
                            // the exception. Normally, this occurs during the backpropagation
                            // phase.
                            //
                            // While it'd be preferable to throw the original exception
                            // rather than an uninformative OutOfMemory exception,
                            // it would require more overhead than I like to propagate
                            // the exception backward in a GC-safe manner.
                            COMPlusThrowHR(deferredExceptionHR);
                    }
                }
                RETURN pMLCode;

    

            //-------------------------------------------------------------------------                
            case ML_BOOL_N2C:
#if BIGENDIAN
                STDSTSIZE(INT_PTR, LDSRC(BOOL) ? 1 : 0, sizeof(INT_PTR));
#else 
                STDSTSIZE(BYTE, LDSRC(BOOL) ? 1 : 0, sizeof(INT_PTR));
#endif // BIGENDIAN
                break;

            case ML_BOOL_C2N:
#if BIGENDIAN
                STDSTSIZE(BOOL, (LDSRC(INT_PTR) & 0xFF) ? 1 : 0, sizeof(INT_PTR));
#else 
                STDSTSIZE(BOOL, LDSRC(BYTE) ? 1 : 0, sizeof(INT_PTR));
#endif // BIGENDIAN
                break;

#if BIGENDIAN
            // Note: For big-endian architectures, we have to
            // push a pointer to the middle of the RetValBuffer.
            // That's why we have separate ML opcodes for each
            // possible buffer size. 
            case ML_PUSHRETVALBUFFER1:
                ptempRetValBuffer = PTRLOCAL(RetValBuffer*);
                ptempRetValBuffer->m_i32 = 0;
                STPTRDST(PVOID, (BYTE*)ptempRetValBuffer+3);
                INCLOCAL(sizeof(RetValBuffer));
                break;
            case ML_PUSHRETVALBUFFER2:
                ptempRetValBuffer = PTRLOCAL(RetValBuffer*);
                ptempRetValBuffer->m_i32 = 0;
                STPTRDST(PVOID, (BYTE*)ptempRetValBuffer+2);
                INCLOCAL(sizeof(RetValBuffer));
                break;
            case ML_PUSHRETVALBUFFER4:
                ptempRetValBuffer = PTRLOCAL(RetValBuffer*);
                ptempRetValBuffer->m_i32 = 0;
                STPTRDST(PVOID, ptempRetValBuffer);
                INCLOCAL(sizeof(RetValBuffer));
                break;
#else
            case ML_PUSHRETVALBUFFER1: //fallthru
            case ML_PUSHRETVALBUFFER2: //fallthru
            case ML_PUSHRETVALBUFFER4:
                ptempRetValBuffer = PTRLOCAL(RetValBuffer*);
                ptempRetValBuffer->m_i32 = 0;
                STPTRDST(PVOID, ptempRetValBuffer);
                INCLOCAL(sizeof(RetValBuffer));
                break;
#endif

            case ML_PUSHRETVALBUFFER8:
                ptempRetValBuffer = PTRLOCAL(RetValBuffer*);
                ptempRetValBuffer->m_i64 = 0;
                STPTRDST( RetValBuffer*, ptempRetValBuffer);
                INCLOCAL(sizeof(RetValBuffer));
                break;

            case ML_SETSRCTOLOCAL:
                psrc = (const VOID *)(plocals + LDCODE16());
                break;

            case ML_OBJECT_C2N:
                inout = LDCODE8();
                BestFitMapping = LDCODE8();
                ThrowOnUnmappableChar = LDCODE8();
                STPTRDST( LPVOID,
                       ((ML_OBJECT_C2N_SR*)plocalwalk)->DoConversion((OBJECTREF*)psrc, inout, (StringType)LDCODE8(), BestFitMapping, ThrowOnUnmappableChar, pCleanupWorkList) );
                INCSRC(OBJECTREF* &);
                INCLOCAL(sizeof(ML_OBJECT_C2N_SR));
                break;

            case ML_OBJECT_C2N_POST:
                ((ML_OBJECT_C2N_SR*)(plocals + LDCODE16()))->BackPropagate(&fDeferredException);
                break;

            case ML_LATEBOUNDMARKER:
                _ASSERTE(!"This ML stub should never be interpreted! This method should always be called using the CLRToCOMLateBoundWorker!");
                break;

            case ML_COMEVENTCALLMARKER:
                _ASSERTE(!"This ML stub should never be interpreted! This method should always be called using the CLRToCOMEventCallWorker!");
                break;

            case ML_BUMPSRC:
                ((BYTE*&)psrc) += (INT16)LDCODE16();
                break;

            case ML_BUMPDST:
                ((BYTE*&)pdst) += (INT16)LDCODE16();
                break;

            case ML_ARRAYWITHOFFSET_C2N:
                STPTRDST( LPVOID,
                          ((ML_ARRAYWITHOFFSET_C2N_SR*)plocalwalk)->DoConversion(
                                    &( PTRSRC(ArrayWithOffsetData)->m_Array ),
                                    PTRSRC(ArrayWithOffsetData)->m_cbOffset,
                                    PTRSRC(ArrayWithOffsetData)->m_cbCount,
                                    pCleanupWorkList) );
                INCSRC(ArrayWithOffsetData);
                INCLOCAL(sizeof(ML_ARRAYWITHOFFSET_C2N_SR));
                break;

            case ML_ARRAYWITHOFFSET_C2N_POST:
                ((ML_ARRAYWITHOFFSET_C2N_SR*)(plocals + LDCODE16()))->BackPropagate();
                break;

            // 
            // Marshaler opcodes
            //

            case ML_CREATE_MARSHALER_GENERIC_1:
                pMarshaler = new (plocalwalk) CopyMarshaler1(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshaler1);
                break;

            case ML_CREATE_MARSHALER_GENERIC_U1:
                pMarshaler = new (plocalwalk) CopyMarshalerU1(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshalerU1);
                break;

            case ML_CREATE_MARSHALER_GENERIC_2:
                pMarshaler = new (plocalwalk) CopyMarshaler2(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshaler2);
                break;

            case ML_CREATE_MARSHALER_GENERIC_U2:
                pMarshaler = new (plocalwalk) CopyMarshalerU2(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshalerU2);
                break;

            case ML_CREATE_MARSHALER_GENERIC_4:
                pMarshaler = new (plocalwalk) CopyMarshaler4(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshaler4);
                break;

            case ML_CREATE_MARSHALER_GENERIC_8:
                pMarshaler = new (plocalwalk) CopyMarshaler8(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CopyMarshaler8);
                break;

            case ML_CREATE_MARSHALER_WINBOOL:
                pMarshaler = new (plocalwalk) WinBoolMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(WinBoolMarshaler);
                break;

            case ML_CREATE_MARSHALER_CBOOL:
                pMarshaler = new (plocalwalk) CBoolMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CBoolMarshaler);
                break;

            case ML_CREATE_MARSHALER_ANSICHAR:
                pMarshaler = new (plocalwalk) AnsiCharMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((AnsiCharMarshaler*)pMarshaler)->SetBestFitMap(LDCODE8());
                ((AnsiCharMarshaler*)pMarshaler)->SetThrowOnUnmappableChar(LDCODE8());
                plocalwalk += sizeof(AnsiCharMarshaler);
                break;

            case ML_CREATE_MARSHALER_FLOAT:
                pMarshaler = new (plocalwalk) FloatMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(FloatMarshaler);
                break;

            case ML_CREATE_MARSHALER_DOUBLE:
                pMarshaler = new (plocalwalk) DoubleMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(DoubleMarshaler);
                break;

            case ML_CREATE_MARSHALER_CURRENCY:
                pMarshaler = new (plocalwalk) CurrencyMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(CurrencyMarshaler);
                break;

            case ML_CREATE_MARSHALER_DECIMAL:
                pMarshaler = new (plocalwalk) DecimalMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(DecimalMarshaler);
                break;

            case ML_CREATE_MARSHALER_DECIMAL_PTR:
                pMarshaler = new (plocalwalk) DecimalPtrMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(DecimalPtrMarshaler);
                break;

            case ML_CREATE_MARSHALER_GUID:
                pMarshaler = new (plocalwalk) GuidMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(GuidMarshaler);
                break;

            case ML_CREATE_MARSHALER_GUID_PTR:
                pMarshaler = new (plocalwalk) GuidPtrMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(GuidPtrMarshaler);
                break;

            case ML_CREATE_MARSHALER_DATE:
                pMarshaler = new (plocalwalk) DateMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(DateMarshaler);
                break;

            case ML_CREATE_MARSHALER_WSTR:
                pMarshaler = new (plocalwalk) WSTRMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(WSTRMarshaler);
                break;

            case ML_CREATE_MARSHALER_CSTR:
                pMarshaler = new (plocalwalk) CSTRMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((CSTRMarshaler*)pMarshaler)->SetBestFitMap(LDCODE8());
                ((CSTRMarshaler*)pMarshaler)->SetThrowOnUnmappableChar(LDCODE8());
                plocalwalk += sizeof(CSTRMarshaler);
                break;

            case ML_CREATE_MARSHALER_WSTR_BUFFER:
                pMarshaler = new (plocalwalk) WSTRBufferMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                plocalwalk += sizeof(WSTRBufferMarshaler);
                break;

            case ML_CREATE_MARSHALER_CSTR_BUFFER:
                pMarshaler = new (plocalwalk) CSTRBufferMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                ((CSTRBufferMarshaler*)pMarshaler)->SetBestFitMap(LDCODE8());
                ((CSTRBufferMarshaler*)pMarshaler)->SetThrowOnUnmappableChar(LDCODE8());
                plocalwalk += sizeof(CSTRBufferMarshaler);
                break;

            case ML_CREATE_MARSHALER_CARRAY:
                {
                    ML_CREATE_MARSHALER_CARRAY_OPERANDS mops;
                    LDCODE(&mops, ML_CREATE_MARSHALER_CARRAY_OPERANDS);
                    pMarshaler = new (plocalwalk) NativeArrayMarshaler(pCleanupWorkList);
                    PREFIX_ASSUME(pMarshaler != NULL);
                
                    ((NativeArrayMarshaler*)pMarshaler)->SetElementMethodTable(mops.methodTable);
                    ((NativeArrayMarshaler*)pMarshaler)->SetElementType(mops.elementType);
                    ((NativeArrayMarshaler*)pMarshaler)->SetBestFitMap(mops.bestfitmapping);
                    ((NativeArrayMarshaler*)pMarshaler)->SetThrowOnUnmappableChar(mops.throwonunmappablechar);
                    ((NativeArrayMarshaler*)pMarshaler)->SetMOPS(&mops);

                    plocalwalk += sizeof(NativeArrayMarshaler);
                break;
                }

            case ML_CREATE_MARSHALER_DELEGATE:
                pMarshaler = new (plocalwalk) DelegateMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((DelegateMarshaler*)pMarshaler)->SetMT((MethodTable*)LDCODEPTR());
                plocalwalk += sizeof(DelegateMarshaler);
                break;

            case ML_CREATE_MARSHALER_BLITTABLEPTR:
                pMarshaler = new (plocalwalk) BlittablePtrMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((BlittablePtrMarshaler*)pMarshaler)->SetMT((MethodTable*)LDCODEPTR());
                plocalwalk += sizeof(BlittablePtrMarshaler);
                break;

            case ML_CREATE_MARSHALER_LAYOUTCLASSPTR:
                pMarshaler = new (plocalwalk) LayoutClassPtrMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((LayoutClassPtrMarshaler*)pMarshaler)->SetMT((MethodTable*)LDCODEPTR());
                plocalwalk += sizeof(LayoutClassPtrMarshaler);
                break;

            case ML_CREATE_MARSHALER_ARRAYWITHOFFSET:
                _ASSERTE(!"The ML_CREATE_MARSHALER_ARRAYWITHOFFSET marshaler should never be created!");
                break;

            case ML_CREATE_MARSHALER_BLITTABLEVALUECLASS:
                _ASSERTE(!"The ML_CREATE_MARSHALER_BLITTABLEVALUECLASS marshaler should never be created!");
                break;

            case ML_CREATE_MARSHALER_VALUECLASS:
                _ASSERTE(!"The ML_CREATE_MARSHALER_VALUECLASS marshaler should never be created!");
                break;

            case ML_CREATE_MARSHALER_REFERENCECUSTOMMARSHALER:
                pMarshaler = new (plocalwalk) ReferenceCustomMarshaler(pCleanupWorkList);
                _ASSERTE(pMarshaler != NULL);
                ((ReferenceCustomMarshaler*)pMarshaler)->SetCMHelper((CustomMarshalerHelper *)LDCODEPTR());
                plocalwalk += sizeof(ReferenceCustomMarshaler);
                break;

            case ML_CREATE_MARSHALER_ARGITERATOR:
                _ASSERTE(!"The ML_CREATE_MARSHALER_ARGITERATOR marshaler should never be created!");
                break;

            case ML_CREATE_MARSHALER_BLITTABLEVALUECLASSWITHCOPYCTOR:
                _ASSERTE(!"The ML_CREATE_MARSHALER_BLITTABLEVALUECLASSWITHCOPYCTOR marshaler should never be created!");
                break;

            case ML_MARSHAL_N2C:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalNativeToCLR((void *)psrc, pdst),
                    pMarshaler->GetNativeToCLRDestStackSize(),
                    pMarshaler->GetNativeToCLRSrcStackSize());
                break;

            case ML_MARSHAL_N2C_OUT:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalNativeToCLROut((void *)psrc, pdst),
                    pMarshaler->m_cbCLR,
                    pMarshaler->m_cbNative);
                break;

            case ML_MARSHAL_N2C_BYREF:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalNativeToCLRByref((void *)psrc, pdst),
                    sizeof(void*),
                    sizeof(void*));
                break;

            case ML_MARSHAL_N2C_BYREF_OUT:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalNativeToCLRByrefOut((void *)psrc, pdst),
                    sizeof(void*),
                    sizeof(void*));
                break;

            case ML_UNMARSHAL_N2C_IN:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLRIn();
                break;

            case ML_UNMARSHAL_N2C_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLROut();
                break;

            case ML_UNMARSHAL_N2C_IN_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLRInOut();
                break;

            case ML_UNMARSHAL_N2C_BYREF_IN:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLRByrefIn();
                break;

            case ML_UNMARSHAL_N2C_BYREF_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLRByrefOut();
                break;

            case ML_UNMARSHAL_N2C_BYREF_IN_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalNativeToCLRByrefInOut();
                break;

            case ML_MARSHAL_C2N:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalCLRToNative((void *)psrc, pdst),
                    pMarshaler->GetCLRToNativeDestStackSize(),
                    pMarshaler->GetCLRToNativeSrcStackSize());
                break;

            case ML_MARSHAL_C2N_OUT:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalCLRToNativeOut((void *)psrc, pdst),
                    pMarshaler->m_cbNative,
                    pMarshaler->m_cbCLR);
                break;

            case ML_MARSHAL_C2N_BYREF:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalCLRToNativeByref((void *)psrc, pdst),
                    sizeof(void*),
                    sizeof(void*));
                break;

            case ML_MARSHAL_C2N_BYREF_OUT:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->MarshalCLRToNativeByrefOut((void *)psrc, pdst),
                    sizeof(void*),
                    sizeof(void*));
                break;

            case ML_UNMARSHAL_C2N_IN:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeIn();
                break;

            case ML_UNMARSHAL_C2N_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeOut();
                break;

            case ML_UNMARSHAL_C2N_IN_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeInOut();
                break;

            case ML_UNMARSHAL_C2N_BYREF_IN:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeByrefIn();
                break;

            case ML_UNMARSHAL_C2N_BYREF_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeByrefOut();
                break;

            case ML_UNMARSHAL_C2N_BYREF_IN_OUT:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());
                pMarshaler->UnmarshalCLRToNativeByrefInOut();
                break;

            case ML_PRERETURN_N2C:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->PrereturnNativeFromCLR((void *)psrc  AMD64_ARG((void *)pdstStart)  NOT_AMD64_ARG(pdst)),
                    pMarshaler->m_fReturnsCLRByref ? sizeof(void*) : 0,
                    0);
                break;

            case ML_RETURN_N2C:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());

                MARSHALOP(
                    pMarshaler->ReturnNativeFromCLR(pdst, (void *) psrc),
                    pMarshaler->m_fReturnsNativeByref ? sizeof(void*) : pMarshaler->m_cbNative,
                    pMarshaler->m_fReturnsCLRByref ? 0 : pMarshaler->m_cbCLR);
                break;

            case ML_PRERETURN_N2C_RETVAL:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->PrereturnNativeFromCLRRetval((void *)psrc  AMD64_ARG((void *)pdstStart)  NOT_AMD64_ARG(pdst)),
                    pMarshaler->m_fReturnsCLRByref ? sizeof(void*) : 0,
                    sizeof(void*));
                break;

            case ML_RETURN_N2C_RETVAL:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());

                MARSHALOP(
                    pMarshaler->ReturnNativeFromCLRRetval(pdst, (void *) psrc),
                    0,
                    pMarshaler->m_fReturnsCLRByref ? 0 : pMarshaler->m_cbCLR);
                break;

            case ML_PRERETURN_C2N:                
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->PrereturnCLRFromNative((void *)psrc, pdst),
                    0,
                    pMarshaler->m_fReturnsCLRByref ? sizeof(void*) : 0);
                break;

            case ML_RETURN_C2N:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());

                MARSHALOP(
                    pMarshaler->ReturnCLRFromNative(pdst, (void *) psrc),
                    pMarshaler->m_fReturnsCLRByref ? 0 : pMarshaler->m_cbCLR,
                    pMarshaler->m_fReturnsNativeByref ? sizeof(void*) : pMarshaler->m_cbNative);
                break;

            case ML_PRERETURN_C2N_RETVAL:
                PREFIX_ASSUME(pMarshaler != NULL);
                MARSHALOP(
                    pMarshaler->PrereturnCLRFromNativeRetval((void *)psrc, pdst),
                    sizeof(void*),
                    pMarshaler->m_fReturnsCLRByref ? sizeof(void*) : 0);
                break;

            case ML_RETURN_C2N_RETVAL:
                pMarshaler = (Marshaler*) (plocals + LDCODE16());

                MARSHALOP(
                    pMarshaler->ReturnCLRFromNativeRetval(pdst, (void *) psrc),
                    pMarshaler->m_fReturnsCLRByref ? 0 : pMarshaler->m_cbCLR,
                    0);
                break;

            case ML_SET_COM:
                pMarshaler->SetCLR((void *) psrc, pdst);
                break;

            case ML_PREGET_COM_RETVAL:
                pMarshaler->PregetCLRRetval((void *) psrc, pdst);
                break;

            case ML_PINNEDUNISTR_C2N:
                tempStr = LDSRC(STRINGREF);
                STPTRDST( const WCHAR *, (tempStr == NULL ? NULL : tempStr->GetBuffer()) );
                break;


            case ML_BLITTABLELAYOUTCLASS_C2N:
                tempOR = LDSRC(OBJECTREF);
                _ASSERTE(tempOR == NULL || (tempOR->GetMethodTable()->HasLayout() && tempOR->GetMethodTable()->IsBlittable()));
                STPTRDST( LPVOID, tempOR == NULL ? NULL : tempOR->GetData() );                       
                break;


            case ML_BLITTABLEVALUECLASS_C2N:
                tempU32 = LDCODE32();

                MARSHALOP(
                    memcpyNoGCRefs((BYTE*)pdst + ENDIANFIX(tempU32),
                                (BYTE*)psrc + ENDIANFIX(tempU32),
                                tempU32),
                    StackElemSize(tempU32),
                    StackElemSize(tempU32));
                break;

            case ML_BLITTABLEVALUECLASS_N2C:
                tempU32 = LDCODE32();

                MARSHALOP(
                    memcpyNoGCRefs((BYTE*)pdst + ENDIANFIX(tempU32),
                                (BYTE*)psrc + ENDIANFIX(tempU32),
                                tempU32),
                    StackElemSize(tempU32),
                    StackElemSize(tempU32));
                break;

            case ML_REFBLITTABLEVALUECLASS_C2N:
                tempU32 = LDCODE32();
                pv = LDSRC(LPVOID);

                MARSHALOP(
                    *((LPVOID*)pdst) = pv,
                    sizeof(LPVOID),
                    0);
                break;


            case ML_VALUECLASS_C2N:
                pMT = (MethodTable *)LDCODEPTR();
                
                _ASSERTE(pMT->GetClass()->IsValueClass());
                pv = pCleanupWorkList->NewScheduleLayoutDestroyNative(pMT);
                FmtValueTypeUpdateNative( (LPVOID)&psrc, pMT, (BYTE*)pv, pCleanupWorkList );

                MARSHALOP(
                    memcpyNoGCRefs(pdst, pv, pMT->GetNativeSize()),
                    StackElemSize(pMT->GetNativeSize()),
                    StackElemSize(pMT->GetAlignedNumInstanceFieldBytes()));
                break;

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            case ML_VALUECLASS_C2BYREFN:
                pMT = (MethodTable *)LDCODEPTR();
                _ASSERTE(pMT->GetClass()->IsValueClass());

                pv = pCleanupWorkList->NewScheduleLayoutDestroyNative(pMT);

                MARSHALOP(
                    FmtValueTypeUpdateNative( (LPVOID)&psrc, pMT, (BYTE*)pv, pCleanupWorkList ); *(LPVOID*)pdst = pv,
                    StackElemSize(sizeof(LPVOID)),
                    StackElemSize(pMT->GetAlignedNumInstanceFieldBytes()));
                break;

            case ML_VALUECLASS_BYREFC2N:
                pMT = (MethodTable *)LDCODEPTR();
                _ASSERTE(pMT->GetClass()->IsValueClass());

                pv = pCleanupWorkList->NewScheduleLayoutDestroyNative(pMT);

                MARSHALOP(
                    FmtValueTypeUpdateNative( (LPVOID)psrc, pMT, (BYTE*)pv, pCleanupWorkList ); memcpyNoGCRefs(pdst, pv, pMT->GetNativeSize()),
                    StackElemSize(pMT->GetNativeSize()),
                    StackElemSize(sizeof(LPVOID)));
                break;
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

            case ML_VALUECLASS_N2C:
                pMT = (MethodTable *)LDCODEPTR();
                _ASSERTE(pMT->GetClass()->IsValueClass());

                MARSHALOP(
                    FmtValueTypeUpdateCLR( &pdst, pMT, (BYTE*)psrc , FALSE),
                    StackElemSize(pMT->GetAlignedNumInstanceFieldBytes()),
                    StackElemSize(pMT->GetNativeSize()));
                break;

            case ML_REFVALUECLASS_C2N:
                inout = LDCODE8();
                pMT = (MethodTable *)LDCODEPTR();
                STPTRDST( LPVOID, ((ML_REFVALUECLASS_C2N_SR*)plocalwalk)->DoConversion((VOID**)psrc, pMT, inout, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFVALUECLASS_C2N_SR));

                ((BYTE*&)psrc) += StackElemSize( sizeof(LPVOID) );
                break;

            case ML_REFVALUECLASS_C2N_POST:
                ((ML_REFVALUECLASS_C2N_SR*)(plocals + LDCODE16()))->BackPropagate(&fDeferredException);
                break;

            case ML_REFVALUECLASS_N2C:
                inout = LDCODE8();
                pMT = (MethodTable *)LDCODEPTR();
                STPTRDST( LPVOID, ((ML_REFVALUECLASS_N2C_SR*)plocalwalk)->DoConversion(*(LPVOID*)psrc, inout, pMT, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFVALUECLASS_N2C_SR));

                ((BYTE*&)psrc) += StackElemSize( sizeof(LPVOID) );
                break;

            case ML_REFVALUECLASS_N2C_POST:
                ((ML_REFVALUECLASS_N2C_SR*)(plocals + LDCODE16()))->BackPropagate(&fDeferredException);
                break;

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            case ML_VALUECLASS_N2BYREFC:
                pMT = (MethodTable *)LDCODEPTR();
                STPTRDST( LPVOID, ((ML_REFVALUECLASS_N2C_SR*)plocalwalk)->DoConversion((VOID**)psrc, ML_IN, pMT, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFVALUECLASS_N2C_SR));

                ((BYTE*&)psrc) += StackElemSize(pMT->GetNativeSize());
                break;

            case ML_VALUECLASS_BYREFN2C:
                pMT = (MethodTable *)LDCODEPTR();
                _ASSERTE(pMT->GetClass()->IsValueClass());

                MARSHALOP(
                    FmtValueTypeUpdateCLR( &pdst, pMT, *(BYTE**)psrc , FALSE),
                    StackElemSize(pMT->GetAlignedNumInstanceFieldBytes()),
                    StackElemSize(sizeof(LPVOID)));
                break;
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE


            case ML_PINNEDISOMORPHICARRAY_C2N:
                elemsize = LDCODE16();
                tempOR = LDSRC(OBJECTREF);
                STPTRDST( LPVOID, tempOR == NULL ? NULL : (*(BASEARRAYREF*)&tempOR)->GetDataPtr() );
                break;

            case ML_PINNEDISOMORPHICARRAY_C2N_EXPRESS:
                elemsize = LDCODE16();
                _ASSERTE(elemsize != 0);
                tempOR = LDSRC(OBJECTREF);
                STPTRDST( BYTE*, tempOR == NULL ? NULL : (*(BYTE**)&tempOR) + elemsize );

                break;

            case ML_ARGITERATOR_C2N:
                STDST(va_list, COMVarArgs::MarshalToUnmanagedVaList(PTRSRC(VARARGS)));
                INCSRC(VARARGS);
                break;

            case ML_ARGITERATOR_N2C:
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                _ASSERTE(sizeof(VARARGS) <= ENREGISTERED_PARAMTYPE_MAXSIZE);
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
                MARSHALOP(
                    COMVarArgs::MarshalToManagedVaList(LDSRC(va_list), (VARARGS*)pdst),
                    StackElemSize( sizeof(VARARGS) ),
                    0);
                break;

            case ML_COPYCTOR_C2N:
                pMT     = (MethodTable *)LDCODEPTR();
                tempMD  = (MethodDesc *)LDCODEPTR();
                tempU32 = StackElemSize(pMT->GetNativeSize());
                paramSize = tempU32;
                pParam = pdst;
                
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                if (tempU32 > ENREGISTERED_PARAMTYPE_MAXSIZE)
                {
                    pParam = plocalwalk;
                    INCLOCAL(tempU32);

                    paramSize = sizeof(void*);
                }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                ((BYTE*&)pdst)   -= paramSize;
                ((BYTE*&)pParam) -= paramSize;
#endif

                pv = *((LPVOID*)psrc);
                if (tempMD)
                {
                    MethodDescCallSite method(tempMD);
                    ARG_SLOT args[2] = { PtrToArgSlot(pParam), PtrToArgSlot(pv) };
                    method.CallWithValueTypes(args);
                }
                else
                {
                    memcpyNoGCRefs(pParam, pv, tempU32);
                }

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                ((BYTE*&)pdst) += paramSize;
#endif

                pv = LDSRC(LPVOID); //reload again from gc-protected memory - just to be paranoid
                tempMD = (MethodDesc *)LDCODEPTR();
                if (tempMD)
                {
                    MethodDescCallSite method(tempMD);
                    ARG_SLOT arg = PtrToArgSlot(pv);
                    method.CallWithValueTypes(&arg);
                }
                
                break;


            case ML_COPYCTOR_N2C:
                pMT = (MethodTable *)LDCODEPTR();
                tempU32 = StackElemSize(pMT->GetNativeSize());
                pv = (PVOID)psrc;
                STPTRDST(const VOID *, pv);
                ((BYTE*&)psrc) += tempU32;
                break;

            case ML_CAPTURE_PSRC:
                *((BYTE**)plocalwalk) = ((BYTE*)psrc) + (INT16)LDCODE16();
                INCLOCAL(sizeof(BYTE**));
                break;

            case ML_COPY_SMBLITTABLEVALUETYPE:
                tempU32 = LDCODE32();
                MARSHALOP(
                    memcpyNoGCRefs(pdst, psrc, tempU32),
                    StackElemSize(tempU32), 
                    StackElemSize(tempU32));
                break;

            case ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_C2N:
                // Note: we only handle blittables here.
                tempU32 = LDCODE32(); // get the size
                memcpyNoGCRefs( **((BYTE***)(plocals + LDCODE16())) + ENDIANFIX(tempU32),
                                (BYTE *)psrc + ENDIANFIX(tempU32), tempU32 );
                break;

            case ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_N2C:
                // Note: we only handle blittables here.
                tempU32 = LDCODE32(); // get the size

                MARSHALOP(
                    memcpyNoGCRefs( (BYTE*)pdst + ENDIANFIX(tempU32), plocals + LDCODE16() + ENDIANFIX(tempU32), tempU32 ),
                    StackElemSize(tempU32),
                    0);
                break;

            case ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N:
                tempU32 = LDCODE32(); // get the size;
                STPTRDST( LPVOID, ((ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR*)plocalwalk)->DoConversion(psrc, tempU32));
                INCLOCAL(sizeof(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR));
                ((BYTE*&)psrc) += StackElemSize( sizeof(LPVOID) );
                break;

            case ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_POST:
                ((ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR*)(plocals + LDCODE16()))->BackPropagate(&fDeferredException);
                break;

            case ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C:
                pv = LDSRC(LPVOID);
                STPTRDST(LPVOID, pv);
                *((LPVOID*)plocalwalk) = pv;
                INCLOCAL(sizeof(LPVOID));
                break;

            case ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C_POST:
                pv = *((LPVOID*)(plocals + LDCODE16() ));
                STPTRDST(LPVOID, pv);
                break;

            case ML_PUSHVASIGCOOKIEEX:
#ifdef _DEBUG
                FillMemory(plocalwalk, sizeof(VASigCookieEx), 0xcc);
#endif
                ((VASigCookieEx*)plocalwalk)->sizeOfArgs = LDCODE16();
                ((VASigCookieEx*)plocalwalk)->mdVASig = NULL;
                ((VASigCookieEx*)plocalwalk)->m_pArgs = (const BYTE *)psrc;
                STPTRDST(LPVOID, plocalwalk);
                INCLOCAL(sizeof(VASigCookieEx));
                break;
                
            case ML_CSTR_C2N:
                BestFitMapping = LDCODE8();
                ThrowOnUnmappableChar = LDCODE8();
                STPTRDST( LPSTR, ((ML_CSTR_C2N_SR*)plocalwalk)->DoConversion(LDSRC(STRINGREF), (UINT32)BestFitMapping, (UINT32)ThrowOnUnmappableChar, pCleanupWorkList) );
                INCLOCAL(sizeof(ML_CSTR_C2N_SR));
                break;

            case ML_HANDLEREF_C2N:
                STPTRDST( LPVOID, LDSRC(HANDLEREF).m_handle );
                break;

            case ML_SAFEHANDLE_C2N:
                STPTRDST(LPVOID, SafeHandleC2NHelper((SAFEHANDLE *)psrc, pCleanupWorkList));
                INCSRC(SAFEHANDLE);
                break;

            case ML_REFSAFEHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                inout = LDCODE8();
                STPTRDST(LPVOID, ((ML_REFSAFEHANDLE_C2N_SR*)plocalwalk)->DoConversion(*(SAFEHANDLE **)psrc, pMT, inout, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFSAFEHANDLE_C2N_SR));
                INCSRC(LPVOID);
                break;

            case ML_REFSAFEHANDLE_C2N_POST:
                ((ML_REFSAFEHANDLE_C2N_SR*)(plocals + LDCODE16()))->BackPropagate();
                break;

            case ML_RETSAFEHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                ((ML_RETSAFEHANDLE_C2N_SR*)plocalwalk)->PreReturn(pMT, pCleanupWorkList);
                INCLOCAL(sizeof(ML_RETSAFEHANDLE_C2N_SR));
                break;

            case ML_RETSAFEHANDLE_C2N_POST:
                STDST(SAFEHANDLE, ((ML_RETSAFEHANDLE_C2N_SR*)(plocals + LDCODE16()))->DoReturn((LPVOID*)psrc));
                break;

            case ML_RETVALSAFEHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                STPTRDST(LPVOID, ((ML_REFSAFEHANDLE_C2N_SR*)plocalwalk)->DoConversion(NULL, pMT, ML_OUT, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFSAFEHANDLE_C2N_SR));
                break;

            case ML_RETVALSAFEHANDLE_C2N_POST:
                STDST(SAFEHANDLE, ((ML_REFSAFEHANDLE_C2N_SR*)(plocals + LDCODE16()))->DoReturn());
                break;

            case ML_CRITICALHANDLE_C2N:
                STPTRDST(LPVOID, LDSRC(CRITICALHANDLE)->GetHandle());
                break;

            case ML_REFCRITICALHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                inout = LDCODE8();
                STPTRDST(LPVOID, ((ML_REFCRITICALHANDLE_C2N_SR*)plocalwalk)->DoConversion(*(CRITICALHANDLE **)psrc, pMT, inout, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFCRITICALHANDLE_C2N_SR));
                INCSRC(LPVOID);
                break;

            case ML_REFCRITICALHANDLE_C2N_POST:
                ((ML_REFCRITICALHANDLE_C2N_SR*)(plocals + LDCODE16()))->BackPropagate();
                break;

            case ML_RETCRITICALHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                ((ML_RETCRITICALHANDLE_C2N_SR*)plocalwalk)->PreReturn(pMT, pCleanupWorkList);
                INCLOCAL(sizeof(ML_RETCRITICALHANDLE_C2N_SR));
                break;

            case ML_RETCRITICALHANDLE_C2N_POST:
                STDST(CRITICALHANDLE, ((ML_RETCRITICALHANDLE_C2N_SR*)(plocals + LDCODE16()))->DoReturn((LPVOID*)psrc));
                break;

            case ML_RETVALCRITICALHANDLE_C2N:
                pMT = (MethodTable*)LDCODEPTR();
                STPTRDST(LPVOID, ((ML_REFCRITICALHANDLE_C2N_SR*)plocalwalk)->DoConversion(NULL, pMT, ML_OUT, pCleanupWorkList));
                INCLOCAL(sizeof(ML_REFCRITICALHANDLE_C2N_SR));
                break;

            case ML_RETVALCRITICALHANDLE_C2N_POST:
                STDST(CRITICALHANDLE, ((ML_REFCRITICALHANDLE_C2N_SR*)(plocals + LDCODE16()))->DoReturn());
                break;

            case ML_WSTRBUILDER_C2N:
                STPTRDST( LPWSTR, ((ML_WSTRBUILDER_C2N_SR*)plocalwalk)->DoConversion((STRINGBUFFERREF*)psrc, pCleanupWorkList) );
                INCSRC(STRINGBUFFERREF);
                INCLOCAL(sizeof(ML_WSTRBUILDER_C2N_SR));
                break;

            case ML_WSTRBUILDER_C2N_POST:
                ((ML_WSTRBUILDER_C2N_SR*)(plocals + LDCODE16()))->BackPropagate(&fDeferredException);
                break;

            case ML_CSTRBUILDER_C2N:
                BestFitMapping = LDCODE8();
                ThrowOnUnmappableChar = LDCODE8();
                STPTRDST( LPSTR, ((ML_CSTRBUILDER_C2N_SR*)plocalwalk)->DoConversion((STRINGBUFFERREF*)psrc, BestFitMapping, ThrowOnUnmappableChar, pCleanupWorkList) );
                INCSRC(STRINGBUFFERREF);
                INCLOCAL(sizeof(ML_CSTRBUILDER_C2N_SR));
                break;

            case ML_CSTRBUILDER_C2N_POST:
                ((ML_CSTRBUILDER_C2N_SR*)(plocals + LDCODE16()))->BackPropagate();
                break;

            case ML_CBOOL_C2N:
                STDSTSIZE(BYTE, LDSRC(BYTE) ? 1 : 0, sizeof(INT_PTR));
                break;

            case ML_CBOOL_N2C:
                STDSTSIZE(BYTE, LDSRC(BYTE) ? 1 : 0, sizeof(INT_PTR));
                break;

            case ML_LCID_C2N:
                STDST(LCID, GetThreadCultureIdNoThrow(GetThread(), FALSE));
                break;

            case ML_LCID_N2C:
                {
                    GCX_COOP();

                    OBJECTREF OldCulture = GetThread()->GetCulture(FALSE);
                    GCPROTECT_BEGIN(OldCulture)
                    {
                        ReturnCultureHolder rch(GetThread(), &OldCulture, FALSE);
                        GetThread()->SetCultureId(LDSRC(LCID), FALSE);
                        pCleanupWorkList->ScheduleUnconditionalCultureRestore(&OldCulture);
                        rch.SuppressRelease();
                    }
                    GCPROTECT_END();
                }
                break;

            case ML_STRUCTRETN2C:
                pMT = (MethodTable *)LDCODEPTR();
                ((ML_STRUCTRETN2C_SR*)plocalwalk)->m_pNativeRetBuf = LDSRC(LPVOID);

                if (! (((ML_STRUCTRETN2C_SR*)plocalwalk)->m_pNativeRetBuf) )
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

                ((ML_STRUCTRETN2C_SR*)plocalwalk)->m_pMT = pMT;

                if (IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                {
                    ((ML_STRUCTRETN2C_SR*)plocalwalk)->m_ppProtectedBoxedObj = pCleanupWorkList->NewProtectedObjectRef(pMT->Allocate());
                    NOT_AMD64(STDST(LPVOID, (*((ML_STRUCTRETN2C_SR*)plocalwalk)->m_ppProtectedBoxedObj)->GetData()));
                    // Again, weird calling convention on AMD64.  Please refer to the comment above about STDSTSTART.
                    AMD64_ONLY(STDSTSTART(LPVOID, (*((ML_STRUCTRETN2C_SR*)plocalwalk)->m_ppProtectedBoxedObj)->GetData()));
                }
#ifdef _DEBUG                
                else
                {
                    ((ML_STRUCTRETN2C_SR*)plocalwalk)->m_ppProtectedBoxedObj = (OBJECTREF*)(UINT_PTR)0xbaadf000;
                }
#endif // _DEBUG                
                
                INCLOCAL(sizeof(ML_STRUCTRETN2C_SR));
                break;

            case ML_STRUCTRETN2C_POST:
                {
                    BYTE* pbDummy;
                    
                    ML_STRUCTRETN2C_SR* pStructMarshaler = (ML_STRUCTRETN2C_SR*)(plocals + LDCODE16());
                    pMT = pStructMarshaler->m_pMT;
                    if (!IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                    {
                        // We're unmarshaling from a copy of the small value class, so we need to
                        // fake out the marshaler a bit...
                        pbDummy = ((BYTE*)psrc) - Object::GetOffsetOfFirstField();
                        pStructMarshaler->m_ppProtectedBoxedObj = (OBJECTREF*)&pbDummy;
                    }
                    pStructMarshaler->MarshalRetVal(&fDeferredException);
#ifdef _DEBUG
                    if (!IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                    {
                        pStructMarshaler->m_ppProtectedBoxedObj = (OBJECTREF*)(UINT_PTR)0xbaadf000;
                    }
#endif // _DEBUG
                }
                break;

            case ML_STRUCTRETC2N:
                pMT = (MethodTable *)LDCODEPTR();
                if (IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                {
                    ((ML_STRUCTRETC2N_SR*)plocalwalk)->m_ppProtectedValueTypeBuf = (LPVOID*)psrc;
                    INCSRC(LPVOID);
                }
#ifdef _DEBUG                
                else
                {
                    ((ML_STRUCTRETC2N_SR*)plocalwalk)->m_ppProtectedValueTypeBuf = (LPVOID*)(UINT_PTR)0xbaadf000;
                }
#endif // _DEBUG

                ((ML_STRUCTRETC2N_SR*)plocalwalk)->m_pMT = pMT;
                ((ML_STRUCTRETC2N_SR*)plocalwalk)->m_pNativeRetBuf = (LPVOID)(GetThread()->m_MarshalAlloc.Alloc(pMT->GetNativeSize()));
                STDST(LPVOID, ((ML_STRUCTRETC2N_SR*)plocalwalk)->m_pNativeRetBuf);
                INCLOCAL(sizeof(ML_STRUCTRETC2N_SR));
                break;

            case ML_STRUCTRETC2N_POST:
                {
                    BYTE* pbDummy;
                    ML_STRUCTRETC2N_SR* pStructMarshaler = (ML_STRUCTRETC2N_SR*)(plocals + LDCODE16());
                    pMT = pStructMarshaler->m_pMT;
                    if (!IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                    {
                        // We're unmarshaling into a buffer for the small value class, so we need to
                        // fake out the marshaler a bit...
                        pbDummy = ((BYTE*)pdst);
                        pStructMarshaler->m_ppProtectedValueTypeBuf = (LPVOID*)&pbDummy;
                    }
                    pStructMarshaler->MarshalRetVal(&fDeferredException);
#ifdef _DEBUG
                    if (!IsManagedValueTypeReturnedByRef(pMT->GetNumInstanceFieldBytes()))
                    {
                        pStructMarshaler->m_ppProtectedValueTypeBuf = (LPVOID*)(UINT_PTR)0xbaadf000;
                    }
#endif // _DEBUG
                }
                break;

            case ML_CURRENCYRETC2N:
                ((ML_CURRENCYRETC2N_SR*)plocalwalk)->m_ppProtectedValueTypeBuf = (DECIMAL**)psrc;
                INCSRC(DECIMAL*);
                STDST(CURRENCY*, &( ((ML_CURRENCYRETC2N_SR*)plocalwalk)->m_cy ));
                INCLOCAL(sizeof(ML_CURRENCYRETC2N_SR));
                break;

            case ML_CURRENCYRETC2N_POST:
                ((ML_CURRENCYRETC2N_SR*)(plocals + LDCODE16()))->MarshalRetVal(&fDeferredException);
                break;

            case ML_COPYPINNEDGCREF:
                STDST( LPVOID, LDSRC(LPVOID) );
                break;

            case ML_CURRENCYRETN2C:
                ((ML_CURRENCYRETN2C_SR*)plocalwalk)->m_pcy = LDSRC(CURRENCY*);
                if (!(((ML_CURRENCYRETN2C_SR*)plocalwalk)->m_pcy))
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

                NOT_WIN64(STDST(DECIMAL*, &( ((ML_CURRENCYRETN2C_SR*)plocalwalk)->m_decimal )));
                // Again, weird calling convention on AMD64.  Please refer to the comment above about STDSTSTART.
                AMD64_ONLY(STDSTSTART(DECIMAL*, &( ((ML_CURRENCYRETN2C_SR*)plocalwalk)->m_decimal )));
                INCLOCAL(sizeof(ML_CURRENCYRETN2C_SR));
                break;

            case ML_CURRENCYRETN2C_POST:
                {
                    ML_CURRENCYRETN2C_SR* pCurrencyMarshaler = (ML_CURRENCYRETN2C_SR*)(plocals + LDCODE16());
                    // On IA64 DECIMAL variables are returned by value, not by ref.  So we need to store the
                    // return value back to our buffer before marshaling the return value.
                    IA64_ONLY( pCurrencyMarshaler->m_decimal = *(DECIMAL*)psrc );
                    pCurrencyMarshaler->MarshalRetVal(&fDeferredException);
                }
                break;

            case ML_DATETIMERETN2C:
                ((ML_DATETIMERETN2C_SR*)plocalwalk)->m_pdate = LDSRC(DATE*);
                if (!(((ML_DATETIMERETN2C_SR*)plocalwalk)->m_pdate))
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

                // On WIN64 platform INT64 variables are returned by value, so we don't need to save the 
                // reference to the return buffer here.
                NOT_WIN64(STDST(INT64*, &( ((ML_DATETIMERETN2C_SR*)plocalwalk)->m_datetime )));
                INCLOCAL(sizeof(ML_DATETIMERETN2C_SR));
                break;

            case ML_DATETIMERETN2C_POST:
                {
                    ML_DATETIMERETN2C_SR* pDateTimeMarshaler = (ML_DATETIMERETN2C_SR*)(plocals + LDCODE16());
                    // On WIN64 platform INT64 variables are returned by value.
                    WIN64_ONLY( pDateTimeMarshaler->m_datetime = *(INT64*)psrc );
                    pDateTimeMarshaler->MarshalRetVal(&fDeferredException);
                }
                break;

            case ML_CREATE_MARSHALER_INTERFACE:
                pMarshaler = new (plocalwalk) InterfaceMarshaler(pCleanupWorkList);
                PREFIX_ASSUME(pMarshaler != NULL);
                ((InterfaceMarshaler*)pMarshaler)->SetClassMT((MethodTable *)LDCODEPTR());
                ((InterfaceMarshaler*)pMarshaler)->SetItfMT((MethodTable *)LDCODEPTR());
                ((InterfaceMarshaler*)pMarshaler)->SetIsDispatch(LDCODE8());
                ((InterfaceMarshaler*)pMarshaler)->SetClassIsHint(LDCODE8());
                ((InterfaceMarshaler*)pMarshaler)->SetUseBasicItf(LDCODE8());
                plocalwalk += sizeof(InterfaceMarshaler);
                break;

            case ML_THROWIFHRFAILED:
                {
                    HRESULT   tempHR = LDSRC(HRESULT);
                    if (FAILED(tempHR))
                        COMPlusThrowHR(tempHR, kGetErrorInfo);
                break;
                }



            default:
#ifndef _DEBUG
                __assume(0);
#endif
                _ASSERTE(!"RunML: Unrecognized ML opcode");
                break;
        }
        _ASSERTE(plocalwalk - poldlocalwalk == gMLInfo[*pMLCodeSave].m_cbLocal);
        _ASSERTE(pMLCode - pMLCodeSave == gMLInfo[*pMLCodeSave].m_numOperandBytes + 1);
    }
#undef LDSRC
#undef STDST
#undef INCLOCAL

}






//===========================================================================
// Do conversion for N/Direct parameters of type "Object"
// "Object" is a catch-all type that supports a number of conversions based
// on the runtime type.
//===========================================================================
LPVOID ML_OBJECT_C2N_SR::DoConversion(OBJECTREF       *ppProtectedObjectRef,  
                                      BYTE             inout,
                                      BYTE             fIsAnsi,
                                      BYTE             fBestFitMapping,
                                      BYTE             fThrowOnUnmappableChar,                                      
                                      CleanupWorkList *pCleanup)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(ppProtectedObjectRef != NULL);
        PRECONDITION(inout == ML_IN || inout == ML_OUT || inout == (ML_IN|ML_OUT));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    m_backproptype = BP_NONE;
    m_inout        = inout;
    m_fIsAnsi      = fIsAnsi;

    if (*ppProtectedObjectRef == NULL) 
        RETURN NULL;

    MethodTable *pMT = (*ppProtectedObjectRef)->GetMethodTable();

    if (g_Mscorlib.IsClass(pMT, CLASS__ARRAY_WITH_OFFSET))
    {
        COMPlusThrow(kArgumentException, IDS_EE_BADMARSHAL_ASANYRESTRICTION);
    }
    
    if (pMT->IsArray())
    {
        CorElementType etyp = pMT->GetArrayElementType();
        VARTYPE vt = VT_EMPTY;

        switch (etyp)
        {
            case ELEMENT_TYPE_I1:      vt = VT_I1; break;
            case ELEMENT_TYPE_U1:      vt = VT_UI1; break;
            case ELEMENT_TYPE_I2:      vt = VT_I2; break;
            case ELEMENT_TYPE_U2:      vt = VT_UI2; break;
            IN_WIN32(case ELEMENT_TYPE_I:)
            case ELEMENT_TYPE_I4:      vt = VT_I4; break;
            IN_WIN32(case ELEMENT_TYPE_U:)
            case ELEMENT_TYPE_U4:      vt = VT_UI4; break;
            IN_WIN64(case ELEMENT_TYPE_I:)
            case ELEMENT_TYPE_I8:      vt = VT_I8; break;
            IN_WIN64(case ELEMENT_TYPE_U:)
            case ELEMENT_TYPE_U8:      vt = VT_UI8; break;
            case ELEMENT_TYPE_R4:      vt = VT_R4; break;
            case ELEMENT_TYPE_R8:      vt = VT_R8; break;
            case ELEMENT_TYPE_CHAR:    vt = m_fIsAnsi ? VTHACK_ANSICHAR : VT_UI2 ; break;
            case ELEMENT_TYPE_BOOLEAN: vt = VTHACK_WINBOOL; break;
            default:
                COMPlusThrow(kArgumentException, IDS_EE_NDIRECT_BADOBJECT);
        }

        m_backproptype = BP_UNMARSHAL;
        Marshaler *pMarshaler;
        pMarshaler = new (&m_nativearraymarshaler) NativeArrayMarshaler(pCleanup);
        
        ((NativeArrayMarshaler*)pMarshaler)->SetElementMethodTable(pMT);
        ((NativeArrayMarshaler*)pMarshaler)->SetElementType(vt);
        ((NativeArrayMarshaler*)pMarshaler)->SetElementCount( (*((BASEARRAYREF*)ppProtectedObjectRef))->GetNumComponents() );
        ((NativeArrayMarshaler*)pMarshaler)->SetBestFitMap(fBestFitMapping);
        ((NativeArrayMarshaler*)pMarshaler)->SetThrowOnUnmappableChar(fThrowOnUnmappableChar);

        ML_CREATE_MARSHALER_CARRAY_OPERANDS mops;
        mops.methodTable = pMT;
        mops.elementType = vt;
        mops.countParamIdx = 0;
        mops.countSize   = 0;
        mops.multiplier  = 0;
        mops.additive    = 1;
        mops.bestfitmapping = fBestFitMapping;
        mops.throwonunmappablechar = fThrowOnUnmappableChar;

        ((NativeArrayMarshaler*)pMarshaler)->SetMOPS(&mops);
    }
    else
    {
        if (pMT == g_pStringClass)
        {
            m_inout = ML_IN;
            m_backproptype = BP_UNMARSHAL;

            if (m_fIsAnsi)
            {
                new (&m_cstrmarshaler) CSTRMarshaler(pCleanup);
                ((CSTRMarshaler*)m_cstrmarshaler)->SetBestFitMap(fBestFitMapping);
                ((CSTRMarshaler*)m_cstrmarshaler)->SetThrowOnUnmappableChar(fThrowOnUnmappableChar);
            }
            else
                new (&m_wstrmarshaler) WSTRMarshaler(pCleanup);
            }
        else
        {
            if (g_Mscorlib.IsClass(pMT, CLASS__STRING_BUILDER))
            {
                if (m_fIsAnsi)
                {
                    new (&m_cstrbuffermarshaler) CSTRBufferMarshaler(pCleanup);
                    ((CSTRBufferMarshaler*)m_cstrbuffermarshaler)->SetBestFitMap(fBestFitMapping);
                    ((CSTRBufferMarshaler*)m_cstrbuffermarshaler)->SetThrowOnUnmappableChar(fThrowOnUnmappableChar);
                }
                else
                    new (&m_wstrbuffermarshaler) WSTRBufferMarshaler(pCleanup);

                m_backproptype = BP_UNMARSHAL;

            }
            else if (pMT->GetClass()->HasLayout())
            {
                m_backproptype = BP_UNMARSHAL;
                LayoutClassPtrMarshaler *pMarshaler;
                pMarshaler = new (&m_layoutclassptrmarshaler) LayoutClassPtrMarshaler(pCleanup);
                _ASSERTE(pMarshaler != NULL);
                pMarshaler->SetMT(pMT);
            }
        }
    }

    if (m_backproptype != BP_UNMARSHAL)
    {
        // If we got here, we were passed an unsupported type
        COMPlusThrow(kArgumentException, IDS_EE_NDIRECT_BADOBJECT);
    }

    LPVOID nativevalue = NULL;
    switch (m_inout)
    {
        case ML_IN: //fallthru
        case ML_IN|ML_OUT:
            ((Marshaler*)&m_marshaler)->MarshalCLRToNative(ppProtectedObjectRef, &nativevalue);
            break;
            
        case ML_OUT:
            ((Marshaler*)&m_marshaler)->MarshalCLRToNativeOut(ppProtectedObjectRef, &nativevalue);
            break;
            
        default:
            _ASSERTE(0);
    }
    RETURN nativevalue;
}



VOID ML_OBJECT_C2N_SR::BackPropagate(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    switch (m_backproptype)
    {
        case BP_NONE:
            //nothing
            break;

        case BP_UNMARSHAL:
            switch (m_inout)
            {
                case ML_IN:
                    ((Marshaler*)&m_marshaler)->UnmarshalCLRToNativeIn();
                    break;

                case ML_OUT:
                    ((Marshaler*)&m_marshaler)->UnmarshalCLRToNativeOut();
                    break;

                case ML_IN|ML_OUT:
                    ((Marshaler*)&m_marshaler)->UnmarshalCLRToNativeInOut();
                    break;

                default:
                    _ASSERTE(0);

            }
            break;

        default:
            _ASSERTE(0);
    }
}

//----------------------------------------------------------------------
// Convert ArrayWithOffset to native array
//----------------------------------------------------------------------
LPVOID ML_ARRAYWITHOFFSET_C2N_SR::DoConversion(BASEARRAYREF    *ppProtectedArrayRef, //pointer to GC-protected BASERARRAYREF,
                                               INT32            cbOffset,
                                               INT32            cbCount,
                                               CleanupWorkList *pCleanup)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(ppProtectedArrayRef != NULL);
        PRECONDITION(cbOffset >= 0);
        PRECONDITION(cbCount > 0);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    m_ppProtectedArrayRef = ppProtectedArrayRef;
    if (!*ppProtectedArrayRef)
        RETURN NULL;
    else
    {
        m_cbOffset = cbOffset;
        m_cbCount  = cbCount;
        if (cbCount > kStackBufferSize)
            m_pNativeArray = GetThread()->m_MarshalAlloc.Alloc(cbCount);
        else
            m_pNativeArray = m_StackBuffer;

        memcpyNoGCRefs(m_pNativeArray, cbOffset + (LPBYTE) ((*ppProtectedArrayRef)->GetDataPtr()), cbCount);
        RETURN m_pNativeArray;
    }
}





//----------------------------------------------------------------------
// Backpropagates changes to the native array back to the CLR array.
//----------------------------------------------------------------------
VOID   ML_ARRAYWITHOFFSET_C2N_SR::BackPropagate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    if (*m_ppProtectedArrayRef != NULL)
    {
       memcpyNoGCRefs(m_cbOffset + (LPBYTE) ((*m_ppProtectedArrayRef)->GetDataPtr()), m_pNativeArray, m_cbCount);
    }
}

LPVOID ML_REFVALUECLASS_C2N_SR::DoConversion(LPVOID          *ppProtectedData,
                                             MethodTable     *pMT,
                                             BYTE             fInOut,
                                             CleanupWorkList *pCleanup)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppProtectedData));
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(fInOut == ML_IN || fInOut == ML_OUT || fInOut == (ML_IN|ML_OUT));
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    m_ppProtectedData = ppProtectedData;
    m_pMT             = pMT;
    m_inout           = fInOut;

    m_buf = (BYTE*)(pCleanup->NewScheduleLayoutDestroyNative(pMT));
    
    if (m_inout & ML_IN)
        FmtValueTypeUpdateNative(ppProtectedData, pMT, m_buf, pCleanup);
    else
        FillMemory(m_buf, pMT->GetNativeSize(), 0);

    RETURN m_buf;
}

VOID ML_REFVALUECLASS_C2N_SR::BackPropagate(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    if (m_inout & ML_OUT)
        FmtValueTypeUpdateCLR(m_ppProtectedData, m_pMT, m_buf, FALSE);
}

LPVOID __stdcall ML_REFSAFEHANDLE_C2N_SR::DoConversionStatic(ML_REFSAFEHANDLE_C2N_SR *pThis, SAFEHANDLE *pSafeHandleObj, MethodTable *pMT, BYTE fInOut, CleanupWorkList *pCleanup)
{
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pSafeHandleObj, NULL_OK));
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    // Store the source pointer to be able to back propagate later.
    pThis->m_psrc = pSafeHandleObj;

    // Make sure that output only parameters have an initial null handle (we
    // need this for correct back propagation logic).
    if (fInOut == ML_OUT && pSafeHandleObj)
        *pSafeHandleObj = NULL;

    if (fInOut & ML_IN)
    {
        if ((*pSafeHandleObj) == NULL)
            COMPlusThrow(kArgumentNullException, L"ArgumentNull_SafeHandle");

        // The SafeHandleHolder will AddRef the SafeHandle.
        SafeHandleHolder shh(pSafeHandleObj);
        pThis->m_handle = (*pSafeHandleObj)->GetHandle();
        pCleanup->ScheduleSafeHandleRelease(pSafeHandleObj);

        // The release was scheduled successfully so we can suppress
        // releasing the safe handle here.
        shh.SuppressRelease();
    }

    if (fInOut & ML_OUT)
    {
        SAFEHANDLE SafeHandleObj = NULL;
        GCPROTECT_BEGIN(SafeHandleObj)
        {
            // Pre-allocate a SafeHandle object to make sure we don't run out of memory
            // after the call out to unmanaged has completed. If this were to happen, we 
            // would leak the handle which is bad.
            SafeHandleObj = (SAFEHANDLE)AllocateObject(pMT);
            CallDefaultConstructor(SafeHandleObj);

            // If the handle isn't valid on input initialize it to the value set
            // by the safe handle ctor above. We want to make sure the handle is
            // either invalid or set to a valid value by the unmanaged function
            // when we back propagate.
            if (!(fInOut & ML_IN))
                pThis->m_handle = SafeHandleObj->GetHandle();

            pThis->m_pProtectedSafeHandleObj = (SAFEHANDLE*)pCleanup->NewProtectedObjectRef((OBJECTREF)SafeHandleObj);

            // Schedule an unconditional back propagation of the handle in to the SafeHandle.
            pCleanup->ScheduleRefSafeHandleBackPropagate(&SafeHandleObj, &pThis->m_handle, pThis->m_handle);
        }
        GCPROTECT_END();
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    RETURN &pThis->m_handle;
}

void __stdcall ML_REFSAFEHANDLE_C2N_SR::BackPropagateStatic(ML_REFSAFEHANDLE_C2N_SR *pThis)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    //BEGIN_ENTRYPOINT_VOIDRET;
    // Only back propagate the new SafeHandle if the native handle was updated
    // (or we didn't have an input safe handle in the first place).
    if (*pThis->m_psrc == NULL || (*pThis->m_psrc)->GetHandle() != pThis->m_handle)
        SetObjectReference((OBJECTREF*)pThis->m_psrc, (OBJECTREF)(*pThis->m_pProtectedSafeHandleObj), GetAppDomain());
    //END_ENTRYPOINT_VOIDRET;

}

SafeHandle * __stdcall ML_REFSAFEHANDLE_C2N_SR::DoReturnStatic(ML_REFSAFEHANDLE_C2N_SR *pThis)
{

    CONTRACT(SafeHandle*)
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    SafeHandle* handle = NULL;

    //BEGIN_ENTRYPOINT_VOIDRET;

    handle = (SafeHandle*)OBJECTREFToObject(*pThis->m_pProtectedSafeHandleObj);

    //END_ENTRYPOINT_VOIDRET;
    
    RETURN handle;
}

VOID __stdcall ML_RETSAFEHANDLE_C2N_SR::PreReturnStatic(ML_RETSAFEHANDLE_C2N_SR *pThis, MethodTable *pMT, CleanupWorkList *pCleanup)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pCleanup));
    }
    CONTRACTL_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    SAFEHANDLE SafeHandleObj = NULL;
    GCPROTECT_BEGIN(SafeHandleObj)
    {
        SafeHandleObj = (SAFEHANDLE)AllocateObject(pMT);
        CallDefaultConstructor(SafeHandleObj);
        pThis->m_pProtectedSafeHandleObj = (SAFEHANDLE*)pCleanup->NewProtectedObjectRef((OBJECTREF)SafeHandleObj);
    }
    GCPROTECT_END();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    
}

SafeHandle * __stdcall ML_RETSAFEHANDLE_C2N_SR::DoReturnStatic(ML_RETSAFEHANDLE_C2N_SR *pThis, LPVOID *pHandle)
{
    CONTRACT(SafeHandle*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pHandle));
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;


    SafeHandle* handle = NULL;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    SAFEHANDLE SafeHandleObj = *pThis->m_pProtectedSafeHandleObj;
    GCPROTECT_BEGIN(SafeHandleObj)
    {
        SafeHandleObj->SetHandle(*pHandle);
    }
    GCPROTECT_END();

    handle = (SafeHandle*)OBJECTREFToObject(SafeHandleObj);
    
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    RETURN handle;

}

LPVOID __stdcall SafeHandleC2NHelper(SAFEHANDLE *pSafeHandleObj, CleanupWorkList *pCleanup)
{

    CONTRACT(LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pSafeHandleObj));
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    LPVOID handle= NULL;


    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    if (*pSafeHandleObj == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_SafeHandle");

    // The SafeHandleHolder will AddRef the SafeHandle.
    SafeHandleHolder shh(pSafeHandleObj);
    pCleanup->ScheduleSafeHandleRelease(pSafeHandleObj);

    // The release was scheduled successfully so we can suppress
    // releasing the safe handle here.
    shh.SuppressRelease();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    handle = (*pSafeHandleObj)->GetHandle();

    // Return the SafeHandle's internal handle.
    RETURN handle;
}

LPVOID __stdcall ML_REFCRITICALHANDLE_C2N_SR::DoConversionStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis, CRITICALHANDLE *pCriticalHandleObj, MethodTable *pMT, BYTE fInOut, CleanupWorkList *pCleanup)
{
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pCriticalHandleObj, NULL_OK));
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    // Store the source pointer to be able to back propagate later.
    pThis->m_psrc = pCriticalHandleObj;

    // Make sure that output only parameters have an initial null handle (we
    // need this for correct back propagation logic).
    if (fInOut == ML_OUT && pCriticalHandleObj)
        *pCriticalHandleObj = NULL;

    if (fInOut & ML_IN)
    {
        if ((*pCriticalHandleObj) == NULL)
            COMPlusThrow(kArgumentNullException, L"ArgumentNull_CriticalHandle");

        pThis->m_handle = (*pCriticalHandleObj)->GetHandle();
    }

    if (fInOut & ML_OUT)
    {
        CRITICALHANDLE CriticalHandleObj = NULL;
        GCPROTECT_BEGIN(CriticalHandleObj)
        {
            // Pre-allocate a CriticalHandle object to make sure we don't run out of memory
            // after the call out to unmanaged has completed. If this were to happen, we 
            // would leak the handle which is bad.
            CriticalHandleObj = (CRITICALHANDLE)AllocateObject(pMT);
            CallDefaultConstructor(CriticalHandleObj);

            // If the handle isn't valid on input initialize it to the value set
            // by the safe handle ctor above. We want to make sure the handle is
            // either invalid or set to a valid value by the unmanaged function
            // when we back propagate.
            if (!(fInOut & ML_IN))
                pThis->m_handle = CriticalHandleObj->GetHandle();

            pThis->m_pProtectedCriticalHandleObj = (CRITICALHANDLE*)pCleanup->NewProtectedObjectRef((OBJECTREF)CriticalHandleObj);

            // Schedule an unconditional back propagation of the handle in to the CriticalHandle.
            pCleanup->ScheduleRefCriticalHandleBackPropagate(&CriticalHandleObj, &pThis->m_handle, pThis->m_handle);
        }
        GCPROTECT_END();
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    RETURN &pThis->m_handle;
}

void __stdcall ML_REFCRITICALHANDLE_C2N_SR::BackPropagateStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    //BEGIN_ENTRYPOINT_VOIDRET;

    // Only back propagate the new CriticalHandle if the native handle was updated
    // (or we didn't have an input critical handle in the first place).
    if (*pThis->m_psrc == NULL || (*pThis->m_psrc)->GetHandle() != pThis->m_handle)
        SetObjectReference((OBJECTREF*)pThis->m_psrc, (OBJECTREF)(*pThis->m_pProtectedCriticalHandleObj), GetAppDomain());

    //END_ENTRYPOINT_VOIDRET;
}

CriticalHandle * __stdcall ML_REFCRITICALHANDLE_C2N_SR::DoReturnStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis)
{
    CONTRACT(CriticalHandle*)
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    CriticalHandle* handle = NULL;
    //BEGIN_ENTRYPOINT_VOIDRET;

    handle = (CriticalHandle*)OBJECTREFToObject(*pThis->m_pProtectedCriticalHandleObj);

    //END_ENTRYPOINT_VOIDRET;

    RETURN handle;
}

VOID __stdcall ML_RETCRITICALHANDLE_C2N_SR::PreReturnStatic(ML_RETCRITICALHANDLE_C2N_SR *pThis, MethodTable *pMT, CleanupWorkList *pCleanup)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pCleanup));
    }
    CONTRACTL_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    CRITICALHANDLE CriticalHandleObj = NULL;
    GCPROTECT_BEGIN(CriticalHandleObj)
    {
        CriticalHandleObj = (CRITICALHANDLE)AllocateObject(pMT);
        CallDefaultConstructor(CriticalHandleObj);
        pThis->m_pProtectedCriticalHandleObj = (CRITICALHANDLE*)pCleanup->NewProtectedObjectRef((OBJECTREF)CriticalHandleObj);
    }
    GCPROTECT_END();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
}

CriticalHandle * __stdcall ML_RETCRITICALHANDLE_C2N_SR::DoReturnStatic(ML_RETCRITICALHANDLE_C2N_SR *pThis, LPVOID *pHandle)
{
    CONTRACT(CriticalHandle*)
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pHandle));
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    CriticalHandle* handle = NULL;

    //BEGIN_ENTRYPOINT_VOIDRET;

    CRITICALHANDLE CriticalHandleObj = *pThis->m_pProtectedCriticalHandleObj;
    GCPROTECT_BEGIN(CriticalHandleObj)
    {
        CriticalHandleObj->SetHandle(*pHandle);
    }
    GCPROTECT_END();

    handle = (CriticalHandle*)OBJECTREFToObject(CriticalHandleObj);
    //END_ENTRYPOINT_VOIDRET;

    RETURN handle;
}

LPVOID ML_REFVALUECLASS_N2C_SR::DoConversion(LPVOID pUmgdVALUECLASS, BYTE fInOut, MethodTable *pMT, CleanupWorkList *pCleanup)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pUmgdVALUECLASS, NULL_OK));
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    // Since refs cannot be NULL in the managed world, we need to guard against
    // unmanaged code passing in NULL for a ref structure.
    if (!pUmgdVALUECLASS)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

    m_fInOut = fInOut;
    m_pMT    = pMT;
    m_pUnmgdVC = pUmgdVALUECLASS;

    OBJECTREF BoxedVALUECLASSObj = pMT->Allocate();
    m_pBoxedVC = pCleanup->NewProtectedObjectRef(BoxedVALUECLASSObj);

    if (fInOut & ML_IN)
        LayoutUpdateCLR((VOID**)m_pBoxedVC, Object::GetOffsetOfFirstField(), pMT, (BYTE*)pUmgdVALUECLASS, FALSE);

    RETURN (LPVOID)((*m_pBoxedVC)->GetData());
}
        

VOID ML_REFVALUECLASS_N2C_SR::BackPropagate(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    if (m_fInOut & ML_OUT)
    {
        EX_TRY
        {
            if (m_fInOut & ML_IN)
                FmtClassDestroyNative(m_pUnmgdVC, m_pMT);
                
            LayoutUpdateNative((VOID**)m_pBoxedVC, Object::GetOffsetOfFirstField(), m_pMT, (BYTE*)m_pUnmgdVC, NULL);
        }
        EX_CATCH
        {
            *pfDeferredException = TRUE;
        }
        EX_END_CATCH (SwallowAllExceptions);
    }
}

LPVOID ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR::DoConversion(const VOID * psrc, UINT32 cbSize)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(psrc != NULL);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    m_psrc = psrc;
    m_cbSize = cbSize;
    m_pTempCopy = (LPVOID)(GetThread()->m_MarshalAlloc.Alloc(cbSize));
    RETURN m_pTempCopy;
}

VOID ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR::BackPropagate(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    memcpyNoGCRefs(*((VOID**)m_psrc), m_pTempCopy, m_cbSize);
}




LPSTR ML_CSTR_C2N_SR::DoConversion(STRINGREF pStringRef, UINT32 fBestFitMapping, UINT32 fThrowOnUnmappableChar, CleanupWorkList *pCleanup)
{
    CONTRACT (LPSTR)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pCleanup));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    if (pStringRef == NULL)
        RETURN NULL;
    
    else
    {
        BYTE  *pbuf = NULL;

        HRESULT hr = S_OK;
        BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), hr = COR_E_STACKOVERFLOW);

        int    nc;
        WCHAR *pc;
        RefInterpretGetStringValuesDangerousForGC(pStringRef, &pc, &nc);
        
        int    cb = (nc + 1) * GetMaxDBCSCharByteSize() + 1;
        pbuf = m_buf;
        if (cb > (int) sizeof(m_buf))
        {   
            pbuf = (BYTE*)CoTaskMemAlloc(cb);
            if (!pbuf)
                ENCLOSE_IN_EXCEPTION_HANDLER( COMPlusThrowOM );  // called by managed code by way of a stub
            
            if ( FAILED( pCleanup->ScheduleCoTaskFreeNonThrow(pbuf) ) )
                ENCLOSE_IN_EXCEPTION_HANDLER( COMPlusThrowOM );
        }
        
        DWORD mblength = 0;
        mblength = InternalWideToAnsi(pStringRef->GetBuffer(),
                                      nc+1,
                                      (LPSTR) pbuf,
                                      ((nc+1) * GetMaxDBCSCharByteSize()) + 1,
                                      fBestFitMapping,
                                      fThrowOnUnmappableChar);
        ((CHAR*)pbuf)[mblength] = '\0';

        END_SO_INTOLERANT_CODE;

        if (hr == COR_E_STACKOVERFLOW)
            ENCLOSE_IN_EXCEPTION_HANDLER( COMPlusThrowSO );  // called by managed code by way of a stub
            
        RETURN (LPSTR)(pbuf);
    }
}




LPWSTR ML_WSTRBUILDER_C2N_SR::DoConversion(STRINGBUFFERREF *ppProtectedStringBuffer, CleanupWorkList *pCleanup)
{
    CONTRACT (LPWSTR)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(ppProtectedStringBuffer != NULL);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    m_ppProtectedStringBuffer = ppProtectedStringBuffer;
    STRINGBUFFERREF stringRef = *m_ppProtectedStringBuffer;

    if (stringRef == NULL)
        m_pNative = NULL;

    else
    {
        UINT32 capacity = (UINT32)COMStringBuffer::NativeGetCapacity(stringRef);
        m_pNative = (LPWSTR)(GetThread()->m_MarshalAlloc.Alloc((unsigned int) (max(256, (capacity+3) * sizeof(WCHAR)))));
        m_pNative[capacity+1] = L'\0';
        m_pSentinel = &(m_pNative[capacity+1]);
        m_pSentinel[1] = 0xabab;
#ifdef _DEBUG
        FillMemory(m_pNative, (capacity+1)*sizeof(WCHAR), 0xcc);
#endif
        SIZE_T length = COMStringBuffer::NativeGetLength(stringRef);

        memcpyNoGCRefs((WCHAR *) m_pNative, COMStringBuffer::NativeGetBuffer(stringRef),
                       length * sizeof(WCHAR));
        ((WCHAR*)m_pNative)[length] = 0;
    }
    RETURN m_pNative;
}



VOID ML_WSTRBUILDER_C2N_SR::BackPropagate(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (m_pNative != NULL)
    {
        if (L'\0' != m_pSentinel[0] || 0xabab != m_pSentinel[1])
        {
            // ! Though our normal protocol is to set *pfDeferredException to TRUE and return normally,
            // ! this mechanism doesn't let us return informative exceptions. So we'll throw this one 
            // ! immediately. The excuse for doing this is that this exception's only purpose in
            // ! life is to tell the programmer that he introduced a bug that probably corrupted the heap.
            // ! Hence, assuming the process even survives long enough to deliver our message,
            // ! memory leaks from any other backpropagations bypassed as a result is the least of 
            // ! our problems.
            COMPlusThrow(kIndexOutOfRangeException, IDS_PINVOKE_STRINGBUILDEROVERFLOW);
        }

        COMStringBuffer::ReplaceBuffer(m_ppProtectedStringBuffer,
                                       m_pNative, (INT32)wcslen(m_pNative));
    }
}



LPSTR __stdcall ML_CSTRBUILDER_C2N_SR::DoConversionStatic(ML_CSTRBUILDER_C2N_SR* pThis, STRINGBUFFERREF *ppProtectedStringBuffer, BYTE fBestFitMapping, BYTE fThrowOnUnmappableChar, CleanupWorkList *pCleanup)
{
    CONTRACT (LPSTR)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pThis));
        PRECONDITION(ppProtectedStringBuffer != NULL);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;


    pThis->m_ppProtectedStringBuffer = ppProtectedStringBuffer;
    STRINGBUFFERREF stringRef = *(pThis->m_ppProtectedStringBuffer);

    if (stringRef == NULL)
    {
        pThis->m_pNative = NULL;
    }
    else
    {
        BEGIN_ENTRYPOINT_THROWS;
        UINT32 capacity = (UINT32)COMStringBuffer::NativeGetCapacity(stringRef);
        UINT32 capacityPlusSentinal = 0;

        if (!ClrSafeInt<UINT32>::multiply(capacity, GetMaxDBCSCharByteSize(), capacity))
            COMPlusThrow(kIndexOutOfRangeException);
        if (!ClrSafeInt<UINT32>::addition(capacity, 5, capacityPlusSentinal))
            COMPlusThrow(kIndexOutOfRangeException);

        // capacity is the count of wide chars, allocate buffer big enough for maximum
        // conversion to DBCS.
        pThis->m_pNative = (LPSTR)(GetThread()->m_MarshalAlloc.AllocSafeThrow(max(256, capacityPlusSentinal)));

        pThis->m_pSentinel = &(pThis->m_pNative[capacity+1]);
        ((CHAR*)pThis->m_pSentinel)[0] = '\0';
        ((CHAR*)pThis->m_pSentinel)[1] = '\0';
        ((CHAR*)pThis->m_pSentinel)[2] = '\0';
        ((CHAR*)pThis->m_pSentinel)[3] = (CHAR)(SIZE_T)0xab;

#ifdef _DEBUG
        FillMemory(pThis->m_pNative, (capacity+1) * sizeof(CHAR), 0xcc);
#endif
        // Due to some Windows APIs returning non zero-terminated strings
        //  on win9x DBCS systems, we'll eagerly fill the string with zeros
        //  to catch this case.
        if (RunningOnWin95())
            FillMemory(pThis->m_pNative, (capacity+1) * sizeof(CHAR), 0x00);


        UINT32 length = (UINT32)COMStringBuffer::NativeGetLength(stringRef);
        DWORD mblength = 0;

        mblength = InternalWideToAnsi(COMStringBuffer::NativeGetBuffer(stringRef),
                                      length,
                                      (LPSTR) pThis->m_pNative,
                                      capacity + 1,
                                      fBestFitMapping,
                                      fThrowOnUnmappableChar);
        ((CHAR*)pThis->m_pNative)[mblength] = '\0';
        
        END_ENTRYPOINT_THROWS;
    }

    RETURN pThis->m_pNative;
}



VOID __stdcall ML_CSTRBUILDER_C2N_SR::BackPropagateStatic(ML_CSTRBUILDER_C2N_SR* pThis)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pThis));
    }
    CONTRACTL_END;

    
    if (pThis->m_pNative != NULL)
    {
        INSTALL_UNWIND_AND_CONTINUE_HANDLER;

        if (0 != pThis->m_pSentinel[0] || 
            0 != pThis->m_pSentinel[1] ||
            0 != pThis->m_pSentinel[2]  ||
            (CHAR)(SIZE_T)0xab != pThis->m_pSentinel[3])
        {
            // ! Though our normal protocol is to set *pfDeferredException to TRUE and return normally,
            // ! this mechanism doesn't let us return informative exceptions. So we'll throw this one 
            // ! immediately. The excuse for doing this is that this exception's only purpose in
            // ! life is to tell the programmer that he introduced a bug that probably corrupted the heap.
            // ! Hence, assuming the process even survives long enough to deliver our message,
            // ! memory leaks from any other backpropagations bypassed as a result is the least of 
            // ! our problems.
            COMPlusThrow(kIndexOutOfRangeException, IDS_PINVOKE_STRINGBUILDEROVERFLOW);
        }


        COMStringBuffer::ReplaceBufferAnsi(pThis->m_ppProtectedStringBuffer,
                                           pThis->m_pNative, (INT32)strlen(pThis->m_pNative));
                                           
        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    }
}



void ML_STRUCTRETN2C_SR::MarshalRetVal(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    EX_TRY
    {
        LayoutUpdateNative( (LPVOID*)m_ppProtectedBoxedObj, Object::GetOffsetOfFirstField(), m_pMT, (BYTE*)m_pNativeRetBuf, NULL);
    }
    EX_CATCH
    {
        *pfDeferredException = TRUE;
    }
    EX_END_CATCH (SwallowAllExceptions);
}




void ML_STRUCTRETC2N_SR::MarshalRetVal(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    EX_TRY
    {
        LayoutUpdateCLR( m_ppProtectedValueTypeBuf, 0, m_pMT, (BYTE*)m_pNativeRetBuf, TRUE);
    }
    EX_CATCH
    {
        *pfDeferredException = TRUE;
    }
    EX_END_CATCH (SwallowAllExceptions);
}



void ML_CURRENCYRETC2N_SR::MarshalRetVal(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    GCX_PREEMP();
    
    HRESULT hr = S_OK;
    EX_TRY
    {
        hr = VarDecFromCy(m_cy, *m_ppProtectedValueTypeBuf);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);
    
    if (FAILED(hr))
    {
        *pfDeferredException = TRUE;
        return;
    }

    if (FAILED(DecimalCanonicalize(*m_ppProtectedValueTypeBuf)))
        *pfDeferredException = TRUE;
}


void ML_CURRENCYRETN2C_SR::MarshalRetVal(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pfDeferredException));
    }
    CONTRACTL_END;

    GCX_PREEMP();
    
    HRESULT hr = S_OK;
    EX_TRY
    {
        hr = VarCyFromDec(&m_decimal, m_pcy);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (FAILED(hr))
        *pfDeferredException = TRUE;
}


void ML_DATETIMERETN2C_SR::MarshalRetVal(BOOL *pfDeferredException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    *m_pdate = COMDateTime::TicksToDoubleDate(m_datetime);
}


//====================================================================
// Helper fcns called from standalone stubs.
//====================================================================
VOID STDMETHODCALLTYPE DoMLCreateMarshalerWStr(Frame *pFrame, CleanupWorkList *pCleanup, UINT8 *plocalwalk)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pCleanup));
        PRECONDITION(CheckPointer(plocalwalk));        
    }
    CONTRACTL_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    pCleanup->IsVisibleToGc();
    new (plocalwalk) WSTRMarshaler(pCleanup);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

}

VOID STDMETHODCALLTYPE DoMLCreateMarshalerCStr(Frame *pFrame, CleanupWorkList *pCleanup, UINT8 *plocalwalk, UINT32 fBestFitMapping, UINT32 fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pCleanup));
        PRECONDITION(CheckPointer(plocalwalk));        
    }
    CONTRACTL_END;
    
    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    pCleanup->IsVisibleToGc();
    new (plocalwalk) CSTRMarshaler(pCleanup);
    ((CSTRMarshaler*)plocalwalk)->SetBestFitMap(fBestFitMapping);
    ((CSTRMarshaler*)plocalwalk)->SetThrowOnUnmappableChar(fThrowOnUnmappableChar);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
}


VOID STDMETHODCALLTYPE DoMLPrereturnC2N(Marshaler *pMarshaler, LPVOID pstackout)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pMarshaler));
        PRECONDITION(CheckPointer(pstackout));
    }
    CONTRACTL_END;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    pMarshaler->PrereturnCLRFromNativeRetval(NULL, pstackout);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

}


LPVOID STDMETHODCALLTYPE DoMLReturnC2NRetVal(Marshaler *pMarshaler)
{
    CONTRACT (LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pMarshaler));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    // WARNING!!!!! "dstobjref" holds an OBJECTREF: Don't add any operations
    // that might cause a GC!
    LPVOID dstobjref = NULL;

    
    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub
    
    pMarshaler->ReturnCLRFromNativeRetval( (LPVOID)&dstobjref, NULL );

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    
    RETURN dstobjref;
}


