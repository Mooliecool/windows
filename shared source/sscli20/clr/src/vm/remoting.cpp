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

/*===========================================================================
**
** File:    remoting.cpp
**        
** Purpose: Defines various remoting related objects such as
**          proxies
**
** Date:    Feb 16, 1999
**
=============================================================================*/
#include "common.h"
#include "virtualcallstub.h"
#include "excep.h"
#include "comstring.h"
#include "comdelegate.h"
#include "remoting.h"
#include "field.h"
#include "siginfo.hpp"
#include "stackbuildersink.h"
#include "eehash.h"
#include "profilepriv.h"
#include "message.h"
#include "eeconfig.h"
#include "comcallablewrapper.h"
#include "interopconverter.h"
#include "asmconstants.h"
#include "crossdomaincalls.h"
#include "contractimpl.h"
#include "typestring.h"
#include "generics.h"
#include "appdomain.inl"
#include "dbginterface.h"

// Macros

#define IDS_REMOTING_LOCK           "Remoting Services" // Remoting services lock
#define IDS_TPMETHODTABLE_LOCK      "TP Method Table"   // Transparent Proxy Method table

#ifndef DACCESS_COMPILE


// These hold label offsets into non-virtual thunks. They are used by
// CNonVirtualThunkMgr::DoTraceStub and ::TraceManager to help the
// debugger figure out where the thunk is going to go.
DWORD g_dwNonVirtualThunkRemotingLabelOffset = 0;
DWORD g_dwNonVirtualThunkReCheckLabelOffset = 0;

// Statics

MethodTable *CRemotingServices::s_pMarshalByRefObjectClass;    
MethodTable *CRemotingServices::s_pServerIdentityClass;
MethodTable *CRemotingServices::s_pProxyAttributeClass;
MethodTable *CRemotingServices::s_pContextClass;
MethodTable *CRemotingServices::s_pRuntimeTypeClass;

MethodDesc *CRemotingServices::s_pRPPrivateInvoke;
MethodDesc *CRemotingServices::s_pRPInvokeStatic;
MethodDesc *CRemotingServices::s_pWrapMethodDesc;
MethodDesc *CRemotingServices::s_pIsCurrentContextOK;
MethodDesc *CRemotingServices::s_pCheckCast;
MethodDesc *CRemotingServices::s_pFieldSetterDesc;
MethodDesc *CRemotingServices::s_pFieldGetterDesc;
MethodDesc *CRemotingServices::s_pObjectGetTypeDesc;
MethodDesc *CRemotingServices::s_pGetTypeDesc;
MethodDesc *CRemotingServices::s_pProxyForDomainDesc;
MethodDesc *CRemotingServices::s_pServerContextForProxyDesc;
MethodDesc *CRemotingServices::s_pServerDomainIdForProxyDesc;
DWORD CRemotingServices::s_dwTPOffset;
DWORD CRemotingServices::s_dwServerOffsetInRealProxy;
DWORD CRemotingServices::s_dwSrvIdentityOffsetInRealProxy;
DWORD CRemotingServices::s_dwIdOffset;
DWORD CRemotingServices::s_dwServerCtxOffset;
DWORD CRemotingServices::s_dwTPOrObjOffsetInIdentity;
DWORD CRemotingServices::s_dwMBRIDOffset;
DWORD CRemotingServices::s_dwLeaseOffsetInIdentity;
DWORD CRemotingServices::s_dwURIOffsetInIdentity;
CrstStatic CRemotingServices::s_RemotingCrst;
BOOL CRemotingServices::s_fRemotingStarted;
MethodDesc *CRemotingServices::s_pRenewLeaseOnCallDesc;



#ifdef REMOTING_PERF
HANDLE CRemotingServices::s_hTimingData = NULL;
#endif

// CTPMethodTable Statics
DWORD CTPMethodTable::s_dwCommitedTPSlots;
DWORD CTPMethodTable::s_dwReservedTPSlots;
MethodTable *CTPMethodTable::s_pThunkTable;
EEClass *CTPMethodTable::s_pTransparentProxyClass;
DWORD CTPMethodTable::s_dwGCInfoBytes;
DWORD CTPMethodTable::s_dwMTDataSlots;
DWORD CTPMethodTable::s_dwRPOffset;
DWORD CTPMethodTable::s_dwMTOffset;
DWORD CTPMethodTable::s_dwItfMTOffset;
DWORD CTPMethodTable::s_dwStubOffset;
DWORD CTPMethodTable::s_dwStubDataOffset;
DWORD CTPMethodTable::s_dwMaxSlots;
MethodTable *CTPMethodTable::s_pTPMT;
MethodTable *CTPMethodTable::s_pRemotingProxyClass;
Stub *CTPMethodTable::s_pDelegateStub;
CrstStatic CTPMethodTable::s_TPMethodTableCrst;
EEThunkHashTable *CTPMethodTable::s_pThunkHashTable;
BOOL CTPMethodTable::s_fTPTableFieldsInitialized;

#endif // !DACCESS_COMPILE


SPTR_IMPL(Stub, CTPMethodTable, s_pTPStub);


#ifndef DACCESS_COMPILE

// CVirtualThunks statics
CVirtualThunks *CVirtualThunks::s_pVirtualThunks;

// CVirtualThunkMgr statics                                                     
CVirtualThunkMgr *CVirtualThunkMgr::s_pVirtualThunkMgr;

#ifndef HAS_REMOTING_PRECODE
// CNonVirtualThunk statics
CNonVirtualThunk *CNonVirtualThunk::s_pNonVirtualThunks;

// CNonVirtualThunkMgr statics
CNonVirtualThunkMgr *CNonVirtualThunkMgr::s_pNonVirtualThunkMgr;
#endif

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::Initialize    public
//
//  Synopsis:   Initialized remoting state
//+----------------------------------------------------------------------------
VOID CRemotingServices::Initialize()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pRPPrivateInvoke = NULL;
    s_pRPInvokeStatic = NULL;
    s_dwTPOffset = NULL;
    s_fRemotingStarted = FALSE;

    // Initialize the remoting services critical section
    s_RemotingCrst.Init(IDS_REMOTING_LOCK, CrstRemoting, CrstFlags(CRST_REENTRANCY|CRST_HOST_BREAKABLE));

    CTPMethodTable::Initialize();
}

 //+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::ResetInterfaceCache    public
//
//  Synopsis:   Clears the interface cast cache on a transparent proxy.
//              
//+----------------------------------------------------------------------------
FCIMPL1(VOID, CRemotingServices::ResetInterfaceCache, Object* orTP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    orTP->SetOffsetPtr(CTPMethodTable::GetOffsetOfInterfaceMT(), NULL);
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::IsTransparentProxy    public
//
//  Synopsis:   Check whether the supplied object is proxy or not. This 
//              represents the overloaded method that takes an object.
//              
//+----------------------------------------------------------------------------
FCIMPL1(FC_BOOL_RET, CRemotingServices::FCIsTransparentProxy, Object* orTP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FC_RETURN_BOOL(IsTransparentProxy(orTP));
}
FCIMPLEND

INT32 CRemotingServices::IsTransparentProxy(Object* orTP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    INT32 fIsTPMT = FALSE;

    if(orTP != NULL)
    {
        // Check if the supplied object has transparent proxy method table
        MethodTable *pMT = orTP->GetMethodTable();
        fIsTPMT = pMT->IsTransparentProxyType() ? TRUE : FALSE;
    }

    LOG((LF_REMOTING, LL_EVERYTHING, "!IsTransparentProxyEx(0x%x) returning %s",
         orTP, fIsTPMT ? "TRUE" : "FALSE"));

    return(fIsTPMT);
}

// Called from RemotingServices::ConfigureRemoting to remember that
// a config file has been parsed.
FCIMPL0(VOID, CRemotingServices::SetRemotingConfiguredFlag)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    // Mark a flag for the current appDomain to remember the fact
    // that ConfigureRemoting has been called.
    GetThread()->GetDomain()->SetRemotingConfigured();
}
FCIMPLEND


//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetRealProxy    public
//
//  Synopsis:   Returns the real proxy backing the transparent
//              proxy
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRemotingServices::FCGetRealProxy, Object* objTP)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return GetRealProxy(objTP);
}
FCIMPLEND

Object* CRemotingServices::GetRealProxy(Object* objTP)
{   
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF rv = NULL;

    if ((objTP != NULL) && (IsTransparentProxy(objTP)))
    {
        _ASSERTE(s_fRemotingStarted);
        rv = CTPMethodTable::GetRP(OBJECTREF(objTP));
    }

    LOG((LF_REMOTING, LL_INFO100, "!GetRealProxy(0x%x) returning 0x%x\n", objTP, OBJECTREFToObject(rv)));

    return OBJECTREFToObject(rv);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CreateTransparentProxy    public
//
//  Synopsis:   Creates a new transparent proxy for the supplied real
//              proxy
//+----------------------------------------------------------------------------
FCIMPL4(Object*, CRemotingServices::CreateTransparentProxy, Object* orRPUNSAFE, ReflectClassBaseObject* pClassToProxyUNSAFE, LPVOID pStub, Object* orStubDataUNSAFE)
{
    struct _gc
    {
        OBJECTREF orRP;
        OBJECTREF orTP;
        OBJECTREF orStubData;
    } gc;
    gc.orRP = (OBJECTREF) orRPUNSAFE;
    gc.orTP = NULL;
    gc.orStubData = (OBJECTREF) orStubDataUNSAFE;

    TypeHandle ty = ((REFLECTCLASSBASEREF) pClassToProxyUNSAFE)->GetType();

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(gc.orRP != NULL);
        PRECONDITION(!ty.IsNull());
        PRECONDITION(CheckPointer(pStub, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Ensure remoting has been started.
    EnsureRemotingStarted();

    // Check if the supplied object has a transparent proxy already
    if (((DWORD_PTR)gc.orRP->GetPtrOffset(s_dwTPOffset)) != NULL)
        COMPlusThrow(kArgumentException, L"Remoting_TP_NonNull");

    // Create a tranparent proxy that behaves as an object of the desired class
    CTPMethodTable::CreateTPOfClassForRP(ty, &gc.orRP, &gc.orTP);
    
    // Set the stub pointer
    gc.orTP->SetOffsetPtr(CTPMethodTable::GetOffsetOfStub(), pStub);

    // Set the stub data
    gc.orTP->SetOffsetObjectRef(CTPMethodTable::GetOffsetOfStubData(), (size_t)OBJECTREFToObject(gc.orStubData));

    COUNTER_ONLY(GetPrivatePerfCounters().m_Context.cProxies++);
    
    LOG((LF_REMOTING, LL_INFO100, "CreateTransparentProxy returning 0x%x\n", OBJECTREFToObject(gc.orTP)));

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.orTP);
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::EnsureRemotingStarted
//
//  Synopsis:   Startup the remoting services.
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::EnsureRemotingStarted()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (!CRemotingServices::s_fRemotingStarted)
        CRemotingServices::StartRemoting();

    if (!CTPMethodTable::s_fTPTableFieldsInitialized)
        CTPMethodTable::EnsureFieldsInitialized();
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::StartRemoting    private
//
//  Synopsis:   Initialize the static fields of CRemotingServices class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::StartRemoting()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Acquire the remoting lock before initializing fields
    GCX_PREEMP();
        CrstHolder ch(&s_RemotingCrst);
    GCX_POP();

    // Make sure that no other thread has initialized the fields
    if (!s_fRemotingStarted)
    {
        InitActivationServicesClass();     
        InitRealProxyClass();
        InitRemotingProxyClass();
        InitIdentityClass();
        InitServerIdentityClass();
        InitContextClass();
        InitMarshalByRefObjectClass();
        InitRemotingServicesClass();
        InitProxyAttributeClass();
        InitObjectClass();
        InitLeaseClass();
        InitRuntimeTypeClass();


        // *********   NOTE   ************ 
        // This must always be the last statement in this block to prevent races
        // 
        *((BOOL volatile*)&s_fRemotingStarted) = TRUE;
        // ********* END NOTE ************        
    }
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitActivationServicesClass    private
//
//  Synopsis:   Extract the method descriptors and fields of ActivationServices class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitActivationServicesClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pIsCurrentContextOK = g_Mscorlib.GetMethod(METHOD__ACTIVATION_SERVICES__IS_CURRENT_CONTEXT_OK);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitRealProxyClass    private
//
//  Synopsis:   Extract the method descriptors and fields of Real Proxy class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitRealProxyClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    // Now store the methoddesc of the PrivateInvoke method on the RealProxy class
    s_pRPPrivateInvoke = g_Mscorlib.GetMethod(METHOD__REAL_PROXY__PRIVATE_INVOKE);

    // Now find the offset to the _tp field inside the RealProxy class
    s_dwTPOffset = RealProxyObject::GetOffsetOfTP() - Object::GetOffsetOfFirstField();
    _ASSERTE(s_dwTPOffset == 0);

    // Now find the offset to the _identity field inside the 
    // RealProxy  class
    s_dwIdOffset = RealProxyObject::GetOffsetOfIdentity() - Object::GetOffsetOfFirstField();

    s_dwServerOffsetInRealProxy = RealProxyObject::GetOffsetOfServerObject() - Object::GetOffsetOfFirstField();

    s_dwSrvIdentityOffsetInRealProxy = RealProxyObject::GetOffsetOfServerIdentity() - Object::GetOffsetOfFirstField();
    
    return;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitRemotingProxyClass    private
//
//  Synopsis:   Extract the method descriptors and fields of RemotingProxy class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitRemotingProxyClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pRPInvokeStatic = g_Mscorlib.GetMethod(METHOD__REMOTING_PROXY__INVOKE);

    // Note: We cannot do this inside TPMethodTable::InitializeFields ..
    // that causes recursions if in some situation only the latter is called
    // If you do this you will see Asserts when running any process under CorDbg
    // This is because jitting of NV methods on MBR objects calls 
    // InitializeFields and when actually doing that we should not need to
    // JIT another NV method on some MBR object.
    CTPMethodTable::s_pRemotingProxyClass = g_Mscorlib.GetClass(CLASS__REMOTING_PROXY);
    _ASSERTE(CTPMethodTable::s_pRemotingProxyClass);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitServerIdentityClass    private
//
//  Synopsis:   Extract the method descriptors and fields of ServerIdentity class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitServerIdentityClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pServerIdentityClass = g_Mscorlib.GetClass(CLASS__SERVER_IDENTITY);

    s_dwServerCtxOffset = g_Mscorlib.GetFieldOffset(FIELD__SERVER_IDENTITY__SERVER_CONTEXT);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitIdentityClass    private
//
//  Synopsis:   Extract the method descriptors and fields of Identity class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitIdentityClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_dwTPOrObjOffsetInIdentity = g_Mscorlib.GetFieldOffset(FIELD__IDENTITY__TP_OR_OBJECT);

    s_dwLeaseOffsetInIdentity = g_Mscorlib.GetFieldOffset(FIELD__IDENTITY__LEASE);

    s_dwURIOffsetInIdentity = g_Mscorlib.GetFieldOffset(FIELD__IDENTITY__OBJURI);
    
    return;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitContextClass    private
//
//  Synopsis:   Extract the method descriptors and fields of Contexts class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitContextClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pContextClass = g_Mscorlib.GetClass(CLASS__CONTEXT);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitMarshalByRefObjectClass    private
//
//  Synopsis:   Extract the method descriptors and fields of MarshalByRefObject class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitMarshalByRefObjectClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pMarshalByRefObjectClass = g_Mscorlib.GetClass(CLASS__MARSHAL_BY_REF_OBJECT);
    s_dwMBRIDOffset = MarshalByRefObjectBaseObject::GetOffsetOfServerIdentity() - Object::GetOffsetOfFirstField();
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitRemotingServicesClass    private
//
//  Synopsis:   Extract the method descriptors and fields of RemotingServices class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitRemotingServicesClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pCheckCast = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__CHECK_CAST);

    // Need these to call wrap/unwrap from the VM (message.cpp).
    // Also used by JIT helpers to wrap/unwrap
    s_pWrapMethodDesc = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__WRAP);
    s_pProxyForDomainDesc = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__CREATE_PROXY_FOR_DOMAIN);
    s_pServerContextForProxyDesc = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__GET_SERVER_CONTEXT_FOR_PROXY);
    s_pServerDomainIdForProxyDesc = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__GET_SERVER_DOMAIN_ID_FOR_PROXY);
    s_pGetTypeDesc = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__GET_TYPE);
}


