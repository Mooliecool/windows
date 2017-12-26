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

#include "security.h"
#include "perfcounters.h"
#include "eventtrace.h"
#include "timeline.h"
#include "appdomainstack.inl"


// Statics
DWORD SecurityPolicy::s_dwGlobalSettings = 0;
EnumSecurityState SecurityPolicy::s_eSecurityState = StateUnknown;

void *SecurityProperties::operator new(size_t size, LoaderHeap *pHeap)
{
    WRAPPER_CONTRACT;
    return pHeap->AllocMem(size);
}

void SecurityProperties::operator delete(void *pMem)
{
    LEAF_CONTRACT;
    // No action required
}

void SecurityPolicy::InitPolicyConfig()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    SecurityConfig::ReadRegistryExtensionsInfo();

    // Note: These buffers should be at least as big as the longest possible
    // string that will be placed into them by the code below.
    const size_t cchcache = MAX_PATH + sizeof( L"defaultusersecurity.config.cch" ) / sizeof( WCHAR ) + 1;
    const size_t cchconfig = MAX_PATH + sizeof( L"defaultusersecurity.config.cch" ) / sizeof( WCHAR ) + 1;
    NewArrayHolder<WCHAR> cache(new WCHAR[cchcache]);
    NewArrayHolder<WCHAR> config(new WCHAR[cchconfig]);

    HRESULT hr = SecurityConfig::GetMachineDirectory(config, MAX_PATH);
    if (FAILED(hr))
        ThrowHR(hr);

    wcscat_s( config, cchconfig, L"security.config" );
    wcscpy_s( cache, cchcache, config );
    wcscat_s( cache, cchcache, L".cch" );
    SecurityConfig::InitData( SecurityConfig::MachinePolicyLevel, config, cache );

    hr = SecurityConfig::GetMachineDirectory(config, MAX_PATH);
    if (FAILED(hr))
        ThrowHR(hr);

    wcscat_s( config, cchconfig, L"enterprisesec.config" );
    wcscpy_s( cache, cchcache, config );
    wcscat_s( cache, cchcache, L".cch" );
    SecurityConfig::InitData( SecurityConfig::EnterprisePolicyLevel, config, cache );

    BOOL result = SecurityConfig::GetUserDirectory(config, MAX_PATH);
    if (result) {
        wcscat_s( config, cchconfig, L"security.config" );
        wcscpy_s( cache, cchcache, config );
        wcscat_s( cache, cchcache, L".cch" );
        SecurityConfig::InitData( SecurityConfig::UserPolicyLevel, config, cache );
    }
}

void SecurityPolicy::Start()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;


    SecurityAttributes::Init();
    SecurityConfig::Init();
    SecurityPolicy::InitPolicyConfig();

    HRESULT hr = GetSecuritySettings(&s_dwGlobalSettings);
    IfFailThrow(hr);
    g_pCertificateCache = new CertificateCache();
}

void SecurityPolicy::Stop()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    SecurityAttributes::Shutdown();
}

void SecurityPolicy::SaveCache()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    Thread *pThread = GetThread();
    if (pThread == NULL)
    {
        BOOL fRet = FALSE;
        EX_TRY
        {
            // If CLR is hosted, a host can deny a thread during SetupThread call.
            if (IsShutdownSpecialThread())
            {
                SetupInternalThread();
            }
            else
            {
                SetupThread();
            }
        }
        EX_CATCH
        {
            fRet = TRUE;
        }
        EX_END_CATCH(SwallowAllExceptions);
        if (fRet)
        {
            return;
        }
    }

    SecurityConfig::SaveCacheData( SecurityConfig::MachinePolicyLevel );
    SecurityConfig::SaveCacheData( SecurityConfig::UserPolicyLevel );
    SecurityConfig::SaveCacheData( SecurityConfig::EnterprisePolicyLevel );

    SecurityConfig::Cleanup();
}

EnumSecurityState SecurityPolicy::CheckCASOffMutex() {
    // Rotor does not support OpenMutex.
    return StateOn;
}



FCIMPL0(FC_BOOL_RET, SecurityPolicy::FcallDoesFullTrustMeanFullTrust)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    FC_RETURN_BOOL(Security::DoesFullTrustMeanFullTrust());
}
FCIMPLEND

