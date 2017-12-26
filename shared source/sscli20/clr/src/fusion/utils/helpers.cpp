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
#define BYPASS_REG_CREATE_OPEN_RESTRICTION

#include <windows.h>
#include "corpriv.h"
#include "mscoree.h"
#include "strongname.h"
#include "fusionp.h"
#include "helpers.h"
#include "list.h"
#include "policy.h"
#include "naming.h"
#include "appctx.h"
#include "actasm.h"
#include "cacheutils.h"
#include "util.h"
#include "lock.h"
#include "parse.h"
#include "dbglog.h"


#define MAX_DRIVE_ROOT_LEN                     4

// Prototypes
typedef DWORD (*pfnGetSystemWindowsDirectoryW)(LPWSTR lpBuffer, UINT uSize);

typedef BOOL (*pfnGetVolumePathNameW)(LPCTSTR lpszFileName, 
                                    LPTSTR lpszVolumePathName, 
                                    DWORD cchBufferLength);

typedef BOOL (*PFNGETUSERPROFILEDIRECTORY)(HANDLE hToken, LPWSTR lpProfileDir, LPDWORD lpcchSize);

// Globals
WCHAR g_UserFolderPath[MAX_PATH+1];
BOOL  g_bUserFolderPathInited;
pfnGetVolumePathNameW  g_pfnGetVolumePathNameW;
const WCHAR g_wzInvalidAsmNameChars[] = { L':', L'/', L'\\' , L'*', L'<', L'>', L'?', L'|', L'"'};

// Externals
extern CRITSEC_COOKIE g_csSingleUse;
extern LCID g_lcid;
extern HINSTANCE g_hInst;


HRESULT AppCtxGetWrapper(IApplicationContext *pAppCtx, LPCWSTR wzTag,
                         __deref_out WCHAR **ppwzValue)
{
    HRESULT                               hr = S_OK;
    WCHAR                                *wzBuf = NULL;
    DWORD                                 cbBuf;

    if (!pAppCtx || !wzTag || !ppwzValue) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    cbBuf = 0;
    hr = pAppCtx->Get(wzTag, wzBuf, &cbBuf, 0);

    if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
        hr = S_FALSE;
        *ppwzValue = NULL;
        goto Exit;
    }

    _ASSERTE(hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));

    wzBuf = NEW(WCHAR[cbBuf]);
    if (!wzBuf) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pAppCtx->Get(wzTag, wzBuf, &cbBuf, 0);

    if (FAILED(hr)) {
        *ppwzValue = NULL;
        SAFEDELETEARRAY(wzBuf);
    }
    else {
        *ppwzValue = wzBuf;
    }

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// NameObjGetWrapper
// ---------------------------------------------------------------------------
HRESULT NameObjGetWrapper(IAssemblyName *pName, DWORD nIdx, 
    LPBYTE *ppbBuf, LPDWORD pcbBuf)
{
    HRESULT hr = S_OK;
    
    LPBYTE pbAlloc;
    DWORD cbAlloc;

    _ASSERTE(pName && ppbBuf && pcbBuf);

    // Get property size
    hr = pName->GetProperty(nIdx, NULL, &(cbAlloc = 0));
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
    {
        // Property is set; alloc buf
        pbAlloc = NEW(BYTE[cbAlloc]);
        if (!pbAlloc)
        {
            hr = E_OUTOFMEMORY;                
            goto exit;
        }

        // Get the property
        if (FAILED(hr = pName->GetProperty(nIdx, pbAlloc, &cbAlloc)))
            goto exit;
            
        *ppbBuf = pbAlloc;
        *pcbBuf = cbAlloc;
    }
    else
    {
        // If property unset, hr should be S_OK
        if (hr != S_OK)
            goto exit;

        // Success, returning 0 bytes, ensure buf is null.    
        *ppbBuf = NULL;
        *pcbBuf = 0;
    }

exit:
    return hr;
}

