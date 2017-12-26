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
/*============================================================
**
** Header:  Custom marshaler information used when marshaling
**          a parameter with a custom marshaler. 
**  
**                                                        </STRIP>
===========================================================*/

#include "common.h"
#include "custommarshalerinfo.h"
#include "comstring.h"
#include "mlinfo.h"
#include "mdaassistantsptr.h"

//==========================================================================
// Implementation of the custom marshaler info class.
//==========================================================================

CustomMarshalerInfo::CustomMarshalerInfo(BaseDomain *pDomain, TypeHandle hndCustomMarshalerType, TypeHandle hndManagedType, LPCUTF8 strCookie, DWORD cCookieStrBytes)
: m_NativeSize(0)
, m_hndManagedType(hndManagedType)
, m_hndCustomMarshaler(NULL)
, m_pMarshalNativeToManagedMD(NULL)
, m_pMarshalManagedToNativeMD(NULL)
, m_pCleanUpNativeDataMD(NULL)
, m_pCleanUpManagedDataMD(NULL)
, m_bDataIsByValue(FALSE)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDomain));
    }
    CONTRACTL_END;


    // Make sure the custom marshaller implements ICustomMarshaler.
    if (!hndCustomMarshalerType.GetMethodTable()->CanCastToNonVariantInterface(g_Mscorlib.GetClass(CLASS__ICUSTOM_MARSHALER)))
    {
        DefineFullyQualifiedNameForClassW()
        COMPlusThrow(kApplicationException,
                     IDS_EE_ICUSTOMMARSHALERNOTIMPL,
                     GetFullyQualifiedNameForClassW(hndCustomMarshalerType.GetClass()));
    }

    // Determine if this type is a value class.
    m_bDataIsByValue = m_hndManagedType.GetClass()->IsValueClass();

    // Custom marshalling of value classes is not currently supported.
    if (m_bDataIsByValue)
        COMPlusThrow(kNotSupportedException, L"NotSupported_ValueClassCM");

    // Run the <clinit> on the marshaler since it might not have run yet.
    hndCustomMarshalerType.GetMethodTable()->EnsureInstanceActive();
    hndCustomMarshalerType.GetMethodTable()->CheckRunClassInitThrowing();

    // Create a COM+ string that will contain the string cookie.
    STRINGREF CookieStringObj = COMString::NewString(strCookie, cCookieStrBytes);
    GCPROTECT_BEGIN(CookieStringObj);

    // Load the method desc's for all the methods in the ICustomMarshaler interface.
    m_pMarshalNativeToManagedMD = GetCustomMarshalerMD(CustomMarshalerMethods_MarshalNativeToManaged, hndCustomMarshalerType);
    m_pMarshalManagedToNativeMD = GetCustomMarshalerMD(CustomMarshalerMethods_MarshalManagedToNative, hndCustomMarshalerType);
    m_pCleanUpNativeDataMD = GetCustomMarshalerMD(CustomMarshalerMethods_CleanUpNativeData, hndCustomMarshalerType);
    m_pCleanUpManagedDataMD = GetCustomMarshalerMD(CustomMarshalerMethods_CleanUpManagedData, hndCustomMarshalerType);

    // Load the method desc for the static method to retrieve the instance.
    MethodDesc *pGetCustomMarshalerMD = GetCustomMarshalerMD(CustomMarshalerMethods_GetInstance, hndCustomMarshalerType);
    MethodDescCallSite getCustomMarshaler(pGetCustomMarshalerMD, (OBJECTREF*)&CookieStringObj);

    pGetCustomMarshalerMD->EnsureActive();

    // Prepare the arguments that will be passed to GetCustomMarshaler.
    ARG_SLOT GetCustomMarshalerArgs[] = { 
        ObjToArgSlot(CookieStringObj)
    };

    // Call the GetCustomMarshaler method to retrieve the custom marshaler to use.
    OBJECTREF CustomMarshalerObj = getCustomMarshaler.Call_RetOBJECTREF(GetCustomMarshalerArgs);
    if (!CustomMarshalerObj)
    {
        DefineFullyQualifiedNameForClassW()
        COMPlusThrow(kApplicationException,
                     IDS_EE_NOCUSTOMMARSHALER,
                     GetFullyQualifiedNameForClassW(hndCustomMarshalerType.GetClass()));
    }
    m_hndCustomMarshaler = pDomain->CreateHandle(CustomMarshalerObj);

    // Retrieve the size of the native data.
    if (m_bDataIsByValue)
    {
        _ASSERTE(!"Value classes are not yet supported by the custom marshaler!");
    }
    else
    {
        m_NativeSize = sizeof(void *);
    }

    GCPROTECT_END();
}


