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

#ifndef __SECURITYDESCRIPTOR_ASSEMBLY_H__
#define __SECURITYDESCRIPTOR_ASSEMBLY_H__
#include "securitydescriptor.h"
class Assembly;
class DomainAssembly;

// Security flags for the objects that store security information
#define CORSEC_ASSERTED             0x000020 // Asseted permission set present on frame
#define CORSEC_DENIED               0x000040 // Denied permission set present on frame
#define CORSEC_REDUCED              0x000080 // Reduced permission set present on frame


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

#define MAX_PASSED_DEMANDS 10

//------------------------------------------------------------------
//
//          ASSEMBLY SECURITY DESCRIPTOR
//
//------------------------------------------------------------------

#ifndef DACCESS_COMPILE
void StoreObjectInLazyHandle(OBJECTHANDLE& handle, OBJECTREF ref, AppDomain* app_dom);
#endif
class AssemblySecurityDescriptor : public SecurityDescriptor
{
friend class SecurityDescriptor;
friend class SharedSecurityDescriptor;
friend class Security;
friend class SecurityPolicy;
friend class SecurityStackWalk;
friend class SecurityDeclarative;
friend class ApplicationSecurityDescriptor;
friend class SecurityTransparent;
friend class SecurityConfig;
friend class DemandStackWalk;
friend class DomainCompressedStack;
friend class AssertStackWalk;
friend struct MethodSecurityDescriptor;
private:
    //--------------------
    // Members
    //--------------------
    OBJECTHANDLE m_hRequiredPermissionSet;  // Required Requested Permissions
    OBJECTHANDLE m_hOptionalPermissionSet;  // Optional Requested Permissions
    OBJECTHANDLE m_hDeniedPermissionSet;    // Denied Permissions

    DWORD   m_arrPassedLinktimeDemands[MAX_PASSED_DEMANDS];
    DWORD   m_dwNumPassedDemands;

    COR_TRUST                  *m_pSignature;      // Contains the publisher, requested permission
    SharedSecurityDescriptor   *m_pSharedSecDesc;  // Shared state for assemblies loaded into multiple appdomains

    BOOL                m_fAdditionalEvidence;
    BOOL                m_fIsSignatureLoaded;
    BOOL                m_fAssemblyRequestsComputed;

public:
    //--------------------
    // Exposed APIs
    //--------------------
#ifndef DACCESS_COMPILE

    AssemblySecurityDescriptor(AppDomain *pDomain, DomainAssembly *pAssembly);

protected:
    void AddToSharedDescriptor(SharedSecurityDescriptor *pSharedDesc);

    inline void SetRequestedPermissionSet(OBJECTREF RequiredPermissionSet,
                                          OBJECTREF OptionalPermissionSet,
                                          OBJECTREF DeniedPermissionSet)
    {
        WRAPPER_CONTRACT;

        StoreObjectInLazyHandle(m_hRequiredPermissionSet, RequiredPermissionSet, m_pAppDomain);
        StoreObjectInLazyHandle(m_hOptionalPermissionSet, OptionalPermissionSet, m_pAppDomain);
        StoreObjectInLazyHandle(m_hDeniedPermissionSet, DeniedPermissionSet, m_pAppDomain);
        m_fAssemblyRequestsComputed = TRUE;
    }

    BOOL IsAssemblyRequestsComputed() 
    {
        LEAF_CONTRACT;
        return m_fAssemblyRequestsComputed;
    }

    inline void SetAdditionalEvidence(OBJECTREF evidence)
    {
        CONTRACTL {
            THROWS; // From StoreObjectInLazyHandle
            GC_NOTRIGGER;
            MODE_ANY;
        } CONTRACTL_END;

        StoreObjectInLazyHandle(m_hAdditionalEvidence, evidence, m_pAppDomain);
        m_fAdditionalEvidence = TRUE;
    }

    Assembly* GetAssembly();

    virtual VOID Resolve();
    void ResolveWorker();
#endif // #ifndef DACCESS_COMPILE

    inline BOOL HasAdditionalEvidence()
    {
        WRAPPER_CONTRACT;
        return m_fAdditionalEvidence;
    }

    inline OBJECTREF GetAdditionalEvidence(void)
    {
        WRAPPER_CONTRACT;
        return ObjectFromLazyHandle(m_hAdditionalEvidence);
    }

    BOOL CanSkipVerification();
    BOOL CanAssert();
    BOOL HasUnrestrictedUIPermission();

    inline SharedSecurityDescriptor *GetSharedSecDesc()
    { 
        LEAF_CONTRACT;
        return m_pSharedSecDesc; 
    }

    inline BOOL IsSigned() 
    {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        } CONTRACTL_END;

        LoadSignature();
        return (m_pSignature != NULL);
    }

    BOOL IsSignatureLoaded()
    {
        LEAF_CONTRACT;
        return m_fIsSignatureLoaded;
    }

    VOID SetSignatureLoaded()
    {
        LEAF_CONTRACT;
        m_fIsSignatureLoaded = TRUE;
    }

    FORCEINLINE BOOL IsSystem();

    HRESULT LoadSignature(COR_TRUST **ppSignature = NULL);
    virtual OBJECTREF GetRequestedPermissionSet(OBJECTREF *pOptionalPermissionSet, OBJECTREF *pDeniedPermissionSet);
    virtual OBJECTREF GetEvidence();

    //--------------------
    // Internal (to security) Helper Methods
    //--------------------

    virtual DWORD GetZone();
    virtual BOOL QuickIsFullyTrusted();
    virtual OBJECTREF GetSerializedEvidence();

private:
    BOOL ShouldSkipPolicyResolution();
};
typedef DPTR(class AssemblySecurityDescriptor) PTR_AssemblySecurityDescriptor;


// This really isn't in the SecurityDescriptor hierarchy, per-se. It's attached
// to the unmanaged assembly object and used to store common information when
// the assembly is shared across multiple appdomains.
class SharedSecurityDescriptor
{
friend class Security;
friend class SecurityPolicy;
friend class SecurityStackWalk;
friend class SecurityDeclarative;
friend class SecurityDescriptor;
friend class AssemblySecurityDescriptor;
friend class DomainCompressedStack;
friend class NewCompressedStack;
private:
    //--------------------
    // Members
    //--------------------

    // Unmanaged assembly this descriptor is attached to.
    Assembly           *m_pAssembly;

    // All policy resolution is funnelled through the shared descriptor so we
    // can guarantee everyone's using the same grant/denied sets.
    BOOL                m_fResolved;
    BOOL                m_fFullyTrusted;
    BOOL                m_fCanCallUnmanagedCode;
    BOOL                m_fCanAssert;

public:
    //--------------------
    // Exposed APIs
    //--------------------
    SharedSecurityDescriptor(Assembly *pAssembly);

protected:
    // All policy resolution is funnelled through the shared descriptor so we
    // can guarantee everyone's using the same grant/denied sets.
    void Resolve(AssemblySecurityDescriptor *pSecDesc);

    // Is this assembly a system assembly?
    FORCEINLINE BOOL IsSystem();

    BOOL IsResolved()
    { 
        LEAF_CONTRACT;
        return m_fResolved;
    }

    Assembly* GetAssembly()
    { 
        LEAF_CONTRACT;
        return m_pAssembly;
    }

    BOOL IsFullyTrusted();
    BOOL CanCallUnmanagedCode();
    BOOL CanAssert();
};
#endif // #define __SECURITYDESCRIPTOR_ASSEMBLY_H__
