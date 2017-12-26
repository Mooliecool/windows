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
#include "scavenger.h"
#include "asmstrm.h"
#include "fusionheap.h"
#include "cache.h"
#include "naming.h"
#include "util.h"
#include "transprt.h"
#include "cacheutils.h"
#include "enum.h"
#include "list.h"
#include "lock.h"

// global crit-sec for init dbs (reuse, defined at dllmain.cpp)
extern CRITSEC_COOKIE g_csInitClb;

DWORD g_ScavengingThreadId;
HMODULE g_hFusionMod;

#define REG_VAL_FUSION_DOWNLOAD_CACHE_QUOTA_IN_KB           TEXT("DownloadCacheQuotaInKB")
DWORD g_DownloadCacheQuotaInKB;

#define REG_VAL_FUSION_DOWNLOAD_CACHE_USAGE     TEXT("DownloadCacheSize3")
#define TEMP_PEND_DIR           10          // Number of chars for Temp dir names


HRESULT ScavengeDownloadCache();


class CScavengerNode
{
public:

    CScavengerNode();
    ~CScavengerNode();
    static LONG Compare(CScavengerNode *, CScavengerNode *);
    LPWSTR _pwzManifestPath;
    FILETIME _ftLastAccess;
    FILETIME _ftCreation;
    DWORD _dwAsmSize;
};

CScavengerNode::CScavengerNode()
{
    _pwzManifestPath=NULL;
    _dwAsmSize = 0;
    memset( &_ftLastAccess, 0, sizeof(FILETIME));
    memset( &_ftCreation,   0, sizeof(FILETIME));

}

CScavengerNode::~CScavengerNode()
{
    SAFEDELETEARRAY(_pwzManifestPath);
}

LONG CScavengerNode::Compare(CScavengerNode *pItem1, CScavengerNode *pItem2)
{
    return CompareFileTime( &(pItem1->_ftLastAccess), &(pItem2->_ftLastAccess));
}

//------------------- Cache Scavenging APIs ------------- --------------------



HRESULT CreateScavenger(IUnknown **ppAsmScavenger)
{
    HRESULT                       hr = S_OK;
    IAssemblyScavenger           *pScavenger = NULL;

    if (!ppAsmScavenger) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pScavenger = NEW(CScavenger);
    if (!pScavenger) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    *ppAsmScavenger = pScavenger;
    (*ppAsmScavenger)->AddRef();

Exit:
    SAFERELEASE(pScavenger);

    return hr;
}

CScavenger::CScavenger()
{
    _cRef = 1;
}

CScavenger::~CScavenger()
{

}

HRESULT GetDownloadUsage(DWORD *pdwDownloadUsageInKB)
{
    HRESULT                         hr=S_OK;
    DWORD                           dwDownloadUsage=0;
    WCHAR szBuf[20];
    LPWSTR endPtr;

    if (!PAL_FetchConfigurationString(FALSE, REG_VAL_FUSION_DOWNLOAD_CACHE_USAGE, szBuf, ARRAYSIZE(szBuf)))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }    

    dwDownloadUsage = wcstoul(szBuf, &endPtr, 10);

    if (endPtr == szBuf)
    {
        hr = E_FAIL;
        goto exit;
    }

    if (pdwDownloadUsageInKB)
        *pdwDownloadUsageInKB = dwDownloadUsage;

exit:
    return hr;
}

