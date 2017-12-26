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
#include "fusionp.h"
#include "list.h"
#include "adl.h"
#include "asmimprt.h"
#include "asm.h"
#include "cblist.h"
#include "asmint.h"
#include "helpers.h"
#include "appctx.h"
#include "actasm.h"
#include "naming.h"
#include "dbglog.h"
#include "lock.h"
#include "memoryreport.h"
#include "util.h"
#include "corpriv.h"
#include "dlwrap.h"

extern List<CAssemblyDownload *>              *g_pDownloadList;
extern CRITSEC_COOKIE                          g_csDownload;

HRESULT CAssemblyDownload::Create(CAssemblyDownload **ppadl,
                                  IDownloadMgr *pDLMgr,
                                  ICodebaseList *pCodebaseList,
                                  CDebugLog *pdbglog,
                                  LONGLONG llFlags)
{
    HRESULT                    hr = S_OK;
    CAssemblyDownload         *padl = NULL;
        
    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownload");

    if (!ppadl || !pCodebaseList) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Create download object

    padl = NEW(CAssemblyDownload(pCodebaseList, pDLMgr, pdbglog, llFlags));
    if (!padl) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = padl->Init();
    if (FAILED(hr)) {
        SAFEDELETE(padl);
        goto Exit;
    }

    // Done. Give back the pointer to the newly created download object.

    *ppadl = padl;
    padl->AddRef();

Exit:
    return hr;
}

CAssemblyDownload::CAssemblyDownload(ICodebaseList *pCodebaseList,
                                     IDownloadMgr *pDLMgr,
                                     CDebugLog *pdbglog,
                                     LONGLONG llFlags)
: _cRef(0)
, _hrResult(S_OK)
, _state(ADLSTATE_INITIALIZE)
, _pwzUrl(NULL)
, _pDLMgr(pDLMgr)
, _pCodebaseList(pCodebaseList)
, _pdbglog(pdbglog)
, _cs(NULL)
, _llFlags(llFlags)
{
    _dwSig = 0x444d5341; /* 'DMSA' */

    if (_pCodebaseList) {
        _pCodebaseList->AddRef();
    }

    if (_pDLMgr) {
        _pDLMgr->AddRef();
    }

    if (_pdbglog) {
        _pdbglog->AddRef();
    }
}

CAssemblyDownload::~CAssemblyDownload()
{
    LISTNODE                      pos = NULL;

    SAFEDELETEARRAY(_pwzUrl);
    SAFERELEASE(_pCodebaseList);
    SAFERELEASE(_pDLMgr);
    SAFERELEASE(_pdbglog);

    pos = _clientList.GetHeadPosition();
    // If we still have client's we're in trouble. Not only would we be
    // leaking them, these clients were not removed from the list by
    // CompleteAll, so they'll never get the DONE notification.
    _ASSERTE(!pos); 

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }
}

HRESULT CAssemblyDownload::Init()
{
    HRESULT                           hr = S_OK;

    _cs = ClrCreateCriticalSection("Fusion: CAssemblyDownload", CrstFusionAssemblyDownload, CRST_DEFAULT);
    if (!_cs) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

STDMETHODIMP_(ULONG) CAssemblyDownload::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CAssemblyDownload::Release()
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }
    
    return ulRef;
}

HRESULT CAssemblyDownload::SetUrl(LPCWSTR pwzUrl)
{
    HRESULT                           hr = S_OK;
    CCriticalSection                  cs(_cs);

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownload");

    hr = cs.Lock();
    if (FAILED(hr)) {
        return hr;
    }

    SAFEDELETEARRAY(_pwzUrl);

    _pwzUrl = WSTRDupDynamic(pwzUrl);
    if (!_pwzUrl) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    cs.Unlock();
    return hr;
}

HRESULT CAssemblyDownload::AddClient(IAssemblyBindSink *pAsmBindSink, 
                                     BOOL bCallStartBinding)
{
    HRESULT                             hr = S_OK;
    CClientBinding                     *pclient = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownload");

    if (!pAsmBindSink) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Ref count released during CompleteAll
    pclient = NEW(CClientBinding(this, pAsmBindSink));
    if (!pclient) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = AddClient(pclient, bCallStartBinding);
    if (FAILED(hr)) {
        // We failed, so we never got added to the client list.
        SAFERELEASE(pclient);
    }

Exit:
    return hr;
}