CustomMarshalerInfo::~CustomMarshalerInfo()
{
    WRAPPER_CONTRACT;
    
    if (m_hndCustomMarshaler)
    {
        DestroyHandle(m_hndCustomMarshaler);
        m_hndCustomMarshaler = NULL;
    }
}


void *CustomMarshalerInfo::operator new(size_t size, LoaderHeap *pHeap)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pHeap));
    }
    CONTRACTL_END;
    
    return pHeap->AllocMem(sizeof(CustomMarshalerInfo));
}


void CustomMarshalerInfo::operator delete(void *pMem)
{
    // Instances of this class are always allocated on the loader heap so
    // the delete operator has nothing to do.
    LEAF_CONTRACT;
}


OBJECTREF CustomMarshalerInfo::InvokeMarshalNativeToManagedMeth(void *pNative)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNative, NULL_OK));
    }
    CONTRACTL_END;
    
    if (!pNative)
        return NULL;

    MethodDescCallSite marshalNativeToManaged(m_pMarshalNativeToManagedMD, m_hndCustomMarshaler);
    
    ARG_SLOT Args[] = {
        ObjToArgSlot(ObjectFromHandle(m_hndCustomMarshaler)),
        PtrToArgSlot(pNative)
    };

    return marshalNativeToManaged.Call_RetOBJECTREF(Args);
}


void *CustomMarshalerInfo::InvokeMarshalManagedToNativeMeth(OBJECTREF MngObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    void *RetVal = NULL;

    if (!MngObj)
        return NULL;

    GCPROTECT_BEGIN (MngObj);
    MethodDescCallSite marshalManagedToNative(m_pMarshalManagedToNativeMD, m_hndCustomMarshaler);

    ARG_SLOT Args[] = {
        ObjToArgSlot(ObjectFromHandle(m_hndCustomMarshaler)),
        ObjToArgSlot(MngObj)
    };

    RetVal = marshalManagedToNative.Call_RetLPVOID(Args);
    GCPROTECT_END ();
    
    return RetVal;
}


void CustomMarshalerInfo::InvokeCleanUpNativeMeth(void *pNative)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNative, NULL_OK));
    }
    CONTRACTL_END;

    if (!pNative)
        return;

    MethodDescCallSite cleanUpNativeData(m_pCleanUpNativeDataMD, m_hndCustomMarshaler);

    ARG_SLOT Args[] = {
        ObjToArgSlot(ObjectFromHandle(m_hndCustomMarshaler)),
        PtrToArgSlot(pNative)
    };

    cleanUpNativeData.Call(Args);
}


void CustomMarshalerInfo::InvokeCleanUpManagedMeth(OBJECTREF MngObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (!MngObj)
        return;

    GCPROTECT_BEGIN (MngObj);
    MethodDescCallSite cleanUpManagedData(m_pCleanUpManagedDataMD, m_hndCustomMarshaler);

    ARG_SLOT Args[] = {
        ObjToArgSlot(ObjectFromHandle(m_hndCustomMarshaler)),
        ObjToArgSlot(MngObj)
    };

    cleanUpManagedData.Call(Args);
    GCPROTECT_END ();
}


MethodDesc *CustomMarshalerInfo::GetCustomMarshalerMD(EnumCustomMarshalerMethods Method, TypeHandle hndCustomMarshalertype)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;


    MethodTable *pMT = hndCustomMarshalertype.AsMethodTable();

    _ASSERTE(pMT->CanCastToNonVariantInterface(g_Mscorlib.GetClass(CLASS__ICUSTOM_MARSHALER)));

    MethodDesc *pMD = NULL;

    switch (Method)
    {
        case CustomMarshalerMethods_MarshalNativeToManaged:
            pMD = pMT->GetMethodDescForInterfaceMethod(
                       g_Mscorlib.GetMethod(METHOD__ICUSTOM_MARSHALER__MARSHAL_NATIVE_TO_MANAGED));  
            break;
        case CustomMarshalerMethods_MarshalManagedToNative:
            pMD = pMT->GetMethodDescForInterfaceMethod(
                       g_Mscorlib.GetMethod(METHOD__ICUSTOM_MARSHALER__MARSHAL_MANAGED_TO_NATIVE));
            break;
        case CustomMarshalerMethods_CleanUpNativeData:
            pMD = pMT->GetMethodDescForInterfaceMethod(
                        g_Mscorlib.GetMethod(METHOD__ICUSTOM_MARSHALER__CLEANUP_NATIVE_DATA));
            break;

        case CustomMarshalerMethods_CleanUpManagedData:
            pMD = pMT->GetMethodDescForInterfaceMethod(
                        g_Mscorlib.GetMethod(METHOD__ICUSTOM_MARSHALER__CLEANUP_MANAGED_DATA));
            break;
        case CustomMarshalerMethods_GetNativeDataSize:
            pMD = pMT->GetMethodDescForInterfaceMethod(
                        g_Mscorlib.GetMethod(METHOD__ICUSTOM_MARSHALER__GET_NATIVE_DATA_SIZE));
            break;
        case CustomMarshalerMethods_GetInstance:
            // Must look this up by name since it's static
            pMD = pMT->GetClass()->FindMethod("GetInstance", &gsig_SM_Str_RetICustomMarshaler);
            if (!pMD)
            {
                DefineFullyQualifiedNameForClassW()
                COMPlusThrow(kApplicationException,
                             IDS_EE_GETINSTANCENOTIMPL,
                             GetFullyQualifiedNameForClassW(pMT->GetClass()));
            }
            break;
        default:
            _ASSERTE(!"Unknown custom marshaler method");
    }

    _ASSERTE(pMD && "Unable to find specified CustomMarshaler method");

    // Ensure that the value types in the signature are loaded.
    MetaSig::EnsureSigValueTypesLoaded(pMD);

    // Return the specified method desc.
    return pMD;
}


