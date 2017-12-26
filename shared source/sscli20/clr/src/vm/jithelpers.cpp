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

#include "common.h"
#include "jitinterface.h"
#include "codeman.h"
#include "method.hpp"
#include "class.h"
#include "object.h"
#include "field.h"
#include "stublink.h"
#include "virtualcallstub.h"
#include "corjit.h"
#include "eeconfig.h"
#include "comstring.h"
#include "excep.h"
#include "log.h"
#include "excep.h"
#include "float.h"      // for isnan
#include "dbginterface.h"
#include "gcscan.h"
#include "security.h"
#include "securitydescriptor.h"   // to get security method attribute
#include "securitymeta.h"
#include "dllimport.h"
#include "ml.h"
#include "gc.h"
#include "comdelegate.h"
#include "jitperf.h" // to track jit perf
#include "corprof.h"
#include "eeprofinterfaces.h"
#include "remoting.h" // create context bound and remote class instances
#include "perfcounters.h"
#ifdef PROFILING_SUPPORTED
#include "proftoeeinterfaceimpl.h"
#endif
#include "tls.h"
#include "ecall.h"
#include "comobject.h"
#include "comstringbuffer.h"
#include "generics.h"
#include "typestring.h"
#include "stackprobe.h"
#include "typedesc.h"
#include "genericdict.h"
#include "array.h"
#include "debuginfostore.h"
#include "constrainedexecutionregion.h"
#include "security.h"
#include "security.inl"
#include "tokeniter.hpp"
#include "safemath.h"


#include "mdaassistantsptr.h"

//========================================================================
//
// This file contains implementation of all JIT helpers. The helpers are
// divided into following categories:
//
//      INTEGER ARITHMETIC HELPERS
//      FLOATING POINT HELPERS
//      INSTANCE FIELD HELPERS
//      STATIC FIELD HELPERS
//      SHARED STATIC FIELD HELPERS
//      CASTING HELPERS
//      ALLOCATION HELPERS
//      STRING HELPERS
//      ARRAY HELPERS
//      VALUETYPE/BYREF HELPERS
//      GENERICS HELPERS
//      EXCEPTION HELPERS
//      SECURITY HELPERS
//      DEBUGGER/PROFILER HELPERS
//      GC HELPERS
//      INTEROP HELPERS
//
//========================================================================



//========================================================================
//
//      INTEGER ARITHMETIC HELPERS
//
//========================================================================

//
// helper macro to multiply two 32-bit uints
//
#define Mul32x32To64(a, b)  ((UINT64)((UINT32)(a)) * (UINT64)((UINT32)(b)))

//
// helper macro to get high 32-bit of 64-bit int
//
#define Hi32Bits(a)         ((UINT32)((UINT64)(a) >> 32))

//
// helper macro to check whether 64-bit signed int fits into 32-bit signed (compiles into one 32-bit compare)
//
#define Is32BitSigned(a)    (Hi32Bits(a) == Hi32Bits((INT64)(INT32)(a)))

//
// helper function to shift the result by 32-bits
//
inline UINT64 ShiftToHi32Bits(UINT32 x)
{

    ULARGE_INTEGER ret;
    ret.u.HighPart = x;
    ret.u.LowPart = 0;
    return ret.QuadPart;
}

/*********************************************************************/
HCIMPL2_VV(INT64, JIT_LMul, INT64 val1, INT64 val2)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    UINT32 val1High = Hi32Bits(val1);
    UINT32 val2High = Hi32Bits(val2);

    if ((val1High == 0) && (val2High == 0))
        return Mul32x32To64(val1, val2);

    return (val1 * val2);
}
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(INT64, JIT_LMulOvf, INT64 val1, INT64 val2)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    // This short-cut does not actually help since the multiplication 
    // of two 32-bit signed ints compiles into the call to a slow helper
    // if (Is32BitSigned(val1) && Is32BitSigned(val2))
    //     return (INT64)(INT32)val1 * (INT64)(INT32)val2;

    INDEBUG(INT64 expected = val1 * val2;)
    INT64 ret;

        // Remember the sign of the result
    INT32 sign = Hi32Bits(val1) ^ Hi32Bits(val2);

        // Convert to unsigned multiplication
    if (val1 < 0) val1 = -val1;
    if (val2 < 0) val2 = -val2;

        // Get the upper 32 bits of the numbers
    UINT32 val1High = Hi32Bits(val1);
    UINT32 val2High = Hi32Bits(val2);

    UINT64 valMid;

    if (val1High == 0) {
        // Compute the 'middle' bits of the long multiplication
        valMid = Mul32x32To64(val2High, val1);
    }
    else {
        if (val2High != 0)
            goto ThrowExcep;
        // Compute the 'middle' bits of the long multiplication
        valMid = Mul32x32To64(val1High, val2);
    }

        // See if any bits after bit 32 are set
    if (Hi32Bits(valMid) != 0)
        goto ThrowExcep;

    ret = Mul32x32To64(val1, val2) + ShiftToHi32Bits((UINT32)(valMid));

    // check for overflow
    if (Hi32Bits(ret) < (UINT32)valMid)
        goto ThrowExcep;

    if (sign >= 0) {
        // have we spilled into the sign bit?
        if (ret < 0)
            goto ThrowExcep;
    }
    else {
        ret = -ret;
        // have we spilled into the sign bit?
        if (ret > 0)
            goto ThrowExcep;
    }
    _ASSERTE(ret == expected);
    return ret;

ThrowExcep:
    FCThrow(kOverflowException);
}
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(UINT64, JIT_ULMulOvf, UINT64 val1, UINT64 val2)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    INDEBUG(UINT64 expected = val1 * val2;)
    UINT64 ret;

        // Get the upper 32 bits of the numbers
    UINT32 val1High = Hi32Bits(val1);
    UINT32 val2High = Hi32Bits(val2);

    UINT64 valMid;

    if (val1High == 0) {
        if (val2High == 0)
            return Mul32x32To64(val1, val2);
        // Compute the 'middle' bits of the long multiplication
        valMid = Mul32x32To64(val2High, val1);
    }
    else {
        if (val2High != 0)
            goto ThrowExcep;
        // Compute the 'middle' bits of the long multiplication
        valMid = Mul32x32To64(val1High, val2);
    }

        // See if any bits after bit 32 are set
    if (Hi32Bits(valMid) != 0)
        goto ThrowExcep;

    ret = Mul32x32To64(val1, val2) + ShiftToHi32Bits((UINT32)(valMid));

    // check for overflow
    if (Hi32Bits(ret) < (UINT32)valMid)
        goto ThrowExcep;
    
    _ASSERTE(ret == expected);
    return ret;

ThrowExcep:
        FCThrow(kOverflowException);
    }
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(INT64, JIT_LDiv, INT64 dividend, INT64 divisor)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    RuntimeExceptionKind ehKind;

    if (Is32BitSigned(divisor))
    {
        if ((INT32)divisor == 0)
        {
            ehKind = kDivideByZeroException;
            goto ThrowExcep;
        }

        if ((INT32)divisor == -1)
        {
            if ((UINT64) dividend == UI64(0x8000000000000000))
            {
                ehKind = kOverflowException;
                goto ThrowExcep;
            }
            return -dividend;
        }

        // Check for -ive or +ive numbers in the range -2**31 to 2**31
        if (Is32BitSigned(dividend))
            return((INT32)dividend / (INT32)divisor);
    }

    // For all other combinations fallback to int64 div.
    return(dividend / divisor);

ThrowExcep:
    FCThrow(ehKind);
}
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(INT64, JIT_LMod, INT64 dividend, INT64 divisor)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    RuntimeExceptionKind ehKind;

    if (Is32BitSigned(divisor))
    {
        if ((INT32)divisor == 0)
        {
            ehKind = kDivideByZeroException;
            goto ThrowExcep;
        }

        if ((INT32)divisor == -1)
        {
            if ((UINT64) dividend == UI64(0x8000000000000000))
            {
                ehKind = kOverflowException;
                goto ThrowExcep;
            }
            return 0;
        }

        // Check for -ive or +ive numbers in the range -2**31 to 2**31
        if (Is32BitSigned(dividend))
            return((INT32)dividend % (INT32)divisor);
    }

    // For all other combinations fallback to int64 div.
    return(dividend % divisor);

ThrowExcep:
    FCThrow(ehKind);
}
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(UINT64, JIT_ULDiv, UINT64 dividend, UINT64 divisor)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (Hi32Bits(divisor) == 0)
    {
        if ((UINT32)(divisor) == 0)
        FCThrow(kDivideByZeroException);

        if (Hi32Bits(dividend) == 0)
            return((UINT32)dividend / (UINT32)divisor);
    }

    return(dividend / divisor);
}
HCIMPLEND

/*********************************************************************/
HCIMPL2_VV(UINT64, JIT_ULMod, UINT64 dividend, UINT64 divisor)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (Hi32Bits(divisor) == 0)
    {
        if ((UINT32)(divisor) == 0)
        FCThrow(kDivideByZeroException);

        if (Hi32Bits(dividend) == 0)
            return((UINT32)dividend % (UINT32)divisor);
    }

    return(dividend % divisor);
}
HCIMPLEND


#if !defined(_X86_)

/*********************************************************************/
//llshl - long shift left
//
//Purpose:
//   Does a Long Shift Left (signed and unsigned are identical)
//   Shifts a long left any number of bits.
//
extern "C" void __stdcall JIT_LLsh(void)   // val = EDX:EAX  count = ECX
{
    LEAF_CONTRACT;

    PORTABILITY_ASSERT("JIT_LLsh");
}

/*********************************************************************/
//LRsh - long shift right
//
//Purpose:
//   Does a signed Long Shift Right
//   Shifts a long right any number of bits.
//
extern "C" void __stdcall JIT_LRsh(void)   // val = EDX:EAX  count = ECX
{
    LEAF_CONTRACT;

    PORTABILITY_ASSERT("JIT_LRsh");
}

/*********************************************************************/
// LRsz:
//Purpose:
//   Does a unsigned Long Shift Right
//   Shifts a long right any number of bits.
//
extern "C" void __stdcall JIT_LRsz(void)   // val = EDX:EAX  count = ECX
{
    LEAF_CONTRACT;

    PORTABILITY_ASSERT("JIT_LRsz");
}

#endif // !_X86_



//========================================================================
//
//      FLOATING POINT HELPERS
//
//========================================================================

HCIMPL2_VV(double, JIT_FloatDiv, double a, double b)
    //LEAF_CONTRACT;
    return a / b;
HCIMPLEND

HCIMPL2_VV(double, JIT_DoubleDiv, double a, double b)
    //LEAF_CONTRACT;
    return a / b;
HCIMPLEND

HCIMPL2_VV(double, JIT_FloatMul, double a, double b)
    //LEAF_CONTRACT;
    return a * b;
HCIMPLEND

/*********************************************************************/
//
HCIMPL1_V(double, JIT_ULng2Dbl, UINT64 val)
{
    STATIC_CONTRACT_SO_TOLERANT;
    LEAF_CONTRACT;

    double conv = (double) ((INT64) val);
    if (conv < 0)
        conv += (4294967296.0 * 4294967296.0);  // add 2^64
    _ASSERTE(conv >= 0);
    return(conv);
}
HCIMPLEND

/*********************************************************************/
// Call fast Dbl2Lng conversion - used by functions below
FORCEINLINE INT64 FastDbl2Lng(double val)
{
#ifdef _X86_
    WRAPPER_CONTRACT;
    return HCCALL1_V(JIT_Dbl2Lng, val);
#else
    LEAF_CONTRACT;
    return((__int64) val);
#endif
}

/*********************************************************************/
HCIMPL1_V(UINT32, JIT_Dbl2UIntOvf, double val)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

        // Note that this expression also works properly for val = NaN case
    if (val > -1.0 && val < 4294967296.0)
        return((UINT32)FastDbl2Lng(val));

    FCThrow(kOverflowException);
}
HCIMPLEND

/*********************************************************************/
HCIMPL1_V(UINT64, JIT_Dbl2ULng, double val)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    const double two63  = 2147483648.0 * 4294967296.0;
    UINT64 ret;
    if (val < two63) {
        ret = FastDbl2Lng(val);
    }
    else {        
        // subtract 0x8000000000000000, do the convert then add it back again
        ret = FastDbl2Lng(val - two63) + I64(0x8000000000000000);
}
    return ret;
}
HCIMPLEND

/*********************************************************************/
HCIMPL1_V(UINT64, JIT_Dbl2ULngOvf, double val)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    const double two64  = 4294967296.0 * 4294967296.0;
        // Note that this expression also works properly for val = NaN case
    if (val > -1.0 && val < two64) {
        const double two63  = 2147483648.0 * 4294967296.0;
        UINT64 ret;
        if (val < two63) {
            ret = FastDbl2Lng(val);
        }
        else {        
            // subtract 0x8000000000000000, do the convert then add it back again
            ret = FastDbl2Lng(val - two63) + I64(0x8000000000000000);
        }
#ifdef _DEBUG
        // since no overflow can occur, the value always has to be within 1
        double roundTripVal = HCCALL1_V(JIT_ULng2Dbl, ret);
        _ASSERTE(val - 1.0 <= roundTripVal && roundTripVal <= val + 1.0);
#endif // _DEBUG
        return ret;
    }

    FCThrow(kOverflowException);
}
HCIMPLEND



HCIMPL1_V(INT64, JIT_Dbl2Lng, double val)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    return((INT64)val);
}
HCIMPLEND

HCIMPL1_V(int, JIT_Dbl2IntOvf, double val)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    const double two31 = 2147483648.0;

        // Note that this expression also works properly for val = NaN case
    if (val > -two31 - 1 && val < two31)
        return((INT32)val);

    FCThrow(kOverflowException);
}
HCIMPLEND

HCIMPL1_V(INT64, JIT_Dbl2LngOvf, double val)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    const double two63  = 2147483648.0 * 4294967296.0;

        // Note that this expression also works properly for val = NaN case
        // We need to compare with the very next double to two63. 0x402 is epsilon to get us there.
    if (val > -two63 - 0x402 && val < two63)
        return((INT64)val);

    FCThrow(kOverflowException);
}
HCIMPLEND


float __stdcall JIT_FltRem(float divisor, float dividend)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    //
    // From the ECMA standard:
    //
    // If [divisor] is zero or [dividend] is infinity
    //   the result is NaN.
    // If [divisor] is infinity,
    //   the result is [dividend] (negated for –infinity***).
    //
    // ***"negated for -infinity" has been removed from the spec
    //

    if (divisor==0 || !_finite(dividend))
    {
        UINT32 NaN = CLR_NAN_32;
        return *(float *)(&NaN);
    }
    else if (!_finite(divisor) && !_isnan(divisor))
    {
        return dividend;
    }
    // else...
    return (float)fmod((double)dividend,(double)divisor);
}

double __stdcall JIT_DblRem(double divisor, double dividend)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    //
    // From the ECMA standard:
    //
    // If [divisor] is zero or [dividend] is infinity
    //   the result is NaN.
    // If [divisor] is infinity,
    //   the result is [dividend] (negated for –infinity***).
    //
    // ***"negated for -infinity" has been removed from the spec
    //
    if (divisor==0 || !_finite(dividend))
    {
        UINT64 NaN = CLR_NAN_64; 
        return *(double *)(&NaN);
    }
    else if (!_finite(divisor) && !_isnan(divisor))
    {
        return dividend;
    }
    // else...
    return(fmod(dividend,divisor));
}




//========================================================================
//
//      INSTANCE FIELD HELPERS
//
//========================================================================

/*********************************************************************/
// Returns the address of the field in the object (This is an interior
// pointer and the caller has to use it appropriately). obj can be
// either a reference or a byref
HCIMPL2(void*, JIT_GetFieldAddr, Object *obj, FieldDesc* pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
    } CONTRACTL_END;

    void * fldAddr = NULL;

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURN_INTERIOR, objRef);

    if (objRef == NULL)
        COMPlusThrow(kNullReferenceException);

    if(objRef->GetMethodTable()->IsTransparentProxyType())
    {
        objRef = CRemotingServices::GetObjectFromProxy(objRef, FALSE);
        if (objRef->GetMethodTable()->IsTransparentProxyType())
            COMPlusThrow(kInvalidOperationException, L"Remoting_InvalidValueTypeFieldAccess");
    }

    _ASSERTE(objRef->GetMethodTable()->IsMarshaledByRef() ||
              pFD->IsDangerousAppDomainAgileField() ||
              pFD->IsEnCNew());

    fldAddr = pFD->GetAddress(OBJECTREFToObject(objRef));

    HELPER_METHOD_FRAME_END();

    return fldAddr;
}
HCIMPLEND

/*********************************************************************/
HCIMPL2(__int32, JIT_GetField32, Object *obj, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
    } CONTRACTL_END;

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    if (obj == NULL)
        FCThrow(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    INT32 value = 0;

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        switch (pFD->GetFieldType())
        {
        case ELEMENT_TYPE_I1:
            value = (INT8)(pFD->GetValue8(objRef));
            break;
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_U1:
            value = (UINT8)(pFD->GetValue8(objRef));
            break;
        case ELEMENT_TYPE_I2:
            value = (INT16)(pFD->GetValue16(objRef));
            break;
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_U2:
            value = (UINT16)(pFD->GetValue16(objRef));
            break;
        case ELEMENT_TYPE_I4: // can fallthru
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_PTR:)
        IN_WIN32(case ELEMENT_TYPE_I:)
        IN_WIN32(case ELEMENT_TYPE_U:)
            value = pFD->GetValue32(objRef);
            break;

        default:
            _ASSERTE(!"Bad Type");
        }

        HELPER_METHOD_FRAME_END();                     // Tear down the frame
    }
    else
    {
        void* address = pFD->GetAddress(OBJECTREFToObject(objRef));

        switch (pFD->GetFieldType())
        {
        case ELEMENT_TYPE_I1:
            value = *(INT8*)address;
            break;
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_U1:
            value = *(UINT8*)address;
            break;
        case ELEMENT_TYPE_I2:
            value = *(INT16*)address;
            break;
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_U2:
            value = *(UINT16*)address;
            break;
        case ELEMENT_TYPE_I4: // can fallthru
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_PTR:)
        IN_WIN32(case ELEMENT_TYPE_I:)
        IN_WIN32(case ELEMENT_TYPE_U:)
            value = *(INT32*)address;
            break;

        default:
            _ASSERTE(!"Bad Type");
        }
    }

    FC_GC_POLL_RET();
    return value;
}
HCIMPLEND

