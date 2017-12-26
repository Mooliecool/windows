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
//+---------------------------------------------------------------------------
//
//  File:       buildscr.h
//
//  Contents:   Header file defining the objects needed to interface with
//              the MTScript engine.
//
//----------------------------------------------------------------------------

class CProcessSink : public IScriptedProcessSink
{
public:
    CProcessSink();
   ~CProcessSink() {}

    // IUnknown methods

    STDMETHOD(QueryInterface) (REFIID riid, LPVOID * ppv);
    STDMETHOD_(ULONG, AddRef) (void);
    STDMETHOD_(ULONG, Release) (void);

    // IScriptedProcessSink methods

    STDMETHOD(RequestExit)();
    STDMETHOD(ReceiveData)(wchar_t *pszType,
                           wchar_t *pszData,
                           long *plReturn);

private:
    ULONG _ulRefs;
};

