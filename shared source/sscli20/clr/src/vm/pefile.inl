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
// --------------------------------------------------------------------------------
// PEFile.inl
// --------------------------------------------------------------------------------

#ifndef PEFILE_INL_
#define PEFILE_INL_

#include "strongname.h"
#include "fusionbind.h"
#include "check.h"
#include "simplerwlock.hpp"

#if CHECK_INVARIANTS
inline CHECK PEFile::Invariant()
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }   
    CONTRACT_CHECK_END;

    if (m_identity == NULL)
    {
        // dynamic module case
        CHECK(m_ILimage == NULL);
        CHECK(m_nativeImage == NULL);
        CHECK(CheckPointer(m_pEmitter));
    }
    else
    {
        // If m_image is null, then we should have a native image. However, this is not valid initially
        // during construction.  We should find a way to assert this.
        CHECK(CheckPointer((PEImage*) m_ILimage, NULL_OK));
        CHECK(CheckPointer((PEImage*) m_nativeImage, NULL_OK));
    }
    CHECK_OK;
}
#endif // CHECK_INVARIANTS

// ------------------------------------------------------------
// AddRef/Release
// ------------------------------------------------------------

inline void PEFile::AddRef()
{
    CONTRACTL
    {
        PRECONDITION(m_refCount < COUNT_T_MAX);
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    FastInterlockIncrement(&m_refCount);
}

inline ULONG PEFile::Release()
{
    CONTRACT(COUNT_T)
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;

    COUNT_T result = FastInterlockDecrement(&m_refCount);
    if (result == 0)
        delete this;
    
    RETURN result;
}

// ------------------------------------------------------------
// Identity
// ------------------------------------------------------------

inline ULONG PEFile::HashIdentity()
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(m_identity));
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!m_identity->HasID())
    {
        if (!IsLoaded())
            return 0;
        else
            return (ULONG) (SIZE_T) GetLoaded()->GetBase();
    }
    else
        return m_identity->GetIDHash();
}

inline BOOL PEFile::IsShareable()
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(m_identity));
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!m_identity->HasID())
        return FALSE;
    return TRUE ;
}

inline void PEFile::GetMVID(GUID *pMvid)
{
    WRAPPER_CONTRACT;

    GetPersistentMDImport()->GetScopeProps(NULL, pMvid);
}

inline BOOL PEFile::PassiveDomainOnly()
{
    WRAPPER_CONTRACT;
    return m_ILimage !=NULL && m_ILimage->PassiveDomainOnly();
}

// ------------------------------------------------------------
// Loader support routines
// ------------------------------------------------------------

inline void PEFile::SetSkipVerification()
{
    LEAF_CONTRACT;

    m_flags |= PEFILE_SKIP_VERIFICATION; 
}

inline BOOL PEFile::HasSkipVerification()
{
    LEAF_CONTRACT;

    return (m_flags & (PEFILE_SKIP_VERIFICATION | PEFILE_SYSTEM)) != 0; 
}

// ------------------------------------------------------------
// Identity
// ------------------------------------------------------------

#ifndef DACCESS_COMPILE
inline BOOL PEFile::Equals(HMODULE hMod)
{
    WRAPPER_CONTRACT;


    return IsLoaded() && GetLoaded()->GetBase() == (void *) hMod;
}
#endif // DACCESS_COMPILE

// ------------------------------------------------------------
// Descriptive strings
// ------------------------------------------------------------

inline const SString &PEFile::GetPath()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_identity == NULL)
    {
        return SString::Empty();
    }
    else
        return m_identity->GetPath();
}


#ifdef DACCESS_COMPILE
inline const SString &PEFile::GetModuleFileNameHint()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_identity == NULL)
    {
        return SString::Empty();
    }
    else
        return m_identity->GetModuleFileNameHintForDAC();
}
#endif // DACCESS_COMPILE

#ifdef LOGGING
inline LPCWSTR PEFile::GetDebugName()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
#ifdef _DEBUG
    return m_pDebugName;
#else
    return GetPath();
