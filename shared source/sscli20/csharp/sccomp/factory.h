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
// ===========================================================================
// File: factory.h
//
// ===========================================================================

#ifndef __factory_h__
#define __factory_h__

#include "csiface.h"

////////////////////////////////////////////////////////////////////////////////
// CFactory
//
// This class is the compiler factory.

class CFactory :
    public CComObjectRootMT,
    public CComCoClass<CFactory, &CLSID_CSCompilerFactory>,
    public ICSCompilerFactory
{
public:
    BEGIN_COM_MAP(CFactory)
        COM_INTERFACE_ENTRY(ICSCompilerFactory)
    END_COM_MAP()

    DECLARE_NO_REGISTRY( ) 

    CFactory();  // constructor

    // ICSCompilerFactory
    STDMETHOD(CreateNameTable)(ICSNameTable **ppTable);
    STDMETHOD(CreateCompiler)(DWORD dwFlags, ICSCompilerHost *pHost, ICSNameTable *pNameTable, ICSCompiler **ppCompiler);
    STDMETHOD(CreateLexer)(ICSNameTable *pNameTable, CompatibilityMode eKeywordMode, ICSLexer **ppLexer);
    STDMETHOD(CheckVersion)(HINSTANCE hInstance, BSTR *pbstrError);
    STDMETHOD(CheckCompilerVersion)(HINSTANCE hInstance, DWORD dwVersion, BSTR *pbstrError);

    static HRESULT StaticCheckCompilerVersion(HINSTANCE hInstance, DWORD dwVersion, BSTR *pbstrError);
};

#endif // __factory_h__