// This is an ecall
FCIMPL3(void, SecurityPolicy::GetGrantedPermissions, OBJECTREF* ppGranted, OBJECTREF* ppRefused, OBJECTREF* stackmark)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;
    HELPER_METHOD_FRAME_BEGIN_0();
    //-[autocvtpro]-------------------------------------------------------

    // Skip one frame (SecurityManager.IsGranted) to get to the caller

    AppDomain* pDomain = NULL;
    Assembly* callerAssembly = SystemDomain::GetCallersAssembly( (StackCrawlMark*)stackmark, &pDomain );

    _ASSERTE( callerAssembly != NULL);

    AssemblySecurityDescriptor* pSecDesc = callerAssembly->GetSecurityDescriptor(pDomain);

    _ASSERTE( pSecDesc != NULL );

    OBJECTREF token = pSecDesc->GetGrantedPermissionSet(ppRefused);
    *(ppGranted) = token;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(DWORD, SecurityPolicy::GetImpersonationFlowMode)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;    
    return (g_pConfig->ImpersonationMode());
    
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SecurityPolicy::IsSecurityOnNative)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    FC_RETURN_BOOL(IsSecurityOn());
}
FCIMPLEND

// This is for ecall.
FCIMPL0(DWORD, SecurityPolicy::GetGlobalSecurity)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return GlobalSettings();
}
FCIMPLEND


FCIMPL2(void, SecurityPolicy::SetGlobalSecurity, DWORD mask, DWORD flags)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;
    HELPER_METHOD_FRAME_BEGIN_0();
    //-[autocvtpro]-------------------------------------------------------

    SecurityPolicy::SetGlobalSettings( mask, flags );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(void, SecurityPolicy::SaveGlobalSecurity)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;
    HELPER_METHOD_FRAME_BEGIN_0();

    HRESULT hr = SetSecuritySettings(GlobalSettings());
    IfFailThrow(hr);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//---------------------------------------------------------
// Does a method have a REQ_SO CustomAttribute?
//
// S_OK    = yes
// S_FALSE = no
// FAILED  = unknown because something failed.
//---------------------------------------------------------
/*static*/
HRESULT SecurityPolicy::HasREQ_SOAttribute(MethodDesc* pMethod)
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;


    DWORD dwAttr = pMethod->GetAttrs();

    return (dwAttr & mdRequireSecObject) ? S_OK : S_FALSE;
}

void SecurityPolicy::CreateSecurityException(__in_z char *szDemandClass, DWORD dwFlags, OBJECTREF *pThrowable)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    MAKE_WIDEPTR_FROMUTF8(wszDemandClass, szDemandClass);

    static MethodTable * pMT = NULL;
    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__SECURITY_EXCEPTION);
    _ASSERTE(pMT && "Unable to load the throwable class !");

    static MethodTable * pMTSecPerm = NULL;
    if (pMTSecPerm == NULL)
        pMTSecPerm = g_Mscorlib.GetClass(CLASS__SECURITY_PERMISSION);
    _ASSERTE(pMTSecPerm && "Unable to load the security permission class !");

    struct _gc {
        STRINGREF strDemandClass;
        OBJECTREF secPerm;
        STRINGREF strPermState;
        OBJECTREF secPermType;
        OBJECTREF secElement;
    } gc;
    memset(&gc, 0, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    gc.strDemandClass = COMString::NewString(wszDemandClass);
    if (gc.strDemandClass == NULL) COMPlusThrowOM();
    // Get the type seen by reflection
    gc.secPermType = pMTSecPerm->GetManagedClassObject();
    // Allocate the security exception object
    *pThrowable = AllocateObject(pMT);
    if (*pThrowable == NULL) COMPlusThrowOM();
    // Allocate the security permission object
    gc.secPerm = AllocateObject(pMTSecPerm);
    if (gc.secPerm == NULL) COMPlusThrowOM();

    // Call the construtor with the correct flag
    MethodDescCallSite ctor(METHOD__SECURITY_PERMISSION__CTOR);
    ARG_SLOT arg3[2] = {
        ObjToArgSlot(gc.secPerm),
        (ARG_SLOT)dwFlags
    };
    ctor.Call(arg3);

    // Now, get the ToXml method
    MethodDescCallSite toXML(METHOD__SECURITY_PERMISSION__TOXML, &gc.secPerm);
    ARG_SLOT arg4 = ObjToArgSlot(gc.secPerm);
    gc.secElement = toXML.Call_RetOBJECTREF(&arg4);

    MethodDescCallSite toString(METHOD__SECURITY_ELEMENT__TO_STRING, &gc.secElement);
    ARG_SLOT arg5 = ObjToArgSlot(gc.secElement);
    gc.strPermState = toString.Call_RetSTRINGREF(&arg5);

    MethodDescCallSite exceptionCtor(METHOD__SECURITY_EXCEPTION__CTOR);

    ARG_SLOT arg6[4] = {
        ObjToArgSlot(*pThrowable),
        ObjToArgSlot(gc.strDemandClass),
        ObjToArgSlot(gc.secPermType),
        ObjToArgSlot(gc.strPermState),
    };
    exceptionCtor.Call(arg6);

    GCPROTECT_END();
}