HRESULT CAssemblyDownload::AddClient(CClientBinding *pclient, BOOL bCallStartBinding,
                                     BOOL bSwitchDownloader)
{
    HRESULT                         hr = S_OK;
    CCriticalSection                cs(_cs);

    if (!pclient) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Cannot add new client in these states
    // ADLSTATE_COMPLETE_ALL is okay because we will just
    // call OnStopBinding on the next message receipt.

    hr = cs.Lock(); // ensure state is correct
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (pclient->IsAborted()) {
        hr = E_ABORT;
        cs.Unlock();
        goto Exit;
    }

    if (_state == ADLSTATE_DONE) {
        
        // We are trying to piggyback on a download that is already finished.
        if (SUCCEEDED(_hrResult)) {
            // A download just finished, and installation worked.
            hr = HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
        }
        else {
            // The download/installation didn't succeed. The client must
            // initiate a new download altogether.
            hr = _hrResult;
        }
            
        goto LeaveCSExit;
    }
    else if (_state == ADLSTATE_ABORT) {
        hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        goto LeaveCSExit;
    }

    // Critical section protects us here too
    if (!_clientList.AddTail(pclient))
    {
        hr = E_OUTOFMEMORY;
        goto LeaveCSExit;
    }

    if (bSwitchDownloader) {
        pclient->SwitchDownloader(this);
    }

    cs.Unlock();

    if (bCallStartBinding) {
        pclient->CallStartBinding();
    }

    goto Exit;

LeaveCSExit:
    cs.Unlock();

Exit:
    return hr;    
}

HRESULT CAssemblyDownload::KickOffDownload(BOOL bFirstDownload)
{
    HRESULT                        hr = S_OK;
    WCHAR                          wzFilePath[MAX_PATH];
    BOOL                           bIsFileUrl = FALSE;
    CCriticalSection               cs(_cs);
    CCriticalSection               csDownload(g_csDownload);

    wzFilePath[0] = L'\0';

    // If we're aborted, or done, we can't do anything here
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_state == ADLSTATE_DONE) {
        hr = S_FALSE;
        cs.Unlock();
        goto Exit;
    }

    if (bFirstDownload) {
        CAssemblyDownload                  *padlDuplicate = NULL;

        // This is a top-level download (ie. not a probe download called from
        // DownloadNextCodebase

        hr = CheckDuplicate(&padlDuplicate);
        if (FAILED(hr)) {
            cs.Unlock();
            goto Exit;
        }
        else if (padlDuplicate) {
            LISTNODE                            pos = NULL;
            CClientBinding                     *pbinding = NULL;

            // There should only ever be one client because we only check
            // dupes before we start a real asm download, and this CAsmDownload
            // hasn't been added to the global download list yet.

            pos = _clientList.GetHeadPosition();
            _ASSERTE(pos && _clientList.GetCount() == 1);

            pbinding = _clientList.GetAt(pos);
            _ASSERTE(pbinding);

            cs.Unlock();

            // NOTE: AddClient is going to try to switch downloaders, which
            // will cause this object to be released, meaning this object
            // could be destroyed by the time AddClient returns.
            // AddRef ourselves here to make sure we stay alive long
            // enough to cleanup.
            
            AddRef();
            hr = padlDuplicate->AddClient(pbinding, FALSE, TRUE);
            if (hr == E_ABORT) {
                // Client actually aborted. So the in-progress duplicate
                // did not accept this client. By this time, the real
                // abort should have been executed on this object, so
                // we should be in our done state.

                _ASSERTE(!_clientList.GetHeadPosition());
                _ASSERTE(_state == ADLSTATE_DONE);
                
                // Return E_PENDING to indicate to the caller
                // that the result will be reported via the bind sink
                // (which would have been abort).

                SAFERELEASE(padlDuplicate);
                hr = E_PENDING;
                Release();

                goto Exit;
            }
            else if (hr == S_OK) {
                // Successfully transferred ourselves to the new client
                // Return E_PENDING to client so it knows it will
                // will get the result reported back by the sink.

                // This object is very likely dead now.

                _clientList.RemoveAll();
                SAFERELEASE(padlDuplicate);
                hr = E_PENDING;
                Release();

                goto Exit;
            }
                
            // If we get here, then it means that the
            // old downloader didn't accept this client for
            // other reasons (likely too late to add a new client; it
            // may already been in a done or aborted state itself).
            // In this case, fall through and continue to existing
            // bind as if there was no duplicate.

            Release();
            SAFERELEASE(padlDuplicate);

            hr = cs.Lock();
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        // Not a duplicate. Add ourselves to the global download list.
        
        hr = csDownload.Lock();
        if (FAILED(hr)) {
            cs.Unlock();
            goto Exit;
        }

        AddRef();
        if (!g_pDownloadList->AddTail(this))
        {
            hr = E_OUTOFMEMORY;
            cs.Unlock();
            csDownload.Unlock();
            goto Exit;
        }

        csDownload.Unlock();
        cs.Unlock();
    }
    else {
        cs.Unlock();
    }

    hr = GetNextCodebase(&bIsFileUrl, wzFilePath, MAX_PATH);
    if (FAILED(hr)) {
         if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
             hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
         }

        _hrResult = hr;
        hr = DownloadComplete(hr, NULL, NULL, FALSE);

        // Not really pending, just tell client the result is reported via
        // bind sink.

        if (SUCCEEDED(hr)) {
            hr = E_PENDING;
        }
        
        goto Exit;
    }

    if (bIsFileUrl) {
        hr = DownloadComplete(S_OK, wzFilePath, NULL, FALSE);

        // We're not really pending, but E_PENDING means that the client
        // will get the IAssembly via the bind sink (not the ppv returned
        // in the call to BindToObject).

        if (SUCCEEDED(hr)) {
            hr = E_PENDING; 
        }
        goto Exit;
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        goto Exit;
    }

