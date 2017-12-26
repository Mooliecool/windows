//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Defines the utility functions of PresentationHost.
//
//  History
//      2002/06/19-murrayw
//          Created
//      2003/06/30-[....]
//          Ported ByteRangeDownloader to WCP
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

//******************************************************************************
//
// Defines
//
//******************************************************************************

#define EMBEDDINGFLAG       L"Embedding"
#define DEBUGFLAG           L"Debug"
#define EVENTFLAG           L"Event"
#define DEBUGSECURITYZONEURLFLAG    L"DebugSecurityZoneURL"
#define LAUNCHDOTAPPLICATIONFLAG    L"LaunchApplication"
#define REGISTERSERVERFLAG  L"RegServer"
#define UNREGISTERSERVERFLAG L"UnregServer"

#define URL_SCHEME_SECURE   L"https"

// Individual flags
#define FLAG_EMBEDDING      0x01
#define FLAG_DEBUG          0x02
#define FLAG_EVENT          0x04
#define FLAG_DEBUGSECURITYZONEURL   0x08
#define FLAG_LAUNCHDOTAPPLICATION   0x10
#define FLAG_REGISTERSERVER 0x20
#define FLAG_UNREGISTERSERVER 0x40

// Combinations of flags
#define FLAGS_NONE           0x00

#define PATH_BUFFER_SIZE    (MAX_PATH + 64)
#define DEBUGSECURITYZONEURL_BUFFER_SIZE     (MAX_PATH + 64)

#ifdef APPLICATION_SHIM
// DFSHIM stuff for launching click once .application
#define DFDLL L"dfdll.dll"
#define URLACTION_MANAGED_UNSIGNED  0x00002004
#define URLACTION_MANAGED_SIGNED    0x00002001
typedef HRESULT (*fnActivateDeploymentEx)(__in LPCWSTR deploymentUrl, DWORD dwUnsignedPolicy, DWORD dwSignedPolicy);
#endif

//******************************************************************************
//
// Prototypes
//
//******************************************************************************

HRESULT ParseCommandLine(__out_ecount(1) LPDWORD pdwFlags, 
                         __out_ecount(nFileNameSize) LPWSTR pwzFileName,
                         size_t nFileNameSize,
                         __out_ecount(nEventNameSize) LPWSTR pwzEventName,
                         size_t nEventNameSize,
                         __out_ecount(nDebugSecurityZoneURLSize) LPWSTR pwzDebugSecurityZoneURL,
                         size_t nDebugSecurityZoneURLSize,
                         __out_ecount_opt(nDotApplicationURLSize) LPWSTR pwzDotApplicationURL,
                         size_t nDotApplicationURLSize);

BOOL MsgWaitForSingleObject(__in HANDLE hEvent, DWORD dwMilliSeconds, __out_ecount(1) BOOL* fWmQuit);

MimeType GetMimeTypeFromString(LPCWSTR szMimeString);

BOOL IsValidMimeType(__in MimeType mime);

// Returned message buffer must be freed with LocalFree().
wchar_t *TryDecodeDownloadError(HRESULT hr);

// Defined in dll\Utilities.cxx
size_t LoadResourceString(int id, __out_ecount(cchBufSize) wchar_t *pBuf, size_t cchBufSize);
BSTR LoadResourceString(int id);

#ifdef DOCUMENT_SHIM
HRESULT EndsWith(LPCWSTR szTarget, LPCWSTR szMatch);
#endif


//*********************************************************************************
//                              Click Once Helpers
//*********************************************************************************
#ifdef APPLICATION_SHIM
HRESULT LaunchClickOnceApplication(LPCWSTR szApplicationURL);
#endif

