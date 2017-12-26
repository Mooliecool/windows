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
// PEImage.cpp
// --------------------------------------------------------------------------------

#include "common.h"

#include "peimage.h"
#include "eeconfig.h"
#include "apithreadstress.h"
#include <objbase.h>

#include "eventtrace.h"
#include "peimagelayout.inl"

#ifndef DACCESS_COMPILE


CrstStatic  PEImage::s_hashLock;
PtrHashMap *PEImage::s_Images = NULL;

extern LCID g_lcid; // fusion path comparison lcid

/* static */
void PEImage::Startup()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        POSTCONDITION(CheckStartup());
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEImage");

    if (CheckStartup())
        RETURN;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());

    s_hashLock.Init("PEImage", CrstPEImage, CRST_REENTRANCY);
    LockOwner lock = { &s_hashLock, IsOwnerOfCrst };
    s_Images         = ::new PtrHashMap;
    s_Images->Init(CompareImage, FALSE, &lock);
    PEImageLayout::Startup();
    END_SO_INTOLERANT_CODE;

    RETURN;
}

/* static */
CHECK PEImage::CheckStartup()
{
    WRAPPER_CONTRACT;
    CHECK(s_Images != NULL);
    CHECK_OK;
}

/* static */
CHECK PEImage::CheckLayoutFormat(PEDecoder *pe)
{
    CONTRACT_CHECK
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_CHECK_END;

    // If we are in a compilation domain, we will allow
    // non-IL only files to be treated as IL only


    if (GetAppDomain == NULL
        || GetAppDomain() == NULL
        || (!GetAppDomain()->IsCompilationDomain()))
    {
        CHECK(pe->IsILOnly());
    }

    CHECK(!pe->HasNativeHeader());
    CHECK_OK;
}

/* static */
ULONG PEImage::HashStreamIds(UINT64 id1, DWORD id2)
{
    LEAF_CONTRACT;

    ULONG hash = 5381;

    hash ^= id2;
    hash = _rotl(hash, 4);

    void *data = &id1;
    hash ^= *(INT32 *) data;

    hash = _rotl(hash, 4);
    ((INT32 *&)data)++;
    hash ^= *(INT32 *) data;

    //there are hash values we do not allow
    if (hash<DELETED)
        hash|=0x100;

    return hash;
}

PEImage::~PEImage()
{
    CONTRACTL
    {
        PRECONDITION(CheckStartup());
        PRECONDITION(m_refCount == 0|| (m_bInitedForSearch && m_refCount == 1));
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_pLayoutLock)
        delete m_pLayoutLock;
    if(m_hFile!=INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);
    if (m_bInitedForSearch)
        return;
    else
    {
        GCX_PREEMP();

        for (unsigned int i=0;i<COUNTOF(m_pLayouts);i++)
        {
            if (m_pLayouts[i]!=NULL)
                m_pLayouts[i]->Release();
        }

        if (m_pPdbStream)
            m_pPdbStream->Release();

        if (m_pMDImport)
            m_pMDImport->Release();
        if(m_pNativeMDImport)
            m_pNativeMDImport->Release();
    }
}

/* static */
BOOL PEImage::CompareImage(UPTR path, UPTR mapping)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer((PEImage *)(path<<1)));
        PRECONDITION(CheckPointer((PEImage *)mapping));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL ret = FALSE;

    HRESULT hr;
    EX_TRY
    {
        ret = ((PEImage *)(path<<1))->Equals(((PEImage *)mapping));
    }
    EX_CATCH_HRESULT(hr);

    return ret;
}




    // Thread stress



