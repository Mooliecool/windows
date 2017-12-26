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
#include "object.h"
#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "comvariant.h"
#include "comstring.h"
#include "comstringcommon.h"
#include "security.h"
#include "olevariant.h"
#include "comdatetime.h"
#include "fieldmarshaler.h"
#include "comdecimal.h"
#include "mdaassistantsptr.h"

/* ------------------------------------------------------------------------- *
 * Local constants
 * ------------------------------------------------------------------------- */

#define NO_MAPPING ((BYTE) -1)



void SafeVariantClearHelper(VARIANT* pVar)
{
    WRAPPER_CONTRACT;

    BEGIN_SO_TOLERANT_CODE(GetThread());
    VariantClear(pVar);
    END_SO_TOLERANT_CODE;
}

void SafeVariantClear(VARIANT* pVar)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (pVar)
    {
        GCX_PREEMP();
        PAL_CPP_TRY
        {
            // This is a holder to tell the contract system that we're catching all exceptions.
            CLR_TRY_MARKER();

            // Most of time, oleaut32.dll is loaded already when we get here.
            // Sometimes, CLR initializes Variant without loading oleaut32.dll, e.g. VT_BOOL.
            // It is better for performance with EX_TRY than 
            CONTRACT_VIOLATION(ThrowsViolation);
            SafeVariantClearHelper(pVar);
        }
        PAL_CPP_CATCH_ALL
        {
#if defined(STACK_GUARDS_DEBUG)
        // Catching and just swallowing an exception means we need to tell
        // the SO code that it should go back to normal operation, as it
        // currently thinks that the exception is still on the fly.
        GetThread()->GetCurrentStackGuard()->RestoreCurrentGuard();
#endif
        }
        PAL_CPP_ENDTRY;

        FillMemory(pVar, sizeof(VARIANT), 0x00);
    }
}

FORCEINLINE void EmptyVariant(VARIANT* value)
{
    WRAPPER_CONTRACT;
    SafeVariantClear(value);
}

class VariantEmptyHolder : public Wrapper<VARIANT*, ::DoNothing<VARIANT*>, EmptyVariant, NULL>
{
public:
    VariantEmptyHolder(VARIANT* p = NULL) : 
        Wrapper<VARIANT*, ::DoNothing<VARIANT*>, EmptyVariant, NULL>(p)
    {
        WRAPPER_CONTRACT;
    }

    FORCEINLINE void operator=(VARIANT* p)
    {
        WRAPPER_CONTRACT;
        
        Wrapper<VARIANT*, ::DoNothing<VARIANT*>, EmptyVariant, NULL>::operator=(p);
    }
};


/* ------------------------------------------------------------------------- *
 * Boolean marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalBoolVariantOleToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pComVariant));
        PRECONDITION(CheckPointer(pOleVariant));
    }
    CONTRACTL_END;
    
    *(INT64*)pComVariant->GetData() = V_BOOL(pOleVariant) ? 1 : 0;
}


void OleVariant::MarshalBoolArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                          MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
   
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    VARIANT_BOOL *pOle = (VARIANT_BOOL *) oleArray;
    VARIANT_BOOL *pOleEnd = pOle + elementCount;
    
    UCHAR *pCom = (UCHAR *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        C_ASSERT(sizeof(VARIANT_BOOL) == sizeof(UINT16));
        (*(pCom++)) = MAYBE_UNALIGNED_READ(pOle, 16) ? 1 : 0;
        pOle++;
    }
}

void OleVariant::MarshalBoolArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    VARIANT_BOOL *pOle = (VARIANT_BOOL *) oleArray;
    VARIANT_BOOL *pOleEnd = pOle + elementCount;
    
    UCHAR *pCom = (UCHAR *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        C_ASSERT(sizeof(VARIANT_BOOL) == sizeof(UINT16));
        MAYBE_UNALIGNED_WRITE(pOle, 16, *pCom ? VARIANT_TRUE : VARIANT_FALSE);
        pOle++; pCom++;
    }
}




/* ------------------------------------------------------------------------- *
 * WinBoolean marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalWinBoolVariantOleToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    LEAF_CONTRACT;
    
    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalWinBoolVariantComToOle(VariantData *pComVariant, 
                                            VARIANT *pOleVariant)
{
    LEAF_CONTRACT;
    
    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalWinBoolVariantOleRefToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    LEAF_CONTRACT;

    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalWinBoolArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                          MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    BOOL *pOle = (BOOL *) oleArray;
    BOOL *pOleEnd = pOle + elementCount;
    
    UCHAR *pCom = (UCHAR *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        C_ASSERT(sizeof(BOOL) == sizeof(UINT32));
        (*(pCom++)) = MAYBE_UNALIGNED_READ(pOle, 32) ? 1 : 0;
        pOle++;
    }
}

void OleVariant::MarshalWinBoolArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    BOOL *pOle = (BOOL *) oleArray;
    BOOL *pOleEnd = pOle + elementCount;
    
    UCHAR *pCom = (UCHAR *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        C_ASSERT(sizeof(BOOL) == sizeof(UINT32));
        MAYBE_UNALIGNED_WRITE(pOle, 32, *pCom ? 1 : 0);
        pOle++; pCom++;
    }    
}


/* ------------------------------------------------------------------------- *
 * Ansi char marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalAnsiCharVariantOleToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    LEAF_CONTRACT;
    
    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalAnsiCharVariantComToOle(VariantData *pComVariant, 
                                            VARIANT *pOleVariant)
{
    LEAF_CONTRACT;
    
    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalAnsiCharVariantOleRefToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    LEAF_CONTRACT;
    
    _ASSERTE(!"Not supposed to get here.");
}

void OleVariant::MarshalAnsiCharArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                          MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    WCHAR *pCom = (WCHAR *) (*pComArray)->GetDataPtr();

    if (0 == elementCount)
    {
        *pCom = '\0';
        return;
    }

    if (0 == MultiByteToWideChar(CP_ACP,
                        MB_PRECOMPOSED,
                        (const CHAR *)oleArray,
                        (int)elementCount,
                        pCom,
                        (int)elementCount))
    {
        COMPlusThrowWin32();
    }
}

void OleVariant::MarshalAnsiCharArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    const WCHAR *pCom = (const WCHAR *) (*pComArray)->GetDataPtr();

    InternalWideToAnsi((WCHAR*)pCom, (int)elementCount, (CHAR*)oleArray, (int)(elementCount << 1),
                        fBestFitMapping, fThrowOnUnmappableChar);
}


/* ------------------------------------------------------------------------- *
 * Interface marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalInterfaceVariantOleToCom(VARIANT *pOleVariant, 
                                                 VariantData *pComVariant)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;
    
    IUnknown *unk = V_UNKNOWN(pOleVariant);

    OBJECTREF obj;
    if (unk == NULL)
        obj = NULL;
    else
        obj = GetObjectRefFromComIP(V_UNKNOWN(pOleVariant));

    pComVariant->SetObjRef(obj);
}

void OleVariant::MarshalInterfaceVariantComToOle(VariantData *pComVariant, 
                                                 VARIANT *pOleVariant)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;
    
    OBJECTREF *obj = pComVariant->GetObjRefPtr();
    VARTYPE vt = pComVariant->GetVT();
    
    ASSERT_PROTECTED(obj);

    if (*obj == NULL)
    {
        // If there is no VT set in the managed variant, then default to VT_UNKNOWN.
        if (vt == VT_EMPTY)
            vt = VT_UNKNOWN;

        V_UNKNOWN(pOleVariant) = NULL;
        V_VT(pOleVariant) = vt;
    }
    else
    {
        V_UNKNOWN(pOleVariant) = GetComIPFromObjectRef(obj);
        V_VT(pOleVariant) = VT_UNKNOWN;
    }
}

void OleVariant::MarshalInterfaceVariantOleRefToCom(VARIANT *pOleVariant, 
                                                 VariantData *pComVariant)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;
    
    IUnknown *unk = V_UNKNOWN(pOleVariant);

    OBJECTREF obj;
    if (unk == NULL)
        obj = NULL;
    else
        obj = GetObjectRefFromComIP(*V_UNKNOWNREF(pOleVariant));

    pComVariant->SetObjRef(obj);
}

void OleVariant::MarshalInterfaceArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                               MethodTable *pElementMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;

    ASSERT_PROTECTED(pComArray);
    
    SIZE_T elementCount = (*pComArray)->GetNumComponents();
    TypeHandle hndElementType = TypeHandle(pElementMT);

    IUnknown **pOle = (IUnknown **) oleArray;
    IUnknown **pOleEnd = pOle + elementCount;

    BASEARRAYREF unprotectedArray = *pComArray;
    OBJECTREF *pCom = (OBJECTREF *) unprotectedArray->GetDataPtr();

#if CHECK_APP_DOMAIN_LEAKS
    AppDomain *pDomain = unprotectedArray->GetAppDomain();
#endif  // CHECK_APP_DOMAIN_LEAKS

    OBJECTREF obj = NULL; 
    GCPROTECT_BEGIN(obj)
    {
        while (pOle < pOleEnd)
        {
            IUnknown *unk = *pOle++;
        
            if (unk == NULL)
                obj = NULL;
            else 
                obj = GetObjectRefFromComIP(unk);

            //
            // Make sure the object can be cast to the destination type.
            //

            if (!hndElementType.IsNull() && !CanCastComObject(obj, hndElementType))
            {
                StackSString ssObjClsName;
                StackSString ssDestClsName;
                obj->GetClass()->_GetFullyQualifiedNameForClass(ssObjClsName);
                hndElementType.GetClass()->_GetFullyQualifiedNameForClass(ssDestClsName);
                COMPlusThrow(kInvalidCastException, IDS_EE_CANNOTCAST,
                             ssObjClsName.GetUnicode(), ssDestClsName.GetUnicode());
            }       

            //
            // Reset pCom pointer only if array object has moved, rather than
            // recomputing every time through the loop.  Beware implicit calls to
            // ValidateObject inside OBJECTREF methods.
            //

            if (*(void **)&unprotectedArray != *(void **)&*pComArray)
            {
                SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
                unprotectedArray = *pComArray;
                pCom = (OBJECTREF *) (unprotectedArray->GetAddress() + currentOffset);
            }

            SetObjectReference(pCom++, obj, pDomain);
        }
    }
    GCPROTECT_END();
}

void OleVariant::MarshalIUnknownArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                              MethodTable *pElementMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    WRAPPER_CONTRACT;
    
    MarshalInterfaceArrayComToOleHelper(pComArray, oleArray, pElementMT, FALSE);
}

void OleVariant::ClearInterfaceArray(void *oleArray, SIZE_T cElements, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
    }
    CONTRACTL_END;

    IUnknown **pOle = (IUnknown **) oleArray;
    IUnknown **pOleEnd = pOle + cElements;

    while (pOle < pOleEnd)
    {
        IUnknown *pUnk = *pOle++;
        
        if (pUnk != NULL)
        {
            ULONG cbRef = SafeRelease(pUnk);
            LogInteropRelease(pUnk, cbRef, "VariantClearInterfacArray");
        }
    }
}


/* ------------------------------------------------------------------------- *
 * BSTR marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalBSTRVariantOleToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;
    
    BSTR bstr = V_BSTR(pOleVariant);
    
    STRINGREF string;


    if (bstr == NULL)
        string = NULL;
    else
        string = COMString::NewString(bstr);
    

    pComVariant->SetObjRef((OBJECTREF) string);
}

void OleVariant::MarshalBSTRVariantComToOle(VariantData *pComVariant, 
                                            VARIANT *pOleVariant)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;

    STRINGREF stringRef = (STRINGREF) pComVariant->GetObjRef();

    BSTR bstr;
    if (stringRef == NULL)
        bstr = NULL;
    else 
    {
        bstr = SysAllocStringLen(stringRef->GetBuffer(), stringRef->GetStringLength());
        if (bstr == NULL)
            COMPlusThrowOM();
    }

    V_BSTR(pOleVariant) = bstr;
}

void OleVariant::MarshalBSTRArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                          MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    BSTR *pOle = (BSTR *) oleArray;
    BSTR *pOleEnd = pOle + elementCount;

    BASEARRAYREF unprotectedArray = *pComArray;
    STRINGREF *pCom = (STRINGREF *) unprotectedArray->GetDataPtr();
    
#if CHECK_APP_DOMAIN_LEAKS
    AppDomain *pDomain = unprotectedArray->GetAppDomain();
#endif  // CHECK_APP_DOMAIN_LEAKS

    while (pOle < pOleEnd)
    {
        BSTR bstr = *pOle++;
    
        STRINGREF string;
        

        if (bstr == NULL)
            string = NULL;
        else
            string = COMString::NewString(bstr);

        //
        // Reset pCom pointer only if array object has moved, rather than
        // recomputing it every time through the loop.  Beware implicit calls to
        // ValidateObject inside OBJECTREF methods.
        //

        if (*(void **)&unprotectedArray != *(void **)&*pComArray)
        {
            SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
            unprotectedArray = *pComArray;
            pCom = (STRINGREF *) (unprotectedArray->GetAddress() + currentOffset);
        }

        SetObjectReference((OBJECTREF*) pCom++, (OBJECTREF) string, pDomain);
    }
}

void OleVariant::MarshalBSTRArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;

    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    BSTR *pOle = (BSTR *) oleArray;
    BSTR *pOleEnd = pOle + elementCount;

    STRINGREF *pCom = (STRINGREF *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        //
        // We aren't calling anything which might cause a GC, so don't worry about
        // the array moving here.
        //

        STRINGREF stringRef = *pCom++;

        BSTR bstr;
        if (stringRef == NULL)
            bstr = NULL;
        else 
        {
            bstr = SysAllocStringLen(stringRef->GetBuffer(), stringRef->GetStringLength());
            if (bstr == NULL)
                COMPlusThrowOM();
        }

        *pOle++ = bstr;
    }
}

void OleVariant::ClearBSTRArray(void *oleArray, SIZE_T cElements, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
    }
    CONTRACTL_END;
    
    BSTR *pOle = (BSTR *) oleArray;
    BSTR *pOleEnd = pOle + cElements;

    while (pOle < pOleEnd)
    {
        BSTR bstr = *pOle++;

        
        if (bstr != NULL)
            SysFreeString(bstr);

    }
}



/* ------------------------------------------------------------------------- *
 * Structure marshaling routines
 * ------------------------------------------------------------------------- */
void OleVariant::MarshalNonBlittableRecordArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                                        MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
        PRECONDITION(CheckPointer(pInterfaceMT));        
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();
    SIZE_T elemSize     = pInterfaceMT->GetNativeSize();

    BYTE *pOle = (BYTE *) oleArray;
    BYTE *pOleEnd = pOle + elemSize * elementCount;

    UINT dstofs = ArrayBase::GetDataPtrOffset( (*pComArray)->GetMethodTable() );
    while (pOle < pOleEnd)
    {
        LayoutUpdateCLR( (LPVOID*)pComArray, dstofs, pInterfaceMT, pOle, FALSE );
        dstofs += (*pComArray)->GetComponentSize();
        pOle += elemSize;
    }
}

void OleVariant::MarshalNonBlittableRecordArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
        PRECONDITION(CheckPointer(pInterfaceMT));        
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();
    SIZE_T elemSize     = pInterfaceMT->GetNativeSize();

    BYTE *pOle = (BYTE *) oleArray;
    BYTE *pOleEnd = pOle + elemSize * elementCount;

    UINT srcofs = ArrayBase::GetDataPtrOffset( (*pComArray)->GetMethodTable() );
    while (pOle < pOleEnd)
    {
        LayoutUpdateNative( (LPVOID*)pComArray, srcofs, pInterfaceMT, pOle, NULL );
        pOle += elemSize;
        srcofs += (*pComArray)->GetComponentSize();
    }
}

void OleVariant::ClearNonBlittableRecordArray(void *oleArray, SIZE_T cElements, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pInterfaceMT));        
    }
    CONTRACTL_END;
    
    SIZE_T elemSize     = pInterfaceMT->GetNativeSize();
    BYTE *pOle = (BYTE *) oleArray;
    BYTE *pOleEnd = pOle + elemSize * cElements;
    while (pOle < pOleEnd)
    {
        LayoutDestroyNative(pOle, pInterfaceMT);
        pOle += elemSize;
    }
}