/*********************************************************************/
HCIMPL2(__int64, JIT_GetField64, Object *obj, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrow(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);
    INT64 value = 0;

    // Check whether we are getting a field value on a proxy.
    // If so, then ask remoting services to extract the value from the 
    // instance.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        value = pFD->GetValue64(objRef);

        HELPER_METHOD_FRAME_END();              // Tear down the frame
    }
    else
    {
        value = *((__int64 *)pFD->GetAddress(OBJECTREFToObject(objRef)));
    }
    FC_GC_POLL_RET();
    return value;
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetField32, Object *obj, FieldDesc *pFD, __int32 value)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
    } CONTRACTL_END;

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        switch (pFD->GetFieldType())
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
            pFD->SetValue8(objRef, value);
            break;

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
            pFD->SetValue16(objRef, value);
            break;

        case ELEMENT_TYPE_I4: // can fallthru
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_PTR:)
        IN_WIN32(case ELEMENT_TYPE_I:)
        IN_WIN32(case ELEMENT_TYPE_U:)
            pFD->SetValue32(objRef, value);
            break;

        default:
            _ASSERTE(!"Bad Type");
        }

        HELPER_METHOD_FRAME_END();          // Tear down the frame
    }
    else
    {
        void* address = pFD->GetAddress(OBJECTREFToObject(objRef));

        switch (pFD->GetFieldType())
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
            *(INT8*)address = (INT8)value;
            break;

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
            *(INT16*)address = (INT16)value;
            break;

        case ELEMENT_TYPE_I4: // can fallthru
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_PTR:)
        IN_WIN32(case ELEMENT_TYPE_I:)
        IN_WIN32(case ELEMENT_TYPE_U:)
            *(INT32*)address = value;
            break;

        default:
            _ASSERTE(!"Bad Type");
        }
    }

    FC_GC_POLL();
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetField64, Object *obj, FieldDesc *pFD, __int64 value)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        pFD->SetValue64(objRef, value);

        HELPER_METHOD_FRAME_END();          // Tear down the frame
    }
    else
    {
        *((__int64 *)pFD->GetAddress(OBJECTREFToObject(objRef))) = value;
    }
    FC_GC_POLL();
}
HCIMPLEND


/*********************************************************************/
HCIMPL2(float, JIT_GetFieldFloat, Object *obj, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
        PRECONDITION(ELEMENT_TYPE_R4 == pFD->GetFieldType());  // Should only be used to get float fields.
    } CONTRACTL_END;

    float value  = 0;

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    if (obj == NULL)
        FCThrow(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        *(__int32*)&value = pFD->GetValue32(objRef);

        HELPER_METHOD_FRAME_END();                     // Tear down the frame
    }
    else
    {
        value = *(float*)pFD->GetAddress(OBJECTREFToObject(objRef));
    }

    FC_GC_POLL_RET();

    return value;
}
HCIMPLEND

/*********************************************************************/
HCIMPL2(double, JIT_GetFieldDouble, Object *obj, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
        PRECONDITION(ELEMENT_TYPE_R8 == pFD->GetFieldType());  // Should only be used to get double fields.
    } CONTRACTL_END;

    double value = 0.0;

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    if (obj == NULL)
        FCThrow(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so, then ask remoting services to extract the value from the 
    // instance.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        *(__int64*)&value = pFD->GetValue64(objRef);

        HELPER_METHOD_FRAME_END();              // Tear down the frame
    }
    else
    {
        value = *(double*)pFD->GetAddress(OBJECTREFToObject(objRef));
    }

    FC_GC_POLL_RET();

    return value;
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetFieldFloat, Object *obj, FieldDesc *pFD, float value)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
        PRECONDITION(ELEMENT_TYPE_R4 == pFD->GetFieldType());  // Should only be used to set float fields.
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame
        
        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);
        
        pFD->SetValue32(objRef, *(__int32*)&value);
        
        HELPER_METHOD_FRAME_END();          // Tear down the frame
    }
    else
    {
        *(float*)pFD->GetAddress(OBJECTREFToObject(objRef)) = value;
    }

    FC_GC_POLL();
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetFieldDouble, Object *obj, FieldDesc *pFD, double value)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsStatic());  // This is an instance field helper
        PRECONDITION(ELEMENT_TYPE_R8 == pFD->GetFieldType());  // Should only be used to set double fields.
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Check whether we are getting a field value on a proxy.
    // If so we will have to erect a frame.
    if(objRef->IsThunking())
    {
        // This could be a cross context field access. 
        HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame

        // Try an unwrap operation in case that we are not being called
        // in the same context as the server.
        // If that is the case then GetObjectFromProxy will return
        // the server object.
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

        pFD->SetValue64(objRef, *(__int64*)&value);

        HELPER_METHOD_FRAME_END();          // Tear down the frame
    }
    else
    {
        *(double*)pFD->GetAddress(OBJECTREFToObject(objRef)) = value;
    }
    FC_GC_POLL();
}
HCIMPLEND


/*********************************************************************/
HCIMPL2(Object*, JIT_GetFieldObj, Object *obj, FieldDesc *pFD)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    // Assert that we are called only for objects
    _ASSERTE(!pFD->IsPrimitive() && !pFD->IsByValue());

    if (obj == NULL)
        FCThrow(kNullReferenceException);

    OBJECTREF result = NULL;

#if CHECK_APP_DOMAIN_LEAKS
    if (pFD->IsFieldOfValueType())
    {
        // This case should only occur for dangerous fields
        _ASSERTE(pFD->IsDangerousAppDomainAgileField());
        result = *(OBJECTREF*)pFD->GetAddress((void*)obj);
    }
    else
#endif // CHECK_APP_DOMAIN_LEAKS
    {
        OBJECTREF objRef = ObjectToOBJECTREF(obj);

        // We should use this helper to get field values for marshalbyref types
        // or proxy types
        _ASSERTE(objRef->GetMethodTable()->IsMarshaledByRef() || objRef->IsThunking()
                 || pFD->IsDangerousAppDomainAgileField());

        // Check whether we are getting a field value on a proxy.
        // If so we will have to erect a frame
        if(objRef->IsThunking())
        {
            // This could be a cross context field access. 
            HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, objRef, result);    // Set up a frame
            
            // Try an unwrap operation in case that we are not being called
            // in the same context as the server.
            // If that is the case then GetObjectFromProxy will return
            // the server object.
            objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

            result = pFD->GetRefValue(objRef);

            HELPER_METHOD_FRAME_END();          // Tear down the frame
        }
        else
        {
            result = ObjectToOBJECTREF(*((Object**) pFD->GetAddress(OBJECTREFToObject(objRef))));
        }
    }

    FC_GC_POLL_AND_RETURN_OBJREF(OBJECTREFToObject(result));
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetFieldObj, Object *obj, FieldDesc *pFD, Object *value)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(!pFD->IsStatic());
        PRECONDITION(!pFD->IsPrimitive() && !pFD->IsByValue());  // Assert that we are called only for objects
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF valRef = ObjectToOBJECTREF(value);

#if CHECK_APP_DOMAIN_LEAKS
    if (pFD->IsFieldOfValueType())
    {
        BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
        // This case should only occur for dangerous fields
        _ASSERTE(pFD->IsDangerousAppDomainAgileField());
        SetObjectReference((OBJECTREF*) pFD->GetAddress((void*)obj),
                           valRef, GetAppDomain());
        END_SO_INTOLERANT_CODE;
    }
    else
#endif // CHECK_APP_DOMAIN_LEAKS
    {
        OBJECTREF objRef = ObjectToOBJECTREF(obj);

       // We should use this helper to get field values for marshalbyref types
        // or proxy types
        _ASSERTE(objRef->GetMethodTable()->IsMarshaledByRef() || objRef->IsThunking()
                 || pFD->IsDangerousAppDomainAgileField());

        // Check whether we are getting a field value on a proxy.
        // If so we will have to erect a frame.
        if(objRef->IsThunking())
        {
            // This could be a cross context field access. 
            HELPER_METHOD_FRAME_BEGIN_2(objRef, valRef);        // Set up a frame
            
            // Try an unwrap operation in case that we are not being called
            // in the same context as the server.
            // If that is the case then GetObjectFromProxy will return
            // the server object.
            objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);

            pFD->SetRefValue(objRef, valRef);

            HELPER_METHOD_FRAME_END();          // Tear down the frame
        }
        else
        {
            // This "violation" isn't a really a violation. SetRefValue() can trigger GC on
            // cross-context field access. But we specifically tested for this case above so
            // that we wouldn't take the overhead of setting up a helpermethodframe for
            // the common case where no context marshaling occurs. So this call is perfectly safe.
            // We just don't have a "correct" way to express it.
            CONTRACT_VIOLATION(GCViolation);
            pFD->SetRefValue(objRef, valRef);
        }
    }
    FC_GC_POLL();
}
HCIMPLEND


/*********************************************************************/
HCIMPL3(VOID, JIT_GetFieldStruct, LPVOID retBuff, Object *obj, FieldDesc *pFD)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);


    // This may be a  cross context field access. Setup a frame as we will
    // transition to managed code later
    //

    // This is an instance field helper
    _ASSERTE(!pFD->IsStatic());

    // Assert that we are not called for objects or primitive types
    _ASSERTE(!pFD->IsPrimitive());

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // We should only be using this helper to get field values for marshalbyref types
    // or proxy types
    _ASSERTE(objRef->GetMethodTable()->IsMarshaledByRef() || objRef->IsThunking());

    HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame

    // Try an unwrap operation in case that we are not being called
    // in the same context as the server.
    // If that is the case then GetObjectFromProxy will return
    // the server object.
    if(objRef->GetMethodTable()->IsTransparentProxyType())
    {
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);
    }

    CRemotingServices::FieldAccessor(pFD, objRef, retBuff, TRUE);

    HELPER_METHOD_FRAME_END();          // Tear down the frame
}
HCIMPLEND

/*********************************************************************/
HCIMPL3(VOID, JIT_SetFieldStruct, Object *obj, FieldDesc *pFD, LPVOID valuePtr)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    // Assert that we are not called for objects or primitive types
    _ASSERTE(!pFD->IsPrimitive());

    if (obj == NULL)
        FCThrowVoid(kNullReferenceException);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // We should use this helper to get field values for marshalbyref types
    // or proxy types
    _ASSERTE(objRef->GetMethodTable()->IsMarshaledByRef() || objRef->IsThunking()
             || pFD->IsDangerousAppDomainAgileField());

#ifdef CHECK_APP_DOMAIN_LEAKS
    if (pFD->IsDangerousAppDomainAgileField())
    {
        //
        // Verify that the object we are assigning to is also agile
        //

        if (objRef->IsAppDomainAgile())
        {
            // !!! validate that all dangerous fields of valuePtr are domain agile
        }
    }
#endif // CHECK_APP_DOMAIN_LEAKS


    // This may be a  cross context field access. Setup a frame as we will
    // transition to managed code later
    //

    HELPER_METHOD_FRAME_BEGIN_1(objRef);        // Set up a frame

    // Try an unwrap operation in case that we are not being called
    // in the same context as the server.
    // If that is the case then GetObjectFromProxy will return
    // the server object.
    if(objRef->GetMethodTable()->IsTransparentProxyType())
    {
        objRef = CRemotingServices::GetObjectFromProxy(objRef, TRUE);
    }

    CRemotingServices::FieldAccessor(pFD, objRef, valuePtr, FALSE);

    HELPER_METHOD_FRAME_END();          // Tear down the frame
}
HCIMPLEND



//========================================================================
//
//      STATIC FIELD HELPERS
//
//========================================================================

/*********************************************************************/
HCIMPL1(void*, JIT_GetStaticFieldAddr, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(pFD->IsThreadStatic() ||
                     pFD->IsContextStatic() ||
                     pFD->GetEnclosingMethodTable()->IsDomainNeutral() ||
                     pFD->GetEnclosingMethodTable()->IsDynamicStatics()
                     WIN64_ONLY(|| pFD->IsRVA() && pFD->GetModule()->IsRvaFieldTls(pFD->GetOffset())));
        PRECONDITION(pFD->IsStatic());
    } CONTRACTL_END;

    void *addr = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURN_INTERIOR);

    {
        MethodTable *pMT = pFD->GetEnclosingMethodTable();
        pMT->CheckRestore();

        pMT->CheckRunClassInitThrowing();

        if (pFD->IsContextStatic())
        {
            addr = Context::GetStaticFieldAddress(pFD);
        }
        else
        if(pFD->IsThreadStatic())
        {
            addr = Thread::GetStaticFieldAddress(pFD);
        }
        else
        {
            void *base = (void*) pFD->GetBase(false/*bRunCCtor*/);
            _ASSERTE(base != NULL);
            addr = pFD->GetStaticAddress(base);
        }
    }

    HELPER_METHOD_FRAME_END();

    return addr;
}
HCIMPLEND

// Slow helper to tailcall from the fast one
HCIMPL1(void, JIT_InitClass_Framed, MethodTable* pMT)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    // We don't want to be calling JIT_InitClass at all for perf reasons
    // on the Global Class <Module> as the Class loading logic ensures that we
    // already have initialized the Gloabl Class <Module>
    CONSISTENCY_CHECK(!pMT->IsGlobalClass());

    pMT->CheckRestore();
    pMT->CheckRunClassInitThrowing();

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND


/*************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL1(void, JIT_InitClass, CORINFO_CLASS_HANDLE typeHnd_)
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(typeHnd_);
    MethodTable *pMT = typeHnd.AsMethodTable();
    if (pMT->IsClassInited())
        return;

    // Tailcall to the slow helper
    ENDFORBIDGC();
    HCCALL1(JIT_InitClass_Framed, pMT);
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*************************************************************/
HCIMPL2(void, JIT_InitInstantiatedClass, CORINFO_CLASS_HANDLE typeHnd_, CORINFO_METHOD_HANDLE methHnd_)
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(typeHnd_ != NULL || methHnd_ != NULL);
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();    // Set up a frame

    MethodTable * pMT = (MethodTable*) typeHnd_;
    MethodDesc *  pMD = (MethodDesc*)  methHnd_;

    if (pMD != NULL)
    {
        if (pMD->GetClass()->IsSharedByGenericInstantiations())
        {
            pMT = ClassLoader::LoadGenericInstantiationThrowing(pMD->GetModule(),
                                                                pMD->GetClass()->GetCl(),
                                                                pMD->GetNumGenericClassArgs(),
                                                                pMD->GetExactClassInstantiation(pMT)).GetMethodTable();
        }
        else
            pMT = pMD->GetMethodTable();
    }

    pMT->CheckRestore();
    pMT->EnsureInstanceActive();
    pMT->CheckRunClassInitThrowing();
    HELPER_METHOD_FRAME_END();
HCIMPLEND

/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL1(void *, JIT_GetThreadFieldAddr_Primitive, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(pFD->IsStatic());
        PRECONDITION(pFD->IsThreadStatic());
        PRECONDITION(!pFD->IsSharedByGenericInstantiations());
        PRECONDITION(CorTypeInfo::IsPrimitiveType(pFD->GetFieldType()));
        PRECONDITION(pFD->GetOffset() <= FIELD_OFFSET_LAST_REAL_OFFSET);
    } CONTRACTL_END;

    // for static field the MethodTable is exact even for generic classes
    MethodTable* pMT = pFD->GetEnclosingMethodTable();

    Thread *pThread = NULL;
    STATIC_DATA *pData = NULL;
    DWORD dwClassOffset = 0;
    BYTE *dataBits = NULL;

    if (!pMT->IsRestored() || !pMT->IsClassInited())
        goto SLOW;

    pThread = GetThread();
    pData = pMT->IsDomainNeutral() ? pThread->GetSharedStaticData() : pThread->GetUnsharedStaticData();
    if (pData == 0)
        goto SLOW;

    // Note: returns -1 if the offset is not allocated yet
    dwClassOffset = pMT->GetThreadStaticsOffset();
    if (dwClassOffset >= pData->cElem)
        goto SLOW;

    dataBits = (BYTE*) pData->dataPtr[dwClassOffset];
    if (dataBits == 0)
        goto SLOW;

    return &dataBits[pFD->GetOffsetUnsafe()];

SLOW:
    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL1(JIT_GetStaticFieldAddr, pFD);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif


/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL1(void *, JIT_GetThreadFieldAddr_Objref, FieldDesc *pFD)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(pFD->IsStatic());
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(!pFD->IsSharedByGenericInstantiations());
        PRECONDITION(pFD->IsThreadStatic());
        PRECONDITION(CorTypeInfo::IsObjRef(pFD->GetFieldType()));
        PRECONDITION(pFD->GetOffset() <= FIELD_OFFSET_LAST_REAL_OFFSET);
    } CONTRACTL_END;

    // for static field the MethodTable is exact even for generic classes
    MethodTable* pMT = pFD->GetEnclosingMethodTable();

    Thread *pThread = NULL;
    STATIC_DATA *pData = NULL;
    DWORD dwClassOffset = 0;
    BYTE *dataBits = NULL;
    int slot = 0;

    if (!pMT->IsRestored() || !pMT->IsClassInited())
        goto SLOW;

    pThread = GetThread();
    pData = pMT->IsDomainNeutral() ? pThread->GetSharedStaticData() : pThread->GetUnsharedStaticData();
    if (pData == 0)
        goto SLOW;

    // Note: returns -1 if the offset is not allocated yet
    dwClassOffset = pMT->GetThreadStaticsOffset();
    if (dwClassOffset >= pData->cElem)
        goto SLOW;

    dataBits = (BYTE*) pData->dataPtr[dwClassOffset];
    if (dataBits == 0)
        goto SLOW;

    slot = *((int*) &dataBits[pFD->GetOffsetUnsafe()]);
    if (slot == 0)
        goto SLOW;

    return (void *)pThread->CalculateAddressForManagedStatic(slot);

SLOW:
    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL1(JIT_GetStaticFieldAddr, pFD);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif



//========================================================================
//
//      SHARED STATIC FIELD HELPERS
//
//========================================================================

HCIMPL2(void*, JIT_GetSharedNonGCStaticBase_Portable, SIZE_T moduleDomainID, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule = NULL;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    // If type doesn't have a class constructor, the contents of this if statement may 
    // still get executed.  JIT_GetSharedNonGCStaticBaseNoCtor should be used in this case.
    if (pLocalModule->IsPrecomputedClassInitialized(dwClassDomainID))
    {
        return (void*)pLocalModule->GetPrecomputedNonGCStaticsBasePointer();
    }

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL2(JIT_GetSharedNonGCStaticBase_Helper, pLocalModule, dwClassDomainID);
}
HCIMPLEND

