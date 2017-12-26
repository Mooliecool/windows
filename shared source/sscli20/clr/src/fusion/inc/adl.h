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
#ifndef __ADL_H_
#define __ADL_H_

#include "asmcache.h"
#include "clbind.h"
#include "list.h"
#include "fusionpriv.h"
#include "dbglog.h"

typedef enum tagADLSTATE {
    ADLSTATE_INITIALIZE,
    ADLSTATE_DOWNLOADING,
    ADLSTATE_ABORT,
    ADLSTATE_DOWNLOAD_COMPLETE,
    ADLSTATE_SETUP,
    ADLSTATE_COMPLETE_ALL,
    ADLSTATE_DONE
} ADLSTATE;

//
// CAssemblyDownload
//

class CAssemblyDownload {
    public:

        CAssemblyDownload(ICodebaseList *pCodebaseList, IDownloadMgr *pDLMgr,
                          CDebugLog *pdbglog, LONGLONG llFlags);
        virtual ~CAssemblyDownload();

        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        static HRESULT Create(CAssemblyDownload **ppadl,
                              IDownloadMgr *pDLMgr,
                              ICodebaseList *pCodebaseList,
                              CDebugLog *pdbglog,
                              LONGLONG llFlags);

        HRESULT AddClient(IAssemblyBindSink *pAsmBindSink, BOOL bCallStartBinding);
        HRESULT AddClient(CClientBinding *pclient, BOOL bCallStartBinding, BOOL bSwitchDownloader = FALSE);

        HRESULT SetUrl(LPCWSTR pwzUrl);

        HRESULT KickOffDownload(BOOL bFirstDownload);
        HRESULT DownloadComplete(HRESULT hrResult, 
                                LPCOLESTR pwzFileName, 
                                const FILETIME *pftLastMod,
                                BOOL bTerminate);
        HRESULT DoSetup(LPCOLESTR pwzFileName, const FILETIME *pftLastMod);
        HRESULT CompleteAll(IUnknown *pUnk, IUnknown *pAsmNI);
        HRESULT ClientAbort(CClientBinding *pclient);
        HRESULT FatalAbort(HRESULT hrResult);

        HRESULT ReportProgress(ULONG ulStatusCode, ULONG ulProgress,
                               ULONG ulProgressMax, DWORD dwNotification,
                               LPCWSTR wzNotification,
                               HRESULT hrNotification);

        HRESULT PreDownload(BOOL bCallCompleteAll, void **ppv, void **ppvNI);

        HRESULT GetDownloadMgr(IDownloadMgr **ppDLMgr);
        HRESULT SetResult(HRESULT hrResult);
                               
    private:
        HRESULT Init();

        HRESULT RealAbort(CClientBinding *pclient);
        HRESULT PrepNextDownload(LPCWSTR pwzNextCodebase, DWORD dwFlags);
        HRESULT DownloadNextCodebase();
        HRESULT CheckDuplicate(CAssemblyDownload **ppadlDuplicate);
        HRESULT GetNextCodebase(BOOL *pbIsFileUrl, 
                                __out_ecount(cbLen) LPWSTR wzFilePath,
                                DWORD cbLen);

        static void CALLBACK RealHttpDownload(ULONG_PTR dwParam);

        HRESULT ReleaseFromGlobalDownloadList();

    private:
        DWORD                                         _dwSig;
        LONG                                          _cRef;
        HRESULT                                       _hrResult;
        ADLSTATE                                      _state;
        List<CClientBinding *>                        _clientList;
        LPWSTR                                        _pwzUrl;
        IDownloadMgr                                 *_pDLMgr;
        ICodebaseList                                *_pCodebaseList;
        CDebugLog                                    *_pdbglog;
        CRITSEC_COOKIE                                _cs;
        LONGLONG                                      _llFlags;
};

DWORD WINAPI DownloadThreadLoop(LPVOID lpParameter);

HRESULT InitializeDownloadThread();

#endif  // __ADL_H_