ULONG PEImage::Release()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    _ASSERTE(!m_bInitedForSearch);
    CONTRACT_VIOLATION(FaultViolation|ThrowsViolation);
    COUNT_T result = 0;
    {
        // Use scoping to hold the hash lock
        CrstPreempHolder holder(&s_hashLock);

        // Decrement and check the refcount - if we hit 0, remove it from the hash and delete it.
        result=FastInterlockDecrement(&m_refCount);
        if (result == 0 )
        {

            LOG((LF_LOADER, LL_INFO100, "PEImage: Closing Image %S\n", (LPCWSTR) m_path));
            if(m_bInHashMap)
            {
#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
                DWORD dwHash=m_path.IsEmpty()?HashStreamIds(m_StreamAsmId, m_dwStreamModuleId):m_path.Hash();
#else
                DWORD dwHash=m_path.IsEmpty()?HashStreamIds(m_StreamAsmId, m_dwStreamModuleId):m_path.HashCaseInsensitive(PEImage::GetFileSystemLocale());
#endif

                s_Images->DeleteValue(dwHash, this);
            }

        }
    }
    // This needs to be done outside of the hash lock, since this can call FreeLibrary,
    // which can cause _CorDllMain to be executed, which can cause the hash lock to be
    // taken again because we need to release the IJW fixup data in another PEImage hash.
    if (result == 0)
            delete this;

    return result;
}

/* static */
CHECK PEImage::CheckCanonicalFullPath(const SString &path)
{
    CONTRACT_CHECK
    {
        PRECONDITION(CheckValue(path));
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    CCHECK_START
    {
        // This is not intended to be an exhaustive test, just to provide a sanity check

        SString::CIterator i = path.Begin();

        if (path.Skip(i, SL(L"\\\\")))
        {
            // Network path
        }
        else if (iswalpha(*i))
        {
            // Drive path
            i++;
            CCHECK(path.Skip(i, SL(":\\")));
        }
        else
        {
            CCHECK_FAIL("Not a full path");
        }

        while (i != path.End())
        {
            // Check for multiple slashes
            CCHECK(*i != '\\');

            // Check for . or ..
            if ((path.Skip(i, SString(SString::Ascii, ".."))
                 || path.Skip(i, SString(SString::Ascii, ".")))
                && (path.Match(i, '\\')))
            {
                CCHECK_FAIL("Illegal . or ..");
            }

            if (!path.Find(i, '\\'))
                break;

            i++;
        }
    }
    CCHECK_END;

    CHECK_OK;
}

extern LCID g_lcid; // fusion path comparison lcid

/* static */
LCID PEImage::GetFileSystemLocale()
{
    LEAF_CONTRACT;

    return g_lcid;
}

BOOL PEImage::PathEquals(const SString &p1, const SString &p2)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
    return p1.Equals(p2);
#else
    return p1.EqualsCaseInsensitive(p2, g_lcid);
#endif
}


BOOL PEImage::Equals(PEImage *pImage)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pImage));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (pImage == this)
        return TRUE;


    if (GetPath().IsEmpty())
    {
        if (m_fIsIStream)
            return CompareID(this, pImage);
        else
            return FALSE;
    }
    else
    {
        BOOL ret = FALSE;
        HRESULT hr;
        EX_TRY
        {

#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
            if (GetPath().Equals(pImage->GetPath()))
#else
            if (GetPath().EqualsCaseInsensitive(pImage->GetPath(), GetFileSystemLocale()))
#endif
                ret = TRUE;
        }
        EX_CATCH_HRESULT(hr);
        return ret;
    }

    // <FUTURE>@future: it would be nice to allow equality for other types of non-disk files, e.g. based</FUTURE>
    // on MVID or strong name signature
}




void PEImage::ComputeHash(ALG_ID algorithm, SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckStartup());
        PRECONDITION(CheckValue(result));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    PEImageLayoutHolder pview(GetLayout(PEImageLayout::LAYOUT_FLAT,PEImage::LAYOUT_CREATEIFNEEDED));


    HandleCSPHolder hProv;
    HandleHashHolder hHash;
    DWORD size = 0;
    DWORD sizeCount = sizeof(UINT32);

    if (!WszCryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)
        || !CryptCreateHash(hProv, algorithm, 0, 0, &hHash)
        || !CryptHashData(hHash, (const BYTE *) pview->GetBase(), pview->GetSize(), 0)
        || !CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *) &size, &sizeCount, 0))
        ThrowLastError();

    BYTE *buffer = result.OpenRawBuffer(size);

    if (!CryptGetHashParam(hHash, HP_HASHVAL, buffer, &size, 0))
        ThrowLastError();

    result.CloseRawBuffer(size);
}