// No constructor version of JIT_GetSharedNonGCStaticBase.  Does not check if class has 
// been initialized.
HCIMPL2(void*, JIT_GetSharedNonGCStaticBaseNoCtor_Portable, SIZE_T moduleDomainID, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule = NULL;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    return (void*)pLocalModule->GetPrecomputedNonGCStaticsBasePointer();
}
HCIMPLEND

HCIMPL2(void*, JIT_GetSharedGCStaticBase_Portable, SIZE_T moduleDomainID, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule = NULL;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    // If type doesn't have a class constructor, the contents of this if statement may 
    // still get executed.  JIT_GetSharedGCStaticBaseNoCtor should be used in this case.
    if (pLocalModule->IsPrecomputedClassInitialized(dwClassDomainID))
    {
        return (void*)pLocalModule->GetPrecomputedGCStaticsBasePointer();
    }
    
    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL2(JIT_GetSharedGCStaticBase_Helper, pLocalModule, dwClassDomainID);
}
HCIMPLEND

// No constructor version of JIT_GetSharedGCStaticBase.  Does not check if class has been
// initialized.
HCIMPL2(void*, JIT_GetSharedGCStaticBaseNoCtor_Portable, SIZE_T moduleDomainID, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule = NULL;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    return (void*)pLocalModule->GetPrecomputedGCStaticsBasePointer();
}
HCIMPLEND

// The following two functions can be tail called from platform dependent versions of 
// JIT_GetSharedGCStaticBase and JIT_GetShareNonGCStaticBase 
HCIMPL2(void*, JIT_GetSharedNonGCStaticBase_Helper, DomainLocalModule *pLocalModule, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    
    // Obtain Method table
    DWORD rid = (DWORD)(dwClassDomainID) + 1;
    TypeHandle th = pLocalModule->GetDomainFile()->GetModule()->LookupTypeDef(TokenFromRid(rid, mdtTypeDef));
    
    _ASSERTE(!th.IsNull());
    
    MethodTable *pMT = th.AsMethodTable();
    
    PREFIX_ASSUME(pMT != NULL);
    pMT->CheckRunClassInitThrowing();
    HELPER_METHOD_FRAME_END();
    
    return (void*)pLocalModule->GetPrecomputedNonGCStaticsBasePointer();
}
HCIMPLEND

HCIMPL2(void*, JIT_GetSharedGCStaticBase_Helper, DomainLocalModule *pLocalModule, DWORD dwClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURN_INTERIOR);
    
    // Obtain Method table
    DWORD rid = (DWORD)(dwClassDomainID) + 1;
    TypeHandle th = pLocalModule->GetDomainFile()->GetModule()->LookupTypeDef(TokenFromRid(rid, mdtTypeDef));
    
    _ASSERTE(!th.IsNull());
    
    MethodTable *pMT = th.AsMethodTable();
    
    PREFIX_ASSUME(pMT != NULL);
    pMT->CheckRunClassInitThrowing();
    HELPER_METHOD_FRAME_END();

    return (void*)pLocalModule->GetPrecomputedGCStaticsBasePointer();
}
HCIMPLEND

/*********************************************************************/
// Slow helper to tail call from the fast one
HCIMPL2(void*, JIT_GetSharedNonGCStaticBaseDynamicClass_Helper, DomainLocalModule *pLocalModule, DWORD dwDynamicClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    void* result = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    MethodTable *pMT = pLocalModule->GetDomainFile()->GetModule()->GetDynamicClassMT(dwDynamicClassDomainID);
    _ASSERTE(pMT);

    pMT->CheckRunClassInitThrowing();

    result = (void*)pLocalModule->GetDynamicEntryNonGCStaticsBasePointer(dwDynamicClassDomainID);
    HELPER_METHOD_FRAME_END();

    return result;
}
HCIMPLEND

/*************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL2(void*, JIT_GetSharedNonGCStaticBaseDynamicClass, SIZE_T moduleDomainID, DWORD dwDynamicClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    if (pLocalModule->IsDynamicClassInitialized(dwDynamicClassDomainID))
    {
        return (void*)pLocalModule->GetDynamicEntry(dwDynamicClassDomainID)->GetNonGCStaticsBasePointer();
    }

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL2(JIT_GetSharedNonGCStaticBaseDynamicClass_Helper, pLocalModule, dwDynamicClassDomainID);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*************************************************************/
// Slow helper to tail call from the fast one
HCIMPL2(void*, JIT_GetSharedGCStaticBaseDynamicClass_Helper, DomainLocalModule *pLocalModule, DWORD dwDynamicClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    void* result = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURN_INTERIOR);

    MethodTable *pMT = pLocalModule->GetDomainFile()->GetModule()->GetDynamicClassMT(dwDynamicClassDomainID);
    _ASSERTE(pMT);

    pMT->CheckRunClassInitThrowing();

    result = (void*)pLocalModule->GetDynamicEntryGCStaticsBasePointer(dwDynamicClassDomainID);
    HELPER_METHOD_FRAME_END();

    return result;
}
HCIMPLEND

/*************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL2(void*, JIT_GetSharedGCStaticBaseDynamicClass, SIZE_T moduleDomainID, DWORD dwDynamicClassDomainID)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    DomainLocalModule *pLocalModule;

    if (!DomainLocalBlock::IsModuleID(moduleDomainID))
        pLocalModule = (DomainLocalModule *) moduleDomainID;
    else
    {
        DomainLocalBlock *pLocalBlock = GetAppDomain()->GetDomainLocalBlock();
        pLocalModule = pLocalBlock->GetModuleSlot(moduleDomainID);
    }

    if (pLocalModule->IsDynamicClassInitialized(dwDynamicClassDomainID))
    {
        return (void*)pLocalModule->GetDynamicEntry(dwDynamicClassDomainID)->GetGCStaticsBasePointer();
    }

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL2(JIT_GetSharedGCStaticBaseDynamicClass_Helper, pLocalModule, dwDynamicClassDomainID);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*********************************************************************/
// Slow helper to tail call from the fast one
HCIMPL1(void*, JIT_GetGenericsGCStaticBase_Framed, MethodTable *pMT)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->HasGenericsStaticsInfo());
    } CONTRACTL_END;

    void* base = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURN_INTERIOR);

    pMT->CheckRestore();
    pMT->CheckRunClassInitThrowing();

    base = (void*) pMT->GetGCStaticsBasePointer();
    CONSISTENCY_CHECK(base != NULL);

    HELPER_METHOD_FRAME_END();

    return base;
}
HCIMPLEND

/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL1(void*, JIT_GetGenericsGCStaticBase, MethodTable *pMT)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->HasGenericsStaticsInfo());
    } CONTRACTL_END;

    GenericsStaticsInfo *pInfo = pMT->GetGenericsStaticsInfo();
    _ASSERTE(pInfo);

    DomainLocalModule *pLocalModule = pInfo->m_pModuleForStatics->GetDomainLocalModule();
    _ASSERTE(pLocalModule);

    DWORD id = (DWORD) pInfo->m_DynamicTypeID;
    void *base = (void*) pLocalModule->GetDynamicEntryGCStaticsBasePointerIfAllocatedAndInited(id);
    if (base != NULL)
        return base;

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL1(JIT_GetGenericsGCStaticBase_Framed, pMT);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*********************************************************************/
// Slow helper to tail call from the fast one
HCIMPL1(void*, JIT_GetGenericsNonGCStaticBase_Framed, MethodTable *pMT)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->HasGenericsStaticsInfo());
    } CONTRACTL_END;

    void* base = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    pMT->CheckRestore();
    pMT->CheckRunClassInitThrowing();

    base = (void*) pMT->GetNonGCStaticsBasePointer();

    HELPER_METHOD_FRAME_END();

    return base;
}
HCIMPLEND

/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL1(void*, JIT_GetGenericsNonGCStaticBase, MethodTable *pMT)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->HasGenericsStaticsInfo());
    } CONTRACTL_END;

    // This fast path will typically always be taken once the slow framed path below
    // has executed once.  Sometimes the slow path will be executed more than once,
    // e.g. if static fields are accessed during the call to CheckRunClassInitThrowing()
    // in the slow path.

    GenericsStaticsInfo *pInfo = pMT->GetGenericsStaticsInfo();
    _ASSERTE(pInfo);

    DomainLocalModule *pLocalModule = pInfo->m_pModuleForStatics->GetDomainLocalModule();
    _ASSERTE(pLocalModule);

    DWORD id = (DWORD) pInfo->m_DynamicTypeID;
    void *base = (void*) pLocalModule->GetDynamicEntryNonGCStaticsBasePointerIfAllocatedAndInited(id);
    if (base != NULL)
        return base;

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL1(JIT_GetGenericsNonGCStaticBase_Framed, pMT);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif


//========================================================================
//
//      CASTING HELPERS
//
//========================================================================

TypeHandle::CastResult ArrayIsInstanceOfNoGC(Object *pObject, TypeHandle toTypeHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pObject));
        PRECONDITION(pObject->GetMethodTable()->IsArray());
        PRECONDITION(toTypeHnd.IsArray());
    } CONTRACTL_END;

    ArrayBase *pArray = (ArrayBase*) pObject;
    ArrayTypeDesc *toArrayType = toTypeHnd.AsArray();

    // GetRank touches EEClass. Try to avoid it for SZArrays.
    if (toArrayType->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY)
    {
        if (pArray->GetMethodTable()->GetInternalCorElementType() != ELEMENT_TYPE_SZARRAY)
            return TypeHandle::CannotCast;
    }
    else
    {
        if (pArray->GetRank() != toArrayType->GetRank())
            return TypeHandle::CannotCast;
    }
    _ASSERTE(pArray->GetRank() == toArrayType->GetRank());

    // ArrayBase::GetTypeHandle consults the loader tables to find the
    // exact type handle for an array object.  This can be disproportionately slow - but after
    // all, why should we need to go looking up hash tables just to do a cast test?
    //
    // Thus we can always special-case the casting logic to avoid fetching this
    // exact type handle.  Here we have only done so for one
    // particular case, i.e. when we are trying to cast to an array type where
    // there is an exact match between the rank, kind and element type of the two
    // array types.  This happens when, for example, assigning an int32[] into an int32[][].
    //

    TypeHandle elementTypeHandle = pArray->GetArrayElementTypeHandle();
    TypeHandle toElementTypeHandle = toArrayType->GetArrayElementTypeHandle();

    if (elementTypeHandle == toElementTypeHandle)
        return TypeHandle::CanCast;

    ArrayTypeDesc arrayType(pArray->GetMethodTable(), elementTypeHandle);
    return arrayType.CanCastToNoGC(toTypeHnd);
}

TypeHandle::CastResult ObjIsInstanceOfNoGC(Object *pObject, TypeHandle toTypeHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pObject));
    } CONTRACTL_END;


    MethodTable *pMT = pObject->GetMethodTable();

    // Quick exact match first
    if (TypeHandle(pMT) == toTypeHnd)
        return TypeHandle::CanCast;

    if (pMT->IsThunking() ||
        pMT->IsComObjectType() && toTypeHnd.IsInterface())
        return TypeHandle::MaybeCast;

    if (pMT->IsArray())
    {
        if (toTypeHnd.IsArray())
            return ArrayIsInstanceOfNoGC(pObject, toTypeHnd);

        if (toTypeHnd.IsInterface())
            return TypeHandle::MaybeCast;

        if (toTypeHnd == TypeHandle(g_pObjectClass) || toTypeHnd == TypeHandle(g_pArrayClass))
            return TypeHandle::CanCast;

        return TypeHandle::CannotCast;
    }
    else if (Nullable::IsNullableForType(toTypeHnd, pMT))
    {
            // allow an object of type T to be cast to Nullable<T> (they have the same representation)
        return TypeHandle::CanCast;
    }

    TypeHandle ElemTypeHnd = TypeHandle(pMT);
    return ElemTypeHnd.CanCastToNoGC(toTypeHnd);
}

BOOL ObjIsInstanceOf(Object *pObject, TypeHandle toTypeHnd)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pObject));
    } CONTRACTL_END;

    BOOL fCast = FALSE;

    OBJECTREF obj = ObjectToOBJECTREF(pObject);

    GCPROTECT_BEGIN(obj);

    TypeHandle ElemTypeHnd = obj->GetTypeHandle();

    // If we are trying to store a proxy in the array we need to delegate to remoting
    // services which will determine whether the proxy and the array element
    // type are compatible.
    if(ElemTypeHnd.GetMethodTable()->IsThunking())
    {
        _ASSERTE(CRemotingServices::IsTransparentProxy(OBJECTREFToObject(obj)));
        fCast = CRemotingServices::CheckCast(obj, toTypeHnd);
    }
    else
    // Start by doing a quick static cast check to see if the type information captured in
    // the metadata indicates that the cast is legal.
    if (ElemTypeHnd.CanCastTo(toTypeHnd))
    {
        fCast = TRUE;
    }
    else
    if(ElemTypeHnd.GetMethodTable()->IsArray() && toTypeHnd.IsInterface())
    {
        fCast = ArraySupportsBizarreInterface(obj, toTypeHnd.GetMethodTable());
    }
    else if (Nullable::IsNullableForType(toTypeHnd, obj->GetMethodTable()))
    {
            // allow an object of type T to be cast to Nullable<T> (they have the same representation)
        fCast = TRUE;
    }

    GCPROTECT_END();

    return(fCast);
}

//
// This optimization is intended for all non-framed casting helpers
//

#ifdef _MSC_VER
#pragma optimize("",on)
#endif

HCIMPL2(Object*, JIT_ChkCastClass_Portable, MethodTable* pTargetMT, Object* pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    //
    // casts pObject to type pMT
    //

    if (NULL == pObject)
    {
        return NULL;
    }

    MethodTable* pMT = pObject->GetMethodTable();

    do {
        if (pMT == pTargetMT)
            return pObject;

        pMT = pMT->GetParentMethodTable();
    } while (pMT);

    ENDFORBIDGC();
    return HCCALL2(JITutil_ChkCastAny, CORINFO_CLASS_HANDLE(pTargetMT), pObject);
}
HCIMPLEND

//
// This helper assumes that the check for the trivial cases has been inlined by the JIT.
//
HCIMPL2(Object*, JIT_ChkCastClassSpecial_Portable, MethodTable* pTargetMT, Object* pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
        // This assumes that the check for the trivial cases has been inlined by the JIT.
        PRECONDITION(pObject != NULL);
        PRECONDITION(pObject->GetMethodTable() != pTargetMT);
    } CONTRACTL_END;

    MethodTable* pMT = pObject->GetMethodTable()->GetParentMethodTable();

    while (pMT)
    {
        if (pMT == pTargetMT)
            return pObject;

        pMT = pMT->GetParentMethodTable();
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_ChkCastAny, CORINFO_CLASS_HANDLE(pTargetMT), pObject);
}
HCIMPLEND

HCIMPL2(Object*, JIT_IsInstanceOfClass_Portable, MethodTable* pTargetMT, Object* pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    //
    // casts pObject to type pMT
    //

    if (NULL == pObject)
    {
        return NULL;
    }

    MethodTable* pMT = pObject->GetMethodTable();

    do {
        if (pMT == pTargetMT)
            return pObject;

        pMT = pMT->GetParentMethodTable();
    } while (pMT);

    if (!pObject->GetMethodTable()->IsThunking())
    {
        return NULL;
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_IsInstanceOfAny, CORINFO_CLASS_HANDLE(pTargetMT), pObject);
}
HCIMPLEND

HCIMPL2(Object*, JIT_ChkCastInterface_Portable, MethodTable *pInterfaceMT, Object* pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
        PRECONDITION(pInterfaceMT->IsInterface());
    } CONTRACTL_END;

    if (NULL == pObject)
    {
        return pObject;
    }

    if (pObject->GetMethodTable()->FindRestoredInterfaceInMap(pInterfaceMT))
    {
        return pObject;
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_ChkCastAny, CORINFO_CLASS_HANDLE(pInterfaceMT), pObject);
}
HCIMPLEND

HCIMPL2(Object*, JIT_IsInstanceOfInterface_Portable, MethodTable *pInterfaceMT, Object* pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
        PRECONDITION(pInterfaceMT->IsInterface());
    } CONTRACTL_END;

    if (NULL == pObject)
    {
        return NULL;
    }

    if (pObject->GetMethodTable()->FindRestoredInterfaceInMap(pInterfaceMT))
    {
        return pObject;
    }

    if (!pObject->GetMethodTable()->InstanceRequiresNonTrivialInterfaceCast())
    {
        return NULL;
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_IsInstanceOfAny, CORINFO_CLASS_HANDLE(pInterfaceMT), pObject);
}
HCIMPLEND

HCIMPL2(Object *, JIT_ChkCastArray, CORINFO_CLASS_HANDLE type, Object *pObject)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        MODE_COOPERATIVE;
        PRECONDITION(TypeHandle(type).IsArray());
    } CONTRACTL_END;

    if (pObject == NULL)
    {
        return NULL;
    }

    OBJECTREF refObj = ObjectToOBJECTREF(pObject);
    VALIDATEOBJECTREF(refObj);

    TypeHandle::CastResult result = refObj->GetMethodTable()->IsArray() ? 
        ArrayIsInstanceOfNoGC(pObject, TypeHandle(type)) : TypeHandle::CannotCast;

    if (result == TypeHandle::CanCast)
    {
        return pObject;
    }

    ENDFORBIDGC();
    Object* pRet = HCCALL2(JITutil_ChkCastAny, type, pObject);
    // Make sure that the fast helper have not lied
    _ASSERTE(result != TypeHandle::CannotCast);
    return pRet;
}
HCIMPLEND


HCIMPL2(Object *, JIT_IsInstanceOfArray, CORINFO_CLASS_HANDLE type, Object *pObject)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(TypeHandle(type).IsArray());
    } CONTRACTL_END;

    if (pObject == NULL)
    {
        return NULL;
    }

    OBJECTREF refObj = ObjectToOBJECTREF(pObject);
    VALIDATEOBJECTREF(refObj);
    MethodTable *pMT = refObj->GetMethodTable();

    if (!pMT->IsArray())
    {
        // We know that the clsHnd is an array so check the object.  If it is not an array return null
        return NULL;
    }
    else
    {
        switch (ArrayIsInstanceOfNoGC(pObject, TypeHandle(type))) {
        case TypeHandle::CanCast:
            return pObject;
        case TypeHandle::CannotCast:
            return NULL;
        default:
            // fall through to the slow helper
            break;
        }
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_IsInstanceOfAny, type, pObject);
}
HCIMPLEND