//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitProxyAttributeClass    private
//
//  Synopsis:   Cache the ProxyAttribute class method table
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitProxyAttributeClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (s_pProxyAttributeClass == NULL)
        s_pProxyAttributeClass = g_Mscorlib.GetClass(CLASS__PROXY_ATTRIBUTE);
}

MethodTable *CRemotingServices::GetProxyAttributeClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    InitProxyAttributeClass();
    
    RETURN s_pProxyAttributeClass;
}


//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitObjectClass    private
//
//  Synopsis:   Extract the method descriptors and fields of Object class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitObjectClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    s_pFieldSetterDesc = g_Mscorlib.GetMethod(METHOD__OBJECT__FIELD_SETTER);
    s_pFieldGetterDesc = g_Mscorlib.GetMethod(METHOD__OBJECT__FIELD_GETTER);
    s_pObjectGetTypeDesc = g_Mscorlib.GetMethod(METHOD__OBJECT__GET_TYPE);
}

BOOL CRemotingServices::InitLeaseClass()
{
    BOOL fReturn = TRUE;

    s_pRenewLeaseOnCallDesc = g_Mscorlib.GetMethod(METHOD__LEASE__RENEW_ON_CALL);

    return fReturn;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::InitRuntimeTypeClass    private
//
//  Synopsis:   Extract the method table of the RuntimeType class
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::InitRuntimeTypeClass()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Keep this idempotent, multiple threads can race here and we can be called
    // more than once.
    s_pRuntimeTypeClass = g_Mscorlib.GetClass(CLASS__CLASS);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::IsRemoteActivationRequired    public
//
//  Synopsis:   Determines whether we can activate in the current context.  
//              If not, then we will end up creating the object in a different 
//              context/appdomain/process/machine and so on...
//              This is used to provide the appropriate activator to JIT
//              (if we return true here ... JIT_NewCrossContext will get called
//              when the "new" executes)
//
//
//  Note:       Called by getNewHelper
//+----------------------------------------------------------------------------
BOOL CRemotingServices::IsRemoteActivationRequired(TypeHandle ty)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
        
    MethodTable *pMT = ty.GetMethodTable();

    PREFIX_ASSUME(pMT != NULL);
    _ASSERTE(!pMT->IsThunking());
    
    BOOL fRequiresNewContext = pMT->IsMarshaledByRef();

    // Contextful classes imply marshal by ref but not vice versa
    _ASSERTE(!fRequiresNewContext || 
             !(pMT->IsContextful() && !pMT->IsMarshaledByRef()));

    LOG((LF_REMOTING, LL_EVERYTHING, "IsRemoteActivationRequired returning %d\n", fRequiresNewContext));

    return fRequiresNewContext; 
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::RequiresManagedActivation    private
//
//  Synopsis:   Determine if a config file has been parsed or if there
//              are any attributes on the class that would require us
//              to go into the managed activation codepath.
//              
//
//  Note:       Called by CreateProxyOrObject (JIT_NewCrossContext)
//+----------------------------------------------------------------------------
ManagedActivationType __stdcall CRemotingServices::RequiresManagedActivation(TypeHandle ty)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(!ty.IsNull());
    }
    CONTRACTL_END;
    
    MethodTable* pMT = ty.GetMethodTable();

    PREFIX_ASSUME(pMT != NULL);
    if (!pMT->IsMarshaledByRef())
        return NoManagedActivation;

#ifdef _DEBUG
   
    ManagedActivationType bManaged = NoManagedActivation;
    if (pMT->IsRemotingConfigChecked())
    {
        // We have done work to figure this out in the past ... 
        // use the cached result
        bManaged = pMT->RequiresManagedActivation() ? ManagedActivation : NoManagedActivation;
    } 
    else if (pMT->IsContextful() || pMT->GetClass()->HasRemotingProxyAttribute()) 
    {
        // Contextful and classes that have a remoting proxy attribute 
        // (whether they are MarshalByRef or ContextFul) always take the slow 
        // path of managed activation
        bManaged = ManagedActivation;
    }
    else
    {
        // If we have parsed a config file that might have configured
        // this Type to be activated remotely 
        if (GetAppDomain()->IsRemotingConfigured())
        {
            bManaged = ManagedActivation;
            // We will remember if the activation is actually going
            // remote based on if the managed call to IsContextOK returned us
            // a proxy or not
        }


    }

#endif // _DEBUG

    if (pMT->RequiresManagedActivation()) 
    {
        // Contextful and classes that have a remoting proxy attribute 
        // (whether they are MarshalByRef or ContextFul) always take the slow 
        // path of managed activation
        _ASSERTE(bManaged == ManagedActivation);
        return ManagedActivation;
    }
    
    ManagedActivationType bMng = NoManagedActivation;
    if (!pMT->IsRemotingConfigChecked())
    {
        g_IBCLogger.LogMethodTableAccess(pMT);

        // If we have parsed a config file that might have configured
        // this Type to be activated remotely   
        if (GetAppDomain()->IsRemotingConfigured())
        {
            bMng = ManagedActivation;
            // We will remember if the activation is actually going
            // remote based on if the managed call to IsContextOK returned us
            // a proxy or not
        }
        
        
        if (bMng == NoManagedActivation)
        {
            pMT->SetRemotingConfigChecked();
        }
    }

    _ASSERTE(bManaged == bMng);
    return bMng;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CreateProxyOrObject    public
//
//  Synopsis:   Determine if the current context is appropriate
//              for activation. If the current context is OK then it creates 
//              an object else it creates a proxy.
//              
//
//  Note:       Called by JIT_NewCrossContext 
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::CreateProxyOrObject(MethodTable* pMT, 
    BOOL fIsCom /*default:FALSE*/, BOOL fIsNewObj /*default:FALSE*/)
    /* fIsCom == Did we come here through CoCreateInstance */
    /* fIsNewObj == Did we come here through Jit_NewCrossContext (newObj) */
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(!pMT->IsThunking());

        // By the time we reach here, we have alread checked that the class requires
        // managed activation. This check is made either through the JIT_NewCrossContext helper
        // or Activator.CreateInstance codepath.
        PRECONDITION(RequiresManagedActivation(TypeHandle(pMT)) || IsRemoteActivationRequired(TypeHandle(pMT)));
    }
    CONTRACTL_END;
    
    // Ensure remoting has been started.
    EnsureRemotingStarted();

    // Get the address of IsCurrentContextOK in managed code
    MethodDesc* pTargetMD = NULL;
    Object *pServer = NULL;
    

    {
        pTargetMD = CRemotingServices::MDofIsCurrentContextOK();
    }

    // Arrays are not created by JIT_NewCrossContext
    _ASSERTE(!pMT->GetClass()->IsArrayClass());

    // Get the type seen by reflection
    REFLECTCLASSBASEREF reflectType = (REFLECTCLASSBASEREF) pMT->GetManagedClassObject();
    LPVOID pvType = NULL;
    *(REFLECTCLASSBASEREF *)&pvType = reflectType;

    // This will return either an uninitialized object or a proxy
    pServer = (Object *)CTPMethodTable::CallTarget(pTargetMD, pvType, NULL, (LPVOID)(size_t)(fIsNewObj?1:0));

    if (!pMT->IsContextful() && !pMT->IsComObjectType())
    {   
        // Cache the result of the activation attempt ... 
        // if a strictly MBR class is not configured for remote 
        // activation we will not go 
        // through this slow path next time! 
        // (see RequiresManagedActivation)
        if (IsTransparentProxy(pServer))
        {
            // Set the flag that this class is remote activate
            // which means activation will go to managed code.
            pMT->SetRequiresManagedActivation();
        }
        else
        {
            // Set only the flag that no managed checks are required
            // for this class next time.
            pMT->SetRemotingConfigChecked();
        }
    }

    LOG((LF_REMOTING, LL_INFO1000, "CreateProxyOrObject returning 0x%p\n", pServer));
    if (pMT->IsContextful())
    {
        COUNTER_ONLY(GetPrivatePerfCounters().m_Context.cObjAlloc++);
    }
    return ObjectToOBJECTREF(pServer);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::AllocateUninitializedObject    public
//
//  Synopsis:   Allocates an uninitialized object of the given type
//
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRemotingServices::AllocateUninitializedObject, ReflectClassBaseObject* pClassOfObjectUNSAFE)
{   
    OBJECTREF newobj = NULL;
    MethodTable *pMT = ((REFLECTCLASSBASEREF) pClassOfObjectUNSAFE)->GetType().GetMethodTable();
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pMT));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    PREFIX_ASSUME(pMT != NULL);

    // Make sure that this private allocator function is used by remoting 
    // only for marshalbyref objects
    if (!pMT->IsMarshaledByRef())
        COMPlusThrow(kRemotingException,L"Remoting_Proxy_ProxyTypeIsNotMBR");

    // if this is an abstract class then we will
    //  fail this
    if (pMT->IsAbstract())
        COMPlusThrow(kMemberAccessException,L"Acc_CreateAbst");

    newobj = pMT->Allocate();

    LOG((LF_REMOTING, LL_INFO1000, "AllocateUninitializedObject returning 0x%p\n", OBJECTREFToObject(newobj)));

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(newobj);
}
FCIMPLEND


//+----------------------------------------------------------------------------
//
//  Method:     VOID RemotingServices::CallDefaultCtor(callDefaultCtorArgs* pArgs)
//  Synopsis:   call default ctor
//+----------------------------------------------------------------------------
FCIMPL1(VOID, CRemotingServices::CallDefaultCtor, Object* orefUNSAFE)
{
    OBJECTREF oref = (OBJECTREF) orefUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(oref);
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(oref != NULL);
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    CallDefaultConstructor(oref);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::AllocateInitializedObject    public
//
//  Synopsis:   Allocates an uninitialized object of the given type
//
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRemotingServices::AllocateInitializedObject, ReflectClassBaseObject* pClassOfObjectUNSAFE)
{   
    OBJECTREF newobj = NULL;
    MethodTable *pMT = ((REFLECTCLASSBASEREF) pClassOfObjectUNSAFE)->GetType().GetMethodTable();
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, newobj);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pMT));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Make sure that this private allocator function is used by remoting 
    // only for marshalbyref objects
    _ASSERTE(!pMT->IsContextful() || pMT->IsMarshaledByRef());

    newobj = pMT->Allocate();

    CallDefaultConstructor(newobj);

    LOG((LF_REMOTING, LL_INFO1000, "AllocateInitializedObject returning 0x%p\n", OBJECTREFToObject(newobj)));

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(newobj);
}
FCIMPLEND

#ifndef HAS_REMOTING_PRECODE
//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetStubForNonVirtualMethod   public
//
//  Synopsis:   Get a stub for a non virtual method. 
//
//+----------------------------------------------------------------------------
Stub* CRemotingServices::GetStubForNonVirtualMethod(MethodDesc* pMD, LPVOID pvAddrOfCode, Stub* pInnerStub)
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(CheckPointer(pvAddrOfCode));
        PRECONDITION(CheckPointer(pInnerStub, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    CPUSTUBLINKER sl;
    Stub* pStub = CTPMethodTable::CreateStubForNonVirtualMethod(pMD, &sl, pvAddrOfCode, pInnerStub);
    
    RETURN pStub;
}
#endif // HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetNonVirtualSlotForVirtualMethod   public
//
//  Synopsis:   Get a thunk for a non-virtual call to a virtual method.
//              Virtual methods do not normally get thunked in the vtable. This
//              is because virtual calls use the objetc's vtable, and proxied objects
//              would use the proxy's vtable. Hence local object (which would
//              have the real vtable) can make virtual calls without going through
//              the thunk.
//              However, if the virtual function is called non-virtually, we have
//              a problem (since this would bypass the proxy's vtable). Since this
//              is not a common case, we fix it by using a stub in such cases.
//
//+----------------------------------------------------------------------------
PTR_TADDR CRemotingServices::GetNonVirtualSlotForVirtualMethod(MethodDesc* pMD)
{
    CONTRACT (PTR_TADDR)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRemotingInterceptedViaVirtualDispatch());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

#ifdef HAS_REMOTING_PRECODE
    Precode* pPrecode = Precode::GetPrecodeFromEntryPoint((TADDR)pMD->GetDomain()->GetFuncPtrStubs()->GetFuncPtrStub(pMD, PRECODE_REMOTING));

    // The slot is allocated right after the precode itself
    RETURN (PTR_TADDR)(PTR_HOST_TO_TADDR(pPrecode) + pPrecode->SizeOf());
#else
    RETURN CTPMethodTable::GetOrCreateNonVirtualSlotForVirtualMethod(pMD);
#endif
}

#ifndef HAS_REMOTING_PRECODE
//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::DestroyThunk   public
//
//  Synopsis:   Destroy the thunk for the non virtual method. 
//
//+----------------------------------------------------------------------------
void CRemotingServices::DestroyThunk(MethodDesc* pMD)
{
    WRAPPER_CONTRACT;
    
    // Delegate to a helper routine
    CTPMethodTable::DestroyThunk(pMD);
} 
#endif // HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CheckCast   public
//
//  Synopsis:   Checks either 
//              (1) If the object type supports the given interface OR
//              (2) If the given type is present in the hierarchy of the 
//              object type
//+----------------------------------------------------------------------------
BOOL CRemotingServices::CheckCast(OBJECTREF orTP, TypeHandle objTy, TypeHandle ty)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(orTP != NULL);
        PRECONDITION(!objTy.IsNull());
        PRECONDITION(!ty.IsNull());

        // Object class can never be an interface. We use a separate cached
        // entry for storing interfaces that the proxy supports.
        PRECONDITION(!objTy.IsInterface());
    }
    CONTRACTL_END;
    
    // Early out if someone's trying to cast us to a type desc (such as a byref,
    // array or function pointer).
    if (ty.IsTypeDesc())
        return FALSE;

    BOOL fCastOK = FALSE;    

    // (1) We are trying to cast to an interface 
    if(ty.IsInterface())
    {
        // Do a quick check for interface cast by comparing it against the
        // cached entry
        MethodTable *pItfMT = (MethodTable *)orTP->GetPtrOffset(CTPMethodTable::GetOffsetOfInterfaceMT());
        if(NULL != pItfMT)
        {
            if(pItfMT == ty.GetMethodTable())
                fCastOK = TRUE;
            else
                fCastOK = pItfMT->CanCastToInterface(ty.GetMethodTable());
        }

        if(!fCastOK)
            fCastOK = objTy.GetMethodTable()->CanCastToInterface(ty.GetMethodTable());        
    }
    // (2) Everything else...
    else
    {
        // Walk up the class hierarchy and find a matching class
        while (ty != objTy)
        {
            if (objTy.IsNull())
            {
                // Oh-oh, the cast did not succeed. Maybe we have to refine
                // the proxy to match the clients view
                break;
            }            

            // Continue searching
            objTy = objTy.GetParent();
        }

        if(objTy == ty)
            fCastOK = TRUE;
    }

    return fCastOK;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CheckCast   public