CHECK PEImage::CheckHash(ALG_ID algorithm, const void *pbHash, COUNT_T cbHash)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckStartup());
        INSTANCE_CHECK;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    StackSBuffer hash;
    ComputeHash(algorithm, hash);

    CHECK(hash.Equals((const BYTE *) pbHash, cbHash));

    CHECK_OK;
}

IMDInternalImport* PEImage::GetMDImport()
{
    WRAPPER_CONTRACT;
    if (!m_pMDImport)
        OpenMDImport();
    return m_pMDImport;
}

IMDInternalImport* PEImage::GetNativeMDImport()
{
    WRAPPER_CONTRACT;
    if (!m_pNativeMDImport)
        OpenNativeMDImport();
    return m_pNativeMDImport;
}

void PEImage::OpenNativeMDImport()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNativeHeader());
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (m_pNativeMDImport==NULL)
    {
        IMDInternalImport* m_pNewImport;
        COUNT_T cMeta=0;
        const void* pMeta=GetMetadata(&cMeta);

        if(pMeta==NULL)
            return;

        IfFailThrow(GetMetaDataInternalInterface((void *) pMeta,
                                                 cMeta,
                                                 ofRead,
                                                 IID_IMDInternalImport,
                                                 (void **) &m_pNewImport));

        if(FastInterlockCompareExchangePointer((void**)&m_pNativeMDImport,m_pNewImport,NULL))
            m_pNewImport->Release();
    }
    _ASSERTE(m_pNativeMDImport);
}

void PEImage::OpenMDImport()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        PRECONDITION(HasContents());
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (m_pMDImport==NULL)
    {
        IMDInternalImport* m_pNewImport;
        const void* pMeta=NULL;
        COUNT_T cMeta=0;
        if(HasNTHeaders() && HasCorHeader())
            pMeta=GetMetadata(&cMeta);

        if(pMeta==NULL)
            return;


        IfFailThrow(GetMetaDataInternalInterface((void *) pMeta,
                                                 cMeta,
                                                 ofRead,
                                                 IID_IMDInternalImport,
                                                 (void **) &m_pNewImport));

        if(FastInterlockCompareExchangePointer((void**)&m_pMDImport,m_pNewImport,NULL))
        {
            m_pNewImport->Release();
        } 
        else
        {
            // grab the module name. This information is only used for dac. But we need to get
            // it when module is instantiated in the managed process. The module name is stored
            // in Metadata's module table in UTF8. Convert it to unicode.
            //
            if (m_path.IsEmpty())
            {
                // No need to check error here since this info is only used by DAC when inspecting
                // dump file.
                //
                LPCSTR strModuleName;
                m_pMDImport->GetScopeProps(&strModuleName, NULL);
                m_sModuleFileNameHintUsedByDac.SetUTF8(strModuleName);
                m_sModuleFileNameHintUsedByDac.Normalize();
             }
         }
    }
    _ASSERTE(m_pMDImport);

}

void PEImage::GetMVID(GUID *pMvid)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pMvid));
        PRECONDITION(HasCorHeader());
        PRECONDITION(HasContents());
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (HasNativeHeader())
        GetNativeMDImport()->GetScopeProps(NULL, pMvid);
    else
        GetMDImport()->GetScopeProps(NULL, pMvid);

#ifdef _DEBUG
    COUNT_T cMeta;
    const void *pMeta = GetMetadata(&cMeta);
    GUID MvidDEBUG;

    if (pMeta == NULL)
        ThrowHR(COR_E_BADIMAGEFORMAT);

    SafeComHolder<IMDInternalImport> pMDImport;

    IfFailThrow(GetMetaDataInternalInterface((void *) pMeta,
                                             cMeta,
                                             ofRead,
                                             IID_IMDInternalImport,
                                             (void **) &pMDImport));

    pMDImport->GetScopeProps(NULL, &MvidDEBUG);

    _ASSERTE(memcmp(pMvid, &MvidDEBUG, sizeof(GUID)) == 0);

#endif // _DEBUG
}

