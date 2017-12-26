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

// ML.H -
//
// Marshaling engine.

#ifndef __ML_H__
#define __ML_H__


//=========================================================================
// Support for ML (Marshaling Language.)
//
// ML is a special-purpose interpreted language used for implementing
// parameter-translation thunks between managed and unmanaged code. Its
// main clients are N/Direct, the managed-unmanaged thunks for IAT's
// and exports and COM interop.
//
// ML has an easily JIT-compilable subset so the interpreter is used
// only for thunking calls with complex data types where the basic cost
// of translating the data overwhelms the interpreter overhead.
// Hence, minimizing ML stub size is given priority over speed of
// interpretation.
//
// The ML interpreter state consists of:
//
//   - A "SRC" post-incremented pointer which walks over
//     a buffer containing arguments to be translated.
//
//   - A "DST" pointer which walks over a buffer which receives
//     the translated arguments. Depending on a flag passed to the
//     interpreter (RunML), "DST" is either post-incremented or
//     pre-decremented. This is required since depending on the
//     mutual calling convention, the ML stream may need to reverse
//     the order of the arguments while translating them.
//
//   - A "LOCALWALK" post-incremented pointer which walks over
//     the LOCAL array, an array of bytes that the ML stream can use
//     for temporary storage. Each ML opcode uses a fixed number of local
//     bytes (most of them zero.) Each time an ML opcode is executed,
//     LOCALWALK is incremented by the associated number of bytes.
//
//   - A "LOCAL" pointer which never changes value. This always points
//     to the start of the local space so that locals can be
//     accessed via a fixed offset rather than thru LOCALWALK.
//
//   - A pointer to a CleanupWorkList structure where ML opcodes
//     can throw stuff to be cleaned up on exceptions and exit.
//     The ML opcodes that specifically require a CleanupWorkList
//     are marked as such in the gMLInfo database. If no such instructions
//     appear in the ML stream, you need not provide a CleanupWorkList.
//
// Most ML opcodes perform the following action:
//
//     1. Load some value from *SRC, and increment SRC by the size of
//        that value.
//     2. Perform a conversion on that value.
//     3. If the direction mode is -1, predecrement DST by the size of the
//        result.
//     4. Store the result in *DST.
//     5. If the direction mode is +1, postincrement DST by the size of the
//        value.
//
// Some ML opcodes work in pairs, e.g. one to convert a StringBuffer to
// an LPWSTR prior to a DLL function call, then another to copy any changes
// back to the StringBuffer after the DLL function call. Such opcode pairs
// typically use the LOCAL space to communicate.
//
// The MLOPDEF.H file contains the definitions of all ML opcodes.
//
// ML opcodes should all be designed so that an ML-stream is relocatable
// without fixups. This allows two ML stubs to be compared for equality
// (and thus, reusability) via a simple comparision algorithm.
//=========================================================================



#include "vars.hpp"
#include "util.hpp"
#include "object.h"
#include "comvariant.h"

class CleanupWorkList;
struct MLInfo;

typedef UINT8 MLCode;


extern const MLInfo gMLInfo[];


#define ML_IN  0x10
#define ML_OUT 0x20

//----------------------------------------------------------------------
// Create the enum for each ML opcode.
//----------------------------------------------------------------------
#undef DEFINE_ML
#define DEFINE_ML(name,operandbytes,frequiredCleanup,cblocals,Hndl) name,
enum _MLOpcode
{
#include "mlopdef.h"
    ML_COUNT,           // defines number of ML opcodes
};


#undef DEFINE_ML
#define DEFINE_ML(name,operandbytes,frequiredCleanup,cblocals,hndl) 
enum _MLArrayMarshaler
{
#include "mlopdef.h"
};


//===========================================================================
// A common header that precedes ML stubs for NDirect & COM interop.
// This doesn't really belong in ml.h but I don't want to create another
// header just for this.
//===========================================================================

