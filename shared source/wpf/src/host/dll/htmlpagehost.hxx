//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Helpers for hosting HTMLDocument ("Trident"), which is used in the deployment progress page
//      and in the unhandled exception page.
//
//  History:
//      2007/12/xx   [....]     Created, stuff mostly factored out of ErrorPage.*
//
//------------------------------------------------------------------------

//







#pragma once

class CHTMLPageHostingHelper
{
public:
    static HRESULT CreateHTMLDocumentControl(
        HWND hHostWindow, __in_opt IStream *pInitStream, 
        __deref_opt_out IUnknown **ppAxContainer, __deref_out IHTMLDocument2 **ppHtmlDoc);

    static bool HandleNavigationKeys(const MSG &msg);
};
