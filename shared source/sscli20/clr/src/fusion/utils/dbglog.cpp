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
#include <windows.h>
#include "fusionp.h"
#include "dbglog.h"
#include "helpers.h"
#include "util.h"
#include "lock.h"
#include "memoryreport.h"
#include "cache.h"
#include "utilcode.h"
#include "cordbpriv.h"

#include "dlwrap.h"

// Copy of CustomerDebugProbes::IsDebuggerAttached()
// in vm\vars.hpp. 
//
extern bool g_fProcessDetach;
extern DWORD g_CORDebuggerControlFlags;
BOOL IsDebuggerAttached()
{

#if DEBUGGING_SUPPORTED
    if ((g_CORDebuggerControlFlags & DBCF_ATTACHED) && !g_fProcessDetach)
        return TRUE;
#endif
    return FALSE;
}

CDebugLog *g_pInformationalLog;

// FUSION_BIND_LOG_CATEGORY_MAX + 1 to include clickonce log.
// ClickOnce is always the last one.
LPWSTR g_wzLogCategories[FUSION_BIND_LOG_CATEGORY_MAX+1] = {
        L"Default",
        L"NativeImage",
        L"ClickOnce"};

extern WCHAR g_wzEXEPath[MAX_PATH+1];
extern WCHAR g_FusionDllPath[MAX_PATH+1];

extern CRITSEC_COOKIE g_csBindLog;
extern DWORD g_dwForceLog;
extern DWORD g_dwLogFailures;

static DWORD CountEntities(LPCWSTR pwzStr)
{
    DWORD                       dwEntities = 0;

    _ASSERTE(pwzStr);

    while (*pwzStr) {
        if (*pwzStr == L'>' || *pwzStr == L'<') {
            dwEntities++;
        }

        pwzStr++;
    }

    return dwEntities;
}

HRESULT DumpMessage(HANDLE hFile, LPCWSTR pwzMsg)
{
    HRESULT                                        hr = S_OK;
    DWORD                                          dwLen = 0;
    DWORD                                          dwWritten = 0;
    CHAR                                           szBuf[MAX_DBG_STR_LEN];

    dwLen = WszWideCharToMultiByte(CP_UTF8, 0, pwzMsg, -1, szBuf, MAX_DBG_STR_LEN, NULL, false);
    if (!dwLen) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    // dwLen includes NULL terminator. We don't want to write this out.

    if (dwLen > 1) {
        dwLen--;

        if (!WriteFile(hFile, szBuf, dwLen, &dwWritten, NULL)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Exit;
        }
    }
    
Exit:
    return hr;
}


//
// CDebugLogElement Class
//

HRESULT CDebugLogElement::Create(DWORD dwDetailLvl, LPCWSTR pwzMsg,
                                 BOOL bEscapeEntities,
                                 CDebugLogElement **ppLogElem)
{
    HRESULT                                  hr = S_OK;
    CDebugLogElement                        *pLogElem = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    if (!ppLogElem || !pwzMsg) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppLogElem = NULL;

    pLogElem = NEW(CDebugLogElement(dwDetailLvl));
    if (!pLogElem) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pLogElem->Init(pwzMsg, bEscapeEntities);
    if (FAILED(hr)) {
        SAFEDELETE(pLogElem);
        goto Exit;
    }

    *ppLogElem = pLogElem;

Exit:
    return hr;
}
                                 
CDebugLogElement::CDebugLogElement(DWORD dwDetailLvl)
: _pszMsg(NULL)
, _dwDetailLvl(dwDetailLvl)
{
}

CDebugLogElement::~CDebugLogElement()
{
    SAFEDELETEARRAY(_pszMsg);
}

