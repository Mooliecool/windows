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

#ifndef __SECURITYPOLICY_H__
#define __SECURITYPOLICY_H__

#include "crst.h"
#include "corpermp.h"
#include "objecthandle.h"
#include "securityattributes.h"
#include "declsec.h"
#include "fcall.h"
#include "cgensys.h"
#include "rwlock.h"
#include "comstring.h"

#define SPFLAGSASSERTION        0x01
#define SPFLAGSUNMANAGEDCODE    0x02
#define SPFLAGSSKIPVERIFICATION 0x04

#define CORSEC_STACKWALK_HALTED       0x00000001   // Stack walk was halted
#define CORSEC_SKIP_INTERNAL_FRAMES   0x00000002   // Skip reflection/remoting frames in stack walk
#define CORSEC_FT_ASSERT              0x00000004   // Hit a FT-assert during the stackwalk
#define CORSEC_CS_REACHED             0x00000008   // Hit a CS during the stackwalk


// Forward declarations to avoid pulling in too many headers.
class Frame;
class FramedMethodFrame;
class ClassLoader;
class Thread;
class CrawlFrame;
class SystemNative;
class NDirect;
class SystemDomain;
class AssemblySecurityDescriptor;
class PEFileSecurityDescriptor;
class SharedSecurityDescriptor;
class SecurityStackWalkData;
class DemandStackWalk;
class SecurityDescriptor;
class COMPrincipal;
class Security;
class SecurityAttributes;

#define CLR_CASOFF_MUTEX L"Global\\CLR_CASOFF_MUTEX"


// Always add the "Global\\"

#define NeedGlobalObject() TRUE

#define CLR_CASOFF_MUTEX_NO_GLOBAL L"Global\\CLR_CASOFF_MUTEX"


enum EnumSecurityState
{
    StateUnknown = 0,
    StateOff     = 1,
    StateOn      = 2
};

class SecurityPolicy
{
    friend class Security;
    friend class SecurityDescriptor;
    friend class AssemblySecurityDescriptor;
    friend class ApplicationSecurityDescriptor;
    friend class PEFileSecurityDescriptor;
    friend class DemandStackWalk;
    friend class SecurityDeclarative;
    friend class COMPrincipal;
    friend class SecurityStackWalk;
    friend class SecurityAttributes;
    friend class SecurityRuntime;
    friend class SecurityTransparent;

private:
    // -----------------------------------------------------------
    // Members
    // -----------------------------------------------------------

    // The global disable settings (see CorPerm.h)
    static DWORD  s_dwGlobalSettings;
    static EnumSecurityState s_eSecurityState;

public:
    // -----------------------------------------------------------
    // FCalls
    // -----------------------------------------------------------

    static FCDECL0(FC_BOOL_RET, FcallDoesFullTrustMeanFullTrust);
    static FCDECL2(FC_BOOL_RET, IsSameType, StringObject* pLeft, StringObject* pRight);
    static FCDECL1(FC_BOOL_RET, SetThreadSecurity, CLR_BOOL fThreadSecurity);
    static FCDECL2(void, SetGlobalSecurity, DWORD mask, DWORD flags);
    static FCDECL0(void, SaveGlobalSecurity);
    static FCDECL3(void, GetGrantedPermissions, OBJECTREF* ppGranted, OBJECTREF* ppRefused, OBJECTREF* stackmark);
    static FCDECL0(DWORD, GetImpersonationFlowMode);
    static FCDECL0(FC_BOOL_RET, IsDefaultThreadSecurityInfo);
    static FCDECL4(Object*, GetPublicKey, Object* pThisUNSAFE, CLR_BOOL bExported, U1Array* pArrayUNSAFE, StringObject* pContainerUNSAFE);
    static FCDECL1(DWORD, CreateFromUrl, StringObject* urlUNSAFE);
    static FCDECL1(Object*, EcallGetLongPathName, StringObject* shortPathUNSAFE);
    static FCDECL0(FC_BOOL_RET, IsSecurityOnNative);
    static FCDECL0(DWORD, GetGlobalSecurity);
    static FCDECL1(FC_BOOL_RET, LocalDrive, StringObject* pathUNSAFE);
    static FCDECL1(Object*, GetDeviceName, StringObject* driveLetterUNSAFE);
    static FCDECL0(VOID, IncrementOverridesCount);
    static FCDECL0(VOID, DecrementOverridesCount);
    static FCDECL0(VOID, IncrementAssertCount);
    static FCDECL0(VOID, DecrementAssertCount);
    static FCDECL1(Object*, GetEvidence, AssemblyBaseObject* pThisUNSAFE);



private:
    // -----------------------------------------------------------
    // Init methods
    // -----------------------------------------------------------

