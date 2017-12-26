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
#ifndef __DBGLOG_H_INCLUDED__
#define __DBGLOG_H_INCLUDED__

#include "list.h"


// Logging constants and globals

#define REG_VAL_FUSION_LOG_PATH              TEXT("LogPath")
#define REG_VAL_FUSION_LOG_LEVEL             TEXT("LoggingLevel")
#define REG_VAL_FUSION_LOG_FORCE             TEXT("ForceLog")
#define REG_VAL_FUSION_LOG_FAILURES          TEXT("LogFailures")
#define REG_VAL_FUSION_LOG_ENABLE            TEXT("EnableLog")
#define REG_VAL_FUSION_LOG_RESOURCE_BINDS    TEXT("LogResourceBinds")

extern DWORD g_dwLogInMemory;
extern DWORD g_dwLogLevel;
extern DWORD g_dwForceLog;
extern DWORD g_dwLogFailures;
extern DWORD g_dwLogResourceBinds;

extern LPWSTR g_wzLogCategories[FUSION_BIND_LOG_CATEGORY_MAX+1];
extern BOOL  g_bLogToWininet;
extern WCHAR g_wzCustomLogPath[MAX_PATH];

BOOL IsDebuggerAttached();

// Debug Output macros (for easy compile-time disable of logging)

inline BOOL IsLoggingNeeded()
{
    return g_dwLogFailures || g_dwForceLog || g_dwLogResourceBinds || g_dwLogInMemory || IsDebuggerAttached();
}


#define DEBUGOUTHEADER(pdbglog, dwLvl, pszLogMsg)  if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOutHeader(dwLvl, FALSE, pszLogMsg); }
#define DEBUGOUTHEADER1(pdbglog, dwLvl, pszLogMsg, param1) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOutHeader(dwLvl, FALSE, pszLogMsg, param1); }
#define DEBUGOUTHEADER2(pdbglog, dwLvl, pszLogMsg, param1, param2) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOutHeader(dwLvl, FALSE, pszLogMsg, param1, param2); }

#define DEBUGOUT(pdbglog, dwLvl, pszLogMsg)  if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, 0, pszLogMsg); }
#define DEBUGOUT1(pdbglog, dwLvl, pszLogMsg, param1) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, 0, pszLogMsg, param1); }
#define DEBUGOUT2(pdbglog, dwLvl, pszLogMsg, param1, param2) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, 0, pszLogMsg, param1, param2); }

#define DUMPDEBUGLOG(pdbglog, dwLvl) if (pdbglog && IsLoggingNeeded()) { pdbglog->Flush(dwLvl, 0); }

#define DEBUGOUTNGEN(pdbglog, dwLvl, pszLogMsg)  if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, FUSION_BIND_LOG_CATEGORY_NGEN, pszLogMsg); }
#define DEBUGOUTNGEN1(pdbglog, dwLvl, pszLogMsg, param1) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, FUSION_BIND_LOG_CATEGORY_NGEN, pszLogMsg, param1); }
#define DEBUGOUTNGEN2(pdbglog, dwLvl, pszLogMsg, param1, param2) if (pdbglog && IsLoggingNeeded()) { pdbglog->DebugOut(dwLvl, FUSION_BIND_LOG_CATEGORY_NGEN, pszLogMsg, param1, param2); }

#define DUMPDEBUGLOGNGEN(pdbglog, dwLvl) if (pdbglog && IsLoggingNeeded()) { pdbglog->Flush(dwLvl, FUSION_BIND_LOG_CATEGORY_NGEN); }


#define MAX_DBG_STR_LEN                 1024
#define MAX_FORMAT_MESSAGE_SIZE         512
#define MAX_DATE_LEN                    128
#define DEBUG_LOG_HTML_START            L"<html><pre>\r\n"
#define DEBUG_LOG_HTML_META_LANGUAGE    L"<meta http-equiv=\"Content-Type\" content=\"charset=unicode-1-1-utf-8\">"
#define DEBUG_LOG_MARK_OF_THE_WEB       L"<!-- saved from url=(0015)assemblybinder: -->"
#define DEBUG_LOG_HTML_END              L"\r\n</pre></html>"
#define DEBUG_LOG_NEW_LINE              "\r\n"

