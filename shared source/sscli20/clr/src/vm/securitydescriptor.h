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

#ifndef __SECURITYDESCRIPTOR_H__
#define __SECURITYDESCRIPTOR_H__

#include "securityconfig.h"
#include "securityattributes.h"
#include "securitypolicy.h"

// Security flags for the objects that store security information
#define CORSEC_ASSERTED             0x000020 // Asseted permission set present on frame
#define CORSEC_DENIED               0x000040 // Denied permission set present on frame
#define CORSEC_REDUCED              0x000080 // Reduced permission set present on frame

/******** Location of serialized security evidence **********/

#define s_strSecurityEvidence "Security.Evidence"

/************************************************************/

 /*  Inline Functions to support lazy handles - 
  *  read/write to handle that may not have been created yet 
  *  SecurityDescriptor and ApplicationSecurityDescriptor currently use these
  */
inline OBJECTREF ObjectFromLazyHandle(OBJECTHANDLE handle)
{
    WRAPPER_CONTRACT;
    if (handle != NULL) {
        return ObjectFromHandle(handle);
    }
    else {
        return NULL;
    }
}

#ifndef DACCESS_COMPILE

inline void StoreObjectInLazyHandle(OBJECTHANDLE& handle, OBJECTREF ref, AppDomain* app_dom);


#endif // #ifndef DACCESS_COMPILE


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
//
// A Security Descriptor is placed on AppDomain and Assembly (Unmanged) objects.
// AppDomain and Assembly could be from different zones.
// Security Descriptor could also be placed on a native frame.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// SecurityDescriptor is the base class for all security descriptors.
// Extend this class to implement SecurityDescriptors for Assemblies and
// AppDomains.
//
///////////////////////////////////////////////////////////////////////////////

class SecurityDescriptor
{
friend class ApplicationSecurityDescriptor;
friend class SharedSecurityDescriptor;
friend class Security;
friend class SecurityPolicy;
friend class SecurityStackWalk;
friend class SecurityDeclarative;
friend class SecurityConfig;
friend class DemandStackWalk;
friend class DomainCompressedStack;
friend class AssertStackWalk;
friend class SecurityRuntime;
protected:
    //--------------------
    // Members
    //--------------------
    OBJECTHANDLE m_hAdditionalEvidence;     // Evidence Object

    // The unmanaged DomainAssembly object
    DomainAssembly     *m_pAssem;

    // The PEFile associated with the DomainAssembly
    PEFile      *m_pPEFile;

    // The AppDomain context
    AppDomain*   m_pAppDomain;

    // Figuring out the zone is expensive, so we cache the result.
    DWORD        m_dwZone;

    BOOL         m_fSDResolved;
    BOOL         m_fFullyTrusted;
    BOOL         m_fIsZoneComputed;
    BOOL         m_fEvidenceComputed;

    DWORD        m_dwSpecialFlags;

private:
    OBJECTHANDLE m_hGrantedPermissionSet;   // Granted Permission
    OBJECTHANDLE m_hGrantDeniedPermissionSet;// Specifically Denied Permissions

protected:
    //--------------------
    // Constructor
    //--------------------

#ifndef DACCESS_COMPILE

    SecurityDescriptor(AppDomain *pAppDomain, DomainAssembly *pAssembly, PEFile* pPEFile) :
        m_pAssem(pAssembly),
        m_pPEFile(pPEFile),
        m_pAppDomain(pAppDomain),
        m_dwZone((DWORD)NoZone),
        m_fSDResolved(FALSE),
        m_fFullyTrusted(FALSE),
        m_fIsZoneComputed(FALSE),
        m_fEvidenceComputed(FALSE),
        m_dwSpecialFlags(0xFFFFFFFF)
    {
        WRAPPER_CONTRACT; // ctor ends up calling PermissionRequestSpecialFlags::PermissionRequestSpecialFlags

        m_hGrantedPermissionSet = NULL;
        m_hGrantDeniedPermissionSet = NULL;
        m_hAdditionalEvidence = NULL;
    }

    //--------------------
    // Exposed API's
    //--------------------
    OBJECTREF GetGrantedPermissionSet(OBJECTREF* RefusedPermissions);