#endif
}
#endif

// ------------------------------------------------------------
// Classification
// ------------------------------------------------------------

inline BOOL PEFile::IsAssembly()
{
    LEAF_CONTRACT;

    return (m_flags & PEFILE_ASSEMBLY) != 0;
}

inline BOOL PEFile::IsModule()
{
    LEAF_CONTRACT;

    return (m_flags & PEFILE_MODULE) != 0;
}

inline BOOL PEFile::IsSystem()
{
    LEAF_CONTRACT;

    return (m_flags & PEFILE_SYSTEM) != 0;
}

inline BOOL PEFile::IsDynamic()
{
    LEAF_CONTRACT;

    return m_identity == NULL;
}

inline BOOL PEFile::IsResource()
{
    WRAPPER_CONTRACT;

    return IsModule() && PTR_PEModule(PTR_HOST_TO_TADDR(this))->IsResource();
}

inline BOOL PEFile::IsIStream()
{
    LEAF_CONTRACT;

    return (m_flags & PEFILE_ISTREAM) != 0;
}

inline BOOL PEFile::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;

    if (IsModule())
    {
        return PTR_PEModule(PTR_HOST_TO_TADDR(this))->GetAssembly()->IsIntrospectionOnly();
    }
    else
    {
        return (m_flags & PEFILE_INTROSPECTIONONLY) != 0;
    }
}

inline PEAssembly *PEFile::GetAssembly()
{
    WRAPPER_CONTRACT;
    
    if (IsAssembly())
        return PTR_PEAssembly(PTR_HOST_TO_TADDR(this));
    else
        return PTR_PEModule(PTR_HOST_TO_TADDR(this))->GetAssembly();
}

// ------------------------------------------------------------
// Hash support
// ------------------------------------------------------------

#ifndef DACCESS_COMPILE
inline void PEFile::GetImageBits(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    EnsureImageOpened();
    // We don't cache any other hashes right now.
    if (m_ILimage)
        m_ILimage->GetImageBits(PEImageLayout::LAYOUT_FLAT,result);
}

inline void PEFile::GetHash(ALG_ID algorithm, SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (algorithm == CALG_SHA1)
    {
        GetSHA1Hash(result);
    }
    else
    {
        EnsureImageOpened();
        // We don't cache any other hashes right now.
        m_ILimage->ComputeHash(algorithm, result);
    }
}
    
inline CHECK PEFile::CheckHash(ALG_ID algorithm, const void *hash, COUNT_T size)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(hash));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    StackSBuffer hashBuffer;
    GetHash(algorithm, hashBuffer);

    CHECK(hashBuffer.Equals((const BYTE *)hash, size));

    CHECK_OK;
}
#endif // DACCESS_COMPILE

// ------------------------------------------------------------
// Metadata access
// ------------------------------------------------------------

inline BOOL PEFile::HasMetadata()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    return !IsResource();
}


inline IMDInternalImport *PEFile::GetPersistentMDImport()
{
/*
    CONTRACT(IMDInternalImport *) 
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;
*/
#if !defined(__GNUC__)
    _ASSERTE(!IsResource());
#endif    
#ifdef DACCESS_COMPILE
    WRAPPER_CONTRACT;
    return DacGetMDImport(this, true);
#else
    LEAF_CONTRACT;
    return m_pMDImport;
#endif
}

inline IMDInternalImport *PEFile::GetMDImportWithRef()
{
/*
    CONTRACT(IMDInternalImport *) 
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;
*/
#if !defined(__GNUC__)
    _ASSERTE(!IsResource());
#endif    
#ifdef DACCESS_COMPILE
    WRAPPER_CONTRACT;
    return DacGetMDImport(this, true);
#else
    LEAF_CONTRACT;
    GCX_PREEMP();
    SimpleReadLockHolder lock(m_pMetadataLock);
    if(m_pMDImport)
        m_pMDImport->AddRef();
    return m_pMDImport;
#endif
}

#ifndef DACCESS_COMPILE