//
//  Synopsis:   Refine the type hierarchy that the proxy represents to match
//              the client view. If the client is trying to cast the proxy
//              to a type not supported by the server object then we 
//              return NULL
//
//+----------------------------------------------------------------------------
BOOL CRemotingServices::CheckCast(OBJECTREF orTP, TypeHandle ty)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(orTP != NULL);
        PRECONDITION(!ty.IsNull());
    }
    CONTRACTL_END;

    BOOL fCastOK = FALSE;
    
    GCPROTECT_BEGIN(orTP);

    // Make sure the type being cast to has been restored.
    ty.CheckRestore();
    
    MethodTable *pMT = orTP->GetMethodTable();

    // Make sure that we have a transparent proxy
    _ASSERTE(pMT->IsTransparentProxyType());

    pMT = orTP->GetTrueMethodTable();

    // Do a cast check without taking a lock
    fCastOK = CheckCast(orTP, TypeHandle(pMT), ty);

    if(!fCastOK && !ty.IsTypeDesc())
    {
        // We reach here only if any of the types in the current type hierarchy
        // represented by the proxy does not match the given type.     
        // Call a helper routine in managed RemotingServices to find out 
        // whether the server object supports the given type
        MethodDesc* pTargetMD = MDofCheckCast();
        fCastOK = CTPMethodTable::CheckCast(pTargetMD, orTP, ty);
    }

    if (fCastOK)
    {
        // Do the type equivalence tests
        CRealProxy::UpdateOptFlags(orTP);
    }

    GCPROTECT_END();
    
    LOG((LF_REMOTING, LL_INFO100, "CheckCast returning %s for object 0x%x and class 0x%x \n", (fCastOK ? "TRUE" : "FALSE")));

    return (fCastOK);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::NativeCheckCast    public
//
//  Synopsis:   Does a CheckCast to force the expansion of the MethodTable for 
//              the object (possibly a proxy) contained in pvObj.  Returns True if
//              the object is not a proxy or if it can be cast to the specified
//              type.
//+----------------------------------------------------------------------------
FCIMPL2(Object*, CRemotingServices::NativeCheckCast, Object* pObj, ReflectClassBaseObject* pType) 
{
    CONTRACTL
    {
        DISABLED(THROWS);
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(pObj != NULL);
        PRECONDITION(pType != NULL);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // pType may actually represent a type derived from Type other than
    // RuntimeType (in which case we can't handle it). Throw
    // InvalidCastException in this case.
    // Better check we're intialized first (some folks in serialization call
    // this without remoting being involved in any other way). Do this on the
    // error path to keep the mainline fast.
    if (pType->GetMethodTable() != s_pRuntimeTypeClass)
    {
        HELPER_METHOD_FRAME_BEGIN_RET_2(pObj, pType);
        InitRuntimeTypeClass(); // Idempotent, OK (and cheap) to call it more than once.
        if (pType->GetMethodTable() != s_pRuntimeTypeClass)
            COMPlusThrow(kInvalidCastException, L"Arg_ObjObj");
        HELPER_METHOD_FRAME_END();
    }
    
    OBJECTREF orObj(pObj);
    REFLECTCLASSBASEREF typeObj(pType);

    //Get the type of the object which we have and the class to which we're widening.
    TypeHandle ty = typeObj->GetType();
    MethodTable *pMTOfObj = orObj->GetMethodTable();

    //Always initialize retval
    // If it's thunking, check what we actually have.
    if (pMTOfObj->IsThunking())
    {
        HELPER_METHOD_FRAME_BEGIN_RET_1(orObj);
        
        if (!CRemotingServices::CheckCast(orObj, ty))
            orObj = 0;
        
        HELPER_METHOD_FRAME_END();
    } 
    return OBJECTREFToObject(orObj);
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::FieldAccessor   public
//
//  Synopsis:   Sets/Gets the value of the field given an instance or a proxy
//+----------------------------------------------------------------------------
void CRemotingServices::FieldAccessor(FieldDesc* pFD, OBJECTREF o, LPVOID pVal, BOOL fIsGetter)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(o != NULL);
        PRECONDITION(CheckPointer(pVal, NULL_OK));
        PRECONDITION(o->IsThunking() || o->GetMethodTable()->IsMarshaledByRef());
    }
    CONTRACTL_END;

    MethodTable *pMT = o->GetMethodTable();
    TypeHandle fldClass;
    TypeHandle thRealObjectType;

    GCPROTECT_BEGIN(o);
    GCPROTECT_BEGININTERIOR(pVal);

    // If the field descriptor type is not exact (i.e. it's a representative
    // descriptor for a generic field) then we need to be more careful
    // determining the properties of the field.
    if (pFD->IsSharedByGenericInstantiations())
    {
        // We need to resolve the field type in the context of the actual object
        // it belongs to. If we've been handed a proxy we have to go grab the
        // proxied type for this to work.
        thRealObjectType = o->GetTypeHandle();
        if (!pMT->IsMarshaledByRef())
            thRealObjectType = CTPMethodTable::GetClassBeingProxied(o);
        
        // Evaluate the field signature in the type context of the parent object.
        MetaSig sig(pFD, thRealObjectType);
        sig.NextArg();
        fldClass = sig.GetLastTypeHandleThrowing();
    }
    else
    {
        fldClass = pFD->GetFieldTypeHandleThrowing();
    }

    GCPROTECT_END();
    GCPROTECT_END();

    CorElementType fieldType = fldClass.GetSignatureCorElementType();
    UINT cbSize = GetSizeForCorElementType(fieldType);
    BOOL fIsGCRef = CorTypeInfo::IsObjRef(fieldType);
    BOOL fIsByValue = fieldType == ELEMENT_TYPE_VALUETYPE;

    if(pMT->IsMarshaledByRef())
    {
        GCX_FORBID();
        
        _ASSERTE(!o->IsThunking());
    
        // This is a reference to a real object. Get/Set the field value
        // and return
        LPVOID pFieldAddress = pFD->GetAddress((LPVOID)OBJECTREFToObject(o));
        LPVOID pDest = (fIsGetter ? pVal : pFieldAddress);
        LPVOID pSrc  = (fIsGetter ? pFieldAddress : pVal);
        if(fIsGCRef && !fIsGetter)
        {
            SetObjectReference((OBJECTREF*)pDest, ObjectToOBJECTREF(*(Object **)pSrc), o->GetAppDomain());
        }
        else if(fIsByValue) 
        {
            CopyValueClass(pDest, pSrc, fldClass.AsMethodTable(), o->GetAppDomain());
        }
        else
        {    
            CopyDestToSrc(pDest, pSrc, cbSize);
        }
    }
    else
    {
        // Call the managed code to start the field access call
        CallFieldAccessor(pFD, o, pVal, fIsGetter, fIsByValue, fIsGCRef, thRealObjectType, fldClass, fieldType, cbSize);        
    }
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CopyDestToSrc   private
//
//  Synopsis:   Copies the specified number of bytes from the src to dest
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::CopyDestToSrc(LPVOID pDest, LPVOID pSrc, UINT cbSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pDest));
        PRECONDITION(CheckPointer(pSrc));
    }
    CONTRACTL_END;

    switch (cbSize)
    {
        case 1:
            *(volatile INT8*)pDest = *(INT8*)pSrc;
            break;
    
        case 2:
            *(volatile INT16*)pDest = *(INT16*)pSrc;
            break;
    
        case 4:
            *(volatile INT32*)pDest = *(INT32*)pSrc;
            break;
    
        case 8:
            *(volatile INT64*)pDest = *(INT64*)pSrc;
            break;
    
        default:
            UNREACHABLE();
            break;
    }
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CallFieldAccessor   private
//
//  Synopsis:   Sets up the arguments and calls RealProxy::FieldAccessor
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::CallFieldAccessor(FieldDesc* pFD,
                                          OBJECTREF o, 
                                          VOID* pVal, 
                                          BOOL fIsGetter, 
                                          BOOL fIsByValue, 
                                          BOOL fIsGCRef,
                                          TypeHandle ty,
                                          TypeHandle fldTy,
                                          CorElementType fieldType, 
                                          UINT cbSize)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(o != NULL);
        PRECONDITION(CheckPointer(pVal));
    }
    CONTRACTL_END;
    
    //****************************WARNING******************************
    // GC Protect all non-primitive variables
    //*****************************************************************
    
    FieldArgs fieldArgs;
    fieldArgs.obj = NULL;
    fieldArgs.val = NULL;
    fieldArgs.typeName = NULL;
    fieldArgs.fieldName = NULL;    

    GCPROTECT_BEGIN(fieldArgs);
    GCPROTECT_BEGININTERIOR(pVal);

    fieldArgs.obj = o;

    // protect the field value if it is a gc-ref type
    if(fIsGCRef)
        fieldArgs.val = ObjectToOBJECTREF(*(Object **)pVal);


    // Set up the arguments
    
    // Argument 1: String typeName
    // Argument 2: String fieldName
    // Get the type name and field name strings
    GetTypeAndFieldName(&fieldArgs, pFD, ty);
    
    // Argument 3: Object val
    OBJECTREF val = NULL;
    if(!fIsGetter)
    {
        // If we are setting a field value then we create a variant data 
        // structure to hold the field value        
        // Extract the field from the gc protected structure if it is an object
        // else use the value passed to the function
        LPVOID pvFieldVal = (fIsGCRef ? (LPVOID)&(fieldArgs.val) : pVal);
        OBJECTREF *lpVal = &val;
        GCPROTECT_BEGININTERIOR (pvFieldVal);
        CMessage::GetObjectFromStack(lpVal, &pvFieldVal, fieldType, fldTy, TRUE); 
        GCPROTECT_END ();
    }
        
    // Get the method descriptor of the call
    MethodDesc *pMD = (fIsGetter ? MDofFieldGetter() : MDofFieldSetter());
            
    // Call the field accessor function 
    //////////////////////////////// GETTER ///////////////////////////////////
    if(fIsGetter)
    {       
        // Set up the return value
        OBJECTREF oRet = NULL;

        GCPROTECT_BEGIN (oRet);
        CRemotingServices__CallFieldGetter(pMD, 
                             (LPVOID)OBJECTREFToObject(fieldArgs.obj),
                             (LPVOID)OBJECTREFToObject(fieldArgs.typeName),
                             (LPVOID)OBJECTREFToObject(fieldArgs.fieldName),
                             (LPVOID)&(oRet));

        // If we are getting a field value then extract the field value
        // based on the type of the field    
        if(fIsGCRef)
        {
            // Do a check cast to ensure that the field type and the 
            // return value are compatible
            OBJECTREF orRet = oRet;
            OBJECTREF orSaved = orRet;
            if(IsTransparentProxy(OBJECTREFToObject(orRet)))
            {
                GCPROTECT_BEGIN(orRet);

                if(!CheckCast(orRet, fldTy))
                    COMPlusThrow(kInvalidCastException, L"Arg_ObjObj");

                orSaved = orRet;

                GCPROTECT_END();
            }

            *(OBJECTREF *)pVal = orSaved;
        }
        else if (fIsByValue) 
        {       
            // Copy from the source to the destination
            if (oRet != NULL) 
            {
                fldTy.GetMethodTable()->UnBoxIntoUnchecked(pVal, oRet);
            }
        }
        else
        {
            if (oRet != NULL)
                CopyDestToSrc(pVal, oRet->UnBox(), cbSize);
        }    
        GCPROTECT_END ();
    }
    ///////////////////////// SETTER //////////////////////////////////////////
    else
    {    
        CRemotingServices__CallFieldSetter(pMD,
                             (LPVOID)OBJECTREFToObject(fieldArgs.obj), 
                             (LPVOID)OBJECTREFToObject(fieldArgs.typeName), 
                             (LPVOID)OBJECTREFToObject(fieldArgs.fieldName),
                             (LPVOID)OBJECTREFToObject(val));
    }

    GCPROTECT_END(); // pVal
    GCPROTECT_END(); // fieldArgs
}
  
//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetTypeAndFieldName   private
//
//  Synopsis:   Get the type name and field name of the 
//
//+----------------------------------------------------------------------------
VOID CRemotingServices::GetTypeAndFieldName(FieldArgs *pArgs, FieldDesc *pFD, TypeHandle thEnclosingClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pArgs));
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END;

    LPCUTF8 pszFieldName = pFD->GetName();
    TypeHandle thDeclaringType = Generics::GetFieldDeclaringType(thEnclosingClass, pFD);

    // Extract the type name and field name string
    pArgs->typeName = (STRINGREF)TypeString::ConstructName(thDeclaringType, TypeString::FormatNamespace | TypeString::FormatFullInst);
    pArgs->fieldName = COMString::NewString(pszFieldName);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::MatchField   private
//
//  Synopsis:   Find out whether the given field name is the same as the name
//              of the field descriptor field name.
//
//+----------------------------------------------------------------------------
BOOL CRemotingServices::MatchField(FieldDesc* pCurField, LPCUTF8 szFieldName)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pCurField));
        PRECONDITION(CheckPointer(szFieldName));
    }
    CONTRACTL_END;

    // Get the name of the field
    LPCUTF8 pwzCurFieldName = pCurField->GetName();
    
    return strcmp(pwzCurFieldName, szFieldName) == 0;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::Unwrap   public
//
//  Synopsis:   Unwrap a proxy to return the underlying object
//              
//
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRemotingServices::FCUnwrap, Object* pvTP)
{
    LEAF_CONTRACT;
    
    return pvTP;
}
FCIMPLEND    

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::AlwaysUnwrap   public
//
//  Synopsis:   Unwrap a proxy to return the underlying object
//              
//
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRemotingServices::FCAlwaysUnwrap, Object* obj)
{   
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return AlwaysUnwrap(obj);
}
FCIMPLEND

