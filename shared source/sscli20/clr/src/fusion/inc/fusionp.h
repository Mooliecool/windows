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
#ifndef __FUSIONP_H_
#define __FUSIONP_H_

#include "windows.h"
#include "debugmacros.h"
#include "winerror.h"
#include "corerror.h"
#include "cor.h"
#include "../../dlls/mscorrc/fusres.h"
#include "fusionpriv.h"
#include "clrhost.h"
#include "shlwapi.h"
#include "winwrap.h"
#include "ex.h"
#include "fusion.h"
#include "fusionheap.h"
#include "eeconfig.h"
#include "strsafe.h"
//#include "dontuse.h"

#define IsPE64(x) ( (x == peIA64) || (x == peAMD64) )
#define IsPE32(x) ( (x == peI386) )
#define IsPEMSIL(x) ( (x == peMSIL) )

#define MAX_RANDOM_ATTEMPTS        0xFFFF

#define PLATFORM_TYPE_UNKNOWN       ((DWORD)(-1))
#define PLATFORM_TYPE_WIN95         ((DWORD)(0))
#define PLATFORM_TYPE_WINNT         ((DWORD)(1))
#define PLATFORM_TYPE_UNIX          ((DWORD)(2))

#define DIR_SEPARATOR_CHAR TEXT('\\')

#define URL_DIR_SEPERATOR_CHAR      L'/'
#define URL_DIR_SEPERATOR_STRING    L"/"

#define DWORD_STRING_LEN (sizeof(DWORD)*2) // this should be 8 chars; "ff00ff00" DWORD represented in string format.

#define ATTR_SEPARATOR_CHAR     L'_'
#define ATTR_SEPARATOR_STRING   L"_"

EXTERN_C DWORD GlobalPlatformType;

extern BOOL   g_bRunningOnNT;
extern BOOL   g_bRunningOnNT5OrHigher;
extern BOOL   g_bRunningOnNT51OrHigher;
extern BOOL   g_bRunningOnNT6OrHigher;

extern WORD   g_wProcessorArchitecture;     // Default to 32 bit
extern BOOL   g_fWow64Process;      // Wow64 Process
extern PEKIND g_peKindProcess;      // process default PEKIND

extern const WCHAR  g_wzMSIL[];
extern const WCHAR  g_wzX86[];
extern const WCHAR  g_wzIA64[];
extern const WCHAR  g_wzAMD64[];

#define IsProcess32() true

#undef SAFEDELETE
#define SAFEDELETE(p) if ((p) != NULL) { FUSION_DELETE_SINGLETON((p)); (p) = NULL; };

#undef SAFERELEASE
#define SAFERELEASE(p) if ((p) != NULL) { (p)->Release(); (p) = NULL; };

#undef SAFEDELETEARRAY
#define SAFEDELETEARRAY(p) if ((p) != NULL) { FUSION_DELETE_ARRAY((p)); (p) = NULL; };

#undef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// New cache flags
typedef enum
{
    CACHE_INVALID        = 0,
    CACHE_ZAP            = ASM_CACHE_ZAP,       // Stay the same as fusion.h
    CACHE_GAC            = ASM_CACHE_GAC,       // Stay the same as fusion.h
    CACHE_DOWNLOAD       = ASM_CACHE_DOWNLOAD,  // Stay the same as fusion.h
    CACHE_ROOT           = ASM_CACHE_ROOT,      // Stay the same as fusion.h
    CACHE_GAC_MSIL       = 0x10,
    CACHE_GAC_32         = 0x20,
    CACHE_GAC_64         = 0x40,
    CACHE_GAC_ANY        = CACHE_GAC
                        |  CACHE_GAC_MSIL
                        |  CACHE_GAC_32
                        |  CACHE_GAC_64
} CACHE_FLAGS;

#define CACHE_FLAGS_MASK    0x7F

#define IsGacType(x) ( x & CACHE_GAC_ANY)
#define IsZapType(x) ( x & CACHE_ZAP )

#define REG_KEY_FUSION_SETTINGS             TEXT("Software\\Microsoft\\Fusion")

#define FUSION_CACHE_DIR_ROOT_SZ            TEXT("\\assembly")
#define FUSION_CACHE_DIR_DOWNLOADED_SZ      TEXT("\\assembly\\dl3")
#define FUSION_CACHE_DIR_GAC_SZ             TEXT("\\GAC")
#define FUSION_CACHE_DIR_GACNE_SZ           TEXT("\\GAC_MSIL")
#define FUSION_CACHE_DIR_GAC32_SZ           TEXT("\\GAC_32")
#define FUSION_CACHE_DIR_GAC64_SZ           TEXT("\\GAC_64")
#define FUSION_CACHE_DIR_ZAP_SZ             TEXT("\\NativeImages_")
#define FUSION_CACHE_DIR_ZAP_SHADOW_SZ      TEXT("_shadow")
#define FUSION_CACHE_STAGING_DIR_SZ         TEXT("\\assembly\\tmp")
#define FUSION_CACHE_PENDING_DEL_DIR_SZ     TEXT("\\assembly\\temp")

#define MAX_PATH_MODIFIER_LENGTH            ARRAYSIZE(L"_65535")

#define FILENAME_PUBLISHER_PCY_TIMESTAMP    L"\\PublisherPolicy.tme"