//==========================================================================
// Implementation of the custom marshaler hashtable helper.
//==========================================================================

EEHashEntry_t * EECMHelperHashtableHelper::AllocateEntry(EECMHelperHashtableKey *pKey, BOOL bDeepCopy, void* pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END;
    
    EEHashEntry_t *pEntry;

    if (bDeepCopy)
    {
        pEntry = (EEHashEntry_t *) new (nothrow) BYTE[sizeof(EEHashEntry) - 1 + 
            sizeof(EECMHelperHashtableKey) + pKey->GetMarshalerTypeNameByteCount() + pKey->GetCookieStringByteCount()];
        if (!pEntry)
            return NULL;

        EECMHelperHashtableKey *pEntryKey = (EECMHelperHashtableKey *) pEntry->Key;
        pEntryKey->m_cMarshalerTypeNameBytes = pKey->GetMarshalerTypeNameByteCount();
        pEntryKey->m_strMarshalerTypeName = (LPSTR) pEntry->Key + sizeof(EECMHelperHashtableKey);
        pEntryKey->m_cCookieStrBytes = pKey->GetCookieStringByteCount();
        pEntryKey->m_strCookie = (LPSTR) pEntry->Key + sizeof(EECMHelperHashtableKey) + pEntryKey->m_cMarshalerTypeNameBytes;
        pEntryKey->m_bSharedHelper = pKey->IsSharedHelper();
        memcpy((void*)pEntryKey->m_strMarshalerTypeName, pKey->GetMarshalerTypeName(), pKey->GetMarshalerTypeNameByteCount()); 
        memcpy((void*)pEntryKey->m_strCookie, pKey->GetCookieString(), pKey->GetCookieStringByteCount()); 
    }
    else
    {
        pEntry = (EEHashEntry_t *) 
            new (nothrow) BYTE[sizeof(EEHashEntry) - 1 + sizeof(EECMHelperHashtableKey)];
        if (!pEntry)
            return NULL;

        EECMHelperHashtableKey *pEntryKey = (EECMHelperHashtableKey *) pEntry->Key;
        pEntryKey->m_cMarshalerTypeNameBytes = pKey->GetMarshalerTypeNameByteCount();
        pEntryKey->m_strMarshalerTypeName = pKey->GetMarshalerTypeName();
        pEntryKey->m_cCookieStrBytes = pKey->GetCookieStringByteCount();
        pEntryKey->m_strCookie = pKey->GetCookieString();
        pEntryKey->m_bSharedHelper = pKey->IsSharedHelper();
    }

    return pEntry;
}


void EECMHelperHashtableHelper::DeleteEntry(EEHashEntry_t *pEntry, void* pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pEntry));
    }
    CONTRACTL_END;
    
    delete[] (BYTE*)pEntry;
}


BOOL EECMHelperHashtableHelper::CompareKeys(EEHashEntry_t *pEntry, EECMHelperHashtableKey *pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pEntry));
        PRECONDITION(CheckPointer(pKey));
    }
    CONTRACTL_END;
    
    EECMHelperHashtableKey *pEntryKey = (EECMHelperHashtableKey *) pEntry->Key;

    if (pEntryKey->IsSharedHelper() != pKey->IsSharedHelper())
        return FALSE;

    if (pEntryKey->GetMarshalerTypeNameByteCount() != pKey->GetMarshalerTypeNameByteCount())
        return FALSE;

    if (memcmp(pEntryKey->GetMarshalerTypeName(), pKey->GetMarshalerTypeName(), pEntryKey->GetMarshalerTypeNameByteCount()) != 0)
        return FALSE;

    if (pEntryKey->GetCookieStringByteCount() != pKey->GetCookieStringByteCount())
        return FALSE;

    if (memcmp(pEntryKey->GetCookieString(), pKey->GetCookieString(), pEntryKey->GetCookieStringByteCount()) != 0)
        return FALSE;

    return TRUE;
}