HRESULT GetFileLastModified(LPCWSTR pwzFileName, FILETIME *pftLastModified)
{
    HRESULT                   hr = S_OK;
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    _ASSERTE(pwzFileName && pftLastModified);

    if (!WszGetFileAttributesEx(pwzFileName, GetFileExInfoStandard, &fileInfo)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    else {
        memcpy(pftLastModified, &(fileInfo.ftLastWriteTime), sizeof(FILETIME));
    }

    return hr;
}

// GetWindowsDirectory has so many crazy gotchas....this really gets
// the windows directory, regardless of if it's NT5 w/ terminal server or
// not.

DWORD GetRealWindowsDirectory(__out_ecount(uSize) LPWSTR wszRealWindowsDir, UINT uSize)
{
    if (!PAL_GetMachineConfigurationDirectory(wszRealWindowsDir, uSize))
    {
        return FALSE;
    }        

    return lstrlenW(wszRealWindowsDir);
}

HRESULT SetAppCfgFilePath(IApplicationContext *pAppCtx, LPCWSTR wzFilePath)
{
    HRESULT                              hr = S_OK;
    CApplicationContext                 *pCAppCtx = static_cast<CApplicationContext *>(pAppCtx);   // dynamic_cast

    _ASSERTE(pAppCtx && wzFilePath);

    hr = pCAppCtx->Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pAppCtx->Set(ACTAG_APP_CFG_LOCAL_FILEPATH, (void *)wzFilePath,
                      (sizeof(WCHAR) * (lstrlenW(wzFilePath) + 1)), 0);
                      
    pCAppCtx->Unlock();

Exit:
    return hr;
}

HRESULT MakeUniqueTempDirectory(LPCWSTR wzTempDir, 
                                __out_ecount(dwLen) LPWSTR wzUniqueTempDir,
                                DWORD dwLen)
{
    int                 n = 1;
    HRESULT             hr = S_OK;
    CCriticalSection    cs(g_csSingleUse);
    static const WCHAR  wzFusion[] = L"Fusion";
    BOOL                fLocked = FALSE;
    BOOL                fExists = FALSE;

    _ASSERTE(wzTempDir && wzUniqueTempDir);

    // tempdir + fusion + 9 (3 random chars + '.tmp' + '\' + 1 null terminator)
    if(dwLen < (DWORD) (lstrlenW(wzTempDir) + lstrlenW(wzFusion) + 9)) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto Exit;
    }

    //execute entire function under critical section
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    fLocked = TRUE;

    for(;;) {
        WCHAR       wzRandomName[4];        // 3 random chars + 1 null term
        
        if(n++ > 100) {    // avoid infinite loop!
            hr = E_FAIL;
            goto Exit;
        }

        // Get 3 random characters
        wzRandomName[0] = L'\0';
        GetRandomName(wzRandomName, 3);
        
        hr = StringCchPrintf(wzUniqueTempDir, dwLen, L"%ws%ws%ws.tmp", wzTempDir, wzFusion, wzRandomName);
        if (FAILED(hr)) {
            goto Exit;
        }
        hr = CheckFileExistence(wzUniqueTempDir, &fExists, NULL);
        if(FAILED(hr)) {
            goto Exit;
        }
        else if(!fExists) {
            break;
        }
    }

    if (!WszCreateDirectory(wzUniqueTempDir, NULL)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    hr = PathAddBackslashWrap(wzUniqueTempDir, dwLen);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    if(fLocked) {
        cs.Unlock();
    }
        
    return hr;
}

HRESULT RemoveDirectoryAndChildren(LPCWSTR szDir)
{
    HRESULT hr = S_OK;
    HANDLE hf = INVALID_HANDLE_VALUE;
    TCHAR szBuf[MAX_PATH];
    WIN32_FIND_DATA fd;
    LPWSTR wzCanonicalized=NULL;
    WCHAR wzPath[MAX_PATH];
    DWORD dwSize;

    if (!szDir || !lstrlenW(szDir)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    wzPath[0] = L'\0';

    wzCanonicalized = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzCanonicalized)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwSize = MAX_URL_LENGTH;
    hr = UrlCanonicalizeW(szDir, wzCanonicalized, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = MAX_PATH;
    hr = PathCreateFromUrlW(wzCanonicalized, wzPath, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Cannot delete root. Path must have greater length than "x:\"
    if (lstrlenW(wzPath) < 4) {
        _ASSERTE(!"Cannot delete root directory!");
        hr = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
        goto Exit;
    }

    if (WszRemoveDirectory(wzPath)) {
        goto Exit;
    }

    // ha! we have a case where the directory is probbaly not empty

    hr = StringCbCopy(szBuf, sizeof(szBuf), wzPath);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = StringCbCat(szBuf, sizeof(szBuf), TEXT("\\*"));
    if (FAILED(hr)) {
        goto Exit;
    }

    if ((hf = WszFindFirstFile(szBuf, &fd)) == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    do {

        if ( (FusionCompareStringI(fd.cFileName, TEXT(".")) == 0) || 
             (FusionCompareStringI(fd.cFileName, TEXT("..")) == 0))
            continue;

        hr = StringCchPrintf(szBuf, MAX_PATH-1, TEXT("%s\\%s"), wzPath, fd.cFileName);
        if (FAILED(hr)) {
            goto Exit;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

            WszSetFileAttributes(szBuf, 
                FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_NORMAL);

            if (FAILED((hr=RemoveDirectoryAndChildren(szBuf)))) {
                goto Exit;
            }

        } else {

            WszSetFileAttributes(szBuf, FILE_ATTRIBUTE_NORMAL);
            if (!WszDeleteFile(szBuf)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Exit;
            }
        }


    } while (WszFindNextFile(hf, &fd));


    if (GetLastError() != ERROR_NO_MORE_FILES) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    if (hf != INVALID_HANDLE_VALUE) {
        FindClose(hf);
        hf = INVALID_HANDLE_VALUE;
    }

    // here if all subdirs/children removed
    /// re-attempt to remove the main dir
    if (!WszRemoveDirectory(wzPath)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

Exit:
    if(hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED))
        hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);

    if (hf != INVALID_HANDLE_VALUE)
        FindClose(hf);

    SAFEDELETEARRAY(wzCanonicalized);
    return hr;
}

HRESULT GetPDBName(LPCWSTR wzFileName,
                __out_ecount_opt(*pdwSize) LPWSTR wzPDBName, 
                __inout DWORD *pdwSize)
{
    HRESULT                          hr = S_OK;
    LPWSTR                           wzExt = NULL;
    
    _ASSERTE(wzFileName && wzPDBName && pdwSize);


    hr = StringCchCopy(wzPDBName, *pdwSize, wzFileName);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzExt = PathFindExtension(wzPDBName);
    if (lstrlenW(wzExt) < 4) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    hr = StringCchCopy(wzExt, 5, L".PDB");
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

STDAPI CopyPDBs(IAssembly *pAsm)
{
    HRESULT                                       hr = S_OK;
    IAssemblyName                                *pName = NULL;
    IAssemblyModuleImport                        *pModImport = NULL;
    DWORD                                         dwSize;
    WCHAR                                         wzAsmCachePath[MAX_PATH];
    WCHAR                                         wzFileName[MAX_PATH];
    WCHAR                                         wzSourcePath[MAX_PATH];
    WCHAR                                         wzPDBName[MAX_PATH];
    WCHAR                                         wzPDBSourcePath[MAX_PATH];
    WCHAR                                         wzPDBTargetPath[MAX_PATH];
    WCHAR                                         wzModPath[MAX_PATH];
    LPWSTR                                        wzCodebase=NULL;
    LPWSTR                                        wzModName = NULL;
    DWORD                                         dwIdx = 0;
    LPWSTR                                        wzTmp = NULL;
    BOOL                                          fExists = FALSE;

    if (!pAsm) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (pAsm->GetAssemblyLocation(NULL) == E_NOTIMPL) {
        // This is a registered "known assembly" (ie. the process EXE).
        // We don't copy PDBs for the process EXE because it's never
        // shadow copied.

        hr = S_FALSE;
        goto Exit;
    }

    // Find the source location. Make sure this is a file:// URL (ie. we
    // don't support retrieving the PDB over http://).

    hr = pAsm->GetAssemblyNameDef(&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzCodebase = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzCodebase)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzCodebase[0] = L'\0';

    dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
    hr = pName->GetProperty(ASM_NAME_CODEBASE_URL, (void *)wzCodebase, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!UrlIsW(wzCodebase, URLIS_FILEURL)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwSize = MAX_PATH;
    hr = PathCreateFromUrlWrap(wzCodebase, wzSourcePath, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzTmp = PathFindFileName(wzSourcePath);
    _ASSERTE(wzTmp > (LPWSTR)wzSourcePath);
    *wzTmp = L'\0';
        
   // Find the target location in the cache.
   
    dwSize = MAX_PATH;
    hr = pAsm->GetManifestModulePath(wzAsmCachePath, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzTmp = PathFindFileName(wzAsmCachePath);
    _ASSERTE(wzTmp > (LPWSTR)wzAsmCachePath);

    hr = StringCbCopy(wzFileName, sizeof(wzFileName), wzTmp);
    if (FAILED(hr)) {
        goto Exit;
    }
    *wzTmp = L'\0';



    dwSize = MAX_PATH;
    hr = GetPDBName(wzFileName, wzPDBName, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchPrintf(wzPDBSourcePath, MAX_PATH, L"%ws%ws", wzSourcePath, wzPDBName);
    if (FAILED(hr)) {
        goto Exit;
    }
    hr = StringCchPrintf(wzPDBTargetPath, MAX_PATH, L"%ws%ws", wzAsmCachePath, wzPDBName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CheckFileExistence(wzPDBTargetPath, &fExists, NULL);
    if(FAILED(hr)) {
        goto Exit;
    }
    else if(!fExists && FusionCompareStringI(wzPDBSourcePath, wzPDBTargetPath)) {
        WszCopyFile(wzPDBSourcePath, wzPDBTargetPath, TRUE);
    }

    // Copy the module PDBs.

    dwIdx = 0;
    while (SUCCEEDED(hr)) {
        hr = pAsm->GetNextAssemblyModule(dwIdx++, &pModImport);

        if (SUCCEEDED(hr)) {
            if (pModImport->IsAvailable()) {
                dwSize = MAX_PATH;
                hr = pModImport->GetModulePath(wzModPath, &dwSize);
                if (FAILED(hr)) {
                    SAFERELEASE(pModImport);
                    goto Exit;
                }

                wzModName = PathFindFileName(wzModPath);
                _ASSERTE(wzModName);

                dwSize = MAX_PATH;
                hr = GetPDBName(wzModName, wzPDBName, &dwSize);
                if (FAILED(hr)) {
                    SAFERELEASE(pModImport);
                    goto Exit;
                }

                hr = StringCchPrintf(wzPDBSourcePath, MAX_PATH, L"%ws%ws", wzSourcePath,
                           wzPDBName);
                if (FAILED(hr)) {
                    goto Exit;
                }
                hr = StringCchPrintf(wzPDBTargetPath, MAX_PATH, L"%ws%ws", wzAsmCachePath,
                           wzPDBName);
                if (FAILED(hr)) {
                    goto Exit;
                }

                hr = CheckFileExistence(wzPDBTargetPath, &fExists, NULL);
                if(FAILED(hr)) {
                    goto Exit;
                }
                else if(!fExists && FusionCompareStringI(wzPDBSourcePath, wzPDBTargetPath)) {
                    WszCopyFile(wzPDBSourcePath, wzPDBTargetPath, TRUE);
                }
            }

            SAFERELEASE(pModImport);
        }
    }

    // Copy complete. Return success.

    if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
        hr = S_OK;
    }

Exit:
    SAFERELEASE(pName);
    SAFEDELETEARRAY(wzCodebase);
    return hr;
}

//
//
//
//
//
BOOL VerifySignature(LPCWSTR szFilePath, LPBOOL pfWasVerified, DWORD dwFlags)
{
    DWORD                           dwFlagsOut = 0;
    BOOL                            fRet = FALSE;

    // Verify the signature
    if (!StrongNameSignatureVerification(szFilePath, dwFlags, &dwFlagsOut)) {
        goto exit;
    }

    if (pfWasVerified) {
        *pfWasVerified = ((dwFlagsOut & SN_OUTFLAG_WAS_VERIFIED) != 0);
    }

    fRet = TRUE;

exit:

    return fRet;
}

// ---------------------------------------------------------------------------
// CreateFilePathHierarchy
// ---------------------------------------------------------------------------
HRESULT CreateFilePathHierarchy( LPCOLESTR pszName )
{
    HRESULT hr=S_OK;
    LPTSTR  pszFileName;
    TCHAR   szPath[MAX_PATH];
    DWORD   dw = 0;

    // _ASSERTE (pszPath ) ;
    if (lstrlenW(pszName) >= MAX_PATH) {
        return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }
        
    hr = StringCbCopy(szPath, sizeof(szPath), pszName);
    if (FAILED(hr)) {
        return hr;
    }

    pszFileName = PathFindFileName ( szPath );

    if ( pszFileName <= szPath )
        return E_INVALIDARG; // Send some error 

    *(pszFileName-1) = 0;

    dw = WszGetFileAttributes( szPath );
    if ( dw != INVALID_FILE_ATTRIBUTES )
        return S_OK;
        
    hr = HRESULT_FROM_WIN32(GetLastError());

    switch (hr)
    {
        case HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
            {
                hr =  CreateFilePathHierarchy(szPath);
                if (hr != S_OK)
                    return hr;
            }

        case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
            {
                if ( WszCreateDirectory( szPath, NULL ) )
                    return S_OK;
                else
                {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    if(hr == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS))
                        hr = S_OK;
                    else
                        return hr;
                }
            }

        default:
            return hr;
    }
}

// ---------------------------------------------------------------------------
// Helper function to generate random name.
DWORD GetRandomName (
        __out_ecount(dwLen+1) LPTSTR szDirName, DWORD dwLen)
{
    static unsigned Counter;
    LARGE_INTEGER liRand;
    LARGE_INTEGER li;

    for (DWORD i = 0; i < dwLen; i++)
    {
        // Try using high performance counter, otherwise just use
        // the tick count
        if (QueryPerformanceCounter(&li)) {
            liRand.QuadPart = li.QuadPart + Counter++;
        }
        else {
            liRand.QuadPart = (GetTickCount() + Counter++);
        }
        BYTE bRand = (BYTE) (liRand.QuadPart % 36);

        // 10 digits + 26 letters
        if (bRand < 10)
            *szDirName++ = TEXT('0') + bRand;
        else
            *szDirName++ = TEXT('A') + bRand - 10;
    }

    *szDirName = 0;

    return dwLen; // returns length not including null
}

HRESULT GetRandomFileName(__out_ecount(dwFileName) LPTSTR pszPath, DWORD dwFileName)
{
    HRESULT hr=S_OK;
    LPTSTR  pszFileName=NULL;
    DWORD dwPathLen = 0;
    DWORD dwErr=0;

    _ASSERTE(pszPath);
    //_ASSERTE(IsPathRelative(pszPath))

    hr = StringCchCat(pszPath, MAX_PATH, TEXT("\\") );
    if (FAILED(hr)) {
        return hr;
    }
    
    dwPathLen = lstrlenW(pszPath);

    DWORD dwTotalLen = dwPathLen + dwFileName + 1;
    if (dwTotalLen<dwPathLen || dwTotalLen< dwFileName || dwTotalLen >= MAX_PATH) {
        return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }

    pszFileName = pszPath + dwPathLen;

    // Loop until we get a unique file name.
    int i;
    for (i = 0; i < MAX_RANDOM_ATTEMPTS; i++) {
        GetRandomName (pszFileName, dwFileName);
        if(WszGetFileAttributes(pszPath) != INVALID_FILE_ATTRIBUTES)
            continue;

        dwErr = GetLastError();                
        if (dwErr == ERROR_FILE_NOT_FOUND)
        {
            hr = S_OK;
            break;
        }

        if (dwErr == ERROR_PATH_NOT_FOUND)
        {
            if(FAILED(hr = CreateFilePathHierarchy(pszPath)))
                break;
            else
                continue;
        }

        hr = HRESULT_FROM_WIN32(dwErr);
        break;
    }

    if (i >= MAX_RANDOM_ATTEMPTS) {
        hr = E_UNEXPECTED;
    }

    return hr;

}
// ---------------------------------------------------------------------------
// Creates a new Dir for assemblies
HRESULT CreateDirectoryForAssembly(
        DWORD dwDirSize, 
        __out_ecount(*pcwPath) LPTSTR pszPath, 
        __inout LPDWORD pcwPath)
{
    HRESULT hr=S_OK;
    DWORD dwErr;
    DWORD cszStore;
    LPTSTR pszDir=NULL;
    DWORD dwTotalSize;

    // Check output buffer can contain a full path.
    _ASSERTE (!pcwPath || *pcwPath >= MAX_PATH);

    if (!pszPath)
    {
        *pcwPath = MAX_PATH;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto done;
    }

    cszStore = lstrlenW (pszPath);
    dwTotalSize = cszStore + dwDirSize + 1;
    if (dwTotalSize<cszStore || dwTotalSize<dwDirSize || dwTotalSize>*pcwPath) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto done;
    }
    
    pszDir = pszPath + cszStore;

    // Loop until we create a unique dir.
    int i;
    for (i = 0; i < MAX_RANDOM_ATTEMPTS; i++)
    {
        GetRandomName (pszDir, dwDirSize);

        hr = CreateFilePathHierarchy(pszPath);
        if(hr != S_OK)
            goto done;

        if (WszCreateDirectory (pszPath, NULL))
            break;
        dwErr = GetLastError();
        if (dwErr == ERROR_ALREADY_EXISTS)
            continue;
        hr = HRESULT_FROM_WIN32(dwErr);
        goto done;
    }

    if (i >= MAX_RANDOM_ATTEMPTS) {
        hr = E_UNEXPECTED;
        goto done;
    }

    *pcwPath = cszStore + dwDirSize + 1;
    hr = S_OK;

done:
    return hr;
}

HRESULT VersionFromString(LPCWSTR wzVersionIn, WORD *pwVerMajor, WORD *pwVerMinor,
                          WORD *pwVerBld, WORD *pwVerRev)
{
    HRESULT     hr = S_OK;
    const WCHAR *pchBegin = NULL;
    const WCHAR *pchEnd = NULL;
    WORD        *pawVersions[4] = {pwVerMajor, pwVerMinor, pwVerBld, pwVerRev};
    int         iDotCount = 0;
    
    if (!wzVersionIn || !pwVerMajor || !pwVerMinor || !pwVerRev || !pwVerBld) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Greater than our maximum version length disallowed
    if((DWORD) lstrlenW(wzVersionIn) > MAX_VERSION_DISPLAY_SIZE) {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
        goto Exit;
    }

    *pwVerMajor = *pwVerMinor =
        *pwVerRev = *pwVerBld = 0;

    pchBegin = pchEnd = wzVersionIn;

    //    Numerics and '.' are valid and any version part
    //    (Major, Minor, Bld, Rev) greater than 35565 is disallowed
    //    
    for(;;) {
        if(*pchEnd >= L'0' && *pchEnd <= L'9') {
            pchEnd++;
        }
        else {
            WCHAR   wzVersionValueStr[6];
            int     iValue = 0;

            // Point to same place or greater than 5 characters (i.e. '65535' )
            if(pchBegin == pchEnd || pchEnd - pchBegin > 5) {
                hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                goto Exit;
            }

            // We only accept dots and NULL
            if(*pchEnd != L'.' && *pchEnd != L'\0') {
                hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);  // Badly formatted version
                goto Exit;
            }

            // Make sure we only have 3 dots
            if(iDotCount > 2 && *pchEnd != L'\0') {
                hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);  // Badly formatted version
                goto Exit;
            }

            // Copy the string value out
            hr = StringCchCopyN(wzVersionValueStr, ARRAYSIZE(wzVersionValueStr), pchBegin, (DWORD)(pchEnd - pchBegin));
            if (FAILED(hr)) {
                goto Exit;
            }

            // Verify the value
            iValue = StrToIntW(wzVersionValueStr);
            if(iValue < 0 || iValue > 65535L) {
                hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);  // Badly formatted version
                goto Exit;
            }

            *(pawVersions[iDotCount]) = (WORD) iValue;

            // All done?
            if(*pchEnd == L'\0') {
                if(iDotCount != 3) {
                    hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);  // Badly formatted version
                    goto Exit;
                }
                break;
            }
            
            iDotCount++;
            pchEnd++;
            pchBegin = pchEnd;
        }
    }

Exit:
    // If we failed, zero out version fields
    if(FAILED(hr) && (pwVerMajor && pwVerMinor && pwVerBld && pwVerRev)) {
         *pwVerMajor = *pwVerMinor =
            *pwVerBld = *pwVerRev = 0;
    }
    
    return hr;
}

// implemented in clr\src\vm/util.cpp
BOOL GetUserDir(__out_ecount(bufferCount) WCHAR * buffer, size_t bufferCount, BOOL fRoaming);

HRESULT FusionGetUserFolderPath()
{
    if (!PAL_GetUserConfigurationDirectory(g_UserFolderPath, MAX_PATH)) {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    return S_OK;
}

DWORD HashString(LPCWSTR wzKey, DWORD dwStart, DWORD dwHashSize, BOOL bCaseSensitive)
{
    DWORD                                 dwHash = dwStart;
    LPCWSTR                               lpWChar;

    _ASSERTE(wzKey);

    if (!wzKey) {
        return dwHashSize ? (dwStart % dwHashSize) : dwStart;
    }

    for (lpWChar = wzKey ; *lpWChar; lpWChar++) {
        if (bCaseSensitive) {
            dwHash = (dwHash * 65599) + (DWORD)(*lpWChar);
        }
        else {
            dwHash = (dwHash * 65599) + (DWORD)TOLOWER(*lpWChar);
        }
    }

    if (dwHashSize) {
        dwHash %= dwHashSize;
    }

    return dwHash;
}

DWORD HashBlob(LPBYTE pbKey, DWORD dwLen, DWORD dwStart, DWORD dwHashSize)
{
    DWORD   i;
    DWORD   dwHash = dwStart;

    _ASSERTE(pbKey);

    for (i = 0; i < dwLen; i++) {
        dwHash = (dwHash * 65599) + pbKey[i];
    }

    if (dwHashSize) {
        dwHash %= dwHashSize;
    }

    return dwHash;
}

HRESULT ExtractXMLAttribute(__deref_out LPWSTR *ppwzValue, XML_NODE_INFO **aNodeInfo,
                            USHORT *pCurIdx, USHORT cNumRecs)
{
    HRESULT                                  hr = S_OK;

    _ASSERTE(ppwzValue && aNodeInfo && pCurIdx && cNumRecs);

    _ASSERTE(!*ppwzValue);

    (*pCurIdx)++;
    while (*pCurIdx < cNumRecs) {
        
        if (aNodeInfo[*pCurIdx]->dwType == XML_PCDATA ||
            aNodeInfo[*pCurIdx]->dwType == XML_ENTITYREF) {

            hr = AppendString(ppwzValue, aNodeInfo[*pCurIdx]->pwcText,
                              aNodeInfo[*pCurIdx]->ulLen);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        else {
            // Reached end of data
            break;
        }

        (*pCurIdx)++;

        if (*ppwzValue && !(*ppwzValue)[0]) {
            SAFEDELETEARRAY(*ppwzValue);
        }
    }

Exit:
    if (FAILED(hr)) {
        SAFEDELETEARRAY(*ppwzValue);
    }
    return hr;
}

HRESULT AppendString(__deref_inout LPWSTR *ppwzHead, LPCWSTR pwzTail, DWORD dwLen)
{
    HRESULT                                    hr = S_OK;
    LPWSTR                                     pwzBuf = NULL;
    DWORD                                      dwLenBuf;
    
    _ASSERTE(ppwzHead && pwzTail);

    if (!*ppwzHead) {
        *ppwzHead = NEW(WCHAR[dwLen + 1]);

        if (!*ppwzHead) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
 
        hr = StringCchCopyN(*ppwzHead, dwLen + 1, pwzTail, dwLen);
        if (FAILED(hr)) {
            SAFEDELETEARRAY(*ppwzHead);
            goto Exit;
        }
    }
    else {
        dwLenBuf = lstrlenW(*ppwzHead) + dwLen + 1;

        pwzBuf = NEW(WCHAR[dwLenBuf]);
        if (!pwzBuf) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCchCopy(pwzBuf, dwLenBuf, *ppwzHead);
        if (FAILED(hr)) {
            SAFEDELETEARRAY(pwzBuf);
            goto Exit;
        }
        
        hr = StringCchCatN(pwzBuf, dwLenBuf, pwzTail, dwLen-1);
        if (FAILED(hr)) {
            SAFEDELETEARRAY(pwzBuf);
            goto Exit;
        }

        SAFEDELETEARRAY(*ppwzHead);

        *ppwzHead = pwzBuf;
    }

Exit:
    return hr;
}

LPWSTR GetNextDelimitedString(__deref_inout_z LPWSTR *ppwzList, WCHAR wcDelimiter)
{
    LPWSTR                         wzCurString = NULL;
    LPWSTR                         wzPos = NULL;

    if (!ppwzList) {
        goto Exit;
    }

    wzCurString = *ppwzList;
    wzPos = *ppwzList;

    while (*wzPos && *wzPos != wcDelimiter) {
        wzPos++;
    }

    if (*wzPos == wcDelimiter) {
        // Found a delimiter
        *wzPos = L'\0';
        *ppwzList = (wzPos + 1);
    }
    else {
        // End of string
        *ppwzList = NULL;
    }

Exit:
    return wzCurString;
}

LPWSTR g_pwzCORVersion = NULL;

HRESULT GetCORVersion()
{
    HRESULT     hr = S_OK;
    LPWSTR      pwzCORVersion = NULL;
    DWORD       dwLen = 0;
    void       *pwzOldVersion = NULL;

    if (g_pwzCORVersion) {
        return S_OK;
    }

    GetCORVersion(NULL, 0, &dwLen);
    _ASSERTE(dwLen);

    pwzCORVersion = NEW(WCHAR[dwLen]);
    if (!pwzCORVersion) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = GetCORVersion(pwzCORVersion, dwLen, &dwLen);
    if (FAILED(hr)) {
        goto Exit;
    }

    pwzOldVersion = InterlockedCompareExchangePointer((void **)&g_pwzCORVersion, pwzCORVersion, NULL);

    if (pwzOldVersion) { //oops, we lost
        SAFEDELETEARRAY(pwzCORVersion);
    }
    
Exit:
    if (FAILED(hr)) {
        SAFEDELETEARRAY(pwzCORVersion);
    }

    return hr;
}



DWORD GetFileSizeInKB(DWORD dwFileSizeLow, DWORD dwFileSizeHigh)
{
    const ULONG dwKBMask = (1023); // 1024-1
    ULONG   dwFileSizeInKB = dwFileSizeLow >> 10 ; // strip of 10 LSB bits to convert from bytes to KB.

    if(dwKBMask & dwFileSizeLow)
        dwFileSizeInKB++; // Round up to the next KB.

    if(dwFileSizeHigh)
        dwFileSizeInKB += (dwFileSizeHigh * (1 << 22) );

    return dwFileSizeInKB;
}



HRESULT GetManifestFileLock(LPCWSTR pszFilename, HANDLE *phFile)
{
    HRESULT                                hr = S_OK;
    HANDLE                                 hFile = INVALID_HANDLE_VALUE;
    DWORD                                  dwShareMode = FILE_SHARE_READ;

    _ASSERTE(pszFilename);

        dwShareMode |= FILE_SHARE_DELETE;        

    hFile = WszCreateFile(pszFilename, GENERIC_READ, dwShareMode, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    if(phFile)
    {
        *phFile = hFile;
        hFile = INVALID_HANDLE_VALUE;
    }

exit:

    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    return hr;

}

DWORD PAL_IniReadStringEx(HINI hIni, LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                 LPWSTR *ppwzReturnedString)
{
    DWORD                                        dwRet;
    LPWSTR                                       pwzBuf = NULL;
    int                                          iSizeCur = INI_READ_BUFFER_SIZE;


    for (;;) {
        pwzBuf = NEW(WCHAR[iSizeCur]);
        if (!pwzBuf) {
            dwRet = 0;
            *ppwzReturnedString = NULL;
            goto Exit;
        }
    
        dwRet = PAL_IniReadString(hIni, lpAppName, lpKeyName, 
                                        pwzBuf, iSizeCur);
        if (lpAppName && lpKeyName && dwRet == (DWORD) iSizeCur - 1) {
            SAFEDELETEARRAY(pwzBuf);
            iSizeCur += INI_READ_BUFFER_SIZE;
        }
        else if ((!lpAppName || !lpKeyName) && dwRet == (DWORD) iSizeCur - 2) {
            SAFEDELETEARRAY(pwzBuf);
            iSizeCur += INI_READ_BUFFER_SIZE;
        }
        else {
            break;
        }
    }

    *ppwzReturnedString = pwzBuf;

Exit:
    return dwRet;
}

static const BYTE GuidMap[] = { 3, 2, 1, 0, '-', 5, 4, '-', 7, 6, '-',
    8, 9, '-', 10, 11, 12, 13, 14, 15};

static const BYTE GuidMap2[] = { 3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11,
    12, 13, 14, 15};

static const WCHAR wszDigits[] = L"0123456789ABCDEF";

void FusionFormatGUID(const GUID *guid, __out_ecount(cchSize) LPWSTR pszBuf, DWORD cchSize, BOOL bUseBrace)
{

    _ASSERTE(pszBuf && cchSize);
    _ASSERTE(bUseBrace? (cchSize >= sizeof(GUID) * 2 + 7) : (cchSize >= sizeof(GUID) * 2 + 1));

    DWORD i;
    LPWSTR p = pszBuf;

    const BYTE * pBytes = (const BYTE *) guid;

    if (bUseBrace) {
        *p++ = L'{';
        for (i = 0; i < ARRAYSIZE(GuidMap); i++) {
            if (GuidMap[i] == '-') {
                *p++ = L'-';
            }
            else {
                *p++ = wszDigits[ (pBytes[GuidMap[i]] & 0xF0) >> 4 ];
                *p++ = wszDigits[ (pBytes[GuidMap[i]] & 0x0F) ];
            }
        }
        *p++ = L'}';
        *p   = L'\0';
    }
    else {
        CParseUtils::BinToUnicodeHex(pBytes, sizeof(GUID), pszBuf);
    }
}

BOOL PathIsRelativeWrap(LPCWSTR pwzPath)
{
    BOOL                             bRet = FALSE;
    
    _ASSERTE(pwzPath);

    if (pwzPath[0] == L'\\' || pwzPath[0] == L'/') {
        goto Exit;
    }

    if (PathIsURLW(pwzPath)) {
        goto Exit;
    }

    bRet = PathIsRelativeW(pwzPath);

Exit:
    return bRet;
}

//
// URL Combine madness from shlwapi:
//
//   \\server\share\ + Hello%23 = file://server/share/Hello%23 (left unescaped)
//   d:\a b\         + bin      = file://a%20b/bin
//
        
HRESULT UrlCombineUnescape(LPCWSTR pszBase, LPCWSTR pszRelative,
       __out_ecount(*pcchCombined) LPWSTR pszCombined, 
       __inout LPDWORD pcchCombined, DWORD dwFlags)
{
    HRESULT                                   hr = S_OK;
    DWORD                                     dwSize;
    LPWSTR                                    pwzCombined = NULL;
    LPWSTR                                    pwzFileCombined = NULL;

    pwzCombined = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzCombined) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    // If we're just combining an absolute file path to an relative file
    // path, do this by concatenating the strings, and canonicalizing it.
    // This avoids UrlCombine randomness where you could end up with
    // a partially escaped (and partially unescaped) resulting URL!

    if (!PathIsURLW(pszBase) && PathIsRelativeWrap(pszRelative)) {
        pwzFileCombined = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzFileCombined) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCchPrintf(pwzFileCombined, MAX_URL_LENGTH, L"%ws%ws", pszBase, pszRelative);
        if (FAILED(hr)) {
            goto Exit;
        }

        // UrlCanonicalizeUnescape will remove extra slashes as well
        hr = UrlCanonicalizeUnescape(pwzFileCombined, pszCombined, pcchCombined, 0);
        if(FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        dwSize = MAX_URL_LENGTH;
        hr = UrlCombineW(pszBase, pszRelative, pwzCombined, &dwSize, dwFlags);
        if (FAILED(hr)) {
            goto Exit;
        }

        // Don't unescape if the relative part was already an URL because
        // URLs wouldn't have been escaped during the UrlCombined.

        if (UrlIsW(pwzCombined, URLIS_FILEURL)) {
            hr = UrlUnescapeW(pwzCombined, pszCombined, pcchCombined, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        else {
            if (*pcchCombined >= dwSize) {
                hr = StringCchCopy(pszCombined, *pcchCombined, pwzCombined);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            *pcchCombined = dwSize;
        }
    }

    // Remove any extra slashes, returns S_FALSE if nothing changed
    hr = UrlRemoveExtraBackSlashes(pszCombined);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = S_OK;

Exit:
    SAFEDELETEARRAY(pwzCombined);
    SAFEDELETEARRAY(pwzFileCombined);

    return hr;
}

HRESULT UrlCanonicalizeUnescape(LPCWSTR pszUrl, 
        __out_ecount(*pcchCanonicalized) LPWSTR pszCanonicalized, 
        __inout LPDWORD pcchCanonicalized, DWORD dwFlags)
{
    HRESULT     hr = S_OK;
    WCHAR       wzCanonical[MAX_URL_LENGTH];
    DWORD       dwSize = 0;
    LPWSTR      pwzCur = NULL;

    *wzCanonical = L'\0';

    if(UrlIsW(pszUrl, URLIS_FILEURL) || !PathIsURLW(pszUrl)) {
        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeW(pszUrl, wzCanonical, &dwSize, dwFlags);
        if(FAILED(hr)) {
            goto Exit;
        }

        hr = UrlUnescapeW(wzCanonical, pszCanonicalized, pcchCanonicalized, 0);
    }
    else {
        hr = UrlCanonicalizeW(pszUrl, pszCanonicalized, pcchCanonicalized, dwFlags /*| URL_ESCAPE_PERCENT*/);
    }

    if(FAILED(hr)) {
        goto Exit;
    }

    // Canonicalization not guaranteed to convert \ into / characters!
    //
    // Ex.
    //    1) c:\#folder\web\bin/foo.dll
    //           -> file:///c:/#folder\web\bin/foo.dll (?!)
    //    2) c:\Afolder\web\bin/foo.dll
    //           -> file:///c:/Afolder/web/bin/foo.dll
    //    3) c:\A#older\web\bin/foo.dll
    //           -> file:///c:/A%23older/web/bin/foo.dll
    
    pwzCur = pszCanonicalized;
    
    while (*pwzCur) {
        if (*pwzCur == L'\\') {
            *pwzCur = L'/';
        }

        pwzCur++;
    }

    // Remove any extra slashes, returns S_FALSE if nothing changed
    hr = UrlRemoveExtraBackSlashes(pszCanonicalized);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = S_OK;

Exit:
    return hr;
}


HRESULT PathCreateFromUrlWrap(LPCWSTR pszUrl, __out_ecount(*pcchPath) LPWSTR pszPath, LPDWORD pcchPath, DWORD dwFlags)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dw;
    WCHAR                                       wzEscaped[MAX_URL_LENGTH];

    if (!UrlIsW(pszUrl, URLIS_FILEURL)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    dw = MAX_URL_LENGTH;
    hr = UrlEscapeW(pszUrl, wzEscaped, &dw, URL_ESCAPE_PERCENT);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = PathCreateFromUrlW(wzEscaped, pszPath, pcchPath, dwFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

#define FILE_URL_PREFIX              L"file://"

LPWSTR StripFilePrefix(__inout_z LPWSTR pwzURL)
{
    LPWSTR                         szCodebase = pwzURL;

    _ASSERTE(pwzURL);

    if(!FusionCompareStringNI(szCodebase, FILE_URL_PREFIX, lstrlenW(FILE_URL_PREFIX))) {
        szCodebase += lstrlenW(FILE_URL_PREFIX);

        if (*(szCodebase + 1) == L':') {
            
            goto Exit;
        }

#if !defined(PLATFORM_UNIX)
        if (*szCodebase == L'/') {
            szCodebase++;
        }
        else {
            // UNC Path, go back two characters to preserve \\

            szCodebase -= 2;

            LPWSTR    pwzTmp = szCodebase;

            while (*pwzTmp) {
                if (*pwzTmp == L'/') {
                    *pwzTmp = L'\\';
                }

                pwzTmp++;
            }
        }
#else
        // Unix doesn't have a distinction between local and network path
        _ASSERTE(*szCodebase == L'/' || *szCodebase == L'\\');
#endif
    }

Exit:
    return szCodebase;
}

HRESULT CheckFileExistence(LPCWSTR pwzFile, BOOL *pbExists, LPDWORD pdwAttrib)
{
    HRESULT                               hr = S_OK;
    DWORD                                 dw;

    _ASSERTE(pwzFile && pbExists);

    if(pdwAttrib) {
        *pdwAttrib = 0;
    }

    dw = WszGetFileAttributes(pwzFile);
    if (dw == INVALID_FILE_ATTRIBUTES) {
        hr = HRESULT_FROM_WIN32(GetLastError());

        if ((hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) 
         || (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))) {
            *pbExists = FALSE;
            hr = S_OK;
        }

        goto Exit;
    }

    if(pdwAttrib) {
        *pdwAttrib = dw;
    }
    
    *pbExists = TRUE;

Exit:
    return hr;
}
    
#define IS_UPPER_A_TO_Z(x) (((x) >= L'A') && ((x) <= L'Z'))
#define IS_LOWER_A_TO_Z(x) (((x) >= L'a') && ((x) <= L'z'))
#define IS_0_TO_9(x) (((x) >= L'0') && ((x) <= L'9'))
#define CAN_SIMPLE_UPCASE(x) (((x)&~0x7f) == 0)
#define SIMPLE_UPCASE(x) (IS_LOWER_A_TO_Z(x) ? ((x) - L'a' + L'A') : (x))

WCHAR FusionMapChar(WCHAR wc)
{
    WCHAR                     wTmp;

    wTmp = toupper(wc);

    return wTmp;
}

int FusionCompareStringNI(LPCWSTR pwz1, LPCWSTR pwz2, int nChar)
{
    return FusionCompareStringN(pwz1, pwz2, nChar, FALSE);
}


// if nChar < 0, compare the whole string
int FusionCompareStringN(LPCWSTR pwz1, LPCWSTR pwz2, int nChar, BOOL bCaseSensitive)
{
    int                               iRet = 0;
    int                               nCount = 0;
    WCHAR                             ch1;
    WCHAR                             ch2;
    _ASSERTE(pwz1 && pwz2);

    // same point always return equal.
    if (pwz1 == pwz2) {
        return 0;
    }

    // Case sensitive comparison 
    if (bCaseSensitive) {
        if (nChar >= 0)
            return StrCmpN(pwz1, pwz2, nChar);
        else
            return StrCmp(pwz1, pwz2);
    }

    
    for (;;) {
        ch1 = *pwz1++;
        ch2 = *pwz2++;

        if (ch1 == L'\0' || ch2 == L'\0') {
            break;
        }
        
        // We use OS mapping table 
        ch1 = (CAN_SIMPLE_UPCASE(ch1)) ? (SIMPLE_UPCASE(ch1)) : (FusionMapChar(ch1));
        ch2 = (CAN_SIMPLE_UPCASE(ch2)) ? (SIMPLE_UPCASE(ch2)) : (FusionMapChar(ch2));
        nCount++;

        if (ch1 != ch2 || (nChar >= 0 && nCount >= nChar)) {
            break;
        }
    }

    if (ch1 > ch2) {
        iRet = 1;
    }
    else if (ch1 < ch2) {
        iRet = -1;
    }

    return iRet; 
}

int FusionCompareStringI(LPCWSTR pwz1, LPCWSTR pwz2)
{
    return FusionCompareStringN(pwz1, pwz2, -1, FALSE);
}

int FusionCompareString(LPCWSTR pwz1, LPCWSTR pwz2, BOOL bCaseSensitive)
{
    return FusionCompareStringN(pwz1, pwz2, -1, bCaseSensitive);
}

// Base 32 encoding uses most letters, and all numbers. Some letters are
// removed to prevent accidental generation of offensive words.
//
// Translates 5 8-bit sequences into 8 5-bit sequences.

static WCHAR g_achBase32[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9',
                               L'A', L'B', L'C', L'D', L'E', L'G', L'H', L'J', L'K', L'L',
                               L'M', L'N', L'O', L'P', L'Q', L'R', L'T', L'V', L'W', L'X',
                               L'Y', L'Z' };

HRESULT Base32Encode(BYTE *pbData, DWORD cbData, __deref_out LPWSTR *ppwzBase32)
{
    HRESULT                                hr = S_OK;
    DWORD                                  dwSizeBase32String;
    LPWSTR                                 pwzBase32 = NULL;
    LPWSTR                                 pwzCur = NULL;
    int                                    shift = 0;
    ULONG                                  accum = 0;
    ULONG                                  value;
    DWORD                                  dwRemainder;


    if (!pbData || !ppwzBase32) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppwzBase32 = NULL;

    // Figure out size of resulting string

    dwSizeBase32String = (cbData / 5) * 8;
    dwRemainder = cbData % 5;

    if (dwRemainder) {
        // A little more than we need (we can pad with '=' like in base64,
        // but since we don't need to decode, why bother).

        dwSizeBase32String += 8;
    }

    dwSizeBase32String++;

    pwzBase32 = NEW(WCHAR[dwSizeBase32String]);
    if (!pwzBase32) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    memset(pwzBase32, 0, dwSizeBase32String * sizeof(WCHAR));

    pwzCur = pwzBase32;

    //
    // 12345678 ABCDEF12
    //
    // You are processing up to two bytes at a time. The "shift" represents
    // the number of bits from the previously processed byte that haven't
    // been accounted for yet. That is, it's the number of bits that have
    // been read into the accumulator, but not processed yet.
    //

    while (cbData) {
        // Move current byte into low bits of accumulator

        accum = (accum << 8) | *pbData++;
        shift += 8;
        --cbData;


        while (shift >= 5) {
            // By subtracting five from the number of unprocessed
            // characters remaining, and shifting the accumulator
            // by that amount, we are essentially shifting all but
            // 5 characters (the top most bits that we want). 
            shift -= 5;
            value = (accum >> shift) & 0x1FL;
            *pwzCur++ = g_achBase32[value];
        }
    }

    // If shift is non-zero here, there's less than five bits remaining.
    // Pad this with zeros.

    if (shift) {
        value = (accum << (5 - shift)) & 0x1FL;
        *pwzCur++ = g_achBase32[value];
    }

    *ppwzBase32 = pwzBase32;

Exit:
    if (FAILED(hr)) {
        SAFEDELETEARRAY(pwzBase32);
    }

    return hr;
}

HRESULT PathAddBackslashWrap(__inout_ecount(dwMaxLen) LPWSTR pwzPath, DWORD dwMaxLen)
{
    HRESULT                        hr = S_OK;
    DWORD                          dwLen;

    _ASSERTE(pwzPath);

    dwLen = lstrlenW(pwzPath) + 2;

    if (dwLen > dwMaxLen) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    PathAddBackslashW(pwzPath);

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// Removes extra '/' from a URL path
// ---------------------------------------------------------------------------
HRESULT UrlRemoveExtraBackSlashes(__inout_z LPWSTR pwzUrlPath)
{
    HRESULT     hr = S_FALSE;
    LPWSTR      pwzCurPos = NULL;
    DWORD       dwSlash = 0;

    if(!pwzUrlPath || !lstrlenW(pwzUrlPath)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Is is a FILE:// Url?
    if(!UrlIs(pwzUrlPath, URLIS_FILEURL)) {
        goto Exit;
    }

    pwzUrlPath += lstrlenW(FILE_URL_PREFIX);
    pwzCurPos = pwzUrlPath;

    while(*pwzUrlPath) {
        if(*pwzUrlPath == L'/') {
            dwSlash++;
        }
        else {
            dwSlash = 0;
        }

        if(dwSlash < 2) {
            *pwzCurPos = *pwzUrlPath;
            pwzCurPos++;
        }

        pwzUrlPath++;
    }

    // Changed?
    if(pwzCurPos != pwzUrlPath) {
        *pwzCurPos = L'\0';
        hr = S_OK;
    }

Exit:
    return hr;
}

HRESULT ConvertStringToArrayPtrs(
  LPCWSTR wzModuleList,
  __deref_out LPWSTR **ppwzArray,
  WCHAR wSeperatorChar,
  LPDWORD pdwModCount)
{
    HRESULT hr = S_OK;
    LPWSTR *pwzArray = NULL;
    LPCWSTR pBegin;
    LPCWSTR pEnd;
    DWORD dwModCount = 0;

    _ASSERTE(wzModuleList && wzModuleList[0]);
    _ASSERTE(ppwzArray);
    _ASSERTE(pdwModCount);

    dwModCount = 1;
    pBegin = wzModuleList;

    while(*pBegin) {
        pEnd = wcschr(pBegin, wSeperatorChar);

        if(!pEnd) {
            break;
        }

        pBegin = pEnd + 1;
        dwModCount++;
    }

    pwzArray = NEW(LPWSTR[dwModCount]);
    if(!pwzArray) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    memset(pwzArray, 0, sizeof(LPWSTR) * dwModCount);

    pBegin = wzModuleList;

    for(DWORD dwCount=0; dwCount < dwModCount; dwCount++) {
        LPWSTR pNewBuf;
        DWORD dwLen;

        pEnd = wcschr(pBegin, wSeperatorChar);
        if(!pEnd) {
            pEnd = pBegin + lstrlenW(pBegin);
        }

        dwLen = (DWORD) (pEnd - pBegin) + 1;
        pNewBuf = NEW(WCHAR[dwLen]);
        if(!pNewBuf) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        
        hr = StringCchCopyN(pNewBuf, dwLen, pBegin, dwLen-1);
        if (FAILED(hr)) {
            goto Exit;
        }

        pwzArray[dwCount] = pNewBuf;

        pBegin = pEnd + 1;
    }

    *ppwzArray = pwzArray;
    *pdwModCount = dwModCount;

Exit:
    if(FAILED(hr) && pwzArray) {
        for(DWORD x=0; x<dwModCount; x++) {
            SAFEDELETEARRAY(pwzArray[x]);
        }
        SAFEDELETEARRAY(pwzArray);
    }

    return hr;
}

HRESULT ConvertArrayPtrsToString(
  LPCWSTR *pwzArrayStr,
  DWORD dwModCount,
  WCHAR wSeperatorChar,
  __deref_out LPWSTR *ppwzString,
  LPDWORD pdwLen)
{
    HRESULT hr = S_OK;
    DWORD dwStrLen = 0;
    DWORD dwCnt;

    _ASSERTE(pwzArrayStr);
    _ASSERTE(dwModCount);
    _ASSERTE(ppwzString);

    // Get the length of all strings
    for(dwCnt = 0; dwCnt < dwModCount; dwCnt++) {
        dwStrLen += lstrlenW(pwzArrayStr[dwCnt]);
    }

    // Add space for char seperators
    dwStrLen += dwModCount;
    *ppwzString = NEW(WCHAR[dwStrLen]);

    if(!*ppwzString) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    memset(*ppwzString, 0, dwStrLen * sizeof(WCHAR));
    dwCnt = 0;

    // Copy strings to new buffer
    while(dwCnt < dwModCount) {
        hr = StringCchCat(*ppwzString, dwStrLen, pwzArrayStr[dwCnt]);
        if (FAILED(hr)) {
            goto Exit;
        }

        dwCnt++;

        if(dwCnt != dwModCount) {
            (*ppwzString)[lstrlenW(*ppwzString)] = wSeperatorChar;
        }
    }

    if (pdwLen) {
        *pdwLen = dwStrLen;
    }

Exit:
    return hr;
}

// A better way to get the display name of a custom assembly.
HRESULT LoggingGetDisplayName(IAssemblyName *pName, 
        __out_ecount(*pdwLen) LPWSTR pwzBuf, 
        __inout DWORD *pdwLen, 
        BOOL bUseLoaderCallBack)
{
    HRESULT hr = S_OK;
    WCHAR  wzName[MAX_PATH];
    WCHAR  wzCustom[MAX_PATH];
    WCHAR  wzZapInfo[MAX_PATH];
    DWORD  dwSize;
    DWORD  dwTotalSize = 0;
    DWORD  dwFlags = 0;
    BOOL   bIsCustom;

    _ASSERTE(pName && pdwLen);

    wzName[0] = L'\0';
    wzCustom[0] = L'\0';
    wzZapInfo[0] = L'\0';

    dwFlags = ASM_DISPLAYF_VERSION
            | ASM_DISPLAYF_CULTURE
            | ASM_DISPLAYF_PUBLIC_KEY_TOKEN
            | ASM_DISPLAYF_RETARGET;

    bIsCustom = CAssemblyName::IsCustom(pName);

    if (!bUseLoaderCallBack && bIsCustom) {
        dwFlags |= ASM_DISPLAYF_CUSTOM|ASM_DISPLAYF_CONFIG_MASK;
    }

    dwSize = MAX_PATH;
    IfFailGo(pName->GetDisplayName(wzName, &dwSize, dwFlags));

    if (dwSize == 0) {
        // no name
        *pdwLen = 0;
        goto ErrExit;
    }

    dwTotalSize = dwSize;


    if (*pdwLen < dwTotalSize) {
        *pdwLen = dwTotalSize;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto ErrExit;
    }

    hr = StringCchCopy(pwzBuf, *pdwLen, wzName);
    if (FAILED(hr)) {
        goto ErrExit;
    }

    if (wzZapInfo[0] != L'\0') {
        hr = StringCchCat(pwzBuf, *pdwLen, L"");
        if (FAILED(hr)) {
            goto ErrExit;
        }

        hr = StringCchCat(pwzBuf, *pdwLen, wzZapInfo);
        if (FAILED(hr)) {
            goto ErrExit;
        }
    }

    *pdwLen = dwSize;

ErrExit:
    return hr;
}


// operate on the input string!!!
void PathConvertBackSlashesToUnderScore(__inout_z LPWSTR pwzPath)
{
    if (!pwzPath) {
        return;
    }

    for (LPWSTR pwzStr = pwzPath; *pwzStr; pwzStr++) {
        if (*pwzStr == L'\\') {
            *pwzStr = L'_';
        }
    }
}

void ReplaceInvalidFileNameCharactersToUnderscore(__inout_z LPWSTR pwzPath)
{
    if (!pwzPath) {
        return;
    }

    for (LPWSTR pwzStr = pwzPath; *pwzStr; pwzStr++) {
        for(DWORD iCnt = 0; iCnt < ARRAYSIZE(g_wzInvalidAsmNameChars); iCnt++) {
            if(*pwzStr == g_wzInvalidAsmNameChars[iCnt]) {
                *pwzStr = L'_';
            }
        }
    }
}

HRESULT ValidateAsmInstallFolderChars(LPCWSTR pszFolderName)
{
    HRESULT hr = S_OK;
    DWORD   iCnt;

    if(!pszFolderName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    for(iCnt = 0; iCnt < ARRAYSIZE(g_wzInvalidAsmNameChars); iCnt++) {
        if(StrChrW(pszFolderName, g_wzInvalidAsmNameChars[iCnt])) {
            hr = FUSION_E_INVALID_NAME;
            break;
        }
    }

Exit:

    return hr;
}

HRESULT MapProcessorArchitectureToPEKIND(LPCWSTR pwzProcArch, PEKIND *pe)
{
    PEKIND      PeKind = peNone;
    HRESULT     hr = S_OK;

    _ASSERTE(pwzProcArch && pe);

    if(!FusionCompareStringI(pwzProcArch, g_wzMSIL)) {
        PeKind = peMSIL;
    }
    else if(!FusionCompareStringI(pwzProcArch, g_wzX86)) {
        PeKind = peI386;
    }
    else if(!FusionCompareStringI(pwzProcArch, g_wzIA64)) {
        PeKind = peIA64;
    }
    else if(!FusionCompareStringI(pwzProcArch, g_wzAMD64)) {
        PeKind = peAMD64;
    }

    if (PeKind == peNone){
        hr = FUSION_E_INVALID_NAME;
    }

    *pe = PeKind;
    return hr;
}

PEKIND MapCacheTypeToPEKIND(DWORD dwCacheType)
{
    return peMSIL;
}

LPCWSTR ProcessorArchitectureFromPEKIND(PEKIND pe)
{
    switch (pe) {
        case peMSIL:
            return g_wzMSIL;
        case peI386:
            return g_wzX86;
        case peIA64:
            return g_wzIA64;
        case peAMD64:
            return g_wzAMD64;
        case peNone:
            return L"Legacy";
        default:
            return L"Unknown";
    }
}


WCHAR FusionMapCharLower(WCHAR wc)
{
    WCHAR                     wTmp;

    wTmp = toupper(wc);

    return wTmp;
}

#define CAN_SIMPLE_LOWERCASE(x) (((x)&~0x7f) == 0)
#define SIMPLE_LOWERCASE(x) (IS_UPPER_A_TO_Z(x) ? ((x) - L'A' + L'a') : (x))

HRESULT CanonicalizeIdentity(__inout_z LPWSTR pwzIdentity)
{
    HRESULT                                       hr = S_OK;
    LPWSTR                                        pwzCur = pwzIdentity;

    _ASSERTE(pwzIdentity);

    while (*pwzCur) {
        *pwzCur = (CAN_SIMPLE_LOWERCASE(*pwzCur)) ? (SIMPLE_LOWERCASE(*pwzCur)) : (FusionMapCharLower(*pwzCur));
        pwzCur++;
    }

    return hr;
}
    
HRESULT IsSameCodebase(IAssembly *pAsm, LPCWSTR wzSourceURL, BOOL *bSame)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzCodebase = NULL;
    IAssemblyName *pName = NULL;
    DWORD   dwLen = 0;
    WCHAR   wzPath[MAX_PATH];

    // check to see if we have the same codebase or not. 
    pwzCodebase = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzCodebase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pAsm->GetAssemblyNameDef(&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwLen = MAX_URL_LENGTH * sizeof(WCHAR);
    hr = pName->GetProperty(ASM_NAME_CODEBASE_URL, (LPBYTE)pwzCodebase, &dwLen);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!dwLen) {
        // no codebase url? Try file path. 
        dwLen = MAX_PATH;
        hr = pAsm->GetManifestModulePath(wzPath, &dwLen);
        if (FAILED(hr)) {
            goto Exit;
        }

        dwLen = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(wzPath, pwzCodebase, &dwLen, 0);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    
    _ASSERTE(pwzCodebase[0]);

    if (!FusionCompareStringI(wzSourceURL, pwzCodebase)) {
        *bSame = TRUE;
    }
    else {
        *bSame = FALSE;
    }
        
Exit:
    SAFEDELETEARRAY(pwzCodebase);
    SAFERELEASE(pName);
    return hr;
}

CAssemblyIdentityCache::CAssemblyIdentityCache() 
{
    _ppAsmName = NULL;
    _ppAsmTextualIdentity = NULL;
    _Size = 0;
}

HRESULT CAssemblyIdentityCache::Create(CAssemblyIdentityCache **ppAsmCache, ULONG uSize) 
{
    CAssemblyIdentityCache *pCache = NULL;
    HRESULT hr = S_OK;
    
    _ASSERTE(ppAsmCache);
    _ASSERTE(uSize > 0);    
    *ppAsmCache = NULL;
    
    pCache = NEW(CAssemblyIdentityCache);
    if (!pCache) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    hr = pCache->Init(uSize);
    if (FAILED(hr)) {
        SAFEDELETE(pCache);
        goto Exit;
    }
    
    *ppAsmCache = pCache;
Exit: 
    return hr;        
}

HRESULT CAssemblyIdentityCache::Init(ULONG uSize) 
{
    HRESULT hr = S_OK;

    _Size = uSize;    
    _ppAsmName = NEW(LPASSEMBLYNAME[_Size]);
    if (!_ppAsmName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    for (ULONG i = 0; i<_Size; i++) {
        _ppAsmName[i] = NULL;
    }

    _ppAsmTextualIdentity = NEW(LPWSTR[_Size]);
    if (!_ppAsmTextualIdentity) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    for (ULONG i = 0; i<_Size; i++) {
        _ppAsmTextualIdentity[i] = NULL;
    }
    
Exit:
    return hr;
}

CAssemblyIdentityCache::~CAssemblyIdentityCache() 
{
    if (_ppAsmName) {
        for (ULONG i = 0; i<_Size; i++) {
            SAFERELEASE(_ppAsmName[i]);
        }
        SAFEDELARRAY(_ppAsmName);
    }
    
    if (_ppAsmTextualIdentity) {
        for (ULONG i =0; i<_Size; i++) {
            SAFEDELARRAY(_ppAsmTextualIdentity[i]);
        }
        SAFEDELARRAY(_ppAsmTextualIdentity);
    }
}

HRESULT CAssemblyIdentityCache::GetNameObject(
            LPASSEMBLYNAME * ppAssemblyName, 
            LPCWSTR pszAssemblyName, 
            DWORD dwFlags, 
            LPVOID pvReserved) 
{
    HRESULT hr = S_OK;
    ULONG uIndex;
    IAssemblyName *pName = NULL;
    
    _ASSERTE(ppAssemblyName);
    _ASSERTE(pszAssemblyName);
    _ASSERTE(dwFlags == CANOF_PARSE_DISPLAY_NAME);
    _ASSERTE(!pvReserved);
    *ppAssemblyName = NULL;
        
    uIndex= HashString(pszAssemblyName, 0, _Size, FALSE);
    if (_ppAsmTextualIdentity[uIndex]) {
        _ASSERTE(_ppAsmName[uIndex]);
        if (!FusionCompareStringI(pszAssemblyName, _ppAsmTextualIdentity[uIndex])) {
            *ppAssemblyName = _ppAsmName[uIndex];
            (*ppAssemblyName)->AddRef();
            goto Exit;
        }
        SAFEDELARRAY(_ppAsmTextualIdentity[uIndex]);
        SAFERELEASE(_ppAsmName[uIndex])
    } else {
        _ASSERTE(!_ppAsmName[uIndex]);
    }

    hr = CreateAssemblyNameObject(&pName, pszAssemblyName, dwFlags, pvReserved);
    if (FAILED(hr)) 
        goto Exit;

    _ppAsmTextualIdentity[uIndex] = WSTRDupDynamic(pszAssemblyName);
    if (!_ppAsmTextualIdentity[uIndex]) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    _ppAsmName[uIndex] = pName;
    pName->AddRef();
    *ppAssemblyName = pName;
    pName->AddRef();
    
Exit:
    SAFERELEASE(pName);
    return hr;
}

// This removes leading/trailing whitespaces from the input string
void TrimString(__inout_z LPWSTR pwzStr)
{
    LPWSTR wzBegin = pwzStr;
    DWORD   dwCh = lstrlenW(pwzStr);
    LPWSTR  wzEnd = pwzStr + dwCh - 1;  // one before the null terminator

    // trim the leading spaces
    while (*wzBegin) {
        if (iswspace(*wzBegin)) {
            wzBegin++;
            continue;
        }

        break;
    }

    // trim the trailing spaces
    while(wzEnd > wzBegin) {
        if (iswspace(*wzEnd)) {
            *wzEnd-- = L'\0';
            continue;
        }
        break;
    }

    if ((wzBegin == pwzStr) && (wzEnd == pwzStr + dwCh - 1)) {
        // no change, return;
        return;
    }
    else {
        for (int i = 0; i < wzEnd + 2 - wzBegin; i++) {
            pwzStr[i] = wzBegin[i];
        }
    }
}