/*********************************************************************/
// IsInstanceOf test used for unusual cases (naked type parameters, variant generic types)
// Unlike the IsInstanceOfInterface, IsInstanceOfClass, and IsIsntanceofArray functions,
// this test must deal with all kinds of type tests
HCIMPL2(Object *, JIT_IsInstanceOfAny, CORINFO_CLASS_HANDLE type, Object* obj)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (NULL == obj)
    {
        return NULL;
    }

    switch (ObjIsInstanceOfNoGC(obj, TypeHandle(type))) {
    case TypeHandle::CanCast:
        return obj;
    case TypeHandle::CannotCast:
        return NULL;
    default:
        // fall through to the slow helper
        break;
    }

    ENDFORBIDGC();
    return HCCALL2(JITutil_IsInstanceOfAny, type, obj);
}
HCIMPLEND

// ChkCast test used for unusual cases (naked type parameters, variant generic types)
// Unlike the ChkCastInterface, ChkCastClass, and ChkCastArray functions,
// this test must deal with all kinds of type tests
HCIMPL2(Object *, JIT_ChkCastAny, CORINFO_CLASS_HANDLE type, Object *obj)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (NULL == obj)
    {
        return NULL;
    }

    TypeHandle::CastResult result = ObjIsInstanceOfNoGC(obj, TypeHandle(type));

    if (result == TypeHandle::CanCast)
    {
        return obj;
    }

    ENDFORBIDGC();
    Object* pRet = HCCALL2(JITutil_ChkCastAny, type, obj);
    // Make sure that the fast helper have not lied
    _ASSERTE(result != TypeHandle::CannotCast);
    return pRet;
}
HCIMPLEND

#ifdef _MSC_VER
#pragma optimize("",on)
#endif

//
// Framed helpers
//
HCIMPL2(Object *, JITutil_ChkCastAny, CORINFO_CLASS_HANDLE type, Object *obj)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    // This case should be handled by frameless helper
     _ASSERTE(obj != NULL);

    OBJECTREF oref = ObjectToOBJECTREF (obj);
    VALIDATEOBJECTREF(oref);

    TypeHandle clsHnd(type);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, oref);
    if (!ObjIsInstanceOf(OBJECTREFToObject(oref), clsHnd))
    {
        TypeHandle thCastFrom = oref->GetTypeHandle();
        COMPlusThrowInvalidCastException(thCastFrom, clsHnd);
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(oref);
}
HCIMPLEND

HCIMPL2(Object *, JITutil_IsInstanceOfAny, CORINFO_CLASS_HANDLE type, Object *obj)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    // This case should be handled by frameless helper
     _ASSERTE(obj != NULL);

    OBJECTREF oref = ObjectToOBJECTREF (obj);
    VALIDATEOBJECTREF(oref);

    TypeHandle clsHnd(type);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, oref);
    if (!ObjIsInstanceOf(OBJECTREFToObject(oref), clsHnd))
        oref = NULL;
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(oref);
}
HCIMPLEND



//========================================================================
//
//      ALLOCATION HELPERS
//
//========================================================================

/*************************************************************/
HCIMPL1(Object*, JIT_New, CORINFO_CLASS_HANDLE typeHnd_)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    TypeHandle typeHnd(typeHnd_);

    _ASSERTE(typeHnd.IsUnsharedMT());                                   // we never use this helper for arrays
    MethodTable *pMT = typeHnd.AsMethodTable();
    pMT->CheckRestore();

#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GetThread()->DisableStressHeap();
    }
#endif // _DEBUG

    newobj = AllocateObject(pMT);

    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND

/*************************************************************/
HCIMPL1(Object*, JIT_NewSpecial, CORINFO_CLASS_HANDLE typeHnd_)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    TypeHandle typeHnd(typeHnd_);

    _ASSERTE(typeHnd.IsUnsharedMT());                                   // we never use this helper for arrays
    MethodTable *pMT = typeHnd.AsMethodTable();
    pMT->CheckRestore();

#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GetThread()->DisableStressHeap();
    }
#endif // _DEBUG

    newobj = AllocateObjectSpecial(pMT);

    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND

/*************************************************************/
HCIMPL1(Object*, JIT_NewFast, CORINFO_CLASS_HANDLE typeHnd_)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    TypeHandle typeHnd(typeHnd_);

    _ASSERTE(typeHnd.IsUnsharedMT());                                   // we never use this helper for arrays
    MethodTable *pMT = typeHnd.AsMethodTable();

    // Don't bother to restore the method table; assume that the prestub of the
    // constructor will do that check.

#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GetThread()->DisableStressHeap();
    }
#endif // _DEBUG

    _ASSERTE(!(pMT->IsComObjectType()));

    newobj = FastAllocateObject(pMT);

    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND

/*************************************************************/
HCIMPL1(Object*, JIT_NewCrossContext_Portable, CORINFO_CLASS_HANDLE typeHnd_)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    TypeHandle typeHnd(typeHnd_);

    _ASSERTE(typeHnd.IsUnsharedMT());                                   // we never use this helper for arrays
    MethodTable *pMT = typeHnd.AsMethodTable();

    // Don't bother to restore the method table; assume that the prestub of the
    // constructor will do that check.

#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GetThread()->DisableStressHeap();
    }
#endif // _DEBUG

    if (CRemotingServices::RequiresManagedActivation(typeHnd))
    {
        if (pMT->IsComObjectType())
        {
            newobj = AllocateObjectSpecial(pMT);
        }
        else
        {
            // Remoting services determines if the current context is appropriate
            // for activation. If the current context is OK then it creates an object
            // else it creates a proxy.
            newobj = CRemotingServices::CreateProxyOrObject(pMT);
        }
    }
    else
    {
    newobj = FastAllocateObject(pMT);
    }

    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND



//========================================================================
//
//      STRING HELPERS
//
//========================================================================

/*********************************************************************/
/* We don't use HCIMPL macros because this is not a real helper call */
/* This function just needs mangled arguments like a helper call     */

HCIMPL1_RAW(StringObject*, UnframedAllocateString, DWORD stringLength)
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    STRINGREF result;

    result = SlowAllocateString(stringLength+1);
    result->SetStringLength(stringLength);
    return((StringObject*) OBJECTREFToObject(result));
HCIMPLEND_RAW

HCIMPL1(StringObject*, FramedAllocateString, DWORD stringLength)
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    STRINGREF result = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();    // Set up a frame

    result = SlowAllocateString(stringLength+1);
    result->SetStringLength(stringLength);

    HELPER_METHOD_FRAME_END();
    return((StringObject*) OBJECTREFToObject(result));
HCIMPLEND

/*********************************************************************/
OBJECTHANDLE ConstructStringLiteral(CORINFO_MODULE_HANDLE scopeHnd, mdToken metaTok)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    _ASSERTE(TypeFromToken(metaTok) == mdtString);

    Module* module = GetModule(scopeHnd);


    // If our module is ngenned and we're calling this API, it means that we're not going through
    // the fixup mechanism for strings. This can happen 2 ways:
    //
    //      a) Lazy string object construction: This happens when JIT decides that initizalizing a
    //         string via fixup on method entry is very expensive. This is normally done for strings
    //         that appear in rarely executed blocks, such as throw blocks.
    //
    //      b) The ngen image isn't complete (it's missing classes), therefore we're jitting methods.
    //
    //  If we went ahead and called ResolveStringRef directly, we would be breaking the per module
    //  interning we're guaranteeing, so we will have to detect the case and handle it appropiately.
    if (module->HasNativeImage() && module->IsNoStringInterning())
    {
        return module->ResolveStringRef(metaTok, module->GetAssembly()->Parent(), true);
    }

    return module->ResolveStringRef(metaTok, module->GetAssembly()->Parent(), false);
}

/*********************************************************************/
HCIMPL2(Object *, JIT_StrCns, unsigned metaTok, CORINFO_MODULE_HANDLE scopeHnd)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTHANDLE hndStr = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);

    // Retrieve the handle to the COM+ string object.
    hndStr = ConstructStringLiteral(scopeHnd, metaTok);
    HELPER_METHOD_FRAME_END();

    // Don't use ObjectFromHandle; this isn't a real handle
    return *(Object**)hndStr;
}
HCIMPLEND



//========================================================================
//
//      ARRAY HELPERS
//
//========================================================================

/*************************************************************/
HCIMPL2(Object*, JIT_NewArr1, CORINFO_CLASS_HANDLE arrayTypeHnd_, INT32 size)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newArray = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    TypeHandle typeHnd(arrayTypeHnd_);

    _ASSERTE(typeHnd.GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);
    typeHnd.CheckRestore();
    ArrayTypeDesc* pArrayClassRef = typeHnd.AsArray();

    if (size < 0)
        COMPlusThrow(kOverflowException);

    //
    // is this a primitive type?
    //

    CorElementType elemType = pArrayClassRef->GetArrayElementTypeHandle().GetSignatureCorElementType();

    if (CorTypeInfo::IsPrimitiveType(elemType)) {
#ifdef _DEBUG
        if (g_pConfig->FastGCStressLevel()) {
            GetThread()->DisableStressHeap();
        }
#endif // _DEBUG

        // Disallow the creation of void[] (an array of System.Void)
        if (elemType == ELEMENT_TYPE_VOID)
            COMPlusThrow(kArgumentException);

        BOOL bAllocateInLargeHeap = FALSE;
        if ((elemType == ELEMENT_TYPE_R8) && 
            (size >= int(g_pConfig->GetDoubleArrayToLargeObjectHeap())))
        {
            STRESS_LOG1(LF_GC, LL_INFO10, "Allocating double array of size %d to large object heap\n", size);
            bAllocateInLargeHeap = TRUE;
        }

        if (g_pPredefinedArrayTypes[elemType] == NULL)
            g_pPredefinedArrayTypes[elemType] = pArrayClassRef;

        newArray = FastAllocatePrimitiveArray(pArrayClassRef->GetMethodTable(), size, bAllocateInLargeHeap);
    }
    else
    {
        // call class init if necessary
        pArrayClassRef->GetMethodTable()->CheckRunClassInitThrowing();
#ifdef _DEBUG
        if (g_pConfig->FastGCStressLevel()) {
            GetThread()->DisableStressHeap();
        }
#endif // _DEBUG
        newArray = AllocateArrayEx(typeHnd, &size, 1);
    }


    HELPER_METHOD_FRAME_END();

    return(OBJECTREFToObject(newArray));
}
HCIMPLEND

/*********************************************************************
// Allocate a multi-dimensional array
*/

OBJECTREF allocNewMDArr(CORINFO_CLASS_HANDLE classHnd, unsigned dwNumArgs, va_list args)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(dwNumArgs > 0);
    } CONTRACTL_END;

    // Get the arguments in the right order

    INT32* fwdArgList;

#ifdef _X86_
    fwdArgList = (INT32*)args;

    // reverse the order
    INT32* p = fwdArgList;
    INT32* q = fwdArgList + (dwNumArgs-1);
    while (p < q)
    {
        INT32 t = *p; *p = *q; *q = t;
        p++; q--;
    }

#elif defined(_PPC_)
    fwdArgList = (INT32*) args;
#else
    // create an array where fwdArgList[0] == arg[0] ...
    fwdArgList = (INT32*) _alloca(dwNumArgs * sizeof(INT32));

    for (unsigned i = 0; i <dwNumArgs; i++)
    {
        fwdArgList[i] = va_arg(args, INT32);
    }
#endif

    TypeHandle typeHnd(classHnd);

    typeHnd.CheckRestore();
    // Should be using one of the fast new helpers, if you aren't an array
    _ASSERTE(typeHnd.GetMethodTable()->IsArray());

    OBJECTREF ret = AllocateArrayEx(typeHnd, fwdArgList, dwNumArgs);

    return ret;
}

/*********************************************************************
// Allocate a multi-dimensional array with lower bounds specified.
// The caller pushes both sizes AND/OR bounds for every dimension
*/

HCIMPL2VA(Object*, JIT_NewMDArr, CORINFO_CLASS_HANDLE classHnd,
                                 CORINFO_METHOD_HANDLE ctorMeth)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF    ret = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, ret);    // Set up a frame

    MethodDesc *pMethod = GetMethod(ctorMeth);
    Module *pModule = pMethod->GetModule();

    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;

    pMethod->GetSig(&pSig, &cbSigSize);

    MetaSig msig(pSig, cbSigSize, pModule, NULL);

    unsigned dwNumArgs = msig.NumFixedArgs();

    _ASSERTE(dwNumArgs > 0);

    _ASSERTE(!pMethod->IsStatic());
    // Should be using one of the fast new helpers, if you aren't an array
    _ASSERTE(pMethod->GetMethodTable()->IsArray());

    va_list dimsAndBounds;
    va_start(dimsAndBounds, ctorMeth);

    ret = allocNewMDArr(classHnd, dwNumArgs, dimsAndBounds);
    va_end(dimsAndBounds);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(ret);
}
HCIMPLEND

/*********************************************************************
// Allocate a multi-dimensional array without lower bounds specified.
// The caller pushes ONLY sizes for every dimension.
// This variant avoids touching the signature for the methods, and
// hence is more efficient.
*/

HCIMPL2VA(Object*, JIT_NewMDArrNoLBounds, CORINFO_CLASS_HANDLE classHnd,
                                          CORINFO_METHOD_HANDLE ctorMeth)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF    ret = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, ret);    // Set up a frame

    TypeHandle typeHnd(classHnd);
    // Should be using one of the fast new helpers, if you aren't an array
    typeHnd.CheckRestore();
    _ASSERTE(typeHnd.GetMethodTable()->IsArray());
    ArrayTypeDesc * arrayDesc = typeHnd.AsArray();

    va_list dims;
    va_start(dims, ctorMeth);

    ret = allocNewMDArr(classHnd, arrayDesc->GetRank(), dims);
    va_end(dims);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(ret);
}
HCIMPLEND

/*************************************************************/
/* returns '&array[idx], after doing all the proper checks */

#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL3(void*, JIT_Ldelema_Ref, PtrArray* array, unsigned idx, CORINFO_CLASS_HANDLE type)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    RuntimeExceptionKind except;
       // This has been carefully arranged to insure that in the common
        // case the branches are predicted properly (fall through).
        // and that we dont spill registers unnecessarily etc.
    if (array != 0)
        if (idx < array->GetNumComponents())
            if (array->GetArrayElementTypeHandle() == TypeHandle(type))
                return(&array->m_Array[idx]);
            else
                except = kArrayTypeMismatchException;
        else
            except = kIndexOutOfRangeException;
    else
        except = kNullReferenceException;

    FCThrow(except);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("", on )           /* put optimization back */
#endif

//===========================================================================
// This routine is called if the Array store needs a frame constructed
// in order to do the array check.  It should only be called from
// the array store check helpers.

HCIMPL2(FC_BOOL_RET, ArrayStoreCheck, Object** pElement, PtrArray** pArray)
{
    CONTRACTL
    {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Below
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BOOL ret = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_CAPTURE_DEPTH_2 | Frame::FRAME_ATTR_EXACT_DEPTH, *pElement, *pArray);

#ifdef STRESS_HEAP

    // Force a GC on every jit if the stress level is high enough
    if (g_pConfig->GetGCStressLevel() != 0
#ifdef _DEBUG
        && !g_pConfig->FastGCStressLevel()
#endif
        )
        GCHeap::GetGCHeap()->StressHeap();
#endif // STRESS_HEAP

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
      (*pElement)->AssignAppDomain((*pArray)->GetAppDomain());
#endif // CHECK_APP_DOMAIN_LEAKS

    ret = ObjIsInstanceOf(*pElement, (*pArray)->GetArrayElementTypeHandle());

    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(ret);
}
HCIMPLEND

/****************************************************************************/
/* assigns 'val to 'array[idx], after doing all the proper checks */

HCIMPL3(void, JIT_Stelem_Ref_Portable, PtrArray* array, unsigned idx, Object *val)
{
    if (!array) 
    {
        FCThrowVoid(kNullReferenceException);
    }
    if (idx >= array->GetNumComponents()) 
    {
        FCThrowVoid(kIndexOutOfRangeException);
    }

    if (val) 
    {
        MethodTable *valMT = val->GetMethodTable();
        TypeHandle arrayElemTH = array->GetArrayElementTypeHandle();

        if (arrayElemTH != TypeHandle(valMT) && arrayElemTH != TypeHandle(g_pObjectClass))
        {   
            TypeHandle::CastResult result = ObjIsInstanceOfNoGC(val, arrayElemTH);
            if (result != TypeHandle::CanCast)
            {
                HELPER_METHOD_FRAME_BEGIN_2(val, array);

                // This is equivalent to ArrayStoreCheck(&val, &array);
#ifdef STRESS_HEAP
                // Force a GC on every jit if the stress level is high enough
                if (g_pConfig->GetGCStressLevel() != 0
#ifdef _DEBUG
                    && !g_pConfig->FastGCStressLevel()
#endif
                   )
                    GCHeap::GetGCHeap()->StressHeap();
#endif

#if CHECK_APP_DOMAIN_LEAKS
                // If the instance is agile or check agile
                if (!arrayElemTH.IsAppDomainAgile() && !arrayElemTH.IsCheckAppDomainAgile() && g_pConfig->AppDomainLeaks())
                {
                    val->AssignAppDomain(array->GetAppDomain());
                }
#endif
                if (!ObjIsInstanceOf(val, arrayElemTH)) 
                {
                    COMPlusThrow(kArrayTypeMismatchException);
                }

                HELPER_METHOD_FRAME_END();
            }
        }

        // The performance gain of the optimized JIT_Stelem_Ref in
        // jitinterfacex86.cpp is mainly due to calling JIT_WriteBarrierReg_Buf.
        // By calling write barrier directly here,
        // we can avoid translating in-line assembly from MSVC to gcc
        // while keeping most of the performance gain.
        HCCALL2(JIT_WriteBarrier, (Object **)&array->m_Array[idx], val);

    }
    else
    {
        // no need to go through write-barrier for NULL
        ClearObjectReference(&array->m_Array[idx]);
    }
}
HCIMPLEND



//========================================================================
//
//      VALUETYPE/BYREF HELPERS
//
//========================================================================