/* ------------------------------------------------------------------------- *
 * LPWSTR marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalLPWSTRArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                            MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    LPWSTR *pOle = (LPWSTR *) oleArray;
    LPWSTR *pOleEnd = pOle + elementCount;

    BASEARRAYREF unprotectedArray = *pComArray;
    STRINGREF *pCom = (STRINGREF *) unprotectedArray->GetDataPtr();
    
#if CHECK_APP_DOMAIN_LEAKS
    AppDomain *pDomain = unprotectedArray->GetAppDomain();
#endif  // CHECK_APP_DOMAIN_LEAKS

    while (pOle < pOleEnd)
    {
        LPWSTR lpwstr = *pOle++;
    
        STRINGREF string;
        if (lpwstr == NULL)
            string = NULL;
        else
            string = COMString::NewString(lpwstr);

        //
        // Reset pCom pointer only if array object has moved, rather than
        // recomputing it every time through the loop.  Beware implicit calls to
        // ValidateObject inside OBJECTREF methods.
        //

        if (*(void **)&unprotectedArray != *(void **)&*pComArray)
        {
            SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
            unprotectedArray = *pComArray;
            pCom = (STRINGREF *) (unprotectedArray->GetAddress() + currentOffset);
        }

        SetObjectReference((OBJECTREF*) pCom++, (OBJECTREF) string, pDomain);
    }
}

void OleVariant::MarshalLPWSTRRArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                             MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;

    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    LPWSTR *pOle = (LPWSTR *) oleArray;
    LPWSTR *pOleEnd = pOle + elementCount;

    STRINGREF *pCom = (STRINGREF *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        //
        // We aren't calling anything which might cause a GC, so don't worry about
        // the array moving here.
        //

        STRINGREF stringRef = *pCom++;

        LPWSTR lpwstr;
        if (stringRef == NULL)
        {
            lpwstr = NULL;
        }
        else 
        {
            // Retrieve the length of the string.
            int Length = stringRef->GetStringLength();
            int allocLength = (Length + 1) * sizeof(WCHAR);
            if (allocLength < Length)
                ThrowOutOfMemory();

            // Allocate the string using CoTaskMemAlloc.
            lpwstr = (LPWSTR)CoTaskMemAlloc(allocLength);
            if (lpwstr == NULL)
                ThrowOutOfMemory();

            // Copy the COM+ string into the newly allocated LPWSTR.
            memcpyNoGCRefs(lpwstr, stringRef->GetBuffer(), (Length + 1) * sizeof(WCHAR));
            lpwstr[Length] = 0;
        }

        *pOle++ = lpwstr;
    }
}

void OleVariant::ClearLPWSTRArray(void *oleArray, SIZE_T cElements, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
    }
    CONTRACTL_END;
    
    LPWSTR *pOle = (LPWSTR *) oleArray;
    LPWSTR *pOleEnd = pOle + cElements;

    while (pOle < pOleEnd)
    {
        LPWSTR lpwstr = *pOle++;
        
        if (lpwstr != NULL)
            CoTaskMemFree(lpwstr);
    }
}

/* ------------------------------------------------------------------------- *
 * LPWSTR marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalLPSTRArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                           MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    LPSTR *pOle = (LPSTR *) oleArray;
    LPSTR *pOleEnd = pOle + elementCount;

    BASEARRAYREF unprotectedArray = *pComArray;
    STRINGREF *pCom = (STRINGREF *) unprotectedArray->GetDataPtr();
    
#if CHECK_APP_DOMAIN_LEAKS
    AppDomain *pDomain = unprotectedArray->GetAppDomain();
#endif  // CHECK_APP_DOMAIN_LEAKS

    while (pOle < pOleEnd)
    {
        LPSTR lpstr = *pOle++;
    
        STRINGREF string;
        if (lpstr == NULL)
            string = NULL;
        else
            string = COMString::NewString(lpstr);

        //
        // Reset pCom pointer only if array object has moved, rather than
        // recomputing it every time through the loop.  Beware implicit calls to
        // ValidateObject inside OBJECTREF methods.
        //

        if (*(void **)&unprotectedArray != *(void **)&*pComArray)
        {
            SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
            unprotectedArray = *pComArray;
            pCom = (STRINGREF *) (unprotectedArray->GetAddress() + currentOffset);
        }

        SetObjectReference((OBJECTREF*) pCom++, (OBJECTREF) string, pDomain);
    }
}

void OleVariant::MarshalLPSTRRArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                            MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;

    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    LPSTR *pOle = (LPSTR *) oleArray;
    LPSTR *pOleEnd = pOle + elementCount;

    STRINGREF *pCom = (STRINGREF *) (*pComArray)->GetDataPtr();

    while (pOle < pOleEnd)
    {
        //
        // We aren't calling anything which might cause a GC, so don't worry about
        // the array moving here.
        //
        STRINGREF stringRef = *pCom++;

        CoTaskMemHolder<CHAR> lpstr(NULL);
        if (stringRef == NULL)
        {
            lpstr = NULL;
        }
        else 
        {
            // Retrieve the length of the string.
            int Length = stringRef->GetStringLength();
            int allocLength = (Length + 1) * sizeof(char);
            if (allocLength < Length)
                ThrowOutOfMemory();

            // Allocate the string using CoTaskMemAlloc.
            lpstr = (LPSTR)CoTaskMemAlloc(allocLength);
            if (lpstr == NULL)
                ThrowOutOfMemory();

            // Convert the unicode string to an ansi string.
            InternalWideToAnsi(stringRef->GetBuffer(), Length, lpstr, Length, fBestFitMapping, fThrowOnUnmappableChar);
            lpstr[Length] = 0;
        }

        *pOle++ = lpstr;
        lpstr.SuppressRelease();
    }
}

void OleVariant::ClearLPSTRArray(void *oleArray, SIZE_T cElements, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
    }
    CONTRACTL_END;
    
    LPSTR *pOle = (LPSTR *) oleArray;
    LPSTR *pOleEnd = pOle + cElements;

    while (pOle < pOleEnd)
    {
        LPSTR lpstr = *pOle++;
        
        if (lpstr != NULL)
            CoTaskMemFree(lpstr);
    }
}

/* ------------------------------------------------------------------------- *
 * Date marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalDateVariantOleToCom(VARIANT *pOleVariant, 
                                            VariantData *pComVariant)
{
    WRAPPER_CONTRACT;
    
    *(INT64*)pComVariant->GetData() = COMDateTime::DoubleDateToTicks(V_DATE(pOleVariant));
}

void OleVariant::MarshalDateVariantComToOle(VariantData *pComVariant, 
                                            VARIANT *pOleVariant)
{
    WRAPPER_CONTRACT;
    
    V_DATE(pOleVariant) = COMDateTime::TicksToDoubleDate(*(INT64*)pComVariant->GetData());
}

void OleVariant::MarshalDateVariantOleRefToCom(VARIANT *pOleVariant, 
                                               VariantData *pComVariant)
{
    WRAPPER_CONTRACT;
    
    *(INT64*)pComVariant->GetData() = COMDateTime::DoubleDateToTicks(*V_DATEREF(pOleVariant));
}

void OleVariant::MarshalDateArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                          MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    DATE *pOle = (DATE *) oleArray;
    DATE *pOleEnd = pOle + elementCount;
    
    INT64 *pCom = (INT64 *) (*pComArray)->GetDataPtr();

    //
    // We aren't calling anything which might cause a GC, so don't worry about
    // the array moving here.
    //

    while (pOle < pOleEnd)
        *pCom++ = COMDateTime::DoubleDateToTicks(*pOle++);
}

void OleVariant::MarshalDateArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                          MethodTable *pInterfaceMT, BOOL fBestFitMapping,
                                          BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
    }
    CONTRACTL_END;
    
    ASSERT_PROTECTED(pComArray);

    SIZE_T elementCount = (*pComArray)->GetNumComponents();

    DATE *pOle = (DATE *) oleArray;
    DATE *pOleEnd = pOle + elementCount;
    
    INT64 *pCom = (INT64 *) (*pComArray)->GetDataPtr();

    //
    // We aren't calling anything which might cause a GC, so don't worry about
    // the array moving here.
    //

    while (pOle < pOleEnd)
        *pOle++ = COMDateTime::TicksToDoubleDate(*pCom++);
}

/* ------------------------------------------------------------------------- *
 * Decimal marshaling routines
 * ------------------------------------------------------------------------- */

void OleVariant::MarshalDecimalVariantOleToCom(VARIANT *pOleVariant, 
                                               VariantData *pComVariant)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;

    OBJECTREF pDecimalRef = AllocateObject(COMDecimal::GetMethodTable());

    DECIMAL* pDecimal = (DECIMAL *) pDecimalRef->UnBox();
    *pDecimal = V_DECIMAL(pOleVariant);
    // Mashaling uses the reserved value to store the variant type, so clear it out when marshaling back
    pDecimal->wReserved = 0;
    
    pComVariant->SetObjRef(pDecimalRef);
}

void OleVariant::MarshalDecimalVariantComToOle(VariantData *pComVariant, 
                                               VARIANT *pOleVariant)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;
    
    VARTYPE vt = V_VT(pOleVariant);
    _ASSERTE(vt == VT_DECIMAL);
    V_DECIMAL(pOleVariant) = * (DECIMAL*) pComVariant->GetObjRef()->UnBox();
    V_VT(pOleVariant) = vt;
}

void OleVariant::MarshalDecimalVariantOleRefToCom(VARIANT *pOleVariant, 
                                                  VariantData *pComVariant )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pOleVariant));
        PRECONDITION(CheckPointer(pComVariant));
    }
    CONTRACTL_END;

    OBJECTREF pDecimalRef = AllocateObject(COMDecimal::GetMethodTable());

    DECIMAL* pDecimal = (DECIMAL *) pDecimalRef->UnBox();
    *pDecimal = *V_DECIMALREF(pOleVariant);
    // Mashaling uses the reserved value to store the variant type, so clear it out when marshaling back
    pDecimal->wReserved = 0;
    
    pComVariant->SetObjRef(pDecimalRef);
}

/* ------------------------------------------------------------------------- *
 * Record marshaling routines
 * ------------------------------------------------------------------------- */


void OleVariant::MarshalRecordArrayOleToCom(void *oleArray, BASEARRAYREF *pComArray,
                                            MethodTable *pElementMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
        PRECONDITION(CheckPointer(pElementMT));
    }
    CONTRACTL_END;

    Module* pModule = pElementMT->GetModule();
    if (!Security::CanCallUnmanagedCode(pModule))
    {
        COMPlusThrow(kArgumentException, IDS_EE_VTRECORD_SECURITY);
    }

    if (pElementMT->IsBlittable())
    {
        // The array is blittable so we can simply copy it.
        _ASSERTE(pComArray);
        SIZE_T elementCount = (*pComArray)->GetNumComponents();
        SIZE_T elemSize     = pElementMT->GetNativeSize();
        memcpyNoGCRefs((*pComArray)->GetDataPtr(), oleArray, elementCount * elemSize);
    }
    else
    {
        // The array is non blittable so we need to marshal the elements.
        _ASSERTE(pElementMT->GetClass()->HasLayout());
        MarshalNonBlittableRecordArrayOleToCom(oleArray, pComArray, pElementMT);
    }
}

void OleVariant::MarshalRecordArrayComToOle(BASEARRAYREF *pComArray, void *oleArray,
                                            MethodTable *pElementMT, BOOL fBestFitMapping,
                                            BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pComArray));
        PRECONDITION(CheckPointer(pElementMT));
    }
    CONTRACTL_END;

    Module* pModule = pElementMT->GetModule();
    if (!Security::CanCallUnmanagedCode(pModule))
    {
        COMPlusThrow(kArgumentException, IDS_EE_VTRECORD_SECURITY);
    }

    if (pElementMT->IsBlittable())
    {
        // The array is blittable so we can simply copy it.
        _ASSERTE(pComArray);
        SIZE_T elementCount = (*pComArray)->GetNumComponents();
        SIZE_T elemSize     = pElementMT->GetNativeSize();
        memcpyNoGCRefs(oleArray, (*pComArray)->GetDataPtr(), elementCount * elemSize);
    }
    else
    {
        // The array is non blittable so we need to marshal the elements.
        _ASSERTE(pElementMT->GetClass()->HasLayout());
        MarshalNonBlittableRecordArrayComToOle(pComArray, oleArray, pElementMT, fBestFitMapping, fThrowOnUnmappableChar);
    }
}


void OleVariant::ClearRecordArray(void *oleArray, SIZE_T cElements, MethodTable *pElementMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(oleArray));
        PRECONDITION(CheckPointer(pElementMT));
    }
    CONTRACTL_END;

    if (!pElementMT->IsBlittable())
    {
        _ASSERTE(pElementMT->GetClass()->HasLayout());
        ClearNonBlittableRecordArray(oleArray, cElements, pElementMT);
    }
}

/* ------------------------------------------------------------------------- *
 * Mapping routines
 * ------------------------------------------------------------------------- */

VARTYPE OleVariant::GetVarTypeForCVType(CVTypes type)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE map[] = 
    {
        VT_EMPTY,           // CV_EMPTY
        VT_VOID,            // CV_VOID
        VT_BOOL,            // CV_BOOLEAN
        VT_UI2,             // CV_CHAR
        VT_I1,              // CV_I1
        VT_UI1,             // CV_U1
        VT_I2,              // CV_I2
        VT_UI2,             // CV_U2
        VT_I4,              // CV_I4
        VT_UI4,             // CV_U4
        VT_I8,              // CV_I8
        VT_UI8,             // CV_U8
        VT_R4,              // CV_R4
        VT_R8,              // CV_R8
        VT_BSTR,            // CV_STRING
        NO_MAPPING,         // CV_PTR
        VT_DATE,            // CV_DATETIME
        NO_MAPPING,         // CV_TIMESPAN
        VT_DISPATCH,        // CV_OBJECT
        VT_DECIMAL,         // CV_DECIMAL
        VT_CY,              // CV_CURRENCY
        VT_I4,              // CV_ENUM
        VT_ERROR,           // CV_MISSING
        VT_NULL             // CV_NULL
    };

    _ASSERTE(type < (CVTypes) (sizeof(map) / sizeof(map[0])));

    VARTYPE vt = VARTYPE(map[type]);

    if (vt == NO_MAPPING)
        COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);

    return vt;
}

