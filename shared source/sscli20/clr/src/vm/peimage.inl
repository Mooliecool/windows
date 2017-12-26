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
// PEImage.inl
// --------------------------------------------------------------------------------

#ifndef PEIMAGE_INL_
#define PEIMAGE_INL_

#include "peimage.h"

inline void PEImage::AddRef()
{
    CONTRACT_VOID
    {
        PRECONDITION(m_refCount>0 && m_refCount < COUNT_T_MAX);
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    FastInterlockIncrement(&m_refCount);

    RETURN;
}

inline const SString &PEImage::GetPath()
{
    LEAF_CONTRACT;

    return m_path;
}

#ifdef DACCESS_COMPILE
inline const SString &PEImage::GetModuleFileNameHintForDAC()
{
    LEAF_CONTRACT;

    return m_sModuleFileNameHintUsedByDac;
}
#endif 



inline BOOL PEImage::IsFile()
{
    WRAPPER_CONTRACT;

    return !m_path.IsEmpty();
}

#ifndef DACCESS_COMPILE
inline void   PEImage::SetLayout(DWORD dwLayout, PEImageLayout* pLayout)
{
    LEAF_CONTRACT;
    _ASSERTE(dwLayout<IMAGE_COUNT);
    _ASSERTE(m_pLayouts[dwLayout]==NULL);
    FastInterlockExchangePointer((volatile PVOID*)(m_pLayouts+dwLayout),pLayout);
}
#endif  // DACCESS_COMPILE
inline PTR_PEImageLayout PEImage::GetLoadedLayout()
{
    LEAF_CONTRACT;
    _ASSERTE(m_pLayouts[IMAGE_LOADED]!=NULL);
    return m_pLayouts[IMAGE_LOADED]; //no addref
}

inline PTR_PEImageLayout PEImage::GetLoadedIntrospectionLayout()
{
    LEAF_CONTRACT;
    _ASSERTE(m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]!=NULL);
    return m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]; //no addref
}

inline PTR_PEImageLayout PEImage::GetLayoutInternal(DWORD imageLayoutMask,DWORD flags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    PTR_PEImageLayout pRetVal=NULL;
 
    if (imageLayoutMask&PEImageLayout::LAYOUT_LOADED)
        pRetVal=m_pLayouts[IMAGE_LOADED];
    if (pRetVal==NULL &&(imageLayoutMask&PEImageLayout::LAYOUT_LOADED_FOR_INTROSPECTION)) 
        pRetVal=m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION];
    if (pRetVal==NULL && (imageLayoutMask&PEImageLayout::LAYOUT_MAPPED))
        pRetVal=m_pLayouts[IMAGE_MAPPED];
    if (pRetVal==NULL && (imageLayoutMask&PEImageLayout::LAYOUT_FLAT))
        pRetVal=m_pLayouts[IMAGE_FLAT];

#ifndef DACCESS_COMPILE
    if (pRetVal==NULL && (flags&LAYOUT_CREATEIFNEEDED))    
    {
        _ASSERTE(!m_path.IsEmpty() || m_fIsIStream);

        // Win9x doesn't support SEC_IMAGE so use FLAT layout if possible
        if (RunningOnWin95() && imageLayoutMask&PEImageLayout::LAYOUT_FLAT)
            imageLayoutMask&=~PEImageLayout::LAYOUT_MAPPED;
        
        if (imageLayoutMask&PEImageLayout::LAYOUT_MAPPED)
        {
            {
                PEImageLayoutHolder flatPE(GetLayoutInternal(PEImageLayout::LAYOUT_FLAT,LAYOUT_CREATEIFNEEDED));
                if (!flatPE->CheckFormat())
                    ThrowFormat();
                pRetVal=PEImageLayout::LoadFromFlat(flatPE);
                pRetVal->AddRef();
                SetLayout(IMAGE_MAPPED,pRetVal);
            }

        }
        else
        if (imageLayoutMask&PEImageLayout::LAYOUT_FLAT)    
        {
            _ASSERTE(!m_fIsIStream); //images created from streams should always have this one
            pRetVal=PEImageLayout::LoadFlat(GetFileHandle(),this);
            m_pLayouts[IMAGE_FLAT]=pRetVal;
        }
        
    }
    if (pRetVal)
    {
        if (flags&LAYOUT_PERSISTENT)
            pRetVal->MakePersistent();
        pRetVal->AddRef();
    }