Exit:
    if (FAILED(hr) && hr != E_PENDING) {
        _hrResult = hr;

        // Fatal error!
        
        // If we added ourselves to the download list, we should remove
        // ourselves immediately!

        ReleaseFromGlobalDownloadList();
    }

    return hr;
}


/* static */
void CALLBACK CAssemblyDownload::RealHttpDownload(ULONG_PTR dwParam)
{
    // this is called by an APC, we have an arbitrary amount of stack remaining...
    BEGIN_ENTRYPOINT_VOIDRET;
}

HRESULT CAssemblyDownload::ReleaseFromGlobalDownloadList()
{
    HRESULT hr = S_OK;
    LISTNODE         listnode;
    CCriticalSection csDL(g_csDownload);

    hr= csDL.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    listnode = g_pDownloadList->Find(this);
    if (listnode) {
        g_pDownloadList->RemoveAt(listnode);
        // release ourselves since we are removing from the global dl list
        Release();
    }

Exit:
    return hr;
}

HRESULT CAssemblyDownload::GetNextCodebase(BOOL *pbIsFileUrl, __out_ecount(cbLen) LPWSTR wzFilePath,
                                           DWORD cbLen)
{
    HRESULT                                 hr = S_OK;
    LPWSTR                                  wzNextCodebase = NULL;
    DWORD                                   cbCodebase;
    DWORD                                   dwSize;
    BOOL                                    bIsFileUrl = FALSE;
    DWORD                                   dwFlags = 0;
    CCriticalSection                        cs(_cs);

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownload");

    _ASSERTE(pbIsFileUrl && wzFilePath);

    *pbIsFileUrl = FALSE;

    for (;;) {

        cbCodebase = 0;
        hr = _pCodebaseList->GetCodebase(0, &dwFlags, NULL, &cbCodebase);
        if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            // could not get codebase
            hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
            goto Exit;
        }
    
        wzNextCodebase = NEW(WCHAR[cbCodebase]);
        if (!wzNextCodebase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    
        hr = _pCodebaseList->GetCodebase(0, &dwFlags, wzNextCodebase, &cbCodebase);
        if (FAILED(hr)) {
            goto Exit;
        }
    
        hr = _pCodebaseList->RemoveCodebase(0);
        if (FAILED(hr)) {
            goto Exit;
        }
    
        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_ATTEMPT_NEW_DOWNLOAD, wzNextCodebase);

        // Check Loader location access permission
        if (dwFlags & ASMLOC_CODEBASE_HINT) {
            hr = RuntimeCheckLocationAccess(wzNextCodebase);
            if (FAILED(hr)) {
                DEBUGOUT2(_pdbglog, 0, ID_FUSLOG_LOADER_PERMISSION_CHECK_FAILURE, wzNextCodebase, hr);
                goto Exit;
            }
        }

        // Check if we are a UNC or file:// URL. If we are, we don't have
        // to do a download, and can call setup right away.

        bIsFileUrl = UrlIsW(wzNextCodebase, URLIS_FILEURL);
        if (bIsFileUrl) {
            BOOL                  bExists;

            dwSize = cbLen;
            if (FAILED(PathCreateFromUrlWrap(wzNextCodebase, wzFilePath, &dwSize, 0))) {
                wzFilePath[0] = L'\0';
            }
            else {
                // Always store the extension as lower case.
                LPWSTR pwzExtension = PathFindExtension(wzFilePath);
                if (pwzExtension && *pwzExtension) {
                    if (!FusionCompareStringI(pwzExtension, L".DLL")) {
                        memcpy(pwzExtension, L".dll", sizeof(L".dll"));
                    }
                    else if (!FusionCompareStringI(pwzExtension, L".EXE")) {
                        memcpy(pwzExtension, L".exe", sizeof(L".exe"));
                    }
                }
            }

                if (!wzFilePath[0]) {
                    bExists = FALSE;
                } else {
                    hr = CheckFileExistence(wzFilePath, &bExists, NULL);
                    if (FAILED(hr)) {
                        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_CODEBASE_RETRIEVE_FAILURE, hr);
                        _hrResult = hr;
                        goto Exit;
                    }
                }
            
            if (!bExists) {
                // File doesn't exist. Try the next URL.
                ReportProgress(dwFlags, 0, 0, ASM_NOTIFICATION_ATTEMPT_NEXT_CODEBASE,
                               (LPCWSTR)wzNextCodebase, _hrResult);

                // Re-check state

                if (FAILED(cs.Lock())) {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }

                if (_state == ADLSTATE_DONE) {
                    // We could get here if we were aborted.

                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                    cs.Unlock();
                    goto Exit;
                }

                cs.Unlock();

                _hrResult = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

                SAFEDELETEARRAY(wzNextCodebase);
                continue;
            }
        }
        else {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            goto Exit;
        }

        break;
    }

    *pbIsFileUrl = bIsFileUrl;
    hr = PrepNextDownload(wzNextCodebase, dwFlags);
    