#define MLHF_TYPECAT_NORMAL               0          //! This value must stay at zero!!!
#define MLHF_TYPECAT_VALUECLASS           1
#define MLHF_TYPECAT_FPU                  2
#define MLHF_TYPECAT_GCREF                3          //Requires GC protection 

#define MLHF_MANAGEDRETVAL_TYPECAT_MASK   0x0003
#define MLHF_UNMANAGEDRETVAL_TYPECAT_MASK 0x000c
#define MLHF_64BITMANAGEDRETVAL           0x0010     // Managed return value is 64 bits
#define MLHF_64BITUNMANAGEDRETVAL         0x0020     // Unmanaged return value is 64 bits
#define MLHF_NATIVERESULT                 0x0040     // com call returns native result (not HRESULT)
#define MLHF_SETLASTERROR                 0x0080     // Must preserve last error (N/Direct only)
#define MLHF_THISCALL                     0x0100     // Requires thiscall mod
#define MLHF_THISCALLHIDDENARG            0x0200     // Requires thiscall and has hidden structure buffer arg
#define MLHF_DISPCALLWITHWRAPPERS         0x0400     // Dispatch call that requires arg wrapping

#ifdef LOP_FRIENDLY_FRAMES
#define MLHF_LOP_FRIENDLY_FRAMES_CALLER_POPS_ARGS             0x1000     // Additional flag to propagate cdecl so we can disable
#endif //LOP_FRIENDLY_FRAMES                         // the LOP profilable stub generation for cdecl


#define MLHF_NEEDS_RESTORING              0x8000     // ML stub needs restore from prejit state

// These flags need to special marshalling, and IL compilers can inline the stub
#define MLHF_CAN_BE_INLINED_MASK          (MLHF_64BITMANAGEDRETVAL|MLHF_THISCALL)

// MLHeader and the following MLCode double as hash keys
// for the MLStubCache. Thus, it's imperative that there be no
// unused "pad" fields that contain unstable values.
#include <pshpack1.h>

typedef DPTR(struct MLHeader) PTR_MLHeader;
struct MLHeader
{
    const MLCode* GetMLCode() const
    {
        LEAF_CONTRACT;
        
        return (const MLCode *)(PTR_HOST_TO_TADDR(this) + sizeof(*this));
    }

    VOID SetManagedRetValTypeCat(int mlhftypecat)
    {
        LEAF_CONTRACT;
        
        m_Flags &= ~3;
        m_Flags |= mlhftypecat;
    }

    VOID SetUnmanagedRetValTypeCat(int mlhftypecat)
    {
        LEAF_CONTRACT;
        
        m_Flags &= ~0x0c;
        m_Flags |= (mlhftypecat << 2);
    }

    int GetManagedRetValTypeCat() const
    {
        LEAF_CONTRACT;
        
        return m_Flags & 3;
    }

    int GetUnmanagedRetValTypeCat() const
    {
        LEAF_CONTRACT;
        
        return (m_Flags >> 2) & 3;
    }

    BOOL ReturnsValueClass() const
    {
        WRAPPER_CONTRACT;
        
        return MLHF_TYPECAT_VALUECLASS == this->GetManagedRetValTypeCat();
    }

    VOID SetReturnValueClass() 
    {
        WRAPPER_CONTRACT;
        
        SetManagedRetValTypeCat(MLHF_TYPECAT_VALUECLASS);
    }

    UINT16        m_cbDstBuffer;  //# of bytes required in the destination buffer
    UINT16        m_cbLocals;     //# of bytes required in the local array
    UINT16        m_cbStackPop;   //# of stack bytes that must be popped off (== CbStackPop)
    UINT16        m_Flags;        //flags (see MLHF_* values)   
};

#include <poppack.h>


//----------------------------------------------------------------------
// Declare a database of MLCode information.
//----------------------------------------------------------------------
struct MLInfo
{
    // Size, in bytes, of the ML instruction not including the opcode byte
    // itself.
    UINT8       m_numOperandBytes;