#endif
    return PTR_PEImageLayout(PTR_HOST_TO_TADDR(pRetVal));
}


inline BOOL PEImage::HasLoadedLayout()
{
    LEAF_CONTRACT;
    return m_pLayouts[IMAGE_LOADED]!=NULL;
}

inline BOOL PEImage::IsOpened()
{
    LEAF_CONTRACT;
    return m_pLayouts[IMAGE_LOADED]!=NULL ||m_pLayouts[IMAGE_MAPPED]!=NULL || m_pLayouts[IMAGE_FLAT] !=NULL || m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]!=NULL;
}


inline BOOL PEImage::HasLoadedIntrospectionLayout() //introspection only!!!
{
    LEAF_CONTRACT;
    return m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]!=NULL;
}

inline CHECK PEImage::CheckILFormat() 
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        CHECK(GetLoadedLayout()->CheckILFormat());
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        CHECK(pLayout->CheckILFormat());
    }
    CHECK_OK;
};


inline const BOOL PEImage::HasNTHeaders()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasNTHeaders();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasNTHeaders();
    }
}

inline const BOOL PEImage::HasCorHeader()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasCorHeader();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasCorHeader();
    }
}

inline void PEImage::SetPassiveDomainOnly()
{
    LEAF_CONTRACT;
    m_bPassiveDomainOnly=TRUE;
}

inline BOOL PEImage::PassiveDomainOnly()
{
    LEAF_CONTRACT;
    return m_bPassiveDomainOnly;
}


inline const BOOL PEImage::HasDirectoryEntry(int entry)   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasDirectoryEntry(entry);
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasDirectoryEntry(entry);
    }
}


inline const mdToken PEImage::GetEntryPointToken( BOOL bIgoreNativeEntryPoint)   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
    {
        PTR_PEImageLayout pLayout = GetLoadedLayout();
        if (bIgoreNativeEntryPoint && (pLayout->GetCorHeader()->Flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT)))
            return mdTokenNil;
        return pLayout->GetEntryPointToken();
    }
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        if (bIgoreNativeEntryPoint && (pLayout->GetCorHeader()->Flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT)))
            return mdTokenNil;
        return pLayout->GetEntryPointToken();
    }
}
inline const BOOL PEImage::HasV1Metadata()   
{
    WRAPPER_CONTRACT;
    return GetMDImport()->GetMetadataStreamVersion()==MD_STREAM_VER_1X;
}

inline const BOOL PEImage::IsILOnly()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->IsILOnly();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->IsILOnly();
    }
}


inline const BOOL PEImage::IsDll()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->IsDll();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->IsDll();
    }
}

inline const BOOL PEImage::HasStrongNameSignature()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasStrongNameSignature();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasStrongNameSignature();
    }
}

inline const HRESULT PEImage::VerifyStrongName(DWORD* verifyOutputFlags)  
{
    WRAPPER_CONTRACT;
    _ASSERTE(verifyOutputFlags);
    if (m_bSignatureInfoCached)
    {
        if (SUCCEEDED(m_hrSignatureInfoStatus))
            *verifyOutputFlags=m_dwSignatureInfo;
        return m_hrSignatureInfoStatus;
    }
    BOOL result;

    PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_FLAT,0));
    if(pLayout!=NULL)
    {
       result = StrongNameSignatureVerificationFromImage((BYTE *) pLayout->GetBase(), pLayout->GetSize(), 
                                                          SN_INFLAG_INSTALL|SN_INFLAG_ALL_ACCESS, 
                                                        verifyOutputFlags);
    }
    else
    {
        CONSISTENCY_CHECK(!GetPath().IsEmpty());
        _ASSERTE(IsFileLocked());
        result = StrongNameSignatureVerification(GetPath(),
                                                 SN_INFLAG_INSTALL|SN_INFLAG_ALL_ACCESS|SN_INFLAG_RUNTIME,
                                                 verifyOutputFlags);
    }

    HRESULT hr=result?S_OK: StrongNameErrorInfo();


    if (SUCCEEDED(hr) || !Exception::IsTransient(hr))
    {
        m_hrSignatureInfoStatus=hr;
        m_dwSignatureInfo=*verifyOutputFlags;
        m_bSignatureInfoCached=TRUE;
    }
    return hr;
}    

