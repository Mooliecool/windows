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
#include "fusionp.h"
#include "asmstrm.h"
#include "asmimprt.h"
#include "disk.h"
#include "naming.h"
#include "policy.h"
#include "helpers.h"
#include <winver.h>
#include "fusionheap.h"
#include "lock.h"

// Cache the provider so we can avoid repetive calls to CryptAcquireContext
// and CryptReleaseContext. Calling these crypto APIs repetively (~10K times)
// will result in failure and "out of memory" messages on Win9x systems.
// The real problem is CryptAcquireContext/CryptReleaseContext load and
// unload the CSP DLLs, and repetive loading/unloading of rsabase.dll and
// rsaenh.dll expose this problem. We will work-around this by caching the
// context, but cannot clean it up at DLL_PROCESS_DETACH time since this
// will result in a FreeLibrary (which you can't do at that time).

HCRYPTPROV g_hProv = 0;

CAssemblyStream::CAssemblyStream (CAssemblyCacheItem* pParent)
{
    // We keep a refcount on the containing assembly cache item to
    // ensure that any error results in a rollback after all file
    // handles have been closed.
    _dwSig = 0x534d5341; /* 'SMSA' */
    _cRef = 1;
    _hf = INVALID_HANDLE_VALUE;
    *_szPath = TEXT('\0');
    _dwFormat = 0;
    _pParent = pParent;
    _pParent->AddRef();
    _hHash = NULL;

}

HRESULT CAssemblyStream::Init (LPCOLESTR pszPath, DWORD dwFormat)
{
    HRESULT                            hr = S_OK;
    DWORD                              cwPath;
    BOOL                               bRet;

    _ASSERTE(pszPath);

    _dwFormat = dwFormat;
    cwPath = lstrlenW(pszPath) + 1;

    _ASSERTE(cwPath < MAX_PATH);
    memcpy(_szPath, pszPath, sizeof(TCHAR) * cwPath);

    _hf = WszCreateFile(pszPath, GENERIC_WRITE, 0 /* no sharing */,
                     NULL, CREATE_NEW, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (_hf == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        ReleaseParent(hr);
        goto Exit;
    }

    if (!g_hProv) {
        HCRYPTPROV hProv;
  
        bRet = CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
        if (!bRet) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            ReleaseParent(hr);
            goto Exit;

        }

        if (InterlockedCompareExchangePointer((void **)&g_hProv, (void *)hProv, 0)) {
            // Lost the race. Release our provider.
            CryptReleaseContext(hProv, 0);
        }
    }

    bRet = CryptCreateHash(g_hProv, CALG_SHA1, 0, 0, &_hHash);
    if (!bRet) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        ReleaseParent(hr);
        goto Exit;
    }

Exit:
    return hr;
}


STDMETHODIMP CAssemblyStream::Write(VOID const *pv, ULONG cb,
            ULONG *pcbWritten)
{
    BOOL fRet = WriteFile (_hf, pv, cb, pcbWritten, NULL);
    if (fRet)
    {
        CryptHashData(_hHash, (PBYTE) pv, *pcbWritten, 0);
        return S_OK;
    }
    else
    {   // Report the error.
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        ReleaseParent (hr);
        return hr;
    }
}