Exit:
    SAFEDELETEARRAY(wzNextCodebase);

    return hr;
}

HRESULT CAssemblyDownload::DownloadComplete(HRESULT hrResult,
                                            LPCOLESTR pwzFileName,
                                            const FILETIME *pftLastMod,
                                            BOOL bTerminate)
{
    CCriticalSection           cs(_cs);

    // Terminate the protocol


    _hrResult = cs.Lock();
    if (FAILED(_hrResult)) {
        goto Exit;
    }
    
    if (_state == ADLSTATE_DONE) {
        _hrResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        cs.Unlock();
        goto Exit;
    }
    else if (_state == ADLSTATE_ABORT) {
        // Only happens from the fatal abort case
        _hrResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    else {
        _state = ADLSTATE_DOWNLOAD_COMPLETE;
        _hrResult = hrResult;
    }

    cs.Unlock();

    if (SUCCEEDED(hrResult)) {
        // Download successful, change to next state.
        _ASSERTE(pwzFileName);

        _hrResult = cs.Lock();
        if (FAILED(_hrResult)) {
            goto Exit;
        }

        if (_state != ADLSTATE_ABORT) {
            _state = ADLSTATE_SETUP;
        }

        cs.Unlock();

        hrResult = DoSetup(pwzFileName, pftLastMod);
        if (hrResult == S_FALSE) {
            hrResult = DownloadNextCodebase();
        }
    }
    else {
        // Failed Download. 
        if (_hrResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            hrResult = DownloadNextCodebase();
        }
        else {
            // This is the fatal abort case
            CompleteAll(NULL, NULL);
        }
    }

Exit:
    return hrResult;
}

HRESULT CAssemblyDownload::DownloadNextCodebase()
{
    HRESULT                 hr = S_OK;
    DWORD                   dwNumCodebase;

    _pCodebaseList->GetCount(&dwNumCodebase);

    if (dwNumCodebase) {
        // Try next codebase

        hr = KickOffDownload(FALSE);
    }
    else {
        IUnknown                            *pUnk = NULL;

        // No more codebases remaining
        
        if (_pDLMgr) {
            hr = _pDLMgr->ProbeFailed(&pUnk);
            if (hr == S_OK) {
                if (pUnk) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_PROBE_FAIL_BUT_ASM_FOUND);
                }
                _hrResult = S_OK;
            }
            else if (hr == S_FALSE) {
                // Probing failed, but we were redirected to a new codebase.

                _pCodebaseList->GetCount(&dwNumCodebase);
                _ASSERTE(dwNumCodebase);

                hr = KickOffDownload(FALSE);

                goto Exit;
            }
            else {
                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                _hrResult = hr;
            }
        }

        CompleteAll(pUnk, NULL);
        SAFERELEASE(pUnk);
    }