inline IMetaDataImport2 *PEFile::GetRWImporter2()
{
    CONTRACT(IMetaDataImport2 *) 
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);
        GC_NOTRIGGER;
        THROWS;
        MODE_ANY;
    }
    CONTRACT_END;

    if (m_pImporter2 == NULL)
        OpenImporter();

    RETURN m_pImporter2;
}


inline IMetaDataImport *PEFile::GetRWImporter()
{
    CONTRACT(IMetaDataImport *) 
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    if (m_pImporter == NULL)
        OpenImporter();

    RETURN m_pImporter;
}

inline IMetaDataEmit *PEFile::GetEmitter()
{
    CONTRACT(IMetaDataEmit *) 
    {
        INSTANCE_CHECK;
        MODE_ANY;
        GC_NOTRIGGER;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);        
        THROWS;
    }
    CONTRACT_END;

    if (m_pEmitter == NULL)
        OpenEmitter();

    RETURN m_pEmitter;
}

inline IMetaDataAssemblyImport *PEFile::GetAssemblyImporter()
{
    CONTRACT(IMetaDataAssemblyImport *) 
    {
        INSTANCE_CHECK;
        MODE_ANY;
        GC_NOTRIGGER;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);
        THROWS;
    }
    CONTRACT_END;

    if (m_pAssemblyImporter == NULL)
        OpenAssemblyImporter();

    RETURN m_pAssemblyImporter;
}

inline IMetaDataAssemblyEmit *PEFile::GetAssemblyEmitter()
{
    CONTRACT(IMetaDataAssemblyEmit *) 
    {
        INSTANCE_CHECK;
        MODE_ANY;
        GC_NOTRIGGER;
        PRECONDITION(!IsResource());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);        
    }
    CONTRACT_END;

    if (m_pAssemblyEmitter == NULL)
        OpenAssemblyEmitter();

    RETURN m_pAssemblyEmitter;
}

#endif // DACCESS_COMPILE

inline LPCUTF8 PEFile::GetSimpleName()
{
    CONTRACT(LPCUTF8)
    {
        INSTANCE_CHECK;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACT_END;

    if (IsAssembly())
        RETURN (PTR_PEAssembly(PTR_HOST_TO_TADDR(this)))->GetSimpleName();
    else if (IsModule())
        RETURN (PTR_PEModule(PTR_HOST_TO_TADDR(this)))->GetSimpleName();
    else 
    {
        LPCSTR name="";
        GetMDImport()->GetScopeProps(&name, NULL);
        RETURN name;
    }
}

// ------------------------------------------------------------
// PE file access
// ------------------------------------------------------------

inline BOOL PEFile::HasSecurityDirectory()
{
    WRAPPER_CONTRACT;

    if (IsResource() || IsDynamic())
        return FALSE;


    if (!m_ILimage->HasNTHeaders())
        return FALSE;

    return m_ILimage->HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_SECURITY);
}

inline WORD PEFile::GetSubsystem()
{
    WRAPPER_CONTRACT;

    if (IsResource() || IsDynamic())
        return 0;

 
    return GetLoadedIL()->GetSubsystem();
}

inline mdToken PEFile::GetEntryPointToken(
#ifdef _DEBUG      
            BOOL bAssumeLoaded
#endif //_DEBUG           
            )
{
    WRAPPER_CONTRACT;

    if (IsResource() || IsDynamic())
        return mdTokenNil;


    _ASSERTE (!bAssumeLoaded || m_ILimage->HasLoadedLayout ());
    return m_ILimage->GetEntryPointToken (TRUE);
}

inline BOOL PEFile::IsNativeLoaded()
{
    WRAPPER_CONTRACT;
    return (m_nativeImage && m_bHasPersistentMDImport && m_nativeImage->HasLoadedLayout());
}

inline BOOL PEFile::IsILOnly()
{
    LEAF_CONTRACT;
    CONTRACT_VIOLATION(ThrowsViolation|GCViolation);
    if (IsResource() || IsDynamic())
        return FALSE;



    return m_ILimage->IsILOnly();
}