HRESULT SetDownLoadUsage(   /* [in] */ BOOL  bUpdate,
                            /* [in] */ int   dwDownloadUsage)
{
    HRESULT                         hr=S_OK;
    DWORD                           dwCurrDownloadCacheSize=0;
    WCHAR szBuf[20];

    if(bUpdate)
    {
        hr = GetDownloadUsage(&dwCurrDownloadCacheSize);
        dwDownloadUsage += dwCurrDownloadCacheSize;
    }

    if(dwDownloadUsage < 0)
        dwDownloadUsage = 0;

    StringCchPrintf(szBuf, ARRAYSIZE(szBuf), L"%d", dwDownloadUsage);

    if (!PAL_SetConfigurationString(FALSE, REG_VAL_FUSION_DOWNLOAD_CACHE_USAGE, szBuf))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

DWORD GetDownloadTarget()
{
    HRESULT hr;
    DWORD dwCurrUsage = 0;

    hr = GetDownloadUsage(&dwCurrUsage);

    if(dwCurrUsage > g_DownloadCacheQuotaInKB)
        return g_DownloadCacheQuotaInKB/2 + 1;
    else
        return 0;

}

HRESULT GetScavengerQuotasFromReg(DWORD *pdwZapQuotaInGAC,
                                  DWORD *pdwDownloadQuotaAdmin,
                                  DWORD *pdwDownloadQuotaUser)
{
    HRESULT                         hr=S_OK;
    DWORD                           dwDownloadLMQuota=0;
    DWORD                           dwDownloadCUQuota=0;
    WCHAR szBuf[20];
    LPWSTR endPtr;

    if (PAL_FetchConfigurationString(TRUE, REG_VAL_FUSION_DOWNLOAD_CACHE_QUOTA_IN_KB, szBuf, ARRAYSIZE(szBuf)))
    {
        dwDownloadLMQuota = wcstoul(szBuf, &endPtr, 10);
    }    
    
    if (PAL_FetchConfigurationString(FALSE, REG_VAL_FUSION_DOWNLOAD_CACHE_QUOTA_IN_KB, szBuf, ARRAYSIZE(szBuf)))
    {
        dwDownloadCUQuota = wcstoul(szBuf, &endPtr, 10);
    }

    if(!dwDownloadLMQuota)
    {
        g_DownloadCacheQuotaInKB = 50000; // default Download Cache Quota
    }
    else
    {
        g_DownloadCacheQuotaInKB = dwDownloadLMQuota;
    }

    if(dwDownloadCUQuota)
    {
        g_DownloadCacheQuotaInKB = min(dwDownloadCUQuota, g_DownloadCacheQuotaInKB);
    }

    if(pdwZapQuotaInGAC)
         *pdwZapQuotaInGAC = 0;

    if(pdwDownloadQuotaAdmin)
         *pdwDownloadQuotaAdmin = g_DownloadCacheQuotaInKB;

    if(pdwDownloadQuotaUser)
         *pdwDownloadQuotaUser = g_DownloadCacheQuotaInKB;

    return hr;
}

HRESULT  CScavenger::GetCurrentCacheUsage( /* [in] */ DWORD *pdwZapUsage,
                                           /* [in] */ DWORD *pdwDownloadUsage)
{
    if(pdwZapUsage)
        *pdwZapUsage = 0;

    return GetDownloadUsage(pdwDownloadUsage);
}

HRESULT CScavenger::GetCacheDiskQuotas( /* [out] */ DWORD *pdwZapQuotaInGAC,
                                                /* [out] */ DWORD *pdwDownloadQuotaAdmin,
                                                /* [out] */ DWORD *pdwDownloadQuotaUser)
{
    return GetScavengerQuotasFromReg(pdwZapQuotaInGAC, pdwDownloadQuotaAdmin, pdwDownloadQuotaUser);
}

HRESULT CScavenger::SetCacheDiskQuotas(
                            /* [in] */ DWORD dwZapQuotaInGAC,
                            /* [in] */ DWORD dwDownloadQuotaAdmin,
                            /* [in] */ DWORD dwDownloadQuotaUser)
{
    HRESULT             hr=S_OK;
    DWORD               dwDownloadQuota = dwDownloadQuotaAdmin;
    BOOL                fAdminAccess = TRUE;
    WCHAR                           szBuf[20];
    BOOL                            bPerMachine = FALSE;

    if(!fAdminAccess)
    {
        bPerMachine = FALSE;
        dwDownloadQuota = dwDownloadQuotaUser;
    }
    else
    {
        bPerMachine = TRUE;
        dwDownloadQuota = dwDownloadQuotaAdmin;
    }

    StringCchPrintf(szBuf, ARRAYSIZE(szBuf), L"%d", dwDownloadQuota);

    if (!PAL_SetConfigurationString(bPerMachine, REG_VAL_FUSION_DOWNLOAD_CACHE_USAGE, szBuf))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    GetScavengerQuotasFromReg(NULL, NULL, NULL);

    return hr;
}

// ---------------------------------------------------------------------------
// CScavenger::ScavengeAssemblyCache
// Flush private cache and if required, scavenge private cache based on LRU.
//---------------------------------------------------------------------------
HRESULT CScavenger::ScavengeAssemblyCache()
{
    return DoScavengingIfRequired( TRUE );
}

HRESULT DeleteAssemblyFiles(DWORD dwCacheFlags, LPCWSTR pszCustomPath, LPCWSTR pszManFilePath)
{
    HRESULT hr = S_OK;
    LPTSTR  pszTemp = NULL;
    LPCWSTR  pszManifestPath = pszManFilePath;
    TCHAR   szPendDelDirPath[MAX_PATH+1];
    TCHAR   szAsmDirPath[MAX_PATH+1];
    WCHAR   wzManifestPath[MAX_PATH+1];

    DWORD dwLen = 0;

    if(!pszManifestPath) {
        hr = E_INVALIDARG;
        goto exit;
    }

    *szPendDelDirPath = L'\0';
    *szAsmDirPath = L'\0';
    *wzManifestPath = L'\0';

    if(lstrlenW(pszManifestPath) >= (int) ARRAYSIZE(szAsmDirPath) - 1) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto exit;
    }

    hr = StringCbCopy(szAsmDirPath, sizeof(szAsmDirPath), pszManifestPath);
    if (FAILED(hr)) {
        goto exit;
    }

    pszTemp = PathFindFileName(szAsmDirPath);
    if(pszTemp > szAsmDirPath) {
        *(pszTemp-1) = L'\0';
    }

    dwLen = ARRAYSIZE(szPendDelDirPath) - 1;
    hr = GetPendingDeletePath(pszCustomPath, dwCacheFlags, szPendDelDirPath, &dwLen);
    if (FAILED(hr)) {
        goto exit;
    }

    if(lstrlenW(szPendDelDirPath) + TEMP_PEND_DIR + 1 >= (int) ARRAYSIZE(szPendDelDirPath) - 1) {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }

    hr = GetRandomFileName(szPendDelDirPath, TEMP_PEND_DIR);
    if(FAILED(hr)) {
        goto exit;
    }

    if(!WszMoveFile(szAsmDirPath, szPendDelDirPath)) {
        hr = HRESULT_FROM_WIN32(GetLastError());

        if( hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ||
            hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND) ) {
            hr = S_OK;
        }
        else {

            // looks like there are some in-use files here.
            // move all the asm files to pend del dir.
            if(!WszCreateDirectory(szPendDelDirPath, NULL)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                if (hr != HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) {
                    goto exit;
                }
                hr = S_OK;
            }

            hr = MoveAllFilesFromDir(szAsmDirPath, szPendDelDirPath);
            if(SUCCEEDED(hr)) {
                // assembly deleted successfully delete/pend all temp files.
                hr = RemoveDirectoryAndChildren(szPendDelDirPath);
                hr = S_OK; // don't worry about passing back error here. its already in pend-del dir.
            }
            else {
                // could not delete assembly; restore all files back to original state.
                HRESULT hrTemp;
                hrTemp = MoveAllFilesFromDir(szPendDelDirPath, szAsmDirPath);

                hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
            }
        }
    }
    else {
        hr = RemoveDirectoryAndChildren(szPendDelDirPath);
        hr = S_OK; // don't worry about passing back error here. its already in pend-del dir.
    }

    // Remove the parent directory as well
    if(SUCCEEDED(hr)) {
        pszTemp = PathFindFileName(szAsmDirPath);

        if(pszTemp > szAsmDirPath) {
            *(pszTemp-1) = L'\0';
        }

        // now that we have two levels of dirs...try to remove parent dir also
        // this succeeds only if it is empty, don't worry about return value.
        WszRemoveDirectory(szAsmDirPath);
    }