Exit:
    return hr;
}

HRESULT CAssemblyDownload::PrepNextDownload(LPCWSTR pwzNextCodebase, DWORD dwFlags)
{
    HRESULT                                  hr = S_OK;
    CCriticalSection                         cs(_cs);


    // Set the new URL
    
    hr = SetUrl(pwzNextCodebase);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Notify all clients that we are trying the next codebase

    ReportProgress(dwFlags, 0, 0, ASM_NOTIFICATION_ATTEMPT_NEXT_CODEBASE,
                   (LPCWSTR)_pwzUrl, _hrResult);

    // Re-initialize our state
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_state == ADLSTATE_DONE) {
        // We could get here if we were aborted

        hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        cs.Unlock();
        goto Exit;
    }
        
    _state = ADLSTATE_INITIALIZE;

    cs.Unlock();

Exit:
    return hr;
}

HRESULT CAssemblyDownload::DoSetup(LPCOLESTR pwzFileName, const FILETIME *pftLastMod)
{
    HRESULT                            hr = S_OK;
    IUnknown                          *pUnk = NULL;
    IUnknown                          *pAsmNI = NULL;
    CCriticalSection                   cs(_cs);


    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_state == ADLSTATE_ABORT) {
        _hrResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        hr = _hrResult;

        cs.Unlock();
        CompleteAll(NULL, NULL);
        goto Exit;
    }
    cs.Unlock();

    if (_pDLMgr) {
        _hrResult = _pDLMgr->DoSetup(_pwzUrl, pwzFileName, pftLastMod, &pUnk, &pAsmNI);
        if (_hrResult == S_FALSE) {
            hr = cs.Lock();
            if (FAILED(hr)) {
                goto Exit;
            }

            _state = ADLSTATE_DOWNLOADING;

            cs.Unlock();

            hr = S_FALSE;
            goto Exit;
        }
    }
    else {
        _hrResult = S_OK;
    }

    if (FAILED(_hrResult)) {
        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_ASM_SETUP_FAILURE, _hrResult);
        _pCodebaseList->RemoveAll();
    }

    // Store _hrResult, since it is possible that after CompleteAll, this
    // object may be destroyed. See note in CompleteAll code.

    hr = _hrResult;

    CompleteAll(pUnk, pAsmNI);

    SAFERELEASE(pUnk);
    SAFERELEASE(pAsmNI);

Exit:
    return hr;
}