inline BOOL PEFile::IsDll()
{
    WRAPPER_CONTRACT;

    if (IsResource() || IsDynamic())
        return TRUE;


    return m_ILimage->IsDll();
}

inline const void *PEFile::GetIL(RVA il)
{
/*
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;
*/

    WRAPPER_CONTRACT;

    _ASSERTE(!IsDynamic() && !IsResource());
#ifndef DACCESS_COMPILE
    _ASSERTE(CheckLoaded());
#endif
PEImageLayout *image = NULL;

    {
        image = GetLoadedIL();
    }
    if (il < image->GetVirtualSize())
        return (void *) image->GetRvaData(il);
    else
    {
        // RVA is a Phony RVA
        return ((const BYTE*) image->GetBase() + il);
    }
}

inline CHECK PEFile::CheckIL(RVA il)
{
/*
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;
*/

    WRAPPER_CONTRACT;

    _ASSERTE(!IsDynamic() && !IsResource());
    _ASSERTE(CheckLoaded());


    CHECK(GetLoadedIL()->CheckRva(il));
    CHECK_OK;
}

inline CHECK PEFile::CheckIL(RVA il, COUNT_T size)
{
/*
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;
*/

    WRAPPER_CONTRACT;
    _ASSERTE(!IsDynamic() && !IsResource());
    _ASSERTE(CheckLoaded());
    

    CHECK(GetLoadedIL()->CheckRva(il, size));
    CHECK_OK;
}

inline RVA PEFile::GetPhonyILRva(const void *pIL)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckPointer(pIL));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    return (BYTE *) pIL - (BYTE *)GetLoadedIL()->GetBase();
}

inline BYTE *PEFile::GetPhonyILBase()
{
    CONTRACT(BYTE *)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    // Note it is important to get the IL base from the native image if 
    // available, since we are using the metadata from the native image
    // which has different IL rva's.
    if (HasNativeImageMetadata())
        RETURN (BYTE *) GetLoadedNative()->GetBase();
    else
        RETURN (BYTE *) GetLoadedIL()->GetBase();
            
}

inline void *PEFile::GetRvaField(RVA field)
{    
    CONTRACT(void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckRvaField(field));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Note that the native image Rva fields are currently cut off before 
    // this point.  We should not get here for an IL only native image.
    
    RETURN (void *)GetLoadedIL()->GetRvaData(field,NULL_OK);
}

inline CHECK PEFile::CheckRvaField(RVA field)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;
        
    // Note that the native image Rva fields are currently cut off before 
    // this point.  We should not get here for an IL only native image.
    
    CHECK(GetLoadedIL()->CheckRva(field,NULL_OK));
    CHECK_OK;
}

inline CHECK PEFile::CheckRvaField(RVA field, COUNT_T size)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    // Note that the native image Rva fields are currently cut off before 
    // this point.  We should not get here for an IL only native image.
    
    CHECK(GetLoadedIL()->CheckRva(field, size,0,NULL_OK));
    CHECK_OK;
}

inline BOOL PEFile::HasTls()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckLoaded());
    }
    CONTRACTL_END;

    if (IsResource() || IsDynamic())
        return FALSE;
    else if (IsILOnly())
        return FALSE;
    else
        return GetLoadedIL()->HasTls();
}

inline BOOL PEFile::IsRvaFieldTls(RVA field)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckLoaded());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Check ILOnly since we may not have m_image contents
    if (IsILOnly())
        return FALSE;

    if (!GetLoadedIL()->HasTls())
        return FALSE;

    void *address = (void *)GetLoadedIL()->GetRvaData(field);

    COUNT_T tlsSize;
    void *tlsRange = GetLoadedIL()->GetTlsRange(&tlsSize);

    return (address >= tlsRange
            && address < ((BYTE*)tlsRange+tlsSize));
}

inline UINT32 PEFile::GetFieldTlsOffset(RVA field)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckRvaField(field));
        PRECONDITION(IsRvaFieldTls(field));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
        
    return (BYTE *) GetRvaField(field) - (BYTE *) GetLoadedIL()->GetTlsRange();
}
        