//
// GetCVTypeForVarType returns the COM+ variant type for a given 
// VARTYPE.  This is called by the marshaller in the context of
// a function call.
//

CVTypes OleVariant::GetCVTypeForVarType(VARTYPE vt)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE map[] = 
    {
        CV_EMPTY,           // VT_EMPTY
        CV_NULL,            // VT_NULL
        CV_I2,              // VT_I2
        CV_I4,              // VT_I4
        CV_R4,              // VT_R4
        CV_R8,              // VT_R8
        CV_DECIMAL,         // VT_CY
        CV_DATETIME,        // VT_DATE
        CV_STRING,          // VT_BSTR
        CV_OBJECT,          // VT_DISPATCH
        CV_I4,              // VT_ERROR
        CV_BOOLEAN,         // VT_BOOL
        NO_MAPPING,         // VT_VARIANT
        CV_OBJECT,          // VT_UNKNOWN
        CV_DECIMAL,         // VT_DECIMAL
        NO_MAPPING,         // unused
        CV_I1,              // VT_I1
        CV_U1,              // VT_UI1
        CV_U2,              // VT_UI2
        CV_U4,              // VT_UI4
        CV_I8,              // VT_I8
        CV_U8,              // VT_UI8
        CV_I4,              // VT_INT
        CV_U4,              // VT_UINT
        CV_VOID,            // VT_VOID
        NO_MAPPING,         // VT_HRESULT
        NO_MAPPING,         // VT_PTR
        NO_MAPPING,         // VT_SAFEARRAY
        NO_MAPPING,         // VT_CARRAY
        NO_MAPPING,         // VT_USERDEFINED
        NO_MAPPING,         // VT_LPSTR
        NO_MAPPING,         // VT_LPWSTR
        NO_MAPPING,         // unused
        NO_MAPPING,         // unused
        NO_MAPPING,         // unused
        NO_MAPPING,         // unused
        CV_OBJECT,          // VT_RECORD
    };

    CVTypes type = CV_LAST;

    // Validate the arguments.
    _ASSERTE((vt & VT_BYREF) == 0);

    // Array's map to CV_OBJECT.
    if (vt & VT_ARRAY)
        return CV_OBJECT;

    if (vt > VT_RECORD || (BYTE)(type = (CVTypes) map[vt]) == NO_MAPPING)
        COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_TYPE);

    return type;
} // CVTypes OleVariant::GetCVTypeForVarType()


// GetVarTypeForComVariant retusn the VARTYPE for the contents
// of a COM+ variant.
//
VARTYPE OleVariant::GetVarTypeForComVariant(VariantData *pComVariant)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    CVTypes type = pComVariant->GetType();
    VARTYPE vt;

    vt = pComVariant->GetVT();
    if (vt != VT_EMPTY)
    {
        // This variant was originally unmarshaled from unmanaged, and had the original VT recorded in it.
        // We'll always use that over inference.
        return vt;
    }

    if (type == CV_OBJECT)
    {
        GCX_COOP();

        OBJECTREF obj = pComVariant->GetObjRef();

        // Null objects will be converted to VT_DISPATCH variants with a null
        // IDispatch pointer.
        if (obj == NULL)
            return VT_DISPATCH;

        // Retrieve the object's method table.
        MethodTable *pMT = obj->GetMethodTable();

        // Handle the value class case.
        if (pMT->IsValueClass())
            return VT_RECORD;

        // Handle the array case.
        if (pMT->IsArray())
        {
            vt = GetElementVarTypeForArrayRef((BASEARRAYREF)obj);
            if (vt == VT_ARRAY)
                vt = VT_VARIANT;

            return vt | VT_ARRAY;
        }

        return VT_UNKNOWN;
    }

    return GetVarTypeForCVType(type);
}


VARTYPE OleVariant::GetVarTypeForTypeHandle(TypeHandle type)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    // Handle primitive types.
    CorElementType elemType = type.GetSignatureCorElementType();
    if (elemType <= ELEMENT_TYPE_R8) 
        return GetVarTypeForCVType(COMVariant::CorElementTypeToCVTypes(elemType));

    // Handle objects.
    if (type.IsUnsharedMT()) 
    {
        // We need to make sure the CVClasses table is populated.
        if(GetTypeHandleForCVType(CV_DATETIME) == type)
            return VT_DATE;
        if(GetTypeHandleForCVType(CV_DECIMAL) == type)
            return VT_DECIMAL;
        if (type == TypeHandle(g_pStringClass))
            return VT_BSTR;
        if (type == TypeHandle(g_pObjectClass))
            return VT_VARIANT;
#ifdef _X86_
        if (type == TypeHandle(g_Mscorlib.GetClass(CLASS__INTPTR)))
            return VT_INT;
        if (type == TypeHandle(g_Mscorlib.GetClass(CLASS__UINTPTR)))
            return VT_UINT;
#else
        if (type == TypeHandle(g_Mscorlib.GetClass(CLASS__INTPTR)))
            return VT_I8;
        if (type == TypeHandle(g_Mscorlib.GetClass(CLASS__UINTPTR)))
            return VT_UI8;
#endif

        if (type.IsEnum())
            return GetVarTypeForCVType((CVTypes)type.GetInternalCorElementType());
       
        if (type.GetMethodTable()->IsValueClass())
            return VT_RECORD;


        return VT_UNKNOWN;
    }

    // Handle array's.
    if (!CorTypeInfo::IsArray(elemType))
    {
        // Non interop compatible type.
        COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);
    }

    return VT_ARRAY;
}

//
// GetElementVarTypeForArrayRef returns the safearray variant type for the
// underlying elements in the array.  
//

VARTYPE OleVariant::GetElementVarTypeForArrayRef(BASEARRAYREF pArrayRef) 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    TypeHandle elemTypeHnd = pArrayRef->GetArrayElementTypeHandle();
    return(GetVarTypeForTypeHandle(elemTypeHnd));
}

BOOL OleVariant::IsValidArrayForSafeArrayElementType(BASEARRAYREF *pArrayRef, VARTYPE vtExpected)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
        
    // Retrieve the VARTYPE for the managed array.
    VARTYPE vtActual = GetElementVarTypeForArrayRef(*pArrayRef);

    // If the actual type is the same as the expected type, then the array is valid.
    if (vtActual == vtExpected)
        return TRUE;

    // Check for additional supported VARTYPES.
    switch (vtExpected)
    {
        case VT_I4:
            return vtActual == VT_INT;

        case VT_INT:
            return vtActual == VT_I4;

        case VT_UI4:
            return vtActual == VT_UINT;

        case VT_UINT:
            return vtActual == VT_UI4;

        case VT_UNKNOWN:
            return vtActual == VT_VARIANT || vtActual == VT_DISPATCH;

        case VT_DISPATCH:
            return vtActual == VT_VARIANT;

        case VT_CY:
            return vtActual == VT_DECIMAL;

        default:
            return FALSE;
    }
}


//
// GetArrayClassForVarType returns the element class name and underlying method table
// to use to represent an array with the given variant type.  
//