HRESULT CAssemblyDownload::CompleteAll(IUnknown *pUnk, IUnknown *pAsmNI)
{
    HRESULT                       hr = S_OK;
    LISTNODE                      pos = 0;
    CClientBinding               *pclient = NULL;
    CCriticalSection              cs(_cs);
    CCriticalSection              csDownload(g_csDownload);
    FusionBindInfo                bindInfo;

    memset(&bindInfo, 0, sizeof(bindInfo));

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_state >= ADLSTATE_COMPLETE_ALL) {
        hr = _hrResult;
        cs.Unlock();
        goto Exit;
    }

    _state = ADLSTATE_COMPLETE_ALL;
    cs.Unlock();

    hr = ReleaseFromGlobalDownloadList();
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = _pDLMgr->CacheBindingResult(_hrResult);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = _pDLMgr->GetBindInfo(&bindInfo);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (FAILED(_hrResult) && !IsLoggingNeeded() && !(_llFlags & ASM_BINDF_DO_NOT_PROBE_NATIVE_IMAGE)) {
        SAFERELEASE(bindInfo.pdbglog);
        CreateInformationalLogObject();
        bindInfo.pdbglog = g_pInformationalLog;
        if (bindInfo.pdbglog) {
            bindInfo.pdbglog->AddRef();
        }
    }

    // AddRef ourselves because this object may be destroyed after the
    // following loop. We send the DONE notification to the client, who
    // will probably release the IBinding. This decreases the ref count on
    // the CClientBinding to 1, and we will then immediately release the
    // remaining count on the CClientBinding. This causes us to Release
    // this CAssemblyDownload.
    //
    // It is possible that the only ref count left on the CAssemblyDownload
    // after this block is held by the download protocol hook
    // (COInetProtocolHook). If he has already been released, this object
    // will be gone!
    //
    // Under normal circumstances, it seems that this doesn't usually happen.
    // That is, the COInetProtocolHook usually is released well after this
    // point, so this object is kept alive, however, better safe than sorry.
    //
    // Also, if this is file://, it's ok because BTO is still on the stack
    // and BTO has a ref count on this obj until BTO retruns (ie. this
    // small scenario won't happen in file:// binds).
    //
    // Need to be careful when we unwind the stack here that we don't
    // touch any member vars.
    
    AddRef();

    for (;;) {
        hr = cs.Lock();
        if (FAILED(hr)) {
            goto Exit;
        }

        pos = _clientList.GetHeadPosition();
        if (!pos) {
            _state = ADLSTATE_DONE;
            cs.Unlock();
            break;
        }
        pclient = _clientList.GetAt(pos);

        _ASSERTE(pclient);
        _ASSERTE(pclient->GetBindSink());

        _clientList.RemoveAt(pos);

        cs.Unlock();

        // Report bind log available

        pclient->GetBindSink()->OnProgress(ASM_NOTIFICATION_BIND_INFO,
                                           S_OK, NULL, 0, 0, &bindInfo, NULL);

        // Report native image done notification
        if (pAsmNI) {
            pclient->GetBindSink()->OnProgress(ASM_NOTIFICATION_NATIVE_IMAGE_DONE,
                                            S_OK, NULL, 0, 0, NULL, pAsmNI);
        }        
        // Report done notificaton

        pclient->GetBindSink()->OnProgress(ASM_NOTIFICATION_DONE,
                                           _hrResult, NULL, 0, 0, NULL,
                                           pUnk);
        pclient->Release();
    }

    if (g_dwForceLog || (_pDLMgr->LogResult() == S_OK && FAILED(_hrResult)) ||
        _pDLMgr->LogResult() == E_FAIL) {
        if (_pdbglog) {
            _pdbglog->SetResultCode(FUSION_BIND_LOG_CATEGORY_DEFAULT,_hrResult);
        }

        DUMPDEBUGLOG(_pdbglog, g_dwLogLevel);
    }
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    _state = ADLSTATE_DONE;
    cs.Unlock();

    

    // It is possible that we're going to be destroyed here. See note
    // above.

    Release();

Exit:
    SAFERELEASE(bindInfo.pdbglog);
    SAFERELEASE(bindInfo.pNamePolicy);

    return hr;
}

HRESULT CAssemblyDownload::FatalAbort(HRESULT hrResult)
{
    HRESULT                       hr = S_OK;


    return hr;
}