inline UINT32 PEFile::GetTlsIndex()
{
    CONTRACTL
    {
        PRECONDITION(CheckLoaded());
        INSTANCE_CHECK;
        PRECONDITION(HasTls());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    return GetLoadedIL()->GetTlsIndex();
}

inline const void *PEFile::GetInternalPInvokeTarget(RVA target)
{
    CONTRACT(void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckInternalPInvokeTarget(target));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
     }
    CONTRACT_END;
//    if (HasNativeImageMetadata())
//        RETURN (void *)GetLoadedNative()->GetRvaData(target+GetLoadedNative()->GetNativeILBase());
//    else
        RETURN (void*)GetLoadedIL()->GetRvaData(target);
}

inline CHECK PEFile::CheckInternalPInvokeTarget(RVA target)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    CHECK(!IsILOnly());
    CHECK(GetLoadedIL()->CheckRva(target));
    
    CHECK_OK;
}
    
inline PCCOR_SIGNATURE  PEFile::GetSignature(RVA signature)
{    
    CONTRACT(PCCOR_SIGNATURE)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic() || signature == 0);
        PRECONDITION(!IsResource());
        PRECONDITION(CheckSignatureRva(signature));
        POSTCONDITION(CheckSignature(RETVAL));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    if (signature == 0)
        RETURN NULL;
    else
        RETURN (PCCOR_SIGNATURE) GetLoadedIL()->GetRvaData(signature);
}

inline RVA PEFile::GetSignatureRva(PCCOR_SIGNATURE signature)
{
    CONTRACT(RVA)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic() || signature == NULL);
        PRECONDITION(!IsResource());
        PRECONDITION(CheckSignature(signature));
        POSTCONDITION(CheckSignatureRva(RETVAL));
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    if (signature == NULL)
        RETURN 0;
    else
        RETURN GetLoadedIL()->GetDataRva(PTR_HOST_TO_TADDR((void *)signature));
}

inline CHECK PEFile::CheckSignature(PCCOR_SIGNATURE signature)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic() || signature == NULL);
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;
        
    CHECK(GetLoadedIL()->CheckData(signature,NULL_OK));
    CHECK_OK;
}

inline CHECK PEFile::CheckSignatureRva(RVA signature)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic() || signature == NULL);
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;
        
    CHECK(GetLoadedIL()->CheckRva(signature,NULL_OK));
    CHECK_OK;
}

inline IMAGE_COR_VTABLEFIXUP *PEFile::GetVTableFixups(COUNT_T *pCount/*=NULL*/)
{
    CONTRACT(IMAGE_COR_VTABLEFIXUP *)
    {
        PRECONDITION(CheckLoaded());
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    if (IsResource() || IsDynamic() || IsILOnly())
    {
        if (pCount != NULL)
            *pCount = 0;
        RETURN NULL;
    }
    else
        RETURN GetLoadedIL()->GetVTableFixups(pCount);
}

inline void *PEFile::GetVTable(RVA rva)
{
    CONTRACT(void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        PRECONDITION(!IsILOnly());
        PRECONDITION(GetLoadedIL()->CheckRva(rva));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    RETURN (void *)GetLoadedIL()->GetRvaData(rva);
}

inline HMODULE PEFile::GetIJWBase()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(!IsDynamic());
        PRECONDITION(!IsResource());
        PRECONDITION(CheckLoaded());
        PRECONDITION(!IsILOnly());
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    return (HMODULE) GetLoadedIL()->GetBase();
}

inline void *PEFile::GetDebuggerContents(COUNT_T *pSize/*=NULL*/)
{
    CONTRACT(BYTE *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    // We cannot in general force a LoadLibrary; we might be in the 
    // helper thread.  The debugger will have to expect a zero base
    // in some circumstances.

    if (IsLoaded())
    {
        if (pSize != NULL)
            *pSize = GetLoaded()->GetSize();

        RETURN (BYTE *) GetLoaded()->GetBase();
    }
    else
    {
        if (pSize != NULL)
            *pSize = 0;

        RETURN NULL;
    }
}

inline const void *PEFile::GetLoadedImageContents(COUNT_T *pSize/*=NULL*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (IsLoaded() && !IsDynamic())
    {
        if (pSize != NULL)
        {
            *pSize = GetLoaded()->GetSize();
        }
        return GetLoaded()->GetBase();
    }
    else
    {
        if (pSize != NULL)
        {
            *pSize = 0;
        }
        return NULL;
    }
}

#ifndef DACCESS_COMPILE
inline const void *PEFile::GetManagedFileContents(COUNT_T *pSize/*=NULL*/)
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckLoaded());
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        POSTCONDITION((!GetLoaded()->GetSize()) || CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Right now, we will trigger a LoadLibrary for the caller's sake, 
    // even if we are in a scenario where we could normally avoid it.
    LoadLibrary(FALSE);

    if (pSize != NULL)
        *pSize = GetLoadedIL()->GetSize();

    RETURN GetLoadedIL()->GetBase();
}
#endif // DACCESS_COMPILE

// ------------------------------------------------------------
// Native image access
// ------------------------------------------------------------

inline BOOL PEFile::HasNativeImage()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (m_nativeImage != NULL);
}