TypeHandle OleVariant::GetArrayForVarType(VARTYPE vt, TypeHandle elemType, unsigned rank) 
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    CorElementType baseElement = ELEMENT_TYPE_END;
    TypeHandle baseType;
    
    if (!elemType.IsNull() && elemType.IsEnum())
    {
        baseType = elemType;       
    }
    else
    {
        switch (vt)
        {
        case VT_BOOL:
        case VTHACK_WINBOOL:
            baseElement = ELEMENT_TYPE_BOOLEAN;
            break;

        case VTHACK_ANSICHAR:
            baseElement = ELEMENT_TYPE_CHAR;
            break;

        case VT_UI1:
            baseElement = ELEMENT_TYPE_U1;
            break;

        case VT_I1:
            baseElement = ELEMENT_TYPE_I1;
            break;

        case VT_UI2:
            baseElement = ELEMENT_TYPE_U2;
            break;

        case VT_I2:
            baseElement = ELEMENT_TYPE_I2;
            break;

        case VT_UI4:
        case VT_UINT:
        case VT_ERROR:
            if (vt == VT_UI4)
            {
                if (elemType.IsNull() || elemType == TypeHandle(g_pObjectClass))
                {
                baseElement = ELEMENT_TYPE_U4;
                }
                else
                {
                    switch (elemType.AsMethodTable()->GetInternalCorElementType())
                    {
                        case ELEMENT_TYPE_U4:
                            baseElement = ELEMENT_TYPE_U4;
                            break;
                        case ELEMENT_TYPE_U:
                            baseElement = ELEMENT_TYPE_U;
                            break;
                        default:
                            _ASSERTE(0);
                    }
                }
            }
            else
            {
                baseElement = ELEMENT_TYPE_U4;
            }
            break;

        case VT_I4:
        case VT_INT:
            if (vt == VT_I4)
            {
                if (elemType.IsNull() || elemType == TypeHandle(g_pObjectClass))
                {
                    baseElement = ELEMENT_TYPE_I4;
                }
                else
                {
                    switch (elemType.AsMethodTable()->GetInternalCorElementType())
                    {
                        case ELEMENT_TYPE_I4:
                            baseElement = ELEMENT_TYPE_I4;
                            break;
                        case ELEMENT_TYPE_I:
                            baseElement = ELEMENT_TYPE_I;
                            break;
                        default:
                            _ASSERTE(0);
                    }
                }
            }
            else
            {
                baseElement = ELEMENT_TYPE_I4;
            }
            break;

        case VT_I8:
            if (elemType.IsNull() || elemType == TypeHandle(g_pObjectClass))
            {
                baseElement = ELEMENT_TYPE_I8;
            }
            else
            {
                switch (elemType.AsMethodTable()->GetInternalCorElementType())
                {
                    case ELEMENT_TYPE_I8:
                        baseElement = ELEMENT_TYPE_I8;
                        break;
                    case ELEMENT_TYPE_I:
                        baseElement = ELEMENT_TYPE_I;
                        break;
                    default:
                        _ASSERTE(0);
                }
            }
            break;

        case VT_UI8:
            if (elemType.IsNull() || elemType == TypeHandle(g_pObjectClass))
            {
                baseElement = ELEMENT_TYPE_U8;
            }
            else
            {
                switch (elemType.AsMethodTable()->GetInternalCorElementType())
                {
                    case ELEMENT_TYPE_U8:
                        baseElement = ELEMENT_TYPE_U8;
                        break;
                    case ELEMENT_TYPE_U:
                        baseElement = ELEMENT_TYPE_U;
                        break;
                    default:
                        _ASSERTE(0);
                }
            }
            break;

        case VT_R4:
            baseElement = ELEMENT_TYPE_R4;
            break;

        case VT_R8:
            baseElement = ELEMENT_TYPE_R8;
            break;

        case VT_CY:
            baseType = TypeHandle(g_Mscorlib.GetClass(CLASS__DECIMAL));
            break;

        case VT_DATE:
            baseType = TypeHandle(g_Mscorlib.GetClass(CLASS__DATE_TIME));
            break;

        case VT_DECIMAL:
            baseType = TypeHandle(g_Mscorlib.GetClass(CLASS__DECIMAL));
            break;

        case VT_VARIANT:

            //
            // It would be nice if our conversion between SAFEARRAY and
            // array ref were symmetric.  Right now it is not, because a
            // jagged array converted to a SAFEARRAY and back will come
            // back as an array of variants.
            //
            // We could try to detect the case where we can turn a
            // safearray of variants into a jagged array.  Basically we
            // needs to make sure that all of the variants in the array
            // have the same array type.  (And if that is array of
            // variant, we need to look recursively for another layer.)
            //
            // We also needs to check the dimensions of each array stored
            // in the variant to make sure they have the same rank, and
            // this rank is needed to build the correct array class name.
            // (Note that it will be impossible to tell the rank if all
            // elements in the array are NULL.)
            // 


            baseType = TypeHandle(g_Mscorlib.GetClass(CLASS__OBJECT));
            break;

        case VT_BSTR:
        case VT_LPWSTR:
        case VT_LPSTR:
            baseElement = ELEMENT_TYPE_STRING;
            break;

        case VT_DISPATCH:
        case VT_UNKNOWN:
            if (elemType.IsNull())
                baseType = TypeHandle(g_Mscorlib.GetClass(CLASS__OBJECT));
            else
                baseType = elemType;
            break;

        case VT_RECORD:
            _ASSERTE(!elemType.IsNull());   
            baseType = elemType;
            break;

        default:
            COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);
        }
    }

    if (baseType.IsNull())
        baseType = TypeHandle(g_Mscorlib.GetElementType(baseElement));

    _ASSERTE(!baseType.IsNull());
    
    return ClassLoader::LoadArrayTypeThrowing(baseType, rank == 0 ? ELEMENT_TYPE_SZARRAY : ELEMENT_TYPE_ARRAY, rank == 0 ? 1 : rank);
}

//
// GetElementSizeForVarType returns the array element size for the given variant type.
//

UINT OleVariant::GetElementSizeForVarType(VARTYPE vt, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE map[] = 
    {
        0,                      // VT_EMPTY
        0,                      // VT_NULL
        2,                      // VT_I2
        4,                      // VT_I4
        4,                      // VT_R4
        8,                      // VT_R8
        sizeof(CURRENCY),       // VT_CY
        sizeof(DATE),           // VT_DATE
        sizeof(BSTR),           // VT_BSTR
        sizeof(IDispatch*),     // VT_DISPATCH
        sizeof(SCODE),          // VT_ERROR
        sizeof(VARIANT_BOOL),   // VT_BOOL
        sizeof(VARIANT),        // VT_VARIANT
        sizeof(IUnknown*),      // VT_UNKNOWN
        sizeof(DECIMAL),        // VT_DECIMAL
        0,                      // unused
        1,                      // VT_I1
        1,                      // VT_UI1
        2,                      // VT_UI2
        4,                      // VT_UI4
        8,                      // VT_I8
        8,                      // VT_UI8
        4,                      // VT_INT 
        4,                      // VT_UINT
        0,                      // VT_VOID
        sizeof(HRESULT),        // VT_HRESULT
        sizeof(void*),          // VT_PTR
        sizeof(SAFEARRAY*),     // VT_SAFEARRAY
        sizeof(void*),          // VT_CARRAY
        sizeof(void*),          // VT_USERDEFINED
        sizeof(LPSTR),          // VT_LPSTR
        sizeof(LPWSTR),         // VT_LPWSTR
    };

    // Special cases
    switch (vt)
    {
        case VTHACK_WINBOOL:
            return sizeof(BOOL);
            break;
        case VTHACK_ANSICHAR:
            return sizeof(CHAR)*2;  // *2 to leave room for MBCS.
            break;
        default:
            break;
    }

    // VT_ARRAY indicates a safe array which is always sizeof(SAFEARRAY *).
    if (vt & VT_ARRAY)
        return sizeof(SAFEARRAY*);

    if (vt == VTHACK_NONBLITTABLERECORD || vt == VTHACK_BLITTABLERECORD || vt == VT_RECORD)
    {
        PREFIX_ASSUME(pInterfaceMT != NULL);
        return pInterfaceMT->GetNativeSize();
    }
    else if (vt > VT_LPWSTR)
        return 0;
    else
        return map[vt];
}

//
// GetMarshalerForVarType returns the marshaler for the
// given VARTYPE.
//

const OleVariant::Marshaler *OleVariant::GetMarshalerForVarType(VARTYPE vt, BOOL fThrow)
{
    CONTRACT (const OleVariant::Marshaler*)
    {
        if (fThrow) THROWS; else NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;


    switch (vt)
    {
    case VT_BOOL:
        {
            static const Marshaler boolMarshaler = 
            {
                MarshalBoolVariantOleToCom,
                NULL,
                NULL,
                MarshalBoolArrayOleToCom,
                MarshalBoolArrayComToOle,
                NULL
            };

            RETURN &boolMarshaler;
        }

    case VT_DATE:
        {
            static const Marshaler dateMarshaler = 
            {
                MarshalDateVariantOleToCom,
                MarshalDateVariantComToOle,
                MarshalDateVariantOleRefToCom,
                MarshalDateArrayOleToCom,
                MarshalDateArrayComToOle,
                NULL
            };

            RETURN &dateMarshaler;
        }

    case VT_DECIMAL:
        {
            static const Marshaler decimalMarshaler = 
            {
                MarshalDecimalVariantOleToCom,
                MarshalDecimalVariantComToOle,
                MarshalDecimalVariantOleRefToCom,
                NULL, NULL, NULL
            };

            RETURN &decimalMarshaler;
        }


    case VT_BSTR:
        {
            static const Marshaler bstrMarshaler = 
            {
                MarshalBSTRVariantOleToCom,
                MarshalBSTRVariantComToOle,
                NULL,
                MarshalBSTRArrayOleToCom,
                MarshalBSTRArrayComToOle,
                ClearBSTRArray,
            };

            RETURN &bstrMarshaler;
        }

    case VTHACK_NONBLITTABLERECORD:
        {
            static const Marshaler nonblittablerecordMarshaler = 
            {
                NULL,
                NULL,
                NULL,
                MarshalNonBlittableRecordArrayOleToCom,
                MarshalNonBlittableRecordArrayComToOle,
                ClearNonBlittableRecordArray,
            };

            RETURN &nonblittablerecordMarshaler;
        }

    case VT_UNKNOWN:
        {
            static const Marshaler unknownMarshaler = 
            {
                MarshalInterfaceVariantOleToCom,
                MarshalInterfaceVariantComToOle,
                MarshalInterfaceVariantOleRefToCom,
                MarshalInterfaceArrayOleToCom,
                MarshalIUnknownArrayComToOle,
                ClearInterfaceArray
            };

            RETURN &unknownMarshaler;
        }


    case VTHACK_WINBOOL:
        {
            static const Marshaler winboolMarshaler = 
            {
                MarshalWinBoolVariantOleToCom,
                MarshalWinBoolVariantComToOle,
                MarshalWinBoolVariantOleRefToCom,
                MarshalWinBoolArrayOleToCom,
                MarshalWinBoolArrayComToOle,
                NULL
            };
            RETURN &winboolMarshaler;
        }

    case VTHACK_ANSICHAR:
        {
            static const Marshaler ansicharMarshaler = 
            {
                MarshalAnsiCharVariantOleToCom,
                MarshalAnsiCharVariantComToOle,
                MarshalAnsiCharVariantOleRefToCom,
                MarshalAnsiCharArrayOleToCom,
                MarshalAnsiCharArrayComToOle,
                NULL
            };
            RETURN &ansicharMarshaler;
        }

    case VT_LPSTR:
        {
            static const Marshaler lpstrMarshaler = 
            {
                NULL, NULL, NULL,
                MarshalLPSTRArrayOleToCom,
                MarshalLPSTRRArrayComToOle,
                ClearLPSTRArray
            };
            RETURN &lpstrMarshaler;
        }

    case VT_LPWSTR:
        {
            static const Marshaler lpwstrMarshaler = 
            {
                NULL, NULL, NULL,
                MarshalLPWSTRArrayOleToCom,
                MarshalLPWSTRRArrayComToOle,
                ClearLPWSTRArray
            };
            RETURN &lpwstrMarshaler;
        }

    case VT_CARRAY:
    case VT_USERDEFINED:
        if (fThrow)
        {
            COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);
        }
        else
        {
            RETURN NULL;
        }


    case VT_RECORD:
        {
            static const Marshaler recordMarshaler = 
            {
                NULL, NULL, NULL,
                MarshalRecordArrayOleToCom,
                MarshalRecordArrayComToOle,
                ClearRecordArray
            };
            RETURN &recordMarshaler;
        }

    case VTHACK_BLITTABLERECORD:
        RETURN NULL; // Requires no marshaling

    default:
        RETURN NULL;
    }
} // OleVariant::Marshaler *OleVariant::GetMarshalerForVarType()

