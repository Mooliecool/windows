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
#include "vars.hpp"
#include "excep.h"
#include "interoputil.h"
#include "interopconverter.h"
#include "remoting.h"
#include "olevariant.h"
#include "comcallablewrapper.h"


//--------------------------------------------------------
// ConvertObjectToBSTR
// serializes object to a BSTR, caller needs to SysFree the Bstr
// and GCPROTECT the oref parameter.
//--------------------------------------------------------------------------------
void ConvertObjectToBSTR(OBJECTREF* oref, BSTR* pBStr)
{    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pBStr));
        PRECONDITION(IsProtectedByGCFrame (oref));
    }
    CONTRACTL_END;

    *pBStr = NULL;

    // We will be using the remoting services so make sure remoting is started up.
    CRemotingServices::EnsureRemotingStarted();

    MethodDescCallSite marshalToBuffer(METHOD__REMOTING_SERVICES__MARSHAL_TO_BUFFER);

    ARG_SLOT args[] =
    {
        ObjToArgSlot(*oref)
    };

    BASEARRAYREF aref = (BASEARRAYREF) marshalToBuffer.Call_RetOBJECTREF(args);

    _ASSERTE(!aref->IsMultiDimArray());

    ULONG cbSize = aref->GetNumComponents();
    BYTE* pBuf  = (BYTE *)aref->GetDataPtr();

    BSTR bstr = SysAllocStringByteLen(NULL, cbSize);
    if (bstr == NULL)
        COMPlusThrowOM();

    CopyMemory(bstr, pBuf, cbSize);
    *pBStr = bstr;
}

//--------------------------------------------------------------------------------
// ConvertBSTRToObject
// deserializes a BSTR, created using ConvertObjectToBSTR, this api SysFree's the BSTR
//--------------------------------------------------------------------------------
OBJECTREF ConvertBSTRToObject(BSTR bstr)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BSTRHolder localBstr(bstr);

    OBJECTREF oref = NULL;

    // We will be using the remoting services so make sure remoting is started up.
    CRemotingServices::EnsureRemotingStarted();

    MethodDescCallSite unmarshalFromBuffer(METHOD__REMOTING_SERVICES__UNMARSHAL_FROM_BUFFER);

    // convert BSTR to a byte array

    // allocate a byte array
    INT32 elementCount = SysStringByteLen(bstr);
    TypeHandle t = OleVariant::GetArrayForVarType(VT_UI1, TypeHandle((MethodTable *)NULL));
    BASEARRAYREF aref = (BASEARRAYREF) AllocateArrayEx(t, &elementCount, 1);
    // copy the bstr data into the managed byte array
    memcpyNoGCRefs(aref->GetDataPtr(), bstr, elementCount);

    ARG_SLOT args[] = 
    {
        ObjToArgSlot((OBJECTREF)aref)
    };

    oref = unmarshalFromBuffer.Call_RetOBJECTREF(args);

    return oref;
}

//--------------------------------------------------------------------------------
// UnMarshalObjectForCurrentDomain
// unmarshal the managed object for the current domain
//--------------------------------------------------------------------------------
struct ConvertObjectToBSTR_Args
{
    OBJECTREF* oref;
    BSTR *pBStr;
};

void ConvertObjectToBSTR_Wrapper(LPVOID ptr)
{
    WRAPPER_CONTRACT;
    
    ConvertObjectToBSTR_Args *args = (ConvertObjectToBSTR_Args *)ptr;
    ConvertObjectToBSTR(args->oref, args->pBStr);
}

void UnMarshalObjectForCurrentDomain(ADID pObjDomain, ComCallWrapper* pWrap, OBJECTREF* pResult)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pWrap));
        PRECONDITION(CheckPointer(pResult));
    }
    CONTRACTL_END;

    Thread* pThread = GetThread();
    _ASSERTE(pThread);

    _ASSERTE(pThread->GetDomain() != NULL);
    _ASSERTE(pThread->GetDomain()->GetId()!= pObjDomain);

    BSTR bstr = NULL;
    ConvertObjectToBSTR_Args args;
    args.pBStr = &bstr;

    OBJECTREF oref = pWrap->GetObjectRef();

    GCPROTECT_BEGIN(oref);
    {
        args.oref = &oref;
        pThread->DoADCallBack(pObjDomain, ConvertObjectToBSTR_Wrapper, &args);
    }
    GCPROTECT_END();

    _ASSERTE(bstr != NULL);
    *pResult = ConvertBSTRToObject(bstr);
}


//--------------------------------------------------------
// Only IUnknown* is supported without FEATURE_COMINTEROP
//--------------------------------------------------------
IUnknown* __stdcall GetComIPFromObjectRef(OBJECTREF* poref)
{
    CONTRACT (IUnknown*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(poref != NULL);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    NewHolder<ComCallWrapper> pWrapper (new ComCallWrapper());
    pWrapper->Init(poref);

    pWrapper.SuppressRelease();
    RETURN pWrapper;
}

OBJECTREF __stdcall GetObjectRefFromComIP(IUnknown* pUnk, MethodTable* pMTClass)
{
    CONTRACT (OBJECTREF)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pUnk));
//        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    ComCallWrapper *pWrapper = ComCallWrapper::GetCCWFromIUnknown(pUnk);

    if (pWrapper == NULL)
        COMPlusThrow(kInvalidCastException);

    ADID pObjDomain = pWrapper->GetDomainID();

    OBJECTREF oref = NULL;

    if (pObjDomain == GetAppDomain()->GetId())
    {
        oref = pWrapper->GetObjectRef();
    }
    else
    {
        // unmarshal the object to the current domain
        GCPROTECT_BEGIN(oref)
        UnMarshalObjectForCurrentDomain(pObjDomain, pWrapper, &oref);
        GCPROTECT_END();
    }

    // make sure we can cast to the specified class
    if(oref != NULL && pMTClass != NULL)
    {
        GCPROTECT_BEGIN(oref)

        if(!ClassLoader::CanCastToClassOrInterface(oref, pMTClass->GetClass()->GetMethodTable()))
        {
            StackSString ssObjClsName;
            StackSString ssDestClsName;

            oref->GetTrueMethodTable()->GetClass()->_GetFullyQualifiedNameForClass(ssObjClsName);
            pMTClass->GetClass()->_GetFullyQualifiedNameForClass(ssDestClsName);
            COMPlusThrow(kInvalidCastException, IDS_EE_CANNOTCAST, ssObjClsName.GetUnicode(), ssDestClsName.GetUnicode());
        }

        GCPROTECT_END();
    }

    RETURN oref;
}