inline PTR_PEImageLayout PEFile::GetLoadedIL() 
{
    WRAPPER_CONTRACT;
    if(IsIntrospectionOnly())
        return m_ILimage->GetLoadedIntrospectionLayout();
    
    _ASSERTE(m_ILimage && m_ILimage->GetLoadedLayout());
    return m_ILimage->GetLoadedLayout();
};

inline PTR_PEImageLayout PEFile::GetAnyILWithRef() 
{
    WRAPPER_CONTRACT;
    _ASSERTE(m_ILimage);
    return m_ILimage->GetLayout(PEImageLayout::LAYOUT_ANY,PEImage::LAYOUT_CREATEIFNEEDED);
};


inline BOOL PEFile::IsLoaded(BOOL bAllowNative/*=TRUE*/) 
{
    WRAPPER_CONTRACT;
    if(IsDynamic())
        return TRUE;
    if(IsIntrospectionOnly())
    {
        return m_ILimage->HasLoadedIntrospectionLayout();
    }
        return m_ILimage->HasLoadedLayout();
};


inline PTR_PEImageLayout PEFile::GetLoaded() 
{
    WRAPPER_CONTRACT;
    return HasNativeImage()?GetLoadedNative():GetLoadedIL();
};

inline PTR_PEImageLayout PEFile::GetLoadedNative()
{
    WRAPPER_CONTRACT;
    PEImage* pImage=GetPersistentNativeImage();
    _ASSERTE(pImage && pImage->GetLoadedLayout());
    return pImage->GetLoadedLayout();
};

inline PEImage *PEFile::GetPersistentNativeImage()
{
    CONTRACT(PEImage *)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNativeImage());
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(m_bHasPersistentMDImport);
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;        
        PRECONDITION(m_bHasPersistentMDImport);
    }
    CONTRACT_END;

    RETURN m_nativeImage;
}

#ifndef DACCESS_COMPILE
inline PEImage *PEFile::GetNativeImageWithRef()
{
    CONTRACT(PEImage *)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS; 
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL,NULL_OK));
    }
    CONTRACT_END;
    GCX_PREEMP();
    SimpleReadLockHolder mdlock(m_pMetadataLock);
    if(m_nativeImage)
        m_nativeImage->AddRef();
    RETURN m_nativeImage;
}
#endif // DACCESS_COMPILE

inline BOOL PEFile::HasNativeImageMetadata()
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN ((m_flags & PEFILE_HAS_NATIVE_IMAGE_METADATA) != 0);
}