// Warning! VariantClear's previous contents of pVarOut.
void OleVariant::MarshalOleVariantForObject(OBJECTREF * const & pObj, VARIANT *pOle)
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pObj));        
        PRECONDITION(*pObj == NULL || (IsProtectedByGCFrame (pObj)));

        PRECONDITION(CheckPointer(pOle));
    }
    CONTRACTL_END;
    
    SafeVariantClear(pOle);

#ifdef _DEBUG
    FillMemory(pOle, sizeof(VARIANT),0xdd);
    V_VT(pOle) = VT_EMPTY;
#endif

    // For perf reasons, let's handle the more common and easy cases
    // without transitioning to managed code.
    if (*pObj == NULL)
    {
        // null maps to VT_EMPTY - nothing to do here.
    }
    else
    {


        MethodTable *pMT = (*pObj)->GetMethodTable();
        if (pMT == TheInt32Class())
        {
            V_I4(pOle) = *(LONG*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_I4;
        }
        else if (pMT == g_pStringClass)
        {
            if (*(pObj) == NULL)
            {
                V_BSTR(pOle) = NULL;
            }
            else
            {
                STRINGREF stringRef = (STRINGREF)(*pObj);
                V_BSTR(pOle) = SysAllocStringLen(stringRef->GetBuffer(), stringRef->GetStringLength());
                if (NULL == V_BSTR(pOle))
                    COMPlusThrowOM();
            }
            
            V_VT(pOle) = VT_BSTR;
        }
        else if (pMT == TheInt16Class())
        {
            V_I2(pOle) = *(SHORT*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_I2;
        }
        else if (pMT == TheSByteClass())
        {
            V_I1(pOle) = *(CHAR*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_I1;
        }
        else if (pMT == TheUInt32Class())
        {
            V_UI4(pOle) = *(ULONG*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_UI4;
        }
        else if (pMT == TheUInt16Class())
        {
            V_UI2(pOle) = *(USHORT*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_UI2;
        }
        else if (pMT == TheByteClass())
        {
            V_UI1(pOle) = *(BYTE*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_UI1;
        }
        else if (pMT == TheSingleClass())
        {
            V_R4(pOle) = *(FLOAT*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_R4;
        }
        else if (pMT == TheDoubleClass())
        {
            V_R8(pOle) = *(DOUBLE*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_R8;
        }
        else if (pMT == TheBooleanClass())
        {
            V_BOOL(pOle) = *(U1*)( (*pObj)->GetData() ) ? VARIANT_TRUE : VARIANT_FALSE;
            V_VT(pOle) = VT_BOOL;
        }
        else if (pMT == TheIntPtrClass())
        {
            *(LPVOID*)&(V_INT(pOle)) = *(LPVOID*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_INT;
        }
        else if (pMT == TheUIntPtrClass())
        {
            *(LPVOID*)&(V_UINT(pOle)) = *(LPVOID*)( (*pObj)->GetData() );
            V_VT(pOle) = VT_UINT;
        }
        else
        {
            MethodDescCallSite convertObjectToVariant(METHOD__VARIANT__CONVERT_OBJECT_TO_VARIANT);
        
            VariantData managedVariant;
            FillMemory(&managedVariant, sizeof(managedVariant), 0);
            GCPROTECT_BEGIN_VARIANTDATA(managedVariant)
            {
                ARG_SLOT args[] = { 
                        ObjToArgSlot(*pObj),
                        PtrToArgSlot(&managedVariant),
                        };
                
                convertObjectToVariant.Call(args);
                
                OleVariant::MarshalOleVariantForComVariant(&managedVariant, pOle);
            }
            GCPROTECT_END_VARIANTDATA();
        }


    }
}

void OleVariant::MarshalOleRefVariantForObject(OBJECTREF *pObj, VARIANT *pOle)
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(IsProtectedByGCFrame (pObj));
        PRECONDITION(CheckPointer(pOle));
        PRECONDITION(V_VT(pOle) & VT_BYREF);
    }
    CONTRACTL_END;

    // Let's try to handle the common trivial cases quickly first before
    // running the generalized stuff.
    MethodTable *pMT = (*pObj) == NULL ? NULL : (*pObj)->GetMethodTable();
    if ( (V_VT(pOle) == (VT_BYREF | VT_I4) || V_VT(pOle) == (VT_BYREF | VT_UI4)) && (pMT == TheInt32Class() || pMT == TheUInt32Class()) )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_I4REF(pOle)) = *(LONG*)( (*pObj)->GetData() );
    }
    else if ( (V_VT(pOle) == (VT_BYREF | VT_I2) || V_VT(pOle) == (VT_BYREF | VT_UI2)) && (pMT == TheInt16Class() || pMT == TheUInt16Class()) )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_I2REF(pOle)) = *(SHORT*)( (*pObj)->GetData() );
    }
    else if ( (V_VT(pOle) == (VT_BYREF | VT_I1) || V_VT(pOle) == (VT_BYREF | VT_UI1)) && (pMT == TheSByteClass() || pMT == TheByteClass()) )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_I1REF(pOle)) = *(CHAR*)( (*pObj)->GetData() );
    }
    else if ( V_VT(pOle) == (VT_BYREF | VT_R4) && pMT == TheSingleClass() )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_R4REF(pOle)) = *(FLOAT*)( (*pObj)->GetData() );
    }
    else if ( V_VT(pOle) == (VT_BYREF | VT_R8) && pMT == TheDoubleClass() )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_R8REF(pOle)) = *(DOUBLE*)( (*pObj)->GetData() );
    }
    else if ( V_VT(pOle) == (VT_BYREF | VT_BOOL) && pMT == TheBooleanClass() )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(V_BOOLREF(pOle)) =  ( *(U1*)( (*pObj)->GetData() ) ) ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else if ( (V_VT(pOle) == (VT_BYREF | VT_INT) || V_VT(pOle) == (VT_BYREF | VT_UINT)) && (pMT == TheIntPtrClass() || pMT == TheUIntPtrClass()) )
    {
        // deallocation of old value optimized away since there's nothing to
        // deallocate for this vartype.

        *(LPVOID*)(V_INTREF(pOle)) = *(LPVOID*)( (*pObj)->GetData() );
    }
    else if ( V_VT(pOle) == (VT_BYREF | VT_BSTR) && pMT == g_pStringClass )
    {


        if (*(V_BSTRREF(pOle)))
        {
            SysFreeString(*(V_BSTRREF(pOle)));
            *(V_BSTRREF(pOle)) = NULL;
        }
        STRINGREF stringRef = (STRINGREF)(*pObj);

        if (stringRef == NULL)
        {
            *(V_BSTRREF(pOle)) = NULL;
        }
        else
        {
            *(V_BSTRREF(pOle)) = SysAllocStringLen(stringRef->GetBuffer(), stringRef->GetStringLength());
            if (*(V_BSTRREF(pOle)) == NULL)
            {
                COMPlusThrowOM();
            }
        }


    }
    else
    {
        MethodDescCallSite castVariant(METHOD__VARIANT__CAST_VARIANT);

        VARIANT vtmp;
        VARTYPE vt = V_VT(pOle) & ~VT_BYREF;


        ExtractContentsFromByrefVariant(pOle, &vtmp);
        SafeVariantClear(&vtmp);
    
        if (vt == VT_VARIANT)
        {
            // Since variants can contain any VARTYPE we simply convert the object to 
            // a variant and stuff it back into the byref variant.
            MarshalOleVariantForObject(pObj, &vtmp);
            InsertContentsIntoByrefVariant(&vtmp, pOle);
        }
        else if (vt & VT_ARRAY)
        {
            // Since the marshal cast helper does not support array's the best we can do
            // is marshal the object back to a variant and hope it is of the right type.
            // If it is not then we must throw an exception.
            MarshalOleVariantForObject(pObj, &vtmp);
            if (V_VT(&vtmp) != vt)
                COMPlusThrow(kInvalidCastException, IDS_EE_CANNOT_COERCE_BYREF_VARIANT);
            InsertContentsIntoByrefVariant(&vtmp, pOle);
        }
        else
        {
            // The variant is not an array so we can use the marshal cast helper
            // to coerce the object to the proper type.
            VariantData vd;
            FillMemory(&vd, sizeof(vd), 0);
            GCPROTECT_BEGIN_VARIANTDATA(vd);
            {
                if ( (*pObj) == NULL &&
                     (vt == VT_BSTR ||
                      vt == VT_DISPATCH ||
                      vt == VT_UNKNOWN ||
                      vt == VT_PTR ||
                      vt == VT_CARRAY ||
                      vt == VT_SAFEARRAY ||
                      vt == VT_LPSTR ||
                      vt == VT_LPWSTR) )
                {
                    // Have to handle this specially since the managed variant
                    // conversion will return a VT_EMPTY which isn't what we want.
                    V_VT(&vtmp) = vt;
                    V_UNKNOWN(&vtmp) = NULL;
                }
                else
                {
                    ARG_SLOT args[3];
                    args[0] = ObjToArgSlot(*pObj);
                    args[1] = (ARG_SLOT)vt;
                    args[2] = PtrToArgSlot(&vd);
                    castVariant.Call(args);
                    OleVariant::MarshalOleVariantForComVariant(&vd, &vtmp);
                }
                // If the variant types are still not the same then call VariantChangeType to
                // try and coerse them.
                if (V_VT(&vtmp) != vt)
                {
                    COMPlusThrow(kInvalidCastException, IDS_EE_CANNOT_COERCE_BYREF_VARIANT);
                }
                else
                {
                    InsertContentsIntoByrefVariant(&vtmp, pOle);
                }
            }
            GCPROTECT_END_VARIANTDATA();
        }
    }
}