    // Boolean: does it or does not require a valid CleanupWorkList.
    UINT8       m_frequiresCleanup;

    // Number of bytes of localspace used.
    UINT16      m_cbLocal;

    // Boolean: does it require a handle
    UINT8       m_frequiresHandle;

#ifdef _DEBUG
    // Human-readable name of ML opcode.
    LPCSTR      m_szDebugName;
#endif
};

//----------------------------------------------------------------------
// struct to compute the summary of a series of ML codes
//----------------------------------------------------------------------
struct MLSummary
{
    MLSummary() : 
        m_fRequiresCleanup(0),
        m_cbMLSize(0),
        m_cbTotalLocals(0),
        m_cbTotalHandles(0)
    {
        LEAF_CONTRACT;
    }

    // helper that computes summary info
    VOID ComputeMLSummary(const MLCode *pMLCode);

    // whether this stream requires cleanup
    BOOL        m_fRequiresCleanup;
    // total size of the ML Stream
    unsigned    m_cbMLSize;
    // total size of the locals
    unsigned    m_cbTotalLocals;
    // total number of handles needed
    unsigned    m_cbTotalHandles;
};


//----------------------------------------------------------------------
// ArrayWithOffset
//----------------------------------------------------------------------
struct ML_ARRAYWITHOFFSET_C2N_SR
{
public:
    //----------------------------------------------------------------------
    // Convert ArrayWithOffset to native array
    //----------------------------------------------------------------------
    LPVOID DoConversion(BASEARRAYREF *ppProtectedArrayRef, //pointer to GC-protected BASERARRAYREF,
                        INT32         cbOffset,
                        INT32         cbCount,
                        CleanupWorkList *pCleanup);

    //----------------------------------------------------------------------
    // Backpropagates changes to the native array back to the COM+ array.
    //----------------------------------------------------------------------
    VOID   BackPropagate();

private:
    enum
    {
        //----------------------------------------------------------------------
        // Size in bytes of the stack buffer used for short arrays.
        //----------------------------------------------------------------------
#ifdef _DEBUG
        kStackBufferSize = 4
#else
        kStackBufferSize = 270
#endif
    };

    BASEARRAYREF*    m_ppProtectedArrayRef;
    UINT32           m_cbOffset;
    UINT32           m_cbCount;
    LPVOID           m_pNativeArray;
    BYTE             m_StackBuffer[kStackBufferSize];
};


//----------------------------------------------------------------------
// A rather hacky way of marshaling byref valuetypes from unmanaged to managed.
// We have to use this backdoor route because the marshaling architecture
// makes it hard to do the necessary GC promoting.
//----------------------------------------------------------------------
struct ML_REFVALUECLASS_N2C_SR
{
public:
    LPVOID       DoConversion(LPVOID pUmgdVC, BYTE fInOut, MethodTable *pMT, CleanupWorkList *pCleanup);
    VOID         BackPropagate(BOOL *pfDeferredException);

private:
    OBJECTREF*   m_pBoxedVC;  // Boxed valueclass
    LPVOID       m_pUnmgdVC;
    MethodTable* m_pMT;
    BYTE         m_fInOut;
};


struct ML_REFVALUECLASS_C2N_SR
{
public:
    LPVOID DoConversion(LPVOID          *ppProtectedData,
                        MethodTable     *pMT,
                        BYTE             fInOut,
                        CleanupWorkList *pCleanup);
    
    VOID BackPropagate(BOOL *pfDeferredException);

private:
    LPVOID*      m_ppProtectedData;
    MethodTable* m_pMT;
    BYTE*        m_buf;
    BYTE         m_inout;
};


struct ML_REFSAFEHANDLE_C2N_SR
{
public:
    LPVOID DoConversion(SAFEHANDLE      *ppProtectedData,
                        MethodTable     *pMT,
                        BYTE             fInOut,
                        CleanupWorkList *pCleanup)
    {
        return DoConversionStatic(this, ppProtectedData, pMT, fInOut, pCleanup);
    }