#define PAD_DIGITS_FOR_STRING(x) (((x) > 9) ? TEXT("") : TEXT("0"))


class CDebugLogElement {
    public:
        CDebugLogElement(DWORD dwDetailLvl);
        virtual ~CDebugLogElement();

        static HRESULT Create(DWORD dwDetailLvl, LPCWSTR pwzMsg,
                              BOOL bEscapeEntities,
                              CDebugLogElement **ppLogElem);
        HRESULT Init(LPCWSTR pwzMsg, BOOL bEscapeEntities);

    public:
        WCHAR                               *_pszMsg;
        DWORD                                _dwDetailLvl;
};

class CDebugLog : public IFusionBindLog {
    public:
        CDebugLog();
        virtual ~CDebugLog();

        static HRESULT Create(IApplicationContext *pAppCtx, 
                              IAssemblyName *pName,
                              LPCWSTR szCodebase,
                              CDebugLog **ppdl);

        static HRESULT Create(IApplicationContext *pAppCtx, 
                              LPCWSTR szAsmName,
                              CDebugLog **ppdl);
        // IUnknown methods
        
        STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IFusionBindLog methods

        STDMETHODIMP SetResultCode(DWORD dwLogCategory, HRESULT hr);
        STDMETHODIMP GetResultCode(DWORD dwLogCategory, HRESULT *pHr);
        STDMETHODIMP GetBindLog(DWORD dwDetailLevel, DWORD dwLogCategory,
                                __out_bcount_opt(*pcbDebugLog) LPWSTR pwzDebugLog,  DWORD *pcbDebugLog);

        STDMETHODIMP LogMessage(DWORD dwDetailLevel, DWORD dwLogCategory, 
                                LPCWSTR wzDebugStr);

        STDMETHODIMP Flush(DWORD dwDetailLevel, DWORD dwLogCategory);

        // CDebugLog functions
        HRESULT DebugOut(DWORD dwDetailLvl, DWORD dwLogCategory, DWORD dwResId, ...);

        HRESULT DebugOutHeader(DWORD dwDetailLvl, BOOL bPrepend, DWORD dwResId, ...);

        HRESULT LogMessage(DWORD dwDetailLvl, DWORD dwLogCategory, LPCWSTR wzDebugStr, BOOL bPrepend, BOOL bEscapeEntities);
        HRESULT LogHeaderMessage(DWORD dwDetailLvl, LPCWSTR wzDebugStr, BOOL bPrepend, BOOL bEscapeEntities);

        HRESULT DumpDebugLog(DWORD dwDetailLvl, DWORD dwLogCategory, HRESULT hrLog);
        
    private:
        HRESULT CreateLogFile(HANDLE *phFile, DWORD dwLogCategory, 
                              LPCWSTR wzFileName, LPCWSTR wzEXEName, 
                              HRESULT hrLog);
        HRESULT CloseLogFile(HANDLE *phFile);
        HRESULT Init(IApplicationContext *pAppCtx,
                     IAssemblyName *pName,
                     LPCWSTR szCodebase);
        HRESULT Init(IApplicationContext *pAppCtx, LPCWSTR szAsmName);
        HRESULT SetAsmName(IAssemblyName *pName, LPCWSTR szCodebase);
        HRESULT SetProperties(IApplicationContext *pAppCtx);
        HRESULT DumpResult(HANDLE hFile, HRESULT hrLog);

    private:
        List<CDebugLogElement *>                   _listDbgMsg[FUSION_BIND_LOG_CATEGORY_MAX];
        List<CDebugLogElement *>                   _listHeaderMsg; 
        HRESULT                                    _hrResult[FUSION_BIND_LOG_CATEGORY_MAX];
        LONG                                       _cRef;
        LPWSTR                                     _pwzAsmName;
        LPWSTR                                     _wzEXEName;
        BOOL                                       _bWroteDetails; 

};

HRESULT DumpMessage(HANDLE hFile, LPCWSTR pwzMsg);

HRESULT CreateInformationalLogObject();

extern CDebugLog *g_pInformationalLog;
#endif