Object* CRemotingServices::AlwaysUnwrap(Object* obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF objRef = (OBJECTREF)obj;

    if(IsTransparentProxy(OBJECTREFToObject(objRef)))
        objRef = GetObjectFromProxy(objRef, TRUE);

    return OBJECTREFToObject(objRef);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::Wrap   public
//
//  Synopsis:   Wrap a contextful object to create a proxy
//              Delegates to a helper method to do the actual work
//
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::Wrap(OBJECTREF obj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // Basic sanity check
    VALIDATEOBJECTREF(obj);

    // ******************* WARNING ********************************************
    // Do not throw any exceptions or provoke GC without setting up a frame.
    // At present its the callers responsibility to setup a frame that can 
    // handle exceptions.
    // ************************************************************************    
    OBJECTREF orProxy = obj;
    if(obj != NULL && (obj->GetMethodTable()->IsContextful()))       
    {
        if(!IsTransparentProxy(OBJECTREFToObject(obj)))
        {
            // See if we can extract the proxy from the object
            orProxy = GetProxyFromObject(obj);
            if(orProxy == NULL)
            {
                // ask the remoting services to wrap the object
                orProxy = CRemotingServices::WrapHelper(obj);
            }
        }
    }

    return orProxy;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::WrapHelper   public
//
//  Synopsis:   Wrap an object to return a proxy. This function assumes that 
//              a fcall frame is already setup.
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::WrapHelper(OBJECTREF obj)
{
    // Basic sanity check
    VALIDATEOBJECTREF(obj);
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(obj != NULL);
        PRECONDITION(!IsTransparentProxy(OBJECTREFToObject(obj)));
        PRECONDITION(obj->GetMethodTable()->IsContextful());
    }
    CONTRACTL_END;
    

    // Default return value indicates an error
    OBJECTREF newobj = NULL;
    MethodDesc* pTargetMD = NULL;
    
    // Ensure remoting has been started.
    EnsureRemotingStarted();

    // Get the address of wrap in managed code        
    pTargetMD = CRemotingServices::MDofWrap();

    // call the managed method to wrap
    newobj = ObjectToOBJECTREF( (Object *)CTPMethodTable::CallTarget(pTargetMD,
                                            (LPVOID)OBJECTREFToObject(obj),
                                            NULL));    

    return newobj;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetProxyFromObject   public
//
//  Synopsis:   Extract the proxy from the field in the 
//              ContextBoundObject class
//              
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::GetProxyFromObject(OBJECTREF obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(obj != NULL);
    }
    CONTRACTL_END;

    // Basic sanity check
    VALIDATEOBJECTREF(obj);

    // We can derive a proxy for contextful types only.
    _ASSERTE(obj->GetMethodTable()->IsContextful());

    OBJECTREF srvID = (OBJECTREF)(Object*)obj->GetPtrOffset(s_dwMBRIDOffset);
    OBJECTREF orProxy = NULL;
    
    if (srvID != NULL)
        orProxy = (OBJECTREF)(Object*)srvID->GetPtrOffset(s_dwTPOrObjOffsetInIdentity);

    // This should either be null or a proxy type
    _ASSERTE((orProxy == NULL) || IsTransparentProxy(OBJECTREFToObject(orProxy)));

    return orProxy;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::IsProxyToRemoteObject   public
//
//  Synopsis:   Check if the proxy is to a remote object
//              (1) TRUE : if object is non local (ie outside this PROCESS) otherwise
//              (2) FALSE 
//+----------------------------------------------------------------------------
BOOL CRemotingServices::IsProxyToRemoteObject(OBJECTREF obj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(obj != NULL);
    }
    CONTRACTL_END;
    
    // Basic sanity check
    VALIDATEOBJECTREF(obj);
 
    // If remoting is not started, for now let us just return FALSE
    if(!s_fRemotingStarted)
        return FALSE;
 
    if(!obj->GetMethodTable()->IsTransparentProxyType())
        return FALSE;
    
    // so it is a transparent proxy
    AppDomain *pDomain = GetServerDomainForProxy(obj);
    if(pDomain != NULL)
        return TRUE;

    return FALSE;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetObjectFromProxy   public
//
//  Synopsis:   Extract the object given a proxy. 
//              fMatchContexts if
//              (1) TRUE It matches the current context with the server context
//              and if they match then returns the object else the proxy
//              (2) FALSE returns the object without matching the contexts.
//              WARNING!! This should be used by code which is context-aware.
//              
//              
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::GetObjectFromProxy(OBJECTREF obj, BOOL fMatchContexts)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(obj != NULL);
        PRECONDITION(s_fRemotingStarted);
        PRECONDITION(IsTransparentProxy(OBJECTREFToObject(obj)));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Basic sanity check
    VALIDATEOBJECTREF(obj);

    OBJECTREF oref = NULL;
    if (CTPMethodTable__GenericCheckForContextMatch(OBJECTREFToObject(obj)))
    {
        OBJECTREF objRef = ObjectToOBJECTREF(GetRealProxy(OBJECTREFToObject(obj)));
        oref = (OBJECTREF)(Object*)objRef->GetPtrOffset(s_dwServerOffsetInRealProxy);
        if (oref != NULL)
            obj = oref; 
    }

    return obj;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetServerIdentityFromProxy   private
//
//  Synopsis:   Gets the server identity (if one exists) from a proxy
//              
//              
//              
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::GetServerIdentityFromProxy(OBJECTREF obj)
{    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(obj != NULL);
        PRECONDITION(IsTransparentProxy(OBJECTREFToObject(obj)));
    }
    CONTRACTL_END;


    // Extract the real proxy underlying the transparent proxy
    OBJECTREF pObj = ObjectToOBJECTREF(GetRealProxy(OBJECTREFToObject(obj)));

    OBJECTREF id = NULL;
        
    // Extract the identity object
    pObj = (OBJECTREF)(Object*)pObj->GetPtrOffset(s_dwIdOffset);

    // Extract the _identity from the real proxy only if it is an instance of 
    // remoting proxy
    if((pObj != NULL) && IsInstanceOfServerIdentity(pObj->GetMethodTable()))
        id = pObj;

    return id;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetServerDomainForProxy public
//
//  Synopsis:   Returns the AppDomain corresponding to the server
//              if the proxy and the server are in the same process.
//              
//+----------------------------------------------------------------------------
AppDomain *CRemotingServices::GetServerDomainForProxy(OBJECTREF proxy)
{
    CONTRACT (AppDomain*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(proxy != NULL);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    // call the managed method 
    Context *pContext = (Context *)GetServerContextForProxy(proxy);
    if (pContext)
        RETURN pContext->GetDomain();
    else 
        RETURN NULL; 
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetServerDomainIdForProxy public
//
//  Synopsis:   Returns the AppDomain ID corresponding to the server
//              if the proxy and the server are in the same process.
//              Returns 0 if it cannot determine.
//              
//+----------------------------------------------------------------------------
int CRemotingServices::GetServerDomainIdForProxy(OBJECTREF proxy)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(proxy != NULL);
        PRECONDITION(IsTransparentProxy(OBJECTREFToObject(proxy)));
    }
    CONTRACTL_END;

    // Get the address of GetDomainIdForProxy in managed code
    MethodDesc* pTargetMD = CRemotingServices::MDofGetServerDomainIdForProxy();

    // This will just read the appDomain ID from the marshaled data
    // for the proxy. It returns 0 if the proxy is to a server in another
    // process. It may also return 0 if it cannot determine the server
    // domain ID (eg. for Well Known Object proxies).

    // call the managed method
    return (int)(INT_PTR)CTPMethodTable::CallTarget(
                pTargetMD,
                (LPVOID)OBJECTREFToObject(proxy),
                NULL);
}


//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetServerContextForProxy public
//
//  Synopsis:   Returns the AppDomain corresponding to the server
//              if the proxy and the server are in the same process.
//              
//+----------------------------------------------------------------------------
Context *CRemotingServices::GetServerContextForProxy(OBJECTREF proxy)
{
    CONTRACT (Context*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(proxy != NULL);
        PRECONDITION(IsTransparentProxy(OBJECTREFToObject(proxy)));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
 
    // Get the address of GetAppDomainForProxy in managed code        
    MethodDesc* pTargetMD = CRemotingServices::MDofGetServerContextForProxy();
    
    // This will return the correct VM Context object for the server if 
    // the proxy is true cross domain proxy to a server in another domain 
    // in the same process. The managed method will Assert if called on a proxy
    // which is either half-built or does not have an ObjRef ... which may
    // happen for eg. if the proxy and the server are in the same appdomain.

    // we return NULL if the server object for the proxy is in another 
    // process or if the appDomain for the server is invalid or if we cannot
    // determine the context (eg. well known object proxies).

    // call the managed method 
    RETURN (Context *)CTPMethodTable::CallTarget(
                            pTargetMD,
                            (LPVOID)OBJECTREFToObject(proxy),
                            NULL);    
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CreateProxyForDomain   public
//
//  Synopsis:   Create a proxy for the app domain object by calling marshal
//              inside the newly created domain and unmarshaling in the old
//              domain
//              
//+----------------------------------------------------------------------------
OBJECTREF CRemotingServices::CreateProxyForDomain(AppDomain* pDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDomain));
    }
    CONTRACTL_END;

    // Ensure remoting has been started.
    EnsureRemotingStarted();

    MethodDesc* pTargetMD = MDOfCreateProxyForDomain();

    // Call the managed method which will marshal and unmarshal the 
    // appdomain object to create the proxy

    // We pass the ContextID of the default context of the new appDomain
    // object. This helps the boot-strapping! (i.e. entering the new domain
    // to marshal itself out).

    Object *proxy = (Object *)CTPMethodTable::CallTarget(
                                    pTargetMD, 
                                    (LPVOID)(DWORD_PTR)pDomain->GetId().m_dwId,
                                    (LPVOID)pDomain->GetDefaultContext());
    return ObjectToOBJECTREF(proxy);
}

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetClass   public
//
//  Synopsis:   Extract the true class of the object whose proxy is given.
//              
//              
//+----------------------------------------------------------------------------
REFLECTCLASSBASEREF CRemotingServices::GetClass(OBJECTREF pThis)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(pThis != NULL);
    }
    CONTRACTL_END;
    
    REFLECTCLASSBASEREF refClass = NULL;
    MethodTable *pMT = NULL;

    GCPROTECT_BEGIN(pThis);

    // For proxies to objects in the same appdomain, we always know the
    // correct type
    if(GetServerIdentityFromProxy(pThis) != NULL)
    {
        pMT = pThis->GetTrueMethodTable();
    }
    else
    {   
        // For everything else either we have refined the proxy to its correct type
        // or we have to consult the objref to get the true type

        MethodDesc* pTargetMD = CRemotingServices::MDofGetType();

        refClass = (REFLECTCLASSBASEREF)(ObjectToOBJECTREF((Object *)CTPMethodTable::CallTarget(pTargetMD, 
            (LPVOID)OBJECTREFToObject(pThis), NULL)));

        if(refClass == NULL)
        {
            // There was no objref associated with the proxy or it is a proxy
            // that we do not understand. 
            // In this case, we return the class that is stored in the proxy
            pMT = pThis->GetTrueMethodTable();
        }

        _ASSERTE(refClass != NULL || pMT != NULL);

        // Refine the proxy to the class just retrieved
        if(refClass != NULL)
        {
            CTPMethodTable::RefineProxy(pThis,refClass->GetType());
        }
    }    

    if (refClass == NULL)
    {
        PREFIX_ASSUME(pMT != NULL);
        refClass = (REFLECTCLASSBASEREF)pMT->GetManagedClassObject();
    }

    GCPROTECT_END();

    _ASSERTE(refClass != NULL);
    return refClass;
}

//+----------------------------------------------------------------------------
//
//  Method:     CRealProxy::SetStubData   public
//
//  Synopsis:   Set the stub data in the transparent proxy
//+----------------------------------------------------------------------------
FCIMPL2(VOID, CRealProxy::SetStubData, LPVOID pvRP, LPVOID pvStubData)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pvRP));
        PRECONDITION(CheckPointer(pvStubData));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    BOOL fThrow = FALSE;
    OBJECTREF orRP = ObjectToOBJECTREF((Object *)pvRP);    
    
    if(orRP != NULL)
    {
        OBJECTREF orTP = ObjectToOBJECTREF((Object *)orRP->GetPtrOffset(CRemotingServices::GetTPOffset()));
        if(orTP != NULL)
        {
            orTP->SetOffsetObjectRef(
                    CTPMethodTable::GetOffsetOfStubData(), 
                    (size_t)pvStubData);
        }
        else
        {
            fThrow = TRUE;
        }
    }
    else
    {
        fThrow = TRUE;
    }
    
    if(fThrow)
        FCThrowVoid(kArgumentNullException);
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRealProxy::GetStubData   public
//
//  Synopsis:   Get the stub data in the transparent proxy
//+----------------------------------------------------------------------------
FCIMPL1(LPVOID, CRealProxy::GetStubData, LPVOID pvRP)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pvRP));
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
    BOOL fThrow = FALSE;
    OBJECTREF orRP = ObjectToOBJECTREF((Object *)pvRP);    
    LPVOID pvRet = NULL;

    if(orRP != NULL)
    {
        OBJECTREF orTP = ObjectToOBJECTREF((Object *)orRP->GetPtrOffset(CRemotingServices::GetTPOffset()));
        if(orTP != NULL)
            pvRet = (LPVOID)orTP->GetPtrOffset(CTPMethodTable::GetOffsetOfStubData());
        else
            fThrow = TRUE;
    }
    else
    {
        fThrow = TRUE;
    }
    
    if(fThrow)
        FCThrow(kArgumentNullException);

    return pvRet;
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRealProxy::GetDefaultStub   public
//
//  Synopsis:   Get the default stub implemented by us which matches contexts
//+----------------------------------------------------------------------------
FCIMPL0(LPVOID, CRealProxy::GetDefaultStub)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (LPVOID)CRemotingServices__CheckForContextMatch;
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRealProxy::GetStub   public
//
//  Synopsis:   Get the stub pointer in the transparent proxy 
//+----------------------------------------------------------------------------
FCIMPL1(ULONG_PTR, CRealProxy::GetStub, LPVOID pvRP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pvRP));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    ULONG_PTR stub = 0;
    OBJECTREF orRP = ObjectToOBJECTREF((Object *)pvRP);    
    OBJECTREF orTP = ObjectToOBJECTREF((Object *)orRP->GetPtrOffset(CRemotingServices::GetTPOffset()));
            
    stub = (ULONG_PTR)orTP->GetPtrOffset(CTPMethodTable::GetOffsetOfStub()); 

    return stub;
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CRealProxy::GetProxiedType   public
//
//  Synopsis:   Get the type that is represented by the transparent proxy 
//+----------------------------------------------------------------------------
FCIMPL1(Object*, CRealProxy::GetProxiedType, Object* orRPUNSAFE)
{
    REFLECTCLASSBASEREF refClass = NULL;
    OBJECTREF orRP = (OBJECTREF) orRPUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, orRP);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF orTP = ObjectToOBJECTREF((Object *)orRP->GetPtrOffset(CRemotingServices::GetTPOffset()));
    
    refClass = CRemotingServices::GetClass(orTP);
    _ASSERTE(refClass != NULL);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refClass);
}
FCIMPLEND

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::Initialize   public
//
//  Synopsis:   Initialized data structures needed for managing tranparent
//              proxies
//+----------------------------------------------------------------------------
VOID CTPMethodTable::Initialize()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;
    
    // Init    
    s_dwCommitedTPSlots = 0;
    s_dwReservedTPSlots = 0;
    s_pThunkTable = NULL;
    s_pTransparentProxyClass = NULL;
    s_dwGCInfoBytes = 0;
    s_dwMTDataSlots = 0;
    s_dwRPOffset = 0;
    s_dwMTOffset = 0;
    s_dwItfMTOffset = 0;
    s_dwStubOffset = 0;
    s_dwStubDataOffset = 0;
    s_dwMaxSlots = 0;
    s_pTPMT = NULL;    
    s_pTPStub = NULL;    
    s_pDelegateStub = NULL;    
    s_fTPTableFieldsInitialized = FALSE;

    // Initialize the thunks
    CVirtualThunks::Initialize();