    static LPVOID __stdcall DoConversionStatic(ML_REFSAFEHANDLE_C2N_SR *pThis,
                                     SAFEHANDLE      *ppProtectedData,
                                     MethodTable     *pMT,
                                     BYTE             fInOut,
                                     CleanupWorkList *pCleanup);

    void BackPropagate()
    {
        BackPropagateStatic(this);
    }
    static void __stdcall BackPropagateStatic(ML_REFSAFEHANDLE_C2N_SR *pThis);

    SAFEHANDLE DoReturn()
    {
        return (SAFEHANDLE)DoReturnStatic(this);
    }
    static SafeHandle * __stdcall DoReturnStatic(ML_REFSAFEHANDLE_C2N_SR *pThis);

private:
    LPVOID          m_handle;
    SAFEHANDLE*     m_pProtectedSafeHandleObj;
    SAFEHANDLE*     m_psrc;
};


struct ML_RETSAFEHANDLE_C2N_SR
{
public:
    VOID PreReturn(MethodTable     *pMT,
                   CleanupWorkList *pCleanup)
    {
        PreReturnStatic(this, pMT, pCleanup);
    }
    static VOID __stdcall PreReturnStatic(ML_RETSAFEHANDLE_C2N_SR *pThis,
                                MethodTable     *pMT,
                                CleanupWorkList *pCleanup);

    SAFEHANDLE DoReturn(LPVOID *pHandle)
    {
        return (SAFEHANDLE)DoReturnStatic(this, pHandle);
    }
    static SafeHandle * __stdcall DoReturnStatic(ML_RETSAFEHANDLE_C2N_SR *pThis,
                                      LPVOID *pHandle);

    static INT32 OffsetOfHandleRef()
    {
        return offsetof(ML_RETSAFEHANDLE_C2N_SR, m_pProtectedSafeHandleObj);
    }

private:
    SAFEHANDLE*      m_pProtectedSafeHandleObj;
};


LPVOID __stdcall SafeHandleC2NHelper(SAFEHANDLE *pSafeHandleObj, CleanupWorkList *pCleanup);


struct ML_REFCRITICALHANDLE_C2N_SR
{
public:
    LPVOID DoConversion(CRITICALHANDLE  *ppProtectedData,
                        MethodTable     *pMT,
                        BYTE             fInOut,
                        CleanupWorkList *pCleanup)
    {
        return DoConversionStatic(this, ppProtectedData, pMT, fInOut, pCleanup);
    }

    static LPVOID __stdcall DoConversionStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis,
                                     CRITICALHANDLE  *ppProtectedData,
                                     MethodTable     *pMT,
                                     BYTE             fInOut,
                                     CleanupWorkList *pCleanup);

    void BackPropagate()
    {
        BackPropagateStatic(this);
    }
    static void __stdcall BackPropagateStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis);

    CRITICALHANDLE DoReturn()
    {
        return (CRITICALHANDLE)DoReturnStatic(this);
    }
    static CriticalHandle * __stdcall DoReturnStatic(ML_REFCRITICALHANDLE_C2N_SR *pThis);

private:
    LPVOID          m_handle;
    CRITICALHANDLE* m_pProtectedCriticalHandleObj;
    CRITICALHANDLE* m_psrc;
};


struct ML_RETCRITICALHANDLE_C2N_SR
{
public:
    VOID PreReturn(MethodTable     *pMT,
                   CleanupWorkList *pCleanup)
    {
        PreReturnStatic(this, pMT, pCleanup);
    }
    static VOID __stdcall PreReturnStatic(ML_RETCRITICALHANDLE_C2N_SR *pThis,
                                MethodTable     *pMT,
                                CleanupWorkList *pCleanup);