exit :

    return hr;
}

// ---------------------------------------------------------------------------
// CScavenger::DeleteAssembly
//         Deletes the given TransCache entry after deleting bits.
// ---------------------------------------------------------------------------
HRESULT
CScavenger::DeleteAssembly( DWORD dwCacheFlags, LPCWSTR pszCustomPath, LPCWSTR pszManFilePath, BOOL bForceDelete)
{
    HRESULT     hr = S_OK;
    LPTSTR      pszTemp = NULL;
    TCHAR       szPendDelDirPath[MAX_PATH+1];
    TCHAR       szAsmDirPath[MAX_PATH+1];
    DWORD       dwLen = 0;
    LPCWSTR      pszManifestPath = pszManFilePath;
    DWORD       dwAttrib = 0;
    BOOL        fExists = FALSE;

    _ASSERTE( pszManFilePath);

    if(bForceDelete)
        return DeleteAssemblyFiles( dwCacheFlags, pszCustomPath, pszManFilePath);

    dwLen = lstrlenW(pszManifestPath);
    _ASSERTE(dwLen <= MAX_PATH);

    hr = CheckFileExistence(pszManifestPath, &fExists, &dwAttrib);
    if(FAILED(hr)) {
        goto exit;
    }
    else if(!fExists) {
        if(bForceDelete) {
            return DeleteAssemblyFiles( dwCacheFlags, pszCustomPath, pszManFilePath);
        }

        hr = S_FALSE;
        goto exit;
    }

    hr = StringCbCopy(szAsmDirPath, sizeof(szAsmDirPath), pszManifestPath);
    if (FAILED(hr)) {
        goto exit;
    }

    if(!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        // looks manifestFilePath is passed in. knock-off the filename.
        pszTemp = PathFindFileName(szAsmDirPath);

        if(pszTemp > szAsmDirPath) {
            *(pszTemp-1) = L'\0';
        }
    }

    dwLen = MAX_PATH;
    hr = GetPendingDeletePath( pszCustomPath, dwCacheFlags, szPendDelDirPath, &dwLen);
    if (FAILED(hr)) {
        goto exit;
    }

    if(lstrlenW(szPendDelDirPath) + TEMP_PEND_DIR + 1 >= MAX_PATH) {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }

    GetRandomFileName( szPendDelDirPath, TEMP_PEND_DIR);


    if(!WszMoveFile( szAsmDirPath, szPendDelDirPath)) {
        hr = HRESULT_FROM_WIN32(GetLastError());

        if( hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ||
            hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
            hr = S_OK;
            goto exit;
        }
    }
    else {
        hr = RemoveDirectoryAndChildren(szPendDelDirPath);
        hr = S_OK; // don't worry about passing back error here. its already in pend-del dir.
        goto exit;
    }

exit :
    if( hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) ||
        hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION) ) {

        // We cannot delete this as someone else has locked it...
        hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
    }

    if(hr == S_OK) {
        pszTemp = PathFindFileName(szAsmDirPath);

        if(pszTemp > szAsmDirPath) {
            *(pszTemp-1) = L'\0';
            WszRemoveDirectory(szAsmDirPath);
        }
    }

    return hr;
}