void PEImage::GetEffectiveStrongNameSignature(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        PRECONDITION(HasStrongNameSignature());
        PRECONDITION(HasContents());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (IsStrongNameSigned())
    {
        GetHashedStrongNameSignature(result);
    }
    else
    {
        // The image is delay signed. In this case, we can use a relatively weak notion
        // of identity, since the strong name is already spoofable.  So, we use the MVID.

        result.SetSize(max(sizeof(GUID), 20));
        result.Zero();

        // Need to declare a GUID on the stack so that it is aligned
        // for non-x86 architectures

        GUID mvid;
        GetMVID(&mvid);
        result.Copy(result.Begin(), &mvid, sizeof(mvid));
    }
}



void PEImage::GetHashedStrongNameSignature(SBuffer &result)
{
    COUNT_T size;
    const void *sig = GetStrongNameSignature(&size);

    HandleCSPHolder hProv;
    HandleHashHolder hHash;
    DWORD hashSize = 0;
    DWORD sizeCount = sizeof(UINT32);

    if (!WszCryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)
        || !CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)
        || !CryptHashData(hHash, (const BYTE *) sig, size, 0)
        || !CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *) &hashSize, &sizeCount, 0))
        ThrowLastError();

    BYTE *buffer = result.OpenRawBuffer(hashSize);

    if (!CryptGetHashParam(hHash, HP_HASHVAL, buffer, &hashSize, 0))
        ThrowLastError();

    result.CloseRawBuffer(hashSize);
}

void DECLSPEC_NORETURN PEImage::ThrowFormat()
{
    WRAPPER_CONTRACT;
    EEFileLoadException::Throw(m_path, COR_E_BADIMAGEFORMAT);
}

// --------------------------------------------------------------------------------
// Exports for the metadata APIs.  Eventually, PEImage should be moved to utilcode
// so the metadata APIs can call them directly.
// --------------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE RuntimeOpenImage(LPCWSTR pszFileName, HCORMODULE* hHandle)
{
    WRAPPER_CONTRACT;
    return RuntimeOpenImageInternal(pszFileName, hHandle, NULL, MDInternalImport_Default);
}