inline BOOL PEImage::IsStrongNameSigned()   
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->IsStrongNameSigned();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->IsStrongNameSigned();
    }
}

inline void PEImage::GetIdentitySignature(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        PRECONDITION(HasContents());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Note this is essentially duplicated code with PEFile::GetIdentitySignature.  However,
    // that routine has the difference of caching any computed hash value on the PEFile. 

    if (HasStrongNameSignature())
        GetEffectiveStrongNameSignature(result);
    else
        ComputeHash(CALG_SHA1, result);
}


inline  void  PEImage::InitForSearch(UINT64 uStreamAsmId,DWORD dwModuleId)
{
    LEAF_CONTRACT;
    m_bInitedForSearch=TRUE;
    m_StreamAsmId=uStreamAsmId;
    m_dwStreamModuleId=dwModuleId;
    m_fIsIStream=TRUE;
}

inline  void  PEImage::InitForSearch(const SString& path)
{
    LEAF_CONTRACT;
    m_bInitedForSearch=TRUE;
    m_path=path;
}

#ifndef DACCESS_COMPILE
/* static */
inline BOOL PEImage::CompareID(PEImage *pPE1, PEImage *pPE2)
{
    LEAF_CONTRACT;

    if (pPE1->m_StreamAsmId != pPE2->m_StreamAsmId)
        return FALSE;

    if (pPE1->m_dwStreamModuleId != pPE2->m_dwStreamModuleId)
        return FALSE;

    return TRUE;
}

/* static */
inline PTR_PEImage PEImage::FindById(UINT64 uStreamAsmId, DWORD dwModuleId)
{
    PEImage sImage;
    sImage.InitForSearch(uStreamAsmId,dwModuleId);
    CrstPreempHolder holder(&s_hashLock);
    PEImage* found = (PEImage *) s_Images->LookupValue(HashStreamIds(uStreamAsmId, dwModuleId), &sImage);
    if (found == (PEImage*) INVALIDENTRY)
        return NULL;
    found->AddRef();
    return PTR_PEImage(PTR_HOST_TO_TADDR(found));
}
#endif

inline void PEImage::GetImageBits(DWORD layout, SBuffer &result)
{
    WRAPPER_CONTRACT;
    PEImageLayoutHolder pLayout(GetLayout(layout,LAYOUT_CREATEIFNEEDED));
    BYTE* buffer=result.OpenRawBuffer(pLayout->GetSize());
    PREFIX_ASSUME(buffer != NULL);
    memcpyNoGCRefs(buffer,pLayout->GetBase(),pLayout->GetSize());
    result.CloseRawBuffer(pLayout->GetSize());
}







inline const void *PEImage::GetMetadata(COUNT_T *pSize) 
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->GetMetadata(pSize);
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED|LAYOUT_PERSISTENT));
        return pLayout->GetMetadata(pSize);
    }
}



inline BOOL PEImage::HasNativeHeader() 
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasNativeHeader();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasNativeHeader();
    }
}

inline BOOL PEImage::HasContents() 
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->HasContents();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->HasContents();
    }
}


inline CHECK PEImage::CheckFormat()
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        CHECK(GetLoadedLayout()->CheckFormat());
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        CHECK(pLayout->CheckFormat());
    }
    CHECK_OK;
}
inline const void *PEImage::GetStrongNameSignature(COUNT_T *pSize) 
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->GetStrongNameSignature(pSize);
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED|LAYOUT_PERSISTENT));
        return pLayout->GetStrongNameSignature(pSize);
    }
}