// ---------------------------------------------------------------------------
// CScavenger::NukeDowloadedCache()
// scavenging interface
//---------------------------------------------------------------------------
HRESULT CScavenger::NukeDownloadedCache()
{
    HRESULT hr=S_OK;
    WCHAR szCachePath[MAX_PATH+1];
    LPWSTR pszCacheLoc = NULL;

        BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(hr = COR_E_STACKOVERFLOW);

    hr = GetCacheLoc(CACHE_DOWNLOAD, &pszCacheLoc);
    if (FAILED(hr)) {
        goto exit;
    }

    if ((lstrlenW(pszCacheLoc) + lstrlenW(FUSION_CACHE_DIR_DOWNLOADED_SZ) + 1) >= MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto exit;
    }

    hr = StringCbCopy(szCachePath, sizeof(szCachePath), pszCacheLoc);
    if (FAILED(hr)) {
        goto exit;
    }

    PathRemoveBackslash(szCachePath);

    hr = StringCbCat(szCachePath, sizeof(szCachePath), FUSION_CACHE_DIR_DOWNLOADED_SZ);
    if (FAILED(hr)) {
        goto exit;
    }

    // remove the complete downloaded dir. tree
    hr = RemoveDirectoryAndChildren(szCachePath);

    if( (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) )  
           || (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) )
    {
        hr = S_OK;
    }

    if(SUCCEEDED(hr))
    {
        SetDownLoadUsage( FALSE, 0);
    }

        END_SO_INTOLERANT_CODE;

exit :
    return hr;
}

//
// IUnknown boilerplate...
//