HRESULT CDebugLogElement::Init(LPCWSTR pwzMsg, BOOL bEscapeEntities)
{
    HRESULT                     hr = S_OK;
    const DWORD                 cchReplacementSize = sizeof("&gt;") - 1;
    LPWSTR                      pwzCur;
    DWORD                       dwLen;
    DWORD                       dwSize;
    DWORD                       dwEntities;
    DWORD                       i;
    LPWSTR                      pwzEnd;
    SIZE_T                      cch;

    _ASSERTE(pwzMsg);

    if (!bEscapeEntities) {
        _pszMsg = WSTRDupDynamic(pwzMsg);
        if (!_pszMsg) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }
    else {
        // Perform entity replacement on all ">" and "<" characters
    
        dwLen = lstrlenW(pwzMsg);
        dwEntities = CountEntities(pwzMsg);
    
        dwSize = dwLen + dwEntities * cchReplacementSize + 1;
    
        _pszMsg = NEW(WCHAR[dwSize]);
        if (!_pszMsg) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    
        pwzCur = _pszMsg;
        pwzEnd = _pszMsg + dwSize;
    
        for (i = 0; i < dwLen; i++) {
            cch = (SIZE_T)(pwzEnd - pwzCur);

            if (pwzMsg[i] == L'<') {
                hr = StringCchCopy(pwzCur, cch, L"&lt;");
                if (FAILED(hr)) {
                    goto Exit;
                }
                pwzCur += cchReplacementSize;
            }
            else if (pwzMsg[i] == L'>') {
                hr = StringCchCopy(pwzCur, cch, L"&gt;");
                if (FAILED(hr)) {
                    goto Exit;
                }
                pwzCur += cchReplacementSize;
            }
            else {
                *pwzCur++ = pwzMsg[i];
            }
        }
    
        *pwzCur = L'\0';
    }
    
Exit:
    return hr;
}

//
// CDebugLog Class
//

HRESULT CDebugLog::Create(IApplicationContext *pAppCtx, 
                          IAssemblyName *pName,
                          LPCWSTR szCodebase,
                          CDebugLog **ppdl)
{
    HRESULT                                   hr = S_OK;
    CDebugLog                                *pdl = NULL;

    _ASSERTE((pName || szCodebase) && ppdl);

    *ppdl = NULL;

    pdl = NEW(CDebugLog);
    if (!pdl) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pdl->Init(pAppCtx, pName, szCodebase);
    if (FAILED(hr)) {
        SAFEDELETE(pdl);
        goto Exit;
    }

    
    *ppdl = pdl;

Exit:
    return hr;
}

HRESULT CDebugLog::Create(IApplicationContext *pAppCtx, 
                          LPCWSTR szAsmName,
                          CDebugLog **ppdl)
{
    HRESULT                                   hr = S_OK;
    CDebugLog                                *pdl = NULL;

    _ASSERTE(szAsmName && ppdl);

    *ppdl = NULL;

    pdl = NEW(CDebugLog);
    if (!pdl) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pdl->Init(pAppCtx, szAsmName);
    if (FAILED(hr)) {
        SAFEDELETE(pdl);
        goto Exit;
    }


    *ppdl = pdl;

Exit:
    return hr;
}

CDebugLog::CDebugLog()
: _cRef(1)
, _pwzAsmName(NULL)
, _wzEXEName(NULL)
, _bWroteDetails(FALSE)
{
    for (DWORD i=0; i < FUSION_BIND_LOG_CATEGORY_MAX; i++){
        _hrResult[i] = S_OK;
    }
}

CDebugLog::~CDebugLog()
{
    LISTNODE                                 pos = NULL;
    CDebugLogElement                        *pLogElem = NULL;

    for(DWORD i = 0; i < FUSION_BIND_LOG_CATEGORY_MAX; i++) {
        pos = _listDbgMsg[i].GetHeadPosition();

        while (pos) {
            pLogElem = _listDbgMsg[i].GetNext(pos);
            SAFEDELETE(pLogElem);
        }

        _listDbgMsg[i].RemoveAll();

    }

    SAFEDELETEARRAY(_pwzAsmName);

    pos = _listHeaderMsg.GetHeadPosition();
    while(pos) {
        pLogElem = _listHeaderMsg.GetNext(pos);
        SAFEDELETE(pLogElem);
    }

    _listHeaderMsg.RemoveAll();

    SAFEDELETEARRAY(_wzEXEName);

    
}