    BOOL CheckQuickCache(SecurityConfig::QuickCacheEntryType all, const SecurityConfig::QuickCacheEntryType* zoneTable);
    BOOL IsFullyTrusted();

    inline void SetGrantedPermissionSet(OBJECTREF GrantedPermissionSet, OBJECTREF DeniedPermissionSet, DWORD dwSpecialFlags)
    {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        } CONTRACTL_END;

        StoreObjectInLazyHandle(m_hGrantedPermissionSet, GrantedPermissionSet, m_pAppDomain);
        StoreObjectInLazyHandle(m_hGrantDeniedPermissionSet, DeniedPermissionSet, m_pAppDomain);

        if (dwSpecialFlags & (1 << SECURITY_FULL_TRUST))
            m_fFullyTrusted = TRUE;
        m_dwSpecialFlags = dwSpecialFlags;
        m_fSDResolved = TRUE;
        // make sure the shared security descriptor is updated in case this 
        // is a security descriptor for a shared assembly.
        Resolve();
    }

    inline void SetEvidence(OBJECTREF evidence)
    {
        CONTRACTL {
            THROWS; // From StoreObjectInLazyHandle
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(evidence != NULL);
        } CONTRACTL_END;

        StoreObjectInLazyHandle(m_hAdditionalEvidence, evidence, m_pAppDomain);
        SetEvidenceComputed();
    }
#endif // #ifndef DACCESS_COMPILE

    BOOL CanCallUnmanagedCode();

    DWORD GetSpecialFlags()
    {
        LEAF_CONTRACT;
        return m_dwSpecialFlags;
    }

    inline AppDomain* GetDomain()
    {
        LEAF_CONTRACT;
        return m_pAppDomain;
    }

    BOOL IsEvidenceComputed() 
    {
        LEAF_CONTRACT;
        return m_fEvidenceComputed;
    }
    VOID SetEvidenceComputed()
    {
        LEAF_CONTRACT;
        m_fEvidenceComputed = TRUE;
    }

    BOOL IsZoneComputed()
    {
        LEAF_CONTRACT;
        return m_fIsZoneComputed;
    }
    VOID SetZoneComputed()
    {
        LEAF_CONTRACT;
        m_fIsZoneComputed = TRUE;
    }

    //--------------------
    // Internal (to security) Helper Methods
    //--------------------
#ifndef DACCESS_COMPILE

    static OBJECTREF GetEvidenceForPEFile( PEFile* peFile, OBJECTREF& objAsm, OBJECTREF& objAdditionalEvidence );

    inline void SetZone(DWORD dwZone) {
        m_dwZone = dwZone;
        SetZoneComputed();
    }

#endif // #ifndef DACCESS_COMPILE   

    // Override this method to return the Evidence
    virtual OBJECTREF GetEvidence() = 0;
    virtual DWORD GetZone() = 0;
    virtual VOID Resolve() = 0;
    virtual BOOL QuickIsFullyTrusted() = 0;

    // Checks for security permission for SkipVerification , PInvoke etc.
    BOOL CheckSecurityPermission(int index);

    BOOL IsResolved() const
    {
        LEAF_CONTRACT;
        return m_fSDResolved;
    }

    static OBJECTREF GetSerializedEvidence( PEFile* pPEFile );

    // Checks for one of the special security flags such as FullTrust or UnmanagedCode
    FORCEINLINE BOOL CheckSpecialFlag (DWORD flags) { LEAF_CONTRACT; return (m_dwSpecialFlags & flags); }
};

class PEFileSecurityDescriptor : public SecurityDescriptor
{
    friend class Security;
public:
#ifndef DACCESS_COMPILE
    PEFileSecurityDescriptor(AppDomain* pDomain, PEFile *pPEFile):
        SecurityDescriptor(pDomain, NULL,pPEFile)
    {
    }
#endif

protected:
    BOOL AllowBindingRedirects();

    virtual OBJECTREF GetEvidence();
    virtual DWORD GetZone();
    virtual BOOL QuickIsFullyTrusted();
    virtual VOID Resolve();
    VOID ResolveWorker();
};

#endif // #define __SECURITYDESCRIPTOR_H__