/*************************************************************/
HCIMPL2(Object*, JIT_Box, CORINFO_CLASS_HANDLE type, void* unboxedData)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame
    GCPROTECT_BEGININTERIOR(unboxedData);
    HELPER_METHOD_POLL();

    TypeHandle clsHnd(type);

    _ASSERTE(clsHnd.IsUnsharedMT());  // we never use this helper for arrays
        // Sanity test for class
    _ASSERTE(clsHnd.GetClass()->GetMethodTable()->GetClass() == clsHnd.GetClass());
    MethodTable *pMT = clsHnd.AsMethodTable();

    pMT->CheckRestore();

    // You can only box things that inherit from valuetype or Enum.
    if (!pMT->IsValueType() && !pMT->IsEnum())
        COMPlusThrow(kInvalidCastException, L"Arg_ObjObj");

#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GetThread()->DisableStressHeap();
    }
#endif // _DEBUG

    newobj = pMT->FastBox(&unboxedData);

    GCPROTECT_END();
    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND

/*************************************************************/
HCIMPL2(LPVOID, JIT_Unbox_Nullable, CORINFO_CLASS_HANDLE type, Object* obj)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(type);
    _ASSERTE(Nullable::IsNullableType(typeHnd));

    MethodTable* typeMT = typeHnd.AsMethodTable();
    if (obj != NULL && TypeHandle(obj->GetMethodTable()) != typeMT->GetInstantiation()[0]) {
		// We allow unboxing true nullables to be robust to leaks of true boxed nullables.
		if (obj->GetMethodTable() == typeMT)
			return obj->GetData();
        FCThrow(kInvalidCastException);
	}

    LPVOID ret = 0;
    OBJECTREF objRef = ObjectToOBJECTREF(obj);
    HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);

    OBJECTREF bufferObj = typeMT->Allocate();
    // This is a true boxed nullable, but we don't ever give out the object itself
    ret = bufferObj->GetData(); 
    Nullable::UnBox(ret, objRef, typeMT);

    HELPER_METHOD_FRAME_END();
    return ret;
}
HCIMPLEND

/*************************************************************/
/* the uncommon case for the helper below (allowing enums to be unboxed
   as their underlying type */
LPVOID __fastcall JIT_Unbox_Helper(CORINFO_CLASS_HANDLE type, Object* obj)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(type);

    CorElementType type1 = typeHnd.GetInternalCorElementType();

        // we allow enums and their primtive type to be interchangable

    MethodTable* pMT2 = obj->GetMethodTable();
    CorElementType type2 = pMT2->GetInternalCorElementType();
    if (type1 == type2)
    {
        MethodTable* pMT1 = typeHnd.GetMethodTable();
        if (pMT1 && (pMT1->IsEnum() || pMT1->IsTruePrimitive()) &&
            (pMT2->IsEnum() || pMT2->IsTruePrimitive()))
        {
            _ASSERTE(CorTypeInfo::IsPrimitiveType(type1));
            return(obj->GetData());
        }
    }

    return(0);
}

/*************************************************************/
HCIMPL2(LPVOID, JIT_Unbox, CORINFO_CLASS_HANDLE type, Object* obj)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(type);
    VALIDATEOBJECTREF(obj);
    _ASSERTE(typeHnd.IsUnsharedMT());       // value classes are always unshared
    _ASSERTE(typeHnd.GetClass()->GetMethodTable()->GetClass() == typeHnd.GetClass());

        // This has been tuned so that branch predictions are good
        // (fall through for forward branches) for the common case
    RuntimeExceptionKind except;
    if (obj != 0) {
        if (obj->GetMethodTable() == typeHnd.AsMethodTable())
            return(obj->GetData());
        else {
                // Stuff the uncommon case into a helper so that
                // its register needs don't cause spills that effect
                // the common case above.
            LPVOID ret = JIT_Unbox_Helper(type, obj);
            if (ret != 0)
                return(ret);
        }
        except = kInvalidCastException;
    }
    else
        except = kNullReferenceException;

    FCThrow(except);
}
HCIMPLEND

/*************************************************************/
HCIMPL2_IV(LPVOID, JIT_GetRefAny, CORINFO_CLASS_HANDLE type, TypedByRef typedByRef)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle clsHnd(type);

    if (clsHnd != typedByRef.type) {
        FCThrow(kInvalidCastException);
    }

    return(typedByRef.data);
}
HCIMPLEND

/*************************************************************/
HCIMPL1(void*, JIT_SafeReturnableByref, void* byref)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        SO_TOLERANT;
    } CONTRACTL_END;

    _ASSERTE(!"Need to implement this");
    
    return byref;
}
HCIMPLEND



//========================================================================
//
//      GENERICS HELPERS
//
//========================================================================

/***********************************************************************/
// JIT_GenericHandle and its cache
//
// Perform a "polytypic" operation related to shared generic code at runtime,
// possibly filling in an entry in either a generic dictionary cache assocaited
// with a descriptor or placing an entry in the global JitGenericHandle cache.
//
// A polytypic operation is one such as
//      new List<T>
// or   castclass List<T>
// where the code being executed is shared generic code.  In these cases the outcome
// of the operation depends on the exact value for T, which is acquired from a dynamic
// parameter.
//
// The actual operation always boils down to finding a "handle" (TypeHandle, MethodDesc,
// call address, dispatch stub address etc.) based on some static information (passed as
// tokens) and on the exact runtime type context (passed as one or two parameters classHnd
// and methodHnd).
//
// The static information specifies which polytypic operation (and thus which kind of handle)
// we're interested in.
//
// The dynamic information (the type context, i.e. the exact instantiation of class
// and method type parameters is specified in one of two ways:
//  1. If classHnd is null then the methodHnd should
//     be an exact method descriptor wrapping shared code that satisfies
//     SharedByGenericMethodInstantiations().
//
//     For example:
//       1. We may be running the shared code for a generic method instantiation
//          C::m<object>.  The methodHnd will carry the exact instantiation, e.g. C::m<string>
//
//  2. If classHnd is non-null (e.g. a type D<exact>) then:
//        - methodHnd will indicate the representative code
//          being run (which will be !SharedByGenericMethodInstantiations
//          but will be SharedByGenericClassInstantiations).  Let's say this code is C<repr>::m().
//        - the type D will be a descendent of type C.  In particular D<exact> will
//          relate to some type C<exact'> where C<repr> is the represntative instantiation
//          of C<exact>'
//        - the relevant dictionary will be the one attached to C<exact'>.
//
// The JitGenericHandleCache is a global data structure shared across all application
// domains.  It is only used if generic dictionaries have overflowed.  It is flushed
// each time an application domain is unloaded.

struct JitGenericHandleCacheKey
{
    JitGenericHandleCacheKey(void *context, unsigned annotatedMetaTOK1, unsigned tok2, BaseDomain* pDomain=NULL)
    {
        LEAF_CONTRACT;        
        m_Data1 = (size_t)context;
        m_Data2 = (size_t)annotatedMetaTOK1;
        m_Data3 = (size_t)tok2;
        m_pDomainAndType = 0 | (size_t)pDomain;
    }

    JitGenericHandleCacheKey(MethodTable* pMT, CORINFO_CLASS_HANDLE classHnd, CORINFO_METHOD_HANDLE methodHnd, BaseDomain* pDomain=NULL)
    {
        LEAF_CONTRACT;
        m_Data1 = (size_t)pMT;
        m_Data2 = (size_t)classHnd;
        m_Data3 = (size_t)methodHnd;
        m_pDomainAndType = 1 | (size_t)pDomain;
    }

    size_t GetType() const
    {
        LEAF_CONTRACT;
        return (m_pDomainAndType & 1);
    }

    BaseDomain* GetDomain() const
    {
        LEAF_CONTRACT;
        return (BaseDomain*)(m_pDomainAndType & ~1);
    }

    size_t  m_Data1;
    size_t  m_Data2;
    size_t  m_Data3;

    size_t  m_pDomainAndType; // Which domain the entry belongs to. Not actually part of the key.
                        // Used only so we can scrape the table on AppDomain termination.
                        // NULL appdomain means that the entry should be scratched 
                        // on any appdomain unload.
                        //
                        // The lowest bit is used to indicate the type of the entry:
                        //  0 - JIT_GenericHandle entry
                        //  1 - JIT_VirtualFunctionPointer entry
};

class JitGenericHandleCacheTraits
{
public:
    static EEHashEntry_t *AllocateEntry(const JitGenericHandleCacheKey *pKey, BOOL bDeepCopy, AllocationHeap pHeap = 0)
    {
        LEAF_CONTRACT;
        EEHashEntry_t *pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(JitGenericHandleCacheKey)];
        if (!pEntry)
            return NULL;
        *((JitGenericHandleCacheKey*)pEntry->Key) = *pKey;
        return pEntry;
    }

    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap pHeap = 0)
    {
        LEAF_CONTRACT;
        delete [] (BYTE*)pEntry;
    }

    static BOOL CompareKeys(EEHashEntry_t *pEntry, const JitGenericHandleCacheKey *e2)
    {
        LEAF_CONTRACT;
        const JitGenericHandleCacheKey *e1 = (const JitGenericHandleCacheKey*)&pEntry->Key;
        return (e1->m_Data1 == e2->m_Data1) && (e1->m_Data2 == e2->m_Data2) && (e1->m_Data3 == e2->m_Data3) &&
            (e1->GetType() == e2->GetType()) &&
            // Any domain will work if the lookup key does not specify it
            ((e2->GetDomain() == NULL) || (e1->GetDomain() == e2->GetDomain()));
    }

    static DWORD Hash(const JitGenericHandleCacheKey *k)
    {
        LEAF_CONTRACT;
        return (DWORD)k->m_Data1 + _rotl((DWORD)k->m_Data2,5) + _rotr((DWORD)k->m_Data3,5);
    }

    static const JitGenericHandleCacheKey *GetKey(EEHashEntry_t *pEntry)
    {
        LEAF_CONTRACT;
        return (const JitGenericHandleCacheKey*)&pEntry->Key;
    }
};

typedef EEHashTable<const JitGenericHandleCacheKey *, JitGenericHandleCacheTraits, FALSE> JitGenericHandleCache;

JitGenericHandleCache *g_pJitGenericHandleCache = NULL;    //cache of calls to JIT_GenericHandle
CrstStatic g_pJitGenericHandleCacheCrst;

void AddToGenericHandleCache(JitGenericHandleCacheKey* pKey, HashDatum datum)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(CheckPointer(datum));
    } CONTRACTL_END;

    CrstHolder lock(&g_pJitGenericHandleCacheCrst);

    // Make sure the cache exists
    if (!g_pJitGenericHandleCache)
    {
        // Allocate and initialize the table
        // We need to initialize the Hash table before publishing it, otherwise a concurrent reading would have problem
        NewHolder <JitGenericHandleCache> tempGenericHandleCache (new JitGenericHandleCache());
        LockOwner sLock = {&g_pJitGenericHandleCacheCrst, IsOwnerOfCrst};
        if (!tempGenericHandleCache->Init(59, &sLock))
            COMPlusThrowOM();
        MemoryBarrier();
        g_pJitGenericHandleCache = tempGenericHandleCache.Extract();
    }

    // Write the entry into the cache (if we haven't been beaten to it).
    _ASSERTE(g_pJitGenericHandleCache);

    HashDatum entry;
    if (!g_pJitGenericHandleCache->GetValue(pKey,&entry))
        g_pJitGenericHandleCache->InsertValue(pKey,datum);
}

void AddToGenericHandleCacheNoThrow(JitGenericHandleCacheKey* pKey, HashDatum datum)
{
     CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(CheckPointer(datum));
    } CONTRACTL_END;

    EX_TRY
    {
        AddToGenericHandleCache(pKey, datum);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)  // Swallow any errors.
}

/* static */
void ClearJitGenericHandleCache(AppDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    // We call this on every AppDomain unload, because entries in the cache might include
    // pointers into the AppDomain being unloaded.  We would prefer to
    // only flush entries that have that are no longer valid, but the entries don't yet contain
    // enough information to do that.  However everything in the cache can be found again by calling
    // loader functions, and the total number of entries in the cache is typically very small (indeed
    // normally the cache is not used at all - it is only used when the generic dictionaries overflow).
    if (g_pJitGenericHandleCache)
    {
        // It's not necessary to take the lock here because this function should only be called when EE is suspended,
        // the lock is only taken to fullfill the threadsafety check and to be consistent. If the lock becomes a problem, we
        // could put it in a "ifdef _DEBUG" block
        CrstHolder lock(&g_pJitGenericHandleCacheCrst);
        EEHashTableIteration iter;
        g_pJitGenericHandleCache->IterateStart(&iter);
        BOOL keepGoing = g_pJitGenericHandleCache->IterateNext(&iter);
        while(keepGoing)
        {
            const JitGenericHandleCacheKey *key = g_pJitGenericHandleCache->IterateGetKey(&iter);
            BaseDomain* pKeyDomain = key->GetDomain();
            if (pKeyDomain == pDomain || pKeyDomain == NULL)
            {
                // Advance the iterator before we delete!!  See notes in EEHash.h
                keepGoing = g_pJitGenericHandleCache->IterateNext(&iter);
                g_pJitGenericHandleCache->DeleteValue(key);
            }
            else
            {
                keepGoing = g_pJitGenericHandleCache->IterateNext(&iter);
            }
        }
    }
}

// Factored out most of the body of JIT_GenericHandle so it could be called easily from the CER reliability code to pre-populate the
// cache.
CORINFO_GENERIC_HANDLE JIT_GenericHandleWorker(MethodDesc              *pMD,
                                               TypeHandle               declaringCls,
                                               unsigned                 annotatedMetaTOK1,
                                               unsigned                 token2,
                                               CORINFO_GENERIC_HANDLE  *slotPtr)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    // Method context (this is the code that's requesting the handle)
    SigTypeContext typeContext(pMD, declaringCls);

    _ASSERTE(!pMD->IsSharedByGenericMethodInstantiations());

    // This is where the token lives
    Module* module = pMD->GetModule();


    BaseDomain *pDictDomain = NULL;
    if (declaringCls.IsNull())
    {
        // We have an exact MD in the context.  This MD owns the dictionary.
        pDictDomain = pMD->GetDomain();
    }
    else
    {
        // We have a repr MD in the context, but it might still be an instantiated method
        // We have an exact TH, but it might be a subtype of the type that owns the dictionary.
        // However we know that type that owns the dictionary is declared in the current module,
        // and the computed typeContext gives us the instantiation of that type.

        pDictDomain = BaseDomain::ComputeBaseDomain(module->GetDomain(),
                                                    typeContext.m_classInstCount,
                                                    typeContext.m_classInst);
    }

    DictionaryEntryLayout slotLayout(annotatedMetaTOK1, token2);

    DictionaryEntry res = Dictionary::PopulateEntry(module,
                                                    &typeContext,
                                                    &slotLayout,
                                                    pDictDomain,
                                                    FALSE, /* allow types to be loaded */
                                                    (DictionaryEntry *) slotPtr );

    // If we've overflowed the dictionary write the result to the cache.
    // We allocate on the Windows alloc/free heap which isn't great but we need
    // to be able to free the memory when an AppDomain unloads.
    if (!slotPtr)
    {
        JitGenericHandleCacheKey key((declaringCls.IsNull() ? pMD : declaringCls.AsPtr()), 
            annotatedMetaTOK1, token2, pDictDomain);
        AddToGenericHandleCache(&key, (HashDatum)res);
    }

    return (CORINFO_GENERIC_HANDLE) res;
}

/*********************************************************************/
// slow helper to tail call from the fast one
HCIMPL5(CORINFO_GENERIC_HANDLE, JIT_GenericHandle_Framed,
         CORINFO_METHOD_HANDLE  methodHnd,
         unsigned annotatedMetaTOK1,
         unsigned token2,
         CORINFO_GENERIC_HANDLE* slotPtr,
         CORINFO_CLASS_HANDLE classHnd)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(classHnd, NULL_OK));
        PRECONDITION(TypeHandle(classHnd).IsNull() || TypeHandle(classHnd).IsRestored());
        PRECONDITION(CheckPointer(methodHnd));
        PRECONDITION(GetMethod(methodHnd)->IsRestored());
    } CONTRACTL_END;

    // Result is a generic handle (in fact, a CORINFO_CLASS_HANDLE, CORINFO_METHOD_HANDLE, or a code pointer)
    CORINFO_GENERIC_HANDLE result = NULL;

    MethodDesc* pMD = GetMethod(methodHnd);
    TypeHandle cls(classHnd);

    // Set up a frame
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    // We need to normalize the class passed in (if any) for reliability purposes. That's because preparation of a code region that
    // contains these handle lookups depends on being able to predict exactly which lookups are required (so we can pre-cache the
    // answers and remove any possibility of failure at runtime). This is hard to do if the lookup (in this case the lookup of the
    // dictionary overflow cache) is keyed off the somewhat arbitrary type of the instance on which the call is made (we'd need to
    // prepare for every possible derived type of the type containing the method). So instead we have to locate the exactly
    // instantiated (non-shared) super-type of the class passed in.
    TypeHandle declaringCls;
    if (!cls.IsNull())
    {
        TypeHandle *pInst = pMD->GetExactClassInstantiation(cls);
        MethodTable *pApproxDeclaringMT = pMD->GetMethodTable();
        declaringCls = ClassLoader::LoadGenericInstantiationThrowing(pApproxDeclaringMT->GetModule(),
                                                                     pApproxDeclaringMT->GetCl(),
                                                                     pApproxDeclaringMT->GetNumGenericArgs(),
                                                                     pInst);
    }

    // If we've overflowed the dictionary check the cache
    if (!slotPtr && g_pJitGenericHandleCache)
    {
        JitGenericHandleCacheKey key(declaringCls.AsPtr(), annotatedMetaTOK1, token2);
        HashDatum res;
        if (g_pJitGenericHandleCache->GetValue(&key,&res))
            result = (CORINFO_GENERIC_HANDLE) (DictionaryEntry) res;
    }

    // result is non-NULL only if we found a match in the cache using GetValue
    if (result == NULL)
    {
        result = JIT_GenericHandleWorker(pMD, declaringCls, annotatedMetaTOK1, token2, slotPtr);

        // If we've overflowed the dictionary add the denormalized key for faster lookup next time
        if (!slotPtr && cls != declaringCls)
        {
            // This is not a critical entry - no need to specify appdomain affinity
            JitGenericHandleCacheKey key(cls.AsPtr(), annotatedMetaTOK1, token2);
            AddToGenericHandleCacheNoThrow(&key, (HashDatum)result);
        }
    }

    HELPER_METHOD_FRAME_END();

    // Return the handle
    return result;
}
HCIMPLEND