HRESULT STDMETHODCALLTYPE RuntimeOpenImageInternal(LPCWSTR pszFileName, HCORMODULE* hHandle, DWORD *pdwLength, MDInternalImportFlags flags)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_SO_TOLERANT;
    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    ETWTraceStartup trace(ETW_TYPE_STARTUP_LOADERCATCHALL);
    EX_TRY
    {
        PEImage::Startup();
        PEImageHolder pFile(PEImage::OpenImage(pszFileName, flags));
        if (pdwLength)
        {
            PEImageLayoutHolder pLayout(pFile->GetLayout(PEImageLayout::LAYOUT_MAPPED,PEImage::LAYOUT_CREATEIFNEEDED));
            pFile->GetPEKindAndMachine();
            *pdwLength = pLayout->GetSize();
        }
        *hHandle = (HCORMODULE)pFile.Extract();        
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeOpenImageByStream(IStream* pIStream, UINT64 AssemblyId,
                                                   DWORD dwModuleId,
                                                   HCORMODULE* hHandle, DWORD *pdwLength, MDInternalImportFlags flags)
{
    STATIC_CONTRACT_NOTHROW;
    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {
        PEImage::Startup();

        PEImageHolder pFile(PEImage::OpenImage(pIStream, AssemblyId, dwModuleId, FALSE, flags));
        *hHandle = (HCORMODULE) pFile.Extract();
        if (pdwLength)
        {
            PEImageLayoutHolder pImage(pFile->GetLayout(PEImageLayout::LAYOUT_ANY,0));
            *pdwLength = pImage->GetSize();
            pFile.Extract()->GetPEKindAndMachine();
        }
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeReleaseHandle(HCORMODULE hHandle)
{
    STATIC_CONTRACT_NOTHROW;
    HRESULT hr = S_OK;

    PEImage *pImage = (PEImage*)hHandle;

    if (pImage != NULL)
        pImage->Release();

    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeGetMDInternalImport(HCORMODULE hHandle, MDInternalImportFlags flags, IMDInternalImport** ppMDImport)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    PEImage* pImage=(PEImage*)hHandle;
    HRESULT hr=S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {
        if (!pImage->HasNTHeaders() || !pImage->HasCorHeader())
            hr=HRESULT_FROM_WIN32(ERROR_FILE_INVALID);
        else
        {
            {
                if (!pImage->CheckILFormat())
                    hr=COR_E_BADIMAGEFORMAT;
                else
                {
                    *ppMDImport=pImage->GetMDImport();
                    if (*ppMDImport)
                        (*ppMDImport)->AddRef();
                    else
                        hr=COR_E_BADIMAGEFORMAT;
                }
            }
        }
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeGetImageBase(HCORMODULE hHandle,LPVOID* base, BOOL bMapped, COUNT_T* dwSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    HRESULT hr=S_FALSE;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {
        PEImage* pImage=(PEImage*)hHandle;
        *base=NULL;
        if (!pImage->HasLoadedLayout())
        {
            PEImageLayoutHolder pLayout(pImage->GetLayout(bMapped
                                                            ?PEImageLayout::LAYOUT_MAPPED
                                                            :PEImageLayout::LAYOUT_FLAT,0));
            if (pLayout!=NULL)
            {
                if(dwSize)
                    *dwSize=pLayout->GetSize();
                *base=pLayout->GetBase();
                hr=S_OK;
            }
        }

        if (hr==S_FALSE && pImage->HasLoadedLayout())
        {
            BOOL bIsMapped=pImage->GetLoadedLayout()->IsMapped();
            if ((bIsMapped && bMapped) || (!bIsMapped && !bMapped))
            {
                //the one we want
                *base=pImage->GetLoadedLayout()->GetBase();
                if (dwSize)
                    *dwSize=pImage->GetLoadedLayout()->GetSize();
                hr=S_OK;
            }
        }
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeGetImageKind(HCORMODULE hHandle,DWORD* pdwKind, DWORD* pdwMachine)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    HRESULT hr=S_FALSE;

    PEImage* pImage=(PEImage*)hHandle;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {
        pImage->GetPEKindAndMachine();
        if(pdwKind)
            *pdwKind = pImage->GetPEKind();
        if(pdwMachine)
            *pdwMachine = pImage->GetMachine();
        hr = S_OK;
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT STDMETHODCALLTYPE RuntimeOSHandle(HCORMODULE hHandle, HMODULE* hModule)
{
    LEAF_CONTRACT;
    if(hHandle==NULL || hModule == NULL)
        return E_POINTER;
    PEImage* pImage= (PEImage*) hHandle;
    if (!pImage->HasLoadedLayout())
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    *hModule=(HMODULE)pImage->GetLoadedLayout()->GetBase();
    return S_OK;
}

HRESULT RuntimeGetAssemblyStrongNameHashForModule(HCORMODULE   hModule,
                                                  IMetaDataImport * pMDImport,
                                                  BYTE        *pbSNHash,
                                                  DWORD       *pcbSNHash)
{
    STATIC_CONTRACT_NOTHROW;
    HRESULT hr = S_OK;

    PEImage* pImage = (PEImage*)hModule;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {

        if (pImage->HasStrongNameSignature())
        {
            if (pImage->IsStrongNameSigned())
            {
                SBuffer signature;
                pImage->GetHashedStrongNameSignature(signature);
                *pcbSNHash = min(signature.GetSize(), *pcbSNHash);
                signature.Copy(pbSNHash, signature.Begin(), *pcbSNHash);
            }
            else
            {
                // This assembly is delay signed (in this limited scenario).
                // We'll use the assembly MVID as the hash and leave assembly verification
                // up to the loader to determine if delay signed assemblies are allowed.
                // This allows us to fix the perf degrade observed with the hashing code and
                // detailed in BUG 126760.

                if (max(sizeof(GUID), 20) <= *pcbSNHash)
                {
                    memset(pbSNHash, 0, *pcbSNHash);
                    hr = pMDImport->GetScopeProps(NULL, 0, NULL, (GUID *) pbSNHash);
                }
                else
                    hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

                *pcbSNHash = max(sizeof(GUID), 20);
            }
        }
        else
        {
            hr = CORSEC_E_MISSING_STRONGNAME;
        }
    }
    EX_CATCH_HRESULT(hr);
    END_SO_INTOLERANT_CODE;

    return hr;
}



void PEImage::Init(IStream* pIStream, UINT64 uAsmStreamId,
                   DWORD dwModuleId, BOOL resourceFile)
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckStartup());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    m_StreamAsmId = uAsmStreamId;
    m_dwStreamModuleId = dwModuleId;
    m_fIsIStream = TRUE;

    GCX_PREEMP();
    LOG((LF_LOADER, LL_INFO100, "PEImage: Opening flat stream\n"));

    if (!pIStream)
        ThrowHR(COR_E_FILELOAD);

    // Just copy bytes.

    PEImageLayoutHolder pFlatLayout(PEImageLayout::CreateFromStream(pIStream, this));

    if (!resourceFile) {
        if (!pFlatLayout->CheckCORFormat())
            ThrowFormat();

        if (!CheckLayoutFormat(pFlatLayout))
            ThrowHR(COR_E_NOTSUPPORTED);
    }

    pFlatLayout.SuppressRelease();
    SetLayout(IMAGE_FLAT, pFlatLayout);
    pFlatLayout->MakePersistent();
    RETURN;
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void PEImage::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    DAC_ENUM_DTHIS();

    EMEM_OUT(("MEM: %p PEImage\n", PTR_HOST_TO_TADDR(this)));

    // no lock here as the processs should be suspended
    if (m_pLayouts[IMAGE_FLAT].IsValid() && m_pLayouts[IMAGE_FLAT]!=NULL)
        m_pLayouts[IMAGE_FLAT]->EnumMemoryRegions(flags);
    if (m_pLayouts[IMAGE_MAPPED].IsValid() &&  m_pLayouts[IMAGE_MAPPED]!=NULL)
        m_pLayouts[IMAGE_MAPPED]->EnumMemoryRegions(flags);
    if (m_pLayouts[IMAGE_LOADED].IsValid() &&  m_pLayouts[IMAGE_LOADED]!=NULL)
        m_pLayouts[IMAGE_LOADED]->EnumMemoryRegions(flags);
    if (m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION].IsValid() &&  m_pLayouts[IMAGE_LOADED]!=NULL)
        m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]->EnumMemoryRegions(flags);

    PAL_TRY
    {
        // Fusion loaded IL might have ill formed
        // Debug directory. How can this happen??
        // 
        if (HasLoadedLayout() && HasNTHeaders() && HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_DEBUG))
        {
            IMAGE_DATA_DIRECTORY *pidd = GetLoadedLayout()->GetDirectoryEntry(IMAGE_DIRECTORY_ENTRY_DEBUG);
            TADDR taddrBase = (TADDR)GetLoadedLayout()->GetBase();
            UINT iIndex;

            // number of IMAGE_DEBUG_DIRECTORY
            UINT cDebugDir = pidd->Size / sizeof(IMAGE_DEBUG_DIRECTORY);

            // now report the context where the directory points to.
            // pidd->VirtualAddress need to add the base address of ngen image
            //
            DacEnumMemoryRegion(taddrBase + pidd->VirtualAddress, pidd->Size);

            for (iIndex = 0; iIndex < cDebugDir; iIndex++)
            {
                // Get to each debug directory's RawData and write it out.
                IMAGE_DEBUG_DIRECTORY *pDebugDir = PTR_IMAGE_DEBUG_DIRECTORY(taddrBase + pidd->VirtualAddress + iIndex * sizeof(IMAGE_DEBUG_DIRECTORY));
                DacEnumMemoryRegion((TADDR)GetLoadedLayout()->GetBase() + pDebugDir->AddressOfRawData, pDebugDir->SizeOfData);
            }

        }
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    PAL_ENDTRY
    m_path.EnumMemoryRegions();
    m_sModuleFileNameHintUsedByDac.EnumMemoryRegions();
}

#endif // #ifdef DACCESS_COMPILE


PEImage::PEImage():
    m_PdbStreamLock("PEImagePDBStream", CrstPEImage, CRST_DEBUGGER_THREAD),
    m_pPdbStream(NULL),
    m_refCount(1),
    m_bInitedForSearch(FALSE),
    m_bIsTrustedNativeImage(FALSE),
    m_bPassiveDomainOnly(FALSE),
    m_bInHashMap(FALSE),
#ifdef METADATATRACKER_DATA
    m_pMDTracker(NULL),
#endif // METADATATRACKER_DATA
    m_pMDImport(NULL),
    m_pNativeMDImport(NULL),
    m_StreamAsmId(0),
    m_dwStreamModuleId(0),
    m_fIsIStream(FALSE),
    m_hFile(INVALID_HANDLE_VALUE),
    m_bSignatureInfoCached(FALSE),
    m_hrSignatureInfoStatus(E_UNEXPECTED),
    m_dwSignatureInfo(0),
    m_dwPEKind(0),
    m_dwMachine(0),
    m_fUnknownKindAndMachine(TRUE)
{
    WRAPPER_CONTRACT;
    for (DWORD i=0;i<COUNTOF(m_pLayouts);i++)
        m_pLayouts[i]=NULL ;
    m_pLayoutLock=new SimpleRWLock(PREEMPTIVE,LOCK_TYPE_DEFAULT);
}

PTR_PEImageLayout PEImage::GetLayout(DWORD imageLayoutMask,DWORD flags)
{
    WRAPPER_CONTRACT;
    GCX_PREEMP();
    PTR_PEImageLayout pRetVal;
    {
        SimpleReadLockHolder lock(m_pLayoutLock);
        pRetVal=GetLayoutInternal(imageLayoutMask,flags&(~LAYOUT_CREATEIFNEEDED));
        if (pRetVal || (flags&LAYOUT_CREATEIFNEEDED)==0)
            return pRetVal;
    }
    SimpleWriteLockHolder lock(m_pLayoutLock);
    return GetLayoutInternal(imageLayoutMask,flags);
}

#ifndef DACCESS_COMPILE
/* static */
PTR_PEImage PEImage::LoadFlat(const void *flat, COUNT_T size)
{
    CONTRACT(PTR_PEImage)
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
    }
    CONTRACT_END;
    GCX_PREEMP();
    PEImageHolder pImage(new PEImage());
    PTR_PEImageLayout pLayout = PEImageLayout::CreateFlat(flat,size,pImage);
    _ASSERTE(!pLayout->IsMapped());
    pImage->SetLayout(IMAGE_FLAT,pLayout);
    RETURN PTR_PEImage(PTR_HOST_TO_TADDR(pImage.Extract()));
}


void PEImage::Load()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
    }
    CONTRACTL_END;
    LoadFromMapped();
}

void PEImage::SetLoadedHMODULE(HMODULE hMod)
{
    WRAPPER_CONTRACT;
    SimpleWriteLockHolder lock(m_pLayoutLock);
    if(m_pLayouts[IMAGE_LOADED])
    {
        _ASSERTE(m_pLayouts[IMAGE_LOADED]->GetBase()==hMod);
        return;
    }
    SetLayout(IMAGE_LOADED,PEImageLayout::CreateFromHMODULE(hMod,this,TRUE));
}

void PEImage::LoadFromMapped()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
    }
    CONTRACTL_END;
    if (HasLoadedLayout())
    {
        _ASSERTE(GetLoadedLayout()->IsMapped());
        return;
    }
    GCX_PREEMP();
    PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_MAPPED,LAYOUT_CREATEIFNEEDED));
    SimpleWriteLockHolder lock(m_pLayoutLock);
    if(m_pLayouts[IMAGE_LOADED]==NULL)
        SetLayout(IMAGE_LOADED,pLayout.Extract());
}