    CRITICALHANDLE DoReturn(LPVOID *pHandle)
    {
        return (CRITICALHANDLE)DoReturnStatic(this, pHandle);
    }
    static CriticalHandle * __stdcall DoReturnStatic(ML_RETCRITICALHANDLE_C2N_SR *pThis,
                                          LPVOID *pHandle);

    static INT32 OffsetOfHandleRef()
    {
        return offsetof(ML_RETCRITICALHANDLE_C2N_SR, m_pProtectedCriticalHandleObj);
    }

private:
    CRITICALHANDLE* m_pProtectedCriticalHandleObj;
};


//----------------------------------------------------------------------
// For N/Direct "ole" calls, this is the return value buffer that
// the marshaling engine pushes on the stack.
//----------------------------------------------------------------------
union RetValBuffer
{
    UINT32      m_i32;
    UINT64      m_i64;
    LPVOID      m_ptr;
};


#include <pshpack1.h>
// This ML opcode takes lot of parameters that we post-patch: to keep everyone in sync,
// we capture its format in a structure.
struct ML_CREATE_MARSHALER_CARRAY_OPERANDS
{
    MethodTable*    methodTable;
    UINT32          multiplier;
    UINT32          additive;
    VARTYPE         elementType;
    union 
    {
        UINT16      countParamIdx;      //before patching
        INT16       offsetbump;         //after patching
    };
    UINT8           countSize;          //if 0, sizeiz computation disabled: use managed size of array instead
    BYTE            bestfitmapping;
    BYTE            throwonunmappablechar;
};
#include <poppack.h>


// Handle return values of structures.
struct ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR
{
public:
    LPVOID DoConversion(const VOID* psrc, UINT32 cbSize);
    VOID BackPropagate(BOOL* pfDeferredException);
private:
    const VOID*     m_psrc;
    
    // the managed structure can move due to GC, so need temporary fixed buffer
    LPVOID          m_pTempCopy;
    UINT32          m_cbSize;
};

// Handle CSTR's
struct ML_CSTR_C2N_SR
{
public:
    LPSTR DoConversion(STRINGREF pStringRef, UINT32 fBestFitMapping, UINT32 fThrowOnUnmappableChar, CleanupWorkList *pCleanup);

private:
#ifdef _DEBUG
    BYTE    m_buf[8];
#else
    BYTE    m_buf[256];
#endif
};


// Managed layout for SRI.HandleRef class
struct HANDLEREF
{
    OBJECTREF m_wrapper;
    LPVOID    m_handle;
};

// Handle WSTR buffers
struct ML_WSTRBUILDER_C2N_SR
{
public:
    LPWSTR   DoConversion(STRINGBUFFERREF *ppProtectedStringBuffer, CleanupWorkList *pCleanup);
    VOID     BackPropagate(BOOL *pfDeferredException);

private:
    STRINGBUFFERREF* m_ppProtectedStringBuffer;
    LPWSTR          m_pNative;
    LPWSTR          m_pSentinel;
};

// Handle CSTR buffers
struct ML_CSTRBUILDER_C2N_SR
{
public:
    static LPSTR __stdcall DoConversionStatic(ML_CSTRBUILDER_C2N_SR* pThis, STRINGBUFFERREF *ppProtectedStringBuffer, BYTE fBestFitMapping, BYTE fThrowOnUnmappableChar, CleanupWorkList *pCleanup);
    LPSTR DoConversion(STRINGBUFFERREF *ppProtectedStringBuffer, BYTE fBestFitMapping, BYTE fThrowOnUnmappableChar, CleanupWorkList *pCleanup)
    {
        return DoConversionStatic(this, ppProtectedStringBuffer, fBestFitMapping, fThrowOnUnmappableChar, pCleanup);
    }
    
    static VOID __stdcall BackPropagateStatic(ML_CSTRBUILDER_C2N_SR* pThis);
    VOID     BackPropagate()
    {
        BackPropagateStatic(this);
    }

    

private:
    STRINGBUFFERREF*    m_ppProtectedStringBuffer;
    LPSTR               m_pNative; 
    LPSTR               m_pSentinel;
};


