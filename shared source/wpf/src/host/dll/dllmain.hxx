//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Defines the main entry point of PresentationHost used for hosting
//     Windows Client Applications in the browser
//
//  History:
//     2002/06/12-murrayw
//          Created
//     2003/06/03-kusumav
//          Ported to WCP
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 

/**************************************************************************
   global variables
**************************************************************************/

extern HINSTANCE  g_hInstance;
extern DWORD      g_mainThreadId;
extern volatile long       g_ServerRefCount;

extern COleDocument* g_pOleDoc;

extern HANDLE g_hNoHostTimer; // See comment in main.hxx (in the shim)

HRESULT LoadHistoryHelper(__in IStream *loadStream);
// Note: The return strings must be freed with CoTaskMemFree().
HRESULT SaveHistoryHelper(__in IStream *saveStream,
                                           __out int *entryIndex,
                                           __out LPWSTR *uri,
                                           __out LPWSTR *title);

HRESULT ForwardTranslateAccelerator(MSG* pMsg, VARIANT_BOOL appUnhandled);


