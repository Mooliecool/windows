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
#ifndef __CLBIND_H_INCLUDED__
#define __CLBIND_H_INCLUDED__

class CAssemblyDownload;

//
// CClientBinding
//

class CClientBinding : public IAssemblyBinding {
    public:
        CClientBinding(CAssemblyDownload *pad,
                       IAssemblyBindSink *pbindsink);
        virtual ~CClientBinding();

        // IUnknown methods

        STDMETHODIMP QueryInterface(REFIID riid,void ** ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IAssemblyBinding methods
        
        STDMETHODIMP Control(HRESULT hrControl);
        STDMETHODIMP DoDefaultUI(HWND hWnd, DWORD dwFlags);

        // Helpers

        HRESULT CallStartBinding();
        HRESULT SwitchDownloader(CAssemblyDownload *padl);
        BOOL IsAborted();

        IAssemblyBindSink *GetBindSink();
        void SetPendingDelete(BOOL bPending);
        int LockCount();
        int Lock();
        int UnLock();
        BOOL IsPendingDelete();


    private:
        DWORD                                         _dwSig;
        LONG                                          _cRef;
        IAssemblyBindSink                            *_pbindsink;
        CAssemblyDownload                            *_padl;
        ULONG                                         _cLocks;
        BOOL                                          _bPendingDelete;
        BOOL                                          _bAborted;
};
        
#endif