STDMETHODIMP
CScavenger::QueryInterface(REFIID riid, void** ppvObj)
{
    if (!ppvObj) 
        return E_POINTER;
    
    if (   IsEqualIID(riid, IID_IUnknown)
                || IsEqualIID(riid, IID_IAssemblyScavenger)
       )
    {
        *ppvObj = static_cast<IAssemblyScavenger*> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
CScavenger::AddRef()
{
    return InterlockedIncrement (&_cRef);
}

STDMETHODIMP_(ULONG)
CScavenger::Release()
{
    ULONG lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;
    return lRet;
}

STDAPI ClearDownloadCache()
{
    return CScavenger::NukeDownloadedCache();
}

STDAPI NukeDownloadedCache()
{
    return CScavenger::NukeDownloadedCache();
}

HRESULT DeleteAssemblyBits(LPCTSTR pszManifestPath)
{
    HRESULT hr = S_OK;
    TCHAR szPath[MAX_PATH+1];
    DWORD           dwLen = 0;

    if(!pszManifestPath)
        goto exit;

    dwLen = lstrlenW(pszManifestPath);
    _ASSERTE(dwLen <= MAX_PATH);

    hr = StringCbCopy(szPath, sizeof(szPath), pszManifestPath);
    if (FAILED(hr)) {
        goto exit;
    }

    // making c:\foo\a.dll -> c:\foo for RemoveDirectoryAndChd()
    while( szPath[dwLen] != L'\\' && dwLen > 0 )
        dwLen--;

    if( szPath[dwLen] == L'\\')
        szPath[dwLen] = L'\0';

    //  remove the disk file
    hr = RemoveDirectoryAndChildren(szPath);

    if( (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) )  
            || (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) )
    {
        // file is not there, this not an error.
        hr = S_OK;
        goto exit;
    }

    if(hr == S_OK)
    {
        // making c:\foo\a.dll -> c:\foo for RemoveDirectory();
        while( szPath[dwLen] != L'\\' && dwLen > 0 )
            dwLen--;

        if( szPath[dwLen] == L'\\')
            szPath[dwLen] = L'\0';

        // now that we have two levels of dirs...try to remove parent dir also
        // this succeeds only if it is empty, don't worry about return value.
        WszRemoveDirectory(szPath);
    }

exit :
    return hr;
}

HRESULT StartScavenging(LPVOID pSynchronous)
{
    HRESULT            hr = S_OK;
    CCriticalSection   cs(g_csInitClb);

    hr = ScavengeDownloadCache();

    if(FAILED(hr))
        goto exit;


exit:

    HRESULT hrRet = cs.Lock();
    if (FAILED(hrRet)) {
        return hrRet;
    }

    g_ScavengingThreadId=0;

    cs.Unlock();

    if (!pSynchronous)
        FreeLibraryAndExitThread( g_hFusionMod, hr);

    return S_OK;
}


HRESULT CreateScavengerThread(BOOL bSynchronous)
{
    HRESULT hr=S_OK;
    HRESULT hrCS=S_OK;
    HANDLE hThread=0;
    DWORD dwThreadId=0;
    DWORD Error = 0;
    CCriticalSection cs(g_csInitClb);

    hrCS = cs.Lock();
    if (FAILED(hrCS)) {
        return hrCS;
    }
    
    if(g_ScavengingThreadId)
    {
        /*                                                                                   
               */
        hr = S_FALSE;
        goto exit;
    }

    if( bSynchronous )
    {
        g_ScavengingThreadId = GetCurrentThreadId();
        cs.Unlock();
        hr = StartScavenging( (LPVOID)TRUE );
        
        hrCS = cs.Lock();
        if (FAILED(hrCS)) {
            return hrCS;
        }

        g_ScavengingThreadId = 0;
    }
    else
    {
        g_hFusionMod = WszLoadLibrary(g_FusionDllPath);

        hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) StartScavenging, (LPVOID)FALSE,  0, &dwThreadId);

        if (hThread == NULL)
        {
            Error = GetLastError();
            FreeLibrary(g_hFusionMod);
        }
        else
        {
            g_ScavengingThreadId = dwThreadId;
        }
    }

exit :

    cs.Unlock();

    if(hThread)
        CloseHandle(hThread);

    return hr;
}


HRESULT DoScavengingIfRequired(BOOL bSynchronous)
{
    HRESULT hr = S_OK;
        
    if(FAILED(hr = CreateCacheMutex()))
    {
        goto exit;
    }

    if(GetDownloadTarget())
        hr = CreateScavengerThread(bSynchronous);

exit:

    return hr;
}

HRESULT FlushOldAssembly(LPCWSTR pszCustomPath, LPCWSTR pszAsmDirPath, LPCWSTR pszManifestFileName, BOOL bForceDelete)
{
    HRESULT hr = S_OK;
    LPTSTR pszTemp=NULL, pszAsmDirName=NULL;
    TCHAR szParentDirPath[MAX_PATH+1];
    TCHAR szBuf[MAX_PATH+1];
    HANDLE hf = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fd;

    _ASSERTE(pszAsmDirPath);

    hr = StringCbCopy(szParentDirPath, sizeof(szParentDirPath), pszAsmDirPath);
    if (FAILED(hr)) {
        goto exit;
    }

    pszTemp = PathFindFileName(szParentDirPath);

    _ASSERTE(pszTemp > szParentDirPath);

    *(pszTemp-1) = L'\0';

    pszAsmDirName = pszTemp;

    hr = StringCbCopy(szBuf, sizeof(szBuf), szParentDirPath);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(szBuf, sizeof(szBuf), TEXT("\\*"));
    if (FAILED(hr)) {
        goto exit;
    }

    if ((hf = WszFindFirstFile(szBuf, &fd)) == INVALID_HANDLE_VALUE) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    do
    {
        if ( (FusionCompareStringI(fd.cFileName, TEXT(".")) == 0) || 
             (FusionCompareStringI(fd.cFileName, TEXT("..")) == 0))
            continue;

        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            if(FusionCompareStringI(fd.cFileName, pszAsmDirName))
            {
                hr = StringCchPrintf(szBuf, MAX_PATH, L"%s\\%s\\%s", szParentDirPath, fd.cFileName, pszManifestFileName);
                if (FAILED(hr)) {
                    goto exit;
                }
                
                hr = CScavenger::DeleteAssembly(CACHE_DOWNLOAD, pszCustomPath, szBuf, bForceDelete);

                if(hr != S_OK)
                    goto exit;
            }
        }
        else
        {
        }

    } while (WszFindNextFile(hf, &fd));

    if((hr == S_OK) && (GetLastError() != ERROR_NO_MORE_FILES))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