HRESULT CAssemblyDownload::RealAbort(CClientBinding *pclient)
{
    HRESULT                     hr = S_OK;
    LISTNODE                    pos = 0;
    int                         iNum = 0;
    CCriticalSection            cs(_cs);
    
    // Critical section ensures integrity of list, and ensures the
    // state variable is correct.

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_state >= ADLSTATE_COMPLETE_ALL) {
        hr = E_PENDING;  // OnStopBinding is pending. Can't really abort.
        goto LeaveCSExit;
    }

    iNum = _clientList.GetCount();

    if (iNum == 1) {
        // This is the last client interested in the download.
        // We must really do an abort (or try at least).

            // We don't even have a pProt yet (abort was called on the
            // stack).
            _state = ADLSTATE_ABORT;
            _hrResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
            cs.Unlock();

            CompleteAll(NULL, NULL);

            goto Exit;
    }
    else {
        // There is more than one client interested in this download
        // but this particular one wants to abort. Just remove him from
        // the notification list, and call the OnStopBinding. 

        _ASSERTE((iNum > 1) && "We have no clients!");
        pos = _clientList.Find(pclient);

        _ASSERTE(pos && "Can't find client binding in CAssemblyDownload client list");
        _clientList.RemoveAt(pos);


        _ASSERTE(pclient->GetBindSink());

        cs.Unlock();

        pclient->GetBindSink()->OnProgress(ASM_NOTIFICATION_DONE,
                                           HRESULT_FROM_WIN32(ERROR_CANCELLED),
                                           NULL, 0, 0, NULL, NULL);
        pclient->Release();

        goto Exit;
    }

LeaveCSExit:
    cs.Unlock();

Exit:
    return hr;
}

HRESULT CAssemblyDownload::ReportProgress(ULONG ulStatusCode,
                                          ULONG ulProgress,
                                          ULONG ulProgressMax,
                                          DWORD dwNotification,
                                          LPCWSTR wzNotification,
                                          HRESULT hrNotification)
{
    HRESULT                               hr = S_OK;
    LISTNODE                              pos = NULL;
    LISTNODE                              posCur = NULL;
    CClientBinding                       *pclient = NULL;
    CClientBinding                       *pNext = NULL;
    IAssemblyBindSink                    *pbindsink = NULL;
    CCriticalSection                      cs(_cs);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }
    
    pos = _clientList.GetHeadPosition();
    pclient = _clientList.GetAt(pos);
    pclient->Lock();

    cs.Unlock();

    if (ulStatusCode == ASMLOC_CODEBASE_HINT) {
        ulProgress = (ULONG) -1;
        ulProgressMax = (ULONG) -1;
    }

    while (pos) {
        posCur = pos;
        pbindsink = pclient->GetBindSink();
        _ASSERTE(pbindsink);
        pbindsink->OnProgress(dwNotification, hrNotification, wzNotification,
                              ulProgress, ulProgressMax, NULL, NULL);

        hr = cs.Lock();
        if (FAILED(hr)) {
            goto Exit;
        }

        _clientList.GetNext(pos);
        if (pos) {
            pNext = _clientList.GetAt(pos);
            pNext->Lock();
        }
        else {
            pNext = NULL;
        }

        pclient->UnLock();

        if (pclient->IsPendingDelete()) {
            cs.Unlock();
            RealAbort(pclient);
        }
        else {
            cs.Unlock();
        }

        pclient = pNext;
    }

Exit:
    return hr;
}
                
HRESULT CAssemblyDownload::ClientAbort(CClientBinding *pclient)
{
    HRESULT                          hr = S_OK;
    CCriticalSection                 cs(_cs);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (!pclient->LockCount()) {
        cs.Unlock();
        hr = RealAbort(pclient);
    }
    else {
        pclient->SetPendingDelete(TRUE);
        hr = E_PENDING;
        cs.Unlock();
    }

Exit:
    return hr;
}

