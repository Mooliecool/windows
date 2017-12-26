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
#ifndef __ADLMGR_H_INCLUDED__
#define __ADLMGR_H_INCLUDED__

#include "fusionp.h"
#include "dbglog.h"
#include "bindresult.h"
#include "helpers.h"
#include "asm.h"

#define PREFIX_HTTP                        L"http://"
#ifdef PLATFORM_UNIX
 #define BINPATH_LIST_DELIMITER            L':'
 #define SHADOW_COPY_DIR_DELIMITER         L':'
#else //!PLATFORM_UNIX
 #define BINPATH_LIST_DELIMITER            L';'
 #define SHADOW_COPY_DIR_DELIMITER         L';'
#endif //!PLATFORM_UNIX

#define DLTYPE_WHERE_REF                             0x0000001
#define DLTYPE_QUALIFIED_REF                         0x0000002
#define DLTYPE_QUALIFIED_REF_LOADFROM                0x0000003
#define DLTYPE_WHERE_REF_INSPECTION_ONLY             0x0000004
#define DLTYPE_QUALIFIED_REF_INSPECTION_ONLY         0x0000005

// Extended appbase check flags

#define APPBASE_CHECK_DYNAMIC_DIRECTORY              0x00000001
#define APPBASE_CHECK_PARENT_URL                     0x00000002
#define APPBASE_CHECK_SHARED_PATH_HINT               0x00000004

extern const LPWSTR g_wzProbeExtension;

class CDebugLog;
class CHashNode;
class CPolicyCache;
class CLoadContext;
class CCodebaseList;

class CAsmDownloadMgr : public IDownloadMgr, public ICodebaseList
{
    public:
        CAsmDownloadMgr(IAssemblyName *pNameRefSource, 
                        IApplicationContext *pAppCtx,
                        ICodebaseList *pCodebaseList, 
                        CPolicyCache *pPolicyCache,
                        CDebugLog *pdbglog, 
                        LONGLONG llFlags
                        );
        virtual ~CAsmDownloadMgr();

        static HRESULT Create(CAsmDownloadMgr **ppadm,
                              IAssemblyName *pNameRefSource,
                              IApplicationContext *pAppCtx,
                              ICodebaseList *pCodebaseList,
                              LPCWSTR wzBTOCodebase,
                              CDebugLog *pdbglog,
                              void *pvReserved,
                              LONGLONG llFlags
                              );

        // IUnknown methods

        STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IDownloadMgr methods

        STDMETHODIMP PreDownloadCheck(void **ppv, void **ppvNI);

        STDMETHODIMP DoSetup(LPCWSTR wzSourceUrl, 
                             LPCWSTR wzFilePath,
                             const FILETIME *pftLastMod, 
                             IUnknown **ppUnk,
                             IUnknown **ppAsmNI);
        STDMETHODIMP ProbeFailed(IUnknown **ppUnk);
        STDMETHODIMP IsDuplicate(IDownloadMgr *pIDLMgr);
        STDMETHODIMP LogResult();
        STDMETHODIMP DownloadEnabled(BOOL *pbEnabled);
        STDMETHODIMP GetBindInfo(FusionBindInfo *pBindInfo);
        STDMETHODIMP CacheBindingResult(HRESULT hrResult);

        // ICodebaseList methods

        STDMETHODIMP AddCodebase(LPCWSTR wzCodebase, DWORD dwFlags);
        STDMETHODIMP RemoveCodebase(DWORD dwIndex);
        STDMETHODIMP GetCodebase(DWORD dwIndex, 
                                 DWORD *pdwFlags, 
                                 __out_ecount_opt(*pcbCodebase) LPWSTR wzCodebase, 
                                 __inout DWORD *pcbCodebase);
        STDMETHODIMP GetCount(DWORD *pdwCount);
        STDMETHODIMP RemoveAll();

        // Helpers

        HRESULT GetDownloadIdentifier(DWORD *pdwDownloadType,
                                      __deref_out LPWSTR *ppwzID);
        HRESULT GetAppCtx(IApplicationContext **ppAppCtx);

    private:
        HRESULT Init(LPCWSTR wzBTOCodebase, void *pvReserved);

        // Helpers
        HRESULT DoSetupRFS(LPCWSTR wzFilePath, FILETIME *pftLastModified,
                           LPCWSTR wzSourceUrl, BOOL bWhereRefBind,
                           BOOL bPrivateAsmVerify, BOOL *pbBindRecorded);
        HRESULT DoSetupPushToCache(LPCWSTR wzFilePath, LPCWSTR wzSourceUrl,
                                   FILETIME *pftLastModified,
                                   BOOL bWhereRefBind, BOOL bCopyModules,
                                   BOOL bPrivateAsmVerify,
                                   BOOL *pbBindRecorded);


