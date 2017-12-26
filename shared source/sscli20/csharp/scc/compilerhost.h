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

#include "csiface.h"

#define CLIENT_IS_CSC
#include "consoleoutput.h"
#undef CLIENT_IS_CSC


DEFINE_GUID(IID_ICSCommandLineCompilerHost,
0xBD7CF4EF, 0x1821, 0x4719, 0xAA, 0x60, 0xDF, 0x81, 0xB9, 0xEA, 0xFA, 0x01);

/*
 * The compiler host class.
 */
class CompilerHost: public ICSCompilerHost, public ICSCommandLineCompilerHost
{
    ICSCompiler     *pCompiler;
    ConsoleOutput   *m_pOutput;
    UINT            m_uiCodePage;
public:
    CompilerHost (ConsoleOutput *output);
    ~CompilerHost ();

    void        SetCompiler (ICSCompiler *pComp) { pCompiler = pComp; }
    void        SetCodePage (UINT uiCodePage) { m_uiCodePage = uiCodePage; }

    static void ReportError (ICSError *pError, ConsoleOutput *output);

    /* We don't support IUnknown in a standard way because we don't have to! */
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
        if (riid == IID_ICSCommandLineCompilerHost)
        {
            *((ICSCommandLineCompilerHost**)ppvObject) = (ICSCommandLineCompilerHost*) this;
            return S_OK;
        }
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() {return 1;}
    STDMETHODIMP_(ULONG) Release() {return 1;}

    // ICSCompilerHost
    STDMETHOD(ReportErrors)(ICSErrorContainer *pErrors);
    STDMETHOD(GetSourceModule)(PCWSTR pszFile, BOOL fNeedChecksum, ICSSourceModule **ppModule);
    STDMETHOD_(void, OnConfigChange)(long iOptionID, VARIANT vtValue) {}
    STDMETHOD_(void, OnCatastrophicError)(BOOL fException, DWORD dwException, void *pAddress);

    // ICSCommandLineCompilerHost
    STDMETHOD_(void, NotifyBinaryFile)(LPCWSTR pszFileName);
    STDMETHOD_(void, NotifyMetadataFile)(LPCWSTR pszFileName, LPCWSTR pszCorSystemDirectory);
    STDMETHOD(GetBugReportFileName)(LPWSTR pszFileName, DWORD cchLength);

};