// ------------------------------------------------------------
// Descriptive strings
// ------------------------------------------------------------
inline void PEAssembly::GetDisplayName(SString &result, DWORD flags)
{
    CONTRACTL
    {
        PRECONDITION(CheckValue(result));
#ifndef DACCESS_COMPILE
        THROWS;
#else
        NOTHROW;
#endif // DACCESS_COMPILE
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    FusionBind::GetAssemblyNameDisplayName(GetFusionAssemblyName(), result, flags);

#else
    IMDInternalImport* pImport = GetMDImport();        
    if(pImport != NULL)
    {
        // This is for DAC, ONLY for the binding tool.  Don't use for other
        // purposes, since this is not canonicalized through Fusion.
        LPCSTR name;
        AssemblyMetaDataInternal context;
        DWORD dwFlags;
        PBYTE pbPublicKey;
        DWORD cbPublicKey;
        pImport->GetAssemblyProps(TokenFromRid(1, mdtAssembly),
                                  (const void **) &pbPublicKey,
                                  &cbPublicKey,
                                  NULL, 
                                  &name,
                                  &context,
                                  &dwFlags);

        result.SetUTF8(name);

        result.AppendPrintf(L", Version=%u.%u.%u.%u",
                            context.usMajorVersion, context.usMinorVersion,
                            context.usBuildNumber, context.usRevisionNumber);

        result.Append(L", Culture=");
        if (!*context.szLocale)
        {
            result.Append(L"neutral");
        }
        else
        {
            result.AppendUTF8(context.szLocale);
        }

        if (cbPublicKey)
        {
            /*
              //                       
            LPBYTE pbToken = NULL;
            DWORD cbToken;
            CQuickBytes qb;

            if (StrongNameTokenFromPublicKey(pbPublicKey, cbPublicKey,
                                             &pbToken, &cbToken))
            {
                WCHAR* szToken = (WCHAR*) qb.AllocNoThrow(sizeof(WCHAR) * (cbToken+1));
                if (szToken)
                {
#define TOHEX(a) ((a)>=10 ? L'a'+(a)-10 : L'0'+(a))
                    UINT x;
                    UINT y;
                    for ( x = 0, y = 0; x < cbToken; ++x )
                    {
                        UINT v = pbToken[x] >> 4;
                        szToken[y++] = TOHEX( v );  
                        v = pbToken[x] & 0x0F;                 
                        szToken[y++] = TOHEX( v ); 
                    }                                    
                    szToken[y] = L'\0';

                    StrongNameFreeBuffer(pbToken);
                    result.Append(L", PublicKeyToken=");
                    result.Append(szToken);
                }
            }
            */
        }
        else
        {
            result.Append(L", PublicKeyToken=null");
        }

        if (dwFlags & afPA_Mask)
        {
            result.Append(L", ProcessorArchitecture=");

            if (dwFlags & afPA_MSIL)
                result.Append(L"MSIL");
            else if (dwFlags & afPA_x86)
                result.Append(L"x86");
            else if (dwFlags & afPA_IA64)
                result.Append(L"IA64");
            else if (dwFlags & afPA_AMD64)
                result.Append(L"AMD64");
        }
    }
#endif // DACCESS_COMPILE

}

// ------------------------------------------------------------
// Metadata access
// ------------------------------------------------------------

inline LPCSTR PEAssembly::GetSimpleName()
{
    CONTRACTL
    {
        NOTHROW;
        if (!m_bHasPersistentMDImport) { GC_TRIGGERS;} else {DISABLED(GC_TRIGGERS);};
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LPCSTR name = "";
    IMDInternalImport* pImport = GetMDImport();
    if(pImport != NULL)
    {
        _ASSERTE(pImport->IsValidToken(TokenFromRid(1, mdtAssembly)));
        pImport->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, &name, NULL, NULL);
    }
    return name;
}

inline BOOL PEFile::IsStrongNamed()
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_NOTRIGGER);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    _ASSERTE(GetMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
        
    DWORD flags = 0;
    GetMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, NULL, NULL, & flags);
    return (flags & afPublicKey) != NULL;
}