void PEImage::LoadForIntrospection()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
    }
    CONTRACTL_END;
    if (HasLoadedIntrospectionLayout())
        return;
    GCX_PREEMP();
    PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_CREATEIFNEEDED));
    SimpleWriteLockHolder lock(m_pLayoutLock);
    if(m_pLayouts[IMAGE_LOADED_FOR_INTROSPECTION]==NULL)
        SetLayout(IMAGE_LOADED_FOR_INTROSPECTION,pLayout.Extract());
}

void PEImage::LoadNoFile()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(!IsFile());
    }
    CONTRACTL_END;
    if (HasLoadedLayout())
        return;

    GCX_PREEMP();
    PEImageLayoutHolder pLayout(GetLayout(PEImageLayout::LAYOUT_ANY,LAYOUT_PERSISTENT));
    if (!pLayout->CheckILOnly())
        ThrowHR(COR_E_BADIMAGEFORMAT);
    SimpleWriteLockHolder lock(m_pLayoutLock);
    if(m_pLayouts[IMAGE_LOADED]==NULL)
        SetLayout(IMAGE_LOADED,pLayout.Extract());
}


void PEImage::LoadNoMetaData(BOOL bIntrospection)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
    }
    CONTRACTL_END;

     if (bIntrospection)
     {
        if (HasLoadedIntrospectionLayout())
            return;
     }
     else
         if (HasLoadedLayout())
            return;


    GCX_PREEMP();
    SimpleWriteLockHolder lock(m_pLayoutLock);
    int layoutKind=bIntrospection?IMAGE_LOADED_FOR_INTROSPECTION:IMAGE_LOADED;
    if (m_pLayouts[layoutKind]!=NULL)
        return;
    if (m_pLayouts[IMAGE_FLAT]!=NULL)
    {
        m_pLayouts[IMAGE_FLAT]->AddRef();
        SetLayout(layoutKind,m_pLayouts[IMAGE_FLAT]);
    }
    else
    {
        _ASSERTE(!m_path.IsEmpty());
        SetLayout(layoutKind,PEImageLayout::LoadFlat(GetFileHandle(),this));
    }
}