DWORD EECMHelperHashtableHelper::Hash(EECMHelperHashtableKey *pKey)
{
    WRAPPER_CONTRACT;
    
    return (DWORD)
        (HashBytes((const BYTE *) pKey->GetMarshalerTypeName(), pKey->GetMarshalerTypeNameByteCount()) + 
        HashBytes((const BYTE *) pKey->GetCookieString(), pKey->GetCookieStringByteCount()) + 
        (pKey->IsSharedHelper() ? 1 : 0));
}


OBJECTREF CustomMarshalerHelper::InvokeMarshalNativeToManagedMeth(void *pNative)
{
    WRAPPER_CONTRACT;
    return GetCustomMarshalerInfo()->InvokeMarshalNativeToManagedMeth(pNative);
}


void *CustomMarshalerHelper::InvokeMarshalManagedToNativeMeth(OBJECTREF MngObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    void *RetVal = NULL;

    GCPROTECT_BEGIN(MngObj)
    {
        CustomMarshalerInfo *pCMInfo = GetCustomMarshalerInfo();
        RetVal = pCMInfo->InvokeMarshalManagedToNativeMeth(MngObj);
    }
    GCPROTECT_END();

    return RetVal;
}


void CustomMarshalerHelper::InvokeCleanUpNativeMeth(void *pNative)
{
    WRAPPER_CONTRACT;

    OBJECTREF ExceptionObj = NULL;
    GCPROTECT_BEGIN(ExceptionObj)
    {
        EX_TRY
        {
            GetCustomMarshalerInfo()->InvokeCleanUpNativeMeth(pNative);
        }
        EX_CATCH
        {
            ExceptionObj = GET_THROWABLE();
        }
        EX_END_CATCH(SwallowAllExceptions);

    }
    GCPROTECT_END();   
}


void CustomMarshalerHelper::InvokeCleanUpManagedMeth(OBJECTREF MngObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    GCPROTECT_BEGIN(MngObj)
    {
        CustomMarshalerInfo *pCMInfo = GetCustomMarshalerInfo();
        pCMInfo->InvokeCleanUpManagedMeth(MngObj);
    }
    GCPROTECT_END();
}


void *NonSharedCustomMarshalerHelper::operator new(size_t size, LoaderHeap *pHeap)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pHeap));
    }
    CONTRACTL_END;
    
    return pHeap->AllocMem(sizeof(NonSharedCustomMarshalerHelper));
}


void NonSharedCustomMarshalerHelper::operator delete(void *pMem)
{
    // Instances of this class are always allocated on the loader heap so
    // the delete operator has nothing to do.
    LEAF_CONTRACT;
}


SharedCustomMarshalerHelper::SharedCustomMarshalerHelper(Assembly *pAssembly, TypeHandle hndManagedType, LPCUTF8 strMarshalerTypeName, DWORD cMarshalerTypeNameBytes, LPCUTF8 strCookie, DWORD cCookieStrBytes)
: m_pAssembly(pAssembly)
, m_hndManagedType(hndManagedType)
, m_cMarshalerTypeNameBytes(cMarshalerTypeNameBytes)
, m_strMarshalerTypeName(strMarshalerTypeName)
, m_cCookieStrBytes(cCookieStrBytes)
, m_strCookie(strCookie)
{
    WRAPPER_CONTRACT;
}


void *SharedCustomMarshalerHelper::operator new(size_t size, LoaderHeap *pHeap)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pHeap));
    }
    CONTRACTL_END;
    
    return pHeap->AllocMem(sizeof(SharedCustomMarshalerHelper));
}


void SharedCustomMarshalerHelper::operator delete(void *pMem)
{
    // Instances of this class are always allocated on the loader heap so
    // the delete operator has nothing to do.
    LEAF_CONTRACT;
}


CustomMarshalerInfo *SharedCustomMarshalerHelper::GetCustomMarshalerInfo()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    // Retrieve the marshalling data for the current app domain.
    EEMarshalingData *pMarshalingData = GetThread()->GetDomain()->GetMarshalingData();

    // Retrieve the custom marshaling information for the current shared custom
    // marshaling helper.
    return pMarshalingData->GetCustomMarshalerInfo(this);
}