DECLSPEC_NORETURN void SecurityPolicy::ThrowSecurityException(__in_z char *szDemandClass, DWORD dwFlags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    struct _gc {
        OBJECTREF throwable;
    } gc;
    memset(&gc, 0, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    CreateSecurityException(szDemandClass, dwFlags, &gc.throwable);
    COMPlusThrow(gc.throwable);

    GCPROTECT_END();
}

void DECLSPEC_NORETURN SecurityPolicy::ThrowSecurityException(AssemblySecurityDescriptor* pSecDesc)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    static MethodTable * pMT = NULL;

    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__SECURITY_EXCEPTION);

    _ASSERTE(pMT && "Unable to load the throwable class !");

    struct _gc {
        OBJECTREF throwable;
        OBJECTREF grantSet;
        OBJECTREF refusedSet;
    } gc;
    memset(&gc, 0, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    // Allocate the security exception object
    gc.throwable = AllocateObject(pMT);
    if (gc.throwable == NULL) COMPlusThrowOM();

    gc.grantSet = pSecDesc->GetGrantedPermissionSet( &gc.refusedSet );

    MethodDescCallSite ctor(METHOD__SECURITY_EXCEPTION__CTOR2);
    ARG_SLOT args[4] = {
        ObjToArgSlot(gc.throwable),
        ObjToArgSlot(gc.refusedSet),
        ObjToArgSlot(gc.grantSet),
    };
    ctor.Call(args);

    COMPlusThrow(gc.throwable);

    GCPROTECT_END();
}

OBJECTREF SecurityPolicy::ResolvePolicy(OBJECTREF evidence, OBJECTREF reqdPset, OBJECTREF optPset,
                                  OBJECTREF denyPset, OBJECTREF* grantdenied, int* dwSpecialFlags, BOOL checkExecutionPermission)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // If we got here, then we are going to do at least one security
    // check. Make sure security is initialized.

    struct _gc {
        OBJECTREF reqdPset;         // Required Requested Permissions
        OBJECTREF optPset;          // Optional Requested Permissions
        OBJECTREF denyPset;         // Denied Permissions
        OBJECTREF evidence;         // Object containing evidence
        OBJECTREF refRetVal;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.evidence = evidence;
    gc.reqdPset = reqdPset;
    gc.denyPset = denyPset;
    gc.optPset = optPset;

    GCPROTECT_BEGIN(gc);

    MethodDescCallSite resolvePolicy(METHOD__SECURITY_MANAGER__RESOLVE_POLICY);

    ARG_SLOT args[7];
    args[0] = ObjToArgSlot(gc.evidence);
    args[1] = ObjToArgSlot(gc.reqdPset);
    args[2] = ObjToArgSlot(gc.optPset);
    args[3] = ObjToArgSlot(gc.denyPset);
    args[4] = PtrToArgSlot(grantdenied);
    args[5] = PtrToArgSlot(dwSpecialFlags);
    args[6] = (ARG_SLOT) checkExecutionPermission;

    {
        // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require
        // any assemblies currently being loaded.
       OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
        // call policy resolution routine in managed code
        gc.refRetVal = resolvePolicy.Call_RetOBJECTREF(args);
    }

    GCPROTECT_END();

    return gc.refRetVal;
}