inline void  PEImage::Init(const SString& path)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    m_path=path;
    m_path.Normalize();
}
#ifndef DACCESS_COMPILE
/*static*/
inline PTR_PEImage PEImage::FindByLongPath(const SString& path)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(s_hashLock.OwnedByCurrentThread());
    }
    CONTRACTL_END;
    InlineSString<MAX_PATH> sLongPath;
    // Note: GetLongPathName  return the number of characters written NOT INCLUDING the
    //       null character on success, and on failure returns the buffer size required
    //       INCLUDING the null. This means the result must not be equal to MAX_PATH -
    //       it must be greater or less then.
    SIZE_T nLen = WszGetLongPathName(path, sLongPath.OpenUnicodeBuffer(MAX_PATH-1), MAX_PATH);
    CONSISTENCY_CHECK(nLen != MAX_PATH);

    // If this was insufficient buffer, then try again with a reallocated buffer
    if (nLen > MAX_PATH)
    {
        // Close the buffer before reopening
        sLongPath.CloseBuffer();
        INDEBUG(SIZE_T nOldLen = nLen;)
        nLen = WszGetLongPathName(path, sLongPath.OpenUnicodeBuffer(nLen-1), nLen);
        CONSISTENCY_CHECK(nLen == (nOldLen - 1));
    }
    sLongPath.CloseBuffer(nLen);

    // Check for any kind of error other than an insufficient buffer result.
    if (nLen == 0)
    {
        HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
        if(Exception::IsTransient(hr))
            ThrowHR(hr);
        return (PEImage*)INVALIDENTRY;
    }
    return FindByPath(sLongPath);
}

/*static*/
inline PTR_PEImage PEImage::FindByShortPath(const SString& path)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(s_hashLock.OwnedByCurrentThread());
    }
    CONTRACTL_END;
    InlineSString<MAX_PATH> sShortPath;
    // Note: GetLongPathName  return the number of characters written NOT INCLUDING the
    //       null character on success, and on failure returns the buffer size required
    //       INCLUDING the null. This means the result must not be equal to MAX_PATH -
    //       it must be greater or less then.
    SIZE_T nLen = WszGetShortPathName(path, sShortPath.OpenUnicodeBuffer(MAX_PATH-1), MAX_PATH);
    CONSISTENCY_CHECK(nLen != MAX_PATH);

    // If this was insufficient buffer, then try again with a reallocated buffer
    if (nLen > MAX_PATH)
    {
        // Close the buffer before reopening
        sShortPath.CloseBuffer();
        INDEBUG(SIZE_T nOldLen = nLen;)
        nLen = WszGetShortPathName(path, sShortPath.OpenUnicodeBuffer(nLen-1), nLen);
        CONSISTENCY_CHECK(nLen == (nOldLen - 1));
    }
    sShortPath.CloseBuffer(nLen);

    // Check for any kind of error other than an insufficient buffer result.
    if (nLen == 0)
    {
        HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
        if(Exception::IsTransient(hr))
            ThrowHR(hr);
        return (PEImage*)INVALIDENTRY;
    }
    return FindByPath(sShortPath);
}

/*static*/
inline PTR_PEImage PEImage::FindByPath(const SString& path)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(s_hashLock.OwnedByCurrentThread());
    }
    CONTRACTL_END;
    PEImage sImage;
    sImage.InitForSearch(path);
#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
    DWORD dwHash=path.Hash();
#else
    DWORD dwHash=path.HashCaseInsensitive(PEImage::GetFileSystemLocale());
#endif
   return (PEImage *) s_Images->LookupValue(dwHash, &sImage);
    
}
/* static */
inline PTR_PEImage PEImage::OpenImage(const SString &path, MDInternalImportFlags flags /* = MDInternalImport_Default */)
{

    BOOL fUseCache = !((flags & MDInternalImport_NoCache) == MDInternalImport_NoCache);


    if (!fUseCache)
    {
        PEImageHolder pImage(new PEImage);
        pImage->Init(path);
        return PTR_PEImage(PTR_HOST_TO_TADDR(pImage.Extract()));
    }

    CrstPreempHolder holder(&s_hashLock);
    
    PEImage* found = FindByPath(path);
    if(found == (PEImage*) INVALIDENTRY && (flags & MDInternalImport_CheckLongPath))
         found=FindByLongPath(path);
    if(found == (PEImage*) INVALIDENTRY && (flags & MDInternalImport_CheckShortPath))
         found=FindByShortPath(path);

    if (found == (PEImage*) INVALIDENTRY)
    {
        PEImageHolder pImage(new PEImage);
        pImage->Init(path);
        pImage->AddToHashMap();
        return PTR_PEImage(PTR_HOST_TO_TADDR(pImage.Extract()));
    }
    found->AddRef();
    return PTR_PEImage(PTR_HOST_TO_TADDR(found));
}
#endif

