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
#ifndef __HELPERS_H_INCLUDED__
#define __HELPERS_H_INCLUDED__

#include "xmlparser.h"
#include "nodefact.h"

#define FROMHEX(a) ((a)>=L'a' ? a - L'a' + 10 : a - L'0')
#define TOLOWER(a) (((a) >= L'A' && (a) <= L'Z') ? (L'a' + (a - L'A')) : (a))
#define TOUPPER(a) (((a) >= L'a' && (a) <= L'z') ? (L'A' + (a - L'a')) : (a))

extern const WCHAR g_wzInvalidAsmNameChars[];

#define MAX_URL_LENGTH        2084 // same as INTERNET_MAX_URL_LENGTH
#define INI_READ_BUFFER_SIZE  1024  
extern LPWSTR g_pwzCORVersion;

HRESULT AppCtxGetWrapper(IApplicationContext *pAppCtx, LPCWSTR wzTag,
                         __deref_out WCHAR **ppwzValue);
HRESULT NameObjGetWrapper(IAssemblyName *pName, DWORD nIdx, 
                          LPBYTE *ppbBuf, LPDWORD pcbBuf);
HRESULT GetFileLastModified(LPCWSTR pwzFileName, FILETIME *pftLastModified);
DWORD GetRealWindowsDirectory(__out_ecount(uSize) LPWSTR wszRealWindowsDir, UINT uSize);
HRESULT SetAppCfgFilePath(IApplicationContext *pAppCtx, LPCWSTR wzFilePath);

HRESULT CfgEnterCriticalSection(IApplicationContext *pAppCtx);
HRESULT CfgLeaveCriticalSection(IApplicationContext *pAppCtx);
HRESULT MakeUniqueTempDirectory(LPCWSTR wzTempDir, 
                                __out_ecount(dwLen) LPWSTR wzUniqueTempDir,
                                DWORD dwLen);
HRESULT CreateFilePathHierarchy( LPCOLESTR pszName );
DWORD GetRandomName (__out_ecount(dwLen+1) LPTSTR szDirName, DWORD dwLen);
HRESULT CreateDirectoryForAssembly(DWORD dwDirSize, __out_ecount(*pcwPath) LPTSTR pszPath, __inout LPDWORD pcwPath);
HRESULT RemoveDirectoryAndChildren(LPCWSTR szDir);
STDAPI CopyPDBs(IAssembly *pAsm);
void TrimString(__inout_z LPWSTR pwzStr);
HRESULT VersionFromString(LPCWSTR wzVersion, WORD *pwVerMajor, WORD *pwVerMinor,
                          WORD *pwVerBld, WORD *pwVerRev);

BOOL VerifySignature(LPCWSTR szFilePath, LPBOOL pfWasVerified, DWORD dwFlags);
HRESULT FusionGetUserFolderPath();

DWORD HashString(LPCWSTR wzKey, DWORD dwStart, DWORD dwHashSize, BOOL bCaseSensitive);
DWORD HashBlob(LPBYTE pbKey, DWORD dwLen, DWORD dwStart, DWORD dwHashSize);

HRESULT ExtractXMLAttribute(__deref_out LPWSTR *ppwzValue, XML_NODE_INFO **aNodeInfo,
                            USHORT *pCurIdx, USHORT cNumRecs);
HRESULT AppendString(__deref_inout LPWSTR *ppwzHead, LPCWSTR pwzTail, DWORD dwLen);

LPWSTR GetNextDelimitedString(__deref_inout_z LPWSTR *ppwzList, WCHAR wcDelimiter);

// initialize g_pwzCORVersion;
HRESULT GetCORVersion();

HRESULT GetRandomFileName(__out_ecount(dwFileName) LPTSTR pszPath, DWORD dwFileName);

HRESULT GetManifestFileLock(LPCWSTR pszManifestFile, HANDLE *phFile);

DWORD PAL_IniReadStringEx(HINI hIni, LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                 __deref_out LPWSTR *ppwzReturnedString);

void FusionFormatGUID(const GUID *guid, __out_ecount(cchSize) LPWSTR pszBuf, DWORD cchSize, BOOL bUseBrace);

HRESULT UrlCanonicalizeUnescape(LPCWSTR pszUrl, __out_ecount(*pcchCanonicalized) LPWSTR pszCanonicalized, LPDWORD pcchCanonicalized, DWORD dwFlags);
HRESULT UrlCombineUnescape(LPCWSTR pszBase, LPCWSTR pszRelative, __out_ecount(*pcchCombined) LPWSTR pszCombined, LPDWORD pcchCombined, DWORD dwFlags);
HRESULT PathCreateFromUrlWrap(LPCWSTR pszUrl, __out_ecount(*pcchPath) LPWSTR pszPath, LPDWORD pcchPath, DWORD dwFlags);
LPWSTR StripFilePrefix(__inout_z LPWSTR pwzURL);


HRESULT CheckFileExistence(LPCWSTR pwzFile, BOOL *pbExists, LPDWORD pdwAttrib);

