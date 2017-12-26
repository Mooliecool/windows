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

/*
NB: the method below (InMemoryCompile) is used by vsproject
    so please coordinate any changes to the signature with whoever owns that file
*/

typedef HRESULT (*ErrorReporter)(LPCWSTR filename, long line, long column, int error, LPCWSTR message, ERRORKIND level);


struct COMPILEPARAMS {
    int nText;                              // Count of Sources
    LPCWSTR * pszNames;                     // Names (filenames) of sources
    LPCWSTR pszTarget;                      // Output filename
    DWORD optCount;                         // Count of Option/value pairs
    LPCWSTR * pszOptions;                   // Option names
    VARIANT * pValues;                      // Option values
    ErrorReporter reporter;                 // Function to report errors to
};

extern HINSTANCE STDMETHODCALLTYPE GetMessageDll();
extern HRESULT STDMETHODCALLTYPE CreateCompilerFactory (ICSCompilerFactory **ppFactory);
extern "C" HRESULT STDMETHODCALLTYPE InMemoryCompile(LPCWSTR * pszText, const COMPILEPARAMS * params);

extern HINSTANCE STDMETHODCALLTYPE GetMessageDll_Impl();
extern HRESULT STDMETHODCALLTYPE CreateCompilerFactory_Impl(ICSCompilerFactory **ppFactory);
extern "C" HRESULT STDMETHODCALLTYPE InMemoryCompile_Impl(LPCWSTR * pszText, const COMPILEPARAMS * params);