    // Calls all the security-related init methods
    //   Callers:
    //     EEStartupHelper
    static void Start();

    static void Stop();

    // Saves security cache data
    //   Callers:
    //     EEShutDownHelper
    static void SaveCache();



    // -----------------------------------------------------------
    // Policy
    // -----------------------------------------------------------


    // Returns TRUE if security is enabled, and FALSE if it's disabled
    static BOOL IsSecurityOn() 
    {
        if (s_eSecurityState == StateUnknown)
            s_eSecurityState = CheckCASOffMutex();
        return (s_eSecurityState == StateOn);
    }
    static EnumSecurityState CheckCASOffMutex();


    // eagerly resolves policy
    //   Callers:
    //     Assembly::SetDomainAssembly
    static void EarlyResolveThrowing(Assembly *pAssembly, AssemblySecurityDescriptor *pSecDesc);

    // Try to figure out the security zone from the url if we can tell quickly
    //   Callers:
    //     PEFile::GetSecurityIdentity
    static DWORD QuickGetZone( const WCHAR* url );

    // Returns TRUE if the assembly has permission to call unmanaged code
    //   Callers:
    //     CEEInfo::getNewHelper
    //     MakeStubWorker
    //     MethodDesc::DoPrestub
    static BOOL CanCallUnmanagedCode(Module *pModule);

    // Determines if the check for execution permission is enabled
    //   Callers:
    //     AppDomainNative::SetupDomainSecurity
    static inline BOOL IsExecutionPermissionCheckEnabled()
    {
        LEAF_CONTRACT;

        return (s_dwGlobalSettings & CORSETTING_EXECUTION_PERMISSION_CHECK_DISABLED) == 0;
    }

    // Returns S_OK if the method uses any imperative security that requires a security descriptor.
    // It works by checking for System.Security.DynamicSecurityMethodAttribute
    //   Callers:
    //     CEEInfo::getMethodAttribs
    static HRESULT HasREQ_SOAttribute(MethodDesc* pMethod);

    // Throws a security exception
    //   Callers:
    //     JIT_SecurityUnmanagedCodeException
    static void CreateSecurityException(__in_z char *szDemandClass, DWORD dwFlags, OBJECTREF* pThrowable);
    static DECLSPEC_NORETURN void ThrowSecurityException(__in_z char *szDemandClass, DWORD dwFlags);




    // -----------------------------------------------------------
    // Internal (to security) Helper Methods
    // -----------------------------------------------------------

    inline static BOOL GlobalSettings(DWORD dwFlag)
    {
        LEAF_CONTRACT;
        return ((s_dwGlobalSettings & dwFlag) != 0);
    }

    inline static DWORD GlobalSettings()
    {
        LEAF_CONTRACT;
        return s_dwGlobalSettings;
    }

    inline static void SetGlobalSettings(DWORD dwMask, DWORD dwFlags)
    {
        LEAF_CONTRACT;
        s_dwGlobalSettings = (s_dwGlobalSettings & ~dwMask) | dwFlags;
    }

    inline static BOOL IsSecurityOff()
    {
        WRAPPER_CONTRACT;
        return !IsSecurityOn();
    }

    static DECLSPEC_NORETURN void ThrowSecurityException(AssemblySecurityDescriptor* pSecDesc);

    static BOOL CanSkipVerification(DomainAssembly * pAssembly, BOOL fCommit = TRUE);