void OleVariant::MarshalObjectForOleVariant(const VARIANT * pOle, OBJECTREF * const & pObj)
{        
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pOle));
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(*pObj == NULL || (IsProtectedByGCFrame (pObj)));
    }
    CONTRACT_END;


    // if V_ISBYREF(pOle) and V_BYREF(pOle) is null then we have a problem,
    // unless we're dealing with VT_EMPTY or VT_NULL in which case that is ok??
    VARTYPE vt = V_VT(pOle) & ~VT_BYREF;
    if (V_ISBYREF(pOle) && !V_BYREF(pOle) && !(vt == VT_EMPTY || vt == VT_NULL))
        COMPlusThrow(kArgumentException, IDS_EE_INVALID_OLE_VARIANT);

    switch (V_VT(pOle))
    {
        case VT_EMPTY:
            SetObjectReference( pObj,
                                NULL,
                                GetAppDomain() );
            break;

        case VT_I4:
        case VT_INT:
            SetObjectReference( pObj,
                                FastAllocateObject(TheInt32Class()),
                                GetAppDomain() );
            *(LONG*)((*pObj)->GetData()) = V_I4(pOle);
            break;

        case VT_BYREF|VT_I4:
        case VT_BYREF|VT_INT:
            SetObjectReference( pObj,
                                FastAllocateObject(TheInt32Class()),
                                GetAppDomain() );
            *(LONG*)((*pObj)->GetData()) = *(V_I4REF(pOle));
            break;

        case VT_UI4:
        case VT_UINT:
            SetObjectReference( pObj,
                                FastAllocateObject(TheUInt32Class()),
                                GetAppDomain() );
            *(ULONG*)((*pObj)->GetData()) = V_UI4(pOle);
            break;

        case VT_BYREF|VT_UI4:
        case VT_BYREF|VT_UINT:
            SetObjectReference( pObj,
                                FastAllocateObject(TheUInt32Class()),
                                GetAppDomain() );
            *(ULONG*)((*pObj)->GetData()) = *(V_UI4REF(pOle));
            break;

        case VT_I2:
            SetObjectReference( pObj,
                                FastAllocateObject(TheInt16Class()),
                                GetAppDomain() );
            (*(SHORT*)((*pObj)->GetData())) = V_I2(pOle);
            break;

        case VT_BYREF|VT_I2:
            SetObjectReference( pObj,
                                FastAllocateObject(TheInt16Class()),
                                GetAppDomain() );
            *(SHORT*)((*pObj)->GetData()) = *(V_I2REF(pOle));
            break;

        case VT_UI2:
            SetObjectReference( pObj,
                                FastAllocateObject(TheUInt16Class()),
                                GetAppDomain() );
            *(USHORT*)((*pObj)->GetData()) = V_UI2(pOle);
            break;

        case VT_BYREF|VT_UI2:
            SetObjectReference( pObj,
                                FastAllocateObject(TheUInt16Class()),
                                GetAppDomain() );
            *(USHORT*)((*pObj)->GetData()) = *(V_UI2REF(pOle));
            break;

        case VT_I1:
            SetObjectReference( pObj,
                                FastAllocateObject(TheSByteClass()),
                                GetAppDomain() );
            *(CHAR*)((*pObj)->GetData()) = V_I1(pOle);
            break;

        case VT_BYREF|VT_I1:
            SetObjectReference( pObj,
                                FastAllocateObject(TheSByteClass()),
                                GetAppDomain() );
            *(CHAR*)((*pObj)->GetData()) = *(V_I1REF(pOle));
            break;

        case VT_UI1:
            SetObjectReference( pObj,
                                FastAllocateObject(TheByteClass()),
                                GetAppDomain() );
            *(BYTE*)((*pObj)->GetData()) = V_UI1(pOle);
            break;

        case VT_BYREF|VT_UI1:
            SetObjectReference( pObj,
                                FastAllocateObject(TheByteClass()),
                                GetAppDomain() );
            *(BYTE*)((*pObj)->GetData()) = *(V_UI1REF(pOle));
            break;

        case VT_R4:
            SetObjectReference( pObj,
                                FastAllocateObject(TheSingleClass()),
                                GetAppDomain() );
            *(FLOAT*)((*pObj)->GetData()) = V_R4(pOle);
            break;

        case VT_BYREF|VT_R4:
            SetObjectReference( pObj,
                                FastAllocateObject(TheSingleClass()),
                                GetAppDomain() );
            *(FLOAT*)((*pObj)->GetData()) = *(V_R4REF(pOle));
            break;

        case VT_R8:
            SetObjectReference( pObj,
                                FastAllocateObject(TheDoubleClass()),
                                GetAppDomain() );
            *(DOUBLE*)((*pObj)->GetData()) = V_R8(pOle);
            break;

        case VT_BYREF|VT_R8:
            SetObjectReference( pObj,
                                FastAllocateObject(TheDoubleClass()),
                                GetAppDomain() );
            *(DOUBLE*)((*pObj)->GetData()) = *(V_R8REF(pOle));
            break;

        case VT_BOOL:
            SetObjectReference( pObj,
                                FastAllocateObject(TheBooleanClass()),
                                GetAppDomain() );
            *(VARIANT_BOOL*)((*pObj)->GetData()) = V_BOOL(pOle) ? 1 : 0;
            break;

        case VT_BYREF|VT_BOOL:
            SetObjectReference( pObj,
                                FastAllocateObject(TheBooleanClass()),
                                GetAppDomain() );
            *(VARIANT_BOOL*)((*pObj)->GetData()) = *(V_BOOLREF(pOle)) ? 1 : 0;
            break;

        case VT_BSTR:
            SetObjectReference( pObj,
                                V_BSTR(pOle) ? COMString::NewString(V_BSTR(pOle), SysStringLen(V_BSTR(pOle))) : NULL,
                                GetAppDomain() );
            break;

        case VT_BYREF|VT_BSTR:
            SetObjectReference( pObj,
                                *(V_BSTRREF(pOle)) ? COMString::NewString(*(V_BSTRREF(pOle)), SysStringLen(*(V_BSTRREF(pOle)))) : NULL,
                                GetAppDomain() );
            break;

        default:
            {
                MethodDescCallSite convertVariantToObject(METHOD__VARIANT__CONVERT_VARIANT_TO_OBJECT);
            
                VariantData managedVariant;
                FillMemory(&managedVariant, sizeof(managedVariant), 0);
                GCPROTECT_BEGIN_VARIANTDATA(managedVariant)
                {
                    OleVariant::MarshalComVariantForOleVariant((VARIANT*)pOle, &managedVariant);    
                    ARG_SLOT args[] = { PtrToArgSlot(&managedVariant) };
                    SetObjectReference( pObj, 
                                        convertVariantToObject.Call_RetOBJECTREF(args),
                                        GetAppDomain() );
                }
                GCPROTECT_END_VARIANTDATA();
            }
        }
        RETURN;
    }

/* ------------------------------------------------------------------------- *
 * Byref variant manipulation helpers.
 * ------------------------------------------------------------------------- */

void OleVariant::ExtractContentsFromByrefVariant(VARIANT *pByrefVar, VARIANT *pDestVar)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pByrefVar));
        PRECONDITION(CheckPointer(pDestVar));
    }
    CONTRACT_END;

    VARTYPE vt = V_VT(pByrefVar) & ~VT_BYREF;

    // VT_BYREF | VT_EMPTY is not a valid combination.
    if (vt == 0 || vt == VT_EMPTY)
        COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);

    switch (vt)
    {
        case VT_RECORD:
        {
            // VT_RECORD's are weird in that regardless of is the VT_BYREF flag is set or not
            // they have the same internal representation.
            V_RECORD(pDestVar) = V_RECORD(pByrefVar);
            V_RECORDINFO(pDestVar) = V_RECORDINFO(pByrefVar);

            // Set the variant type of the destination variant.
            V_VT(pDestVar) = vt;

            break;
        }

        case VT_VARIANT:
        {
            // A byref variant is not allowed to contain a byref variant.
            if (V_ISBYREF(V_VARIANTREF(pByrefVar)))
                COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);

            // Copy the variant that the byref variant points to into the destination variant.
            // This will replace the VARTYPE of pDestVar with the VARTYPE of the VARIANT being 
            // pointed to.
            memcpyNoGCRefs(pDestVar, V_VARIANTREF(pByrefVar), sizeof(VARIANT));
            break;
        }

        case VT_DECIMAL:
        {
            // Copy the value that the byref variant points to into the destination variant.
            // Decimal's are special in that they occupy the 16 bits of padding between the 
            // VARTYPE and the intVal field.
            memcpyNoGCRefs(&V_DECIMAL(pDestVar), V_DECIMALREF(pByrefVar), sizeof(DECIMAL));

            // Set the variant type of the destination variant.
            V_VT(pDestVar) = vt;

            break;
        }

        default:
        {
            // Copy the value that the byref variant points to into the destination variant.
            SIZE_T sz = OleVariant::GetElementSizeForVarType(vt, NULL);
            memcpyNoGCRefs(&V_INT(pDestVar), V_INTREF(pByrefVar), sz);
 
            // Set the variant type of the destination variant.
            V_VT(pDestVar) = vt;
           
            break;
        }
    }
    
    RETURN;
}

void OleVariant::InsertContentsIntoByrefVariant(VARIANT *pSrcVar, VARIANT *pByrefVar)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pByrefVar));
        PRECONDITION(CheckPointer(pSrcVar));
    }
    CONTRACT_END;

    _ASSERTE(V_VT(pSrcVar) == (V_VT(pByrefVar) & ~VT_BYREF) || V_VT(pByrefVar) == (VT_BYREF | VT_VARIANT));


    VARTYPE vt = V_VT(pByrefVar) & ~VT_BYREF;

    // VT_BYREF | VT_EMPTY is not a valid combination.
    if (vt == 0 || vt == VT_EMPTY)
        COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);

    switch (vt)
    {
        case VT_RECORD:
        {
            // VT_RECORD's are weird in that regardless of is the VT_BYREF flag is set or not
            // they have the same internal representation.
            V_RECORD(pByrefVar) = V_RECORD(pSrcVar);
            V_RECORDINFO(pByrefVar) = V_RECORDINFO(pSrcVar);
            break;
        }

        case VT_VARIANT:
        {
            // Copy the variant that the byref variant points to into the destination variant.
            memcpyNoGCRefs(V_VARIANTREF(pByrefVar), pSrcVar, sizeof(VARIANT));
            break;
        }

        case VT_DECIMAL:
        {
            // Copy the value inside the source variant into the location pointed to by the byref variant.
            memcpyNoGCRefs(V_DECIMALREF(pByrefVar), &V_DECIMAL(pSrcVar), sizeof(DECIMAL));
            break;
        }

        default:
        {
            // Copy the value inside the source variant into the location pointed to by the byref variant.

            SIZE_T sz = OleVariant::GetElementSizeForVarType(vt, NULL);
            memcpyNoGCRefs(V_INTREF(pByrefVar), &V_INT(pSrcVar), sz);
            break;
        }
    }
    RETURN;
}

