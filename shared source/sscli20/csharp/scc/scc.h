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

#ifndef __SCC_H__
#define __SCC_H__

#include "csiface.h"
#include "posdata.h"
#include "msgsids.h"
#include "errorids.h"
#define CLIENT_IS_CSC
#include "cscommon.h"
#undef CLIENT_IS_CSC

////////////////////////////////////////////////////////////////////////////////
// CSourceText -- implements ICSSourceText for a text file

class CSourceText : public ICSSourceText
{
private:
    WCAllocBuffer    m_textBuffer;
    PWSTR       m_pszName;
    LONG        m_iRef;
    Checksum    m_checksum;

public:

    CSourceText ();
    ~CSourceText ();

    HRESULT     Initialize (PCWSTR pszFileName, BOOL fComputeChecksum, UINT uiCodePage = 0);
    void        FreeText () { m_textBuffer.Clear(); }

    // IUnknown
    STDMETHOD_(ULONG, AddRef)() { return InterlockedIncrement (&m_iRef); }
    STDMETHOD_(ULONG, Release)() { ULONG i = InterlockedDecrement (&m_iRef); if (i == 0) delete this; return i; }
    STDMETHOD(QueryInterface)(REFIID riid, void **ppObj) { return E_NOINTERFACE; }

    // ICSSourceText
    STDMETHOD(CloneInMemory)(ICSSourceText **ppClone) { return E_NOTIMPL; }
    STDMETHOD(GetText)(PCWSTR *ppszText, POSDATA *pposEnd) { *ppszText = m_textBuffer; if (pposEnd != NULL) { pposEnd->iLine = 0; pposEnd->iChar = 0; } return S_OK; }
    STDMETHOD(GetName)(PCWSTR *ppszName) { *ppszName = m_pszName; return S_OK; }
    STDMETHOD(GetLineCount)(long *piLines) { return E_NOTIMPL; }   // Let compiler figure out the line count...
    STDMETHOD(ReleaseText)() { FreeText (); return S_OK; }
    STDMETHOD(AdviseChangeEvents)(ICSSourceTextEvents *pSink, DWORD_PTR *pdwCookie) { return E_NOTIMPL; }
    STDMETHOD(UnadviseChangeEvents)(DWORD_PTR dwCookie) { return E_NOTIMPL; }
    STDMETHOD(GenerateChecksum)() { return m_checksum.cbData == 0 ? E_UNEXPECTED : S_FALSE; }
    STDMETHOD(GetChecksum)(Checksum * checksum);
    STDMETHOD(ReportExceptionalOperation)() { return S_OK; }

    void        OutputToReproFile();
};

#endif