void SecurityPolicy::EarlyResolveThrowing(Assembly *pAssembly, AssemblySecurityDescriptor *pSecDesc)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_SECURITYCATCHALL);

    //
    //

    FrameWithCookie<DebuggerClassInitMarkFrame> __dcimf;
    pSecDesc->Resolve();

    // update the PLS with the grant/denied sets of the loaded assembly
    ApplicationSecurityDescriptor* pAppDomainSecDesc = pSecDesc->GetDomain()->GetSecurityDescriptor();
    pAppDomainSecDesc->AddNewSecDescToPLS(pSecDesc);

    __dcimf.Pop();
}


BOOL SecurityPolicy::CanSkipVerification(DomainAssembly * pAssembly, BOOL fCommit)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pAssembly));
    } CONTRACTL_END;

    BOOL canSkipVerification = TRUE;
    if (!pAssembly->IsSystem())
    {
        AssemblySecurityDescriptor *pSec = pAssembly->GetSecurityDescriptor();
        _ASSERTE(pSec);
        if (pSec)
        {
            canSkipVerification = pSec->CanSkipVerification();
        }
        else
        {
            canSkipVerification = FALSE;
        }
        if (canSkipVerification && fCommit)
            GetAppDomain()->OnLinktimeCanSkipVerificationCheck(pAssembly->GetAssembly());
    }

    return canSkipVerification;
}

BOOL SecurityPolicy::CanCallUnmanagedCode(Module *pModule)
{
    CONTRACTL {
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    SharedSecurityDescriptor *pSharedSecDesc = pModule->GetAssembly()->GetSharedSecurityDescriptor();
    if (pSharedSecDesc)
        return pSharedSecDesc->CanCallUnmanagedCode();

    AssemblySecurityDescriptor *pSec = pModule->GetSecurityDescriptor();
    _ASSERTE(pSec);
    return pSec->CanCallUnmanagedCode();
}

// Retrieve the public portion of a public/private key pair. The key pair is
// either exported (available as a byte array) or encapsulated in a Crypto API
// key container (identified by name).
FCIMPL4(Object*, SecurityPolicy::GetPublicKey, Object* pThisUNSAFE, CLR_BOOL bExported, U1Array* pArrayUNSAFE, StringObject* pContainerUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;
    OBJECTREF refRetVal = NULL;

    struct _gc
    {
        OBJECTREF pThis;
        U1ARRAYREF pArray;
        STRINGREF pContainer;
    } gc;
    gc.pThis = (OBJECTREF) pThisUNSAFE;
    gc.pArray = (U1ARRAYREF) pArrayUNSAFE;
    gc.pContainer = (STRINGREF) pContainerUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);
    //-[autocvtpro]-------------------------------------------------------

     LPWSTR      wszKeyContainer = NULL;
    DWORD       cchKeyContainer;
    BYTE       *pbKeyPair = NULL;
    DWORD       cbKeyPair = 0;
    BYTE       *pbPublicKey;
    DWORD       cbPublicKey;
    OBJECTREF   orOutputArray;

    // Read the arguments; either a byte array or a container name.
    if (bExported) {
        // Key pair provided directly in a byte array.
        cbKeyPair = gc.pArray->GetNumComponents();
        pbKeyPair = (BYTE*)_alloca(cbKeyPair);
        memcpy(pbKeyPair, gc.pArray->GetDataPtr(), cbKeyPair);
    } else {
        // Key pair referenced by key container name.
        cchKeyContainer = gc.pContainer->GetStringLength();
        _ASSERTE(cchKeyContainer == wcslen(gc.pContainer->GetBuffer()));
        wszKeyContainer = (LPWSTR)_alloca((cchKeyContainer + 1) * sizeof(WCHAR));
        memcpy(wszKeyContainer, gc.pContainer->GetBuffer(), cchKeyContainer * sizeof(WCHAR));
        wszKeyContainer[cchKeyContainer] = L'\0';
    }

    // Call the strong name routine to extract the public key. Need to switch
    // into GC pre-emptive mode for this call since it might perform a load
    // library (don't need to bother for the StrongNameFreeBuffer call later on).
    BOOL fResult = TRUE;
    {
        GCX_PREEMP();
        fResult = StrongNameGetPublicKey(wszKeyContainer,
                                          pbKeyPair,
                                          cbKeyPair,
                                          &pbPublicKey,
                                          &cbPublicKey);
    }
    if (!fResult)
        COMPlusThrow(kArgumentException, L"Argument_StrongNameGetPublicKey");

    // Translate the unmanaged byte array into managed form.
    SecurityAttributes::CopyEncodingToByteArray(pbPublicKey, cbPublicKey, &orOutputArray);

    StrongNameFreeBuffer(pbPublicKey);

    refRetVal = orOutputArray;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(DWORD, SecurityPolicy::CreateFromUrl, StringObject* urlUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;

    DWORD dwZone = (DWORD) NoZone;

    STRINGREF url = (STRINGREF) urlUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, url);
    //-[autocvtpro]-------------------------------------------------------

    // make local copy of the string for GC-safety
    StackSString strRootFile;
    url->GetSString (strRootFile);

    if (strRootFile.GetUnicode() != NULL)
    {
        dwZone = SecurityPolicy::QuickGetZone(strRootFile.GetUnicode());

    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return dwZone;
}
FCIMPLEND

DWORD SecurityPolicy::QuickGetZone( const WCHAR* url )
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // If we aren't given an url, just early out.
    if (url == NULL)
        return (DWORD) NoZone;

    // A simple portable security zone mapping: 
    //  file URLs are LocalMachine, everything else is Internet
    return UrlIsW(url, URLIS_FILEURL) ? LocalMachine : Internet;
}


