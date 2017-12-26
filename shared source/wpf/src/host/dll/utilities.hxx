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

#define URL_SCHEME_SECURE   L"https"
#define PATH_BUFFER_SIZE    (MAX_PATH + 64)

//******************************************************************************
//
// Prototypes
//
//******************************************************************************

HRESULT TaskAllocString(__in LPCWSTR pstrSrc, __deref_out_ecount(1) LPWSTR *ppstrDest);

HRESULT IsUrlSecure(const WCHAR* pszUrl);

HRESULT GetTopLevelBrowser(__in_ecount(1) IOleClientSite* pOleClientSite, __deref_out_ecount(1) IWebBrowser2** pWebBrowserTop);

// Loads the satellite .dll.mui resource library. Free with FreeMUILibrary().
HINSTANCE LoadResourceDLL();

size_t LoadResourceString(int id, __out_ecount(cchBufSize) wchar_t *pBuf, size_t cchBufSize);
BSTR LoadResourceString(int id);

bool FileExists(const wchar_t *filePath);
bool FileExists(const wchar_t *dir, const wchar_t *shortFilename);

BSTR GetFileVersion(const wchar_t *filePath);

int IsTabCycler(const MSG &msg);

HRESULT TerminateIfHostCrashes(HWND hwndBrowser);

//*********************************************************************************
//                              COM Helpers
//*********************************************************************************
BOOL IsSameObject(IUnknown *pUnkLeft, IUnknown *pUnkRight);


//*********************************************************************************
//                              Menu Helpers
//*********************************************************************************
WORD GetMenuResourceId(enum tagMimeType mimeType);
HMENU GetMenuFromID(__in HMENU hMenu, UINT menuID);
void UpdateMenuItems(__in HMENU hMenu, UINT cCmds, __in_ecount(cCmds) OLECMD *prgCmds);
