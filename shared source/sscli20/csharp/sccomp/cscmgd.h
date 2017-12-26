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
// File: cscmgd.h
//
// ===========================================================================

#ifndef __cscmgd_h__
#define __cscmgd_h__

#include "csiface.h"
#include "alloc.h"
#include "options.h"
#include "namemgr.h"
#include "error.h"
#include "compilerlib.h"

class CInMemorySourceText : public ICSSourceText {
    LPCWSTR m_pszText;
    LPCWSTR m_pszName;
    LONG        m_iRef;


public:
    CInMemorySourceText (LPCWSTR name, LPCWSTR text) : m_pszText(text), m_pszName(name), m_iRef(0) {}

    // IUnknown
    STDMETHOD_(ULONG, AddRef)() { return InterlockedIncrement (&m_iRef); }
    STDMETHOD_(ULONG, Release)() { ULONG i = InterlockedDecrement (&m_iRef); if (i == 0) delete this; return i; }
    STDMETHOD(QueryInterface)(REFIID riid, void **ppObj) { return E_NOINTERFACE; }

    // ICSSourceText
    STDMETHOD(CloneInMemory)(ICSSourceText **ppClone) { return E_NOTIMPL; }
    STDMETHOD(GetText)(PCWSTR *ppszText, POSDATA *pposEnd) { *ppszText = m_pszText; if (pposEnd != NULL) { pposEnd->iLine = 0; pposEnd->iChar = 0; } return S_OK; }
    STDMETHOD(GetName)(PCWSTR *ppszName) { *ppszName = m_pszName; return S_OK; }
    STDMETHOD(GetLineCount)(long *piLines) { return E_NOTIMPL; }
    STDMETHOD(ReleaseText)() { return E_NOTIMPL; }
    STDMETHOD(AdviseChangeEvents)(ICSSourceTextEvents *pSink, DWORD_PTR *pdwCookie) { return E_NOTIMPL; }
    STDMETHOD(UnadviseChangeEvents)(DWORD_PTR dwCookie) { return E_NOTIMPL; }
    STDMETHOD(GenerateChecksum)() { return E_NOTIMPL; }
    STDMETHOD(GetChecksum)(Checksum * checksum) { return E_NOTIMPL; }
    STDMETHOD(ReportExceptionalOperation)() { return S_OK; }
};

class CInMemoryCompilerHost : public ICSCompilerHost {
    ICSCompiler     *pCompiler;
    ICSSourceText   **pSource;
    LPWSTR          *pNames;
    int             m_count;
    int             m_lastFound; // cache the index the last source module found was, to prevent O(n^2) behavior.
    ErrorReporter   m_reporter;
    WCHAR           m_szRelatedWarning[256], m_szRelatedError[256];
    STDMETHOD(ReportError)(ICSError *pError);
    STDMETHOD_(void, Error)(HRESULT hr);
    STDMETHOD_(void, Error)(int id, const wchar_t * psz) { ErrorArgs(id, &psz); }
    STDMETHOD_(void, ErrorArgs)(int id, VarArgList args);

public:
    CInMemoryCompilerHost (ErrorReporter reporter);
    ~CInMemoryCompilerHost ();

    HRESULT Init (int count, LPCWSTR text [], LPCWSTR names []);
    void SetCompiler (ICSCompiler *pComp) { pCompiler = pComp; }
    HRESULT AddInputs(ICSInputSet *pInputSet);
    HRESULT SetConfig(ICSCompilerConfig * pConfig, ICSInputSet * pInputSet, DWORD optCount, LPCWSTR pszOptions[], VARIANT pValues[]);

    /* We don't support IUnknown in a standard way because we don't have to! */
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() {return 1;}
    STDMETHODIMP_(ULONG) Release() {return 1;}

    STDMETHOD(ReportErrors)(ICSErrorContainer *pErrors);
    STDMETHOD(GetSourceModule)(PCWSTR pszFile, BOOL fNeedChecksum, ICSSourceModule **ppModule);
    STDMETHOD_(void, OnConfigChange)(long iOptionID, VARIANT vtValue) {}
    STDMETHOD_(void, OnCatastrophicError)(BOOL fException, DWORD dwException, void *pAddress);
};

#endif // __cscmgd_h__
