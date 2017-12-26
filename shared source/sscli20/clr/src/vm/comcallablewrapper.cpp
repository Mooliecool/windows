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
//--------------------------------------------------------------------------
// ComCallablewrapper.cpp
//
// Implementation for various Wrapper classes
//
//  COMWrapper      : COM callable wrappers for CLR interfaces
//  ContextWrapper  : Wrappers that intercept cross context calls
//--------------------------------------------------------------------------

#include "common.h"
#include "clrtypes.h"

#include "comcallablewrapper.h"
#include "remoting.h"


#include "olevariant.h"
#include "comstring.h"

static OBJECTHANDLE s_hndOleAutBinder;

static MethodDesc* GetInvokeMemberMD()
{
    CONTRACT (MethodDesc*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    static MethodDesc* s_pInvokeMemberMD = NULL;

    // If we already have retrieved the specified MD then just return it.
    if (s_pInvokeMemberMD == NULL)
    {
        // The method desc has not been retrieved yet so find it.
        MethodDesc *pMD = g_Mscorlib.GetMethod(METHOD__CLASS__INVOKE_MEMBER);

        // Ensure that the value types in the signature are loaded.
        MetaSig::EnsureSigValueTypesLoaded(pMD);

        // Cache the method desc.
        s_pInvokeMemberMD = pMD;
    }

    // Return the specified method desc.
    RETURN s_pInvokeMemberMD;
}

static OBJECTREF GetReflectionObject(MethodTable* pClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pClass));
    }
    CONTRACTL_END;
    
    return pClass->GetManagedClassObject();
}

static OBJECTREF GetOleAutBinder()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    // If we have already create the instance of the OleAutBinder then simply return it.
    if (s_hndOleAutBinder)
        return ObjectFromHandle(s_hndOleAutBinder);

    MethodTable *pOleAutBinderClass = g_Mscorlib.GetClass(CLASS__OLE_AUT_BINDER);

    // Allocate an instance of the OleAutBinder class.
    OBJECTREF OleAutBinder = AllocateObject(pOleAutBinderClass);

    // Keep a handle to the OleAutBinder instance.
    s_hndOleAutBinder = CreateGlobalHandle(OleAutBinder);

    return OleAutBinder;
}

STDMETHODIMP ComCallWrapper::QueryInterface(REFIID riid, void** ppv)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
        PRECONDITION(CheckPointer(ppv));
    }
    CONTRACTL_END;
    
    if ((riid == IID_IUnknown) || (riid == IID_IManagedInstanceWrapper))
    {
        *ppv = (IManagedInstanceWrapper*)this;
    }
    else 
    {
        // If you are hitting this assert, you need change your code to 
        // use IManagedInstanceWrapper                                  probably
        _ASSERT(false);

        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();   
    return S_OK;
}

STDMETHODIMP_(ULONG) ComCallWrapper::AddRef(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;
    
    return InterlockedIncrement(&m_cRef);
}
 
STDMETHODIMP_(ULONG) ComCallWrapper::Release(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;
    
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
        delete this;
    return cRef;   
}