FCIMPL1(FC_BOOL_RET, SecurityPolicy::LocalDrive, StringObject* pathUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;

    FC_RETURN_BOOL(TRUE);
}
FCIMPLEND

FCIMPL1(Object*, SecurityPolicy::EcallGetLongPathName, StringObject* shortPathUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    STRINGREF shortPath = (STRINGREF) shortPathUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, shortPath);
    //-[autocvtpro]-------------------------------------------------------

    WCHAR wszBuffer[MAX_PATH];

    StackSString stackShortPath;

    shortPath->GetSString(stackShortPath);  
                
    if (SecurityPolicy::GetLongPathNameHelper( stackShortPath.GetUnicode(), wszBuffer, MAX_PATH ) == 0)
    {
        refRetVal = shortPath;
    }
    else
    {
        refRetVal = COMString::NewString( wszBuffer );
    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

size_t SecurityPolicy::GetLongPathNameHelper( const WCHAR* wszShortPath, __inout_ecount(cchBuffer) __inout_z WCHAR* wszBuffer, DWORD cchBuffer )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    DWORD size = ::GetLongPathName(wszShortPath, wszBuffer, cchBuffer);

    if (size == 0)
    {
        // We have to deal with files that do not exist so just
        // because GetLongPathName doesn't give us anything doesn't
        // mean that we can give up.  We iterate through the input
        // trying GetLongPathName on every subdirectory until
        // it succeeds or we run out of string.

        WCHAR wszIntermediateBuffer[MAX_PATH];

        if (wcslen( wszShortPath ) >= MAX_PATH)
            return 0;

        wcscpy_s( wszIntermediateBuffer, COUNTOF(wszIntermediateBuffer), wszShortPath );

        size_t index = wcslen( wszIntermediateBuffer );

        do
        {
            while (index > 0 && (wszIntermediateBuffer[index-1] != L'\\' && wszIntermediateBuffer[index-1] != L'/'))
                --index;

            if (index == 0)
                break;

			
            wszIntermediateBuffer[index-1] = L'\0';


            size = ::GetLongPathName(wszIntermediateBuffer, wszBuffer, MAX_PATH);

            if (size != 0)
            {
                size_t sizeBuffer = wcslen( wszBuffer );

                if (sizeBuffer + wcslen( &wszIntermediateBuffer[index] ) > MAX_PATH - 2)
                {
                    return 0;
                }
                else
                {
                    if (wszBuffer[sizeBuffer-1] != L'\\' && wszBuffer[sizeBuffer-1] != L'/')
                        wcscat_s( wszBuffer, cchBuffer, L"\\" );
                    wcscat_s( wszBuffer, cchBuffer, &wszIntermediateBuffer[index] );
                    return (DWORD)wcslen( wszBuffer );
                }
            }
        }
        while( true );

        return 0;
    }
    else if (size > MAX_PATH)
    {
        return 0;
    }
    else
    {
        return wcslen( wszBuffer );
    }
}



