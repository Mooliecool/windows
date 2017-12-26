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
#ifndef _CACHEUTILS_H_
#define _CACHEUTILS_H_

#include "transprt.h"
#include "asmitem.h"

#define SIGNATURE_BLOB_LENGTH      0x80
#define SIGNATURE_BLOB_LENGTH_HASH 0x14
#define MVID_LENGTH                sizeof(GUID)

#define NATIVE_IMAGE_SUFFIX         L".ni"

HRESULT SetRootCachePath(LPCWSTR pwzBindingConfigAssemblyStorePath);


HRESULT GetAssemblyStagingPath(LPCTSTR pszCustomPath, DWORD dwCacheFlags,
                               BOOL bUser, __out_ecount_opt(*pcchSize) LPTSTR pszPath, __inout DWORD *pcchSize);

HRESULT CreateAssemblyDirPath(LPCTSTR pszCustomPath, CACHE_FLAGS dwCacheFlags, 
                              BOOL bUser, __out_ecount_opt(*pcchSize) LPTSTR pszPath, __inout DWORD *pcchSize);

HRESULT GetPendingDeletePath(LPCTSTR pszCustomPath, DWORD dwCacheFlags,
                             __out_ecount_opt(*pcchSize) LPTSTR pszPath, __inout DWORD *pcchSize);

HRESULT SetPerUserDownloadDir();
HRESULT GetZapDir(__deref_out LPWSTR *pszZapDir);

HRESULT GetDownloadDir(__deref_out LPWSTR *pszDownLoadDir);

HRESULT SetDownLoadDir();

HRESULT GetAssemblyParentDir(CTransCache *pTransCache, 
                    __out_ecount_opt(*pcchSize) LPWSTR pszParentDir, __inout LPDWORD pcchSize, LPCWSTR pwzCustomPath);

HRESULT ParseDirName(CTransCache *pTransCache, PEKIND dwAsmArchType, 
                    LPCWSTR pszParentDir, LPCWSTR pszAsmDir);

HRESULT RetrieveFromFileStore( CTransCache *pTransCache );

HRESULT GetCacheDirsFromName(IAssemblyName *pName, DWORD dwFlags, 
                    __out_ecount(dwParentDirSz) LPWSTR pszParentDirName, DWORD dwParentDirSz, 
                    __out_ecount(dwSubDirSz) LPWSTR pszSubDirName, DWORD dwSubDirSz, 
                    LPCWSTR pwzCustom);

HRESULT GetZapDirFromName(IAssemblyName *pName, 
                          __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen);

HRESULT GetFullZapDirFromName(IAssemblyName *pName, 
                              __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen);

HRESULT GetFullGACDirFromName(IAssemblyName *pName, DWORD dwCacheType, 
                              __out_ecount_opt(*pdwLen) LPWSTR pwzAsmDir, __inout DWORD *pdwLen);

HRESULT GetManifestFilePathFromName(IAssemblyName *pName, BOOL isDll, DWORD dwCacheType, 
                                __out_ecount_opt(*pdwLen)  LPWSTR pwzAsmDir, __inout DWORD *pdwLen);

HRESULT StoreFusionInfo(IAssemblyName *pName, LPCWSTR pszDir, CACHE_FLAGS dwCacheFlags, DWORD *pdwFileSizeLow);

HRESULT StoreFusionInfoToLegacyGAC(IAssemblyName *pName, LPCWSTR pwzDir);

HRESULT StoreFusionInfoToDownloadCache(IAssemblyName *pName, LPCWSTR pwzDir, DWORD *pdwFileSizeLow);

HRESULT GetFusionInfo(CTransCache *pTC, LPCWSTR pszAsmDir);

HRESULT GetAssemblyKBSize(LPCWSTR pszManifestPath, DWORD *pdwSizeinKB, 
                          LPFILETIME pftLastAccess, LPFILETIME pftCreation);

HRESULT GetCacheLoc(DWORD dwCacheFlags, __deref_out LPWSTR *pszCacheLoc);

HRESULT CheckExistsInGAC(IAssemblyName *pName, DWORD dwCacheType, 
                         LPBOOL pbIsDll, FILETIME *pftLastMod, 
                         IAssembly **ppAsm);

HRESULT GetGACDir(CACHE_FLAGS dwCacheFlags, __deref_out LPWSTR *ppszDir);

// 64 bit supporting functions
void MapAssemblyArchitectureToCacheType(PEKIND dwAsmArchType, CACHE_FLAGS *pdwAsmArchCacheType);

WORD GetRealProcessorType();
HRESULT IsValidAssemblyOnThisPlatform(IAssemblyName *pName, BOOL bForInstall);

HRESULT GetCustomLogPath(__out_ecount_opt(*pdwSize) LPWSTR pwzLogPath, __inout LPDWORD pdwSize);

HRESULT IsProcessWow64(BOOL *pfDisposition);

BOOL WillAffectedByGACChange(DWORD dwCacheTypeChange, DWORD dwCacheTypeHave);

// Assumes both directories exist
HRESULT MoveAllFilesFromDir(LPCWSTR pszSrcDirPath, LPCWSTR pszDestDirPath);

HRESULT FusionMoveDirectory(LPCWSTR pszSrcDirPath, LPCWSTR pszDestDirPath);

HRESULT FusionCopyFiles(LPCWSTR pwzSrcDir, LPCWSTR pwzDestDir, BOOL fCopyDirs);

HRESULT GetCachedDownloadCacheDir(__out_ecount_opt(*pdwSize) LPWSTR pwzBuf, __inout LPDWORD pdwSize);

HRESULT StoreDownloadCacheDir(LPCWSTR pwzBuf);

#define FUSION_GAC_LOCK_RETRY_WAITING_TIME      100
#define FUSION_DOWNLOAD_CACHE_LOCK_RETRY_WAITING_TIME      10


#endif // _CACHEUTILS_H_