int FusionCompareString(LPCWSTR pwz1, LPCWSTR pwz2, BOOL bCaseSensitive = TRUE);
int FusionCompareStringI(LPCWSTR pwz1, LPCWSTR pwz2);
int FusionCompareStringNI(LPCWSTR pwz1, LPCWSTR pwz2, int nChar);
int FusionCompareStringN(LPCWSTR pwz1, LPCWSTR pwz2, int nChar, BOOL bCaseSensitive = TRUE);
HRESULT Base32Encode(__in_bcount(cbData) BYTE *pbData, DWORD cbData, __deref_out LPWSTR *ppwzBase32);
HRESULT PathAddBackslashWrap(__inout_ecount(dwMaxLen) LPWSTR pwzPath, DWORD dwMaxLen);

HRESULT ConvertArrayPtrsToString(LPCWSTR *ppwzArrayStr, DWORD dwModCount, WCHAR wSeperatorChar, __deref_out LPWSTR *ppwzStringm, LPDWORD pdwLen = NULL);

HRESULT ConvertStringToArrayPtrs(LPCWSTR pwzModuleList, __deref_out LPWSTR **ppwzArrayList, WCHAR wSeperatorChar, LPDWORD pdwModCount);

HRESULT UrlRemoveExtraBackSlashes(__inout_z LPWSTR pwzUrlPath);

HRESULT RegQueryValueExWrap(HKEY hKey, LPCWSTR szValueName, DWORD dwType, 
                            LPBYTE *lpData, LPDWORD pcbData, BOOL bRequired);

HRESULT LoggingGetDisplayName(IAssemblyName *pName, 
        __out_ecount(*pdwLen) LPWSTR pwzBuf, 
        __inout DWORD *pdwLen, 
        BOOL bUseLoaderCallBack);
                              
// operate on the input string!!!
void PathConvertBackSlashesToUnderScore(__inout_z LPWSTR pwzPath);

void ReplaceInvalidFileNameCharactersToUnderscore(__inout_z LPWSTR pwzPath);

HRESULT ValidateAsmInstallFolderChars(LPCWSTR pszFolderName);

HRESULT MapProcessorArchitectureToPEKIND(LPCWSTR pwzProcArch, PEKIND *pe);

PEKIND  MapCacheTypeToPEKIND(DWORD dwCacheType);

LPCWSTR ProcessorArchitectureFromPEKIND(PEKIND pe);

HRESULT CanonicalizeIdentity(__inout_z LPWSTR pwzIdentity);

HRESULT IsSameCodebase(IAssembly *pAsm, LPCWSTR pwzCodebase, BOOL *bSame);

// bit mask macro helpers
#define MAX_ID_FROM_MASK(size)          ((size) << 3)
#define MASK_SIZE_FROM_ID(id)           ((id) >> 3)
#define IS_IN_RANGE(id, size)   ((id) <= ((size) << 3))
#define IS_BIT_SET(id, mask)    (mask[((id)-1)>>3] & (0x1 << (((id)-1)&0x7)))
#define SET_BIT(id, mask)       (mask[((id)-1)>>3] |= (0x1<< (((id)-1)&0x7)))
#define UNSET_BIT(id, mask)     (mask[((id)-1)>>3] &= (0xFF - (0x1<<(((id)-1)&0x7))))

class CAssemblyIdentityCache{
public:
        static HRESULT Create(CAssemblyIdentityCache **ppAsmCache, ULONG uSize);
        CAssemblyIdentityCache();
        ~CAssemblyIdentityCache();
        HRESULT Init(ULONG uSize);
        HRESULT GetNameObject(LPASSEMBLYNAME *ppAssemblyName, 
                LPCWSTR pszAssemblyName, 
                DWORD dwFlags, 
                LPVOID pvReserved);
private:
        LPASSEMBLYNAME *_ppAsmName;
        LPWSTR *_ppAsmTextualIdentity;
        ULONG _Size;
};

class AsmBindHistoryInfo{
public:
    AsmBindHistoryInfo()
    {
        pwzAsmName = NULL;
        pwzCulture = NULL; 
        _cRef = 1;
    }

    virtual ~AsmBindHistoryInfo()
    {
        ClearInfo();
    }

    void ClearInfo() {
        SAFEDELETEARRAY(pwzAsmName);
        SAFEDELETEARRAY(pwzCulture);
    }
    
    ULONG AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }
    
    ULONG Release()
    {
        if (InterlockedDecrement(&_cRef) == 0) {
            delete this;
            return 0;
        }

        return _cRef;
    }

public:    
    LPWSTR pwzAsmName;
    LPWSTR pwzCulture;
    WCHAR  wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    WCHAR  wzVerReference[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR  wzVerAppCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR  wzVerPublisherCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR  wzVerAdminCfg[MAX_VERSION_DISPLAY_SIZE+1];

private:
    LONG _cRef;
} ;
#endif // __HELPERS_H_INCLUDED__
