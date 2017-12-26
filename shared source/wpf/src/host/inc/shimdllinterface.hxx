//******************************************************************************
//
// File: ShimDllInterface.hxx
//
// Activation parameters to be passed from the version-independent hosting shim 
// exe to the version-specific hosting DLL.
//
// History:
//
//     2005-05-19: [....] -        Created
//     2007/09/20: [....]          Ported Windows->DevDiv. See SourcesHistory.txt.
//
// Copyright (C) by Microsoft Corporation.  All rights reserved.
// 
//******************************************************************************

#ifndef SHIMDLLINTERFACE_HXX
#define SHIMDLLINTERFACE_HXX

struct ActivateParameters
{
    // The size of this structure. For version control. The sizes MUST be unique across versions.
    DWORD       dwSize;

    // The URI of the application to be deployed or the document to be opened.
    LPCWSTR     pswzUri;

    // The ApplicationIdentity of the application to be deployed. May not refer
    // to an application that has actually been deployed yet.
    LPCWSTR     pswzApplicationIdentity;

    // The RM manifest to use (documents only)
    LPCWSTR     pswzDocumentRmManifest;

    MimeType    mime;
    IStream*    pHistoryStream;
    IStream*    pDocumentStream;
    HANDLE      hDownloadCompletedEvent;
    BOOL        isDebug;
    LPWSTR      pswzDebugSecurityZoneURL;

    IPersistHistory* pPersistHistory;
    HANDLE      hNoHostTimer;

    // The outer object for COM aggregation purposes
    // This will be a CHostShim instance; COleDocument will use it for AddRef and Release calls.
    LPUNKNOWN   pOuterObject;

    // -->
    // Added for v3.5 SP1 servicing (including for Windows 7) and v4.
    // Note that a version of PresentationHostDll will normally never be invoked with this extra parameter
    // if it doesn't support it, because the shim invokes the latest PHDLL, and initially they are updated
    // together (for the above releases).
	// ...Oops, except it turned out we had to deal with the abnormal case of PH v4 finding only an older
	// PHDLL v3 that doesn't support the new feature. See fallback in CHostShim::Execute().

    // Set when the shim encountered an error and wants the DLL to display it (instead of running the app).
    // We have to fully activate the DocObject to be able to show the error page...
    LPCWSTR     pswzErrorMessageToDisplay;
};
#endif // SHIMDLLINTERFACE_HXX