HRESULT CDebugLog::Init(IApplicationContext *pAppCtx, IAssemblyName* pName, LPCWSTR szCodebase)
{
    HRESULT hr = S_OK;
    _ASSERTE(pName || szCodebase);

    hr = SetAsmName(pName, szCodebase);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = SetProperties(pAppCtx);

Exit:
    return hr;
}

HRESULT CDebugLog::Init(IApplicationContext *pAppCtx, LPCWSTR szAsmName)
{
    HRESULT hr = S_OK;
    _ASSERTE(szAsmName);

    _pwzAsmName = WSTRDupDynamic(szAsmName);
    if (!_pwzAsmName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = SetProperties(pAppCtx);

Exit:
    return hr;
}


HRESULT CDebugLog::SetProperties(IApplicationContext *pAppCtx)
{
    HRESULT                                  hr = S_OK;
    LPWSTR                                   wzAppName = NULL;

    // Get the executable name
    if (pAppCtx) {
        hr = ::AppCtxGetWrapper(pAppCtx, ACTAG_APP_NAME, &wzAppName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (wzAppName && lstrlenW(wzAppName)) {
        _wzEXEName = WSTRDupDynamic(wzAppName);
        if (!_wzEXEName) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }
    else {
        LPWSTR               wzFileName;

        // Didn't find EXE name in appctx. Use the .EXE name.
        wzFileName = PathFindFileName(g_wzEXEPath);
        _ASSERTE(wzFileName);

        _wzEXEName = WSTRDupDynamic(wzFileName);
        if (!_wzEXEName) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(wzAppName);

    return hr;
}

HRESULT CDebugLog::SetAsmName(IAssemblyName *pName, LPCWSTR szCodebase)
{
    HRESULT     hr = S_OK;
    DWORD       dwSize = 0;
    BOOL        bWhereRefBind = FALSE;
    LPWSTR      wzBuf = NULL;

    if(!pName) {
        bWhereRefBind = TRUE;
    }
    else {
        dwSize = 0;
        hr = pName->GetProperty(ASM_NAME_NAME, NULL, &dwSize);
        if(FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            goto Exit;
        }
        if (!dwSize) {
            bWhereRefBind = TRUE;
        }
        hr = S_OK;
    }

    if(bWhereRefBind) {
        if(!szCodebase) {
            hr = E_INVALIDARG;
            goto Exit;
        }

        wzBuf = NEW(WCHAR[MAX_URL_LENGTH+1]);
        if (!wzBuf) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        wzBuf[0] = L'\0';
        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(szCodebase, wzBuf, &dwSize, 0);
        if (SUCCEEDED(hr)) {
            _pwzAsmName = WSTRDupDynamic(wzBuf);
            if (!_pwzAsmName) {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }
        }
    }
    else {
        dwSize = 0;
        hr = pName->GetDisplayName(NULL, &dwSize, 0);
        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            goto Exit;
        }

        if (dwSize) {
            _pwzAsmName = NEW(WCHAR[dwSize]);
            if (!_pwzAsmName) {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }

            hr = pName->GetDisplayName(_pwzAsmName, &dwSize, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

Exit:
    SAFEDELETEARRAY(wzBuf);
    return hr;
}

//
// IUnknown
//

STDMETHODIMP CDebugLog::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT                          hr = S_OK;
    
    if (!ppv) 
        return E_POINTER;
    
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IFusionBindLog)) {
        *ppv = static_cast<IFusionBindLog *>(this);
    }
    else {
        *ppv = NULL;
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
}


STDMETHODIMP_(ULONG) CDebugLog::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CDebugLog::Release()
{
    ULONG            ulRef;

    ulRef = InterlockedDecrement(&_cRef);
    
    if (ulRef == 0) {
        delete this;
    }

    return ulRef;
}

//
// IFusionBindLog
//
STDMETHODIMP CDebugLog::SetResultCode(DWORD dwLogCategory, HRESULT hrResult)
{
    HRESULT hr = S_OK;

    if (dwLogCategory >= FUSION_BIND_LOG_CATEGORY_MAX) {
        return E_INVALIDARG;
    }

    _hrResult[dwLogCategory] = hrResult;

    return hr;
}

STDMETHODIMP CDebugLog::GetResultCode(DWORD dwLogCategory, HRESULT *pHr)
{
    if (!pHr || dwLogCategory >= FUSION_BIND_LOG_CATEGORY_MAX) {
        return E_INVALIDARG;
    }

    *pHr = _hrResult[dwLogCategory];

    return S_OK;
}

STDMETHODIMP CDebugLog::GetBindLog(DWORD dwDetailLevel, 
                    DWORD dwLogCategory, 
                    __out_bcount_opt(*pcbDebugLog) LPWSTR pwzDebugLog, 
                    __inout DWORD *pcbDebugLog)
{
    HRESULT                                  hr = S_OK;
    LISTNODE                                 pos = NULL;
    DWORD                                    cbReqd;
    CDebugLogElement                        *pLogElem = NULL;

    if (!pcbDebugLog || dwLogCategory >= FUSION_BIND_LOG_CATEGORY_MAX) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    pos = _listDbgMsg[dwLogCategory].GetHeadPosition();
    if (!pos) {
        // No entries in debug log!
        hr = S_FALSE;
        *pcbDebugLog = 0;
        goto Exit;
    }

    // Calculate total size (entries + new line chars + NULL)

    cbReqd = 0;

    pos = _listHeaderMsg.GetHeadPosition();
    while(pos) {
        pLogElem = _listHeaderMsg.GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLevel) {
            cbReqd += lstrlenW(pLogElem->_pszMsg) * sizeof(WCHAR);
            cbReqd += sizeof(L"\r\n");
        }
    }
   
    pos = _listDbgMsg[dwLogCategory].GetHeadPosition();
    while (pos) {
        pLogElem = _listDbgMsg[dwLogCategory].GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLevel) {
            cbReqd += lstrlenW(pLogElem->_pszMsg) * sizeof(WCHAR);
            cbReqd += sizeof(L"\r\n");
        }
    }

    cbReqd += sizeof(L""); // NULL char

    if (!pwzDebugLog || *pcbDebugLog < cbReqd) {
        *pcbDebugLog = cbReqd;

        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    *pwzDebugLog = L'\0';

    pos = _listHeaderMsg.GetHeadPosition();
    while(pos) {
        pLogElem = _listHeaderMsg.GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLevel) {
            hr = StringCbCat(pwzDebugLog, *pcbDebugLog, pLogElem->_pszMsg);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = StringCbCat(pwzDebugLog, *pcbDebugLog, L"\r\n");
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    pos = _listDbgMsg[dwLogCategory].GetHeadPosition();
    while (pos) {
        pLogElem = _listDbgMsg[dwLogCategory].GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLevel) {
            hr = StringCbCat(pwzDebugLog, *pcbDebugLog, pLogElem->_pszMsg);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            hr = StringCbCat(pwzDebugLog, *pcbDebugLog, L"\r\n");
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    _ASSERTE((DWORD)lstrlenW(pwzDebugLog) * sizeof(WCHAR) < cbReqd);

Exit:
    return hr;
}                                    

STDMETHODIMP CDebugLog::LogMessage(DWORD dwDetailLevel, DWORD dwLogCategory, LPCWSTR wzDebugStr)
{
    if (!IsLoggingNeeded()) {
        return S_FALSE;
    }

    if (dwLogCategory >= FUSION_BIND_LOG_CATEGORY_MAX) {
        return E_INVALIDARG;
    }

    return LogMessage(dwDetailLevel, dwLogCategory, wzDebugStr, FALSE, FALSE);
}

STDMETHODIMP CDebugLog::Flush(DWORD dwDetailLevel, DWORD dwLogCategory)
{
    if (dwLogCategory >= FUSION_BIND_LOG_CATEGORY_MAX) {
        return E_INVALIDARG;
    }

    // Don't log to wininet in when hosted. It is not safe.
    if (g_bFusionHosted && g_bLogToWininet) {
        return S_FALSE;
    }

    // dump if necessary
    if (g_dwForceLog || (g_dwLogFailures && FAILED(_hrResult[dwLogCategory]))) {
        return DumpDebugLog(dwDetailLevel, dwLogCategory, _hrResult[dwLogCategory]); 
    }

    return S_OK;
}

//
// CDebugLog helpers
//

HRESULT CDebugLog::DebugOut(DWORD dwDetailLvl, DWORD dwLogCategory, DWORD dwResId, ...)
{
    HRESULT       hr = S_OK;
    va_list       args;
    WCHAR         wzFormatString[MAX_DBG_STR_LEN];
    WCHAR         wzDebugStr[MAX_DBG_STR_LEN];

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    wzFormatString[0] = L'\0';
    wzDebugStr[0] = L'\0';

    hr = UtilLoadStringRC(dwResId, wzFormatString, MAX_DBG_STR_LEN);
    if (FAILED(hr)){
        goto Exit;
    }

    va_start(args, dwResId);
    StringCchVPrintf(wzDebugStr, MAX_DBG_STR_LEN, wzFormatString, args);
    va_end(args);

    hr = LogMessage(dwDetailLvl, dwLogCategory, wzDebugStr, FALSE, TRUE);

Exit:
    return hr;
}

HRESULT CDebugLog::DebugOutHeader(DWORD dwDetailLvl, BOOL bPrepend, DWORD dwResId, ...)
{
    HRESULT       hr = S_OK;
    va_list       args;
    WCHAR         wzFormatString[MAX_DBG_STR_LEN];
    WCHAR         wzDebugStr[MAX_DBG_STR_LEN];

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    wzFormatString[0] = L'\0';
    wzDebugStr[0] = L'\0';

    hr = UtilLoadStringRC(dwResId, wzFormatString, MAX_DBG_STR_LEN);
    if (FAILED(hr)){
        goto Exit;
    }

    va_start(args, dwResId);
    StringCchVPrintf(wzDebugStr, MAX_DBG_STR_LEN, wzFormatString, args);
    va_end(args);

    hr = LogHeaderMessage(dwDetailLvl, wzDebugStr, FALSE, TRUE);
    
Exit:
    return hr;
}

HRESULT CDebugLog::LogHeaderMessage(DWORD dwDetailLvl, LPCWSTR wzDebugStr, BOOL bPrepend, BOOL bEscapeEntities)
{
    HRESULT                                  hr = S_OK;
    CDebugLogElement                        *pLogElem = NULL;
    LISTNODE                                 pListNode = NULL;
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    hr = CDebugLogElement::Create(dwDetailLvl, wzDebugStr, bEscapeEntities, &pLogElem);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (bPrepend) {
        pListNode = _listHeaderMsg.AddHead(pLogElem);
    }
    else {
        pListNode = _listHeaderMsg.AddTail(pLogElem);
    }

    if (!pListNode)
    {
        SAFEDELETE(pLogElem);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    
Exit:
    return hr;    
}

HRESULT CDebugLog::LogMessage(DWORD dwDetailLvl, DWORD dwLogCategory, LPCWSTR wzDebugStr, BOOL bPrepend, BOOL bEscapeEntities)
{
    HRESULT                                  hr = S_OK;
    CDebugLogElement                        *pLogElem = NULL;
    LISTNODE                                 pListNode = NULL;
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    _ASSERTE(dwLogCategory < FUSION_BIND_LOG_CATEGORY_MAX);

    hr = CDebugLogElement::Create(dwDetailLvl, wzDebugStr, bEscapeEntities, &pLogElem);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (bPrepend) {
        pListNode = _listDbgMsg[dwLogCategory].AddHead(pLogElem);
    }
    else {
        pListNode = _listDbgMsg[dwLogCategory].AddTail(pLogElem);
    }

    if (!pListNode)
    {
        SAFEDELETE(pLogElem);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    

Exit:
    return hr;    
}


HRESULT CDebugLog::DumpDebugLog(DWORD dwDetailLvl, DWORD dwLogCategory, HRESULT hrLog)
{
    HRESULT                                    hr = S_OK;
    HANDLE                                     hFile = INVALID_HANDLE_VALUE;
    LISTNODE                                   pos = NULL;
    LPWSTR                                     wzUrlName = NULL;
    CDebugLogElement                          *pLogElem = NULL;
    WCHAR                                      wzFileName[MAX_PATH];
    WCHAR                                      wzSiteName[MAX_PATH];
    WCHAR                                      wzAppLogDir[MAX_PATH];
    LPWSTR                                     wzResourceName = NULL;
    DWORD                                      dwBytes;
    DWORD                                      dwSize;
    CCriticalSection                           cs(g_csBindLog);

    SO_NOT_MAINLINE_FUNCTION;

    if (!g_dwLogFailures && !g_dwForceLog) {
        return S_FALSE;
    }

    hr = cs.Lock();
    if (FAILED(hr)) {
        return hr;
    }

    pos = _listDbgMsg[dwLogCategory].GetHeadPosition();
    if (!pos) {
        hr = S_FALSE;
        goto Exit;
    }

    {
        BOOL                        bExists;

        ReplaceInvalidFileNameCharactersToUnderscore(_wzEXEName);

        hr = StringCchPrintf(wzAppLogDir, MAX_PATH, L"%ws\\%ws\\%ws", g_wzCustomLogPath, g_wzLogCategories[dwLogCategory], _wzEXEName);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CheckFileExistence(wzAppLogDir, &bExists, NULL);
        if (FAILED(hr)) {
            goto Exit;
        }
        else if (!bExists) {
            hr = CreateFilePathHierarchy(wzAppLogDir);
            if (FAILED(hr)) {
                goto Exit;
            }

            if(!WszCreateDirectory(wzAppLogDir, NULL)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Exit;
            }
        }

        if (PathIsURLW(_pwzAsmName)) {
            // This was a where-ref bind. We can't spit out a filename w/
            // the URL of the bind because the URL has invalid filename chars.
            // The best we can do is show that it was a where-ref bind, and
            // give the filename, and maybe the site.

            dwSize = MAX_PATH;
            wzSiteName[0] = L'\0';
            hr = UrlGetPartW(_pwzAsmName, wzSiteName, &dwSize, URL_PART_HOSTNAME, 0);
            if (FAILED(hr)) {
                goto Exit;
            }

            wzResourceName = PathFindFileName(_pwzAsmName);

            _ASSERTE(wzResourceName);

            if (!wzSiteName[0]) {
                hr = StringCchCopy(wzSiteName, MAX_PATH, L"LocalMachine");
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            if (dwLogCategory == FUSION_BIND_LOG_CATEGORY_NGEN) {
                hr = StringCchPrintf(wzFileName, MAX_PATH, L"%ws\\ExplicitBind!FileName=(%ws).HTM", wzAppLogDir, wzResourceName);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
            else {
                hr = StringCchPrintf(wzFileName, MAX_PATH, L"%ws\\WhereRefBind!Host=(%ws)!FileName=(%ws).HTM",
                           wzAppLogDir, wzSiteName, wzResourceName);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }
        else {
            hr = StringCchPrintf(wzFileName, MAX_PATH, L"%ws\\%ws.HTM", wzAppLogDir, _pwzAsmName);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    // Create the and write the log file

    hr = CreateLogFile(&hFile, dwLogCategory, wzFileName, _wzEXEName, hrLog);
    if (FAILED(hr)) {
        goto Exit;
    }

    // HTML start/end
    
    hr = DumpMessage(hFile, DEBUG_LOG_HTML_META_LANGUAGE);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = DumpMessage(hFile, DEBUG_LOG_MARK_OF_THE_WEB);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = DumpMessage(hFile, DEBUG_LOG_HTML_START);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = DumpResult(hFile, hrLog);
    if (FAILED(hr)) {
        goto Exit;
    }   

    pos = _listHeaderMsg.GetHeadPosition();
    while (pos) {
        pLogElem = _listHeaderMsg.GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLvl) {
            DumpMessage(hFile, pLogElem->_pszMsg);
            WriteFile(hFile, DEBUG_LOG_NEW_LINE, sizeof(DEBUG_LOG_NEW_LINE) - 1,
                      &dwBytes, NULL);
        }
    }   

    pos = _listDbgMsg[dwLogCategory].GetHeadPosition();
    while (pos) {
        pLogElem = _listDbgMsg[dwLogCategory].GetNext(pos);
        _ASSERTE(pLogElem);

        if (pLogElem->_dwDetailLvl <= dwDetailLvl) {
            DumpMessage(hFile, pLogElem->_pszMsg);
            WriteFile(hFile, DEBUG_LOG_NEW_LINE, sizeof(DEBUG_LOG_NEW_LINE) - 1,
                      &dwBytes, NULL);
        }
    }

    // Close the log file and commit the wininet cache entry

    hr = CloseLogFile(&hFile);
    if (FAILED(hr)) {
        goto Exit;
    }


Exit:
    SAFEDELETEARRAY(wzUrlName);
    cs.Unlock();
    return hr;
}

HRESULT CDebugLog::CloseLogFile(HANDLE *phFile)
{
    HRESULT     hr = S_OK;

    if (!phFile) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = DumpMessage(*phFile, DEBUG_LOG_HTML_END);
    if (FAILED(hr)) {
        goto Exit;
    }
    
Exit:
    if(phFile && (*phFile != INVALID_HANDLE_VALUE)) {
        CloseHandle(*phFile);
        *phFile = INVALID_HANDLE_VALUE;
    }

    return hr;
}

HRESULT CDebugLog::CreateLogFile(HANDLE *phFile, DWORD dwLogCategory, LPCWSTR wzFileName,
                                 LPCWSTR wzEXEName, HRESULT hrLog)
{
    HRESULT                              hr = S_OK;
    WCHAR                                wzBuffer[MAX_DBG_STR_LEN];
    WCHAR                                wzBuf[MAX_DBG_STR_LEN];

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLog");

    _ASSERTE(phFile && wzFileName && wzEXEName);

    *phFile = WszCreateFile(wzFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (*phFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }

    if (!_bWroteDetails) {
        hr = UtilLoadStringRC(ID_FUSLOG_DETAILED_LOG, wzBuffer, MAX_DBG_STR_LEN);
        if (FAILED(hr)) {
            goto Exit;
        }

        LogHeaderMessage(0, wzBuffer, TRUE, TRUE);
        
        // Executable path

        hr = UtilLoadStringRC(ID_FUSLOG_EXECUTABLE, wzBuf, MAX_DBG_STR_LEN);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCchPrintf(wzBuffer, MAX_DBG_STR_LEN, L"%ws %ws", wzBuf, g_wzEXEPath);
        if (FAILED(hr)) {
            goto Exit;
        }
        LogHeaderMessage(0, wzBuffer, TRUE, TRUE);
        
        // Fusion.dll path
        
        hr = UtilLoadStringRC(ID_FUSLOG_FUSION_DLL_PATH, wzBuf, MAX_DBG_STR_LEN);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCchPrintf(wzBuffer, MAX_DBG_STR_LEN, L"%ws %ws", wzBuf, g_FusionDllPath);
        if (FAILED(hr)) {
            goto Exit;
        }
        LogHeaderMessage(0, wzBuffer, TRUE, TRUE);
        
        _bWroteDetails = TRUE;
    }
    
Exit:
    return hr;    
}

HRESULT CDebugLog::DumpResult(HANDLE hFile, HRESULT hrLog)
{
    HRESULT                              hr = S_OK;
    SYSTEMTIME                           systime;
    WCHAR                                wzFormatMessage[MAX_FORMAT_MESSAGE_SIZE];
    DWORD                                cchFormatMessage = MAX_FORMAT_MESSAGE_SIZE;
    DWORD                                dwFMResult = 0;
    WCHAR                                wzBuffer[MAX_DBG_STR_LEN];
    WCHAR                                wzBuf[MAX_DBG_STR_LEN];
    WCHAR                                wzResultText[MAX_DBG_STR_LEN];
    WCHAR                                wzDateBuffer[MAX_DATE_LEN];
    WCHAR                                wzTimeBuffer[MAX_DATE_LEN];
    DWORD                                dwBytes = 0;

    // Header text

    GetSystemTime(&systime);

    StringCbPrintf(wzDateBuffer, sizeof(wzDateBuffer), L"%02d/%02d/%04d", systime.wMonth, systime.wDay, systime.wYear);
    StringCbPrintf(wzTimeBuffer, sizeof(wzTimeBuffer), L"%02d:%02d:%02d (UTC)", systime.wHour, systime.wMinute, systime.wSecond);

    hr = UtilLoadStringRC(ID_FUSLOG_HEADER_TEXT, wzBuf, MAX_DBG_STR_LEN);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchPrintf(wzBuffer, MAX_DBG_STR_LEN, L"%ws (%ws @ %ws) ***\r\n", wzBuf, wzDateBuffer, wzTimeBuffer);
    if (FAILED(hr)) {
        goto Exit;
    }
    DumpMessage(hFile, wzBuffer);
    WriteFile(hFile, DEBUG_LOG_NEW_LINE, sizeof(DEBUG_LOG_NEW_LINE) - 1, &dwBytes, NULL);

    // Success/fail
    
    if (SUCCEEDED(hrLog)) {
        hr = UtilLoadStringRC(ID_FUSLOG_OPERATION_SUCCESSFUL, wzBuffer, MAX_DBG_STR_LEN);
        if (FAILED(hr)) {
            goto Exit;;
        }
    }
    else {
        hr = UtilLoadStringRC(ID_FUSLOG_OPERATION_FAILED, wzBuffer, MAX_DBG_STR_LEN);
        if (FAILED(hr)) {
            goto Exit;;
        }
    }
    DumpMessage(hFile, wzBuffer);
    WriteFile(hFile, DEBUG_LOG_NEW_LINE, sizeof(DEBUG_LOG_NEW_LINE) - 1, &dwBytes, NULL);

    // Bind result and FormatMessage text
    
    hr = UtilLoadStringRC(ID_FUSLOG_BIND_RESULT_TEXT, wzResultText, MAX_DBG_STR_LEN);
    if (FAILED(hr)) {
        goto Exit;
    }
        
    dwFMResult = WszFormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hrLog, 0,
                                  wzFormatMessage, cchFormatMessage, NULL);
    if (dwFMResult) {                               
        hr = StringCchPrintf(wzBuffer, MAX_DBG_STR_LEN, wzResultText, hrLog, wzFormatMessage);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        WCHAR                             wzNoDescription[MAX_DBG_STR_LEN];

        hr = UtilLoadStringRC(ID_FUSLOG_NO_DESCRIPTION, wzNoDescription, MAX_DBG_STR_LEN);
        if (FAILED(hr)){
            goto Exit;
        }
        
        hr = StringCchPrintf(wzBuffer, MAX_DBG_STR_LEN, wzResultText, hrLog, wzNoDescription);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    DumpMessage(hFile, wzBuffer);
    WriteFile(hFile, DEBUG_LOG_NEW_LINE, sizeof(DEBUG_LOG_NEW_LINE) - 1, &dwBytes, NULL);

Exit:
    return hr;    
}

HRESULT CreateInformationalLogObject()
{
    HRESULT hr = S_OK;
    CDebugLog *pdbglog = NULL;

    if (g_pInformationalLog) {
        return S_OK;
    }

    pdbglog = NEW(CDebugLog());
    if (!pdbglog) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pdbglog->DebugOut(0, 0, ID_FUSLOG_LOG_TURNED_OFF);
    if (SUCCEEDED(hr)) {
        hr = pdbglog->DebugOut(0, 0, ID_FUSLOG_LOG_TURN_ON_HKLM);
    }
    if (SUCCEEDED(hr)) {
        hr = pdbglog->DebugOut(0, 0, ID_FUSLOG_LOG_PERF_WARNING);
    }
    if (SUCCEEDED(hr)) {
        hr = pdbglog->DebugOut(0, 0, ID_FUSLOG_LOG_TURN_OFF_HKLM);
    }
    if (SUCCEEDED(hr)) {
        // try to update the global pointer
        if (InterlockedCompareExchangePointer((void **)&g_pInformationalLog, pdbglog, NULL)) {
            // oops, lost the update
            // Need to remove the one we created.
            SAFEDELETE(pdbglog);
        }
    }
    else {
        SAFEDELETE(pdbglog);
    }

Exit:
    return hr;
}