extern WCHAR g_UserFolderPath[MAX_PATH+1];
extern WCHAR g_szWindowsDir[MAX_PATH+1];
extern WCHAR g_GACRoot[MAX_PATH+1];    // Assembly
extern WCHAR g_GACDir[MAX_PATH+1];     // GAC
extern WCHAR g_GACNE[MAX_PATH+1];      // GACNE
extern WCHAR g_GAC32[MAX_PATH+1];      // GAC32
extern WCHAR g_GAC64[MAX_PATH+1];      // GAC64
extern BOOL  g_bUseDefaultStore;       // GAC is under %windir%

extern WCHAR g_ZapDir[MAX_PATH+1];
extern DWORD g_ZapQuotaInKB;
extern DWORD g_DownloadCacheQuotaInKB;

extern DWORD g_ScavengingThreadId;
extern WCHAR g_FusionDllPath[MAX_PATH+1];

extern EEConfig *g_pConfig;
inline BOOL DisableCachingBindingFailures()
{
    return g_pConfig?(!(g_pConfig->CacheBindingFailures())):FALSE;
}

extern DWORD g_dwUseLegacyIdentityFormat;

inline BOOL UseLegacyIdentityFormat()
{
    return g_dwUseLegacyIdentityFormat || (g_pConfig?(g_pConfig->UseLegacyIdentityFormat()):FALSE);
}

extern HANDLE  g_hDownloadThreadHandle;

#define MAX_VERSION_DISPLAY_SIZE  sizeof("65535.65535.65535.65535")

#define PUBLIC_KEY_TOKEN_DISPLAY_LEN    16
#define PUBLIC_KEY_TOKEN_LEN            8

#define FUSION_GUID_LENGTH  (50)

// random number. Currectly the max length of valid culture string is 12.
#define MAX_CULTURE_SIZE                128  

#define MAX_DWORD_STRING_SIZE 12        // N

struct TRANSCACHEINFO
{
    DWORD       dwType;           // entry type
    FILETIME    ftCreate;         // created time
    FILETIME    ftLastAccess;     // last access time
    LPWSTR      pwzName;          // Name
    LPWSTR      pwzCulture;       // Culture
    PEKIND      dwAsmImageType;   // Assembly Image Type
    BLOB        blobPKT;          // Public Key Token (hash(PK))
    WORD        wVers[4];         // version
    BLOB        blobSignature;    // Signature blob
    BLOB        blobMVID;         // MVID
    DWORD       dwPinBits;        // Bits for pinning Asm; one bit for each installer
    LPWSTR      pwzCodebaseURL;   // where is the assembly coming from
    FILETIME    ftLastModified;   // Last-modified of codebase url.
    LPWSTR      pwzPath;          // Cache path
    DWORD       dwKBSize;         // size in KB 
    BLOB        blobPK;           // Public Key (if strong)
    LPWSTR      pwzPathModifier;  // Folder name modifier
    BOOL        fLegacyAssembly;  // Old gac assembly
    BOOL        fHasManifestFile; // Assembly has embedded manifest
};

#define     DB_E_DUPLICATE (HRESULT_FROM_WIN32(ERROR_DUP_NAME))


#define OS_WINDOWS                  0           // windows vs. NT
#define OS_NT                       1           // windows vs. NT
#define OS_WIN95                    2           // Win95 or greater
#define OS_NT4                      3           // NT4 or greater
#define OS_NT5                      4           // NT5 or greater
#define OS_MEMPHIS                  5           // Win98 or greater
#define OS_MEMPHIS_GOLD             6           // Win98 Gold
#define OS_WIN2000                  7           // Some derivative of Win2000
#define OS_WIN2000PRO               8           // Windows 2000 Professional (Workstation)
#define OS_WIN2000SERVER            9           // Windows 2000 Server
#define OS_WIN2000ADVSERVER         10          // Windows 2000 Advanced Server
#define OS_WIN2000DATACENTER        11          // Windows 2000 Data Center Server
#define OS_WIN2000TERMINAL          12          // Windows 2000 Terminal Server
#define OS_NT51                     13          // Windows XP or greater
#define OS_NT6                      14

STDAPI_(BOOL) IsOS(DWORD dwOS);


extern MUTEX_COOKIE g_hCacheMutex;

HRESULT CreateCacheMutex();

inline BOOL IsPathSeparator(WCHAR ch)
{
    return (ch == TEXT('\\') || ch == TEXT('/'));
}

// global variables for hosting. Defined in src\vm\corhost.cpp
extern BOOL g_bFusionHosted;
extern ICLRAssemblyReferenceList *g_pHostAsmList;
extern IHostAssemblyStore *g_pHostAssemblyStore;


#define ASM_DISPLAYF_DEFAULT   (ASM_DISPLAYF_VERSION   \
                                |ASM_DISPLAYF_CULTURE   \
                                |ASM_DISPLAYF_PUBLIC_KEY_TOKEN  \
                                |ASM_DISPLAYF_RETARGET)

#define ASM_DISPLAYF_FULL_NO_VERSION ( ASM_DISPLAYF_CULTURE   \
                                |ASM_DISPLAYF_PUBLIC_KEY_TOKEN  \
                                |ASM_DISPLAYF_RETARGET \
                                |ASM_DISPLAYF_PROCESSORARCHITECTURE)

#endif  // __FUSIONP_H_