        HRESULT ShadowCopyDirCheck(LPCWSTR wzSourceURL);
        HRESULT CheckRunFromSource(LPCWSTR wzSourceUrl, BOOL *pbRunFromSource);
        

        HRESULT CreateAssembly(LPCWSTR szPath, LPCWSTR pszURL,
                               FILETIME *pftLastModTime,
                               BOOL bRunFromSource,
                               BOOL bWhereRef,
                               BOOL bPrivateAsmVerify,
                               BOOL bCopyModules,
                               BOOL *pbBindRecorded,
                               IAssembly **ppAsmOut);
        
        HRESULT GetAppCfgCodebaseHint(LPCWSTR pwzAppBase, 
                                    __deref_out LPWSTR *ppwzCodebaseHint);

        HRESULT LookupDownloadCacheAsm(IAssembly **ppAsm);
        HRESULT GetMRUDownloadCacheAsm(LPCWSTR pwzURL, IAssembly **ppAsm);

        HRESULT QualifyAssembly(IAssemblyName **ppNameQualified);

        HRESULT RecordInfo(BOOL bRecordPolicy, BOOL bAddActivation);

        // Probing URL generation
        HRESULT ConstructCodebaseList(LPCWSTR wzPolicyCodebase, 
                                    BOOL bAddBTOCodebase, 
                                    BOOL bAddProbingBase,
                                    BOOL *pbHasHttpOrUncUrl = NULL);

        HRESULT SetAsmLocation(IAssembly *pAsm, DWORD dwAsmLoc);

        HRESULT RecordInfoAndProbeNativeImage(BOOL bRecordPolicy, BOOL 
bAddActication, BOOL bProbeNativeImage);

        HRESULT CheckNeedToPutInDefaultLoadContext(
                        LPCWSTR wzSourceURL,
                        LPCWSTR wzPolicyCodebase,
                        IAssemblyName *pName,
                        BOOL    bWhereRef,
                        BOOL    *pbIsInDefaultContext,
                        BOOL    *pbNeedReProbe,
                        IAssembly **ppAsmOut
                    );

        HRESULT SwitchLoadContext(BOOL bToDefaultContext);

        HRESULT CheckBindingResultCache(void **ppv);

    private:
        DWORD                                       _dwSig;
        LONG                                        _cRef;
        BOOL                                        _bCodebaseHintUsed;
        LONGLONG                                    _llFlags;
        LPWSTR                                      _wzBTOCodebase;
        AsmBindHistoryInfo*                         _pBindHistory;
        IAssemblyName                              *_pNameRefSource;
        IAssemblyName                              *_pNameRefPolicy;
        IApplicationContext                        *_pAppCtx;
        IAssembly                                  *_pAsm;
        ICodebaseList                              *_pCodebaseList;
        CPolicyCache                               *_pPolicyCache;
        CDebugLog                                  *_pdbglog;
        CLoadContext                               *_pLoadContext;
        LPWSTR                                      _pwzProbingBase;
        BOOL                                        _bGACPartial;
        BOOL                                        _bNativeImageExists;
        CNativeImageAssembly                       *_pAsmNI; 
        IHostAssembly                              *_pHostAsm; 
        DWORD                                       _dwPolicyApplied;
        BOOL                                        _bInspectionOnly;
        BOOL                                        _bNeedReVerify;
        LPWSTR                                      _wzOldProbingPath;
        LPWSTR                                      _wzOldProbingUrl;
        DWORD                                       _dwDownloadType;
        LPWSTR                                      _pwzDownloadIdentifier;
        CBindingResultCache                        *_pBindResultCache; 
        CBindingInput                              *_pBindInput; 
        BOOL                                        _bPreviousFailed;
};

class CAssemblyCacheItem;

HRESULT CheckValidAsmLocation(IAssemblyName *pNameDef, LPCWSTR wzSourceUrl,
                              IApplicationContext *pAppCtx,
                              LPCWSTR pwzParentURL,
                              CDebugLog *pdbglog);

HRESULT IsUnderAppBase(IApplicationContext *pAppCtx, 
                       LPCWSTR pwzAppBaseCanonicalized,
                       LPCWSTR pwzParentURLCanonicalized,
                       LPCWSTR pwzSource,
                       DWORD dwExtendedAppBaseFlags);

HRESULT RecoverDeletedBits(CAssemblyCacheItem *pAsmItem, LPCWSTR szPath,
                           CDebugLog *pdbglog);

HRESULT ProcessDevOverride(IAssembly **ppAsmOut, IAssemblyName *pName,
                           IApplicationContext *pAppCtx, LONGLONG llFlags,
                           CDebugLog *pdbglog);

HRESULT CheckDevOverridePath(LPCWSTR pwzDevOverridePath, CCodebaseList *pCodebaseList,
                             IAssemblyName *pName, CDebugLog *pdbglog,
                             IAssembly **ppAsmOut);


#endif  // __ADLMGR_H_INCLUDED__
