//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Declares prototypes for some common functions
//
//  History:
//     2005/06/16-akaza
//          Created
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 
BOOL IsDisableKeySet(LPCTSTR pRegkeyLocationToCheck,LPCTSTR pRegKeyToExtract);
BOOL IsMimeTypeDisabled(__in_ecount(INTERNET_MAX_URL_LENGTH+1) LPOLESTR pUrl, __in_ecount(1) COleDocument *pOleDoc);
BOOL ProcessURLActionOnIE7(DWORD dwAction, __in_ecount(INTERNET_MAX_URL_LENGTH+1) LPOLESTR pUrl, __in_ecount(1) COleDocument *pOleDoc);