FCIMPL1(Object*, SecurityPolicy::GetDeviceName, StringObject* driveLetterUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;        
    } CONTRACTL_END;

    FCUnique(0x88);
    return NULL;
}
FCIMPLEND

FCIMPL1(Object*, SecurityPolicy::GetEvidence, AssemblyBaseObject* pThisUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    OBJECTREF   orEvidence  = NULL;
    ASSEMBLYREF pThis       = (ASSEMBLYREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pThis);

    if (!pThis)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    Assembly *pAssembly = pThis->GetAssembly();
    AssemblySecurityDescriptor *pSecDesc = pAssembly->GetSecurityDescriptor();

    if (pSecDesc->IsSystem())
        orEvidence = SecurityPolicy::GetMscorlibEvidence(pSecDesc);
    else if (pSecDesc->IsEvidenceComputed())
         orEvidence = pSecDesc->GetAdditionalEvidence();
    else
        orEvidence = pSecDesc->GetEvidence();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(orEvidence);
}
FCIMPLEND

OBJECTREF SecurityPolicy::GetMscorlibEvidence(AssemblySecurityDescriptor *pSecDesc)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(pSecDesc->IsSystem());
        PRECONDITION(Security::GetDomain(pSecDesc) == SystemDomain::System()->DefaultDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF evidence = NULL;
    GCPROTECT_BEGIN(evidence);
    AppDomain *pDefaultDomain = SystemDomain::System()->DefaultDomain();
    ENTER_DOMAIN_PTR(pDefaultDomain,ADV_DEFAULTAD)
    {
        if (Security::IsEvidenceComputed(pSecDesc))
            evidence = Security::GetAdditionalEvidence(pSecDesc);
        else
            evidence = Security::GetEvidence(pSecDesc);
    }
    END_DOMAIN_TRANSITION;

    // Caller may be in another appdomain context, in which case we'll
    // need to marshal/unmarshal the evidence across.
    if (pDefaultDomain != GetAppDomain())
        evidence = AppDomainHelper::CrossContextCopyFrom(pDefaultDomain->GetId(), &evidence);

    GCPROTECT_END();
    return evidence;
}

FCIMPL0(void, SecurityPolicy::IncrementOverridesCount)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, FCThrowVoid(kStackOverflowException));
    pThread->IncrementOverridesCount();
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

FCIMPL0(void, SecurityPolicy::DecrementOverridesCount)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, FCThrowVoid(kStackOverflowException));
    pThread->DecrementOverridesCount();
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

FCIMPL0(void, SecurityPolicy::IncrementAssertCount)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, FCThrowVoid(kStackOverflowException));
    pThread->IncrementAssertCount();
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

FCIMPL0(void, SecurityPolicy::DecrementAssertCount)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, FCThrowVoid(kStackOverflowException));
    pThread->DecrementAssertCount();
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND


#define DEFAULT_GLOBAL_POLICY 0

static const WCHAR* gszGlobalPolicySettings = L"GlobalSettings";