    // Determines what permissions to grant the assembly
    //   Callers:
    //     SecurityDescriptor::ResolveWorker
    static OBJECTREF ResolvePolicy(OBJECTREF evidence, OBJECTREF reqdPset, OBJECTREF optPset,
                                   OBJECTREF denyPset, OBJECTREF* grantdenied, int* grantIsUnrestricted, BOOL checkExecutionPermission);

    // Load the policy config/cache files at EE startup
    static void InitPolicyConfig();

    // Like WszGetLongPathName, but it works with nonexistant files too
    static size_t GetLongPathNameHelper( const WCHAR* wszShortPath, __inout_ecount(cchBuffer) __inout_z WCHAR* wszBuffer, DWORD cchBuffer );

    static OBJECTREF GetMscorlibEvidence(AssemblySecurityDescriptor *pSecDesc);
};

struct SharedPermissionObjects
{
    OBJECTHANDLE        hPermissionObject;  // Commonly used Permission Object
    BinderClassID       idClass;            // ID of class
    BinderMethodID      idConstructor;      // ID of constructor to call      
    DWORD               dwPermissionFlag;   // Flag needed by the constructors (Only a single argument is assumed)
};

/******** Shared Permission Objects related constants *******/
#define NUM_PERM_OBJECTS    (sizeof(g_rPermObjectsTemplate) / sizeof(SharedPermissionObjects))

// Constants to use with SecurityPermission
#define SECURITY_PERMISSION_ASSERTION               1      // SecurityPermission.cs
#define SECURITY_PERMISSION_UNMANAGEDCODE           2      // SecurityPermission.cs
#define SECURITY_PERMISSION_SKIPVERIFICATION        4      // SecurityPermission.cs
#define SECURITY_PERMISSION_CONTROLEVIDENCE         0x20   // SecurityPermission.cs
#define SECURITY_PERMISSION_SERIALIZATIONFORMATTER  0X80   // SecurityPermission.cs
#define SECURITY_PERMISSION_CONTROLPRINCIPAL        0x200  // SecurityPermission.cs
#define SECURITY_PERMISSION_BINDINGREDIRECTS        0X2000 // SecurityPermission.cs

// Constants to use with ReflectionPermission
#define REFLECTION_PERMISSION_TYPEINFO              1      // ReflectionPermission.cs
#define REFLECTION_PERMISSION_MEMBERACCESS          2      // ReflectionPermission.cs
#define REFLECTION_PERMISSION_REFLECTIONEMIT        4      // ReflectionPermission.cs

// PermissionState.Unrestricted
#define PERMISSION_STATE_UNRESTRICTED               1      // PermissionState.cs

const SharedPermissionObjects g_rPermObjectsTemplate[] =
{
    {NULL, CLASS__SECURITY_PERMISSION, METHOD__SECURITY_PERMISSION__CTOR, SECURITY_PERMISSION_UNMANAGEDCODE },
    {NULL, CLASS__SECURITY_PERMISSION, METHOD__SECURITY_PERMISSION__CTOR, SECURITY_PERMISSION_SKIPVERIFICATION },
    {NULL, CLASS__REFLECTION_PERMISSION, METHOD__REFLECTION_PERMISSION__CTOR, REFLECTION_PERMISSION_TYPEINFO },
    {NULL, CLASS__SECURITY_PERMISSION, METHOD__SECURITY_PERMISSION__CTOR, SECURITY_PERMISSION_ASSERTION },
    {NULL, CLASS__REFLECTION_PERMISSION, METHOD__REFLECTION_PERMISSION__CTOR, REFLECTION_PERMISSION_MEMBERACCESS },
    {NULL, CLASS__SECURITY_PERMISSION, METHOD__SECURITY_PERMISSION__CTOR, SECURITY_PERMISSION_SERIALIZATIONFORMATTER},
    {NULL, CLASS__REFLECTION_PERMISSION, METHOD__REFLECTION_PERMISSION__CTOR, REFLECTION_PERMISSION_REFLECTIONEMIT},
    {NULL, CLASS__PERMISSION_SET, METHOD__PERMISSION_SET__CTOR, PERMISSION_STATE_UNRESTRICTED},
    {NULL, CLASS__SECURITY_PERMISSION, METHOD__SECURITY_PERMISSION__CTOR, SECURITY_PERMISSION_BINDINGREDIRECTS },
    {NULL, CLASS__UI_PERMISSION, METHOD__UI_PERMISSION__CTOR, PERMISSION_STATE_UNRESTRICTED },
};