#ifndef HAS_REMOTING_PRECODE
    CNonVirtualThunk::Initialize();
#endif

    s_TPMethodTableCrst.Init("s_TPMethodTableCrst", CrstTPMethodTable);
}


//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::EnsureFieldsInitialized    private
//
//  Synopsis:   Initialize the static fields of CTPMethodTable class
//              and the thunk manager classes
//
//+----------------------------------------------------------------------------
void CTPMethodTable::EnsureFieldsInitialized()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(s_fTPTableFieldsInitialized);
    }
    CONTRACT_END;

    if (!s_fTPTableFieldsInitialized)
    {
        // Load Tranparent proxy class (do this before we enter the critical section)
        s_pTransparentProxyClass = g_Mscorlib.GetClass(CLASS__TRANSPARENT_PROXY)->GetClass();

        CrstPreempHolder ch(&s_TPMethodTableCrst);

        if(!s_fTPTableFieldsInitialized)
        {

            s_pTPMT = s_pTransparentProxyClass->GetMethodTable();
            s_pTPMT->SetTransparentProxyType();

            // Obtain size of GCInfo stored above the method table
            CGCDesc *pGCDesc = CGCDesc::GetCGCDescFromMT(s_pTPMT);
            BYTE *pGCTop = (BYTE *) pGCDesc->GetLowestSeries();
            s_dwGCInfoBytes = (DWORD)(((BYTE *) s_pTPMT) - pGCTop);
            _ASSERTE((s_dwGCInfoBytes & 3) == 0);

            // Obtain the number of bytes to be copied for creating the TP
            // method tables containing thunks
            _ASSERTE(((s_dwGCInfoBytes + sizeof(MethodTable)) & (sizeof(SLOT)-1)) == 0);
            s_dwMTDataSlots = ((s_dwGCInfoBytes + sizeof(MethodTable)) / sizeof(SLOT));
            _ASSERTE(sizeof(MethodTable) == MethodTable::GetVtableOffset());

            // We rely on the number of interfaces implemented by the
            // Transparent proxy being 0, so that InterfaceInvoke hints
            // fail and trap to InnerFailStub which also fails and
            // in turn traps to FailStubWorker. In FailStubWorker, we
            // determine the class being proxied and return correct slot.
            _ASSERTE(s_pTPMT->GetNumInterfaces() == 0);

            // Calculate offsets to various fields defined by the
            // __Transparent proxy class
            s_dwRPOffset        = TransparentProxyObject::GetOffsetOfRP() - Object::GetOffsetOfFirstField();
            s_dwMTOffset        = TransparentProxyObject::GetOffsetOfMT() - Object::GetOffsetOfFirstField();
            s_dwItfMTOffset     = TransparentProxyObject::GetOffsetOfInterfaceMT() - Object::GetOffsetOfFirstField();
            s_dwStubOffset      = TransparentProxyObject::GetOffsetOfStub() - Object::GetOffsetOfFirstField();
            s_dwStubDataOffset  = TransparentProxyObject::GetOffsetOfStubData() - Object::GetOffsetOfFirstField();

            _ASSERTE(s_dwStubDataOffset == (TP_OFFSET_STUBDATA  - sizeof(MethodTable*)));
            _ASSERTE(s_dwMTOffset       == (TP_OFFSET_MT        - sizeof(MethodTable*)));
            _ASSERTE(s_dwStubOffset     == (TP_OFFSET_STUB      - sizeof(MethodTable*)));

            // Create the one and only transparent proxy stub
            s_pTPStub = CreateTPStub();
            _ASSERTE(s_pTPStub);

            // Create the one and only delegate stub
            s_pDelegateStub = CreateDelegateStub();
            _ASSERTE(s_pDelegateStub);

            // Set the largest possible vtable size 64K
            s_dwMaxSlots = 64*1024;

            // Create the global thunk table and set the cycle between
            // the transparent proxy class and the global thunk table
            CreateTPMethodTable();

#ifdef HAS_REMOTING_PRECODE
            // Activate the remoting precode helper
            ActivatePrecodeRemotingThunk();
#endif // HAS_REMOTING_PRECODE

            // NOTE: This must always be the last statement in this block
            // to prevent races
            // Load Tranparent proxy class
            s_fTPTableFieldsInitialized = TRUE;
        }
    }
    
    RETURN;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::GetRP       public
//
//  Synopsis:   Get the real proxy backing the transparent proxy
//
//+----------------------------------------------------------------------------
OBJECTREF CTPMethodTable::GetRP(OBJECTREF orTP) 
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(orTP != NULL);
        PRECONDITION(s_fTPTableFieldsInitialized);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (OBJECTREF)(Object*)orTP->GetPtrOffset(s_dwRPOffset);
}

#define PAGE_ROUND_UP(cb) (((cb) + g_SystemInfo.dwPageSize) & ~(g_SystemInfo.dwPageSize - 1))

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateTPMethodTable   private
//
//  Synopsis:   (1) Reserves a transparent proxy method table that is large 
//              enough to support the largest vtable
//              (2) Commits memory for the GC info of the global thunk table and
//              sets the cycle between the transparent proxy class and the 
//              globale thunk table.
//+----------------------------------------------------------------------------