HRESULT STDMETHODCALLTYPE
GetSecuritySettings(DWORD* pdwState)
{
    WRAPPER_CONTRACT;

    DWORD state = DEFAULT_GLOBAL_POLICY;
    if (pdwState == NULL)
        return E_INVALIDARG;


    DWORD val;
    WCHAR temp[16];

    if (PAL_FetchConfigurationString(TRUE, gszGlobalPolicySettings, temp, sizeof(temp) / sizeof(WCHAR)))
    {
        LPWSTR endPtr;
        val = wcstol(temp, &endPtr, 16);         // treat it has hex
        if (endPtr != temp)                      // success
            state = val;
    }


    *pdwState = state;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
SetSecuritySettings(DWORD dwState)
{
    HRESULT hr = S_OK;


    CORTRY {
        WCHAR temp[16];

        // treat it has hex
        if (dwState == DEFAULT_GLOBAL_POLICY)
        {
            PAL_SetConfigurationString(TRUE, gszGlobalPolicySettings, L"");
        }
        else
        {
            _snwprintf(temp, sizeof(temp) / sizeof(WCHAR), L"%08x", dwState);

            if (!PAL_SetConfigurationString(TRUE, gszGlobalPolicySettings, temp))
            {
                CORTHROW(HRESULT_FROM_GetLastError());
            }
        }
    }
    CORCATCH(err) {
        hr = err.corError;
    } COREND;


    return hr;
}

static void SetAssemblyName( SString& ssAssemblyQualifedTypeName, SString& ssAssemblyName )
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    SString::Iterator iter = ssAssemblyQualifedTypeName.Begin();

    if (!ssAssemblyQualifedTypeName.Find( iter, ',' ))
        return;

    iter++;
    while (*iter == ' ' )
        iter++;

    ssAssemblyName.Set( ssAssemblyQualifedTypeName,
                        iter,
                        ssAssemblyQualifedTypeName.End() );
}

FCIMPL2(FC_BOOL_RET, SecurityPolicy::IsSameType, StringObject* pLeft, StringObject* pRight)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF strLeft = (STRINGREF)pLeft;
    STRINGREF strRight = (STRINGREF)pRight;

    BOOL bEqual = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(strLeft, strRight);
    //-[autocvtpro]-------------------------------------------------------

    StackSString ssTypeLeft;
    StackSString ssTypeRight;
    SString ssLeft;
    SString ssRight;

    strLeft->GetSString(ssTypeLeft);
    strRight->GetSString(ssTypeRight);

    SetAssemblyName( ssTypeLeft, ssLeft );
    SetAssemblyName( ssTypeRight, ssRight );

    AppDomain* pDomain = GetAppDomain();
    IApplicationContext* pAppCtx = pDomain->GetFusionContext();

    _ASSERTE( pAppCtx != NULL && "Fusion context not setup yet" );

    StackScratchBuffer sBuffer;
    AssemblySpec specLeft, specRight;
    specLeft.Init(ssLeft.GetANSI(sBuffer));
    specRight.Init(ssRight.GetANSI(sBuffer));

    SafeComHolder<IAssemblyName> pAssemblyNameLeft;
    SafeComHolder<IAssemblyName> pAssemblyNameRight;
    
    IfFailThrow(specLeft.CreateFusionName(&pAssemblyNameLeft));
    IfFailThrow(specRight.CreateFusionName(&pAssemblyNameRight));

    SafeComHolder<IAssemblyName> pAssemblyNamePostPolicyLeft;
    SafeComHolder<IAssemblyName> pAssemblyNamePostPolicyRight;

    if (FAILED(PreBindAssembly(pAppCtx, pAssemblyNameLeft,  NULL, &pAssemblyNamePostPolicyLeft,  NULL)) ||
        FAILED(PreBindAssembly(pAppCtx, pAssemblyNameRight, NULL, &pAssemblyNamePostPolicyRight, NULL)))
    {
        // version-agnostic comparison.
        bEqual = pAssemblyNameLeft->IsEqual(pAssemblyNameRight, ASM_CMPF_NAME | ASM_CMPF_PUBLIC_KEY_TOKEN | ASM_CMPF_CULTURE) == S_OK;
    }
    else
    {
        // version-agnostic comparison.
        bEqual = pAssemblyNamePostPolicyLeft->IsEqual(pAssemblyNamePostPolicyRight, ASM_CMPF_NAME | ASM_CMPF_PUBLIC_KEY_TOKEN | ASM_CMPF_CULTURE) == S_OK;
    }

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bEqual);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, SecurityPolicy::SetThreadSecurity, CLR_BOOL fThreadSecurity)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread* pThread = GetThread();
    BOOL inProgress = pThread->IsSecurityStackwalkInProgess();
    pThread->SetSecurityStackwalkInProgress(fThreadSecurity);
    FC_RETURN_BOOL(inProgress);
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SecurityPolicy::IsDefaultThreadSecurityInfo)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL bRet = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    bRet = SecurityStackWalk::HasFlagsOrFullyTrusted(0);
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bRet);
}
FCIMPLEND