HRESULT CAssemblyDownload::PreDownload(
                        BOOL bCallCompleteAll, 
                        void **ppv,
                        void **ppvNI)
{
    HRESULT                                       hr = S_OK;
    IUnknown                                     *pUnk = NULL;
    IUnknown                                     *pAsmNI = NULL;
    CCriticalSection                              cs(_cs);

    if ((!bCallCompleteAll && !ppv)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Check to make sure we're not in abort state
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    // If someone aborted, then we would have already reported the abort
    // back to the client. If it was the last client, we would have already
    // transitioned to the DONE state as well. In this case, there's nothing
    // to do.

    if (_state == ADLSTATE_DONE) {
        // Tell client the result was reported back by the sink.
        hr = E_PENDING;
        cs.Unlock();
        goto Exit;
    }

    cs.Unlock();
    
    // Do a lookup in the cache and return an IAssembly object if found.

    hr = _pDLMgr->PreDownloadCheck((void **)&pUnk, (void **)&pAsmNI);
    if (hr == S_OK) {
        // We hit in doing the cache lookup
        _ASSERTE(pUnk);

        hr = cs.Lock();
        if (FAILED(hr)) {
            goto Exit;
        }

        if (_state >= ADLSTATE_COMPLETE_ALL || _state == ADLSTATE_ABORT) {
            pUnk->Release();

            if (bCallCompleteAll) {
                hr = S_FALSE;
            }
            else {
                hr = E_ABORT;
            }
            cs.Unlock();
            goto Exit;
        }
            
        cs.Unlock();
        if (bCallCompleteAll) {
            _hrResult = S_OK;
            CompleteAll(pUnk, pAsmNI);
        }
        else {
            *ppv = pUnk;
            pUnk->AddRef();

            if (ppvNI) {
                *ppvNI = pAsmNI;
                if (pAsmNI) {
                    pAsmNI->AddRef();
                }
            }
        }

        pUnk->Release();
        if (pAsmNI) {
            pAsmNI->Release();
        }

        hr = S_FALSE;
        goto Exit;
    }
    else if (FAILED(hr)) {
        // Catastrophic error doing predownload check
        DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_PREDOWNLOAD_FAILURE, hr);
        goto Exit;
    }

    hr = S_OK;
    
Exit:
    if (FAILED(hr) && hr != E_PENDING) {
        _hrResult = hr;
    }

    return hr;
}

HRESULT CAssemblyDownload::CheckDuplicate(CAssemblyDownload **ppadlDuplicate)
{
    HRESULT                             hr = S_OK;
    LISTNODE                            listnode = NULL;
    CAssemblyDownload                  *padlCur = NULL;
    IDownloadMgr                       *pDLMgrCur = NULL;
    CCriticalSection                    csDownload(g_csDownload);
    int                                 i;
    int                                 iCount;

    _ASSERTE(_pDLMgr);
    _ASSERTE(ppadlDuplicate && !*ppadlDuplicate);

    hr = csDownload.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    listnode = g_pDownloadList->GetHeadPosition();
    iCount = g_pDownloadList->GetCount();
    
    for (i = 0; i < iCount; i++) {
        padlCur = g_pDownloadList->GetNext(listnode);
        _ASSERTE(padlCur);

        hr = padlCur->GetDownloadMgr(&pDLMgrCur);
        _ASSERTE(hr == S_OK);

        hr = _pDLMgr->IsDuplicate(pDLMgrCur);
        if (hr == S_OK) {
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_DOWNLOAD_PIGGYBACK);
            SAFERELEASE(pDLMgrCur);

            *ppadlDuplicate = padlCur;
            (*ppadlDuplicate)->AddRef();

            csDownload.Unlock();
            
            goto Exit;
        }

        SAFERELEASE(pDLMgrCur);
    }

    csDownload.Unlock();

Exit:
    
    return hr;
}

HRESULT CAssemblyDownload::GetDownloadMgr(IDownloadMgr **ppDLMgr)
{
    HRESULT                                     hr = S_OK;

    if (!ppDLMgr) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppDLMgr = _pDLMgr;
    _ASSERTE(*ppDLMgr);

    (*ppDLMgr)->AddRef();

Exit:
    return hr;
}

HRESULT CAssemblyDownload::SetResult(HRESULT hrResult)
{
    _hrResult = hrResult;

    return S_OK;
}

DWORD WINAPI DownloadThreadLoop(LPVOID lpParameter)
{
    HRESULT hr = S_OK;
    return hr;
}

extern CRITSEC_COOKIE g_csInitClb;

HANDLE  g_hDownloadThreadHandle;

HRESULT InitializeDownloadThread()
{
    HRESULT hr = S_OK;
    static DWORD dwDownloadThreadId = 0;
    CCriticalSection cs(g_csInitClb);

    if (dwDownloadThreadId != 0) {
        goto Exit;
    }

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    if (dwDownloadThreadId != 0) {
        goto Exit;
    }

    g_hDownloadThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadThreadLoop, NULL, 0, &dwDownloadThreadId);

    if (g_hDownloadThreadHandle == NULL) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    // remove the ref count we have. 
    // CloseHandle(g_hDownloadThreadHandle);

Exit:
    return hr;
}