void CTPMethodTable::CreateTPMethodTable()
{
    CONTRACT_VOID {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(s_pThunkTable));
    } CONTRACT_END;
    
    
    DWORD dwReserveSize = 0;
    DWORD dwMethodTableReserveSize = (DWORD)(s_dwMTDataSlots * sizeof(SLOT));
    dwMethodTableReserveSize += (DWORD)(s_dwMaxSlots * sizeof(SLOT));
    dwReserveSize = PAGE_ROUND_UP(dwMethodTableReserveSize);

    void *pAlloc = ::ClrVirtualAlloc(0, dwReserveSize, MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
    
    if (pAlloc)
    {
        BOOL bFailed = TRUE;

        // Make sure that we have not created the one and only
        // transparent proxy method table before
        _ASSERTE(NULL == s_pThunkTable);

        // Commit the required amount of memory
        DWORD dwCommitSize = 0;

        // MethodTable memory
        DWORD dwMethodTableCommitSize = (s_dwMTDataSlots) * sizeof(SLOT);
        dwCommitSize += dwMethodTableCommitSize;


        if (::ClrVirtualAlloc(pAlloc, dwCommitSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
        {
            // Copy the fixed portion from the true TP Method Table
            memcpy(pAlloc,MTToAlloc(s_pTPMT, s_dwGCInfoBytes), (dwMethodTableCommitSize));

            // Initialize the transparent proxy method table
            InitThunkTable(0, s_dwMaxSlots, AllocToMT((BYTE *) pAlloc, s_dwGCInfoBytes));


            // At this point the transparent proxy class points to the
            // the true TP Method Table and not the transparent 
            // proxy method table. We do not use the true method table
            // any more. Instead we use the transparent proxy method table
            // for allocating transparent proxies. So, we have to make the
            // transparent proxy class point to the one and only transparent 
            // proxy method table
            CTPMethodTable::s_pTransparentProxyClass->SetMethodTableForTransparentProxy(s_pThunkTable);

            // Allocate the slots of the Object class method table because
            // we can reflect on the __Transparent proxy class even though 
            // we never intend to use remoting.
            _ASSERTE(NULL != g_pObjectClass);
            _ASSERTE(0 == GetCommitedTPSlots());
            if(ExtendCommitedSlots(g_pObjectClass->GetNumMethods()))
                bFailed = FALSE;
        }
        else
        {
            ClrVirtualFree(pAlloc, 0, MEM_RELEASE);
        }
        
        if(bFailed)
            DestroyThunkTable();
    }
    else {
        if (pAlloc != NULL)
            ::ClrVirtualFree(pAlloc, 0, MEM_RELEASE);
    }

    // Note that the thunk table is set to null on any failure path
    // via DestroyThunkTable
    if (!s_pThunkTable)
        COMPlusThrowOM();

    RETURN;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::ExtendCommitedSlots   private
//
//  Synopsis:   Extends the commited slots of transparent proxy method table to
//              the desired number
//+----------------------------------------------------------------------------
BOOL CTPMethodTable::ExtendCommitedSlots(DWORD dwSlots)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return FALSE);
        PRECONDITION(s_dwCommitedTPSlots <= dwSlots);
        PRECONDITION(dwSlots <= s_dwReservedTPSlots);
        PRECONDITION((CVirtualThunks::GetVirtualThunks() == NULL) || 
                (s_dwCommitedTPSlots == CVirtualThunks::GetVirtualThunks()->_dwCurrentThunk));
        
        // Either we have initialized everything or we are asked to allocate
        // some slots during initialization
        PRECONDITION(s_fTPTableFieldsInitialized || (0 == s_dwCommitedTPSlots));
    }
    CONTRACTL_END;
    
    // Commit memory for TPMethodTable
    BOOL bAlloc = FALSE;
    void *pAlloc = MTToAlloc(s_pThunkTable, s_dwGCInfoBytes);
    DWORD dwCommitSize = s_dwMTDataSlots * sizeof(SLOT);
    dwCommitSize += dwSlots * sizeof(SLOT);
    
    if (::ClrVirtualAlloc(pAlloc, dwCommitSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
    {
        s_pThunkTable->SetNumVirtuals(dwSlots);
        s_pThunkTable->SetNumMethods(dwSlots);


        bAlloc = AllocateThunks(dwSlots, dwCommitSize);
        if (!bAlloc)
            ClrVirtualFree(pAlloc, dwCommitSize, MEM_DECOMMIT);
    }

    return bAlloc;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::AllocateThunks   private
//
//  Synopsis:   Allocates the desired number of thunks for virtual methods
//+----------------------------------------------------------------------------
BOOL CTPMethodTable::AllocateThunks(DWORD dwSlots, DWORD dwCommitSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    // Check for existing thunks
    DWORD dwCommitThunks = 0;
    DWORD dwAllocThunks = dwSlots;
    MethodTable *pThunkTable = s_pThunkTable;
    
    CVirtualThunks* pThunks = CVirtualThunks::GetVirtualThunks();
    if (pThunks)
    {
        // Compute the sizes of memory to be commited and allocated
        BOOL fCommit;
        if (dwSlots < pThunks->_dwReservedThunks)
        {
            fCommit = TRUE;
            dwCommitThunks = dwSlots;
            dwAllocThunks = 0;
        } 
        else
        {
            fCommit = (pThunks->_dwCurrentThunk != pThunks->_dwReservedThunks);
            dwCommitThunks = pThunks->_dwReservedThunks;
            dwAllocThunks = dwSlots - pThunks->_dwReservedThunks;
        }

        // Commit memory if needed
        if (fCommit)
        {
            DWORD dwCommitSizeTmp = (sizeof(CVirtualThunks) - ConstVirtualThunkSize) +
                                 ((dwCommitThunks - pThunks->_dwStartThunk) * ConstVirtualThunkSize);
            
            if (!::ClrVirtualAlloc(pThunks, dwCommitSizeTmp, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
                return(NULL);

            // Generate thunks that push slot number and jump to TP stub
            DWORD dwStartSlot = pThunks->_dwStartThunk;
            DWORD dwCurrentSlot = pThunks->_dwCurrentThunk;
            while (dwCurrentSlot < dwCommitThunks)
            {
                pThunkTable->SetSlot(dwCurrentSlot, (SLOT) &pThunks->ThunkCode[dwCurrentSlot-dwStartSlot]);
                CreateThunkForVirtualMethod(dwCurrentSlot, (BYTE *) pThunkTable->GetSlot(dwCurrentSlot));
                ++dwCurrentSlot;
            }
            FlushInstructionCache(GetCurrentProcess(), 
                                  &pThunks->ThunkCode[pThunks->_dwCurrentThunk-dwStartSlot], 
                                  (dwCommitThunks-pThunks->_dwCurrentThunk)*ConstVirtualThunkSize);
            s_dwCommitedTPSlots = dwCommitThunks;
            pThunks->_dwCurrentThunk = dwCommitThunks;
        }
    }


    // Allocate memory if necessary
    if (dwAllocThunks)
    {
        DWORD dwReserveSize = ((sizeof(CVirtualThunks) - ConstVirtualThunkSize) +
                               ((dwAllocThunks << 1) * ConstVirtualThunkSize) +
                               g_SystemInfo.dwPageSize) & ~((size_t) g_SystemInfo.dwPageSize - 1);
        
        void *pAlloc = ::ClrVirtualAlloc(0, dwReserveSize,
                                      MEM_RESERVE | MEM_TOP_DOWN,
                                      PAGE_EXECUTE_READWRITE);
        if (pAlloc)
        {
            // Commit the required amount of memory
            DWORD dwCommitSizeTmp = (sizeof(CVirtualThunks) - ConstVirtualThunkSize) +
                                 (dwAllocThunks * ConstVirtualThunkSize);
            
            if (::ClrVirtualAlloc(pAlloc, dwCommitSizeTmp, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
            {
                ((CVirtualThunks *) pAlloc)->_pNext = pThunks;
                pThunks = CVirtualThunks::SetVirtualThunks((CVirtualThunks *) pAlloc);
                pThunks->_dwReservedThunks = (dwReserveSize -
                                             (sizeof(CVirtualThunks) - ConstVirtualThunkSize)) /
                                                 ConstVirtualThunkSize;
                pThunks->_dwStartThunk = dwCommitThunks;
                pThunks->_dwCurrentThunk = dwCommitThunks;

                // Generate thunks that push slot number and jump to TP stub
                DWORD dwStartSlot = pThunks->_dwStartThunk;
                DWORD dwCurrentSlot = pThunks->_dwCurrentThunk;
                while (dwCurrentSlot < dwSlots)
                {
                    pThunkTable->SetSlot(dwCurrentSlot, (SLOT) &pThunks->ThunkCode[dwCurrentSlot-dwStartSlot]);
                    CreateThunkForVirtualMethod(dwCurrentSlot, (BYTE *) pThunkTable->GetSlot(dwCurrentSlot));
                    ++dwCurrentSlot;
                }
                FlushInstructionCache(GetCurrentProcess(), 
                                      &pThunks->ThunkCode[pThunks->_dwCurrentThunk-dwStartSlot], 
                                      (dwSlots-pThunks->_dwCurrentThunk)*ConstVirtualThunkSize);
                s_dwCommitedTPSlots = dwSlots;
                pThunks->_dwCurrentThunk = dwSlots;
            }
            else
            {
                ::ClrVirtualFree(pAlloc, 0, MEM_RELEASE);
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateTPOfClassForRP   private
//
//  Synopsis:   Creates a transparent proxy that behaves as an object of the
//              supplied class
//+----------------------------------------------------------------------------
void CTPMethodTable::CreateTPOfClassForRP(TypeHandle ty, OBJECTREF *pRP, OBJECTREF *pTP)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(!ty.IsNull());
        PRECONDITION(pRP != NULL);
        PRECONDITION(*pRP != NULL);
        PRECONDITION(pTP != NULL);
        POSTCONDITION(*pTP != NULL);
    }
    CONTRACT_END;
    
    // Ensure remoting is started.
    EnsureFieldsInitialized();

    // Get the size of the VTable for the class to proxy
        DWORD dwSlots = ty.GetNumVirtuals();
    if (dwSlots == 0)
        dwSlots = 1;

    // The global thunk table must have been initialized
    _ASSERTE(s_pThunkTable != NULL);

    // Check for the need to extend existing TP method table
    if (dwSlots > GetCommitedTPSlots())
    {            
        CrstPreempHolder ch(&s_TPMethodTableCrst);

        if (dwSlots > GetCommitedTPSlots())
        {
            if (!ExtendCommitedSlots(dwSlots))
                COMPlusThrowOM();
        }
    }

    // Create a TP Object
    IfNullThrow(*pTP = FastAllocateObject(GetMethodTable()));

    // Create the cycle between TP and RP
    (*pRP)->SetOffsetObjectRef(CRemotingServices::GetTPOffset(), (size_t)OBJECTREFToObject(*pTP));

    // Make the TP behave as an object of supplied class
    (*pTP)->SetOffsetObjectRef(s_dwRPOffset, (size_t) OBJECTREFToObject(*pRP));

    // If we are creating a proxy for an interface then the class
    // is the object class else it is the class supplied
    if(ty.IsInterface())
    {
        _ASSERTE(NULL != g_pObjectClass);

        //pTP->SetOffset32(s_dwMTOffset, (DWORD)g_pObjectClass);
        (*pTP)->SetOffsetPtr(s_dwMTOffset, CRemotingServices::GetMarshalByRefClass());

        // Set the cached interface method table to the given interface
        // method table
        (*pTP)->SetOffsetPtr(s_dwItfMTOffset, ty.GetMethodTable()); 
    } 
    else
    {
        (*pTP)->SetOffsetPtr(s_dwMTOffset, ty.GetMethodTable()); 
    }

    RETURN;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::PreCall   private
//
//  Synopsis:   This function replaces the slot number with the function
//              descriptor thus completely setting up the frame
//              
//+----------------------------------------------------------------------------
void __stdcall CTPMethodTable::PreCall(TPMethodFrame* pFrame AMD64_ARG(Object *_pTP))
{
    BEGIN_ENTRYPOINT_THROWS;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(s_fTPTableFieldsInitialized != NULL);
        SO_TOLERANT;
    }
    CONTRACTL_END;


    DWORD dwSlot = (DWORD) pFrame->GetSlotNumber();

    // For virtual calls the slot number is pushed but for 
    // non virtual calls/interface invoke the method descriptor is already 
    // pushed
    if((DWORD) -1 != dwSlot)
    {
        // The frame is not completly setup at this point.
        // Do not throw exceptions or provoke GC
        OBJECTREF pTP;

#if defined(_PPC_)

        // On PPC, we can't call GetThis() because of the RegisterHelper has not
        // been called yet. "this" is either r3 or r4 depending on whether the method
        // has return buffer. The methods with return buffer are marked
        // with "oris r0, r0, 0" (=0x64000000) on the callsite
        PPCReg regThis = (*(DWORD*)pFrame->GetReturnAddress() == 0x64000000) ? kR4 : kR3;

        ArgumentRegisters* pRegArgs = (ArgumentRegisters*)
            ((BYTE*)pFrame + FramedMethodFrame::GetOffsetOfArgumentRegisters());

        pTP = *(OBJECTREF*)(&pRegArgs->r[regThis - kR3]);
#else // !defined(_PPC_)
        pTP = pFrame->GetThis();
#endif // !defined(_PPC_)

        MethodTable* pMT = (MethodTable*)(pTP->GetPtrOffset(s_dwMTOffset));
        _ASSERTE(pMT);
        
        // Replace the slot number with the method descriptor on the stack
        MethodDesc *pMD = pMT->GetMethodDescForSlot(dwSlot);
        pFrame->SetFunction(pMD);
    }

#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
    //we need to zero out the return value buffer because we will report them during GC
    ZeroMemory (pFrame->GetReturnValuePtr(), ENREGISTERED_RETURNTYPE_MAXSIZE);
#endif //ENREGISTERED_RETURNTYPE_MAXSIZE

#ifdef _PPC_
    FramedMethodFrame::RegisterHelper(pFrame, FALSE);
#endif
    END_ENTRYPOINT_THROWS;

}

void InitMessageData(messageData *msgData, 
                     FramedMethodFrame *pFrame, 
                     Module **ppModule, 
                     SigTypeContext *pTypeContext,
                     PCCOR_SIGNATURE *ppSig,
                     DWORD *pcbSigSize)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(msgData));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(pTypeContext));
        PRECONDITION(CheckPointer(ppSig));
        PRECONDITION(CheckPointer(pcbSigSize));
    }
    CONTRACTL_END;
    
    msgData->pFrame = pFrame;
    msgData->iFlags = 0;

    MethodDesc *pMD = pFrame->GetFunction();
    _ASSERTE(!pMD->ContainsGenericVariables());
    _ASSERTE(pMD->IsRuntimeMethodHandle());

    TypeHandle thGoverningType;
    BOOL fIsDelegate = pMD->GetMethodTable()->IsAnyDelegateClass();

    // We want to calculate and store a governing type for the method since
    // sometimes the parent method table might be representative. We get the
    // exact type context from the this reference we're calling on (adjusting
    // for the fact it's a TP).

    // But cope with the common cases first for speed:
    // * If the method is not on a generic type and this is not the async
    //   delegate case (which requires us to unwrap the delegate and have a
    //   look) then we know the method desc's parent method table will be exact.
    // * We require method descs to be exact for the interface case as well (since
    //   the target object doesn't help us resolve the interface type at all).
    // * COM interop can use this code path, but that doesn't support generics so
    //   we can use the quick logic for that too.
    if ((!pMD->HasClassInstantiation() && !fIsDelegate) ||
        pMD->IsInterface() ||
        pMD->IsComPlusCall())
    {
        thGoverningType = TypeHandle(pMD->GetMethodTable());
    }
    else
    {
        OBJECTREF refTarget;
        MethodDesc *pTargetMD;
        MethodTable *pTargetMT;
        if (fIsDelegate)
        {
            // Async delegates are also handled differently in that the method and the
            // this are delegate wrappers round the real method and target.
            fIsDelegate = TRUE;
            pTargetMD = COMDelegate::GetMethodDesc(pFrame->GetThis());

            // Delegates on static methods don't have a useful target instance.
            // But in that case the target method is guaranteed to have exact
            // type information.
            if (pTargetMD->IsStatic())
                pTargetMT = pTargetMD->GetMethodTable();
            else
            {
                refTarget = COMDelegate::GetTargetObject(pFrame->GetThis());
                if (refTarget->IsThunking())
                    pTargetMT = (MethodTable*)refTarget->GetPtrOffset(CTPMethodTable::GetOffsetOfMT());
                else
                    pTargetMT = refTarget->GetMethodTable();
            }
        }
        else
        {
            refTarget = pFrame->GetThis();
            pTargetMD = pMD;
            pTargetMT = (MethodTable*)refTarget->GetPtrOffset(CTPMethodTable::GetOffsetOfMT());
        }

        // One last check to see if we can optimize the delegate case now we've
        // unwrapped it.
        if (fIsDelegate && !pTargetMD->HasClassInstantiation() && !pTargetMT->IsAnyDelegateClass())
        {
            thGoverningType = TypeHandle(pTargetMD->GetMethodTable());
        }
        else
        {
            // Not quite done yet, we need to get the type that declares the method,
            // which may be a superclass of the type we're calling on.
            TypeHandle *pInst = pTargetMD->GetExactClassInstantiation(TypeHandle(pTargetMT));
            MethodTable *pDeclaringMT = pTargetMD->GetMethodTable();
            thGoverningType = ClassLoader::LoadGenericInstantiationThrowing(pDeclaringMT->GetModule(),
                                                                            pDeclaringMT->GetCl(),
                                                                            pDeclaringMT->GetNumGenericArgs(),
                                                                            pInst);
        }
    }

    msgData->thGoverningType = thGoverningType;

    if (fIsDelegate)
    {
        DelegateEEClass* delegateCls = (DelegateEEClass*) pMD->GetMethodTable()->GetClass();

        _ASSERTE(pFrame->GetThis()->GetMethodTable()->IsAnyDelegateClass());

        msgData->pDelegateMD = pMD;
        msgData->pMethodDesc = COMDelegate::GetMethodDesc(pFrame->GetThis());
        
        _ASSERTE(msgData->pMethodDesc != NULL);
        _ASSERTE(!msgData->pMethodDesc->ContainsGenericVariables());
        _ASSERTE(msgData->pMethodDesc->IsRuntimeMethodHandle());

        if (pMD == delegateCls->m_pBeginInvokeMethod)
        {
            msgData->iFlags |= MSGFLG_BEGININVOKE;
        }
        else
        {
            _ASSERTE(pMD == delegateCls->m_pEndInvokeMethod);
            msgData->iFlags |= MSGFLG_ENDINVOKE;
        }
    }
    else
    {
        msgData->pDelegateMD = NULL;
        msgData->pMethodDesc = pMD;
        _ASSERTE(msgData->pMethodDesc->IsRuntimeMethodHandle());
    }

    if (msgData->pMethodDesc->IsOneWay())
    {
        msgData->iFlags |= MSGFLG_ONEWAY;
    }

    if (msgData->pMethodDesc->IsCtor())
    {
        msgData->iFlags |= MSGFLG_CTOR;
    }

    PCCOR_SIGNATURE pSig;
    DWORD cSig;
    Module *pModule;

    if (msgData->pDelegateMD)
    {
        msgData->pDelegateMD->GetSig(&pSig, &cSig);
        pModule = msgData->pDelegateMD->GetModule();
        SigTypeContext::InitTypeContext(msgData->pDelegateMD,pTypeContext);
    }
    else if (msgData->pMethodDesc->IsVarArg()) 
    {
        VASigCookie *pVACookie = pFrame->GetVASigCookie();
        pSig = pVACookie->mdVASig;
        cSig = SigParser::LengthOfSig(pVACookie->mdVASig);
        pModule = pVACookie->pModule;
        SigTypeContext::InitTypeContext(pTypeContext);

    }
    else 
    {
        msgData->pMethodDesc->GetSig(&pSig, &cSig);
        pModule = msgData->pMethodDesc->GetModule();
        SigTypeContext::InitTypeContext(msgData->pMethodDesc, thGoverningType, pTypeContext);
    }

    *ppModule = pModule;
    *ppSig = pSig;
    *pcbSigSize = cSig;
}

VOID CRealProxy::UpdateOptFlags(OBJECTREF refTP)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
        
    DWORD hierarchyDepth = 0;
    OBJECTREF refRP = CTPMethodTable::GetRP(refTP);
   
    OBJECTHANDLE hServerIdentity = (OBJECTHANDLE)refRP->GetPtrOffset(CRemotingServices::GetOffsetOfSrvIdentityInRP());
    if (hServerIdentity == NULL)
        return;

    // Check if the proxy has already been marked as not equivalent.
    // In which case, it can never get marked as anything else
    RealProxyObject *rpTemp = (RealProxyObject *)OBJECTREFToObject(refRP);
    
    DWORD domainID = rpTemp->GetDomainID();
    AppDomainFromIDHolder ad((ADID)domainID, TRUE);
    if (domainID == 0 || ad.IsUnloaded()) //we do not use ptr
        return;  // The appdomain the server belongs to, has been unloaded
    ad.Release();
    DWORD optFlag = rpTemp->GetOptFlags();
    if ((optFlag & OPTIMIZATION_FLAG_INITTED) &&
        !(optFlag & OPTIMIZATION_FLAG_PROXY_EQUIVALENT))
        return;
    
    OBJECTREF refSrvIdentity = ObjectFromHandle(hServerIdentity);
    // Is this a disconnected proxy ?
    if (refSrvIdentity == NULL)
        return;
    
    OBJECTREF refSrvObject = ObjectToOBJECTREF((Object *)refSrvIdentity->GetPtrOffset(CRemotingServices::GetOffsetOfTPOrObjInIdentity()));

    MethodTable *pCliMT = (MethodTable *)refTP->GetPtrOffset(CTPMethodTable::GetOffsetOfMT());
    
    BOOL bProxyQualifies = FALSE;
    BOOL bCastToSharedType = FALSE;


    LPCUTF8 szCliTypeName, szCliNameSpace;
    szCliTypeName = pCliMT->GetClass()->GetFullyQualifiedNameInfo(&szCliNameSpace);
    PREFIX_ASSUME(szCliTypeName != NULL);

    MethodTable *pSrvHierarchy = refSrvObject->GetMethodTable();

    GCPROTECT_BEGIN(refRP);
    while (pSrvHierarchy)
    {
        LPCUTF8 szSrvTypeName, szSrvNameSpace;
        szSrvTypeName = pSrvHierarchy->GetClass()->GetFullyQualifiedNameInfo(&szSrvNameSpace);
        PREFIX_ASSUME(szSrvNameSpace != NULL);

        if (!strcmp(szCliTypeName, szSrvTypeName) && !strcmp(szCliNameSpace, szSrvNameSpace))
        {
            // Check if the types are shared. If they are, no further check neccesary
            if (pSrvHierarchy == pCliMT)
            {
                bProxyQualifies = TRUE;
                bCastToSharedType = TRUE;
            }
            else
            {
                bProxyQualifies = CRealProxy::ProxyTypeIdentityCheck(pCliMT, pSrvHierarchy);
            }
            break;
        }

        pSrvHierarchy = pSrvHierarchy->GetParentMethodTable();
        hierarchyDepth++;
    }
    GCPROTECT_END();

    optFlag = 0;
    if (bProxyQualifies && hierarchyDepth < OPTIMIZATION_FLAG_DEPTH_MASK)
    {
        optFlag = OPTIMIZATION_FLAG_INITTED | OPTIMIZATION_FLAG_PROXY_EQUIVALENT;
        if (bCastToSharedType)
            optFlag |= OPTIMIZATION_FLAG_PROXY_SHARED_TYPE;
        optFlag |= (hierarchyDepth & OPTIMIZATION_FLAG_DEPTH_MASK);
    }
    else
        optFlag = OPTIMIZATION_FLAG_INITTED;

    RealProxyObject *rpUNSAFE = (RealProxyObject *)OBJECTREFToObject(refRP);
    rpUNSAFE->SetOptFlags(optFlag);
}

BOOL CRealProxy::ProxyTypeIdentityCheck(MethodTable *pCliHierarchy, MethodTable *pSrvHierarchy)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
    // We have found the server side type that corresponds to the most derived type
    // on client side, that the proxy is cast to
    // Now do identity check on the server type hierarchy to see if there is an exact match

    BOOL bProxyQualifies = FALSE;
    do
    {
        LPCUTF8 szCliTypeName, szCliNameSpace;
        LPCUTF8 szSrvTypeName, szSrvNameSpace;
        szCliTypeName = pCliHierarchy->GetClass()->GetFullyQualifiedNameInfo(&szCliNameSpace);
        szSrvTypeName = pSrvHierarchy->GetClass()->GetFullyQualifiedNameInfo(&szSrvNameSpace);
        PREFIX_ASSUME(szCliTypeName != NULL);
        PREFIX_ASSUME(szSrvNameSpace != NULL);
    
        // If type names are different, there is no match
        if (strcmp(szCliTypeName, szSrvTypeName) ||
            strcmp(szCliNameSpace, szSrvNameSpace))
        {
            bProxyQualifies = FALSE;
            return bProxyQualifies;
        }

        PEAssembly *pClientPE = pCliHierarchy->GetAssembly()->GetManifestFile();
        PEAssembly *pServerPE = pSrvHierarchy->GetAssembly()->GetManifestFile();
        // If the PE files are different, there is no match
        if (!pClientPE->Equals(pServerPE))
        {
            bProxyQualifies = FALSE;
            return bProxyQualifies;
        }

        // If the number of interfaces implemented are different, there is no match
        if (pSrvHierarchy->GetNumInterfaces() != pCliHierarchy->GetNumInterfaces())
        {
            bProxyQualifies = FALSE;
            return bProxyQualifies;
        }

        MethodTable::InterfaceMapIterator srvItfIt = pSrvHierarchy->IterateInterfaceMap();
        MethodTable::InterfaceMapIterator cliItfIt = pCliHierarchy->IterateInterfaceMap();
        while (srvItfIt.Next())
        {
            BOOL succeeded;
            succeeded = cliItfIt.Next();
            CONSISTENCY_CHECK(succeeded);
            if (!ProxyTypeIdentityCheck(srvItfIt.GetInterface(), cliItfIt.GetInterface()))
            {
                bProxyQualifies = FALSE;
                return bProxyQualifies;
            }
        }
        
        pSrvHierarchy = pSrvHierarchy->GetParentMethodTable();
        pCliHierarchy = pCliHierarchy->GetParentMethodTable();
    }
    while (pSrvHierarchy && pCliHierarchy);

    if (pSrvHierarchy || pCliHierarchy)
    {
        bProxyQualifies = FALSE;
        return bProxyQualifies;
    }
    
    bProxyQualifies = TRUE;
    return bProxyQualifies;

}

class ProfilerClientCallbackHolder
{
public:
    ProfilerClientCallbackHolder(Thread* pThread) : m_pThread(pThread)
    {
#ifdef PROFILING_SUPPORTED
        // If profiling is active, notify it that remoting stuff is kicking in
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingClientInvocationStarted(reinterpret_cast<ThreadID>(m_pThread));
        }
#endif // PROFILING_SUPPORTED
    }

    ~ProfilerClientCallbackHolder()
    {
#ifdef PROFILING_SUPPORTED
        // If profiling is active, tell profiler we've made the call, received the
        // return value, done any processing necessary, and now remoting is done.
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingClientInvocationFinished(reinterpret_cast<ThreadID>(m_pThread));
        }
#endif // PROFILING_SUPPORTED
    }

private:
    Thread* m_pThread;
};


//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::OnCall   private
//
//  Synopsis:   This function gets control in two situations
//              (1) When a call is made on the transparent proxy it delegates to              
//              PrivateInvoke method on the real proxy
//              (2) When a call is made on the constructor it again delegates to the 
//              PrivateInvoke method on the real proxy.
//              
//+----------------------------------------------------------------------------
PlatformDefaultReturnType __stdcall CTPMethodTable::OnCall(TPMethodFrame *pFrame, Thread *pThrd, ARG_SLOT *pReturn)
{
    _ASSERTE(pFrame);
    

    // The frame should be completely setup at this point    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pThrd, NULL_OK));
        PRECONDITION(CheckPointer(pReturn, NULL_OK));
    }
    CONTRACTL_END;

    PlatformDefaultReturnType retval;

    ZeroMemory(&retval, sizeof(retval));

    UINT fpRetSize = 0;
    
    _ASSERTE(pReturn == (ARG_SLOT*)pFrame->GetReturnValuePtr());
    *pReturn = 0;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;


    {
        messageData msgData;
        PCCOR_SIGNATURE pSig = NULL;
        DWORD cbSigSize;
        Module *pModule = NULL;
        SigTypeContext inst;
        InitMessageData(&msgData, pFrame, &pModule, &inst, &pSig, &cbSigSize);

        _ASSERTE(pSig && pModule);

        // Allocate metasig on the stack
        MetaSig mSig(pSig, cbSigSize, pModule, &inst);
        msgData.pSig = &mSig; 

        MethodDesc *pMD = pFrame->GetFunction();    
        if (pMD->GetClass()->IsMultiDelegateClass())
        {
            // check that there is only one target
            if (COMDelegate::IsTrueMulticastDelegate(pFrame->GetThis()))
            {
                COMPlusThrow(kArgumentException, L"Remoting_Delegate_TooManyTargets");
            }
        }

        {
            ProfilerClientCallbackHolder profilerHolder(pThrd);

            OBJECTREF pThisPointer = NULL;

#ifdef PROFILING_SUPPORTED
            GCPROTECT_BEGIN(pThisPointer);
#endif

            if (pMD->GetMethodTable()->IsAnyDelegateClass())
            {
                // this is an async call
                _ASSERTE(pFrame->GetThis()->GetMethodTable()->IsAnyDelegateClass());

                pThisPointer = COMDelegate::GetTargetObject(pFrame->GetThis());
            }
            else
            {
                pThisPointer = pFrame->GetThis();
            }

#ifdef PROFILING_SUPPORTED
            GCPROTECT_END();
#endif

            OBJECTREF firstParameter;
            MethodDesc* pTargetMD = NULL;
            size_t callType = CALLTYPE_INVALIDCALL;
            
            // We are invoking either the constructor or a method on the object
            if(pMD->IsCtor())
            {
                // Get the address of PrivateInvoke in managed code
                pTargetMD = CRemotingServices::MDofPrivateInvoke();
                _ASSERTE(IsTPMethodTable(pThisPointer->GetMethodTable()));
                
                firstParameter = (OBJECTREF)(Object*)(DWORD_PTR)pThisPointer->GetPtrOffset(s_dwRPOffset);

                // Set a field to indicate that it is a constructor call
                callType = CALLTYPE_CONSTRUCTORCALL;
            }
            else
            {
                // Set a field to indicate that it is a method call
                callType = CALLTYPE_METHODCALL;

                if (IsTPMethodTable(pThisPointer->GetMethodTable()))
                {
                    // Extract the real proxy underlying the transparent proxy
                    firstParameter = (OBJECTREF)(Object*)pThisPointer->GetPtrOffset(s_dwRPOffset);

                    // Get the address of PrivateInvoke in managed code
                    pTargetMD = CRemotingServices::MDofPrivateInvoke();
                    _ASSERTE(pTargetMD);
                }
                else 
                {
                    // must be async if this is not a TP 
                    _ASSERTE(pMD->GetMethodTable()->IsAnyDelegateClass());
                    firstParameter = NULL;
                    
                    // Get the address of PrivateInvoke in managed code
                    pTargetMD = CRemotingServices::MDofInvokeStatic();
                }

                // Go ahead and call PrivateInvoke on Real proxy. There is no need to 
                // catch exceptions thrown by it
                // See RealProxy.cs
            }

            _ASSERTE(pTargetMD);

            // Call the appropriate target
            CallTarget(pTargetMD, (LPVOID)OBJECTREFToObject(firstParameter), (LPVOID)&msgData, (LPVOID)callType);

            // Check for the need to trip thread
            if (pThrd->CatchAtSafePoint())
            {
                // There is no need to GC protect the return object as
                // TPFrame is GC protecting it
                CommonTripThread();
            }
        }  // ProfilerClientCallbackHolder

#if defined(_X86_)
        // Set the number of bytes to pop for x86
        pFrame->SetFunction((void *)(size_t)pMD->CbStackPop());
#endif // defined(_X86_)

        *(ARG_SLOT*)&retval = *pReturn;
        
        fpRetSize = msgData.pSig->GetFPReturnSize();

        UpdateFPReturnSizeForHFAReturn(msgData.pSig, &fpRetSize);
    }



    WIN64_ONLY(pFrame->Pop());

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    // floating point return values go in different registers.
    // check that here.
    // Setting the FP retval must be the last thing before the return
    if (fpRetSize)
        setFPReturn(fpRetSize, *pReturn);

    return retval;
}