/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL5(CORINFO_GENERIC_HANDLE, JIT_GenericHandle,
         CORINFO_METHOD_HANDLE  methodHnd,
         unsigned annotatedMetaTOK1,
         unsigned token2,
         CORINFO_GENERIC_HANDLE* slotPtr,
         CORINFO_CLASS_HANDLE classHnd)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(CheckPointer(classHnd, NULL_OK));
        PRECONDITION(TypeHandle(classHnd).IsNull() || TypeHandle(classHnd).IsRestored());
        PRECONDITION(CheckPointer(methodHnd));
        PRECONDITION(GetMethod(methodHnd)->IsRestored());
    } CONTRACTL_END;

    // Cheap test to see if slot is already filled in
    // This allows the caller to omit the null test itself
    // to give a saving in code size at the expense of a helper call every time
    if (slotPtr && *slotPtr)
        return *slotPtr;

    // If we've overflowed the dictionary check the cache
    if (!slotPtr && g_pJitGenericHandleCache)
    {
        MethodDesc* pMD = GetMethod(methodHnd);
        TypeHandle cls(classHnd);

        JitGenericHandleCacheKey key(cls.IsNull() ? pMD : cls.AsPtr(),
            annotatedMetaTOK1, token2);
        HashDatum res;
        if (g_pJitGenericHandleCache->GetValueSpeculative(&key,&res))
            return (CORINFO_GENERIC_HANDLE) (DictionaryEntry) res;
    }

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL5(JIT_GenericHandle_Framed, methodHnd, annotatedMetaTOK1, token2, slotPtr, classHnd);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*********************************************************************/
// Resolve a virtual method at run-time, either because of
// aggressive backpatching or because the call is to a generic
// method which is itself virtual.
//
// classHnd is the actual run-time type for the call is made.
// methodHnd is the exact (instantiated) method descriptor corresponding to the
// static method signature (i.e. might be for a superclass of classHnd)

// slow helper to tail call from the fast one
HCIMPL3(CORINFO_MethodPtr, JIT_VirtualFunctionPointer_Framed, Object * objectUNSAFE,
                                                       CORINFO_CLASS_HANDLE classHnd,
                                                       CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

        // The address of the method that's returned.
    CORINFO_MethodPtr   addr = NULL;

    OBJECTREF objRef = ObjectToOBJECTREF(objectUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);    // Set up a frame

    if (objRef == NULL)
        COMPlusThrow(kNullReferenceException);

    // This is the static method descriptor describing the call.
    // It is not the destination of the call, which we must compute.
    MethodDesc* pStaticMD = (MethodDesc*) methodHnd;
    TypeHandle staticTH(classHnd);

    pStaticMD->CheckRestore();

    // This is the new way of resolving a virtual call, including generic virtual methods.
    // The code is now also used by reflection, remoting etc.
    addr = (CORINFO_MethodPtr) pStaticMD->GetMultiCallableAddrOfVirtualizedCode(&objRef, staticTH);
    _ASSERTE(addr);

    // The cache can be used only if MethodTable is a real one
    if (!objRef->IsThunking())
    {
        // This is not a critical entry - no need to specify appdomain affinity
        JitGenericHandleCacheKey key(objRef->GetMethodTable(), classHnd, methodHnd);
        AddToGenericHandleCacheNoThrow(&key, (HashDatum)addr);
    }

    HELPER_METHOD_FRAME_END();

    return addr;
}
HCIMPLEND

/*********************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL3(CORINFO_MethodPtr, JIT_VirtualFunctionPointer, Object * objectUNSAFE,
                                                       CORINFO_CLASS_HANDLE classHnd,
                                                       CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    OBJECTREF objRef = ObjectToOBJECTREF(objectUNSAFE);

    if (objRef != NULL && g_pJitGenericHandleCache)
    {
        JitGenericHandleCacheKey key(objRef->GetMethodTable(), classHnd, methodHnd);
        HashDatum res;
        if (g_pJitGenericHandleCache->GetValueSpeculative(&key,&res))
            return (CORINFO_GENERIC_HANDLE)res;
    }

    // Tailcall to the slow helper
    ENDFORBIDGC();
    return HCCALL3(JIT_VirtualFunctionPointer_Framed, OBJECTREFToObject(objRef), classHnd, methodHnd);
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

// Helper for synchronized static methods in shared generics code
HCIMPL1(CORINFO_CLASS_HANDLE, JIT_GetClassFromMethodParam, CORINFO_METHOD_HANDLE methHnd_)
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(methHnd_ != NULL);
    } CONTRACTL_END;

    MethodDesc *  pMD = (MethodDesc*)  methHnd_;
    MethodTable * pMT = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();    // Set up a frame

    if (pMD->GetClass()->IsSharedByGenericInstantiations())
    {
        pMT = ClassLoader::LoadGenericInstantiationThrowing(pMD->GetModule(),
                                                            pMD->GetClass()->GetCl(),
                                                            pMD->GetNumGenericClassArgs(),
                                                            pMD->GetExactClassInstantiation(pMT)).GetMethodTable();
    }
    else
    {
        pMT = pMD->GetMethodTable();
    }

    pMT->CheckRestore();
    pMT->EnsureInstanceActive();
    HELPER_METHOD_FRAME_END();

    return((CORINFO_CLASS_HANDLE)pMT);
HCIMPLEND



//========================================================================
//
//      MONITOR HELPERS
//
//========================================================================

/*********************************************************************/
HCIMPL_MONHELPER(JIT_MonEnterWorker_Portable, Object* obj)
{
     CONTRACTL {
         SO_TOLERANT;
         THROWS;
         DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

#if !defined(_X86_) && !defined(_AMD64_)
    {
        class AwareLock *awareLock = NULL;
        SyncBlock* syncBlock = NULL;
        ObjHeader* objHeader = NULL;
        int spincount = 50;
        const int MaxSpinCount = 20000 * g_SystemInfo.dwNumberOfProcessors;
        LONG oldvalue, state;
        DWORD tid;

        Thread *pThread = GetThread();

        if (pThread->IsAbortRequested()) 
        {
            goto FramedLockHelper;
        }

        if (NULL == obj)
        {
            goto FramedLockHelper;
        }

        tid = pThread->GetThreadId();

        objHeader = obj->GetHeader();

        while (true)
        {
            oldvalue = objHeader->m_SyncBlockValue;
            
            if ((oldvalue & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + 
                            BIT_SBLK_SPIN_LOCK + 
                            SBLK_MASK_LOCK_THREADID + 
                            SBLK_MASK_LOCK_RECLEVEL)) == 0)
            {       
                
                if (tid > SBLK_MASK_LOCK_THREADID)
                {
                    goto FramedLockHelper;
                }
                
                LONG newvalue = oldvalue | tid;
                if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
                {
                    pThread->IncLockCount();
                    MONHELPER_STATE(if (pbLockTaken != NULL) *pbLockTaken = 1;)
                    return;
                }
                continue;
            }

            if (oldvalue & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
            {
                goto HaveHashOrSyncBlockIndex;
            }

            if (oldvalue & BIT_SBLK_SPIN_LOCK)
            {
                if (1 == g_SystemInfo.dwNumberOfProcessors)
                {
                    goto FramedLockHelper;
                }
            }
            else if (tid == (DWORD) (oldvalue & SBLK_MASK_LOCK_THREADID))
            {
                LONG newvalue = oldvalue + SBLK_LOCK_RECLEVEL_INC;
                
                if ((newvalue & SBLK_MASK_LOCK_RECLEVEL) == 0)
                {
                    goto FramedLockHelper;
                }

                if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
                {
                    MONHELPER_STATE(if (pbLockTaken != NULL) *pbLockTaken = 1;)
                    return;
                }
            }
            
            // exponential backoff
            for (int i = 0; i < spincount; i++)
            {
                YieldProcessor();
            }
            if (spincount > MaxSpinCount)
            {
                goto FramedLockHelper;
            }
            spincount *= 3;
        } /* while(true) */

    HaveHashOrSyncBlockIndex:
        if (oldvalue & BIT_SBLK_IS_HASHCODE)
        {
            goto FramedLockHelper;;
        }

        syncBlock = obj->PassiveGetSyncBlock();
        if (NULL == syncBlock)
        {
            goto FramedLockHelper;;
        }

        awareLock = syncBlock->QuickGetMonitor();
        state = awareLock->m_MonitorHeld;
        if (state == 0)
        {
            if (FastInterlockCompareExchangeAcquire((LONG*)&(awareLock->m_MonitorHeld), 1, 0) == 0)
            {
                syncBlock->SetAwareLock(pThread,1);
                pThread->IncLockCount();
                MONHELPER_STATE(if (pbLockTaken != NULL) *pbLockTaken = 1;)
                return;
            }
            else
            {
                goto FramedLockHelper;;
            }
        }
        else if (awareLock->GetOwningThread() == pThread) /* monitor is held, but it could be a recursive case */
        {
            awareLock->m_Recursion++;
            MONHELPER_STATE(if (pbLockTaken != NULL) *pbLockTaken = 1;)
            return;
        }
FramedLockHelper: ;
    }
#endif // !_X86_ && !_AMD64_

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // The following makes sure that Monitor.Enter shows up on thread abort
    // stack walks (otherwise Monitor.Enter called within a CER can block a
    // thread abort indefinitely). Setting the __me internal variable (normally
    // only set for fcalls) will cause the helper frame below to be able to
    // backtranslate into the method desc for the Monitor.Enter fcall.
    __me = GetEEFuncEntryPointMacro(JIT_MonEnter);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_1(Frame::FRAME_ATTR_EXACT_DEPTH, objRef);    

    if (objRef == NULL)
        COMPlusThrow(kArgumentNullException);

#ifdef _DEBUG
    Thread *pThread = GetThread();
    DWORD lockCount = pThread->m_dwLockCount;
#endif
    objRef->EnterObjMonitor();
    _ASSERTE ((objRef->GetSyncBlock()->GetMonitor()->m_Recursion == 1 && pThread->m_dwLockCount == lockCount + 1) ||
              pThread->m_dwLockCount == lockCount);
    MONHELPER_STATE(if (pbLockTaken != 0) *pbLockTaken = 1;)
    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL2(FC_BOOL_RET, JIT_MonTryEnter_Portable, Object* obj, INT32 timeOut)
{
     CONTRACTL {
         SO_TOLERANT;
         THROWS;
         DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

#if !defined(_X86_) && !defined(_AMD64_)
    {
        AwareLock* awareLock = NULL;
        SyncBlock* syncBlock = NULL;
        ObjHeader* objHeader = NULL;
        LONG state, oldvalue;
        DWORD tid; 
        int spincount = 50;
        const int MaxSpinCount = 20000 * g_SystemInfo.dwNumberOfProcessors;
       
        Thread *pThread = GetThread();

        if (pThread->IsAbortRequested()) 
        {
            goto FramedLockHelper;
        }

        if ((NULL == obj) || (timeOut < -1))
        {
            goto FramedLockHelper;
        }

        tid = pThread->GetThreadId();
        objHeader = obj->GetHeader();

        while (true)
        {
            oldvalue = objHeader->m_SyncBlockValue;
            
            if ((oldvalue & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + 
                            BIT_SBLK_SPIN_LOCK + 
                            SBLK_MASK_LOCK_THREADID + 
                            SBLK_MASK_LOCK_RECLEVEL)) ==0)
            {       
                
                if (tid > SBLK_MASK_LOCK_THREADID)
                {
                    goto FramedLockHelper;
                }
                
                LONG newvalue = oldvalue | tid;
                if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
                {
                    pThread->IncLockCount();
                    FC_RETURN_BOOL(TRUE);
                }
                continue;
            }

            if (oldvalue & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
            {
                goto HaveHashOrSyncBlockIndex;
            }

            if (oldvalue & BIT_SBLK_SPIN_LOCK)
            {
                if (1 == g_SystemInfo.dwNumberOfProcessors)
                {                
                    goto FramedLockHelper;
                }
            }
            else if (tid == (DWORD) (oldvalue & SBLK_MASK_LOCK_THREADID))
            {
                LONG newvalue = oldvalue + SBLK_LOCK_RECLEVEL_INC;
                
                if ((newvalue & SBLK_MASK_LOCK_RECLEVEL) == 0)
                {
                    goto FramedLockHelper;
                }
                
                if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
                {
                    FC_RETURN_BOOL(TRUE);
                }
            }
            else
            {
                // lock is held by someone else
                if (0 == timeOut)
                {
                    FC_RETURN_BOOL(FALSE);
                }
                else 
                {
                    goto FramedLockHelper;
                }
            }

            // exponential backoff
            for (int i = 0; i < spincount; i++)
            {
                YieldProcessor();
            }
            if (spincount > MaxSpinCount)
            {
                goto FramedLockHelper;
            }
            spincount *= 3;
        } /* while(true) */

    HaveHashOrSyncBlockIndex:
        if (oldvalue & BIT_SBLK_IS_HASHCODE)
        {
            goto FramedLockHelper;
        }

        syncBlock = obj->PassiveGetSyncBlock();
        if (NULL == syncBlock)
        {
            goto FramedLockHelper;
        }
        
        awareLock = syncBlock->QuickGetMonitor();
        state = awareLock->m_MonitorHeld;
        if (state == 0)
        {
            if (FastInterlockCompareExchangeAcquire((LONG*)&(awareLock->m_MonitorHeld), 1, 0) == 0)
            {
                syncBlock->SetAwareLock(pThread,1);
                pThread->IncLockCount();
                FC_RETURN_BOOL(TRUE);
            }
            else
            {
                goto FramedLockHelper;
            }
        }
        else if (awareLock->GetOwningThread() == pThread) /* monitor is held, but it could be a recursive case */
        {
            awareLock->m_Recursion++;
            FC_RETURN_BOOL(TRUE);
        }            
FramedLockHelper: ;
    }
#endif // !_X86_ && !_AMD64_

    BOOL result = FALSE;

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // The following makes sure that Monitor.TryEnter shows up on thread
    // abort stack walks (otherwise Monitor.TryEnter called within a CER can
    // block a thread abort for long periods of time). Setting the __me internal
    // variable (normally only set for fcalls) will cause the helper frame below
    // to be able to backtranslate into the method desc for the Monitor.TryEnter
    // fcall.
    __me = GetEEFuncEntryPointMacro(JIT_MonTryEnter);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_EXACT_DEPTH, objRef);

    if (objRef == NULL)
        COMPlusThrow(kArgumentNullException);

    if (timeOut < -1)
        COMPlusThrow(kArgumentException);

    result = objRef->TryEnterObjMonitor(timeOut);

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(result);
}
HCIMPLEND

/*********************************************************************/
HCIMPL_MONHELPER(JIT_MonExitWorker_Portable, Object* obj)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    MONHELPER_STATE(if (pbLockTaken != 0 && *pbLockTaken == 0) return;)

    AwareLock::LeaveHelperAction action = AwareLock::LeaveHelperAction_None;

    if (obj != NULL)
    {
        action = obj->LeaveObjMonitorHelper(GetThread());

        // Handle the simple case without erecting helper frame
        if (action == AwareLock::LeaveHelperAction_None)
        {
            MONHELPER_STATE(if (pbLockTaken != 0) *pbLockTaken = 0;)
            return;
        }
    }
  
    ThreadPreventAbortHolder preventAbort(TRUE);

    OBJECTREF objRef = ObjectToOBJECTREF(obj);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_1(Frame::FRAME_ATTR_EXACT_DEPTH, objRef);    

    if (action == AwareLock::LeaveHelperAction_Signal)
    {
        // Signal the event
        SyncBlock *psb = objRef->PassiveGetSyncBlock();
        if (psb != NULL)
            psb->QuickGetMonitor()->Signal();
    }
    else
    {
        // Error, yield or contention
        if (objRef == NULL)
            COMPlusThrow(kArgumentNullException);

        if (!objRef->LeaveObjMonitor())
            COMPlusThrow(kSynchronizationLockException);
    }

    preventAbort.Release();
    MONHELPER_STATE(if (pbLockTaken != 0) *pbLockTaken = 0;)

    Thread* pThread = GetThread();
    TESTHOOKCALL(AppDomainCanBeUnloaded(pThread->GetDomain()->GetId().m_dwId,FALSE));
    
    if (pThread->IsAbortRequested()) {
        pThread->HandleThreadAbort();
    }

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL1(void, JIT_MonEnterStatic_Portable, AwareLock *lock)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    _ASSERTE(lock);
    Thread *pCurThread = GetThread();

    for (;;)
    {
        // Read existing lock state.
        LONG state = lock->m_MonitorHeld;

        if (state == 0)
        {
            // Common case: lock not held, no waiters. Attempt to acquire lock by
            // switching lock bit.
            if (FastInterlockCompareExchangeAcquire((LONG*)&(lock->m_MonitorHeld), 1, 0) == 0)
            {
                // We get here if we successfully acquired the mutex.
                lock->m_HoldingThread = pCurThread;
                lock->m_Recursion = 1;
                pCurThread->IncLockCount();
                break;
            }
        }
        else
        {
            // It's possible to get here with waiters but no lock held, but in this
            // case a signal is about to be fired which will wake up a waiter. So
            // for fairness sake we should wait too.
            // Check first for recursive lock attempts on the same thread.
            if (lock->m_HoldingThread == pCurThread)
            {
                // Got the mutex via recursive locking on the same thread.
                _ASSERTE(lock->m_Recursion >= 1);
                lock->m_Recursion++;
                break;
            }

            // Attempt to increment this count of waiters then goto contention
            // handling code.
            if (FastInterlockCompareExchangeAcquire((LONG*)&(lock->m_MonitorHeld), state + 2, state) == state)
            {
                goto MustWait;
            }
        }
    } 

#if defined(_DEBUG) && defined(TRACK_SYNC)
    {
        // The best place to grab this is from the ECall frame
        Frame * pFrame = pCurThread->GetFrame();
        int     caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
        pCurThread->m_pTrackSync->EnterSync(caller, lock);
    }
#endif
    return;

MustWait:
    // The following makes sure that Monitor.Enter shows up on thread abort
    // stack walks (otherwise Monitor.Enter called within a CER can block a
    // thread abort indefinitely). Setting the __me internal variable (normally
    // only set for fcalls) will cause the helper frame below to be able to
    // backtranslate into the method desc for the Monitor.Enter fcall.
    __me = GetEEFuncEntryPointMacro(JIT_MonEnter);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXACT_DEPTH);
    lock->EnterEpilog(pCurThread);
    HELPER_METHOD_FRAME_END_POLL();
    return;
}
HCIMPLEND