#endif //DACCESS_COMPILE

//-------------------------------------------------------------------------------
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//-------------------------------------------------------------------------------
void PEImage::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    result = m_path;
}

// The following enum is copied from inc\Fusion.idl
/*
typedef enum
{
    peNone      = 0x00000000,
    peMSIL      = 0x00000001,
    peI386      = 0x00000002,
    peIA64      = 0x00000003,
    peAMD64     = 0x00000004,
    peInvalid   = 0xffffffff
} PEKIND;
*/

DWORD PEImage::GetFusionProcessorArchitecture()
{
    GetPEKindAndMachine();
    if(!m_fUnknownKindAndMachine)
    {
        DWORD retval;
        if(SUCCEEDED(TranslatePEToArchitectureTypeUtil((CorPEKind)m_dwPEKind,
                                                       m_dwMachine,
                                                       (PEKIND*)&retval)))
        {
            return retval;
        }
    }
    return (DWORD)peNone;
}


HANDLE PEImage::GetFileHandle()
{
    CONTRACTL
    {
        MODE_PREEMPTIVE;
        GC_NOTRIGGER;
        THROWS;
        PRECONDITION(m_pLayoutLock->IsWriterLock());
    }
    CONTRACTL_END;
    if (m_hFile!=INVALID_HANDLE_VALUE)
        return m_hFile;
    {
        ErrorModeHolder mode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
        m_hFile=WszCreateFile((LPCWSTR) m_path,
                                            GENERIC_READ,
                                            RunningOnWin95()?FILE_SHARE_READ:FILE_SHARE_READ|FILE_SHARE_DELETE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL);
    }
    if (m_hFile == INVALID_HANDLE_VALUE)
        ThrowLastError();
    return m_hFile;
}


HANDLE PEImage::GetProtectingFileHandle(BOOL bProtectIfNotOpenedYet)
{
    CONTRACTL
    {
        MODE_PREEMPTIVE;
        GC_NOTRIGGER;
        THROWS;
    }
    CONTRACTL_END;
    if (m_hFile==INVALID_HANDLE_VALUE && !bProtectIfNotOpenedYet)
        return INVALID_HANDLE_VALUE;

    HANDLE hRet=INVALID_HANDLE_VALUE;
    {
        ErrorModeHolder mode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
        hRet=WszCreateFile((LPCWSTR) m_path,
                                            GENERIC_READ,
                                            FILE_SHARE_READ,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL);
    }
    if (m_hFile == INVALID_HANDLE_VALUE)
        ThrowLastError();
    if (m_hFile!=INVALID_HANDLE_VALUE && !CompareFiles(m_hFile,hRet))
        ThrowHR(FUSION_E_REF_DEF_MISMATCH);

    return hRet;
}