// Helper due to inability to combine SEH with anything interesting.
BOOL CTPMethodTable::CheckCastHelper(MethodDesc* pTargetMD, LPVOID pFirst, LPVOID pSecond)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTargetMD));
        PRECONDITION(CheckPointer(pFirst, NULL_OK));
        PRECONDITION(CheckPointer(pSecond, NULL_OK));
    }
    CONTRACTL_END;

    LPVOID ret = CallTarget(pTargetMD, pFirst, pSecond);
    return *(CLR_BOOL*)StackElemEndianessFixup(&ret, sizeof(CLR_BOOL));
}



//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CheckCast   private
//
//  Synopsis:   Call the managed checkcast method to determine whether the 
//              server type can be cast to the given type
//              
//              
//+----------------------------------------------------------------------------
BOOL CTPMethodTable::CheckCast(MethodDesc* pTargetMD, OBJECTREF orTP, TypeHandle ty)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTargetMD));
        PRECONDITION(orTP != NULL);
        PRECONDITION(!ty.IsNull());
    }
    CONTRACTL_END;
    
    REFLECTCLASSBASEREF reflectType = NULL;
    LPVOID pvType = NULL;    
    BOOL fCastOK = FALSE;
    
    typedef struct _GCStruct
    {
        OBJECTREF orTP;
        OBJECTREF orRP;
    } GCStruct;

    GCStruct gcValues;
    gcValues.orTP = orTP;
    gcValues.orRP = GetRP(orTP);

    GCPROTECT_BEGIN (gcValues);

    reflectType = (REFLECTCLASSBASEREF) ty.GetMethodTable()->GetManagedClassObject();
    *(REFLECTCLASSBASEREF *)&pvType = reflectType;

    fCastOK = CheckCastHelper(pTargetMD, 
                              (LPVOID)OBJECTREFToObject(gcValues.orRP),
                              pvType);    

    if(fCastOK)
    {
        _ASSERTE(s_fTPTableFieldsInitialized);

        // The cast succeeded. Replace the current type in the proxy
        // with the given type. 

        CrstPreempHolder ch(&s_TPMethodTableCrst);

        MethodTable *pCurrent = (MethodTable *) gcValues.orTP->GetPtrOffset(s_dwMTOffset);
        
        if(ty.IsInterface())
        {
            // We replace the cached interface method table with the interface
            // method table that we are trying to cast to. This will ensure that
            // casts to this interface, which are likely to happen, will succeed.
            gcValues.orTP->SetOffsetPtr(s_dwItfMTOffset, ty.GetMethodTable());
        }
        else
        {
            BOOL fDerivedClass = FALSE;
            // Check whether this class derives from the current class
            fDerivedClass = CRemotingServices::CheckCast(gcValues.orTP, ty,
                                                         TypeHandle(pCurrent));
            // We replace the current method table only if we cast to a more 
            // derived class
            if(fDerivedClass)
            {
                // Set the method table in the proxy to the given method table
                RefineProxy(gcValues.orTP, ty);
            }
        }
    }

    GCPROTECT_END();
    return fCastOK;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::RefineProxy   public
//
//  Synopsis:   Set the method table in the proxy to the given class' method table.
//              Additionally, expand the TP method table to the required number of slots.
//              
//+----------------------------------------------------------------------------
void CTPMethodTable::RefineProxy(OBJECTREF orTP, TypeHandle ty)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(orTP != NULL);
        PRECONDITION(!ty.IsNull());
    }
    CONTRACTL_END;
    
    // Do the expansion only if necessary
    MethodTable *pMT = ty.GetMethodTable();
    
    if (pMT != (MethodTable *)orTP->GetPtrOffset(s_dwMTOffset))
    {
        orTP->SetOffsetPtr(s_dwMTOffset, pMT); 
    
        // Extend the vtable if necessary
        DWORD dwSlots = ty.GetNumVirtuals();
        if (dwSlots == 0)
            dwSlots = 1;
    
        if((dwSlots > GetCommitedTPSlots()) && !ExtendCommitedSlots(dwSlots))
        {
            // We failed to extend the committed slots. Out of memory.
            COMPlusThrowOM();
        }

    }
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::IsTPMethodTable   private
//
//  Synopsis:   Returns TRUE if the supplied method table is the one and only TP Method
//              Table
//+----------------------------------------------------------------------------
INT32 CTPMethodTable::IsTPMethodTable(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    
    return (s_pThunkTable != NULL && s_pThunkTable == pMT);
}

#ifndef HAS_REMOTING_PRECODE
//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::GetOrCreateNonVirtualSlotForVirtualMethod private
//
//  Synopsis:   Get a slot for a non-virtual call to a virtual method.
//
//+----------------------------------------------------------------------------
PTR_TADDR CTPMethodTable::GetOrCreateNonVirtualSlotForVirtualMethod(MethodDesc* pMD)
{
    CONTRACT (PTR_TADDR)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRemotingInterceptedViaVirtualDispatch());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Ensure the TP MethodTable's fields have been initialized.
    EnsureFieldsInitialized();

    // Create the thunk in a thread safe manner
    CrstPreempHolder ch(&s_TPMethodTableCrst);

    // Create hash table if we do not have one yet
    if (s_pThunkHashTable == NULL)
    {
        NewHolder <EEThunkHashTable> pTempHash(new EEThunkHashTable());

        LockOwner lock = {&s_TPMethodTableCrst, IsOwnerOfCrst};
        IfNullThrow(pTempHash->Init(23,&lock));

        s_pThunkHashTable = pTempHash.Extract();
    }

    PTR_TADDR pSlot;

    if (!s_pThunkHashTable->GetValue(pMD, (HashDatum *)&pSlot))
    {
        const BYTE* pvThunkCode = CreateNonVirtualThunkForVirtualMethod(pMD);

        _ASSERTE(CNonVirtualThunkMgr::IsThunkByASM(pvThunkCode));
        _ASSERTE(CNonVirtualThunkMgr::GetMethodDescByASM(pvThunkCode));

        // Set the generated thunk once and for all..            
        CNonVirtualThunk *pThunk = CNonVirtualThunk::SetNonVirtualThunks(pvThunkCode);

        // Remember the thunk address in a hash table 
        // so that we dont generate it again
        pSlot = (PTR_TADDR)pThunk->GetAddrOfCode();
        s_pThunkHashTable->InsertValue(pMD, (HashDatum)pSlot);
    }

    RETURN pSlot;
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::DestroyThunk   public
//
//  Synopsis:   Destroy the thunk for the non virtual method. 
//
//+----------------------------------------------------------------------------
void CTPMethodTable::DestroyThunk(MethodDesc* pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;
    
    if(s_pThunkHashTable)
    {
        CrstPreempHolder ch(&s_TPMethodTableCrst);

        LPVOID pvCode = NULL;
        s_pThunkHashTable->GetValue(pMD, (HashDatum *)&pvCode);
        CNonVirtualThunk *pThunk = NULL;
        if(NULL != pvCode)
        {
            pThunk = CNonVirtualThunk::AddrToThunk(pvCode);
            delete pThunk;
            s_pThunkHashTable->DeleteValue(pMD);
        }
    }
} 
#endif // HAS_REMOTING_PRECODE