STDMETHODIMP CAssemblyStream::Commit(DWORD dwCommitFlags)
{
    BOOL fRet;
    HRESULT hr;
    IAssemblyName *pName = NULL;
    IAssemblyName *pNameCopy = NULL;
    IAssemblyManifestImport *pImport = NULL;


    if(FAILED(hr = AddSizeToItem()))
        goto exit;

    fRet = CloseHandle (_hf);
    _hf = INVALID_HANDLE_VALUE;
    hr = fRet? S_OK : HRESULT_FROM_WIN32(GetLastError());

    if (FAILED(hr))
        goto exit;

    // If this file contains a manifest extract the
    // name and set it on the parent cache item.
    switch (_dwFormat)
    {
        case STREAM_FORMAT_COMPLIB_MANIFEST:
        {
            // If a manifest interface has not already been
            // set on this item, construct one from path.
            if (!(pImport = _pParent->GetManifestInterface()))
            {
                if (FAILED(hr = CreateAssemblyManifestImport(_szPath, &pImport)))
                    goto exit;

                if(FAILED(hr = _pParent->SetManifestInterface( pImport )))
                    goto exit;
            }

            // Get the read-only name def.
            if (FAILED(hr = pImport->GetAssemblyNameDef(&pName)))
                goto exit;

            // Make a writeable copy of the name def.
            if (FAILED(hr = pName->Clone(&pNameCopy)))
                goto exit;

            // Cache this on the parent cache item.
            if (FAILED(hr = _pParent->SetNameDef(pNameCopy)))
                goto exit;

        }
        break;

        case STREAM_FORMAT_COMPLIB_MODULE:
        {
            if( FAILED(hr = CheckHash()) )
                goto exit;
        }
        break;


    } // end switch

exit:

    SAFERELEASE(pImport);
    SAFERELEASE(pName);
    SAFERELEASE(pNameCopy);

    CryptDestroyHash(_hHash);
    _hHash = 0;

    ReleaseParent (hr);
    return hr;
}

void CAssemblyStream::ReleaseParent (HRESULT hr)
{
    if (_hf != INVALID_HANDLE_VALUE)
    {
        CloseHandle (_hf);
        _hf = INVALID_HANDLE_VALUE;
    }
    if (_pParent) {
        _pParent->StreamDone (hr);
        _pParent->Release();
        _pParent = NULL;
    }
}

CAssemblyStream::~CAssemblyStream ( )
{
    if (_pParent)
        ReleaseParent (STG_E_ABNORMALAPIEXIT);
    _ASSERTE (_hf == INVALID_HANDLE_VALUE);
}

HRESULT CAssemblyStream::CheckHash( )
{
    HRESULT hr = S_OK;
    CModuleHashNode  *pModuleHashNode;
    pModuleHashNode = NEW(CModuleHashNode);
    IAssemblyManifestImport *pManifestImport = NULL;

    if (!pModuleHashNode)
    {
        return E_OUTOFMEMORY;

    }

    BYTE    pbHash[MAX_HASH_LEN];
    DWORD   cbHash=MAX_HASH_LEN;


    if (CryptGetHashParam(_hHash, HP_HASHVAL, pbHash, &cbHash, 0))
    {
        pModuleHashNode->Init(_szPath, CALG_SHA1, cbHash, pbHash );
    }
    else
    {
        pModuleHashNode->Init(_szPath, 0, 0, 0);
    }

    hr = _pParent->AddToStreamHashList(pModuleHashNode);

    SAFERELEASE(pManifestImport);
    return hr;
}

HRESULT CAssemblyStream::AddSizeToItem( )
{

    HRESULT hr=S_OK;
    DWORD dwFileSizeLow = 0, dwFileSizeHigh = 0;

    hr = GetFileSizeRoundedToCluster(_hf, &dwFileSizeLow, &dwFileSizeHigh);
    if(SUCCEEDED(hr))
    {
        _pParent->AddStreamSize(dwFileSizeLow, dwFileSizeHigh);
    }

    return hr ;
}

//
// IStream methods not implemented...
//

STDMETHODIMP CAssemblyStream::Read(VOID *pv, ULONG cb, ULONG *pcbRead)
{
    return E_NOTIMPL;
}


STDMETHODIMP CAssemblyStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
            ULARGE_INTEGER *plibNewPosition)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::SetSize (ULARGE_INTEGER libNewSize)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::CopyTo(IStream *pStm, ULARGE_INTEGER cb,
            ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
    return E_NOTIMPL;
}


STDMETHODIMP CAssemblyStream::Revert()
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::LockRegion(ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::UnlockRegion(ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::Stat(STATSTG *pStatStg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyStream::Clone(IStream **ppStm)
{
    return E_NOTIMPL;
}

//
// IUnknown boilerplate...
//

STDMETHODIMP CAssemblyStream::QueryInterface
    (REFIID riid, LPVOID * ppvObj)
{
    if (!ppvObj) 
        return E_POINTER;
    
    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IStream)
       )
    {
        *ppvObj = static_cast<IStream*> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) CAssemblyStream::AddRef(void)
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CAssemblyStream::Release(void)
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