inline const void *PEFile::GetPublicKey(DWORD *pcbPK)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pcbPK, NULL_OK));
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
    const void *pPK;    
    _ASSERTE(GetMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
    GetMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), &pPK, pcbPK, NULL, NULL, NULL, NULL);
    return pPK;
}            
    
inline ULONG PEFile::GetHashAlgId()
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ULONG hashAlgId;
    _ASSERTE(GetMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
    GetMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, &hashAlgId, NULL, NULL, NULL);
    return hashAlgId;
}

inline LPCSTR PEFile::GetLocale()
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_NOTRIGGER);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    AssemblyMetaDataInternal md;
    _ASSERTE(GetMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
    GetMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, NULL, &md, NULL);
    return md.szLocale;
}

inline DWORD PEFile::GetFlags()
{
    CONTRACTL
    {
        PRECONDITION(IsAssembly());
        NOTHROW;
        WRAPPER(GC_NOTRIGGER);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    DWORD flags;
    _ASSERTE(GetMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
    GetMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, NULL, NULL, &flags);
    return flags;
}

inline COR_TRUST *PEFile::GetSecuritySignature()
{
    LEAF_CONTRACT;

    return m_certificate;
}


// ------------------------------------------------------------
// Hash support
// ------------------------------------------------------------

inline BOOL PEAssembly::HasStrongNameSignature()
{
    WRAPPER_CONTRACT;

    if (m_ILimage == NULL)
        return FALSE;


    return m_ILimage->HasStrongNameSignature();
}


#ifndef DACCESS_COMPILE

inline void PEAssembly::GetEffectiveStrongNameSignature(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        PRECONDITION(HasStrongNameSignature());
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;


    m_ILimage->GetEffectiveStrongNameSignature(result);

    return;
}
#endif // DACCESS_COMPILE


inline BOOL PEAssembly::NeedsModuleHashChecks()
{
    LEAF_CONTRACT;

    return (m_flags & PEFILE_SKIP_MODULE_HASH_CHECKS) == 0;
}

// ------------------------------------------------------------
// Metadata access
// ------------------------------------------------------------

inline PEAssembly *PEModule::GetAssembly()
{
    CONTRACT(PEAssembly *)
    {
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN m_assembly;
}

inline BOOL PEModule::IsResource()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
#ifdef DACCESS_COMPILE
    _ASSERTE(m_bIsResource!=-1);
#else
    if (m_bIsResource==-1)
    {
        DWORD flags;
        m_assembly->GetPersistentMDImport()->GetFileProps(m_token, NULL, NULL, NULL, &flags);
        m_bIsResource=((flags & ffContainsNoMetaData) != 0);
    }
#endif
    
    return m_bIsResource;
}

inline LPCUTF8 PEModule::GetSimpleName()
{
    CONTRACT(LPCUTF8)
    {
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(strlen(RETVAL) > 0);
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACT_END;

    LPCUTF8 name;

    m_assembly->GetPersistentMDImport()->GetFileProps(m_token, &name, NULL, NULL, NULL);

    RETURN name;
}

inline mdFile PEModule::GetToken()
{
    LEAF_CONTRACT;
    return m_token;
}
#ifndef DACCESS_COMPILE
inline void PEFile::RestoreMDImport(IMDInternalImport* pImport)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
    _ASSERTE(m_pMetadataLock->LockTaken() && m_pMetadataLock->IsWriterLock());    
    if (m_pMDImport != NULL)
        return;
    m_pMDImport=pImport;
    if(m_pMDImport)
        m_pMDImport->AddRef();
}
#endif
inline void PEFile::OpenMDImport()
{
    //need synchronization
    _ASSERTE(m_pMetadataLock->LockTaken() && m_pMetadataLock->IsWriterLock());    
    OpenMDImport_Unsafe();
}


inline IStream * PEFile::GetPdbStream() 
{
    WRAPPER_CONTRACT;
    return     m_ILimage->GetPdbStream();
};    

inline void PEFile::ClearPdbStream() 
{
    WRAPPER_CONTRACT;
    m_ILimage->ClearPdbStream();
};    



#endif  // PEFILE_INL_