VOID ComCallWrapper::InvokeByNameCallback(LPVOID ptr)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(ptr));
    }
    CONTRACTL_END;
    
    InvokeByNameArgs* args = (InvokeByNameArgs*)ptr;
    INT32 NumByrefArgs = 0;
    INT32 *aByrefArgMngVariantIndex = NULL;
    INT32 iArg;
    struct _gc
    {
        OBJECTREF Target;
        STRINGREF MemberName;
        PTRARRAYREF ParamArray;
        OBJECTREF TmpObj;
        OBJECTREF RetVal;
        OBJECTREF ReflectionObj;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    gc.MemberName = COMString::NewString(args->MemberName);

    gc.ParamArray = (PTRARRAYREF)AllocateObjectArray(args->ArgCount, g_pObjectClass);

    //
    // Fill in the arguments.
    //

    for (iArg = 0; iArg < args->ArgCount; iArg++)
    {
        // Convert the variant.
        VARIANT *pSrcOleVariant = &args->ArgList[iArg];
        OleVariant::MarshalObjectForOleVariant(pSrcOleVariant, &gc.TmpObj);
        gc.ParamArray->SetAt(iArg, gc.TmpObj);

        // If the argument is byref then add it to the array of byref arguments.
        if (V_VT(pSrcOleVariant) & VT_BYREF)
        {
            if (aByrefArgMngVariantIndex == NULL) {
                aByrefArgMngVariantIndex = (INT32 *)_alloca(sizeof(INT32) * args->ArgCount);
            }

            aByrefArgMngVariantIndex[NumByrefArgs] = iArg;
            NumByrefArgs++;
        }
    }

    gc.Target = ObjectFromHandle(args->pThis->m_hThis);

    //
    // Invoke using IReflect::InvokeMember
    //

    MethodTable *pClass = gc.Target->GetMethodTable();
    gc.ReflectionObj = GetReflectionObject(pClass);

    // Retrieve the method descriptor that will be called on.
    MethodDescCallSite invokeMember(GetInvokeMemberMD(), &gc.ReflectionObj);

    // Prepare the arguments that will be passed to Invoke.
    ARG_SLOT Args[] =
    {
            ObjToArgSlot(gc.ReflectionObj), // IReflect
            ObjToArgSlot(gc.MemberName),    // name
            (ARG_SLOT) args->BindingFlags,  // invokeAttr
            ObjToArgSlot(GetOleAutBinder()),// binder
            ObjToArgSlot(gc.Target),        // target
            ObjToArgSlot(gc.ParamArray),    // args
            ObjToArgSlot(NULL),             // modifiers
            ObjToArgSlot(NULL),             // culture
            ObjToArgSlot(NULL)              // namedParameters
    };

    // Do the actual method invocation.
    gc.RetVal = invokeMember.Call_RetOBJECTREF(Args);

    //
    // Convert the return value and the byref arguments.
    //

    // Convert all the ByRef arguments back.
    for (iArg = 0; iArg < NumByrefArgs; iArg++)
    {
        INT32 i = aByrefArgMngVariantIndex[iArg];
        gc.TmpObj = gc.ParamArray->GetAt(i);
        OleVariant::MarshalOleRefVariantForObject(&gc.TmpObj, &args->ArgList[i]);
    }

    // Convert the return COM+ object to an OLE variant.
    if (args->pRetVal)
        OleVariant::MarshalOleVariantForObject(&gc.RetVal, args->pRetVal);

    GCPROTECT_END();
}

STDMETHODIMP ComCallWrapper::InvokeByName(LPCWSTR MemberName, INT32 BindingFlags, INT32 ArgCount, VARIANT *ArgList, VARIANT *pRetVal)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        PRECONDITION(CheckPointer(MemberName));
        PRECONDITION(CheckPointer(ArgList, NULL_OK));
        PRECONDITION(CheckPointer(pRetVal, NULL_OK));
    }
    CONTRACTL_END;
    
    HRESULT hr = S_OK;
    InvokeByNameArgs args;    

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
    {
        EX_TRY
        {
            if (pRetVal)
                SafeVariantClear(pRetVal);

            args.pThis = this;
            args.MemberName = MemberName;
            args.BindingFlags = BindingFlags;
            args.ArgCount = ArgCount;
            args.ArgList = ArgList;
            args.pRetVal = pRetVal;

            Thread* pThread = GetThread();

            if (m_dwDomainId != pThread->GetDomain()->GetId())
            {
                pThread->DoADCallBack(m_dwDomainId, InvokeByNameCallback, &args);
            }
            else
            {
                InvokeByNameCallback(&args);
            }
        }
        EX_CATCH
        {
            GCX_COOP();
            hr = SetupErrorInfo(GET_THROWABLE());
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    END_EXTERNAL_ENTRYPOINT;

    return hr;
}