// Handle N->C calls to fcns that return structures
struct ML_STRUCTRETN2C_SR
{
public:
    void            MarshalRetVal(BOOL *pfDeferredException);

    LPVOID          m_pNativeRetBuf;
    OBJECTREF*      m_ppProtectedBoxedObj;
    MethodTable*    m_pMT;

};


// Handle C->N calls to fcns that return structures
struct ML_STRUCTRETC2N_SR
{
public:
    void            MarshalRetVal(BOOL *pfDeferredException);

    LPVOID          m_pNativeRetBuf;
    LPVOID*         m_ppProtectedValueTypeBuf;
    MethodTable*    m_pMT;
};

// Handle C->N calls to fcns that return currency
struct ML_CURRENCYRETC2N_SR
{
public:
    void            MarshalRetVal(BOOL *pfDeferredException);
    
    CURRENCY        m_cy;
    DECIMAL**       m_ppProtectedValueTypeBuf;
};


// Handle N->C calls to fcns that return currencies
struct ML_CURRENCYRETN2C_SR
{
public:
    void            MarshalRetVal(BOOL *pfDeferredException);
    
    CURRENCY*       m_pcy;      //unmanaged 
    DECIMAL         m_decimal;  //managed 
};

// Handle N->C calls to fcns that return currencies
struct ML_DATETIMERETN2C_SR
{
public:
    void            MarshalRetVal(BOOL *pfDeferredException);
    
    DATE*           m_pdate; //unmanaged
    INT64           m_datetime; //managed
};



VOID STDMETHODCALLTYPE DoMLCreateMarshalerWStr(Frame *pFrame, CleanupWorkList *pCleanup, UINT8 *plocalwalk);
VOID STDMETHODCALLTYPE DoMLCreateMarshalerCStr(Frame *pFrame, CleanupWorkList *pCleanup, UINT8 *plocalwalk, UINT32 fBestFitMapping, UINT32 fThrowOnUnmappableChar);
VOID STDMETHODCALLTYPE DoMLPrereturnC2N(Marshaler *pMarshaler, LPVOID pstackout);
LPVOID STDMETHODCALLTYPE DoMLReturnC2NRetVal(Marshaler *pMarshaler);


//----------------------------------------------------------------------
// Computes the length of an MLCode stream in bytes, including
// the terminating ML_END opcode.
//----------------------------------------------------------------------
UINT MLStreamLength(const MLCode * const pMLCode);

//----------------------------------------------------------------------
// checks if MLCode stream requires cleanup
//----------------------------------------------------------------------
BOOL MLStreamRequiresCleanup(const MLCode  *pMLCode);


//----------------------------------------------------------------------
// Executes MLCode up to the next ML_END or ML_INTERRUPT opcode.
//
// Inputs:
//    psrc             - sets initial value of SRC register
//    pdst             - sets initial value of DST register
//    plocals          - pointer to ML local var array
//    pParmInfo        - (optional) used to gather platform-specific info
//                       during parameter marshalling process in order to 
//                       generate the appropriate call setup.
//    pCleanupWorkList - (optional) pointer to initialized
//                       CleanupWorkList. this pointer may be NULL if none
//                       of the opcodes in the MLCode stream uses it.
//
// Returns: a pointer to the first MLCode opcode _after_ the one
// that terminated execution.
//----------------------------------------------------------------------
const MLCode *
RunML(const MLCode    *       pMLCode,
      const VOID      *       pSource,
            VOID      *       pDestination,
            UINT8     * const pLocals,
      CleanupWorkList * const pCleanupWorkList
#if defined(TRACK_FLOATING_POINT_REGISTERS)
             , DOUBLE *       pFloatRegs  = NULL        // On WIN64 we use this as a pointer to FPSPILL_SLOT.
#endif // TRACK_FLOATING_POINT_REGISTERS
     );

UINT SizeOfML_OBJECT_C2N_SR();


#endif //__ML_H__