/*********************************************************************/
HCIMPL1(void, JIT_MonExitStatic_Portable, AwareLock *lock)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    _ASSERTE(lock);

    AwareLock::LeaveHelperAction action = lock->LeaveHelper(GetThread());

    // Handle the simple case without erecting helper frame
    if (action == AwareLock::LeaveHelperAction_None)
    {
        return;
    }

    ThreadPreventAbortHolder preventAbort(TRUE);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_ATTRIB(Frame::FRAME_ATTR_EXACT_DEPTH);

    if (action == AwareLock::LeaveHelperAction_Signal)
    {
        // Signal the event
        lock->Signal();
    }
    else
    {
        // Error, yield or contention
        if (!lock->Leave())
            COMPlusThrow(kSynchronizationLockException);
    }

    preventAbort.Release();

    Thread* pThread = GetThread();
    TESTHOOKCALL(AppDomainCanBeUnloaded(pThread->GetDomain()->GetId().m_dwId,FALSE));
    if (pThread->IsAbortRequested()) {
        pThread->HandleThreadAbort();
    }

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
// A helper for JIT_MonEnter that is on the callee side of an ecall
// frame and handles the contention case.

HCIMPL_MONHELPER(JITutil_MonContention, AwareLock* lock)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    // The following makes sure that Monitor.Enter shows up on thread abort
    // stack walks (otherwise Monitor.Enter called within a CER can block a
    // thread abort indefinitely). Setting the __me internal variable (normally
    // only set for fcalls) will cause the helper frame below to be able to
    // backtranslate into the method desc for the Monitor.Enter fcall.
    __me = GetEEFuncEntryPointMacro(JIT_MonEnter);

    // Monitor helpers are used as both hcalls and fcalls, thus we need exact depth.
    HELPER_METHOD_FRAME_BEGIN_ATTRIB(Frame::FRAME_ATTR_EXACT_DEPTH);    

#ifdef _DEBUG
    Thread *pThread = GetThread();
    DWORD lockCount = pThread->m_dwLockCount;
#endif
    lock->Contention();
    _ASSERTE (pThread->m_dwLockCount == lockCount + 1);
    MONHELPER_STATE(if (pbLockTaken != 0) *pbLockTaken = 1;)

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
// A helper for JIT_MonExit and JIT_MonExitStatic that is on the
// callee side of an ecall frame and handles cases that might allocate,
// throw or block.
HCIMPL_MONHELPER(JITutil_MonSignal, AwareLock* lock)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    ThreadPreventAbortHolder preventAbort(TRUE);

    MONHELPER_STATE(if (pbLockTaken != 0 && *pbLockTaken == 0) return;)

    HELPER_METHOD_FRAME_BEGIN_0();

    lock->Signal();
    preventAbort.Release();
    MONHELPER_STATE(if (pbLockTaken != 0) *pbLockTaken = 0;)

    Thread *pThread = GetThread();
    TESTHOOKCALL(AppDomainCanBeUnloaded(pThread->GetDomain()->GetId().m_dwId,FALSE));
    
    if (pThread->IsAbortRequested()) {
        pThread->HandleThreadAbort();
    }

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

HCIMPL1(void *, JIT_GetSyncFromClassHandle, CORINFO_CLASS_HANDLE typeHnd_)
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        PRECONDITION(typeHnd_ != NULL);
    } CONTRACTL_END;

    void * result = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();    // Set up a frame

    TypeHandle typeHnd(typeHnd_);
    MethodTable *pMT = typeHnd.AsMethodTable();

    OBJECTREF ref = pMT->GetManagedClassObject();
    _ASSERTE(ref);

    result = (void*)ref->GetSyncBlock()->GetMonitor();

    HELPER_METHOD_FRAME_END();
    
    return(result);
    
HCIMPLEND


//========================================================================
//
//      EXCEPTION HELPERS
//
//========================================================================

// In general, we want to use COMPlusThrow to throw exceptions.  However,
// the JIT_Throw helper is a special case.  Here, we're called from
// managed code.  We have a guarantee that the first FS:0 handler
// is our COMPlusFrameHandler.  We could call COMPlusThrow(), which pushes
// another handler, but there is a significant (10% on JGFExceptionBench)
// performance gain if we avoid this by calling RaiseTheException()
// directly.
//

/*************************************************************/

HCIMPL1(void, JIT_Throw,  Object* obj)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build
    (repro: jit\Regression\CLR-x86-JIT\V1-M09\b14079\b14079.exe)

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/
    // This "violation" isn't a really a violation. 
    // We are calling a assembly helper that can't have a Global State contract
    CONTRACT_VIOLATION(GlobalStateViolation);
    /* Make no assumptions about the current machine state */
    ResetCurrentContext();

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    OBJECTREF oref = ObjectToOBJECTREF(obj);

#if defined(_DEBUG) && defined(_X86_)
    __helperframe.InsureInit(false, NULL);
    g_ExceptionEIP = (LPVOID)__helperframe.GetReturnAddress();
#endif // defined(_DEBUG) && defined(_X86_)


    if (oref == 0)
        COMPlusThrow(kNullReferenceException);
    else
    if (!IsException(oref->GetMethodTable()))
    {
        GCPROTECT_BEGIN(oref);

        WrapNonCompliantException(&oref);

        GCPROTECT_END();
    }
    else
    {   // We know that the object derives from System.Exception
        if (g_CLRPolicyRequested &&
            oref->GetMethodTable() == g_pOutOfMemoryExceptionClass)
        {
            EEPolicy::HandleOutOfMemory();
        }

        ((EXCEPTIONREF)oref)->ClearStackTraceForThrow();
    }
    RaiseTheExceptionInternalOnly(oref, FALSE);

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*************************************************************/

HCIMPL0(void, JIT_Rethrow)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    OBJECTREF throwable = GetThread()->GetThrowable();
    if (throwable != NULL)
    {
        if (g_CLRPolicyRequested &&
            throwable->GetMethodTable() == g_pOutOfMemoryExceptionClass)
        {
            EEPolicy::HandleOutOfMemory();
        }

        RaiseTheExceptionInternalOnly(throwable, TRUE);
    }
    else
    {
        // This can only be the result of bad IL (or some internal EE failure).
        _ASSERTE(!"No throwable on rethrow");
        RealCOMPlusThrow(kInvalidProgramException, (UINT)IDS_EE_RETHROW_NOT_ALLOWED);
    }

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL0(void, JIT_RngChkFail)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build
    (repro: jit\Regression\CLR-x86-JIT\V1-M09\b14079\b14079.exe)

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    /* Make no assumptions about the current machine state */
    ResetCurrentContext();

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    COMPlusThrow(kIndexOutOfRangeException);

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL0(void, JIT_Overflow)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    /* Make no assumptions about the current machine state */
    ResetCurrentContext();

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    COMPlusThrow(kOverflowException);

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL1(void, JIT_Verification,  int ilOffset)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    COMPlusThrow(kVerificationException);

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL1(void, JIT_SecurityUnmanagedCodeException, CORINFO_CLASS_HANDLE typeHnd_)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXCEPTION);    // Set up a frame

    Security::ThrowSecurityException(g_SecurityPermissionClassName, SPFLAGSUNMANAGEDCODE);

    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

static const RuntimeExceptionKind map[CORINFO_Exception_Count] =
{
    kNullReferenceException,
    kDivideByZeroException,
    kInvalidCastException,
    kIndexOutOfRangeException,
    kOverflowException,
    kSynchronizationLockException,
    kArrayTypeMismatchException,
    kRankException,
    kArgumentNullException,
    kArgumentException,
};

/*********************************************************************/
HCIMPL1(void, JIT_InternalThrow, unsigned exceptNum)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

        // spot check of the array above
    _ASSERTE(map[CORINFO_NullReferenceException] == kNullReferenceException);
    _ASSERTE(map[CORINFO_DivideByZeroException] == kDivideByZeroException);
    _ASSERTE(map[CORINFO_IndexOutOfRangeException] == kIndexOutOfRangeException);
    _ASSERTE(map[CORINFO_OverflowException] == kOverflowException);
    _ASSERTE(map[CORINFO_SynchronizationLockException] == kSynchronizationLockException);
    _ASSERTE(map[CORINFO_ArrayTypeMismatchException] == kArrayTypeMismatchException);
    _ASSERTE(map[CORINFO_RankException] == kRankException);
    _ASSERTE(map[CORINFO_ArgumentNullException] == kArgumentNullException);
    _ASSERTE(map[CORINFO_ArgumentException] == kArgumentException);

    PREFIX_ASSUME(exceptNum < CORINFO_Exception_Count);
    _ASSERTE(map[exceptNum] != 0);

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_EXACT_DEPTH);
    COMPlusThrow(map[exceptNum]);
    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
HCIMPL1(void*, JIT_InternalThrowFromHelper, unsigned exceptNum)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

        // spot check of the array above
    _ASSERTE(map[CORINFO_NullReferenceException] == kNullReferenceException);
    _ASSERTE(map[CORINFO_IndexOutOfRangeException] == kIndexOutOfRangeException);
    _ASSERTE(map[CORINFO_OverflowException] == kOverflowException);

    _ASSERTE(exceptNum < CORINFO_Exception_Count);
         
    _ASSERTE(map[exceptNum] != 0);

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2|Frame::FRAME_ATTR_EXACT_DEPTH);
    COMPlusThrow(map[exceptNum]);
    HELPER_METHOD_FRAME_END();
    return NULL;
}
HCIMPLEND

#ifndef STATUS_STACK_BUFFER_OVERRUN  // Not defined yet in CESDK includes
# define STATUS_STACK_BUFFER_OVERRUN      ((NTSTATUS)0xC0000409L)
#endif

/*********************************************************************
 * Kill process without using any potentially corrupted data:        
 *      o Do not throw an exception
 *      o Do not call any indirect/virtual functions 
 *      o Do not depend on any global data
 *
 * This function is used by the security checks for unsafe buffers (VC's -GS checks)
 */


void DoJITFailFast ()
{
    CONTRACTL {
        MODE_ANY;
        WRAPPER(GC_TRIGGERS);
        WRAPPER(THROWS);
        SO_NOT_MAINLINE; // If process is coming down, SO probe is not going to do much good
    } CONTRACTL_END;

    LOG((LF_ALWAYS, LL_FATALERROR, "Unsafe buffer security check failure: Buffer overrun detected"));

#ifdef _DEBUG
    if (g_pConfig->fAssertOnFailFast())
        _ASSERTE(!"About to FailFast. set ComPlus_AssertOnFailFast=0 if this is expected");
#endif


    TerminateProcess(GetCurrentProcess(), STATUS_STACK_BUFFER_OVERRUN);
}

HCIMPL0(void, JIT_FailFast)
{
    WRAPPER_CONTRACT;
    DoJITFailFast ();
}
HCIMPLEND



//========================================================================
//
//      SECURITY HELPERS
//
//========================================================================
/*************************************************************/
HCIMPL2(void, JIT_CallAllowedBySecurity_Internal, CORINFO_METHOD_HANDLE callerMethodHnd, CORINFO_METHOD_HANDLE calleeMethodHnd)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    //
    // Verify with the security at runtime whether call is allowed.
    // Throws an exception if the call is not allowed, returns if it is allowed.
    //

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    MethodDesc *pCaller = GetMethod(callerMethodHnd);
    MethodDesc* pCallee = GetMethod(calleeMethodHnd);

    Security::EnforceTransparentAssemblyChecks( pCaller, pCallee);

    HELPER_METHOD_FRAME_END_POLL();
}
HCIMPLEND

/*************************************************************/
HCIMPL2(void, JIT_CallAllowedBySecurity, CORINFO_METHOD_HANDLE callerMethodHnd, CORINFO_METHOD_HANDLE calleeMethodHnd)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (SecurityStackWalk::HasFlagsOrFullyTrustedIgnoreMode(0)) 
        return;
    {
        // Tailcall to the slow helper
        ENDFORBIDGC();
        HCCALL2(JIT_CallAllowedBySecurity_Internal, callerMethodHnd, calleeMethodHnd);
    }

}
HCIMPLEND

HCIMPL2(void, JIT_Security_Prolog_Framed, CORINFO_METHOD_HANDLE methHnd_, OBJECTREF* ppFrameSecDesc)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (!Security::UseNoStubsApproach())
        return;

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();
    {
        ASSUME_BYREF_FROM_JIT_STACK_BEGIN(ppFrameSecDesc);

        MethodDesc *pCurrent = GetMethod(methHnd_);

        g_IBCLogger.LogMethodDescAccess(pCurrent);

        // Note: This check is replicated in JIT_Security_Prolog
        if (pCurrent->IsInterceptedForDeclSecurity() &&
            !(pCurrent->IsInterceptedForDeclSecurityCASDemandsOnly() &&
                SecurityStackWalk::HasFlagsOrFullyTrusted(0)))
        {
            MethodSecurityDescriptor MDSecDesc(pCurrent);
            MethodSecurityDescriptor::LookupOrCreateMethodSecurityDescriptor(&MDSecDesc);

            DeclActionInfo* pRuntimeDeclActionInfo = MDSecDesc.GetRuntimeDeclActionInfo();
            if (pRuntimeDeclActionInfo != NULL)
            {
                 // Tell the debugger not to start on any managed code that we call in this method    
                FrameWithCookie<DebuggerSecurityCodeMarkFrame> __dbgSecFrame;

                Security::DoDeclarativeActions(pCurrent, pRuntimeDeclActionInfo, ppFrameSecDesc, &MDSecDesc);

                // Pop the debugger frame
                __dbgSecFrame.Pop();
            }
        }

        ASSUME_BYREF_FROM_JIT_STACK_END();
    }  
    HELPER_METHOD_FRAME_END_POLL();
}
HCIMPLEND

/*************************************************************/
#ifdef _MSC_VER
#pragma optimize("t", on)
#endif
HCIMPL2(void, JIT_Security_Prolog, CORINFO_METHOD_HANDLE methHnd_, OBJECTREF* ppFrameSecDesc)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (!Security::UseNoStubsApproach())
        return;

    //
    // do the security prolog work
    //

    MethodDesc *pCurrent = GetMethod(methHnd_);

    // Note: This check is replicated in JIT_Security_Prolog_Framed
    if (pCurrent->IsInterceptedForDeclSecurity() &&
        !(pCurrent->IsInterceptedForDeclSecurityCASDemandsOnly() &&
            SecurityStackWalk::HasFlagsOrFullyTrusted(0)))
    {
        // Tailcall to the slow helper
        ENDFORBIDGC();
        HCCALL2(JIT_Security_Prolog_Framed, methHnd_, ppFrameSecDesc);
    }
}
HCIMPLEND
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/*************************************************************/
HCIMPL2(void, JIT_Security_Epilog, CORINFO_METHOD_HANDLE methHnd_, OBJECTREF * ppFrameSecDesc)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    //
    // do the security epilog work
    //
    return;
}
HCIMPLEND

/*************************************************************/
HCIMPL1(void, JIT_VerificationRuntimeCheck_Internal, CORINFO_METHOD_HANDLE methHnd_)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
    
    
    //
    // inject a full-demand for unmanaged code permission at runtime
    // around methods in transparent assembly that contains unverifiable code

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();
	{
        // MethodDesc *pMethod = GetMethod(methHnd_);
        // inject a demand for UnmanagedCodePermission
        Security::SpecialDemand(SSWT_DECLARATIVE_DEMAND, SECURITY_UNMANAGED_CODE);
    }
    HELPER_METHOD_FRAME_END_POLL();
}
HCIMPLEND

/*************************************************************/
HCIMPL1(void, JIT_VerificationRuntimeCheck, CORINFO_METHOD_HANDLE methHnd_)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    if (SecurityStackWalk::HasFlagsOrFullyTrustedIgnoreMode(0)) 
        return;
    //
    // inject a full-demand for unmanaged code permission at runtime
    // around methods in transparent assembly that contains unverifiable code
    {
        // Tailcall to the slow helper
        ENDFORBIDGC();
        HCCALL1(JIT_VerificationRuntimeCheck_Internal, methHnd_);
    }

}
HCIMPLEND



//========================================================================
//
//      DEBUGGER/PROFILER HELPERS
//
//========================================================================

/*********************************************************************/
// JIT_UserBreakpoint
// Called by the JIT whenever a cee_break instruction should be executed.
// This ensures that enough info will be pushed onto the stack so that
// we can continue from the exception w/o having special code elsewhere.
// Body of function is written by debugger team
// Args: None
//
// Note: this code is duplicated in the ecall in VM\DebugDebugger:Break,
// so propogate changes to there

HCIMPL0(void, JIT_UserBreakpoint)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
/*
    No contract here: causes crash with AMD64 build

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;
*/

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();    // Set up a frame

#ifdef DEBUGGING_SUPPORTED
    FrameWithCookie<DebuggerExitFrame> __def;

    MethodDescCallSite breakCanThrow(METHOD__DEBUGGER__BREAK_CAN_THROW);

    // Call Diagnostic.Debugger.BreakCanThrow instead. This will make us demand 
    // UnmanagedCode permission if debugger is not attached.
    //
    breakCanThrow.Call((ARG_SLOT*)NULL);

    __def.Pop();
#else // !DEBUGGING_SUPPORTED
    _ASSERTE("JIT_UserBreakpoint called, but debugging support is not available in this build.");
#endif // !DEBUGGING_SUPPORTED

    HELPER_METHOD_FRAME_END_POLL();
}
HCIMPLEND


/*********************************************************************/
// Callback for Just-My-Code probe
// Probe looks like:
//  if (*pFlag != 0) call JIT_DbgIsJustMyCode
// So this is only called if the flag (obtained by GetJMCFlagAddr) is
// non-zero.
// Body of this function is maintained by the debugger people.
HCIMPL0(void, JIT_DbgIsJustMyCode)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    SO_NOT_MAINLINE_FUNCTION;

    return;
}
HCIMPLEND

#if !(defined(_X86_) || defined(_WIN64))
void JIT_ProfilerEnterLeaveStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo, UINT_PTR ArgInfo)
{
    return;
}
void JIT_ProfilerTailcallStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo)
{
    return;
}
#endif // !(_X86_ || _WIN64)