// Array index in SharedPermissionObjects array
// Note: these should all be permissions that implement IUnrestrictedPermission.
// Any changes to these must be reflected in bcl\system\security\codeaccesssecurityengine.cs and the above table

// special flags
#define SECURITY_UNMANAGED_CODE                 0
#define SECURITY_SKIP_VER                       1
#define REFLECTION_TYPE_INFO                    2
#define SECURITY_ASSERT                         3
#define REFLECTION_MEMBER_ACCESS                4
#define SECURITY_SERIALIZATION                  5
#define REFLECTION_EMIT                         6
#define SECURITY_FULL_TRUST                     7
#define SECURITY_BINDING_REDIRECTS              8

// special permissions
#define UI_PERMISSION                           9
#define ENVIRONMENT_PERMISSION                  10
#define FILEDIALOG_PERMISSION                   11
#define FILEIO_PERMISSION                       12
#define REFLECTION_PERMISSION                   13
#define SECURITY_PERMISSION                     14

// additional special flags
#define SECURITY_CONTROL_EVIDENCE               16
#define SECURITY_CONTROL_PRINCIPAL              17

// Class holding a grab bag of security stuff we need on a per-appdomain basis.
struct SecurityContext
{
    SharedPermissionObjects     m_rPermObjects[NUM_PERM_OBJECTS];
    PsetHandleCache             m_rCachedPsets;

    // Cached declarative permissions per method
    EEPtrHashTable m_pCachedMethodPermissionsHash;
    SimpleRWLock * m_prCachedMethodPermissionsLock;

    size_t                      m_nCachedPsetsSize;

    SecurityContext() :
        m_nCachedPsetsSize(0)
    {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        } CONTRACTL_END;
        memcpy(m_rPermObjects, g_rPermObjectsTemplate, sizeof(m_rPermObjects));
        
        // initialize cache of method-level declarative security permissions
        // Note that the method-level permissions are stored elsewhere
        m_prCachedMethodPermissionsLock = new SimpleRWLock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT);
        if (!m_pCachedMethodPermissionsHash.Init(100, &g_lockTrustMeIAmThreadSafe))
            ThrowOutOfMemory();
    }

    ~SecurityContext()
    {
        CONTRACTL {
            NOTHROW;
            GC_TRIGGERS;
            MODE_ANY;
        } CONTRACTL_END;
        m_rCachedPsets.~PsetHandleCache();
        
        // no need to explicitly delete the cache contents, since they will be deallocated with the AppDomain's heap
        if (m_prCachedMethodPermissionsLock) delete m_prCachedMethodPermissionsLock;
    }
};

#ifdef _DEBUG

#define DBG_TRACE_METHOD(cf)                                                \
    do {                                                                    \
        MethodDesc * __pFunc = cf -> GetFunction();                         \
        if (__pFunc) {                                                      \
            LOG((LF_SECURITY, LL_INFO1000,                                  \
                 "    Method: %s.%s\n",                                     \
                 (__pFunc->m_pszDebugClassName == NULL) ?                   \
                "<null>" : __pFunc->m_pszDebugClassName,                    \
                 __pFunc->GetName()));                                      \
        }                                                                   \
    } while (false)

#define DBG_TRACE_STACKWALK(msg, verbose) LOG((LF_SECURITY, (verbose) ? LL_INFO10000 : LL_INFO1000, msg))
#else //_DEBUG

#define DBG_TRACE_METHOD(cf)
#define DBG_TRACE_STACKWALK(msg, verbose)

#endif //_DEBUG


//
// Get and get the global security settings for the VM (from the registry)
//
HRESULT STDMETHODCALLTYPE
GetSecuritySettings(DWORD* dwState);

HRESULT STDMETHODCALLTYPE
SetSecuritySettings(DWORD dwState);



#endif // __SECURITYPOLICY_H__
