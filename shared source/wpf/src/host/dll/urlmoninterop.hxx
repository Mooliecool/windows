//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Declares the class for InternetSecurityManager reuse
//
//  History:
//     2005/06/15 -akaza
//          Created
//     2007/09/20 -[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 

class UrlmonInterop
{
private:
    static IInternetSecurityManager *s_pInternetSecurityManager;
    static IInternetSecurityMgrSite *s_pSecurityMgrSiteImpl;

    static HRESULT RegisterSecurityManagerSite();
    
public:
    static HRESULT ReleaseSecurityManager();
    static HRESULT GetSecurityManager(__out_ecount_opt(1) IInternetSecurityManager **ppInternetSecurityManager);
    static HRESULT ProcessUrlActionWrapper(DWORD dwurlAction,__in_ecount(1) LPOLESTR pUrl,__in_ecount_opt(1) COleDocument* pOleDoc,__out_ecount(1)BOOL &fAllow);
};