#ifdef PROFILING_SUPPORTED
HRESULT ProfToEEInterfaceImpl::SetEnterLeaveFunctionHooksForJit(FunctionEnter *pFuncEnter,
                                                                FunctionLeave *pFuncLeave,
                                                                FunctionTailcall *pFuncTailcall)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (pFuncEnter)
        SetJitHelperFunction(CORINFO_HELP_PROF_FCN_ENTER,    (void *) pFuncEnter);

    if (pFuncLeave)
        SetJitHelperFunction(CORINFO_HELP_PROF_FCN_LEAVE,    (void *) pFuncLeave);

    if (pFuncTailcall)
        SetJitHelperFunction(CORINFO_HELP_PROF_FCN_TAILCALL, (void *) pFuncTailcall);

    return (S_OK);
}
#endif // PROFILING_SUPPORTED

/*************************************************************/
HCIMPL1(void, JIT_LogMethodEnter, CORINFO_METHOD_HANDLE methHnd_)
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    //
    // Record an access to this method desc
    //

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    g_IBCLogger.LogMethodCodeAccess(GetMethod(methHnd_));

    HELPER_METHOD_FRAME_END_POLL();

HCIMPLEND



//========================================================================
//
//      GC HELPERS
//
//========================================================================

/*************************************************************/
HCIMPL3(VOID, JIT_StructWriteBarrier, void *dest, void* src, CORINFO_CLASS_HANDLE typeHnd_)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(typeHnd_);
    MethodTable *pMT = typeHnd.AsMethodTable();

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();    // Set up a frame
    CopyValueClassUnchecked(dest, src, pMT);
    HELPER_METHOD_FRAME_END_POLL();

}
HCIMPLEND

#if !defined(_X86_) && !defined(_AMD64_)
/*************************************************************/
extern "C" void __stdcall JIT_ByRefWriteBarrier()
{
    BEGIN_ENTRYPOINT_THROWS;

    LEAF_CONTRACT;
    PORTABILITY_ASSERT("JIT_ByRefWriteBarrier");
    END_ENTRYPOINT_THROWS;

}
#endif // !_X86_ || !_AMD64_

/*************************************************************/
HCIMPL0(VOID, JIT_PollGC)
{
     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    FC_GC_POLL_NOT_NEEDED();

    Thread  *thread = GetThread();
    if (thread->CatchAtSafePoint())    // Does someone wants this thread stopped?
    {
        HELPER_METHOD_FRAME_BEGIN_NOPOLL();    // Set up a frame
#ifdef _DEBUG
        BOOL GCOnTransition = FALSE;
        if (g_pConfig->FastGCStressLevel()) {
            GCOnTransition = GC_ON_TRANSITIONS (FALSE);
        }
#endif // _DEBUG
        CommonTripThread();         // Indicate we are at a GC safe point
#ifdef _DEBUG
        if (g_pConfig->FastGCStressLevel()) {
            GC_ON_TRANSITIONS (GCOnTransition);
        }
#endif // _DEBUG
        HELPER_METHOD_FRAME_END();
    }
}
HCIMPLEND

/*************************************************************/
// For an inlined N/Direct call (and possibly for other places that need this service)
// we have noticed that the returning thread should trap for one reason or another.
// ECall sets up the frame.

HCIMPL1(void, JIT_RareDisableHelper, Thread* thread)
{
    // We do this here (before we set up a frame), because the following scenario
    // We are in the process of doing an inlined pinvoke.  Since we are in preemtive
    // mode, the thread is allowed to continue.  The thread continues and gets a context
    // switch just after it has cleared the preemptive mode bit but before it gets
    // to this helper.    When we do our stack crawl now, we think this thread is
    // in cooperative mode (and believed that it was suspended in the SuspendEE), so
    // we do a getthreadcontext (on the unsuspended thread!) and get an EIP in jitted code.
    // and proceed.   Assume the crawl of jitted frames is proceeding on the other thread
    // when this thread wakes up and sets up a frame.   Eventually the other thread
    // runs out of jitted frames and sees the frame we just established.  This causes
    // an assert in the stack crawling code.  If this assert is ignored, however, we
    // will end up scanning the jitted frames twice, which will lead to GC holes
    //

     CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    ENDFORBIDGC();
    thread->RareDisablePreemptiveGC();
    BEGINFORBIDGC();

    FC_GC_POLL_NOT_NEEDED();
    HELPER_METHOD_FRAME_BEGIN_NOPOLL();    // Set up a frame
    thread->HandleThreadAbort();
    HELPER_METHOD_FRAME_END();
}
HCIMPLEND

/*********************************************************************/
// This is called by the JIT after every instruction in fully interuptable
// code to make certain our GC tracking is OK
HCIMPL0(VOID, JIT_StressGC_NOP)
{
    CONTRACTL {
        SO_TOLERANT;
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
    } CONTRACTL_END;

}
HCIMPLEND


HCIMPL0(VOID, JIT_StressGC)
{
     CONTRACTL {
        SO_TOLERANT;
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

#ifdef _DEBUG
    HELPER_METHOD_FRAME_BEGIN_0();    // Set up a frame

    bool fSkipGC = false;


    if (!fSkipGC)
        GCHeap::GetGCHeap()->GarbageCollect();


    HELPER_METHOD_FRAME_END();
#endif // _DEBUG
}
HCIMPLEND

/*********************************************************************/
/* we don't use HCIMPL macros because we don't want the overhead even in debug mode */

HCIMPL1_RAW(Object*, JIT_CheckObj, Object* obj)
{
    CONTRACTL {
        SO_TOLERANT;
        DISABLED(NOTHROW);  // NOTHROW currently not supported in FCALLs
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (obj != 0) {
        MethodTable* pMT = obj->GetMethodTable();
        if (pMT->GetNumGenericArgs() == 0 && pMT->GetCanonicalMethodTable() != pMT) {
            _ASSERTE(!"Bad Method Table");
            FreeBuildDebugBreak();
        }
    }
    return obj;
}
HCIMPLEND_RAW



//========================================================================
//
//      INTEROP HELPERS
//
//========================================================================




//========================================================================
//
//      JIT HELPERS INITIALIZATION
//
//========================================================================

// verify consistency of jithelpers.h and corinfo.h
enum __CorInfoHelpFunc {
#define JITHELPER(code, pfnHelper) __##code,
#include "jithelpers.h"
};
#define JITHELPER(code, pfnHelper) C_ASSERT((int)__##code == (int)code);
#include "jithelpers.h"

#ifdef _DEBUG
#define HELPERDEF(code, lpv) { (LPVOID)(lpv), #code },
#else // !_DEBUG
#define HELPERDEF(code, lpv) { (LPVOID)(lpv) },
#endif // !_DEBUG

// static helpers - constant array
const VMHELPDEF hlpFuncTable[CORINFO_HELP_COUNT+1] =
{
#define JITHELPER(code, pfnHelper) HELPERDEF(code, pfnHelper)
#define DYNAMICJITHELPER(code, pfnHelper) HELPERDEF(code, 1 + DYNAMIC_##code)
#include "jithelpers.h"
    // this entry is used by CEECompileInfo::GetStaticHelperImplementation
    // to report back the number of dynamic helpers
    HELPERDEF(MAX, 1 + DYNAMIC_CORINFO_HELP_COUNT)
};

// dynamic helpers - filled in at runtime
VMHELPDEF hlpDynamicFuncTable[DYNAMIC_CORINFO_HELP_COUNT] =
{
#define JITHELPER(code, pfnHelper)
#define DYNAMICJITHELPER(code, pfnHelper) HELPERDEF(code, pfnHelper)
#include "jithelpers.h"
};

#if defined(_DEBUG) && (defined(_AMD64_) || defined(_X86_))
#define HELPERCOUNTDEF(lpv) { (LPVOID)(lpv), NULL, 0 },

VMHELPCOUNTDEF hlpFuncCountTable[CORINFO_HELP_COUNT+1] =
{
#define JITHELPER(code, pfnHelper) HELPERCOUNTDEF(pfnHelper)
#define DYNAMICJITHELPER(code, pfnHelper) HELPERCOUNTDEF(1 + DYNAMIC_##code)
#include "jithelpers.h"
    // this entry is used by CEECompileInfo::GetStaticHelperImplementation
    // to report back the number of dynamic helpers
    HELPERCOUNTDEF(1 + DYNAMIC_CORINFO_HELP_COUNT)
};
#endif

// Set the JIT helper function in the helper table
// Handles the case where the function does not reside in mscorwks.dll

void    _SetJitHelperFunction(DynamicCorInfoHelpFunc ftnNum, void * pFunc)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    hlpDynamicFuncTable[ftnNum].pfnHelper = (void *) pFunc;
}

/*********************************************************************/
// Initialize the part of the JIT helpers that require much of the
// EE infrastructure to be in place.
/*********************************************************************/
void InitJITHelpers2()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    ECall::DynamicallyAssignFCallImpl(GetEEFuncEntryPoint(GetThread), ECall::InternalGetCurrentThread);

    InitJitHelperLogging();

#ifdef HARDBOUND_DYNAMIC_CALLS


    for (int i=0; i<DYNAMIC_CORINFO_HELP_COUNT; i++)
    {
        emitJump(GetDynamicHCallThunk(i), hlpDynamicFuncTable[i].pfnHelper);
    }
#endif

    g_pJitGenericHandleCacheCrst.Init("g_pJitGenericHandleCacheCrst", CrstSyncHashLock, CRST_UNSAFE_COOPGC);
}



//
//

#if defined(_DEBUG) && (defined(_AMD64_) || defined(_X86_))
// *****************************************************************************
//  JitHelperLogging usage:
//      1) Ngen using:
//              COMPLUS_HardPrejitEnabled=0 
//
//         This allows us to instrument even ngen'd image calls to JIT helpers. 
//         Remember to clear the key after ngen-ing and before actually running 
//         the app you want to log.
//
//      2) Then set:
//              COMPLUS_JitHelperLogging=1
//              COMPLUS_LogEnable=1
//              COMPLUS_LogLevel=1
//              COMPLUS_LogToFile=1
//
//      3) Run the app that you want to log; Results will be in COMPLUS.LOG(.X)
//
//      4) JitHelperLogging=2 and JitHelperLogging=3 result in different output
//         as per code in WriteJitHelperCountToSTRESSLOG() below.
// *****************************************************************************
void WriteJitHelperCountToSTRESSLOG()
{
    int jitHelperLoggingLevel = g_pConfig->GetConfigDWORD(L"JitHelperLogging", 0);
    if (jitHelperLoggingLevel != 0)
    {
        DWORD logFacility, logLevel;

        logFacility = LF_ALL;     //LF_ALL/LL_ALWAYS is okay here only because this logging would normally 
        logLevel = LL_ALWAYS;     // would never be turned on at all (used only for performance measurements)

        const int countPos = 60;

        STRESS_LOG0(logFacility, logLevel, "Writing Jit Helper COUNT table to log\n");

        VMHELPCOUNTDEF* hlpFuncCount = hlpFuncCountTable;
        while(hlpFuncCount < (hlpFuncCountTable + CORINFO_HELP_COUNT))
        {
            const char* name;
            LONG count;

            name = hlpFuncCount->helperName;
            count = hlpFuncCount->count;

            int nameLen = 0;
            switch (jitHelperLoggingLevel)
            {
            case 1:
                // This will print a comma seperated list:
                // CORINFO_XXX_HELPER, 10
                // CORINFO_YYYY_HELPER, 11
                STRESS_LOG2(logFacility, logLevel, "%s, %d\n", name, count);
                break;

            case 2:
                // This will print a table like:
                // CORINFO_XXX_HELPER                       10
                // CORINFO_YYYY_HELPER                      11
                if (hlpFuncCount->helperName != NULL)
                    nameLen = (int)strlen(name);
                else
                    nameLen = (int)strlen("(null)");
                    
                if (nameLen < countPos)
                {
                    char* buffer = new char[(countPos - nameLen) + 1];
                    memset(buffer, (int)' ', (countPos-nameLen));
                    buffer[(countPos - nameLen)] = '\0';
                    STRESS_LOG3(logFacility, logLevel, "%s%s %d\n", name, buffer, count);
                }
                else
                {
                    STRESS_LOG2(logFacility, logLevel, "%s %d\n", name, count);
                }
                break;

            case 3:
                // This will print out the counts and the address range of the helper (if we know it)
                // CORINFO_XXX_HELPER, 10, (0x12345678 -> 0x12345778)
                // CORINFO_YYYY_HELPER, 11, (0x00011234 -> 0x00012234)
                STRESS_LOG4(logFacility, logLevel, "%s, %d, (0x%p -> 0x%p)\n", name, count, hlpFuncCount->pfnRealHelper, ((LPBYTE)hlpFuncCount->pfnRealHelper + hlpFuncCount->helperSize));
                break;
                
            default:
                STRESS_LOG1(logFacility, logLevel, "Unsupported JitHelperLogging mode (%d)\n", jitHelperLoggingLevel);
                break;
            }

            hlpFuncCount++;
        }
    }
}

// This will do the work to instrument the JIT helper table.
void InitJitHelperLogging()
{
    if ((g_pConfig->GetConfigDWORD(L"JitHelperLogging", 0) != 0))
    {

#ifdef _X86_
        IMAGE_DOS_HEADER *pDOS = (IMAGE_DOS_HEADER *)g_pMSCorEE;
        _ASSERTE(pDOS->e_magic == VAL16(IMAGE_DOS_SIGNATURE) && pDOS->e_lfanew != 0);
        
        IMAGE_NT_HEADERS *pNT = (IMAGE_NT_HEADERS*)((LPBYTE)g_pMSCorEE + VAL32(pDOS->e_lfanew));
        _ASSERTE(pNT->Signature == VAL32(IMAGE_NT_SIGNATURE)
            && pNT->FileHeader.SizeOfOptionalHeader == VAL16(IMAGE_SIZEOF_NT_OPTIONAL_HEADER)
            && pNT->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR_MAGIC));
#endif // _X86_

        if (g_pConfig->NgenHardBind() == EEConfig::NGEN_HARD_BIND_NONE)
        {
            _ASSERTE(g_pConfig->NgenHardBind() != EEConfig::NGEN_HARD_BIND_NONE && "You are "
                        "trying to log JIT helper method calls while you have NGEN HARD BINDING "
                        "set to 0. This probably means you're really trying to NGEN something for "
                        "logging purposes, NGEN breaks with JitHelperLogging turned on!!!! Please "
                        "set JitHelperLogging=0 while you NGEN, or unset HardPrejitEnabled while "
                        "running managed code.");
            return;
        }

        // Make the static hlpFuncTable read/write for purposes of writing the logging thunks
        DWORD dwOldProtect;
        if (!ClrVirtualProtect((LPVOID)hlpFuncTable, (sizeof(VMHELPDEF) * (CORINFO_HELP_COUNT+1)), PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {   
            ThrowLastError();
        }
        
        // iterate through the jit helper tables replacing helpers with logging thunks
        //
        // NOTE: if NGEN'd images were NGEN'd with hard binding on then static helper
        //       calls will NOT be instrumented.
        VMHELPDEF* hlpFunc = const_cast<VMHELPDEF*>(hlpFuncTable);
        VMHELPCOUNTDEF* hlpFuncCount = hlpFuncCountTable;
        while(hlpFunc < (hlpFuncTable + CORINFO_HELP_COUNT))
        {
            if (hlpFunc->pfnHelper != NULL)
            {
                CPUSTUBLINKER   sl;
                CPUSTUBLINKER*  pSl = &sl;
                
                if (((size_t)hlpFunc->pfnHelper - 1) > DYNAMIC_CORINFO_HELP_COUNT)
                {
                    // While we're here initialize the table of VMHELPCOUNTDEF 
                    // guys with info about this helper
                    hlpFuncCount->pfnRealHelper = hlpFunc->pfnHelper;
                    hlpFuncCount->helperName = hlpFunc->name;
                    hlpFuncCount->count = 0;
                    // How do I get this for x86?
                    hlpFuncCount->helperSize = 0;
                
                    pSl->EmitJITHelperLoggingThunk(GetEEFuncEntryPoint(hlpFunc->pfnHelper), (LPVOID)hlpFuncCount);
                    Stub* pStub = pSl->Link();
                    hlpFunc->pfnHelper = (void*)pStub->GetEntryPoint();
                }
                else
                {
                    _ASSERTE(((size_t)hlpFunc->pfnHelper - 1) >= 0 && 
                             ((size_t)hlpFunc->pfnHelper - 1) < COUNTOF(hlpDynamicFuncTable));
                    VMHELPDEF* dynamicHlpFunc = &hlpDynamicFuncTable[((size_t)hlpFunc->pfnHelper - 1)];

                    // While we're here initialize the table of VMHELPCOUNTDEF 
                    // guys with info about this helper. There is only one table
                    // for the count dudes that contains info about both dynamic
                    // and static helpers.

                    hlpFuncCount->pfnRealHelper = dynamicHlpFunc->pfnHelper;
                    hlpFuncCount->helperName = dynamicHlpFunc->name;
                    hlpFuncCount->count = 0;

                    // Is the address in mscoree.dll at all? (All helpers are in
                    // mscoree.dll)
                    if (dynamicHlpFunc->pfnHelper >= (LPBYTE*)g_pMSCorEE && dynamicHlpFunc->pfnHelper < (LPBYTE*)g_pMSCorEE + VAL32(pNT->OptionalHeader.SizeOfImage))
                    {
                        // See note above. How do I get the size on x86 for a static method?
                        hlpFuncCount->helperSize = 0;
                    }
                    else
                    {
                        Stub::RecoverStubAndSize((TADDR)dynamicHlpFunc->pfnHelper, (DWORD*)&hlpFuncCount->helperSize);
                        hlpFuncCount->helperSize -= sizeof(Stub);
                    }


                    pSl->EmitJITHelperLoggingThunk(GetEEFuncEntryPoint(dynamicHlpFunc->pfnHelper), (LPVOID)hlpFuncCount);
                    Stub* pStub = pSl->Link();
                    dynamicHlpFunc->pfnHelper = (void*)pStub->GetEntryPoint();            
                }
            }
            
            hlpFunc++;
            hlpFuncCount++;
        }

        // Restore original access rights to the static hlpFuncTable
        ClrVirtualProtect((LPVOID)hlpFuncTable, (sizeof(VMHELPDEF) * (CORINFO_HELP_COUNT+1)), dwOldProtect, &dwOldProtect);
    }

    return;
}
#endif // _DEBUG && (_AMD64_ || _X86_)