void OleVariant::CreateByrefVariantForVariant(VARIANT *pSrcVar, VARIANT *pByrefVar)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pByrefVar));
        PRECONDITION(CheckPointer(pSrcVar));
    }
    CONTRACT_END;

    // Set the type of the byref variant based on the type of the source variant.
    VARTYPE vt = V_VT(pSrcVar);

    // VT_BYREF | VT_EMPTY is not a valid combination.
    if (vt == 0 || vt == VT_EMPTY)
        COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);

    switch (vt)
    {
        case VT_RECORD:
        {
            // VT_RECORD's are weird in that regardless of is the VT_BYREF flag is set or not
            // they have the same internal representation.
            V_RECORD(pByrefVar) = V_RECORD(pSrcVar);
            V_RECORDINFO(pByrefVar) = V_RECORDINFO(pSrcVar);
            break;
        }

        case VT_VARIANT:
        {
            V_VARIANTREF(pByrefVar) = pSrcVar;
            break;
        }

        case VT_DECIMAL:
        {
            V_DECIMALREF(pByrefVar) = &V_DECIMAL(pSrcVar);
            break;
        }

        default:
        {
            V_INTREF(pByrefVar) = &V_INT(pSrcVar);
            break;
        }
    }

    V_VT(pByrefVar) = vt | VT_BYREF;

    RETURN;
}

/* ------------------------------------------------------------------------- *
 * Variant marshaling
 * ------------------------------------------------------------------------- */

//
// MarshalComVariantForOleVariant copies the contents of the OLE variant from 
// the COM variant.
//

void OleVariant::MarshalComVariantForOleVariant(VARIANT *pOle, VariantData *pCom)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pOle));
        PRECONDITION(CheckPointer(pCom));        
    }
    CONTRACTL_END;
    
    BOOL byref = V_ISBYREF(pOle);
    VARTYPE vt = V_VT(pOle) & ~VT_BYREF;

    if ((vt & ~VT_ARRAY) >= 128 )
        COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);

    if (byref && !V_BYREF(pOle) && !(vt == VT_EMPTY || vt == VT_NULL))
        COMPlusThrow(kArgumentException, IDS_EE_INVALID_OLE_VARIANT);

    if (byref && vt == VT_VARIANT)
    {
        pOle = V_VARIANTREF(pOle);
        byref = V_ISBYREF(pOle);
        vt = V_VT(pOle) & ~VT_BYREF;

        // Byref VARIANTS are not allowed to be nested.
        if (byref)
            COMPlusThrow(kInvalidOleVariantTypeException, IDS_EE_INVALID_OLE_VARIANT);
    }
    
    CVTypes cvt = GetCVTypeForVarType(vt);
    const Marshaler *marshal = GetMarshalerForVarType(vt, TRUE);

    pCom->SetType(cvt);
    pCom->SetVT(vt); // store away VT for return trip. 
    if (marshal == NULL || (byref 
                            ? marshal->OleRefToComVariant == NULL 
                            : marshal->OleToComVariant == NULL))
    {
        if (cvt==CV_EMPTY || cvt==CV_NULL) 
        {
            if (V_ISBYREF(pOle))
            {
                // Must set ObjectRef field of Variant to a specific instance.
                COMVariant::NewVariant(pCom, CV_U4, (INT32)(size_t)V_BYREF(pOle));
            }
            else
            {
                COMVariant::NewVariant(pCom, cvt, NULL);
            }
        }
        else 
        {
            pCom->SetObjRef(NULL);
            if (byref)
            {
                INT64 data = 0;
                CopyMemory(&data, V_R8REF(pOle), GetElementSizeForVarType(vt, NULL));
                pCom->SetData(&data);
            }
            else
                pCom->SetData(&V_R8(pOle));
        }
    }
    else
    {
        if (byref)
            marshal->OleRefToComVariant(pOle, pCom);
        else
            marshal->OleToComVariant(pOle, pCom);
    }
}

//
// MarshalOleVariantForComVariant copies the contents of the OLE variant from 
// the COM variant.
//

void OleVariant::MarshalOleVariantForComVariant(VariantData *pCom, VARIANT *pOle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCom));
        PRECONDITION(CheckPointer(pOle));
    }
    CONTRACTL_END;

    SafeVariantClear(pOle);

    VariantEmptyHolder veh;
    veh = pOle;

    VARTYPE vt = GetVarTypeForComVariant(pCom);
    V_VT(pOle) = vt;
    
    const Marshaler *marshal = GetMarshalerForVarType(vt, TRUE);

    if (marshal == NULL || marshal->ComToOleVariant == NULL)
    {
        *(INT64*)&V_R8(pOle) = *(INT64*)pCom->GetData();
    }
    else
    {
        marshal->ComToOleVariant(pCom, pOle);
    }

    veh.SuppressRelease();
}

//
// MarshalOleRefVariantForComVariant copies the contents of the OLE variant from 
// the COM variant.
//

void OleVariant::MarshalOleRefVariantForComVariant(VariantData *pCom, VARIANT *pOle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pCom));
        PRECONDITION(CheckPointer(pOle));
        PRECONDITION(V_VT(pOle) & VT_BYREF);
    }
    CONTRACTL_END;

    VARIANT vtmp;
    VARTYPE InitVarType = V_VT(pOle) & ~VT_BYREF;

    // Clear the contents of the original variant.
    ExtractContentsFromByrefVariant(pOle, &vtmp);
    SafeVariantClear(&vtmp);

    // Convert the managed variant to an unmanaged variant.
    OleVariant::MarshalOleVariantForComVariant(pCom, &vtmp);

    // Copy the converted variant into the original variant.
    if (V_VT(&vtmp) != InitVarType)
    {
        if (InitVarType == VT_VARIANT)
        {
            // Since variants can contain any VARTYPE we simply convert the managed 
            // variant to an OLE variant and stuff it back into the byref variant.
            InsertContentsIntoByrefVariant(&vtmp, pOle);
        }
        else
        {
            COMPlusThrow(kInvalidCastException, IDS_EE_CANNOT_COERCE_BYREF_VARIANT);
        }
    }
    else
    {
        // The type is the same so we can simply copy the contents.
        InsertContentsIntoByrefVariant(&vtmp, pOle);
    }
}



void OleVariant::MarshalInterfaceArrayComToOleHelper(BASEARRAYREF *pComArray, void *oleArray,
                                                     MethodTable *pElementMT, BOOL bDefaultIsDispatch)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pComArray));
        PRECONDITION(CheckPointer(oleArray));
    }
    CONTRACTL_END;
   
    ASSERT_PROTECTED(pComArray);
    SIZE_T elementCount = (*pComArray)->GetNumComponents();


    // Determine the start and the end of the data in the OLE array.
    IUnknown **pOle = (IUnknown **) oleArray;
    IUnknown **pOleEnd = pOle + elementCount;

    // Retrieve the start of the data in the managed array.
    BASEARRAYREF unprotectedArray = *pComArray;
    OBJECTREF *pCom = (OBJECTREF *) unprotectedArray->GetDataPtr();

    OBJECTREF TmpObj = NULL;
    GCPROTECT_BEGIN(TmpObj)
    {
        if (pElementMT)
        {
            while (pOle < pOleEnd)
            {
                TmpObj = *pCom++;

                IUnknown *unk;
                if (TmpObj == NULL)
                    unk = NULL;
                else
                    unk = GetComIPFromObjectRef(&TmpObj);

                *pOle++ = unk;

                if (*(void **)&unprotectedArray != *(void **)&*pComArray)
                {
                    SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
                    unprotectedArray = *pComArray;
                    pCom = (OBJECTREF *) (unprotectedArray->GetAddress() + currentOffset);
                }
            }
        }
        else
        {

            while (pOle < pOleEnd)
            {
                TmpObj = *pCom++;

                IUnknown *unk;
                if (TmpObj == NULL)
                    unk = NULL;
                else
                    unk = GetComIPFromObjectRef(&TmpObj);

                *pOle++ = unk;

                if (*(void **)&unprotectedArray != *(void **)&*pComArray)
                {
                    SIZE_T currentOffset = ((BYTE *)pCom) - (*(Object **) &unprotectedArray)->GetAddress();
                    unprotectedArray = *pComArray;
                    pCom = (OBJECTREF *) (unprotectedArray->GetAddress() + currentOffset);
                }
            }
        }
    }
    GCPROTECT_END();
}

void OleVariant::MarshalBSTRArrayComToOleWrapper(BASEARRAYREF *pComArray, void *oleArray)
{
    WRAPPER_CONTRACT;
    
    MarshalBSTRArrayComToOle(pComArray, oleArray, NULL, TRUE, FALSE);
}

void OleVariant::MarshalBSTRArrayOleToComWrapper(void *oleArray, BASEARRAYREF *pComArray)
{
    WRAPPER_CONTRACT;
    
    MarshalBSTRArrayOleToCom(oleArray, pComArray, NULL);
}

void OleVariant::ClearBSTRArrayWrapper(void *oleArray, SIZE_T cElements)
{
    WRAPPER_CONTRACT;
    
    ClearBSTRArray(oleArray, cElements, NULL);
}

// Used by customer checked build to test validity of VARIANT

BOOL OleVariant::CheckVariant(VARIANT* pOle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pOle));
    }
    CONTRACTL_END;
    
    BOOL bValidVariant = FALSE;

    // We need a try/catch here since VariantCopy could cause an AV if the VARIANT isn't valid.
    EX_TRY
    {   
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    return bValidVariant;
}