static LPVOID CallTargetWorker1(MethodDesc* pTargetMD,
                                            LPVOID pvFirst,
                                            LPVOID pvSecond)
{
    LPVOID ret = NULL;
    const void* pTarget = pTargetMD->GetMethodEntryPoint();


#if defined(DEBUGGING_SUPPORTED)
    if (CORDebuggerTraceCall())
    {
        g_pDebugInterface->TraceCall((const BYTE*)pTarget);
    }
#endif // DEBUGGING_SUPPORTED


    BEGIN_CALL_TO_MANAGED();

    ret = CTPMethodTable__CallTargetHelper2(pTarget, pvFirst, pvSecond);

    END_CALL_TO_MANAGED();

    return ret;
}  


//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CallTarget   private
//
//  Synopsis:   Calls the target method on the given object
//+----------------------------------------------------------------------------
LPVOID __stdcall CTPMethodTable::CallTarget (MethodDesc* pTargetMD,
                                            LPVOID pvFirst,
                                            LPVOID pvSecond)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTargetMD));
        PRECONDITION(CheckPointer(pvFirst, NULL_OK));
        PRECONDITION(CheckPointer(pvSecond, NULL_OK));
    }
    CONTRACTL_END;
    
#ifdef _DEBUG

    Thread* curThread = GetThread();
    
    Object* ObjRefTable[OBJREF_TABSIZE];

    if (curThread)
        memcpy(ObjRefTable, curThread->dangerousObjRefs, sizeof(curThread->dangerousObjRefs));
    
#endif // _DEBUG

    LPVOID ret = CallTargetWorker1(pTargetMD, pvFirst, pvSecond);
    
#ifdef _DEBUG
    // Restore dangerousObjRefs when we return back to EE after call
    if (curThread)
        memcpy(curThread->dangerousObjRefs, ObjRefTable, sizeof(curThread->dangerousObjRefs));

    ENABLESTRESSHEAP ();
#endif // _DEBUG

    return ret;
}


static LPVOID CallTargetWorker2(MethodDesc* pTargetMD,
                                    LPVOID pvFirst,
                                    LPVOID pvSecond,
                                    LPVOID pvThird)
{
    LPVOID ret = NULL;
    const void* pTarget = pTargetMD->GetMethodEntryPoint();


#if defined(DEBUGGING_SUPPORTED)
    if (CORDebuggerTraceCall())
    {
        g_pDebugInterface->TraceCall((const BYTE*)pTarget);
    }
#endif // DEBUGGING_SUPPORTED

    BEGIN_CALL_TO_MANAGED();

    ret = CTPMethodTable__CallTargetHelper3(pTarget, pvFirst, pvSecond, pvThird);

    END_CALL_TO_MANAGED();
    return ret;

}

LPVOID __stdcall CTPMethodTable::CallTarget (MethodDesc* pTargetMD,
                                            LPVOID pvFirst,
                                            LPVOID pvSecond,
                                            LPVOID pvThird)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTargetMD));
        PRECONDITION(CheckPointer(pvFirst, NULL_OK));
        PRECONDITION(CheckPointer(pvSecond, NULL_OK));
        PRECONDITION(CheckPointer(pvThird, NULL_OK));
    }
    CONTRACTL_END;
    
#ifdef _DEBUG
    Thread* curThread = GetThread();
    
    Object* ObjRefTable[OBJREF_TABSIZE];
    if (curThread)
        memcpy(ObjRefTable, curThread->dangerousObjRefs, sizeof(curThread->dangerousObjRefs));
    
#endif // _DEBUG

    LPVOID ret = CallTargetWorker2(pTargetMD, pvFirst, pvSecond, pvThird);
    
#ifdef _DEBUG
    // Restore dangerousObjRefs when we return back to EE after call
    if (curThread)
        memcpy(curThread->dangerousObjRefs, ObjRefTable, sizeof(curThread->dangerousObjRefs));

    ENABLESTRESSHEAP ();
#endif // _DEBUG
    
    return ret;
}


#ifndef HAS_REMOTING_PRECODE
//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunk::SetNextThunk   public
//
//  Synopsis:   Creates a thunk for the given address and adds it to the global
//              list
//+----------------------------------------------------------------------------
CNonVirtualThunk* CNonVirtualThunk::SetNonVirtualThunks(const BYTE* pbCode)
{    
    CONTRACT (CNonVirtualThunk*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pbCode));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    CNonVirtualThunk *pThunk = new CNonVirtualThunk(pbCode);            

    // Put the generated thunk in a global list
    // Note: this is called when a NV thunk is being created ..
    // The TPMethodTable critsec is held at this point
    pThunk->SetNextThunk();

    // Set up the stub manager if necessary
    CNonVirtualThunkMgr::InitNonVirtualThunkManager();

    RETURN pThunk;
}

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunk::~CNonVirtualThunk   public
//
//  Synopsis:   Deletes the thunk from the global list of thunks
//              
//+----------------------------------------------------------------------------
CNonVirtualThunk::~CNonVirtualThunk()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(s_pNonVirtualThunks));
    }
    CONTRACTL_END;

    CNonVirtualThunk* pCurr = s_pNonVirtualThunks;
    CNonVirtualThunk* pPrev = NULL;
    BOOL found = FALSE;

    // Note: This is called with the TPMethodTable critsec held
    while(!found && (NULL != pCurr))
    {
        if(pCurr == this)
        {
            found = TRUE;
            
            // Unlink from the chain 
            if(NULL != pPrev)
            {                    
                pPrev->_pNext = pCurr->_pNext;
            }
            else
            {
               // First entry needs to be deleted
                s_pNonVirtualThunks = pCurr->_pNext;
            }
        }
        pPrev = pCurr;
        pCurr = pCurr->_pNext;
    }

    _ASSERTE(found);
}
#endif // HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::InitVirtualThunkManager   public
//
//  Synopsis:   Adds the stub manager to aid debugger in stepping into calls
//              
//+----------------------------------------------------------------------------
void CVirtualThunkMgr::InitVirtualThunkManager(const BYTE* stubAddress)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(stubAddress));
    }
    CONTRACTL_END;
    
    // This is function is already threadsafe since this method is called from within a 
    // critical section                               
    if(NULL == s_pVirtualThunkMgr)
    {
        // Add the stub manager for vtable calls
        s_pVirtualThunkMgr =  new CVirtualThunkMgr(stubAddress);
    
        StubManager::AddStubManager(s_pVirtualThunkMgr);
    }

}

#endif // !DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::CheckIsStub_Internal   public
//
//  Synopsis:   Returns TRUE if the given address is the starting address of
//              the transparent proxy stub
//+----------------------------------------------------------------------------
BOOL CVirtualThunkMgr::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;
    
    BOOL bIsStub = FALSE;

#ifndef DACCESS_COMPILE
    if (!IsThunkByASM((const BYTE *) stubStartAddress))
        return FALSE;
    if(NULL != FindThunk((const BYTE *) stubStartAddress))
        bIsStub = TRUE;       
#endif // !DACCESS_COMPILE

    return bIsStub;
}

#ifndef DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::Entry2MethodDesc   public
//
//  Synopsis:   Convert a starting address to a MethodDesc
//+----------------------------------------------------------------------------
MethodDesc *CVirtualThunkMgr::Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)
{
    CONTRACT (MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(StubStartAddress, NULL_OK));
        PRECONDITION(CheckPointer(pMT, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    if (pMT && IsThunkByASM(StubStartAddress) && CheckIsStub_Worker((TADDR) StubStartAddress))
        RETURN GetMethodDescByASM(StubStartAddress, pMT);
    else
        RETURN NULL;
}

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::FindThunk   private
//
//  Synopsis:   Finds a thunk that matches the given starting address
//+----------------------------------------------------------------------------
LPBYTE CVirtualThunkMgr::FindThunk(const BYTE *stubStartAddress)
{
    CONTRACT (LPBYTE)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(stubStartAddress, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACT_END;
    
    CVirtualThunks* pThunks = CVirtualThunks::GetVirtualThunks();
    LPBYTE pThunkAddr = NULL;

    while(NULL != pThunks)
    {
        DWORD dwStartSlot = pThunks->_dwStartThunk;
        DWORD dwCurrSlot = pThunks->_dwStartThunk;
        DWORD dwMaxSlot = pThunks->_dwCurrentThunk;        
        while (dwCurrSlot < dwMaxSlot)
        {
            LPBYTE pStartAddr =  pThunks->ThunkCode[dwCurrSlot-dwStartSlot].pCode;
            if((stubStartAddress >= pStartAddr) &&
               (stubStartAddress <  (pStartAddr + ConstVirtualThunkSize)))
            {
                pThunkAddr = pStartAddr;
                break;
            }            
            ++dwCurrSlot;
        }

        pThunks = pThunks->GetNextThunk();            
     }

     RETURN pThunkAddr;
}

#endif // !DACCESS_COMPILE



#ifndef HAS_REMOTING_PRECODE

#ifndef DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::InitNonVirtualThunkManager   public
//
//  Synopsis:   Adds the stub manager to aid debugger in stepping into calls
//              
//+----------------------------------------------------------------------------
void CNonVirtualThunkMgr::InitNonVirtualThunkManager()
{   
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    // This function is already thread safe since this method is called from within a 
    // critical section
    if(NULL == s_pNonVirtualThunkMgr)
    {
        // Add the stub manager for non vtable calls
        s_pNonVirtualThunkMgr = new CNonVirtualThunkMgr();
        
        StubManager::AddStubManager(s_pNonVirtualThunkMgr);
    }

}

#endif // !DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::CheckIsStub_Internal   public
//
//  Synopsis:   Returns TRUE if the given address is the starting address of
//              one of our thunks
//+----------------------------------------------------------------------------
BOOL CNonVirtualThunkMgr::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;
    
    BOOL bIsStub = FALSE;

#ifndef DACCESS_COMPILE
    if (!IsThunkByASM((const BYTE *) stubStartAddress))
        return FALSE;
    if(NULL != FindThunk((const BYTE *) stubStartAddress))
        bIsStub = TRUE;       
#endif // !DACCESS_COMPILE

    return bIsStub;
}

#ifndef DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::Entry2MethodDesc   public
//
//  Synopsis:   Convert a starting address to a MethodDesc
//+----------------------------------------------------------------------------
MethodDesc *CNonVirtualThunkMgr::Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)
{
    CONTRACT (MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(StubStartAddress, NULL_OK));
        PRECONDITION(CheckPointer(pMT, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    if (IsThunkByASM(StubStartAddress) && CheckIsStub_Worker((TADDR) StubStartAddress))
        RETURN GetMethodDescByASM(StubStartAddress);
    else
        RETURN NULL;
}

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::FindThunk   private
//
//  Synopsis:   Finds a thunk that matches the given starting address
//+----------------------------------------------------------------------------
CNonVirtualThunk* CNonVirtualThunkMgr::FindThunk(const BYTE *stubStartAddress)
{
    CONTRACT (CNonVirtualThunk*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(stubStartAddress, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACT_END;
    
    CNonVirtualThunk* pThunk = CNonVirtualThunk::GetNonVirtualThunks();

    while(NULL != pThunk)
    {
        if(stubStartAddress == pThunk->GetThunkCode())           
            break;

        pThunk = pThunk->GetNextThunk();            
    }

    RETURN pThunk;
}

#endif // !DACCESS_COMPILE

#endif // HAS_REMOTING_PRECODE


#ifndef DACCESS_COMPILE

//+----------------------------------------------------------------------------
//+- HRESULT MethodDescDispatchHelper(MethodDesc* pMD, ARG_SLOT[] args, ARG_SLOT *pret)
//+----------------------------------------------------------------------------
HRESULT MethodDescDispatchHelper(MethodDescCallSite* pMethodCallSite, ARG_SLOT args[], ARG_SLOT *pret)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMethodCallSite));
        PRECONDITION(CheckPointer(args));
        PRECONDITION(CheckPointer(pret));
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_EXCEPTION_GLUE(&hr, NULL) 
    {
        *pret = pMethodCallSite->Call_RetArgSlot(args);
    }
    END_EXCEPTION_GLUE;

    return hr;
}



//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GetStubForInterfaceMethod
//
//  Synopsis:   Given the exact interface method we wish to invoke on, return
//              the entry point of a stub that will correctly transition into
//              the remoting system passing it this method.
//              We cache the stubs generated on a per module hash.
//
//+----------------------------------------------------------------------------
const BYTE *CRemotingServices::GetStubForInterfaceMethod(MethodDesc *pItfMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pItfMD));
    }
    CONTRACTL_END;

    // Check whether this module even has a hash of stubs associated with it
    // yet.
    Module *pModule = pItfMD->GetLoaderModule();
    if (pModule->m_pRemotingInterfaceThunks == NULL)
    {
        // No hash so create one lazily. Use the remoting Crst to synchonize
        // this creation (plus an individual Crst used to serialize updates to
        // this specific hash).
        GCX_PREEMP();
        CrstHolder ch(&s_RemotingCrst);
        GCX_POP();

        // Has the Crst been created yet?
        if (pModule->m_pRemotingInterfaceThunksCrst == NULL)
            pModule->m_pRemotingInterfaceThunksCrst = new Crst("RemotingInterfaceThunks", CrstRemoting, CRST_UNSAFE_COOPGC);

        // Check that we still need to allocate the hash now we have a mutex.
        if (pModule->m_pRemotingInterfaceThunks == NULL)
        {
            LockOwner sLock = {pModule->m_pRemotingInterfaceThunksCrst, IsOwnerOfCrst};
            NewHolder <EEPtrHashTable> pTempHash(new EEPtrHashTable());
            if (!pTempHash->Init(17, &sLock))
                COMPlusThrowOM();       
            pModule->m_pRemotingInterfaceThunks = pTempHash.Extract();
        }
    }
    
    // Go look for an existing stub for this method.
    Stub *pStub;
    if (!pModule->m_pRemotingInterfaceThunks->GetValue(pItfMD, (HashDatum*)&pStub))
    {
        // None exist, build one.
        CPUSTUBLINKER sl;
        StubHolder<Stub> pNewStub(CreatePrepadSimulationThunk(&sl,
                                                              pItfMD,
                                                              (BYTE*)GetEEFuncEntryPoint(CRemotingServices__DispatchInterfaceCall)));
        pNewStub->SetRequiresMethodDescCallingConvention();

        // Now take the Crst which guards access to the stub hash and check again.
        CrstHolder sCrst(pModule->m_pRemotingInterfaceThunksCrst);
        if (!pModule->m_pRemotingInterfaceThunks->GetValue(pItfMD, (HashDatum*)&pStub))
        {
            // Still not there, so insert the one we just built.
            pModule->m_pRemotingInterfaceThunks->InsertValue(pItfMD, (HashDatum)pNewStub);
            pStub = pNewStub;
            pNewStub.SuppressRelease();
        }
    }

    return pStub->GetEntryPoint();
}


#endif // !DACCESS_COMPILE
