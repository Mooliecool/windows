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
#include "cacheutils.h"
#include "cache.h"
#include "parse.h"
#include "util.h"
#include "disk.h"
#include "naming.h" // for MAX_VERSION_DISPLAY_SIZE
#include "lock.h"
#include "asm.h"
#include "policy.h"
#include "scavenger.h"
#include "dbglog.h"

typedef BOOL (*PFNISWOW64PROCESS)(HANDLE hProcess, BOOL *pWow64Process);
typedef void (*PFNGETNATIVESYSTEMINFO)(LPSYSTEM_INFO lpSystemInfo);
typedef BOOL (*PFNGETVOLUMEINFORMATION) (LPCTSTR lpRootPathName, LPTSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPTSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
typedef BOOL (*PFNCREATEHARDLINK) (LPCTSTR lpFileName, LPCTSTR lpExistingFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
typedef WCHAR (*PFNRTLUPCASEUNICODECHAR)(WCHAR wChar);


extern CRITSEC_COOKIE g_csInitClb;
extern HCRYPTPROV g_hProv;

#define REG_VAL_FUSION_CACHE_LOCATION    TEXT("CacheLocation")
#define FUSION_LOCAL_GAC_FILE            L"fusion.localgac"
WCHAR g_GACRoot[MAX_PATH+1];    // Assembly
WCHAR g_GACDir[MAX_PATH+1];     // GAC
WCHAR g_GACNE[MAX_PATH+1];      // GACNE
WCHAR g_GAC32[MAX_PATH+1];      // GAC32
WCHAR g_GAC64[MAX_PATH+1];      // GAC64
BOOL  g_bUseDefaultStore = FALSE;  

WCHAR g_ZapDir[MAX_PATH+1];

WCHAR g_DownloadDir[MAX_PATH+1];

extern const WCHAR g_szFindAllMask[] = L"\\*";
extern const WCHAR g_szDotDLL[] = L".dll";
extern const WCHAR g_szDotEXE[] = L".exe";

const WCHAR g_szDotCAB[] = L".cab";
const WCHAR g_FusionInfoFile[] = L"__AssemblyInfo__.ini";


#define ASSEMBLY_INFO_STRING L"AssemblyInfo"
#define SIGNATURE_BLOB_KEY_STRING      L"Signature"
#define MVID_KEY_STRING                L"MVID"
#define CUSTOM_BLOB_STRING   L"CustomString"
#define URL_STRING           L"URL"
#define DISPLAY_NAME_STRING  L"DisplayName"
#define CONFIG_MASK_STRING   L"ConfigMask"
#define DEFAULT_INFO_STRING  L"__Default__"

HRESULT FindAndSetObfuscatedDirectory();
HRESULT CreateObfuscatedDirectory();

// Used by CS to control which cache
// paths have been initialized
extern CRITSEC_COOKIE g_csSingleUse;
DWORD   g_dwInializedCachePathFlags = 0;

typedef enum tagSetPathFlags {
    ROOT_DIR_SET            = 0x001,
    GAC_DIR_SET             = 0x002,
    GAC_MSIL_DIR_SET        = 0x004,
    GAC_32_DIR_SET          = 0x008,
    GAC_64_DIR_SET          = 0x010,
    ZAP_DIR_SET             = 0x020,
    DOWNLOAD_DIR_SET        = 0x080,
} SetPathFlags;

BOOL IsCabFile(LPCWSTR pszFileName)
{
    DWORD dwLen = lstrlenW(g_szDotCAB);

    _ASSERTE(pszFileName);

    DWORD dwFileLen = lstrlenW(pszFileName);

    if(dwFileLen < dwLen)
        return FALSE;

    if(FusionCompareStringI(pszFileName+dwFileLen-dwLen, g_szDotCAB))
        return FALSE;
    else return TRUE; // yes file name has .cab extension.

}

HRESULT GetCacheLoc(DWORD dwCacheFlags, __deref_out LPWSTR *pszCacheLoc)
{
    HRESULT hr = S_OK;

    if((dwCacheFlags & CACHE_DOWNLOAD) 
        )
    {
        hr = FusionGetUserFolderPath();
        if(FAILED(hr)){
            goto exit;
        }
                
        *pszCacheLoc = g_UserFolderPath;
    }
    else
    {
        *pszCacheLoc = g_szWindowsDir;
    }

exit :
    return hr;
}

HRESULT SetGACDir(CACHE_FLAGS dwCacheFlags)
{
    CCriticalSection    cs(g_csSingleUse);
    HRESULT             hr = S_OK;
    LPWSTR              pwzDestBuffer = NULL;
    BOOL                fLocked = FALSE;
    DWORD               dwFlagToOr = 0;
    DWORD               dwLen = 0;

    // Validate cache flag
    _ASSERTE( dwCacheFlags & (CACHE_GAC_ANY | CACHE_ROOT));

    // execute entire function under critical section
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    fLocked = TRUE;

    // Is it already set?
    if(dwCacheFlags == CACHE_GAC) {
        if(g_dwInializedCachePathFlags & GAC_DIR_SET) {
            goto Exit;
        }
        dwLen += lstrlenW(FUSION_CACHE_DIR_GAC_SZ);
        pwzDestBuffer = g_GACDir;
        dwFlagToOr = GAC_DIR_SET;
    }
    else if(dwCacheFlags == CACHE_ROOT) {
        if(g_dwInializedCachePathFlags & ROOT_DIR_SET) {
            goto Exit;
        }
        // Size addition below
        pwzDestBuffer = g_GACRoot;
        dwFlagToOr = ROOT_DIR_SET;
    }
    else if(dwCacheFlags == CACHE_GAC_MSIL) {
        if(g_dwInializedCachePathFlags & GAC_MSIL_DIR_SET) {
            goto Exit;
        }
        dwLen += lstrlenW(FUSION_CACHE_DIR_GACNE_SZ);
        pwzDestBuffer = g_GACNE;
        dwFlagToOr = GAC_MSIL_DIR_SET;
    }
    else if(dwCacheFlags == CACHE_GAC_32) {
        if(g_dwInializedCachePathFlags & GAC_32_DIR_SET) {
            goto Exit;
        }
        dwLen += lstrlenW(FUSION_CACHE_DIR_GAC32_SZ);
        pwzDestBuffer = g_GAC32;
        dwFlagToOr = GAC_32_DIR_SET;
    }
    else if(dwCacheFlags == CACHE_GAC_64) {
        if(g_dwInializedCachePathFlags & GAC_64_DIR_SET) {
            goto Exit;
        }
        dwLen += lstrlenW(FUSION_CACHE_DIR_GAC64_SZ);
        pwzDestBuffer = g_GAC64;
        dwFlagToOr = GAC_64_DIR_SET;
    }
    else {
        _ASSERTE(!"Invalid GAC cache flag!");
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwLen += lstrlenW(g_szWindowsDir) + lstrlenW(FUSION_CACHE_DIR_ROOT_SZ) + 1; // Null terminator

    if(dwLen > MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto Exit;
    }

    *pwzDestBuffer = L'\0';

    // Add the windows + root cache dir
    hr = StringCchCopy(pwzDestBuffer, MAX_PATH, g_szWindowsDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    PathRemoveBackslash(pwzDestBuffer);
    hr = StringCchCat(pwzDestBuffer, MAX_PATH, FUSION_CACHE_DIR_ROOT_SZ);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzDestBuffer, MAX_PATH, FUSION_CACHE_DIR_GACNE_SZ);

    if (FAILED(hr)) {
        goto Exit;
    }

    g_dwInializedCachePathFlags |= dwFlagToOr;

Exit:

    if(fLocked) {
        cs.Unlock();
    }

    return hr;
}

HRESULT GetGACDir(CACHE_FLAGS dwCacheFlags, __deref_out LPWSTR *ppszDir)
{
    HRESULT             hr = S_OK;

    _ASSERTE(ppszDir);

    if(dwCacheFlags & CACHE_GAC) {
        if(!(g_dwInializedCachePathFlags & GAC_DIR_SET)) {
            hr = SetGACDir(CACHE_GAC);
            if(FAILED(hr)) {
                goto Exit;
            }
        }
            
        *ppszDir = g_GACDir;
        goto Exit;
    }
    else if(dwCacheFlags & CACHE_ROOT) {
        if(!(g_dwInializedCachePathFlags & ROOT_DIR_SET)) {
            hr = SetGACDir(CACHE_ROOT);
            if(FAILED(hr)) {
                goto Exit;
            }
        }
        
        *ppszDir = g_GACRoot;
        goto Exit;
    }
    else if(dwCacheFlags & CACHE_GAC_MSIL) {
        if(!(g_dwInializedCachePathFlags & GAC_MSIL_DIR_SET)) {
            hr = SetGACDir(CACHE_GAC_MSIL);
            if(FAILED(hr)) {
                goto Exit;
            }
        }

        *ppszDir = g_GACNE;
        goto Exit;
    }
    else if(dwCacheFlags & CACHE_GAC_32) {
        if(!(g_dwInializedCachePathFlags & GAC_32_DIR_SET)) {
            hr = SetGACDir(CACHE_GAC_32);
            if(FAILED(hr)) {
                goto Exit;
            }
        }

        *ppszDir = g_GAC32;
        goto Exit;
    }
    else if(dwCacheFlags & CACHE_GAC_64) {
        if(!(g_dwInializedCachePathFlags & GAC_64_DIR_SET)) {
            hr = SetGACDir(CACHE_GAC_64);
            if(FAILED(hr)) {
                goto Exit;
            }
         }
            
        *ppszDir = g_GAC64;
        goto Exit;
    }
    else {
        _ASSERTE(!"Must specify a GAC flag!");
        hr = E_UNEXPECTED;
        goto Exit;
    }

Exit:

    return hr;
}

HRESULT SetZapDir()
{
    CCriticalSection    cs(g_csSingleUse);
    HRESULT             hr = S_OK;
    DWORD               dwLen = 0;
    BOOL                fLocked = FALSE;

    if(g_dwInializedCachePathFlags & ZAP_DIR_SET) {
        goto Exit;
    }
    // execute entire function under critical section
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    fLocked = TRUE;

    // Already set?
    if(g_dwInializedCachePathFlags & ZAP_DIR_SET) {
        goto Exit;
    }

    hr = GetCORVersion();
    if(FAILED(hr)) {
        goto Exit;
    }

    dwLen = lstrlenW(g_szWindowsDir) + lstrlenW(FUSION_CACHE_DIR_ROOT_SZ) + 
            lstrlenW(FUSION_CACHE_DIR_ZAP_SZ) + lstrlenW(g_pwzCORVersion) + 4; // _32 or _64 + 1 null terminator
    if(dwLen > MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto Exit;
    }

    g_ZapDir[0] = L'\0';

    // Add the windows + root cache dir
    hr = StringCbCopy(g_ZapDir, sizeof(g_ZapDir), g_szWindowsDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    PathRemoveBackslash(g_ZapDir);
    hr = StringCbCat(g_ZapDir, sizeof(g_ZapDir), FUSION_CACHE_DIR_ROOT_SZ);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCbCat(g_ZapDir, sizeof(g_ZapDir), FUSION_CACHE_DIR_ZAP_SZ);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Add the runtime version
    hr = StringCbCat(g_ZapDir, sizeof(g_ZapDir), g_pwzCORVersion);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Add _64 or _32 
    if(IsProcess32()) {
        hr = StringCbCat(g_ZapDir, sizeof(g_ZapDir), L"_32");
    }
    else{
        hr = StringCbCat(g_ZapDir, sizeof(g_ZapDir), L"_64");
    }

    if (FAILED(hr)) {
        goto Exit;
    }

    g_dwInializedCachePathFlags |= ZAP_DIR_SET;

Exit:

    if(fLocked) {
        cs.Unlock();
    }

    return hr;
}

HRESULT GetZapDir(__deref_out LPWSTR *pszZapDir)
{
    HRESULT hr = S_OK;

    _ASSERTE(pszZapDir);

    if(!(g_dwInializedCachePathFlags & ZAP_DIR_SET)) {
        hr = SetZapDir();
        if(FAILED(hr)) {
            goto Exit;
        }
    }

    *pszZapDir = g_ZapDir;

Exit:
    
    return hr;
}

HRESULT SetDownLoadDir()
{
    CCriticalSection    cs(g_csSingleUse);
    HRESULT             hr = S_OK;
    LPWSTR              pszCacheLoc = NULL;
    BOOL                fLocked = FALSE;

    hr = CreateCacheMutex();
    if (FAILED(hr)) {
        return hr;
    }

    hr = GetCacheLoc(CACHE_DOWNLOAD, &pszCacheLoc);
    if(FAILED(hr)) {
        goto exit;
    }
        
    if(!(g_dwInializedCachePathFlags & DOWNLOAD_DIR_SET)) {
        hr = cs.Lock();
        if (FAILED(hr)) {
            return hr;
        }

        fLocked = TRUE;

        // Already set?
        if(g_dwInializedCachePathFlags & DOWNLOAD_DIR_SET) {
            goto exit;
        }
        
        if( (lstrlenW(pszCacheLoc) + lstrlenW(FUSION_CACHE_DIR_DOWNLOADED_SZ)) > MAX_PATH )
        {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        g_DownloadDir[0] = L'\0';
        hr = StringCbCopy(g_DownloadDir, sizeof(g_DownloadDir), pszCacheLoc);
        if (FAILED(hr)) {
            goto exit;
        }
        
        PathRemoveBackslash(g_DownloadDir);
        hr = StringCbCat(g_DownloadDir, sizeof(g_DownloadDir), FUSION_CACHE_DIR_DOWNLOADED_SZ);
        if (FAILED(hr)) {
            goto exit;
        }
    
        hr = PathAddBackslashWrap(g_DownloadDir, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }
        
        hr = FindAndSetObfuscatedDirectory();
        if (FAILED(hr)) {
            goto exit;
        }

        g_dwInializedCachePathFlags |= DOWNLOAD_DIR_SET;
    }

exit:
    if(fLocked) {
        cs.Unlock();
    }

    return hr;
}

HRESULT FindAndSetObfuscatedDirectory()
{
    HRESULT                          hr = S_OK;
    WCHAR                            pwzBuf[MAX_PATH];
    DWORD                            dwSize = MAX_PATH;
    BOOL                             bFound = FALSE;
    CMutex                           cCacheMutex(g_hCacheMutex);

    hr =  cCacheMutex.Lock();
    if(FAILED(hr)) {
        goto Exit;
    }

    pwzBuf[0] = L'\0';
    GetCachedDownloadCacheDir(pwzBuf, &dwSize);

    // there is a cached download cache dir, 
    // make sure it is valid
    if (pwzBuf[0]) {
         if (!FusionCompareStringN(g_DownloadDir, pwzBuf, lstrlenW(g_DownloadDir))) {
            hr = StringCbCopy(g_DownloadDir, sizeof(g_DownloadDir), pwzBuf);
            if (FAILED(hr)) {
                goto Exit;
            }
            bFound = TRUE;
        }       
    }

    if (!bFound) {
        // Create directory, since we can't find it (or it's invalid)
        hr = CreateObfuscatedDirectory();
        if (FAILED(hr)) {
            goto Exit;
        }

        _ASSERTE(g_DownloadDir[0]);

        hr = StoreDownloadCacheDir(g_DownloadDir);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

#define REG_VAL_FUSION_DOWNLOAD_CACHE_DIR   L"DownloadCacheLocation"
HRESULT GetCachedDownloadCacheDir(__out_ecount_opt(*pdwSize) LPWSTR pwzBuf, __inout LPDWORD pdwSize)
{
    HRESULT hr = S_OK;
    if (!PAL_FetchConfigurationString(FALSE, REG_VAL_FUSION_DOWNLOAD_CACHE_DIR, pwzBuf, *pdwSize))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }   

    *pdwSize = lstrlenW(pwzBuf);

exit:
    return hr;
}

HRESULT StoreDownloadCacheDir(LPCWSTR pwzBuf)
{
    HRESULT                         hr = S_OK;
    if (!PAL_SetConfigurationString(FALSE, REG_VAL_FUSION_DOWNLOAD_CACHE_DIR, pwzBuf)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

#define DOWNLOAD_CACHE_OBFUSCATION_LENGTH             15

HRESULT CreateObfuscatedDirectory()
{
    HRESULT     hr = S_OK;
    LPWSTR      pwzRandom = NULL;
    BYTE        bBuffer[DOWNLOAD_CACHE_OBFUSCATION_LENGTH];

        if (!PAL_Random(FALSE, bBuffer, DOWNLOAD_CACHE_OBFUSCATION_LENGTH)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Exit;
        }

    hr = Base32Encode(bBuffer, DOWNLOAD_CACHE_OBFUSCATION_LENGTH, &pwzRandom);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    // Obfuscation directory name is of the 8.3 format:

    if (lstrlenW(g_DownloadDir) + lstrlenW(L"12345678.123\\12345678.123") + 1 >= MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto Exit;
    }

    hr = StringCbCatN(g_DownloadDir, sizeof(g_DownloadDir), pwzRandom, (9-1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCbCat(g_DownloadDir, sizeof(g_DownloadDir), L".");
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCbCatN(g_DownloadDir, sizeof(g_DownloadDir), pwzRandom + 8, (4-1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCbCat(g_DownloadDir, sizeof(g_DownloadDir), L"\\");
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCbCatN(g_DownloadDir, sizeof(g_DownloadDir), pwzRandom + 11, (9-1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCbCat(g_DownloadDir, sizeof(g_DownloadDir), L".");
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCbCatN(g_DownloadDir, sizeof(g_DownloadDir), pwzRandom + 19, (4-1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFEDELETEARRAY(pwzRandom);
    
    return hr;
}

HRESULT GetDownloadDir(__deref_out LPWSTR *pszDownLoadDir)
{
    HRESULT hr = S_OK;

    if((hr = SetDownLoadDir()) != S_OK)
        return hr;

    *pszDownLoadDir = g_DownloadDir;
    return hr;
}

HRESULT GetCacheLocationFromReg(__out_ecount(dwSize) LPWSTR pwzCachePath, DWORD dwSize)
{
    HRESULT     hr = S_OK;
    WCHAR       szBuf[MAX_PATH+1];

    _ASSERTE(pwzCachePath && dwSize);

    *pwzCachePath = L'\0';
    szBuf[0] = L'\0';
    
    if (!PAL_FetchConfigurationString(TRUE, REG_VAL_FUSION_CACHE_LOCATION, szBuf, MAX_PATH))
    {
        hr = S_FALSE;
        goto exit;
    }

    if((DWORD)lstrlenW(szBuf) >= dwSize) {
        hr = S_FALSE;
        goto exit;
    }

    hr = StringCchCopy(pwzCachePath, dwSize, szBuf);
    if (FAILED(hr)) {
        goto exit;
    }

exit:


    return hr;
}

HRESULT SetRootCachePath(LPCWSTR pwzBindingConfigAssemblyStorePath)
{
    HRESULT         hr = S_OK;
    WCHAR           wzLocalGACPath[MAX_PATH + 1];
    WCHAR           wzTempPath[MAX_PATH + 1];
    LPWSTR          pwzFileName = NULL;
    DWORD           dwAttr = 0;
    BOOL            fExists = FALSE;

    wzLocalGACPath[0] = L'\0';
    wzTempPath[0] = L'\0';

    // If we were passed BindingConfiguration override path
    // then use it
    if(pwzBindingConfigAssemblyStorePath && pwzBindingConfigAssemblyStorePath[0]) {
        if(lstrlenW(pwzBindingConfigAssemblyStorePath) + 1 > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            goto Exit;
        }

        hr = StringCbCopy(wzLocalGACPath, sizeof(wzLocalGACPath), pwzBindingConfigAssemblyStorePath);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (!wzLocalGACPath[0]) {
        // Override registry setting if there is an "fusion.localgac" file
        // under the path where fusion.dll is found
        hr = StringCbCopy(wzTempPath, sizeof(wzTempPath), g_FusionDllPath);
        if (FAILED(hr)) {
            goto Exit;
        }

        pwzFileName = PathFindFileName(wzTempPath);
        _ASSERTE(pwzFileName);
        *pwzFileName = L'\0';

        hr = StringCchPrintfW(wzTempPath, ARRAYSIZE(wzTempPath), L"%ws%ws", wzTempPath, FUSION_LOCAL_GAC_FILE);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CheckFileExistence(wzTempPath, &fExists, &dwAttr);
        if(FAILED(hr)) {
            goto Exit;
        }
        else if(fExists) {
            // Remove fusion.localgac from path
            *(pwzFileName-1) = L'\0';
            hr = StringCbCopy(wzLocalGACPath, sizeof(wzLocalGACPath), wzTempPath);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    if (!wzLocalGACPath[0]) {
        hr = GetCacheLocationFromReg(wzLocalGACPath, MAX_PATH);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // If we didn't get a location, then default to windows dir
    if(!wzLocalGACPath[0]) {
        GetRealWindowsDirectory(g_szWindowsDir, ARRAYSIZE(g_szWindowsDir));
        g_bUseDefaultStore = TRUE;  
    }
    else {
        // Initialize the final path
        if(!PathCanonicalize(wzTempPath, wzLocalGACPath)) {
            hr = E_FAIL;
            goto Exit;
        }

        PathRemoveBackslash(wzTempPath);

        hr = StringCbCopy(g_szWindowsDir, sizeof(g_szWindowsDir), wzTempPath);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT GetAssemblyStagingPath(LPCTSTR pszCustomPath, DWORD dwCacheFlags,
                               BOOL bUser, __out_ecount_opt(*pcchSize) LPTSTR pszPath, __inout DWORD *pcchSize)
{
    HRESULT hr = S_OK;
    LPTSTR  pszCacheLoc = NULL;
    DWORD   dwSize = lstrlenW(FUSION_CACHE_STAGING_DIR_SZ) + 10; // 8 chars for dir name, 1 for backslash, 1 for NULL

    _ASSERTE(pcchSize);

    if (pszCustomPath != NULL)
    {
        // Use custom path as the base
        dwSize += lstrlenW(pszCustomPath);
        if(dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }
        
        hr = StringCchCopy(pszPath, *pcchSize, pszCustomPath);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else
    {
        if(FAILED(hr = GetCacheLoc(dwCacheFlags, &pszCacheLoc)))
            goto exit;

        // Else use the default
        dwSize += lstrlenW(pszCacheLoc);
        if(dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }
        
        hr = StringCchCopy(pszPath, *pcchSize, pszCacheLoc);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    PathRemoveBackslash(pszPath);
    hr = StringCchCat(pszPath, *pcchSize, FUSION_CACHE_STAGING_DIR_SZ);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(pszPath, *pcchSize);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = CreateDirectoryForAssembly(8, pszPath, pcchSize);

exit :
    *pcchSize = dwSize;
    return hr;
}

HRESULT GetPendingDeletePath(LPCTSTR pszCustomPath, DWORD dwCacheFlags,
                               __out_ecount_opt(*pcchSize) LPTSTR pszPath, __inout DWORD *pcchSize)
{
    HRESULT hr = S_OK;
    LPTSTR pszCacheLoc = NULL;
    DWORD   dwSize = lstrlenW(FUSION_CACHE_PENDING_DEL_DIR_SZ) + 1;

    if(!pszPath || !pcchSize || !*pcchSize) {
        return E_INVALIDARG;
    }

    if (pszCustomPath != NULL) {
        // Use custom path as the base
        dwSize += lstrlenW(pszCustomPath);
        
        if( dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }
        
        hr = StringCchCopy(pszPath, *pcchSize, pszCustomPath);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else {
        if(FAILED(hr = GetCacheLoc(dwCacheFlags, &pszCacheLoc)))
            goto exit;

        // Else use the default
        dwSize += lstrlenW(pszCacheLoc);
        
        if(dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }
        
        hr = StringCchCopy(pszPath, *pcchSize, pszCacheLoc);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    hr = StringCchCat(pszPath, *pcchSize, FUSION_CACHE_PENDING_DEL_DIR_SZ);
    if (FAILED(hr)) {
        goto exit;
    }

exit:
    *pcchSize = dwSize;
    return hr;
}

HRESULT CreateAssemblyDirPath(
 LPCTSTR pszCustomPath,
 CACHE_FLAGS dwCacheFlags,
 BOOL bUser,
 __out_ecount_opt(*pcchSize) LPTSTR pszPath,
 __inout DWORD *pcchSize)
{
    HRESULT hr = S_OK;
    LPWSTR  pszCacheLoc = NULL;
    DWORD   dwSize = 0;


    if(IsZapType(dwCacheFlags)) {
        hr = GetZapDir(&pszCacheLoc);
        if(hr != S_OK) {
            goto exit;
        }

        dwSize = lstrlenW(pszCacheLoc) + 1;

        if(dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }

        hr = StringCchCopy(pszPath, *pcchSize, pszCacheLoc);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else if(IsGacType(dwCacheFlags) || (dwCacheFlags & CACHE_ROOT)) {
        hr = GetGACDir(dwCacheFlags, &pszCacheLoc);
        if(hr != S_OK) {
            goto exit;
        }

        dwSize = lstrlenW(pszCacheLoc) + 1;

        if(dwSize > *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }

        hr = StringCchCopy(pszPath, *pcchSize, pszCacheLoc);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else if (dwCacheFlags & CACHE_DOWNLOAD) {
        if (pszCustomPath != NULL) {
            dwSize = lstrlenW(pszCustomPath) + lstrlenW(FUSION_CACHE_DIR_DOWNLOADED_SZ) + 1;
            
            // Use custom path as the base
            if(dwSize > *pcchSize) {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                goto exit;
            }
            
            hr = StringCchCopy(pszPath, *pcchSize, pszCustomPath);
            if (FAILED(hr)) {
                goto exit;
            }

            hr = StringCchCat(pszPath, *pcchSize, FUSION_CACHE_DIR_DOWNLOADED_SZ);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else {
            // Else use the default
            hr = GetDownloadDir(&pszCacheLoc);
            if(hr != S_OK) {
                goto exit;
            }

            dwSize = lstrlenW(pszCacheLoc) + 1;
            
            if(dwSize > *pcchSize) {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                goto exit;
            }
            
            hr = StringCchCopy(pszPath, *pcchSize, pszCacheLoc);
            if (FAILED(hr)) {
                goto exit;
            }
        }
    }
    else {
        _ASSERTE(!"Invalid Cache Flag!");
        hr = E_INVALIDARG;
    }

exit:

    *pcchSize = dwSize;
    return hr;
}

HRESULT GetCachePath(ASM_CACHE_FLAGS dwCacheFlags, 
    __out_ecount_opt(*pcchPath) LPWSTR pwzCachePath, __inout PDWORD pcchPath)
{
    HRESULT hr = S_OK;
    LPWSTR  pszTemp = NULL;
    DWORD dwLen=0;

    if( !pcchPath || !dwCacheFlags || (dwCacheFlags & (dwCacheFlags-1))) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if(IsZapType(dwCacheFlags)) {
        hr = GetZapDir(&pszTemp);
    }
    else if(IsGacType(dwCacheFlags) || (dwCacheFlags & CACHE_ROOT)) {
        hr = GetGACDir((CACHE_FLAGS)dwCacheFlags, &pszTemp);
    }
    else if(dwCacheFlags & CACHE_DOWNLOAD) {
        hr = GetDownloadDir(&pszTemp);
    }
    else {
        hr = E_INVALIDARG;
    }

    if(FAILED(hr)) {
        goto Exit;
    }

    dwLen = lstrlenW(pszTemp);

    if(pwzCachePath && (*pcchPath > dwLen)) {
        hr = StringCchCopy(pwzCachePath, *pcchPath, pszTemp);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    *pcchPath = dwLen+1;

Exit:

    return hr;
}

HRESULT VerifySignatureHelper(CTransCache *pTC, DWORD dwVerifyFlags)
{
    HRESULT hr = S_OK;
    BOOL fWasVerified;

    if (!VerifySignature( pTC->_pInfo->pwzPath, 
            &(fWasVerified = FALSE), dwVerifyFlags))
    {
        hr = FUSION_E_SIGNATURE_CHECK_FAILED;
        goto exit;
    }

exit :

    return hr;
}

HRESULT GetDLParentDir(LPCWSTR pszURL, 
    __out_ecount(dwParentDirSz) LPWSTR pszParentDir, DWORD dwParentDirSz, 
    LPCWSTR pwzCustomCachePath)
{
    HRESULT                      hr = S_OK;
    BYTE                        *pbHash = NULL;
    DWORD                        dwLen;
    DWORD                        dwHashSize;
    DWORD                        dwBufSize = MAX_PATH + MAX_URL_LENGTH + 1;
    LPWSTR                       pwzCachePath = NULL;
    LPWSTR                       pwzHashData = NULL;
    HCRYPTPROV                   hProv = 0;
    HCRYPTHASH                   hHash = 0;
    BOOL                         bRet;

    if (!pszURL) {
        hr = S_FALSE;
        goto Exit;
    }
    
    // Combined obfuscated download cache path with the source URL

    if (pwzCustomCachePath && pwzCustomCachePath[0]) {
        pwzCachePath = const_cast<LPWSTR>(pwzCustomCachePath);
    }
    else {
        hr = GetDownloadDir(&pwzCachePath);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    pwzHashData = NEW(WCHAR[dwBufSize]);
    if (!pwzHashData) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = StringCchPrintfW(pwzHashData, dwBufSize, L"%ws%ws", pwzCachePath, pszURL);
    if (FAILED(hr)) {
        goto Exit;
    }

    // make the case insensitive
    hr = CanonicalizeIdentity(pwzHashData);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Hash the download cache + source URL location

    if (!g_hProv) {
        bRet = CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL,
                                    CRYPT_VERIFYCONTEXT);
        if (!bRet) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Exit;
        }

        if (InterlockedCompareExchangePointer((void **)&g_hProv, (void *)hProv, 0)) {
            // Lost the race. Release our provider.

            CryptReleaseContext(hProv, 0);
        }
    }

    bRet = CryptCreateHash(g_hProv, CALG_SHA1, 0, 0, &hHash);
    if (!bRet) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    bRet = CryptHashData(hHash, (BYTE *)pwzHashData, (lstrlenW(pwzHashData) + 1) * sizeof(WCHAR), 0);
    if (!bRet) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    dwLen = sizeof(DWORD);
    bRet = CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwLen, 0);
    if (!bRet) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    // We need at least 32 bits for the hash
    _ASSERTE(dwHashSize >= 4);
    if (dwHashSize < 4) {
        hr = E_UNEXPECTED;
        goto Exit;
    }

    pbHash = NEW(BYTE[dwHashSize]);
    if (!pbHash) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    bRet = CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashSize, 0);
    if (!bRet) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    // Take the top 32 bits, and base-16 encode.
    if (dwParentDirSz < DWORD_STRING_LEN + 1) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }
    CParseUtils::BinToUnicodeHex(pbHash, sizeof(DWORD), pszParentDir);
    pszParentDir[DWORD_STRING_LEN] = L'\0';

Exit:
    if (pwzHashData) {
        SAFEDELETEARRAY(pwzHashData);
    }

    if (hHash) {
        CryptDestroyHash(hHash);
    }

    if (pbHash) {
        SAFEDELETEARRAY(pbHash);
    }

    return hr;
}

HRESULT GetDLAsmDir(LPFILETIME pftLastMod, __in_opt LPCWSTR pwzModifier, 
    __out_ecount(dwMaxLen) LPWSTR pszSubDirName, DWORD dwMaxLen)
{

    HRESULT     hr = S_OK;
    DWORD       dwLen = DWORD_STRING_LEN;
    DWORD       dwBufRequired = (dwLen * 2) + 2;    // DWORD + '_' + DWORD + NULL

    _ASSERTE(pftLastMod && pszSubDirName && dwMaxLen);

    if(pwzModifier) {
        dwBufRequired += lstrlenW(pwzModifier);
    }

    if(dwBufRequired > dwMaxLen) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    CParseUtils::BinToUnicodeHex((LPBYTE)&(pftLastMod->dwLowDateTime), sizeof(DWORD), pszSubDirName);
    pszSubDirName[dwLen] = ATTR_SEPARATOR_CHAR;
    CParseUtils::BinToUnicodeHex((LPBYTE)&(pftLastMod->dwHighDateTime), sizeof(DWORD), pszSubDirName+dwLen+1);
    pszSubDirName[2*dwLen+1] = L'\0';

    // Folder name modifier?
    if(pwzModifier && lstrlenW(pwzModifier)) {
        hr = StringCchCat(pszSubDirName, dwMaxLen, pwzModifier);
    }

Exit:
    return hr;

}

HRESULT ParseDLAsmDir(LPCWSTR pszAsmDir, LPFILETIME pftLastMod, __deref_out LPWSTR *pwzPathMod)
{
    HRESULT     hr = S_OK;
    DWORD       dwNormalLen = DWORD_STRING_LEN * 2 + 1;
    DWORD       dwAsmDirLen = 0;
    
    _ASSERTE(pszAsmDir && pftLastMod && pwzPathMod);

    // We can get 2 different types of folders names now,
    // normal case of "dateTime_LastMod" and new asp.net case
    // "dateTime_LastMod_nnnnn" (nnnnn can be 1-5 chars)

    dwAsmDirLen = lstrlenW(pszAsmDir);

    if(dwAsmDirLen < dwNormalLen) {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto Exit;
    }
    else if(pszAsmDir[DWORD_STRING_LEN] != ATTR_SEPARATOR_CHAR) {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto Exit;
    }
    else if(dwAsmDirLen > dwNormalLen) {
        // Possible Asp.net sub folder name
        if(pszAsmDir[dwNormalLen] != ATTR_SEPARATOR_CHAR) {
            hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            goto Exit;
        }
        else if( (DWORD) lstrlenW(pszAsmDir + dwNormalLen) > MAX_PATH_MODIFIER_LENGTH - 1) {
            hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            goto Exit;
        }

        // If this is set, we leak memory
        _ASSERTE(!*pwzPathMod);

        *pwzPathMod = NEW(WCHAR[MAX_PATH_MODIFIER_LENGTH]);
        if(!*pwzPathMod) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        // Asp.net sub folder name
        *pwzPathMod[0] = L'\0';
        hr = StringCchCopy(*pwzPathMod, MAX_PATH_MODIFIER_LENGTH, pszAsmDir + dwNormalLen);
        if (FAILED(hr)) {
            goto Exit;
        }
    }


    CParseUtils::UnicodeHexToBin(pszAsmDir, DWORD_STRING_LEN, (LPBYTE) &(pftLastMod->dwLowDateTime));
    CParseUtils::UnicodeHexToBin(pszAsmDir + DWORD_STRING_LEN + 1, DWORD_STRING_LEN, (LPBYTE) &(pftLastMod->dwHighDateTime));


Exit:
    return hr;
}

HRESULT GetAssemblyParentDir( CTransCache *pTransCache, 
    __out_ecount_opt(*pcchSize) LPWSTR pszParentDir, __inout LPDWORD pcchSize, LPCWSTR pwzCustomPath)
{
    HRESULT     hr = S_OK;
    CACHE_FLAGS dwCache = CACHE_INVALID;
    WCHAR       wzTempPath[MAX_PATH + 1];

    _ASSERTE(pTransCache && pcchSize);

    wzTempPath[0] = L'\0';
    dwCache = pTransCache->GetCacheType();

    if(IsGacType(dwCache)) {
        if(!pTransCache->_pInfo->pwzName) {
            hr = S_FALSE;
            goto Exit;
        }

        if(lstrlenW(pTransCache->_pInfo->pwzName) > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCbCopy(wzTempPath, sizeof(wzTempPath), pTransCache->_pInfo->pwzName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else if(IsZapType(dwCache)) {
        if(!pTransCache->_pInfo->pwzName) {
            hr = S_FALSE;
            goto Exit;
        }

        if(lstrlenW(pTransCache->_pInfo->pwzName) > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCbCopy(wzTempPath, sizeof(wzTempPath), pTransCache->_pInfo->pwzName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else if(dwCache & CACHE_DOWNLOAD) {
        hr = GetDLParentDir(pTransCache->_pInfo->pwzCodebaseURL, 
            wzTempPath, ARRAYSIZE(wzTempPath), pwzCustomPath);
    }
    else {
        _ASSERTE(!"Invalid cache flags!");
        hr = S_FALSE;
        goto Exit;
    }

    if(SUCCEEDED(hr)) {
        if((DWORD) lstrlenW(wzTempPath) >= *pcchSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCchCopy(pszParentDir, *pcchSize, wzTempPath);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:

    if(pcchSize) {
        *pcchSize = lstrlenW(wzTempPath) + 1;
    }

    return hr;
}

HRESULT ParseAsmDir(PEKIND dwAsmImageType, LPCWSTR pszAsmDir, CTransCache *pTC)
{
    HRESULT hr=S_OK;
    TRANSCACHEINFO *pTCInfo = pTC->_pInfo;
    WCHAR wzDirName[MAX_PATH+1];
    LPWSTR pwzTemp=NULL, pwzStringLeft=NULL,pwzCulture=NULL ;
    DWORD dwLen =0;
    PBYTE lpByte=NULL;

    _ASSERTE(pTC);

    pTCInfo = pTC->_pInfo;

    hr = StringCbCopy(wzDirName, sizeof(wzDirName), pszAsmDir);
    if (FAILED(hr)) {
        goto exit;
    }
    
    pwzStringLeft = wzDirName;

    pwzTemp = StrChrW(pwzStringLeft, ATTR_SEPARATOR_CHAR);
    if(!pwzTemp) {
        _ASSERTE(!"Incorrect directory name!");
        hr = E_UNEXPECTED;
        goto exit;
    }
    *pwzTemp = L'\0';
    hr = VersionFromString(pwzStringLeft, &(pTCInfo->wVers[0]), &(pTCInfo->wVers[1]), &(pTCInfo->wVers[2]), &(pTCInfo->wVers[3]));
    if(FAILED(hr))
        goto exit;

    pwzStringLeft = ++pwzTemp;
    pwzTemp = StrChrW(pwzStringLeft, ATTR_SEPARATOR_CHAR);
    if(!pwzTemp) {
        _ASSERTE(!"Incorrect directory name!");
        hr = E_UNEXPECTED;
        goto exit;
    }
    *pwzTemp = L'\0';
    dwLen = lstrlenW(pwzStringLeft)+1;
    pwzCulture = NEW(WCHAR[dwLen]);
    if(!pwzCulture)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = StringCchCopy(pwzCulture, dwLen, pwzStringLeft);
    if (FAILED(hr)) {
        goto exit;
    }
 
    pwzStringLeft = ++pwzTemp;
    pwzTemp = StrChrW(pwzStringLeft, ATTR_SEPARATOR_CHAR);
    if(pwzTemp)
    {
        *pwzTemp = L'\0';
        // we don't parse beyond this point.
    }
    dwLen = lstrlenW(pwzStringLeft)/2;
    if(dwLen)
    {
        lpByte = NEW(BYTE[dwLen]);

        if(!lpByte)
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }
        CParseUtils::UnicodeHexToBin(pwzStringLeft, lstrlenW(pwzStringLeft), lpByte );
    }
    
    pTCInfo->blobPKT.pBlobData = lpByte; lpByte = NULL;
    pTCInfo->blobPKT.cbSize = dwLen;
    pTCInfo->pwzCulture  = pwzCulture; pwzCulture = NULL;
    pTCInfo->dwAsmImageType = dwAsmImageType;

exit:

    SAFEDELETEARRAY(lpByte);
    SAFEDELETEARRAY(pwzCulture);

    return hr;
}

HRESULT ParseDirName(CTransCache *pTransCache, PEKIND dwAsmArchType, 
                    LPCWSTR pszParentDir, LPCWSTR pszAsmDir)
{
    HRESULT hr=S_OK;
    CACHE_FLAGS     CacheFlags = CACHE_INVALID;

    _ASSERTE(pTransCache && pszParentDir && pszAsmDir);
    _ASSERTE(lstrlenW(pszAsmDir) < MAX_PATH);

    CacheFlags = pTransCache->GetCacheType();

    if(IsGacType(CacheFlags) || IsZapType(CacheFlags)) {
        pTransCache->_pInfo->pwzName = WSTRDupDynamic(pszParentDir);
        if(!pTransCache->_pInfo->pwzName) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        
        hr = ParseAsmDir(dwAsmArchType, pszAsmDir, pTransCache);
        if(FAILED(hr)) {
            goto Exit;
        }

        if(CacheFlags & CACHE_GAC) {
            pTransCache->_pInfo->fLegacyAssembly = TRUE;
        }
    }
    else if(CacheFlags & CACHE_DOWNLOAD) {
        hr = ParseDLAsmDir(pszAsmDir, &(pTransCache->_pInfo->ftLastModified), &(pTransCache->_pInfo->pwzPathModifier));
    }
    else {
        _ASSERTE(!"Invalid cache flag!");
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT GetDLDirsFromName(IAssemblyName *pName, 
    __out_ecount(dwParentDirSz) LPWSTR pszParentDir, DWORD dwParentDirSz,
    __out_ecount(dwSubDirSz) LPWSTR pszSubDirName, DWORD dwSubDirSz, LPCWSTR pwzCustomPath)
{
    CAssemblyName   *pCName = NULL;
    HRESULT         hr = S_OK;
    LPWSTR          pwzCodebaseURL = NULL;
    FILETIME        ftLastMod;
    DWORD           cb=0;
    WCHAR           wzPathModifier[MAX_PATH_MODIFIER_LENGTH];

    _ASSERTE(pName && pszParentDir && dwParentDirSz && pszSubDirName && dwSubDirSz);

    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_CODEBASE_URL,
        (LPBYTE*) &pwzCodebaseURL, &(cb = 0))))
        goto exit;

    hr = GetDLParentDir(pwzCodebaseURL, pszParentDir, dwParentDirSz, pwzCustomPath);

    if(FAILED(hr = pName->GetProperty(ASM_NAME_CODEBASE_LASTMOD,
        &ftLastMod, &(cb = sizeof(FILETIME)))))
        goto exit;

    pCName = static_cast<CAssemblyName*>(pName); // dynamic_cast
    wzPathModifier[0] = L'\0';
    cb = MAX_PATH_MODIFIER_LENGTH;
    hr = pCName->GetPathModifier(wzPathModifier, &cb);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = GetDLAsmDir(&ftLastMod, wzPathModifier, pszSubDirName, dwSubDirSz);

exit:
    SAFEDELETE(pwzCodebaseURL);
    return hr;
}

HRESULT GetAsmDir(__inout_ecount(dwMaxLen) LPWSTR pszSubDirName, DWORD dwMaxLen, DWORD dwCache, 
                  WORD wVers[4], LPCWSTR pszCulture, 
                  PBYTE pPKT, DWORD cbPKT, PBYTE pCustom, DWORD cbCustom,
                  DWORD dwConfigMask)
{
    HRESULT hr = S_OK;
    DWORD dwLen;

    _ASSERTE(pszSubDirName && dwMaxLen && pszCulture);

    if( (MAX_VERSION_DISPLAY_SIZE + lstrlenW(pszCulture) + 
                    (cbPKT * 2) + (cbCustom*2) + 4) > MAX_PATH )
    {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }

    hr = StringCchPrintfW(pszSubDirName, dwMaxLen, L"%hu.%hu.%hu.%hu", wVers[0], wVers[1], wVers[2], wVers[3]);
    if (FAILED(hr)) {
        goto exit;
    }

    dwLen = lstrlenW(pszSubDirName);
    hr = StringCchPrintfW(pszSubDirName+dwLen, dwMaxLen-dwLen, L"_%ws_", pszCulture);
    if (FAILED(hr)) {
        goto exit;
    }
    dwLen = lstrlenW(pszSubDirName);

    if(dwLen + (cbPKT * 2) + 1 > dwMaxLen) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto exit;
    }
        
    CParseUtils::BinToUnicodeHex(pPKT, cbPKT, pszSubDirName+dwLen);
    dwLen += cbPKT * 2;

    if(IsZapType(dwCache))
    {
        _ASSERTE(pCustom);

        if(lstrlenW(pszSubDirName) + lstrlenW(L"_") + DWORD_STRING_LEN + 1 > dwMaxLen) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto exit;
        }

        hr = StringCchCopy(pszSubDirName+dwLen, dwMaxLen - dwLen, L"_");
        if (FAILED(hr)) {
            goto exit;
        }

        dwLen++;

        DWORD dwHash = HashBlob(pCustom, cbCustom, dwConfigMask, 0);
        // Convert to unicode.
        CParseUtils::BinToUnicodeHex( (PBYTE) &dwHash, sizeof(DWORD), pszSubDirName+dwLen);
        dwLen += DWORD_STRING_LEN;
    }
    pszSubDirName[dwLen] = L'\0'; 

    hr = ValidateAsmInstallFolderChars(pszSubDirName);
    if(FAILED(hr)) {
        goto exit;
    }

exit:
    return hr;
}

HRESULT GetCacheDirsFromName(IAssemblyName *pName, DWORD dwFlags, 
    __out_ecount(dwParentDirSz) LPWSTR pszParentDirName, DWORD dwParentDirSz, 
    __out_ecount(dwSubDirSz) LPWSTR pszSubDirName, DWORD dwSubDirSz, 
    LPCWSTR pwzCustomPath)
{
    HRESULT hr = S_OK;
    WORD    wVers[4];
    LPWSTR  pszTextName=NULL;
    DWORD   cb=0;
    PBYTE   pPKT=NULL;
    PBYTE   pCustom=NULL;
    DWORD   cbCustom=0;
    DWORD   cbPKT=0;
    LPWSTR  wzCulture=NULL;
    LPWSTR  szProp=NULL;
    DWORD   dwConfigMask = 0;
    DWORD   dwSize = 0;

    _ASSERTE(pName && dwFlags && pszParentDirName && dwParentDirSz && pszSubDirName && dwSubDirSz);

    if(IsGacType(dwFlags) || IsZapType(dwFlags))
    {
        hr = NameObjGetWrapper(pName, ASM_NAME_NAME, (LPBYTE*) &pszTextName, &(cb = 0));
        if(FAILED(hr)) {
            goto exit;
        }
        PREFIX_ASSUME(pszTextName != NULL);
            
        if(cb > dwParentDirSz) {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        hr = StringCchCopy(pszParentDirName, dwParentDirSz, pszTextName);
        if (FAILED(hr)) {
            goto exit;
        }

        // Version
        for (DWORD i=0; i<4; i++) {
            cb = sizeof(WORD);
            hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION+i, &(wVers[i]), &cb);
            if (FAILED(hr)) {
                goto exit;
            }
        }

        // Culture
        if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_CULTURE, (LPBYTE*) &wzCulture, &cb)))
            goto exit;

        // PublicKeyToken
        if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_PUBLIC_KEY_TOKEN, &pPKT, &cbPKT)))
            goto exit;

        if(IsZapType(dwFlags)) {
            hr = NameObjGetWrapper(pName, ASM_NAME_CUSTOM, &pCustom, &(cbCustom = 0));
            if(FAILED(hr)) {
                goto exit;
            }

            dwSize = sizeof(DWORD);
            if (FAILED(hr = pName->GetProperty(ASM_NAME_CONFIG_MASK, (LPBYTE)&dwConfigMask, &dwSize))) {
                goto exit;
            }
        }

        hr = GetAsmDir(pszSubDirName, dwSubDirSz, dwFlags, wVers,
            wzCulture, pPKT, cbPKT, pCustom, cbCustom, dwConfigMask);
    }
    else if (dwFlags & CACHE_DOWNLOAD)
    {
        hr = GetDLDirsFromName(pName, pszParentDirName, dwParentDirSz, pszSubDirName, dwSubDirSz, pwzCustomPath);
        if(FAILED(hr)) {
            goto exit;
        }
    }

exit:

    SAFEDELETEARRAY(szProp);
    SAFEDELETEARRAY(wzCulture);
    SAFEDELETEARRAY(pPKT);
    SAFEDELETEARRAY(pCustom);
    SAFEDELETEARRAY(pszTextName);

    return hr;
}

#define MAX_ZAP_NAME_LENGTH     20
#define ZAP_ABBR_END_CHAR       L'#'
HRESULT GetZapDirFromName(IAssemblyName *pName, 
        __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen)
{
    HRESULT hr = S_OK;
    WCHAR wzParentDir[MAX_PATH];
    WCHAR wzSubDir[FUSION_GUID_LENGTH + 1];
    DWORD dwNameSize = 0;
    DWORD dwSize = 0;
    GUID  guidMvid;
    
    dwNameSize = MAX_PATH * sizeof(WCHAR);
    hr = pName->GetProperty(ASM_NAME_NAME, (LPBYTE)wzParentDir, &dwNameSize);
    if (FAILED(hr)) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }
    dwNameSize /= sizeof(WCHAR);
    if (dwNameSize > MAX_ZAP_NAME_LENGTH + 1) {
        wzParentDir[MAX_ZAP_NAME_LENGTH - 1] = ZAP_ABBR_END_CHAR;
        wzParentDir[MAX_ZAP_NAME_LENGTH] = L'\0';
        dwNameSize = MAX_ZAP_NAME_LENGTH + 1;
    }

    dwSize = sizeof(GUID);
    hr = pName->GetProperty(ASM_NAME_MVID, (LPBYTE)&guidMvid, &dwSize);
    if (FAILED(hr)) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    FusionFormatGUID(&guidMvid, wzSubDir, ARRAYSIZE(wzSubDir), FALSE);
    dwSize = ARRAYSIZE(wzSubDir) + dwNameSize;

    if (*pdwLen < dwSize) {
        *pdwLen = dwSize;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    hr = StringCchCopy(pwzAsmDir, *pdwLen, wzParentDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzAsmDir, *pdwLen, L"\\");
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCchCat(pwzAsmDir, *pdwLen, wzSubDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    *pdwLen = dwSize;

Exit:
    return hr;
}

HRESULT GetFullZapDirFromName(IAssemblyName *pName, 
        __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzZapDir = NULL;
    WCHAR   pwzSubDir[MAX_PATH];
    DWORD   dwLen = 0;

    _ASSERTE(pName);

    dwLen = ARRAYSIZE(pwzSubDir);
    hr = GetZapDirFromName(pName, pwzSubDir, &dwLen);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    hr = GetZapDir(&pwzZapDir);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    dwLen += lstrlenW(pwzZapDir) + 1/*'\\'*/; 
    if (*pdwLen < dwLen) {
        *pdwLen = dwLen;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto ErrExit;
    }

    hr = StringCchPrintf(pwzAsmDir, *pdwLen, L"%ws\\%ws", pwzZapDir, pwzSubDir);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    *pdwLen = dwLen;

ErrExit:
    return hr;

}
HRESULT GetFullGACDirFromName(IAssemblyName *pName, DWORD dwCacheType, 
        __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzGacDir = NULL;
    WCHAR   pwzParentDir[MAX_PATH];
    WCHAR   pwzSubDir[MAX_PATH];
    DWORD   dwLen = 0;

    _ASSERTE(pName && pdwLen);

    hr = GetCacheDirsFromName(pName, CACHE_GAC, pwzParentDir, ARRAYSIZE(pwzParentDir), pwzSubDir, ARRAYSIZE(pwzSubDir), NULL);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    
    hr = GetGACDir((CACHE_FLAGS) dwCacheType, &pwzGacDir);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    dwLen = lstrlenW(pwzGacDir) + 1/*'\\'*/+ lstrlenW(pwzParentDir) + 1/*'\\'*/ + lstrlenW(pwzSubDir) + 1/*'\0'*/; 

    if (*pdwLen < dwLen) {
        *pdwLen = dwLen;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto ErrExit;
    }

    hr = StringCchPrintf(pwzAsmDir, *pdwLen, L"%ws\\%ws\\%ws", pwzGacDir, pwzParentDir, pwzSubDir);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    *pdwLen = dwLen;
    
ErrExit:
    return hr;
}
   
HRESULT GetManifestFilePathFromName(IAssemblyName *pName,
                                            BOOL isDll, 
                                            DWORD dwCacheType, 
                                            __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, 
                                            __inout DWORD *pdwLen)
{
    HRESULT hr = S_OK;
    DWORD dwSize;
    WCHAR wzAsmDir[MAX_PATH];
    WCHAR wzAsmName[MAX_PATH];
    DWORD dwSizeTemp;

    _ASSERTE(pName);
    _ASSERTE(pdwLen);

    dwSize = MAX_PATH;

    if (IsGacType(dwCacheType)) {
        hr = GetFullGACDirFromName(pName, dwCacheType, wzAsmDir, &dwSize);
    }
    else if(IsZapType(dwCacheType)) {
        hr = GetFullZapDirFromName(pName, wzAsmDir, &dwSize);

    }
    else {
        _ASSERTE(!"Invalid cache type passed in!");
    }

    if (FAILED(hr)) {
        goto ErrExit;
    }

    dwSize = MAX_PATH * sizeof(WCHAR);
    hr = pName->GetProperty(ASM_NAME_NAME, wzAsmName, &dwSize);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    dwSize /= sizeof(WCHAR);
    dwSizeTemp = dwSize;
    dwSizeTemp += sizeof(".dll");
    if (IsZapType(dwCacheType)) {
        dwSizeTemp += lstrlenW(NATIVE_IMAGE_SUFFIX);
    }
    
    if (dwSizeTemp > MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto ErrExit;
    }

    // Add a suffix to native image assembly so that they will not
    // collide for the same IL assembly in .local
    if (IsZapType(dwCacheType)) {
        hr = StringCbCat(wzAsmName, sizeof(wzAsmName), NATIVE_IMAGE_SUFFIX);
        if (FAILED(hr)) {
            goto ErrExit;
        }
    }

    if (isDll) {
        hr = StringCbCat(wzAsmName, sizeof(wzAsmName), g_szDotDLL);
        if (FAILED(hr)) {
            goto ErrExit;
        }
    }
    else {
        hr = StringCbCat(wzAsmName, sizeof(wzAsmName), g_szDotEXE);
    }

    dwSize = lstrlenW(wzAsmDir) + 1 /*'\\'*/ + lstrlenW(wzAsmName) + 1 /*'\0'*/;
    if (*pdwLen < dwSize){
        *pdwLen = dwSize;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto ErrExit;
    }

    hr = StringCchPrintf(pwzAsmDir, *pdwLen, L"%ws\\%ws", wzAsmDir, wzAsmName);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    *pdwLen = dwSize;

ErrExit:
    return hr;
}

HRESULT GetCacheDirsFromTransCache(CTransCache *pTransCache, DWORD dwFlags, 
    __out_ecount(dwParentDirLen) LPWSTR pszParentDirName, DWORD dwParentDirLen, 
    __out_ecount(dwSubDirLen) LPWSTR pszSubDirName, DWORD dwSubDirLen, 
    LPCWSTR pwzCustomPath)
{
    HRESULT hr = S_OK;
    TRANSCACHEINFO *pTCInfo = NULL;
    CACHE_FLAGS     dwCache = CACHE_INVALID;

    _ASSERTE(pTransCache);
    
    dwCache = pTransCache->GetCacheType();
    pTCInfo = (TRANSCACHEINFO*) pTransCache->_pInfo;

    if(IsGacType(dwCache)) {
        if(lstrlenW(pTCInfo->pwzName) >= MAX_PATH)
        {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        hr = StringCchCopy(pszParentDirName, dwParentDirLen, pTCInfo->pwzName);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = GetAsmDir(pszSubDirName, dwSubDirLen, dwCache, 
                       pTCInfo->wVers, 
                       pTCInfo->pwzCulture, 
                       pTCInfo->blobPKT.pBlobData, pTCInfo->blobPKT.cbSize,
                       NULL, 0, 0);
    }
    else if(dwCache & CACHE_DOWNLOAD)
    {
        hr = GetDLParentDir(pTCInfo->pwzCodebaseURL, 
            pszParentDirName, dwParentDirLen, pwzCustomPath);
        if(FAILED(hr)) {
            goto exit;
        }

        hr = GetDLAsmDir(&(pTCInfo->ftLastModified), pTCInfo->pwzPathModifier, pszSubDirName, dwSubDirLen);
    }

exit:

    return hr;
}

HRESULT RetrieveFromFileStore( CTransCache *pTransCache )
{
    HRESULT hr = S_OK;
    TRANSCACHEINFO *pTCInfo = NULL;
    WCHAR wzParentDirName[MAX_PATH+1];
    WCHAR wzSubDirName[MAX_PATH+1];
    DWORD dwLen =0;
    WCHAR wzCacheLocation[MAX_PATH+1];
    CACHE_FLAGS dwAsmCacheType = CACHE_INVALID;
    BOOL    fExists = FALSE;

    _ASSERTE(pTransCache);

    pTCInfo = (TRANSCACHEINFO*) pTransCache->_pInfo;
    dwAsmCacheType = pTransCache->GetCacheType();

    if(IsGacType(dwAsmCacheType) && !pTCInfo->fLegacyAssembly) {
        MapAssemblyArchitectureToCacheType(pTCInfo->dwAsmImageType, &dwAsmCacheType);
        // update the correct gac type
        pTransCache->_dwCacheType = dwAsmCacheType;
    }

    dwLen = MAX_PATH;
    hr = CreateAssemblyDirPath(pTransCache->GetCustomPath(), dwAsmCacheType, 0, wzCacheLocation, &dwLen);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = GetCacheDirsFromTransCache(pTransCache, 0, wzParentDirName, ARRAYSIZE(wzParentDirName),
        wzSubDirName, ARRAYSIZE(wzSubDirName), pTransCache->GetCustomPath());
    if(FAILED(hr))
        goto exit;

    if( (lstrlenW(wzCacheLocation) + lstrlenW(wzParentDirName) + lstrlenW(wzSubDirName) +
            lstrlenW(g_szDotDLL) + max(lstrlenW(wzParentDirName), lstrlenW(pTCInfo->pwzName)) + 4) >= MAX_PATH)
    {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }

    hr = PathAddBackslashWrap(wzCacheLocation, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }
    
    hr = StringCbCat(wzCacheLocation, sizeof(wzCacheLocation), wzParentDirName);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(wzCacheLocation, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }
    
    hr = StringCbCat(wzCacheLocation, sizeof(wzCacheLocation), wzSubDirName);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = CheckFileExistence(wzCacheLocation, &fExists, NULL);
    if(FAILED(hr)) {
        goto exit;
    }
    else if(!fExists) {
        hr = S_FALSE;
        goto exit;
    }

    hr = PathAddBackslashWrap(wzCacheLocation, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }

    // Find AsmName.dll first
    hr = StringCbCat(wzCacheLocation, sizeof(wzCacheLocation), pTCInfo->pwzName);
    if (FAILED(hr)) {
        goto exit;
    }

    dwLen  = lstrlenW(wzCacheLocation);

    hr = StringCbCat(wzCacheLocation, sizeof(wzCacheLocation), g_szDotDLL);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = CheckFileExistence(wzCacheLocation, &fExists, NULL);
    if(FAILED(hr)) {
        goto exit;
    }
    else if(fExists) {
        hr = S_OK;
        goto exit;
    }
    
    // Try AsmName.exe
    hr = StringCbCopy(wzCacheLocation+dwLen, sizeof(wzCacheLocation) - dwLen * sizeof(WCHAR), g_szDotEXE);
    if (FAILED(hr)) {
        goto exit;
    }

    if(!IsGacType(dwAsmCacheType)) {

        hr = S_OK;
        goto exit;
    }

    hr = CheckFileExistence(wzCacheLocation, &fExists, NULL);
    if(FAILED(hr)) {
        goto exit;
    }
    else if(fExists) {
        hr = S_OK;
    }
    else {
        hr = S_FALSE;
    }

exit:

    // On success, replace the name with the fully qualified filepath
    if(hr == S_OK) {
        SAFEDELETEARRAY(pTCInfo->pwzPath);
        pTCInfo->pwzPath = WSTRDupDynamic(wzCacheLocation);

        if(!pTCInfo->pwzPath) {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

HRESULT StoreFusionInfo(IAssemblyName *pName, LPCWSTR pszDir, CACHE_FLAGS dwCacheFlags, DWORD *pdwFileSizeLow)
{
    if (IsGacType(dwCacheFlags)) {
        *pdwFileSizeLow = 0;
        if (dwCacheFlags == CACHE_GAC) { // legacy gac.
            return StoreFusionInfoToLegacyGAC(pName, pszDir);
        }
        else {
            return S_OK;
        }
    }
    else {
        _ASSERTE(dwCacheFlags == CACHE_DOWNLOAD);
        return StoreFusionInfoToDownloadCache(pName, pszDir, pdwFileSizeLow);
    }
}

// for legacy compat only
HRESULT StoreFusionInfoToLegacyGAC(IAssemblyName *pName, LPCWSTR pszDir)
{
    HRESULT hr = S_OK;
    WCHAR   pszFilePath[MAX_PATH+1];
    PBYTE   pSignature=NULL;
    PBYTE   pMVID=NULL;
    DWORD   cbSize=0;
    BOOL    fRet = FALSE;
    LPWSTR  pszBuf=NULL;
    LPWSTR  pszURL = NULL;
    DWORD   dwSize = 0;
    HINI hIni=NULL;

    if(( lstrlenW(pszDir) + lstrlenW(g_FusionInfoFile) + 1) >= MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto exit;
    }

    hr = StringCbCopy(pszFilePath, sizeof(pszFilePath), pszDir);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(pszFilePath, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(pszFilePath, sizeof(pszFilePath), g_FusionInfoFile);
    if (FAILED(hr)) {
        goto exit;
    }

    if ((hIni = PAL_IniCreate()) == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

#define _WriteString(section, key, value) PAL_IniWriteString(hIni, section, key, value)

    pszBuf = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if(!pszBuf)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    cbSize = 0;
    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_SIGNATURE_BLOB, 
            &pSignature, &cbSize)))
        goto exit;

    if(cbSize && (cbSize == SIGNATURE_BLOB_LENGTH_HASH))
    {
        CParseUtils::BinToUnicodeHex(pSignature, cbSize, pszBuf);

        pszBuf[SIGNATURE_BLOB_LENGTH_HASH*2] = L'\0';

        fRet = _WriteString(ASSEMBLY_INFO_STRING, SIGNATURE_BLOB_KEY_STRING, pszBuf);
        if (!fRet)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
    }

    cbSize = 0;
    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_MVID,
            &pMVID, &cbSize)))
        goto exit;

    if(cbSize && (cbSize == MVID_LENGTH))
    {
        CParseUtils::BinToUnicodeHex(pMVID, cbSize, pszBuf);

        pszBuf[MVID_LENGTH*2] = L'\0';

        fRet = _WriteString(ASSEMBLY_INFO_STRING, MVID_KEY_STRING, pszBuf);
        if (!fRet)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
    }

    cbSize = 0;
    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_CODEBASE_URL, (LPBYTE*) &pszURL, &cbSize)))
        goto exit;

    if(cbSize)
    {
        fRet = _WriteString(ASSEMBLY_INFO_STRING, URL_STRING, pszURL);
        if (!fRet)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
    }

    dwSize = MAX_URL_LENGTH+1;
    hr = pName->GetDisplayName(pszBuf, &dwSize, 0);
    if (FAILED(hr)) {
        goto exit;
    }

    if(dwSize)
    {
        fRet = _WriteString(ASSEMBLY_INFO_STRING, DISPLAY_NAME_STRING, pszBuf);
        if (!fRet)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
    }

    if (!PAL_IniSave(hIni, pszFilePath, TRUE))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;        
    }

exit:

    if (hIni != NULL)
        PAL_IniClose(hIni);

    SAFEDELETEARRAY(pszBuf);
    SAFEDELETEARRAY(pSignature);
    SAFEDELETEARRAY(pMVID);
    SAFEDELETEARRAY(pszURL);

    return hr;
}

HRESULT StoreFusionInfoToDownloadCache(IAssemblyName *pName, LPCWSTR pszDir, DWORD *pdwFileSizeLow)
{
    HRESULT hr = S_OK;
    WCHAR   pszFilePath[MAX_PATH+1];
    HANDLE  hFile = INVALID_HANDLE_VALUE;
    LPWSTR  pszBuf = NULL;
    DWORD   dwTotalSize = 0;
    DWORD   dwSize = 0;
    DWORD   dwWritten;
    DWORD   dwSizeHigh = 0;

    if(( lstrlenW(pszDir) + lstrlenW(g_FusionInfoFile) + 1) >= MAX_PATH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto exit;
    }

    hr = StringCbCopy(pszFilePath, sizeof(pszFilePath), pszDir);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(pszFilePath, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(pszFilePath, sizeof(pszFilePath), g_FusionInfoFile);
    if (FAILED(hr)) {
        goto exit;
    }

    hFile = WszCreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ,
                     NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    pszBuf = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if(!pszBuf) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    // first assembly name.
    dwSize = MAX_URL_LENGTH;
    hr = CAssemblyName::GetCustomDisplayName(pName, pszBuf, &dwSize, 0);
    if (FAILED(hr)) {
        goto exit;
    }
    
    if (!WriteFile(hFile, pszBuf, dwSize * sizeof(WCHAR), &dwWritten, NULL)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    dwTotalSize += dwSize * sizeof(WCHAR);

    dwSize = MAX_URL_LENGTH * sizeof (WCHAR);
    pszBuf[0] = L'\0';
    hr = pName->GetProperty(ASM_NAME_CODEBASE_URL, (LPBYTE)pszBuf, &dwSize);
    if (FAILED(hr)) {
        goto exit;
    }

    _ASSERTE(pszBuf[0]);
    if (!WriteFile(hFile, pszBuf, dwSize, &dwWritten, NULL)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    dwTotalSize += dwSize;
    
    hr = GetFileSizeRoundedToCluster(INVALID_HANDLE_VALUE, &dwTotalSize, &dwSizeHigh);
    if (FAILED(hr)) {
        goto exit;
    }

    *pdwFileSizeLow += dwTotalSize;
    
exit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    SAFEDELETEARRAY(pszBuf);

    return hr;
}

// the only time we need to get fusion info is for download cache.
HRESULT GetFusionInfo(CTransCache *pTC, LPCWSTR pszAsmDir)
{
    HRESULT hr = S_OK;
    WCHAR wzFilePath[MAX_PATH+1];
    DWORD cbSize = 0;
    DWORD cbRead = 0;
    IAssemblyName *pName=NULL;
    LPWSTR pszBuf=NULL;
    BOOL    fExists = FALSE;
    TRANSCACHEINFO *pTCInfo = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LPWSTR  pwzName = NULL;
    LPWSTR  pwzURL = NULL;
    DWORD   dwAttrib = 0;

    _ASSERTE(pTC);
    _ASSERTE(pszAsmDir || pTC->_pInfo->pwzPath);

    if(pTC->_pInfo->pwzPath) {  
        // if there is path in transprtCache obj use it. else use second param pszAsmDir
        hr = StringCchPrintf(wzFilePath, MAX_PATH, L"%ws", pTC->_pInfo->pwzPath);
    }
    else {
        hr = StringCchPrintf(wzFilePath, MAX_PATH, L"%ws", pszAsmDir);
    }
    if (FAILED(hr)) {
        goto exit;
    }

    hr = CheckFileExistence(wzFilePath, &fExists, &dwAttrib);
    if(FAILED(hr)) {
        goto exit;
    }
    else if(!fExists) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto exit;
    }

    if(!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        // looks like manifestFilePath is passed in. knock-off the filename.
        LPWSTR pszTemp = PathFindFileName(wzFilePath);
        if(pszTemp > wzFilePath)
        {
            *(pszTemp-1) = L'\0';
        }
    }
    // else we have assembly dir;

    hr = StringCchPrintf(wzFilePath, MAX_PATH, L"%ws\\%ws", wzFilePath, g_FusionInfoFile);
    if (FAILED(hr)) {
        goto exit;
    }
    hFile = WszCreateFile(wzFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND) {
            hr = S_FALSE;
        }
        else {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        goto exit;
    }

    cbSize = GetFileSize(hFile, NULL);
    if (cbSize == INVALID_FILE_SIZE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    if (cbSize == 0) {
        hr = S_FALSE;
        goto exit;
    }

    // +3 to protect against odd size file and make sure we are always zero terminated. 
    cbRead = (cbSize+3)/sizeof(WCHAR);
    pszBuf = NEW(WCHAR[cbRead]);
    if (!pszBuf) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    memset(pszBuf, 0, cbRead * sizeof(WCHAR));
    if (!ReadFile(hFile, pszBuf, cbSize, &cbRead, NULL)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    cbRead = lstrlenW(pszBuf) + 1;
    if (cbRead * sizeof(WCHAR) >= cbSize) {
        // we need at least two strings, and we only have one. 
        hr = S_FALSE;
        goto exit;
    }

    pwzName = pszBuf;
    pwzURL = pszBuf + cbRead;

    pTCInfo = (TRANSCACHEINFO*) pTC->_pInfo;

    SAFEDELETEARRAY(pTCInfo->pwzCodebaseURL);
    pTCInfo->pwzCodebaseURL = WSTRDupDynamic(pwzURL);
    if (!pTCInfo->pwzCodebaseURL) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = CAssemblyName::ParseCustomDisplayName(pwzName, &pName);
    if (FAILED(hr)) {
        goto exit;
    }

    SAFEDELETEARRAY(pTCInfo->pwzName);

    if (FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_NAME,
            (LPBYTE*) &pTCInfo->pwzName, &cbRead)))
        goto exit;

    // Version
    for(DWORD i=0; i<4; i++) {
        cbSize = sizeof(WORD);
        hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION+i, &(pTCInfo->wVers[i]), &cbSize);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    SAFEDELETEARRAY(pTCInfo->pwzCulture);

    // Culture
    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_CULTURE,
        (LPBYTE*) &pTCInfo->pwzCulture, &cbRead)))
        goto exit;

    SAFEDELETEARRAY(pTCInfo->blobPKT.pBlobData);

    // PublicKeyToken
    if(FAILED(hr = NameObjGetWrapper(pName, ASM_NAME_PUBLIC_KEY_TOKEN, 
        &pTCInfo->blobPKT.pBlobData, &pTCInfo->blobPKT.cbSize)))
        goto exit;

exit:
    SAFEDELETEARRAY(pszBuf);
    SAFERELEASE(pName);

    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    return hr;
}

DWORD GetFileSizeInKB(DWORD dwFileSizeLow, DWORD dwFileSizeHigh);

HRESULT GetAssemblyKBSize(LPCWSTR pszManifestPath, DWORD *pdwSizeinKB, 
                          LPFILETIME pftLastAccess, LPFILETIME pftCreation)
{
    HRESULT hr = S_OK;
    LPWSTR pszTemp=NULL;
    TCHAR szSearchPath[MAX_PATH+1];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindFileData;
    DWORD dwLen = 0;
    DWORD dwAsmSize=0;
    LPWSTR pszManifestFileName=NULL;

    dwLen = lstrlenW(pszManifestPath);
    _ASSERTE(dwLen <= MAX_PATH);

    if(pftLastAccess)
        memset(pftLastAccess, 0, sizeof(FILETIME));

    pszManifestFileName = PathFindFileName(pszManifestPath);

    if(!pszManifestFileName || (pszManifestFileName <= pszManifestPath) )
    {
        hr = E_FAIL;
        goto exit;
    }

    hr = StringCchPrintf(szSearchPath, MAX_PATH, L"%ws", pszManifestPath);
    if (FAILED(hr)) {
        goto exit;
    }

    pszTemp = PathFindFileName(szSearchPath);

    if(!pszTemp || (pszTemp <= szSearchPath) )
    {
        hr = E_FAIL;
        goto exit;
    }

    *(pszTemp-1) = L'\0'; // knock-off filename from szSearchPath
    hr = StringCchPrintf(szSearchPath, MAX_PATH, L"%ws\\*", szSearchPath);
    if (FAILED(hr)) {
        goto exit;
    }

    hFind = WszFindFirstFile(szSearchPath, &FindFileData);

    if(hFind == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    do
    {
        // skip directories    
        if (!FusionCompareStringI(FindFileData.cFileName, L".."))
            continue;

        // count the size of dir ??
        if (!FusionCompareStringI(FindFileData.cFileName, L"."))
            continue;

        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // this is assembly Dir; what are dirs doing here ??
            continue;
        }

        hr = GetFileSizeRoundedToCluster(INVALID_HANDLE_VALUE, &(FindFileData.nFileSizeLow), 
                                                               &(FindFileData.nFileSizeHigh));
        if(FAILED(hr))
            goto exit;

        dwAsmSize += GetFileSizeInKB(FindFileData.nFileSizeLow, FindFileData.nFileSizeHigh);
        if(pftLastAccess && pftCreation && !FusionCompareStringI(pszManifestFileName, FindFileData.cFileName))
        {
            memcpy(pftLastAccess, &(FindFileData.ftLastAccessTime), sizeof(FILETIME));
            memcpy(pftCreation,   &(FindFileData.ftCreationTime),   sizeof(FILETIME));
        }

    }while(WszFindNextFile(hFind, &FindFileData));

    if( GetLastError() != ERROR_NO_MORE_FILES)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    if(pdwSizeinKB)
    {
        *pdwSizeinKB = dwAsmSize;
    }

exit:

    if(hFind != INVALID_HANDLE_VALUE)
        FindClose(hFind);

    return hr;
}

MUTEX_COOKIE g_hCacheMutex = NULL;
const WCHAR g_szCacheLock[] = L"FusionDownloadCacheLockV3.dat";

HRESULT CreateCacheMutex()
{
    HRESULT hr = S_OK;
    LPWSTR pwzLockFile = NULL;
    static BOOL bMutexCreated = FALSE;

    if (bMutexCreated == TRUE) {
        goto exit;
    }

    if(FAILED(hr = FusionGetUserFolderPath())) {
            // This guys does not have download cache;  so we don't need mutex.
            hr = S_OK;
            g_hCacheMutex = INVALID_HANDLE_VALUE; 
            bMutexCreated = TRUE;
        goto exit;
    }

    SIZE_T size;
    
    size = lstrlenW(g_UserFolderPath) + ARRAYSIZE(g_szCacheLock) + 2;
    pwzLockFile = NEW(WCHAR[size]);
    if (!pwzLockFile) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = StringCchCopy(pwzLockFile, size, g_UserFolderPath);
    if (FAILED(hr)) {
        goto exit;
    }
    
    PathAddBackslash(pwzLockFile);

    hr = StringCchCopy(pwzLockFile + lstrlenW(pwzLockFile), size - lstrlenW(pwzLockFile), g_szCacheLock);
    if (FAILED(hr)) {
        goto exit;
    }
    
    if (InterlockedCompareExchangePointer(&g_hCacheMutex, pwzLockFile, NULL)) {
        SAFEDELARRAY(pwzLockFile);
    }
    
    bMutexCreated = TRUE;

exit :    
    return hr;
}

HRESULT CheckExistsInGAC(IAssemblyName *pName, DWORD dwCacheType, LPBOOL pbIsDll, FILETIME *pftLastMod, IAssembly **ppAsm)
{
    HRESULT hr = S_OK;
    DWORD dwSize;
    WCHAR wzAsmFullPath[MAX_PATH];
    BOOL bExists = FALSE;
    LPWSTR pwzFileName = NULL;

    // try dll first
    dwSize = MAX_PATH;
    hr = GetManifestFilePathFromName(pName, TRUE/*isDll*/, dwCacheType, wzAsmFullPath, &dwSize);
    if (FAILED(hr)) {
        if ((hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) || (hr == FUSION_E_INVALID_NAME)) {
            hr = S_FALSE;
        }
        goto ErrExit;
    }

    // find the file name
    pwzFileName = PathFindFileName(wzAsmFullPath);
    // now pwzFileName points to '\'
    pwzFileName--;

    *pwzFileName = L'\0';

    // check if the directory exists or not
    hr = CheckFileExistence(wzAsmFullPath, &bExists, NULL);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    if (!bExists) {
        hr = S_FALSE;
        goto ErrExit;
    }
            
    *pwzFileName = L'\\';
    hr = CheckFileExistence(wzAsmFullPath, &bExists, NULL);
    if (FAILED(hr)) {
        goto ErrExit;
    }
    if (bExists) {
        if (pbIsDll) {
            *pbIsDll = TRUE;
        }
        if (ppAsm) {
            hr = CreateAssemblyFromManifestFile(
                    wzAsmFullPath, NULL, NULL, ppAsm);
        }
        goto ErrExit;
    }

    // try exe now
    hr = StringCbCopy(wzAsmFullPath+dwSize-ARRAYSIZE(g_szDotDLL),
           sizeof(wzAsmFullPath) - (dwSize-ARRAYSIZE(g_szDotDLL)) * sizeof(WCHAR), g_szDotEXE);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    hr = CheckFileExistence(wzAsmFullPath, &bExists, NULL);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    if (bExists) {
        if (pbIsDll) {
            *pbIsDll = FALSE;
        }
        if (ppAsm) {
            hr = CreateAssemblyFromManifestFile(
                    wzAsmFullPath, NULL, NULL, ppAsm);
        }
        goto ErrExit;
    }

ErrExit:
    if (SUCCEEDED(hr)) {
        if (bExists) {
            if (*ppAsm) {
                CAssembly *pCAsm = static_cast<CAssembly *>(*ppAsm);
                pCAsm->SetAssemblyLocation(ASM_CACHE_GAC);
            }
            hr = S_OK;
            if (pftLastMod) {
                WIN32_FILE_ATTRIBUTE_DATA fileInfo;
                if (!WszGetFileAttributesEx(wzAsmFullPath, GetFileExInfoStandard, &fileInfo)) {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
                else {
                    memcpy(pftLastMod, &(fileInfo.ftLastWriteTime), sizeof(FILETIME));
                }
            }
        }
        else {
            hr = S_FALSE;
        }
    }

    return hr;
}

void MapAssemblyArchitectureToCacheType(
  PEKIND dwAsmArchType,
  CACHE_FLAGS *pdwAsmArchCacheType)
{
    _ASSERTE(pdwAsmArchCacheType);

    *pdwAsmArchCacheType = CACHE_GAC_MSIL;
}


WORD GetRealProcessorType()
{
    return 0;
}

HRESULT GetCustomLogPath(__out_ecount_opt(*pdwSize) LPWSTR pwzLogPath, __inout LPDWORD pdwSize)
{
    HRESULT hr = S_OK;
    WCHAR szLogPath[MAX_PATH];
    DWORD dwSize;

    szLogPath[0] = L'\0';
    if (!PAL_FetchConfigurationString(TRUE, REG_VAL_FUSION_LOG_PATH, szLogPath, MAX_PATH))
    {
        // fallback to default
        if (PAL_GetUserConfigurationDirectory(szLogPath, MAX_PATH))
        {
            hr = StringCbCat(szLogPath, sizeof(szLogPath), L"\\FusionLogs");
            if (FAILED(hr)) {
                return hr;
            }
        }
    }

    PathRemoveBackslashW(szLogPath);
    
    if (szLogPath[0]) {
        dwSize = lstrlenW(szLogPath)+1;
        if (*pdwSize < dwSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }
        else {
            hr = StringCchCopy(pwzLogPath, *pdwSize, szLogPath);
        }
        *pdwSize = dwSize;
    }
    else {
        *pdwSize = 0;
    }

    return hr;
}

HRESULT IsValidAssemblyOnThisPlatform(IAssemblyName *pName, BOOL bForInstall)
{
    HRESULT         hr = S_OK;
    PEKIND          dwAsmImageType = peNone;
    DWORD           cbSize = sizeof(dwAsmImageType);
    
    _ASSERTE(pName);

    // Get Assembly image type
    hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &(dwAsmImageType), &cbSize);
    if(FAILED(hr)) {
        goto Exit;
    }

    // MSIL / legacy images always allowed
    if(IsPEMSIL(dwAsmImageType) || dwAsmImageType == peNone) {
        goto Exit;
    }
    else if (IsPE32(dwAsmImageType)) {
        // always allow for install
        if (bForInstall) {
            goto Exit;
        }
        else {
            // won't allow bind to x86 while in 64 bit process.
            if (!IsProcess32()) {
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            }
            goto Exit;
        }
    }

    // Everything else, fails match
    hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

Exit:

    return hr;
}

BOOL WillAffectedByGACChange(DWORD dwCacheTypeChange, DWORD dwCacheTypeHave)
{
    switch(dwCacheTypeChange) {
        case CACHE_GAC:
            if (dwCacheTypeHave != CACHE_GAC && dwCacheTypeHave != CACHE_INVALID) 
                return FALSE;
            else 
                return TRUE;
        case CACHE_GAC_MSIL:
            if (dwCacheTypeHave == CACHE_GAC || dwCacheTypeHave == CACHE_GAC_MSIL || dwCacheTypeHave == CACHE_INVALID)
                return TRUE;
            else
                return FALSE;
        case CACHE_GAC_32:
            if (!IsProcess32()) {
                // We are a 64 process. Install 32 bit assembly should not affect anyone.
                return FALSE;
            }
            return TRUE;
        case CACHE_GAC_64:
            if (IsProcess32()) {
                // We are a 32 process. Install 64 bit assembly should not affect anyone.
                return FALSE;
            }
            return TRUE;
        default:
            return FALSE;
    }
}

HRESULT MoveAllFilesFromDir(LPCWSTR pszSrcDirPath, LPCWSTR pszDestDirPath)
{
    HRESULT hr = S_OK;
    TCHAR szDestFilePath[MAX_PATH+1];
    TCHAR szBuf[MAX_PATH+1];
    HANDLE hf = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fd;

    hr = StringCbCopy(szBuf, sizeof(szBuf), pszSrcDirPath);
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

    hr = StringCbCopy(szBuf, sizeof(szBuf), pszSrcDirPath);
    if (FAILED(hr)) {
        goto exit;
    }

    do {
        if ( (FusionCompareStringI(fd.cFileName, TEXT(".")) == 0) || 
             (FusionCompareStringI(fd.cFileName, TEXT("..")) == 0))
            continue;

        hr = StringCchPrintf(szBuf, MAX_PATH-1, TEXT("%s\\%s"), pszSrcDirPath, fd.cFileName);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCchPrintf(szDestFilePath, MAX_PATH-1, TEXT("%s\\%s"), pszDestDirPath, fd.cFileName);
        if (FAILED(hr)) {
            goto exit;
        }

        if(!WszMoveFile( szBuf, szDestFilePath)) {
            hr = HRESULT_FROM_WIN32(GetLastError());

            if( (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) )  
               || (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) ) {
                hr = S_OK;
            }
            else {
                break;
            }
        }
    } while (WszFindNextFile(hf, &fd));

    if ((hr == S_OK) && (GetLastError() != ERROR_NO_MORE_FILES))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    if (hf != INVALID_HANDLE_VALUE)
    {
        FindClose(hf);
        hf = INVALID_HANDLE_VALUE;
    }

    // after moving all files attempt to remove the source dir
    WszRemoveDirectory(pszSrcDirPath); 

exit :

    if (hf != INVALID_HANDLE_VALUE)
    {
        FindClose(hf);
        hf = INVALID_HANDLE_VALUE;
    }

    return hr;
}

HRESULT FusionMoveDirectory(LPCWSTR pszSrcDirPath, LPCWSTR pszDestDirPath)
{
    HRESULT hr = S_OK;

    if(!WszMoveFile(pszSrcDirPath, pszDestDirPath)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        if (hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)
            || hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)) {

            // looks like there are some in-use files here.
            // move all the asm files to pend del dir.
            if(!WszCreateDirectory(pszDestDirPath, NULL)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                if (hr != HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) {
                    goto Exit;
                }
                hr = S_OK;
            }

            hr = MoveAllFilesFromDir(pszSrcDirPath, pszDestDirPath);
            if(FAILED(hr)) {
                // could not move files; restore all files back to original state.
                HRESULT hrTemp;
                hrTemp = MoveAllFilesFromDir(pszDestDirPath, pszSrcDirPath);
                WszRemoveDirectory(pszDestDirPath);
            }
        }
    }
Exit:
    return hr;
}

HRESULT FusionCopyFiles(LPCWSTR pwzSrcDir, LPCWSTR pwzDestDir, BOOL fCopyDirs)
{
    HRESULT     hr = S_OK;
    WCHAR       wzSearch[MAX_PATH + 1];
    WCHAR       wzNewPath[MAX_PATH + 1];
    HANDLE      hFind = INVALID_HANDLE_VALUE;
    DWORD       dwcchDir = 0;
    WIN32_FIND_DATA FindFileData = {0};

    _ASSERTE(pwzSrcDir && pwzDestDir);

    // Create the destination directory if it doesn't exists
    hr = StringCchPrintf(wzNewPath, MAX_PATH, L"%ws\\1", pwzDestDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CreateFilePathHierarchy(wzNewPath);
    if(FAILED(hr)) {
        goto Exit;
    }

    // Get largest filename length
    dwcchDir = (DWORD) max(lstrlenW(pwzSrcDir), lstrlenW(pwzDestDir));

    hr = StringCchPrintf(wzSearch, MAX_PATH, L"%ws\\*", pwzSrcDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    hFind = WszFindFirstFile(wzSearch, &FindFileData);
    if(hFind == INVALID_HANDLE_VALUE) {
        // No files here
        hr = S_OK;
        goto Exit;
    }

    do
    {
        // skip directories    
        if(!FusionCompareStringI(FindFileData.cFileName, L".."))
            continue;

        if(!FusionCompareStringI(FindFileData.cFileName, L"."))
            continue;

        if(fCopyDirs && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            WCHAR       wzNewSrcDir[MAX_PATH + 1];
            WCHAR       wzNewDestDir[MAX_PATH + 1];

            // dir + \ + filename + null
            if(dwcchDir + 1 + lstrlenW(FindFileData.cFileName) + 1 > MAX_PATH) {
                hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                goto Exit;
            }
            
            hr = StringCchPrintf(wzNewSrcDir, MAX_PATH, L"%ws\\%ws", pwzSrcDir, FindFileData.cFileName);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = StringCchPrintf(wzNewDestDir, MAX_PATH, L"%ws\\%ws", pwzDestDir, FindFileData.cFileName);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = FusionCopyFiles(wzNewSrcDir, wzNewDestDir, fCopyDirs);
            if(FAILED(hr)) {
                goto Exit;
            }
            continue;
        }
        else {
            // Found a file, copy it to the new location

            WCHAR   wzSrcFile[MAX_PATH + 1];
            WCHAR   wzDestFile[MAX_PATH + 1];

            // dir + \ + filename + null
            if(dwcchDir + 1 + lstrlenW(FindFileData.cFileName) + 1 > MAX_PATH) {
                hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                goto Exit;
            }

            hr = StringCchPrintf(wzSrcFile, MAX_PATH, L"%ws\\%ws", pwzSrcDir, FindFileData.cFileName);
            if (FAILED(hr)) {
                goto Exit;
            }
            hr = StringCchPrintf(wzDestFile, MAX_PATH, L"%ws\\%ws", pwzDestDir, FindFileData.cFileName);
            if (FAILED(hr)) {
                goto Exit;
            }

            if(!WszCopyFile(wzSrcFile, wzDestFile, FALSE)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Exit;
            }
        }
    }while(WszFindNextFile(hFind, &FindFileData));

Exit:

    if(hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    return hr;
}

// WinSxS stuff

#define ACCESS_DENIED_RETRY_TIMES   10

HRESULT LockCacheDir(CACHE_FLAGS dwCacheFlags, LPCWSTR pwzLockFile, HANDLE *phLock)
{
    HRESULT hr = S_OK;
    DWORD  dwErr = 0;
    HANDLE hLock = INVALID_HANDLE_VALUE;
    DWORD dwWaitTime = 0;
    DWORD dwFileFlags = 0;
    DWORD dwRetry = 0;

    _ASSERTE(IsGacType(dwCacheFlags) || (dwCacheFlags == CACHE_DOWNLOAD));
    _ASSERTE(pwzLockFile);
    _ASSERTE(*pwzLockFile);
    _ASSERTE(phLock);

    if (IsGacType(dwCacheFlags)) {
        hr = CreateFilePathHierarchy(pwzLockFile);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    for (;;) {
        hLock = WszCreateFile(pwzLockFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, dwFileFlags, NULL);
       
        if (hLock != INVALID_HANDLE_VALUE) {
            *phLock = hLock;
            goto Exit;
        }

        dwErr = GetLastError();
        if ((dwErr == ERROR_SHARING_VIOLATION)
           /* fat32 does not handle FILE_FLAG_DELETE_ON_CLOSE right. 
            * On stress it returns ERROR_ACCESS_DENIED occasionally.
            * I don't want to fail because of that.
            * Only do it for download cache, 
            * since ERROR_ACCESS_DENIED is a legitimate error for GAC. 
            * See VSWhidbey 465739 for more detail.*/ 
           || ((dwErr == ERROR_ACCESS_DENIED) && !IsGacType(dwCacheFlags))) {

            /* In the case of impersonation, 
             * the current user may not have right to write a file in the 
             * starting user's profile directory. 
             * if we loop forever it will be a infinite loop deadlock. 
             * We will try limited times before we bail out. */
            if (dwErr == ERROR_ACCESS_DENIED) {
                if (dwRetry > ACCESS_DENIED_RETRY_TIMES) {
                    hr = HRESULT_FROM_WIN32(dwErr);
                    goto Exit;
                }
                dwRetry++;
            }

            // Let's sleep, and come back again.
            if (IsGacType(dwCacheFlags)) {
                dwWaitTime = FUSION_GAC_LOCK_RETRY_WAITING_TIME;
            } else {
                dwWaitTime = FUSION_DOWNLOAD_CACHE_LOCK_RETRY_WAITING_TIME;            
            }
            ClrSleepEx(dwWaitTime, FALSE);
        }
        else {
            hr = HRESULT_FROM_WIN32(dwErr);
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT UnlockCacheDir(HANDLE hLock)
{
    if (hLock != INVALID_HANDLE_VALUE) {
        CloseHandle(hLock);
    }

    return S_OK;
}

//
// ************************************************************************************