exit :

    if(hf != INVALID_HANDLE_VALUE)
    {
        FindClose(hf);
        hf = INVALID_HANDLE_VALUE;
    }

    return hr;
}

HRESULT CleanupTempDir(DWORD dwCacheFlags, LPCWSTR pszCustomPath)
{
    HRESULT hr = S_OK;
    TCHAR szPendDelDirPath[MAX_PATH+1];
    TCHAR szBuf[MAX_PATH+1];
    HANDLE hf = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fd;

    DWORD dwLen = MAX_PATH;

    if (dwCacheFlags != CACHE_ZAP) {
        hr = GetPendingDeletePath( pszCustomPath, dwCacheFlags, szPendDelDirPath, &dwLen);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else {
        LPWSTR pwzZapDir = NULL;
        hr = GetZapDir(&pwzZapDir);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCchPrintf(szPendDelDirPath, ARRAYSIZE(szPendDelDirPath), L"%ws\\temp", pwzZapDir);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    hr = StringCbCopy(szBuf, sizeof(szBuf), szPendDelDirPath);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(szBuf, sizeof(szBuf), TEXT("\\*"));
    if (FAILED(hr)) {
        goto exit;
    }

    if((hf = WszFindFirstFile(szBuf, &fd)) == INVALID_HANDLE_VALUE) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    do
    {
        if ( (FusionCompareStringI(fd.cFileName, TEXT(".")) == 0) || 
             (FusionCompareStringI(fd.cFileName, TEXT("..")) == 0))
            continue;

        hr = StringCchPrintf(szBuf, MAX_PATH-1, TEXT("%s\\%s"), szPendDelDirPath, fd.cFileName);
        if (FAILED(hr)) {
            goto exit;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            hr = RemoveDirectoryAndChildren(szBuf);
        }
        else
        {
            if(!WszDeleteFile(szBuf))
                hr = HRESULT_FROM_WIN32(GetLastError());

        }

    } while (WszFindNextFile(hf, &fd));

    if((hr == S_OK) && (GetLastError() != ERROR_NO_MORE_FILES))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

exit :

    if(hf != INVALID_HANDLE_VALUE)
    {
        FindClose(hf);
        hf = INVALID_HANDLE_VALUE;
    }

    return hr;
}

HRESULT GetCurrTime(FILETIME *pftCurrTime, DWORD dwSeconds)
{
    FILETIME ftCurrTime;
    ULARGE_INTEGER uliTime;

    HRESULT hr = S_OK;
    TCHAR szTempFilePath[MAX_PATH+1];
    HANDLE hFile = INVALID_HANDLE_VALUE;
#define TEMP_FILE_LEN (15)

    memset(pftCurrTime, 0, sizeof(ULARGE_INTEGER));

    DWORD dwLen = MAX_PATH;
    hr = GetPendingDeletePath( NULL, CACHE_DOWNLOAD, szTempFilePath, &dwLen);
    if (FAILED(hr)) {
        goto exit;
    }

    dwLen = MAX_PATH - dwLen;

    dwLen = TEMP_FILE_LEN;
    if((dwLen + lstrlenW(szTempFilePath) + 1)>= MAX_PATH)
    {
        hr = E_FAIL;
        goto exit;
    }

    GetRandomFileName(szTempFilePath, dwLen);

    hFile = WszCreateFile(szTempFilePath, GENERIC_WRITE, 0 /* no sharing */,
                     NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    else if(!GetFileTime(hFile, &ftCurrTime, NULL, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

exit:

    if(FAILED(hr))
    {
        GetSystemTimeAsFileTime(&ftCurrTime);
        hr = S_OK;
    }

    memcpy( &uliTime, &ftCurrTime, sizeof(ULARGE_INTEGER));

    uliTime.QuadPart -= dwSeconds * 10000000;  // 1 second = 10 ** 7 units in SystemTime.
    memcpy(pftCurrTime, &uliTime, sizeof(ULARGE_INTEGER));

    if(hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        WszDeleteFile(szTempFilePath);
    }

    return hr;
}

HRESULT EnumFileStore(DWORD dwFlags, DWORD *pdwSizeOfCacheInKB, List<CScavengerNode *> **ppNodeList )
{
    HRESULT         hr = NOERROR;
    CEnumCache*     pEnumR = NULL;
    CTransCache*    pTCQry = NULL;
    CTransCache*    pTCOut= NULL;
    DWORD           dwTotalKBSize = 0;
    DWORD           dwAsmSize=0;
    List<CScavengerNode *>   *pNodeList=NEW(List<CScavengerNode *>);
    CScavengerNode  *pNode;

    _ASSERTE(ppNodeList);

    *ppNodeList = NULL;

    hr = CTransCache::Create(&pTCQry, dwFlags);
    if( hr != S_OK )
        goto exit;

    pEnumR = NEW(CEnumCache(TRUE, NULL));
    if(!pEnumR)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pEnumR->Init(pTCQry, 0);
    if (FAILED(hr))
       goto exit;

    while( NOERROR == hr )
    {
        // create temp object
        hr = CTransCache::Create(&pTCOut, dwFlags);

        if( hr != S_OK)
            break;

        hr = pEnumR->GetNextRecord(pTCOut);

        if( S_OK == hr )
        {
            pNode = NEW(CScavengerNode);
            if(!pNode)
            {
                hr = E_OUTOFMEMORY;
                goto exit;
            }

            hr = GetAssemblyKBSize(pTCOut->_pInfo->pwzPath, &dwAsmSize, 
                        &(pNode->_ftLastAccess), &(pNode->_ftCreation));
            dwTotalKBSize += dwAsmSize;
            // build ascending order list.
            pNode->_dwAsmSize = dwAsmSize;
            pNode->_pwzManifestPath = pTCOut->_pInfo->pwzPath;
            pTCOut->_pInfo->pwzPath = NULL;
            if (!pNodeList->AddSorted(pNode, (LPVOID) CScavengerNode::Compare))
            {
                hr = E_OUTOFMEMORY;
                SAFEDELETE(pNode);
                goto exit;
            }
            // cleanup
            SAFERELEASE(pTCOut);
        } // nextRecord

    } // while

    if(pdwSizeOfCacheInKB)
        *pdwSizeOfCacheInKB = dwTotalKBSize;

    *ppNodeList = pNodeList;

exit:

    if(!(*ppNodeList))
    {
        SAFEDELETE(pNodeList); // this should call RemoveAll();
    }

    SAFEDELETE(pEnumR);
    SAFERELEASE(pTCOut);
    SAFERELEASE(pTCQry);

    return hr;
}

HRESULT ScavengeDownloadCache()
{
    HRESULT hr = S_OK;
    DWORD dwCacheSizeInKB=0;
    DWORD dwFreedInKB=0;
    List<CScavengerNode *>   *pNodeList=NULL;
    LISTNODE    pAsmList=NULL;
    int iAsmCount=0,i=0;
    CScavengerNode  *pTargetAsm;
    DWORD dwScavengeTo = 0;
    DWORD dwCurrentUsage = 0;
    CMutex  cCacheMutex(g_hCacheMutex);

    hr = CleanupTempDir(CACHE_DOWNLOAD, NULL);

    hr = EnumFileStore(CACHE_DOWNLOAD, &dwCacheSizeInKB,  &pNodeList);

    if(FAILED(hr = cCacheMutex.Lock()))
        goto exit;

    if(FAILED(hr = SetDownLoadUsage(FALSE, dwCacheSizeInKB)))
        goto exit;

    if(FAILED(hr = cCacheMutex.Unlock()))
    {
        goto exit;
    }

    if (pNodeList) {
        pAsmList  = pNodeList->GetHeadPosition();
        iAsmCount = pNodeList->GetCount();

        if(!(dwScavengeTo = GetDownloadTarget()))
            goto exit;

        for(i=0; i<iAsmCount; i++)
        {
            if(FAILED(hr = GetDownloadUsage(&dwCurrentUsage)))
                goto exit;

            if(dwCurrentUsage <= dwScavengeTo)
                break;

            pTargetAsm = pNodeList->GetNext(pAsmList); // Element from list;

            if(FAILED(hr = cCacheMutex.Lock()))
                goto exit;

            hr = CScavenger::DeleteAssembly(CACHE_DOWNLOAD, NULL, pTargetAsm->_pwzManifestPath, FALSE);


            if(SUCCEEDED(hr))
            {
                dwFreedInKB += pTargetAsm->_dwAsmSize;
                SetDownLoadUsage(TRUE, - (int)pTargetAsm->_dwAsmSize);
            }

            if(FAILED(hr = cCacheMutex.Unlock()))
            {
                goto exit;
            }

        }
    }

exit:
    // destroy list.
    if(pNodeList)
    {
        pAsmList  = pNodeList->GetHeadPosition();
        iAsmCount = pNodeList->GetCount();

        for(i=0; i<iAsmCount; i++)
        {
            pTargetAsm = pNodeList->GetNext(pAsmList); // Element from list;
            SAFEDELETE(pTargetAsm);
        }
        pNodeList->RemoveAll();
        SAFEDELETE(pNodeList); // this should call RemoveAll
    }

    return hr;
}

extern "C" HRESULT STDMETHODCALLTYPE DeleteShadowCache(LPCWSTR pwzCachePath, LPCWSTR pwzAppName);

//
// HRESULT DeleteShadowCache(LPWSTR pwzCachePath, LPWSTR pwzAppName)
//
// Given CachePath and AppName, Enumerate assemblies and Uninstall
//
// Returns :
//      S_OK - Assemblie(s) uninstalled
//      S_FALSE - Failed to uninstall 1 or more assemblies
//      E_xxx - inidcates error code
//
HRESULT STDMETHODCALLTYPE DeleteShadowCache(LPCWSTR pwzCachePath, LPCWSTR pwzAppName)
{
    HRESULT             hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    IApplicationContext *pAppCtx = NULL;
    IAssemblyName       *pName = NULL;
    CTransCache         *pTransCache = NULL;
    CCache              *pCache = NULL;
    IAssemblyEnum       *pEnum = NULL;
    WCHAR               wzCacheBase[MAX_PATH];
    DWORD               dwSize = 0;
    BOOL                fDeletionFailure = FALSE;

    if(!pwzCachePath || !pwzAppName || !lstrlenW(pwzCachePath) || !lstrlenW(pwzAppName)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Create an AppCtx, set it's Appbase and name
    hr = CreateApplicationContext(NULL, &pAppCtx);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = pAppCtx->Set(ACTAG_APP_CACHE_BASE, (LPVOID) pwzCachePath, (lstrlenW(pwzCachePath)+1) * sizeof(WCHAR), 0);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = pAppCtx->Set(ACTAG_APP_NAME, (LPVOID) pwzAppName, (lstrlenW(pwzAppName)+1) * sizeof(WCHAR), 0);
    if(FAILED(hr)) {
        goto Exit;
    }

    *wzCacheBase = L'\0';
    dwSize = ARRAYSIZE(wzCacheBase);
    hr = pAppCtx->GetAppCacheDirectory(wzCacheBase, &dwSize);
    if(FAILED(hr)) {
        goto Exit;
    }

    // Clean up the temp directory, No error checks needed
    hr = CleanupTempDir(CACHE_DOWNLOAD, wzCacheBase);

    // Open Cache at this location
    hr = CCache::Create(&pCache, pAppCtx);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = CreateAssemblyEnum(&pEnum, pAppCtx, NULL, CACHE_DOWNLOAD, NULL);
    if(FAILED(hr)) {
        goto Exit;
    }

    while(S_FALSE != pEnum->GetNextAssembly(NULL, &pName, 0)) {

        // Get an Asm
        hr = pCache->RetrieveTransCacheEntry(pName, CACHE_DOWNLOAD, &pTransCache);
        if((hr != S_OK)) {
            goto Exit;
        }

        hr = CScavenger::DeleteAssembly(pTransCache->GetCacheType(), pCache->GetCustomPath(), pTransCache->_pInfo->pwzPath, FALSE);
        if(hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)) {
            // Ignore, try to delete all
            // assemblies
            fDeletionFailure = TRUE;
            hr = S_OK;
        }
        else if(FAILED(hr)) {
            goto Exit;
        }

        SAFERELEASE(pName);
        SAFERELEASE(pTransCache);
    }

  Exit:
    SAFERELEASE(pName);
    SAFERELEASE(pTransCache);
    SAFERELEASE(pCache);
    SAFERELEASE(pAppCtx);
    SAFERELEASE(pEnum);

    if(SUCCEEDED(hr) && fDeletionFailure) {
        hr = S_FALSE;
    }

    END_ENTRYPOINT_NOTHROW;

    return hr;
}