inline BOOL PEImage::IsFileLocked()
{
    WRAPPER_CONTRACT;
    return (m_pLayouts[IMAGE_FLAT])!=NULL || (m_pLayouts[IMAGE_MAPPED])!=NULL ;
}

#ifndef DACCESS_COMPILE
/* static */
inline PTR_PEImage PEImage::OpenImage(IStream *pIStream, UINT64 uStreamAsmId,
                                      DWORD dwModuleId, BOOL resourceFile, MDInternalImportFlags flags /* = MDInternalImport_Default */)  
{
    BOOL fUseCache = !((flags & MDInternalImport_NoCache) == MDInternalImport_NoCache);

    if (!fUseCache)
    {
        PEImageHolder pImage(new PEImage());
        pImage->Init(pIStream, uStreamAsmId, dwModuleId, resourceFile);
        return PTR_PEImage(PTR_HOST_TO_TADDR(pImage.Extract()));
    }

    
    DWORD hash = HashStreamIds(uStreamAsmId, dwModuleId);
    PEImage sImage;
    sImage.InitForSearch(uStreamAsmId,dwModuleId);
    CrstPreempHolder holder(&s_hashLock);
    PEImage* found = (PEImage *) s_Images->LookupValue(hash, &sImage);
    if (found != (PEImage*) INVALIDENTRY)
    {
        found->AddRef();
        return PTR_PEImage(PTR_HOST_TO_TADDR(found));
    }
    PEImageHolder pImage(new PEImage());
    pImage->Init(pIStream, uStreamAsmId, dwModuleId, resourceFile);
    pImage->AddToHashMap();
    return PTR_PEImage(PTR_HOST_TO_TADDR(pImage.Extract()));
}

inline void PEImage::AddToHashMap()
{
    WRAPPER_CONTRACT;
    _ASSERTE(s_hashLock.OwnedByCurrentThread());
#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
    DWORD dwHash=m_path.IsEmpty()?HashStreamIds(m_StreamAsmId, m_dwStreamModuleId):m_path.Hash();
#else
    DWORD dwHash=m_path.IsEmpty()?HashStreamIds(m_StreamAsmId, m_dwStreamModuleId):m_path.HashCaseInsensitive(PEImage::GetFileSystemLocale());
#endif
    s_Images->InsertValue(dwHash,this);
    m_bInHashMap=TRUE;
}


#endif




inline void* PEImage::GetMetadataProfileData()
{
    return NULL;
}

inline BOOL PEImage::Has32BitNTHeaders()
{
    WRAPPER_CONTRACT;
    if (HasLoadedLayout())
        return GetLoadedLayout()->Has32BitNTHeaders();
    else
    {
        PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
        return pLayout->Has32BitNTHeaders();
    }
}

inline BOOL PEImage::HasID()
{
    LEAF_CONTRACT;
    return m_fIsIStream||!GetPath().IsEmpty();
}

inline ULONG PEImage::GetIDHash()
{
    CONTRACT(ULONG)
    {
        PRECONDITION(HasID());
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        POSTCONDITION(RETVAL!=0);
    }
    CONTRACT_END;
    
    if (m_fIsIStream)
        RETURN HashStreamIds(m_StreamAsmId, m_dwStreamModuleId);
    else
    {
#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
        RETURN m_path.Hash();
#else
        RETURN m_path.HashCaseInsensitive(PEImage::GetFileSystemLocale());
#endif
    }

}

inline void  PEImage::GetPEKindAndMachine()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    if(m_fUnknownKindAndMachine)
    {
        if (HasLoadedLayout())
        {
            GetLoadedLayout()->GetPEKindAndMachine(&m_dwPEKind,&m_dwMachine);
            m_fUnknownKindAndMachine = FALSE;
        }
        else
        {
            PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_MAPPED|PEImageLayout::LAYOUT_FLAT, 
                                                    PEImage::LAYOUT_CREATEIFNEEDED));
            if (pLayout!=NULL)
            {
                pLayout->GetPEKindAndMachine(&m_dwPEKind,&m_dwMachine);
                m_fUnknownKindAndMachine = FALSE;
            }
        }
    }
}

inline DWORD PEImage::GetPEKind() { LEAF_CONTRACT; return m_dwPEKind; }
inline DWORD PEImage::GetMachine() { LEAF_CONTRACT; return m_dwMachine; }


#endif  // PEIMAGE_INL_
