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

#ifndef __SECURITYDESCRIPTOR_APPDOMAIN_H__
#define __SECURITYDESCRIPTOR_APPDOMAIN_H__
#include "securitydescriptor.h"
///////////////////////////////////////////////////////////////////////////////
//
//      [SecurityDescriptor]
//      |
//      +----[PEFileSecurityDescriptor]
//      |
//      +----[ApplicationSecurityDescriptor]
//      |
//      +----[AssemblySecurityDescriptor]
//
//      [SharedSecurityDescriptor]
//
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
//
//          APPDOMAIN SECURITY DESCRIPTOR
//
//------------------------------------------------------------------

class ApplicationSecurityDescriptor : public SecurityDescriptor
{
friend class Security;
friend class SecurityPolicy;
friend class SecurityStackWalk;
friend class SecurityDeclarative;
friend class SecurityDescriptor;
friend class AssemblySecurityDescriptor;
friend class AppDomainStack;
friend struct AppDomainStackEntry;
friend class PEFileSecurityDescriptor;
friend class DemandStackWalk;
friend class DomainCompressedStack;

private:
    // Dependency in managed : System.Security.HostSecurityManager.cs
    enum HostSecurityManagerFlags {
        // Flags to control which HostSecurityManager features are provided by the host
        HOST_NONE                   = 0x0000,
        HOST_APP_DOMAIN_EVIDENCE    = 0x0001,
        HOST_POLICY_LEVEL           = 0x0002,
        HOST_ASM_EVIDENCE           = 0x0004,
        HOST_DAT                    = 0x0008,
        HOST_RESOLVE_POLICY         = 0x0010
    };

    // Intersection of granted/denied permissions of all assemblies in domain
    OBJECTHANDLE m_hDomainPermissionListSet; 
    // The bits represent the status of security checks on some specific permissions within this domain
    volatile DWORD m_dwDomainWideSpecialFlags;
    // m_dwDomainWideSpecialFlags bit map
    // Bit 0 = Unmanaged Code access permission. Accessed via SECURITY_UNMANAGED_CODE
    // Bit 1 = Skip verification permission. SECURITY_SKIP_VER
    // Bit 2 = Permission to Reflect over types. REFLECTION_TYPE_INFO
    // Bit 3 = Permission to Assert. SECURITY_ASSERT
    // Bit 4 = Permission to invoke methods. REFLECTION_MEMBER_ACCESS
    // Bit 7 = PermissionSet, fulltrust SECURITY_FULL_TRUST
    // Bit 9 = UIPermission (unrestricted)

    BOOL m_fIsInitializationInProgress; // appdomain is in the initialization stage and is considered FullTrust by the security system.
    BOOL m_fIsDefaultAppdomain;         // appdomain is the default appdomain, or created by the default appdomain without an explicit evidence
    BOOL m_fHomogeneous;                // This AppDomain has an ApplicationTrust
    DWORD m_dwHostSecurityManagerFlags; // Flags indicating what decisions the host wants to participate in.
    BOOL m_fContainsAnyRefusedPermissions;

public:
#ifndef DACCESS_COMPILE
    //--------------------
    // Constructor
    //--------------------
    ApplicationSecurityDescriptor(AppDomain *pAppDomain):
        SecurityDescriptor(pAppDomain, NULL, NULL),
        m_dwDomainWideSpecialFlags(0xFFFFFFFF),
        m_fIsInitializationInProgress(TRUE),
        m_fIsDefaultAppdomain(FALSE),
        m_fHomogeneous(FALSE),
        m_dwHostSecurityManagerFlags(HOST_NONE),
        m_fContainsAnyRefusedPermissions(FALSE)
    {
        LEAF_CONTRACT;
        m_hDomainPermissionListSet = NULL;
    }
#endif // #ifndef DACCESS_COMPILE

protected:
    //--------------------
    // Exposed APIs
    //--------------------

    // Indicates whether the initialization phase is in progress.
    BOOL IsInitializationInProgress()
    {
        LEAF_CONTRACT;
        return m_fIsInitializationInProgress;
    }

    VOID ResetInitializationInProgress()
    {
        LEAF_CONTRACT;
        m_fIsInitializationInProgress = FALSE;
    }

    // The AppDomain is considered a default one (FT) if the property is
    // set and it's not a homogeneous AppDomain (ClickOnce case for example).
    BOOL IsDefaultAppDomain()
    {
        LEAF_CONTRACT;
        return !m_fHomogeneous && m_fIsDefaultAppdomain;
    }

    VOID SetDefaultAppDomain()
    {
        LEAF_CONTRACT;
        m_fIsDefaultAppdomain = TRUE;
    }

    //--------------------
    // Internal (to security) Helper Methods
    //--------------------

#ifndef DACCESS_COMPILE
    virtual VOID Resolve();
    void ResolveWorker();
#endif // #ifndef DACCESS_COMPILE

    void SetHostSecurityManagerFlags (DWORD dwFlags)
    {
        LEAF_CONTRACT;
        m_dwHostSecurityManagerFlags |= dwFlags;
    }

    void SetPolicyLevelFlag ()
    {
        LEAF_CONTRACT;
        m_dwHostSecurityManagerFlags |= HOST_POLICY_LEVEL;
    }

    void SetHomogeneousFlag()
    {
        LEAF_CONTRACT;
        m_fHomogeneous = TRUE;
    }

    BOOL IsHomogeneous()
    {
        LEAF_CONTRACT;
        return m_fHomogeneous && !CallHostSecurityManager();
    }

    BOOL ContainsAnyRefusedPermissions()
    {
        LEAF_CONTRACT;
        return m_fContainsAnyRefusedPermissions;
    }

    // Should the HSM be consulted for security decisions in this AppDomain.
    BOOL CallHostSecurityManager()
    {
        LEAF_CONTRACT;
        return (m_dwHostSecurityManagerFlags & HOST_APP_DOMAIN_EVIDENCE ||
                m_dwHostSecurityManagerFlags & HOST_POLICY_LEVEL ||
                m_dwHostSecurityManagerFlags & HOST_ASM_EVIDENCE ||
                m_dwHostSecurityManagerFlags & HOST_RESOLVE_POLICY);
    }

    // Initialize the PLS on the AppDomain.
    void InitializePLS();

    // Called everytime an AssemblySecurityDescriptor is resolved.
    void AddNewSecDescToPLS(AssemblySecurityDescriptor *pNewSecDescriptor);

    // Checks for one of the special domain wide flags 
    // such as if we are currently in a "fully trusted" environment
    // or if unmanaged code access is allowed at this time
    FORCEINLINE BOOL CheckDomainWideSpecialFlag (DWORD flags) { LEAF_CONTRACT; return (m_dwDomainWideSpecialFlags & flags); }
    FORCEINLINE DWORD GetDomainWideSpecialFlag () { LEAF_CONTRACT; return m_dwDomainWideSpecialFlags; }

    // Check the demand against the PLS in this AppDomain
    BOOL CheckPLS (OBJECTREF* orDemand, DWORD dwDemandSpecialFlags, BOOL fDemandSet);

    virtual OBJECTREF GetEvidence();
    virtual DWORD GetZone();
    virtual BOOL QuickIsFullyTrusted();
};
typedef DPTR(class ApplicationSecurityDescriptor) PTR_ApplicationSecurityDescriptor;

#endif // #define __SECURITYDESCRIPTOR_APPDOMAIN_H__
