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
#include "adlmgr.h"
#include "naming.h"
#include "asm.h"
#include "appctx.h"
#include "asmint.h"
#include "actasm.h"
#include "asmcache.h"
#include "asmimprt.h"
#include "asmitem.h"
#include "cblist.h"
#include "policy.h"
#include "helpers.h"
#include "util.h"
#include "mdlmgr.h"
#include "parse.h"
#include "xmlparser.h"
#include "nodefact.h"
#include "pcycache.h"
#include "cache.h"
#include "transprt.h"
#include "enum.h"
#include "nodefact.h"
#include "lock.h"
#include "probing.h"
#include "hostasm.h"
#include "memoryreport.h"
#include "strongname.h"
#include "bindhelpers.h"


extern WCHAR g_wzEXEPath[MAX_PATH+1];


HRESULT VerifySignatureHelper(CTransCache *pTC, DWORD dwVerifyFlags);

const unsigned int MAX_VERSION_LENGTH = 12; // 65536 is max length == 5 digits
                                            // 5 * 2 = 10 (4 version fields)
                                            // 10 + 2 = 12 (delimiters + NULL)

// DEVOVERRIDE
extern WCHAR g_wzLocalDevOverridePath[MAX_PATH + 1];
extern WCHAR g_wzGlobalDevOverridePath[MAX_PATH + 1];
extern DWORD g_dwDevOverrideFlags;
extern DWORD g_dwDevOverrideEnable;

typedef enum tagEXTENSION_TYPE {
    EXT_UNKNOWN,
} EXTENSION_TYPE;


extern CRITSEC_COOKIE g_csSingleUse;
extern CRITSEC_COOKIE g_csDownload;

extern DWORD g_dwLogResourceBinds;
extern DWORD g_dwForceLog;

HRESULT Extract(LPCSTR lpCabName, LPCWSTR lpUniquePath);

HRESULT CAsmDownloadMgr::Create(CAsmDownloadMgr **ppadm,
                                IAssemblyName *pNameRefSource,
                                IApplicationContext *pAppCtx,
                                ICodebaseList *pCodebaseList,
                                LPCWSTR wzBTOCodebase,
                                CDebugLog *pdbglog,
                                void *pvReserved,
                                LONGLONG llFlags
                                )
{
    HRESULT                             hr = S_OK;
    DWORD                               cbBuf = 0;
    CAsmDownloadMgr                    *padm = NULL;
    CPolicyCache                       *pPolicyCache = NULL;
    DWORD                               dwSize;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");
    
    if (!ppadm || !pNameRefSource || !pAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppadm = NULL;

    // Process flag overrides passed in via app context

    cbBuf = 0;
    hr = pAppCtx->Get(ACTAG_BINPATH_PROBE_ONLY, NULL, &cbBuf, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        llFlags |= ASM_BINDF_BINPATH_PROBE_ONLY;
    }

    cbBuf = 0;
    hr = pAppCtx->Get(ACTAG_FORCE_CACHE_INSTALL, NULL, &cbBuf, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        llFlags |= ASM_BINDF_FORCE_CACHE_INSTALL;
    }

    cbBuf = 0;
    hr = pAppCtx->Get(ACTAG_RFS_INTEGRITY_CHECK, NULL, &cbBuf, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        llFlags |= ASM_BINDF_RFS_INTEGRITY_CHECK;
    }

    cbBuf = 0;
    hr = pAppCtx->Get(ACTAG_RFS_MODULE_CHECK, NULL, &cbBuf, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        llFlags |= ASM_BINDF_RFS_MODULE_CHECK;
    }

    dwSize = sizeof(pPolicyCache);
    pAppCtx->Get(ACTAG_APP_POLICY_CACHE, &pPolicyCache, &dwSize, APP_CTX_FLAGS_INTERFACE);

    // Create download object

    padm = NEW(CAsmDownloadMgr(pNameRefSource, pAppCtx, pCodebaseList,
                               pPolicyCache, pdbglog, llFlags)); 

    if (!padm) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = padm->Init(wzBTOCodebase, pvReserved);
    if (FAILED(hr)) {
        SAFEDELETE(padm);
        goto Exit;
    }

    *ppadm = padm;

Exit:
    SAFERELEASE(pPolicyCache);

    return hr;
}

CAsmDownloadMgr::CAsmDownloadMgr(IAssemblyName *pNameRefSource,
                                 IApplicationContext *pAppCtx,
                                 ICodebaseList *pCodebaseList,
                                 CPolicyCache *pPolicyCache,
                                 CDebugLog *pdbglog,
                                 LONGLONG llFlags
                                 )
: _cRef(1)
, _bCodebaseHintUsed(FALSE)
, _llFlags(llFlags)
, _wzBTOCodebase(NULL)
, _pBindHistory(NULL)
, _pNameRefSource(pNameRefSource)
, _pNameRefPolicy(NULL)
, _pAppCtx(pAppCtx)
, _pAsm(NULL)
, _pCodebaseList(pCodebaseList)
, _pPolicyCache(pPolicyCache)
, _pdbglog(pdbglog)
, _pLoadContext(NULL)
, _pwzProbingBase(NULL)
, _bGACPartial(FALSE)
, _bNativeImageExists(TRUE)
, _pAsmNI(NULL)
,_pHostAsm(NULL)
,_dwPolicyApplied(ePolicyLevelNone)
,_bInspectionOnly(FALSE)
,_bNeedReVerify(FALSE)
,_wzOldProbingPath(NULL)
,_wzOldProbingUrl(NULL)
,_dwDownloadType(DLTYPE_QUALIFIED_REF)
,_pwzDownloadIdentifier(NULL)
,_pBindResultCache(NULL)
,_pBindInput(NULL)
,_bPreviousFailed(FALSE)
{
    _dwSig = 0x4d4d5341; /* 'MMSA' */
    
    if (_pNameRefSource) {
        _pNameRefSource->AddRef();
    }

    if (_pCodebaseList) {
        _pCodebaseList->AddRef();
    }

    if (_pAppCtx) {
        _pAppCtx->AddRef();
    }

    if (_pdbglog) {
        _pdbglog->AddRef();
    }

    if (_pPolicyCache) {
        _pPolicyCache->AddRef();
    }

}

CAsmDownloadMgr::~CAsmDownloadMgr()
{
    SAFERELEASE(_pNameRefSource);
    SAFERELEASE(_pNameRefPolicy);
    SAFERELEASE(_pCodebaseList);
    SAFERELEASE(_pAppCtx);
    SAFERELEASE(_pAsm);
    SAFERELEASE(_pdbglog);
    SAFERELEASE(_pPolicyCache);
    SAFERELEASE(_pLoadContext);


    SAFERELEASE(_pAsmNI);
    SAFEDELETEARRAY(_wzBTOCodebase);
    SAFEDELETEARRAY(_pwzProbingBase);
    SAFEDELETEARRAY(_wzOldProbingPath);
    SAFEDELETEARRAY(_wzOldProbingUrl);
    SAFEDELETEARRAY(_pwzDownloadIdentifier);

    SAFERELEASE(_pHostAsm);
    SAFERELEASE(_pBindHistory);
    SAFERELEASE(_pBindResultCache);
    SAFERELEASE(_pBindInput);
}

HRESULT CAsmDownloadMgr::Init(LPCWSTR wzBTOCodebase, void *pvReserved)
{
    HRESULT                             hr = S_OK;
    DWORD                               dwLen;
    BOOL                                bWhereRefBind = FALSE;
    LPWSTR                              wzProbingBase=NULL;
    LPWSTR                              pwzFullCodebase = NULL;
    LPWSTR                              pwzAppBase = NULL;
    LPWSTR                              pwzAppBaseClean = NULL;
    CApplicationContext                *pCAppCtx = static_cast<CApplicationContext *>(_pAppCtx); 

    _ASSERTE(_pNameRefSource);
   
    hr = pCAppCtx->GetBindingResultCache(&_pBindResultCache);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_llFlags & ASM_BINDF_INSPECTION_ONLY) {
        _bInspectionOnly = TRUE;
        DEBUGOUT(_pdbglog, 0, ID_FUSLOG_INSPECTION_ONLY);
    }

    dwLen = 0;
    if (_pNameRefSource->GetName(&dwLen, NULL) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bWhereRefBind = TRUE;
    }

    if (!_bInspectionOnly) {
        if (bWhereRefBind) {
            hr = pCAppCtx->GetLoadContext(&_pLoadContext, LOADCTX_TYPE_LOADFROM);
            if (FAILED(hr)) {
                goto Exit;
            }
            _ASSERTE(_pLoadContext);
        }
    }

    if (wzBTOCodebase) {
        // canonicalize wzBTOCodebase
        pwzAppBase = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzAppBase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        dwLen = MAX_URL_LENGTH * sizeof(WCHAR);
        hr = _pAppCtx->Get(ACTAG_APP_BASE_URL, pwzAppBase, &dwLen, 0);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (dwLen + 1 >= MAX_URL_LENGTH) {
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            goto Exit;
        }

        if (pwzAppBase[dwLen - 2] != L'/' && pwzAppBase[dwLen - 2] != L'\\') {
            hr = StringCchCat(pwzAppBase, MAX_URL_LENGTH, L"/");
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        pwzAppBaseClean = StripFilePrefix(pwzAppBase);
        if (!pwzAppBaseClean || !lstrlenW(pwzAppBaseClean)) {
            hr = HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME);
            goto Exit;
        }   

        pwzFullCodebase = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzFullCodebase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        dwLen = MAX_URL_LENGTH;
        hr = UrlCombineUnescape(pwzAppBaseClean, wzBTOCodebase, pwzFullCodebase, &dwLen, 0);
        if (FAILED(hr)) {
            goto Exit;
        }

        _wzBTOCodebase = WSTRDupDynamic(pwzFullCodebase);
        if (!_wzBTOCodebase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    if (pvReserved && (_llFlags & ASM_BINDF_PARENT_ASM_HINT) && !bWhereRefBind) {
        IAssembly   *pAsm = (IAssembly *)pvReserved;
        CAssembly   *pCAsm = static_cast<CAssembly *>(pAsm);            

        if (!_bInspectionOnly) {
            _ASSERTE(pAsm->GetFusionLoadContext() != LOADCTX_TYPE_UNKNOWN);
            hr =  pCAppCtx->GetLoadContext(&_pLoadContext, pAsm->GetFusionLoadContext());
            if (FAILED(hr)) {
                goto Exit;
            }

        }

        // If parent is not the default context, then extract the parent
        // asm URL for probing base. If parent was in the LoadFrom context,
        // then it *must* mean that the asm was not located in the GAC,
        // and cannot be found via regular appbase probing. This is because
        // the runtime guarantees this by issuing Loads after each LoadFrom,
        // and discarding the LoadFrom IAssembly if it can be found through
        // Assembly.Load. Thus, the IAssembly we have now *must* have a
        // valid codebase.

        if ((_pLoadContext && _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) || _bInspectionOnly) {
            DWORD               dwSize;

            wzProbingBase = NEW(WCHAR[MAX_URL_LENGTH+1]);
            if (!wzProbingBase)
            {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }

            wzProbingBase[0] = L'\0';

            dwSize = MAX_URL_LENGTH;
            hr = pCAsm->GetProbingBase(wzProbingBase, &dwSize);
            if (FAILED(hr)) {
                goto Exit;
            }

            _ASSERTE(_bInspectionOnly || wzProbingBase[0]);

            if (wzProbingBase[0]) {
                _pwzProbingBase = WSTRDupDynamic(wzProbingBase);
                if (!_pwzProbingBase) {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
            }
        }
    }

    if (!_bInspectionOnly) {
        if (!_pLoadContext) {
            // Use default load context
            hr = pCAppCtx->GetLoadContext(&_pLoadContext, LOADCTX_TYPE_DEFAULT);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_SHOW_START_LOAD_CONTEXT, _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM? L"LoadFrom" : L"default"); 
        if (_pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
            DEBUGOUT(_pdbglog, 0, ID_FUSLOG_NGEN_BIND_LOADFROM_NOT_ALLOWED);
            DEBUGOUTNGEN(_pdbglog, 0, ID_FUSLOG_NGEN_BIND_LOADFROM_NOT_ALLOWED);
        }

        hr = CBindingInput::Create(_pNameRefSource, _wzBTOCodebase, _pLoadContext->GetContextType(), _pwzProbingBase, &_pBindInput);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // get download type and id.
    if (bWhereRefBind) {
        _dwDownloadType = _bInspectionOnly? DLTYPE_WHERE_REF_INSPECTION_ONLY : DLTYPE_WHERE_REF;
    }
    else {
        if (_bInspectionOnly) {
            _dwDownloadType = DLTYPE_QUALIFIED_REF_INSPECTION_ONLY;
        }else if (_pLoadContext->GetContextType() == LOADCTX_TYPE_DEFAULT) {
            _dwDownloadType = DLTYPE_QUALIFIED_REF;
        }else {
            _dwDownloadType = DLTYPE_QUALIFIED_REF_LOADFROM;
        }
    }


Exit:
    SAFEDELETEARRAY(wzProbingBase);
    SAFEDELETEARRAY(pwzFullCodebase);
    SAFEDELETEARRAY(pwzAppBase);
    return hr;
}

//
// IUnknown Methods
//

HRESULT CAsmDownloadMgr::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT                                    hr = S_OK;
    
    if (!ppv) 
        return E_POINTER;
    
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDownloadMgr)) {
        *ppv = static_cast<IDownloadMgr *>(this);
    }
    else if (IsEqualIID(riid, IID_ICodebaseList)) {
        *ppv = static_cast<ICodebaseList *>(this);
    }
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
}

STDMETHODIMP_(ULONG) CAsmDownloadMgr::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CAsmDownloadMgr::Release()
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

//
// IDownloadMgr methods
//

HRESULT CAsmDownloadMgr::DoSetup(LPCWSTR wzSourceUrl, 
                                 LPCWSTR wzFilePath,
                                 const FILETIME *pftLastMod, 
                                 IUnknown **ppUnk,
                                 IUnknown **ppAsmNI)
{
    HRESULT                            hr = S_OK;
    FILETIME                           ftLastModified;
    LPWSTR                             pwzRFS = NULL;
    LPWSTR                             pwzExt = NULL;
    BOOL                               bWhereRefBind = FALSE;
    DWORD                              dwSize = 0;
    BOOL                               bIsFileUrl = FALSE;
    BOOL                               bRunFromSource = FALSE;
    BOOL                               bCopyModules = FALSE;
    IAssembly                         *pAsmCtx = NULL;
    BOOL                               bBindRecorded = FALSE;
    LPWSTR                             wzProbingBase=NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    if (!wzSourceUrl || !wzFilePath) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Remove ? from http source URLs, so cache code doesn't
    // try to construct a dir containing ? (which will fail).

    if (!UrlIsW(wzSourceUrl, URLIS_FILEURL)) {
        LPWSTR               pwzArgument = StrChr(wzSourceUrl, L'?');

        if (pwzArgument) {
            *pwzArgument = L'\0';
        }
    }

    DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_DOWNLOAD_SUCCESS, wzFilePath);

    if (ppUnk) {
        *ppUnk = NULL;
    }

    pwzExt = PathFindExtension(wzFilePath);
    _ASSERTE(pwzExt);


    dwSize = 0;
    hr = _pNameRefPolicy->GetName(&dwSize, NULL);
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bWhereRefBind = TRUE;
    }

    if (!pftLastMod) {
        hr = ::GetFileLastModified(wzFilePath, &ftLastModified);
        if (FAILED(hr)) {
            DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_LAST_MOD_FAILURE, wzFilePath);
            goto Exit;
        }
    }
    else {
        memcpy(&ftLastModified, pftLastMod, sizeof(FILETIME));
    }

    bIsFileUrl = UrlIsW(wzSourceUrl, URLIS_FILEURL);
    if (bIsFileUrl) {
        // This is a file:// URL, so let's see if we can run from source.

        hr = CheckRunFromSource(wzSourceUrl, &bRunFromSource);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (!bRunFromSource) {
            // This is a shadow copy scenario. Copy the modules.
            bCopyModules = TRUE;
        }
    }


    // Setup the assemblies
    if (bRunFromSource) {
        hr = DoSetupRFS(wzFilePath, &ftLastModified, wzSourceUrl, bWhereRefBind, TRUE, &bBindRecorded);
        if (FAILED(hr)) {
            if (_pwzProbingBase) {
                // We are a child of a LoadFrom. Continue to probe even
                // after a failure setting up the assembly
                hr = S_FALSE;
            }
            goto Exit;
        }
    }
    else {
        {
            // Not compressed. Push to cache.
            hr = DoSetupPushToCache(wzFilePath, wzSourceUrl, &ftLastModified,
                                    bWhereRefBind, bCopyModules, TRUE, &bBindRecorded);
        }

        if (FAILED(hr)) {
            if (_pwzProbingBase) {
                // We are a child of a LoadFrom. Continue to probe even
                // after a failure setting up the assembly
                hr = S_FALSE;
            }

            goto Exit;
        }
    }

    // If this is an assembly bind, and we succeeded, add the IAssembly to
    // the list of activated assemblies.

    if (hr == S_OK) {
        // use this code path only if we are still in LOADFROM context.
        if ((bWhereRefBind||_bNeedReVerify) && (_bInspectionOnly || _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM)) {
            IUnknown           *pAsmActivated = NULL;
            CAssembly          *pCAsm = static_cast<CAssembly *>(_pAsm); // dynamic_cast
            LPWSTR              pwzFileName;

            _ASSERTE(pCAsm && lstrlenW(wzSourceUrl) < MAX_URL_LENGTH);

            // Add activation to load context

            // Set the probing base to be equal to the codebase.

            wzProbingBase = NEW(WCHAR[MAX_URL_LENGTH+1]);
            if (!wzProbingBase)
            {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }

            hr = StringCchCopy(wzProbingBase, MAX_URL_LENGTH + 1, wzSourceUrl);
            pwzFileName = PathFindFileName(wzProbingBase);

            _ASSERTE(pwzFileName);

            *pwzFileName = L'\0';

            hr = pCAsm->SetProbingBase(wzProbingBase);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (!_bInspectionOnly) {
                hr = _pLoadContext->AddActivation(_pAsm, &pAsmActivated);
                if (hr == S_FALSE) {
                    SAFERELEASE(_pAsm);
                    hr = pAsmActivated->QueryInterface(IID_IAssembly, (void **)&_pAsm);
                    SAFERELEASE(pAsmActivated);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }
        }
        else if (!bBindRecorded && !_bInspectionOnly) {
            // Binds will have been recorded already if this was a partial
            // assembly name bind, and when we entered CreateAssembly, we
            // found a match already in the activated assemblies list, or in
            // the cache (CreateAssembly takes uses the def to turn a partial
            // into a full-ref, after which, it applies policy, and calls
            // PreDownloadCheck again).
            hr = RecordInfoAndProbeNativeImage(TRUE, TRUE, _bNativeImageExists);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        // This download manager object will keep a ref count on the IAssembly
        // until it is destroyed. We are only destroyed after we call back
        // the client (inside CAssemblyDownload), so at the time we call
        // the client back, the IAssembly is good (and they can either addref
        // it or not, as they choose).

        *ppUnk = _pAsm;
        _pAsm->AddRef();

        if (ppAsmNI && _pAsmNI) {
            hr = _pAsmNI->QueryInterface(IID_IUnknown, (void **)ppAsmNI);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        if (!bBindRecorded && _pdbglog && IsLoggingNeeded()) {
            WCHAR wzPath[MAX_PATH];

            dwSize = MAX_PATH;
            wzPath[0] = L'\0';
            _pAsm->GetManifestModulePath(wzPath, &dwSize);
            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_BIND_SUCCESS, wzPath);
            if (!_bInspectionOnly) {
                DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_SHOW_RESULT_LOAD_CONTEXT, _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM? L"LoadFrom" : L"default");
            }
        }

        hr = S_OK;
    }

Exit:
    SAFERELEASE(pAsmCtx);

    SAFEDELETEARRAY(pwzRFS);
    SAFEDELETEARRAY(wzProbingBase);
    return hr;    
}

HRESULT CAsmDownloadMgr::CheckRunFromSource(LPCWSTR wzSourceUrl,
                                            BOOL *pbRunFromSource)
{
    HRESULT                                 hr = S_OK;
    DWORD                                   dwSize;

    if (!wzSourceUrl || !pbRunFromSource) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    _ASSERTE(UrlIsW(wzSourceUrl, URLIS_FILEURL));

    *pbRunFromSource = TRUE;

    // Default policy is to rfs all file:// URLs. The only exception is
    // to set FORCE_CACHE_INSTALL (ie. Shadow Copy), which can be done
    // through: bind flags, app ctx, or app.cfg file.
    
    dwSize = 0;
    hr = _pAppCtx->Get(ACTAG_FORCE_CACHE_INSTALL, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        _llFlags |= ASM_BINDF_FORCE_CACHE_INSTALL;
    }

    if (_llFlags & ASM_BINDF_FORCE_CACHE_INSTALL) {
        // We're in shadow copy mode

        // Disable FORCE_CACHE_INSTALL (shadow copy) if it's not
        // part of a list of designated shadow copy dirs.

        hr = ShadowCopyDirCheck(wzSourceUrl);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            *pbRunFromSource = FALSE;
        }

    }

    hr = S_OK;

Exit:
    return hr;
}

HRESULT CAsmDownloadMgr::ProbeFailed(IUnknown **ppUnk)
{
    HRESULT                                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    DWORD                                  dwSize;

    if (!ppUnk) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppUnk = NULL;

    if (g_bFusionHosted) {
        goto Exit;
    }

    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_FAILED_PROBING);

    dwSize = 0;
    if (_pNameRefPolicy->GetName(&dwSize, NULL) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        // This is a where-ref bind. Do not look in global cache.
        goto Exit;
    }
    
    hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

Exit:
    return hr;
}


HRESULT CAsmDownloadMgr::DoSetupRFS(LPCWSTR wzFilePath, FILETIME *pftLastModified,
                                    LPCWSTR wzSourceUrl, BOOL bWhereRefBind,
                                    BOOL bPrivateAsmVerify, BOOL *pbBindRecorded)
{
    HRESULT                                   hr = S_OK;
    IAssembly                                *pAsm = NULL;
    IAssemblyModuleImport                    *pCurModImport = NULL;
    BOOL                                      bAsmOK = TRUE;
    BYTE                                      abCurHash[MAX_HASH_LEN];
    BYTE                                      abFileHash[MAX_HASH_LEN];
    DWORD                                     cbModHash;
    DWORD                                     cbFileHash;
    DWORD                                     dwAlgId;
    WCHAR                                     wzModPath[MAX_PATH];
    DWORD                                     cbModPath;
    int                                       idx = 0;
    DWORD                                     dwAsmLoc = ASMLOC_UNKNOWN;

    _ASSERTE(pbBindRecorded);

    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_SETUP_RUN_FROM_SOURCE);

    // Run from source

    hr = CreateAssembly(wzFilePath, wzSourceUrl, pftLastModified, TRUE, bWhereRefBind,
                        bPrivateAsmVerify, FALSE, pbBindRecorded, &pAsm);
    if (FAILED(hr) || hr == S_FALSE) {
        goto Exit;
    }

    if (pAsm) {
        hr = pAsm->GetAssemblyLocation(&dwAsmLoc);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Integrity checking
    // Walk all modules to make sure they are there (and are valid)

    if ((_llFlags & ASM_BINDF_RFS_MODULE_CHECK) && (dwAsmLoc != ASMLOC_GAC)) {

        while (SUCCEEDED(pAsm->GetNextAssemblyModule(idx++, &pCurModImport))) {
            if (!pCurModImport->IsAvailable()) {
                bAsmOK = FALSE;
                SAFERELEASE(pCurModImport);
                break;
            }
    
            if (_llFlags & ASM_BINDF_RFS_INTEGRITY_CHECK) {
    
                // Get the hash of this module from manifest
                hr = pCurModImport->GetHashAlgId(&dwAlgId);
                if (FAILED(hr)) {
                    break;
                }
    
                cbModHash = MAX_HASH_LEN; 
                hr = pCurModImport->GetHashValue(abCurHash, &cbModHash);
                if (FAILED(hr)) {
                    break;
                }
    
                // Get the hash of the file itself
                cbModPath = MAX_PATH;
                hr = pCurModImport->GetModulePath(wzModPath, &cbModPath);
                if (FAILED(hr)) {
                    break;
                }
    
                cbFileHash = MAX_HASH_LEN;
                hr = GetHash(wzModPath, (ALG_ID)dwAlgId, abFileHash, &cbFileHash);
                if (FAILED(hr)) {
                    break;
                }
    
                if (!CompareHashs(cbModHash, abCurHash, abFileHash)) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_MODULE_INTEGRITY_CHECK_FAILURE);
                    bAsmOK = FALSE;
                    SAFERELEASE(pCurModImport);
                    break;
                }
            }
    
            SAFERELEASE(pCurModImport);
        }
    
        if (FAILED(hr)) {
            SAFERELEASE(pCurModImport);
            goto Exit;
        }
    }

    if (bAsmOK) {
        _ASSERTE(pAsm);
        _ASSERTE(!_pAsm || _pAsm == pAsm);
       
        if (_pAsm != pAsm) { 
            SAFERELEASE(_pAsm);
            _pAsm = pAsm;
            _pAsm->AddRef();
        }

        if (!*pbBindRecorded && (dwAsmLoc != ASMLOC_GAC)) {
            SetAsmLocation(_pAsm, ASMLOC_RUN_FROM_SOURCE);
        }
    }
    else {
        // At least one module is missing (or hash invalid), and client
        // requested we check for this condition..
        // Cannot run from source.

        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

Exit:
    SAFERELEASE(pAsm);

    return hr;
}

HRESULT CAsmDownloadMgr::DoSetupPushToCache(LPCWSTR wzFilePath,
                                            LPCWSTR wzSourceUrl,
                                            FILETIME *pftLastModified,
                                            BOOL bWhereRefBind,
                                            BOOL bCopyModules,
                                            BOOL bPrivateAsmVerify,
                                            BOOL *pbBindRecorded)
{
    HRESULT     hr = S_OK;
    DWORD       dwAsmLoc = ASMLOC_UNKNOWN;
    
    _ASSERTE(pbBindRecorded);

    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_SETUP_DOWNLOAD_CACHE);

    // Install to assembly cache

    // Create the assembly - here, pNameRefSource is the original (golden)
    // manifest ref; can be null, simple or strong. pNameRefPolicy is 
    // post-policy name ref if pName is strong only, null otherwise.
    hr = CreateAssembly(wzFilePath, wzSourceUrl, pftLastModified, 
                        FALSE, bWhereRefBind, bPrivateAsmVerify, 
                        bCopyModules, pbBindRecorded, &_pAsm);

    if (FAILED(hr)) {
        DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_SETUP_FAILURE, hr);
        goto Exit;
    }

    if (hr == S_OK && !*pbBindRecorded) {
        if (_pAsm) {
            hr = _pAsm->GetAssemblyLocation(&dwAsmLoc);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        if (dwAsmLoc != ASMLOC_GAC) {
            SetAsmLocation(_pAsm, ASMLOC_DOWNLOAD_CACHE);
        }
    }

Exit:
    return hr;
}

//
// ICodebaseList Methods.
//
// Here, we just delegate the calls to either the object passed in by the
// bind client, or our own CCodebaseList (which is created if we probe).
//

HRESULT CAsmDownloadMgr::AddCodebase(LPCWSTR wzCodebase, DWORD dwFlags)
{
    HRESULT                               hr = S_OK;

    _ASSERTE(_pCodebaseList);

    if (_pCodebaseList) {
        hr = _pCodebaseList->AddCodebase(wzCodebase, dwFlags);
    }

    return hr;
}

HRESULT CAsmDownloadMgr::RemoveCodebase(DWORD dwIndex)
{
    HRESULT                              hr = S_OK;

    _ASSERTE(_pCodebaseList);

    if (_pCodebaseList) {
        hr = _pCodebaseList->RemoveCodebase(dwIndex);
    }
        
    return hr;
}

HRESULT CAsmDownloadMgr::GetCodebase(DWORD dwIndex, DWORD *pdwFlags, __out_ecount_opt(*pcbCodebase) LPWSTR wzCodebase,
                                     DWORD *pcbCodebase)
{
    HRESULT                              hr = S_OK;

    _ASSERTE(_pCodebaseList);

    if (_pCodebaseList) {
        hr = _pCodebaseList->GetCodebase(dwIndex, pdwFlags, wzCodebase, pcbCodebase);
    }

    return hr;
}

HRESULT CAsmDownloadMgr::RemoveAll()
{
    HRESULT                              hr = S_OK;
    
    if (_pCodebaseList) {
        hr = _pCodebaseList->RemoveAll();
    }

    return hr;
}

HRESULT CAsmDownloadMgr::GetCount(DWORD *pdwCount)
{
    HRESULT                              hr = S_OK;

    if (_pCodebaseList) {
        hr = _pCodebaseList->GetCount(pdwCount);
    }

    return hr;
}

HRESULT CAsmDownloadMgr::ConstructCodebaseList(
                            LPCWSTR wzPolicyCodebase, 
                            BOOL bAddBTOCodebase, 
                            BOOL bAddProbingBase,
                            BOOL *pbHasHttpOrUncUrl)
{
    HRESULT                                   hr = S_OK;
    LPWSTR                                    wzAppBase = NULL;
    LPWSTR                                    pwzAppBaseClean = NULL;
    BOOL                                      bGenerateProbeURLs = TRUE;
    LPWSTR                                    wzCodebaseHint = NULL;
    DWORD                                     cbLen;
    DWORD                                     dwLen;
    DWORD                                     dwCount;
    CAssemblyProbe                           *pAsmProbe = NULL;
    BOOL                                      bDisallowAppBaseProbing = FALSE;  
    BOOL                                      bHasHttpOrUncUrl = FALSE;
    WCHAR                                    *pwzPath;  

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    cbLen = 0;
    hr = _pAppCtx->Get(ACTAG_DISALLOW_APP_BASE_PROBING, NULL, &cbLen, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bDisallowAppBaseProbing = TRUE;
        DEBUGOUT(_pdbglog, 0, ID_FUSLOG_DISALLOW_APP_BASE_PROBING);
    }
    hr = S_OK;

    pwzPath = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzPath) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    // If we were passed a pCodebaseList at construction time, we do not
    // have to build probing URLs, as long as there is at least one URL in
    // the codebase list.

    if (_pCodebaseList) {
        hr = _pCodebaseList->GetCount(&dwCount);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK && dwCount) {
            bGenerateProbeURLs = FALSE;
        }
    }
    else {
        _pCodebaseList = NEW(CCodebaseList);
        if (!_pCodebaseList) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    // Get and canonicalize the appbase directory

    wzAppBase = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!wzAppBase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwLen = MAX_URL_LENGTH * sizeof(WCHAR);
    hr = _pAppCtx->Get(ACTAG_APP_BASE_URL, wzAppBase, &dwLen, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pbHasHttpOrUncUrl) {
        bHasHttpOrUncUrl = !UrlIsW(wzAppBase, URLIS_FILEURL);
        if (!bHasHttpOrUncUrl) {
            dwLen = MAX_URL_LENGTH;
            hr = PathCreateFromUrlWrap(wzAppBase, pwzPath, &dwLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
            if (PathIsUNC(pwzPath)) {
                bHasHttpOrUncUrl = TRUE;
            }
        }
    }

    dwLen = lstrlenW(wzAppBase);
    _ASSERTE(dwLen);

    if (dwLen + 2 >= MAX_URL_LENGTH) {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto Exit;
    }

    if (wzAppBase[dwLen - 1] != L'/' && wzAppBase[dwLen - 1] != L'\\') {
        hr = StringCchCat(wzAppBase, MAX_URL_LENGTH, L"/");
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    pwzAppBaseClean = StripFilePrefix(wzAppBase);
    if (!pwzAppBaseClean || !pwzAppBaseClean[0]) {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME);
        goto Exit;
    }   

    // Always add the codebase from BTO to the codebase list, if policy
    // was not applied.

    if (bAddBTOCodebase) {
        if (_wzBTOCodebase && _pNameRefPolicy->IsEqual(_pNameRefSource, ASM_CMPF_DEFAULT) == S_OK) {
            // _wzBTOCodebase is already canonicalize in Init().
            _pCodebaseList->AddCodebase(_wzBTOCodebase, 0);
            if (pbHasHttpOrUncUrl && !bHasHttpOrUncUrl) {
                bHasHttpOrUncUrl = !UrlIsW(_wzBTOCodebase, URLIS_FILEURL);
                if (!bHasHttpOrUncUrl) {
                    dwLen = MAX_URL_LENGTH;
                    hr = PathCreateFromUrlWrap(_wzBTOCodebase, pwzPath, &dwLen, 0);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                    if (PathIsUNC(pwzPath)) {
                        bHasHttpOrUncUrl = TRUE;
                    }
                }
            }
        }
    }
   
    // If there was a policy redirect, and codebase attached to it,
    // add this URL.

    if (wzPolicyCodebase && lstrlenW(wzPolicyCodebase)) {
        _pCodebaseList->AddCodebase(wzPolicyCodebase, ASMLOC_CODEBASE_HINT);

        // It better exist at the location specified; only add this URL to
        // the probe list

        _bCodebaseHintUsed = TRUE;
        if (pbHasHttpOrUncUrl && !bHasHttpOrUncUrl) {
            bHasHttpOrUncUrl = !UrlIsW(wzPolicyCodebase, URLIS_FILEURL);
            if (!bHasHttpOrUncUrl) {
                dwLen = MAX_URL_LENGTH;
                hr = PathCreateFromUrlWrap(wzPolicyCodebase, pwzPath, &dwLen, 0);
                if (FAILED(hr)) {
                    goto Exit;
                }
                if (PathIsUNC(pwzPath)) {
                    bHasHttpOrUncUrl = TRUE;
                }
            }
        }

        goto Exit;
    }

    if (!bGenerateProbeURLs) {
        // If we were provided a codebase list, we just needed to add the
        // binding URL to the list, and then we're done.
        goto Exit;
    }

    // Add any codebase hints from app.cfg, if we didn't get one already
    // because of policy.

    hr = GetAppCfgCodebaseHint(pwzAppBaseClean, &wzCodebaseHint);
    if (hr == S_OK) {
        _pCodebaseList->AddCodebase(wzCodebaseHint, ASMLOC_CODEBASE_HINT);
        _bCodebaseHintUsed = TRUE;

        if (pbHasHttpOrUncUrl && !bHasHttpOrUncUrl) {
            bHasHttpOrUncUrl = !UrlIsW(wzCodebaseHint, URLIS_FILEURL);
            if (!bHasHttpOrUncUrl) {
                dwLen = MAX_URL_LENGTH;
                hr = PathCreateFromUrlWrap(wzCodebaseHint, pwzPath, &dwLen, 0);
                if (FAILED(hr)) {
                    goto Exit;
                }
                if (PathIsUNC(pwzPath)) {
                    bHasHttpOrUncUrl = TRUE;
                }
            }
        }
        goto Exit;
    }

    // Add probing URLs

    hr = CAssemblyProbe::Create(&pAsmProbe, _pNameRefPolicy, _pAppCtx, _llFlags,
                                _pdbglog, bAddProbingBase?_pwzProbingBase:NULL);
    if (FAILED(hr)) {
        goto Exit;
    }
                                    
    hr = pAsmProbe->SetupDefaultProbeList(bDisallowAppBaseProbing? NULL : pwzAppBaseClean, NULL, _pCodebaseList, TRUE, TRUE);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pbHasHttpOrUncUrl && !bHasHttpOrUncUrl && _pwzProbingBase && bAddProbingBase) {
        bHasHttpOrUncUrl = !UrlIsW(_pwzProbingBase, URLIS_FILEURL);
        if (!bHasHttpOrUncUrl) {
            dwLen = MAX_URL_LENGTH;
            hr = PathCreateFromUrlWrap(_pwzProbingBase, pwzPath, &dwLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
            if (PathIsUNC(pwzPath)) {
                bHasHttpOrUncUrl = TRUE;
            }
        }
    }

    if (pbHasHttpOrUncUrl) {
        *pbHasHttpOrUncUrl = bHasHttpOrUncUrl;
    }

Exit:
    SAFEDELETEARRAY(wzCodebaseHint);
    SAFEDELETEARRAY(wzAppBase);
    SAFEDELETEARRAY(pwzPath);

    SAFEDELETE(pAsmProbe);

    return hr;    
}

HRESULT CAsmDownloadMgr::PreDownloadCheck(void **ppv, void **ppvNI)
{
    HRESULT             hr = S_FALSE;
    BOOL                bWhereRefBind = FALSE;
    BOOL                bIsStronglyNamed = FALSE;
    BOOL                bIsPartial = FALSE;
    BOOL                bApplyPolicy = TRUE;
    DWORD               dwCount = 0;
    DWORD               dwSize = 0;
    DWORD               dwCmpMask;
    LPWSTR              wzPolicyCodebase = NULL;
    IAssembly          *pAsm = NULL;
    CNativeImageAssembly *pAsmNI = NULL;
    IHostAssembly      *pHostAsm = NULL;
    IAssemblyName      *pNameDevOverride = NULL;
    WCHAR               wzManifestPath[MAX_PATH];
    DWORD               dwPathSize = MAX_PATH;
    LPWSTR              pwzCodebase = NULL;
    BOOL                bHasHttpOrUncUrl = FALSE;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    if (!ppv) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppv = NULL;

    hr = _pNameRefSource->GetName(&dwSize, NULL);
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        // This is a where-ref bind. Can't lookup in cache, or actasm list.
        bWhereRefBind = TRUE;
    }
    else {
        // Not where-ref. It might be custom, and/or strongly named
        bIsStronglyNamed = CAssemblyName::IsStronglyNamed(_pNameRefSource);
        bIsPartial = CAssemblyName::IsPartial(_pNameRefSource, &dwCmpMask);
    }
    
    // Make sure node factories and private path have been read.
    // This must be done here (can't rely on ApplyPolicy to do this) because
    // policy might not always be applied (but private path needs to be read).
    // The ApplyPolicy call to ReadConfigSettings should return immediately.

    CApplicationContext *pAppCtx;
    pAppCtx = static_cast<CApplicationContext *>(_pAppCtx);
    _ASSERTE(pAppCtx);
    
    hr = pAppCtx->ReadConfigSettings(_pdbglog, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_pdbglog && IsLoggingNeeded()) {
        LPWSTR  pwzConfigFile = NULL;
        AppCtxGetWrapper(_pAppCtx, ACTAG_APP_CFG_LOCAL_FILEPATH, &pwzConfigFile);
        if (pwzConfigFile) {
            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_APP_CONFIG_FILE, pwzConfigFile);
        }
        else {
            DEBUGOUT(_pdbglog, 0, ID_FUSLOG_NO_APP_CONFIG_FILE);
        }
        SAFEDELETEARRAY(pwzConfigFile);

        AppCtxGetWrapper(_pAppCtx, ACTAG_HOST_CONFIG_FILE, &pwzConfigFile);
        if (pwzConfigFile) {
            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_HOST_CONFIG_FILE, pwzConfigFile);
        }
        SAFEDELETEARRAY(pwzConfigFile);

        AppCtxGetWrapper(_pAppCtx, ACTAG_MACHINE_CONFIG, &pwzConfigFile);
        if (pwzConfigFile) {
            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_MACHINE_CFG_FOUND, pwzConfigFile);
        }
        SAFEDELETEARRAY(pwzConfigFile);
    }

    if (bIsPartial) {
        IAssemblyName                    *pNameQualified;

        if (QualifyAssembly(&pNameQualified) == S_OK) {
            SAFERELEASE(_pNameRefSource);
            _pNameRefSource = pNameQualified;

            bIsStronglyNamed = CAssemblyName::IsStronglyNamed(_pNameRefSource);
            bIsPartial = CAssemblyName::IsPartial(_pNameRefSource, &dwCmpMask);
        }
    }

    // Only check policy cache if _pNameRefPolicy == NULL. If _pNameRefPolicy
    // is non-NULL, it means we're entering the predownload check a second
    // time (probably from a partial bind, being re-initiated). In this case,
    // ApplyPolicy is going to reset the policy ref.

    if (bWhereRefBind) {
        bApplyPolicy = FALSE;
        if (!_pNameRefPolicy) {
            hr = _pNameRefSource->Clone(&_pNameRefPolicy);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    else {
        if (_pPolicyCache && !_pNameRefPolicy && bIsStronglyNamed && !bIsPartial && !_bInspectionOnly) {
            hr = _pPolicyCache->LookupPolicy(_pNameRefSource, &_pNameRefPolicy, NULL, &_dwPolicyApplied);
            if (hr == S_OK) {
                bApplyPolicy = FALSE;
                _ASSERTE(_pNameRefPolicy);
            }
        }
    }

    if (bApplyPolicy) {
        // Apply policy and check if we can succeed the bind immediately.
        // This is only necessary if we didn't hit in the policy cache above.
    
        CCriticalSection                         cs(g_csDownload);
        IAssemblyName                           *pNamePolicy = NULL;

        if (_bInspectionOnly) {
            hr = _pNameRefSource->Clone(&pNamePolicy);
        }
        else {
            hr = ApplyPolicy(_pNameRefSource, _pAppCtx, _pdbglog,
                            &pNamePolicy, &wzPolicyCodebase, NULL, 
                            &_dwPolicyApplied);
        }

        if (FAILED(hr)) {
            goto Exit;
        }
        
        hr = cs.Lock();
        if (FAILED(hr)) {
            SAFERELEASE(pNamePolicy);
            goto Exit;
        }

        // This block is protected by g_csDownload because this download
        // object may be in the global list of in-progress assembly
        // binds. The duplicate checking code expects to be able to get to
        // _pNameRefPolicy, but we are in the process of updating it here
        // to make it a qualified ref. This download list duplicate checking
        // is done under g_csDownload, so we need to protect this code
        // with the same CS.

        SAFERELEASE(_pNameRefPolicy);
        _pNameRefPolicy = pNamePolicy;

        cs.Unlock();
    }

    // Do the check after applying policy. Loader relies on we give them back
    // the post policy assembly name.
    //
    // we will check if we have seen this bind before or not. 
    if (!_bInspectionOnly) {
        hr = CheckBindingResultCache(ppv);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    // If we have a fully-specified ref, now _pNameRefPolicy points to
    // exactly what we are looking for. Check the activated assemblies list.

    if (!bWhereRefBind && !bIsPartial && !_bInspectionOnly) {
        IUnknown *pUnk = NULL;
        hr = _pLoadContext->CheckActivated(_pNameRefPolicy, &pUnk, _pdbglog);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr != S_OK) {
            // not found, 
            // if this is in loadfrom context, let's also try default load context
            if (_pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
                CLoadContext *pLoadContext = NULL;
                CApplicationContext *pCAppCtx = static_cast<CApplicationContext *>(_pAppCtx); 

                hr = pCAppCtx->GetLoadContext(&pLoadContext, LOADCTX_TYPE_DEFAULT);
                if (FAILED(hr)) {
                    goto Exit;
                }

                hr = pLoadContext->CheckActivated(_pNameRefPolicy, &pUnk, _pdbglog);
                SAFERELEASE(pLoadContext);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (hr == S_OK) {
                    // if we find it in default load context, we should switch to default load context.
                    hr = SwitchLoadContext(TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }
        }

        if (hr == S_OK) {
            hr = pUnk->QueryInterface(IID_IHostAssembly, (void **)&pHostAsm);
            if (SUCCEEDED(hr)) {
                // host assembly is final. We will not probe for ngen assembly.
                *ppv = pUnk;
                SAFERELEASE(pHostAsm);
                DEBUGOUT(_pdbglog, 0, ID_FUSLOG_LOADCTX_HIT_HOST);
                goto Exit;
            }
            else {
                hr = pUnk->QueryInterface(IID_IAssembly, (void **)&pAsm);
                
                // Release pUnk since we don't need it any more
                SAFERELEASE(pUnk);

                if (FAILED(hr)) {
                    goto Exit;
                }

                dwPathSize = MAX_PATH;
                if (_pdbglog && IsLoggingNeeded() && SUCCEEDED(pAsm->GetManifestModulePath(wzManifestPath, &dwPathSize))) {
                    DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_BIND_SUCCESS, wzManifestPath);
                    DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_SHOW_RESULT_LOAD_CONTEXT, _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM? L"LoadFrom" : L"default");
                }


                *ppv = pAsm;
                if (ppvNI) {
                    *ppvNI = pAsmNI;
                }

                goto Exit;
            }
        }
    }

    // not in activated list. Need probe.
   
    // lookup host first if host has a list.
    if (g_bFusionHosted && g_pHostAsmList) {
        DEBUGOUT(_pdbglog, 0, ID_FUSLOG_CHECK_HOST);
    
        hr = g_pHostAsmList->IsAssemblyReferenceInList(_pNameRefPolicy);
        if (FAILED(hr)) {
            goto Exit;
        }
        if (hr == S_FALSE) {
            DEBUGOUT(_pdbglog, 0, ID_FUSLOG_NOT_IN_CLR_LOADED_LIST);
            hr = HostStoreAssemblyLookup(_pNameRefSource, _pNameRefPolicy, _dwPolicyApplied, _pAppCtx, _pdbglog, &_pHostAsm);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (_pHostAsm) {
                // Resolved by host.
                
                // check ref/def mismatch.
                IAssemblyName *pHostAsmName = NULL;
                
                hr = _pHostAsm->GetAssemblyNameDef(&pHostAsmName);
                if (FAILED(hr)) {
                    goto Exit;
                }
                
                CAssemblyName* pCNameRefPolicy = static_cast<CAssemblyName *>(_pNameRefPolicy);
                
                hr = pCNameRefPolicy->IsEqualLogging(pHostAsmName, ASM_CMPF_DEFAULT, _pdbglog);
                if (FAILED(hr)) {
                    goto Exit;
                }

                SAFERELEASE(pHostAsmName);
                if (hr != S_OK) {
                    hr = HRESULT_FROM_WIN32(FUSION_E_REF_DEF_MISMATCH);
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_REF_DEF_MISMATCH);
                    SAFERELEASE(_pHostAsm);
                    goto Exit;
                }

                if (!_bInspectionOnly) { 
                    hr = RecordInfo(bApplyPolicy, TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }

                hr = S_OK;
                *ppv = _pHostAsm;
                _pHostAsm->AddRef();

                DEBUGOUT(_pdbglog, 0, ID_FUSLOG_HOST_RESOLVED);
                goto Exit;
            }
        }
        else {
            DEBUGOUT(_pdbglog, 0, ID_FUSLOG_IN_CLR_LOADED_LIST);
        }
    }

    // Check DEVOVERRIDE path

    if (bWhereRefBind) {
        LPWSTR                   pwzFileName;
        WCHAR                    wzName[MAX_PATH + 1];
        int                      iLen;

        _ASSERTE(_wzBTOCodebase && _wzBTOCodebase[0]);

        hr = _pNameRefPolicy->Clone(&pNameDevOverride);
        if (FAILED(hr)) {
            goto Exit;
        }

        pwzFileName = PathFindFileName(_wzBTOCodebase);
        iLen = lstrlenW(pwzFileName);
        if (iLen > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            goto Exit;
        }

        hr = StringCbCopy(wzName, sizeof(wzName), pwzFileName);
        if (FAILED(hr)) {
            goto Exit;
        }

        PathRemoveExtensionW(wzName);

        hr = pNameDevOverride->SetProperty(ASM_NAME_NAME, wzName, (lstrlenW(wzName) + 1) * sizeof(WCHAR));
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        pNameDevOverride = _pNameRefPolicy;
        pNameDevOverride->AddRef();
    }

    // don't look at dev override for partial name binding. 
    if (!bIsPartial) {
        hr = ProcessDevOverride(&pAsm, pNameDevOverride, _pAppCtx, _llFlags, _pdbglog);
        if (hr == S_OK) {
            if (!_bInspectionOnly) {
                // need to switch to default load context for dev override. 
                if (_pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
                    hr = SwitchLoadContext(TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }

            SAFERELEASE(_pAsm);
            _pAsm = pAsm;

            SetAsmLocation(_pAsm, ASMLOC_DEV_OVERRIDE);

            SAFERELEASE(_pNameRefPolicy);

            hr = _pAsm->GetAssemblyNameDef(&_pNameRefPolicy);
            if (FAILED(hr)) {
                goto Exit;
            }
           
            if (!_bInspectionOnly) { 
                hr = RecordInfoAndProbeNativeImage(bApplyPolicy && !bIsPartial, TRUE, TRUE);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            hr = S_OK;

            *ppv = _pAsm;
            _pAsm->AddRef();

            if (ppvNI) {
                (*ppvNI) = _pAsmNI;
                if (_pAsmNI) {
                    _pAsmNI->AddRef();
                }
            }

            goto Exit;
        }
    }

        
    if ((!bWhereRefBind && !bIsPartial) && bIsStronglyNamed) {
        // Create the assembly

        hr = CreateAssemblyFromCacheLookup(_pAppCtx, _pNameRefPolicy, &pAsm, _pdbglog);
        if (hr == S_OK) {
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_CACHE_LOOKUP_SUCCESS);

            // need to switch to default load context for GAC assemblies. 

            if (!_bInspectionOnly) {
                if (_pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
                    hr = SwitchLoadContext(TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }

            _ASSERTE(static_cast<CAssembly *>(pAsm) != NULL); // dynamic_cast
            
            SAFERELEASE(_pAsm);
            _pAsm = pAsm;

            SetAsmLocation(_pAsm, ASMLOC_GAC);

            if (!_bInspectionOnly) { 
                hr = RecordInfoAndProbeNativeImage(bApplyPolicy, TRUE, _bNativeImageExists);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            hr = S_OK;
            *ppv = _pAsm;
            _pAsm->AddRef();

            dwPathSize = MAX_PATH;
            if (_pdbglog && IsLoggingNeeded() && SUCCEEDED(_pAsm->GetManifestModulePath(wzManifestPath, &dwPathSize))) {
                DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_BIND_SUCCESS, wzManifestPath);
                if (!_bInspectionOnly) {
                    DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_SHOW_RESULT_LOAD_CONTEXT, _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM? L"LoadFrom" : L"default");
                }
            }

            goto Exit;
        }

        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            goto Exit;
        }
    }

    // if we are hosted, and there is no host allowed list, we will ask the host store before going appbase.
    if (!bWhereRefBind && !bIsPartial && !_bInspectionOnly) {
        if (g_bFusionHosted && !g_pHostAsmList) {
            DEBUGOUT(_pdbglog, 0, ID_FUSLOG_CHECK_HOST);
    
            hr = HostStoreAssemblyLookup(_pNameRefSource, _pNameRefPolicy, _dwPolicyApplied, _pAppCtx, _pdbglog, &_pHostAsm);
            if (FAILED(hr) && (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))) {
                goto Exit;
            }
            hr = S_OK;

            if (_pHostAsm) {
                // Resolved by host.
                
                // check ref/def mismatch.
                IAssemblyName *pHostAsmName = NULL;
                
                hr = _pHostAsm->GetAssemblyNameDef(&pHostAsmName);
                if (FAILED(hr)) {
                    goto Exit;
                }
                
                CAssemblyName* pCNameRefPolicy = static_cast<CAssemblyName *>(_pNameRefPolicy);
                
                hr = pCNameRefPolicy->IsEqualLogging(pHostAsmName, ASM_CMPF_DEFAULT, _pdbglog);
                if (FAILED(hr)) {
                    goto Exit;
                }

                SAFERELEASE(pHostAsmName);
                if (hr != S_OK) {
                    hr = HRESULT_FROM_WIN32(FUSION_E_REF_DEF_MISMATCH);
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_REF_DEF_MISMATCH);
                    SAFERELEASE(_pHostAsm);
                    goto Exit;
                }

                if (!_bInspectionOnly) { 
                    hr = RecordInfo(bApplyPolicy, TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }

                hr = S_OK;
                *ppv = _pHostAsm;
                _pHostAsm->AddRef();

                DEBUGOUT(_pdbglog, 0, ID_FUSLOG_HOST_RESOLVED);
                goto Exit;
            }
        }
    }

    // Couldn't find asm in activated asm list, or cache. Must probe.
    // Don't use the _wzBTOCodebase if this is a re-verify path for where-ref bind.
    hr = ConstructCodebaseList(wzPolicyCodebase, !_bNeedReVerify, !_bNeedReVerify, &bHasHttpOrUncUrl);
    if (FAILED(hr)) {
        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_CODEBASE_CONSTRUCTION_FAILURE);
        goto Exit;
    }
    else {
        // Make sure we have at least one codebase
        hr = GetCount(&dwCount);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (!dwCount) {
            // No codebases in codebase list (either client provided empty
            // codebase list, or we couldn't generate any).
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_CODEBASE_UNAVAILABLE);
            hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            goto Exit;
        }
    }

    // If strongly-named, then look in download cache.
    if (bIsStronglyNamed && !bIsPartial && bHasHttpOrUncUrl) {
        hr = FusionGetUserFolderPath();
            if (FAILED(hr)) {
                goto Exit;
            }
            else {
                hr = LookupDownloadCacheAsm(&pAsm);
                if (hr == S_OK) {
                    WCHAR                    wzPath[MAX_PATH];
                    DWORD                    dwVerifyFlags = SN_INFLAG_USER_ACCESS;
                    BOOL                     bWasVerified = FALSE;
                    IAssemblyName            *pNameDef = NULL;
                    BOOL                     bInDefaultContext = FALSE;
                    BOOL                     bNeedReProbe = FALSE;

                    if (!_bInspectionOnly && _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
                        pwzCodebase = NEW(WCHAR[MAX_URL_LENGTH]);
                        if (!pwzCodebase) {
                            hr = E_OUTOFMEMORY;
                            goto Exit;
                        }
                        dwSize = MAX_URL_LENGTH * sizeof(WCHAR);

                        hr = pAsm->GetAssemblyNameDef(&pNameDef);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        hr = pNameDef->GetProperty(ASM_NAME_CODEBASE_URL, (LPBYTE)pwzCodebase, &dwSize);
                        SAFERELEASE(pNameDef);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        hr = CheckNeedToPutInDefaultLoadContext(pwzCodebase, NULL, _pNameRefPolicy, FALSE, &bInDefaultContext, &bNeedReProbe, NULL);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        if (bInDefaultContext) {
                            hr = SwitchLoadContext(TRUE);
                            if (FAILED(hr)) {
                                goto Exit;
                            }
                        }
                    }

                    wzPath[0] = L'\0';
                    dwSize = MAX_PATH;
                    hr = pAsm->GetManifestModulePath(wzPath, &dwSize);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                    
                    // Lookup in download cache successful. Verify signature.
                    if (!VerifySignature(wzPath, &bWasVerified, dwVerifyFlags)) {
                        hr = FUSION_E_SIGNATURE_CHECK_FAILED;
                        goto Exit;
                    }
            
                    // Success!
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_DOWNLOAD_CACHE_LOOKUP_SUCCESS);

                    SetAsmLocation(pAsm, ASMLOC_DOWNLOAD_CACHE);

                    SAFERELEASE(_pAsm);
                    _pAsm = pAsm;
                    
                    if (!_bInspectionOnly) { 
                        hr = RecordInfoAndProbeNativeImage(bApplyPolicy, TRUE, _bNativeImageExists);
                        if (FAILED(hr)) {
                            goto Exit;
                        }
                    }

                    hr = S_OK;

                    *ppv = _pAsm;
                    _pAsm->AddRef();

                    if (ppvNI) {
                        *ppvNI = _pAsmNI;
                        if (_pAsmNI) {
                            _pAsmNI->AddRef();
                        }
                    }
                    goto Exit;
                }
            }
    }

    // Indicate ready to being probing.
    hr = S_FALSE;

Exit:
    SAFEDELETEARRAY(wzPolicyCodebase);
    SAFEDELETEARRAY(pwzCodebase);

    SAFERELEASE(pNameDevOverride);

    if (FAILED(hr) && !_bPreviousFailed) {
        CacheBindingResult(hr);
    }
    
    return hr;
}

HRESULT CAsmDownloadMgr::GetDownloadIdentifier(DWORD *pdwDownloadType,
                                               __deref_out LPWSTR *ppwzID)
{
    HRESULT     hr = S_OK;
    DWORD       dwSize;
    BOOL        bWhereRefBind = FALSE;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    _ASSERTE(pdwDownloadType && ppwzID);

    if (!_pwzDownloadIdentifier) {
        dwSize = 0;
        if (_pNameRefPolicy->GetName(&dwSize, NULL) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            bWhereRefBind = TRUE;
        }

        // get download type and id.
        if (bWhereRefBind) {
            _pwzDownloadIdentifier = WSTRDupDynamic(_wzBTOCodebase);
            if (!_pwzDownloadIdentifier) {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }
        }
        else {
            dwSize = 0;
            hr = _pNameRefPolicy->GetDisplayName(NULL, &dwSize, 0);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                goto Exit;
            }
        
            _pwzDownloadIdentifier = NEW(WCHAR[dwSize]);
            if (!_pwzDownloadIdentifier) {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }
        
            hr = _pNameRefPolicy->GetDisplayName(_pwzDownloadIdentifier, &dwSize, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    *pdwDownloadType = _dwDownloadType;
    *ppwzID = WSTRDupDynamic(_pwzDownloadIdentifier);
    if (!*ppwzID) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = S_OK;

Exit:
    return hr;
}                                               

HRESULT CAsmDownloadMgr::IsDuplicate(IDownloadMgr *pIDLMgr)
{
    HRESULT                                    hr = S_FALSE;
    IApplicationContext                       *pAppCtxCur = NULL;
    CAsmDownloadMgr                           *pCDLMgr = NULL;
    LPWSTR                                     pwzIDSelf = NULL;
    LPWSTR                                     pwzIDCur = NULL;
    DWORD                                      dwTypeSelf;
    DWORD                                      dwTypeCur;

    if (!pIDLMgr) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // IsDuplicate is called from CAsmDownload::CheckDuplicate(), which is
    // under the g_csDownload crit sect. Thus, there is synchronization
    // between this code and the PreDownloadCheck code, where _pNameRefPolicy
    // may be released, and re-created.

    _ASSERTE(_pNameRefPolicy);
    
    pCDLMgr = static_cast<CAsmDownloadMgr *>(pIDLMgr); // dynamic_cast
    if (!pCDLMgr) {
        hr = E_FAIL;
        goto Exit;
    }

    // Check if app ctx's are equal

    hr = pCDLMgr->GetAppCtx(&pAppCtxCur);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (_pAppCtx != pAppCtxCur) {
        hr = S_FALSE;
        goto Exit;
    }
    
    // don't share loadfrom name based bind.
    if (_dwDownloadType == DLTYPE_QUALIFIED_REF_LOADFROM) {
        hr = S_FALSE;
        goto Exit;
    }

    // Check if identifiers are equal (same display name, or same URL)

    hr = GetDownloadIdentifier(&dwTypeSelf, &pwzIDSelf);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pCDLMgr->GetDownloadIdentifier(&dwTypeCur, &pwzIDCur);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Comapre types and identifiers

    if (dwTypeSelf != dwTypeCur) {
        hr = S_FALSE;
        goto Exit;
    } 

    hr = FusionCompareString(pwzIDCur, pwzIDSelf) ? (S_FALSE) : (S_OK);
    
Exit:
    SAFERELEASE(pAppCtxCur);

    SAFEDELETEARRAY(pwzIDCur);
    SAFEDELETEARRAY(pwzIDSelf);
    
    return hr;
}

HRESULT CAsmDownloadMgr::QualifyAssembly(IAssemblyName **ppNameQualified)
{
    HRESULT                                  hr = S_FALSE;;
    LPWSTR                                   wzDisplayName=NULL;
    DWORD                                    dwSize;
    CAppCtxPolicyConfigs                    *pAppCtxConfig = NULL;
    CApplicationContext                     *pCAppCtx = static_cast<CApplicationContext *>(_pAppCtx);
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    hr = pCAppCtx->ReadConfigSettings(_pdbglog, &pAppCtxConfig);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzDisplayName = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzDisplayName)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwSize = MAX_URL_LENGTH;
    hr = _pNameRefSource->GetDisplayName(wzDisplayName, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pAppCtxConfig->QualifyAssembly(wzDisplayName, ppNameQualified, _pdbglog);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFERELEASE(pAppCtxConfig);
    SAFEDELETEARRAY(wzDisplayName);
    return hr;
}

HRESULT CAsmDownloadMgr::GetAppCfgCodebaseHint(LPCWSTR pwzAppBase, __deref_out LPWSTR *ppwzCodebaseHint)
{
    HRESULT                                  hr = S_OK;
    CAppCtxPolicyConfigs                    *pAppCtxConfig = NULL;
    CApplicationContext                     *pCAppCtx = static_cast<CApplicationContext *>(_pAppCtx);

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    _ASSERTE(ppwzCodebaseHint);

    hr = pCAppCtx->ReadConfigSettings(_pdbglog, &pAppCtxConfig);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pAppCtxConfig->GetAppCfgCodebaseHint(_pNameRefPolicy, pwzAppBase, ppwzCodebaseHint);
    
Exit:
    SAFERELEASE(pAppCtxConfig);
    return hr;
}


HRESULT CAsmDownloadMgr::ShadowCopyDirCheck(LPCWSTR wzSourceURL)
{
    HRESULT                                hr = S_OK;
    WCHAR                                  wzCurDirBuf[MAX_PATH];
    WCHAR                                  wzCurURLBuf[MAX_URL_LENGTH];
    WCHAR                                  wzCurDirClean[MAX_PATH];
    WCHAR                                  wzFilePath[MAX_PATH];
    LPWSTR                                 pwzDirs = NULL;
    LPWSTR                                 pwzDirsCopy = NULL;
    LPWSTR                                 pwzCurDir = NULL;
    DWORD                                  dwSize = 0;
    BOOL                                   bFound = FALSE;

    if (!wzSourceURL || !UrlIsW(wzSourceURL, URLIS_FILEURL)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwSize = MAX_PATH;
    hr = PathCreateFromUrlWrap(wzSourceURL, wzFilePath, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ::AppCtxGetWrapper(_pAppCtx, ACTAG_APP_SHADOW_COPY_DIRS, &pwzDirs);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (hr == S_FALSE) {
        // No list of shadow copy dirs specified. Assume all dirs are ok.
        hr = S_OK;
        goto Exit;
    }

    _ASSERTE(pwzDirs);
    pwzDirsCopy = pwzDirs;

    while (pwzDirs) {
        pwzCurDir = ::GetNextDelimitedString(&pwzDirs, SHADOW_COPY_DIR_DELIMITER);

        if (lstrlenW(pwzCurDir) >= MAX_PATH || !lstrlenW(pwzCurDir)) {
            // Path too long. Disallow shadow copying for this path.
            continue;
        }

        hr = StringCbCopy(wzCurDirBuf, sizeof(wzCurDirBuf), pwzCurDir);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        hr = PathAddBackslashWrap(wzCurDirBuf, MAX_PATH);
        if (FAILED(hr)) {
            continue;
        }
        // Canonicalize and uncanonicalze it to clean up the path

        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(wzCurDirBuf, wzCurURLBuf, &dwSize, 0);
        if (FAILED(hr)) {
            continue;
        }

        dwSize = MAX_PATH;
        hr = PathCreateFromUrlWrap(wzCurURLBuf, wzCurDirClean, &dwSize, 0);
        if (FAILED(hr)) {
            continue;
        }

        if (SUCCEEDED(hr) && !FusionCompareStringNI(wzCurDirClean, wzFilePath, lstrlenW(wzCurDirClean))) {
            bFound = TRUE;
            break;
        }
    }

    hr = (bFound) ? (S_OK) : (S_FALSE);

Exit:
   SAFEDELETEARRAY(pwzDirsCopy);
   
   return hr;
}

HRESULT CAsmDownloadMgr::LogResult()
{
    HRESULT                            hr = S_OK;
    DWORD                              dwSize = MAX_PATH;
    WCHAR                              wzBuf[MAX_PATH];

    wzBuf[0] = L'\0';

    if (_bGACPartial) {
        return E_FAIL;
    }

    if (_bPreviousFailed) {
        hr = S_FALSE;
        goto Exit;
    }

    if (g_dwLogResourceBinds) {
        goto Exit;
    }

    if (_pNameRefPolicy) {
        _pNameRefPolicy->GetProperty(ASM_NAME_CULTURE, wzBuf, &dwSize);

        if ((dwSize > MAX_PATH) || (lstrlenW(wzBuf) && FusionCompareStringI(wzBuf, CFG_CULTURE_NEUTRAL))) {
            // A culture must be set (that is not "neutral").
            hr = S_FALSE;
        }
    }

Exit:
    return hr;
}
        
HRESULT CAsmDownloadMgr::CreateAssembly(LPCWSTR szPath, LPCWSTR pszURL,
                                        FILETIME *pftLastModTime,
                                        BOOL bRunFromSource,
                                        BOOL bWhereRef,
                                        BOOL bPrivateAsmVerify,
                                        BOOL bCopyModules,
                                        BOOL *pbBindRecorded,
                                        IAssembly **ppAsmOut)
{
    HRESULT                              hr = S_OK;
    CAssemblyCacheItem                  *pAsmItem = NULL;
    IAssemblyManifestImport             *pManImport = NULL;
    IAssemblyName                       *pNameDef = NULL;
    CAssemblyName                       *pCNameRefPolicy = NULL;    
    DWORD                                dwCmpMask = 0;
    BOOL                                 fIsPartial = FALSE;
    HANDLE                               hFile = INVALID_HANDLE_VALUE;
    DWORD                                dwIdx = 0;
    DWORD                                dwLen;
    WCHAR                                wzDir[MAX_PATH+1];
    WCHAR                                wzModPath[MAX_PATH+1];
    WCHAR                                wzModName[MAX_PATH+1];
    LPWSTR                               pwzTmp = NULL;
    IAssemblyModuleImport               *pModImport = NULL;
    BOOL                                 bExists;
    BOOL                                 bInDefaultContext = FALSE;
    BOOL                                 bNeedReprobe = FALSE;
    BOOL                                 bSameCodebase = FALSE;
    LPWSTR                               wzPolicyCodebase = NULL;
    LPCWSTR                              pwzRealPath = szPath;
    LPCWSTR                              pwzRealUrl = pszURL;
    CCriticalSection                     cs(g_csDownload);
    IAssemblyName                        *pNamePolicy = NULL;

    if (!szPath || !pszURL || !ppAsmOut || !pftLastModTime || !pbBindRecorded) {
        return E_INVALIDARG;
    }

    *pbBindRecorded = FALSE;

    if (_bNeedReVerify) {
        // we must be in default load context.
        _ASSERTE(_pLoadContext->GetContextType() == LOADCTX_TYPE_DEFAULT);
        _ASSERTE(_wzOldProbingPath);

        if (FusionCompareStringI(pszURL, _wzOldProbingUrl)) {
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_LOADFROM_DEFAULT_MISMATCH);
            // too bad. Have to switch back to LoadFrom context.
            hr = SwitchLoadContext(FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }
            pwzRealPath = _wzOldProbingPath;
            pwzRealUrl = _wzOldProbingUrl;
        }
    }

    // Get the manifest import and name def interfaces. 
    // This is done for all cases (where, simple and strong).
    hr = CreateAssemblyManifestImport(pwzRealPath, &pManImport);
    if (FAILED(hr)) {
        DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_MANIFEST_EXTRACT_FAILURE, hr);
        goto Exit;
    }

    if (!_bNeedReVerify) {
        // Get read-only name def from manifest.
        hr = pManImport->GetAssemblyNameDef(&pNameDef);
        if (FAILED(hr)) {
            DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_NAME_DEF_EXTRACT_FAILURE, hr);
            goto Exit;
        }

        if (_pdbglog && IsLoggingNeeded()) {
            dwLen = MAX_PATH;
            wzModName[0] = L'\0';
            LoggingGetDisplayName(pNameDef, wzModName, &dwLen, FALSE);
            DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_ASSEMBLY_NAME_DEF, wzModName);
        }

        // make sure we don't load assembly with different PA. 
        if (!_bInspectionOnly) {
            hr = IsValidAssemblyOnThisPlatform(pNameDef, FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        // Check to make sure that all private assemblies were located under
        // the appdir (or else fail).

        if (bPrivateAsmVerify && !bWhereRef) {
            hr = CheckValidAsmLocation(pNameDef, pwzRealUrl, _pAppCtx, _pwzProbingBase, _pdbglog);
            if (FAILED(hr)) {
                DEBUGOUT(_pdbglog, 1, ID_FUSLOG_INVALID_PRIVATE_ASM_LOCATION);
                goto Exit;
            }
        }
        
        // Determine if def matches ref using default
        // matching (no ver check for simple names, 
        // ver excluding rev/build for fully specified
        // strong names, also excluding any non-specified
        // values in the ref if partial.
        
        // Get the ref partial comparison mask if any.

        fIsPartial = CAssemblyName::IsPartial(_pNameRefPolicy, &dwCmpMask);
       
        if (!bWhereRef) { 
            pCNameRefPolicy = static_cast<CAssemblyName*>(_pNameRefPolicy); // dynamic_cast
            _ASSERTE(pCNameRefPolicy);

            hr = pCNameRefPolicy->IsEqualLogging(pNameDef, ASM_CMPF_DEFAULT, _pdbglog);
            if (hr != S_OK) {
                // Ref-def mismatch
                if (_pdbglog && IsLoggingNeeded()) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_REF_DEF_MISMATCH);
                }

                hr = FUSION_E_REF_DEF_MISMATCH;

                goto Exit;
            }
        }

        if (fIsPartial || bWhereRef) {
            // If the original ref was partial or where-ref bind, then we need to apply policy now
            // to the def that we found. This may involve a reprobe(!) for the
            // actual eventual assembly.
           
            // re-check if assembly is in host allowed list or not. 
            if (g_pHostAsmList) {
                hr = g_pHostAsmList->IsAssemblyReferenceInList(pNameDef);
                if (FAILED(hr)) {
                    goto Exit;
                }
                if (hr == S_FALSE) {
                    DEBUGOUT(_pdbglog, 0, ID_FUSLOG_NOT_IN_CLR_LOADED_LIST);
                    hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                    goto Exit;
                }
            }

            // by default where-ref don't have to do PreDownloadCheck again.
            bNeedReprobe = !bWhereRef;

            SAFERELEASE(_pNameRefSource);
            SAFERELEASE(_pCodebaseList);

            hr = pNameDef->Clone(&_pNameRefSource);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            // clear the PE.
            hr = _pNameRefSource->SetProperty(ASM_NAME_ARCHITECTURE, NULL, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        
            if (!_bInspectionOnly) {
                if (bWhereRef) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_APPLY_POLICY);

                    if (CAssemblyName::IsStronglyNamed(pNameDef)) {
                        // ApplyPolicy. 
                        hr = ApplyPolicy(_pNameRefSource, _pAppCtx, _pdbglog, &pNamePolicy, &wzPolicyCodebase , NULL, &_dwPolicyApplied);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        pCNameRefPolicy = static_cast<CAssemblyName*>(pNamePolicy); // dynamic_cast
                        _ASSERTE(pCNameRefPolicy);

                        hr = pCNameRefPolicy->IsEqualLogging(pNameDef, ASM_CMPF_DEFAULT, _pdbglog);
                        if (hr != S_OK) {
                            // has policy redirect. Should reprobe. 
                            // Also switch load context. 
                            bNeedReprobe = TRUE;
                            hr = SwitchLoadContext(TRUE);
                            if (FAILED(hr)) {
                                goto Exit;
                            }
                            // Remove _wzBTOCodebase. We don't need it anymore.
                            SAFEDELETEARRAY(_wzBTOCodebase);
                        }
                    }
                    else {
                        hr = _pNameRefSource->Clone(&pNamePolicy);
                        if (FAILED(hr)) {
                            goto Exit;
                        }
                    }

                    hr = cs.Lock();
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    // This block is protected by g_csDownload because this download
                    // object may be in the global list of in-progress assembly
                    // binds. The duplicate checking code expects to be able to get to
                    // _pNameRefPolicy, but we are in the process of updating it here
                    // to make it a qualified ref. This download list duplicate checking
                    // is done under g_csDownload, so we need to protect this code
                    // with the same CS.

                    SAFERELEASE(_pNameRefPolicy);
                    _pNameRefPolicy = pNamePolicy;
                    _pNameRefPolicy->AddRef();

                    cs.Unlock();
                }
                else {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_PARTIAL_ASM_IN_APP_DIR);
                }
            }

            if (bNeedReprobe) {
                hr = PreDownloadCheck((void **)ppAsmOut, NULL);
                if (hr == S_OK) {
                    // We applied policy and were able to locate the post-policy
                    // assembly somewhere (cache or actasm list). Return this
                    // assembly to the caller.
                    //
                    // Also, we must tell the caller that PreDownloadCheck already
                    // added to the activated asm list, etc.

                    _ASSERTE(*ppAsmOut);

                    *pbBindRecorded = TRUE;

                    goto Exit;
                }
                else if (hr == S_FALSE) {
                    if (_pNameRefSource->IsEqual(_pNameRefPolicy, ASM_CMPF_DEFAULT) == S_FALSE) {
                        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_REPROBE_REQUIRED);
                        goto Exit;
                    }
                }
                else if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }

        // check if need to put in default load context or not
        if (!_bInspectionOnly && _pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM) {
            bInDefaultContext = FALSE;
            bNeedReprobe = FALSE;
            hr = CheckNeedToPutInDefaultLoadContext(pwzRealUrl, wzPolicyCodebase, pNameDef, bWhereRef, &bInDefaultContext, &bNeedReprobe, ppAsmOut);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (bNeedReprobe) {
                // Only time need reprobe is where-ref bind.
                _ASSERTE(bWhereRef);
                _bNeedReVerify = TRUE;
                _wzOldProbingPath = WSTRDupDynamic(szPath);
                if (!_wzOldProbingPath) {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }

                _wzOldProbingUrl = WSTRDupDynamic(pwzRealUrl);
                if (!_wzOldProbingUrl) {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
                
                // need to switch to default context to start reprobing.
                hr = SwitchLoadContext(TRUE);
                if (FAILED(hr)) {
                    goto Exit;
                }

                SAFERELEASE(_pCodebaseList);
                hr = PreDownloadCheck((void **)ppAsmOut, NULL);
                if (hr == S_OK) {
                    // find in predownloadcheck
                    _ASSERTE(*ppAsmOut);

                    DWORD dwAsmLoc = ASMLOC_UNKNOWN;
                    hr = (*ppAsmOut)->GetAssemblyLocation(&dwAsmLoc);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if (dwAsmLoc != ASMLOC_GAC) {
                        // check to see if we have the same codebase or not. 
                        hr = IsSameCodebase(*ppAsmOut, pwzRealUrl, &bSameCodebase);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        if (!bSameCodebase) {
                            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MISMATCH);
                            SAFERELEASE(*ppAsmOut);
                            // need to switch back to LoadFrom context and continue.
                            hr = SwitchLoadContext(FALSE);
                            if (FAILED(hr)) {
                                goto Exit;
                            }
                            _bNeedReVerify = FALSE;
                        }
                        else {
                            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MATCH);
                            _bNeedReVerify = FALSE;
                            *pbBindRecorded = TRUE;
                            hr = S_OK;
                            goto Exit;
                        }
                    }
                    else {
                        _bNeedReVerify = FALSE;
                        *pbBindRecorded = TRUE;
                        hr = S_OK;
                        goto Exit;
                    }
                }
                else if (hr == S_FALSE) {
                    // not found. Has to probe.
                    goto Exit;
                }
                else if (FAILED(hr)) {
                    goto Exit;
                }
            }
            else {
                if (bInDefaultContext) {
                    hr = SwitchLoadContext(TRUE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    // If the assembly happens to be in GAC, we will fill ppAsmOut already. We can just return.
                    if (*ppAsmOut) {
#ifdef _DEBUG
                        DWORD dwAsmLoc = ASMLOC_UNKNOWN;
                        (*ppAsmOut)->GetAssemblyLocation(&dwAsmLoc);
                        _ASSERTE(dwAsmLoc == ASMLOC_GAC);
#endif                    
                        hr = S_OK;
                        goto Exit;
                    }

                    // since we switched load context,
                    // we need to check load context again. 
                    IUnknown *pUnk = NULL;
                    hr = _pLoadContext->CheckActivated(_pNameRefPolicy, &pUnk, _pdbglog);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if (hr == S_OK) {
                        hr = pUnk->QueryInterface(IID_IAssembly, (void **)ppAsmOut);
                        // Release pUnk since we don't need it any more
                        SAFERELEASE(pUnk);
                        hr = S_OK;
                        goto Exit;
                    }
                }
                // not in default load context, proceed.
            }
        }
    }

    if (bRunFromSource) {
        hr = CreateAssemblyFromManifestImport(pManImport, pwzRealUrl, pftLastModTime, ppAsmOut);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = CAssemblyCacheItem::Create(_pAppCtx, NULL, (LPWSTR)pwzRealUrl, pftLastModTime,
                                        CACHE_DOWNLOAD,
                                        pManImport, NULL, NULL,
                                        (IAssemblyCacheItem**)&pAsmItem);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CopyAssemblyFile(pAsmItem, pwzRealPath, STREAM_FORMAT_COMPLIB_MANIFEST);
        if (FAILED(hr)) {
            goto Exit;
        }
                
        if (bCopyModules) {
            if (lstrlenW(pwzRealPath) >= MAX_PATH) {
                hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                goto Exit;
            }

            hr = StringCbCopy(wzDir, sizeof(wzDir), pwzRealPath);
            if (FAILED(hr)) {
                goto Exit;
            }

            pwzTmp = PathFindFileName(wzDir);
            *pwzTmp = L'\0';

            // copy modules
            dwIdx = 0;
            while (SUCCEEDED(hr = pManImport->GetNextAssemblyModule(dwIdx++, &pModImport))) {
                dwLen = MAX_PATH;
                hr = pModImport->GetModuleName(wzModName, &dwLen);

                if (FAILED(hr))
                {
                    goto Exit;
                }

                hr = StringCchPrintf(wzModPath, MAX_PATH, L"%s%s", wzDir, wzModName);
                if (FAILED(hr)) {
                    goto Exit;
                }
                hr = CheckFileExistence(wzModPath, &bExists, NULL);
                if (FAILED(hr)) {
                    goto Exit;
                }
                else if (!bExists) {
                    // if module not found, it is ok in this case.
                    // just continue
                    SAFERELEASE(pModImport);
                    continue;
                }

                // Copy to cache.
                if (FAILED(hr = CopyAssemblyFile (pAsmItem, wzModPath, 0)))
                    goto Exit;

                SAFERELEASE(pModImport);
            }
        }
        // Commit the assembly. This inserts into the transport cache.

        hr = pAsmItem->Commit(0, NULL);
        if (FAILED(hr)) {

            // Dups are allowed, the asm item's trans cache entry
            // will be the existing entry if found.
            if (hr != DB_E_DUPLICATE) {
                goto Exit;
            }

            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_DUPLICATE_ASM_COMMIT);

            // Check to see if the manifest file has been deleted. If so,
            // then copy these bits to the cache to restore cache integrity.

            if(!pAsmItem->IsManifestFileLocked()) {
                hr = RecoverDeletedBits(pAsmItem, (LPWSTR)pwzRealPath, _pdbglog);
            }
            else hr = S_OK;
        }

        LPWSTR pszManifestFilePath = pAsmItem->GetManifestPath();

        hFile = pAsmItem->GetFileHandle();
        if(hFile==INVALID_HANDLE_VALUE)
        {
            if(FAILED(hr = GetManifestFileLock(pszManifestFilePath, &hFile)))
                goto Exit;
        }

        hr = CreateAssemblyFromManifestFile(pszManifestFilePath, pwzRealUrl, pftLastModTime, ppAsmOut);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:

    if(*ppAsmOut){
        CAssembly   *pCAsm = static_cast<CAssembly *>(*ppAsmOut); // dynamic_cast
        pCAsm->SetApplicationContextID(_pAppCtx);
        if (pAsmItem) {
            pCAsm->SetFileHandle(hFile);
        }
    }

    SAFEDELETEARRAY(wzPolicyCodebase);
    SAFERELEASE(pAsmItem);
    SAFERELEASE(pManImport);
    SAFERELEASE(pNameDef);
    SAFERELEASE(pNamePolicy);
    SAFERELEASE(pModImport);

    return hr;
}
    
HRESULT RecoverDeletedBits(CAssemblyCacheItem *pAsmItem, LPCWSTR szPath,
                           CDebugLog *pdbglog)
{
    HRESULT                         hr = S_OK;
    LPWSTR                          pszManifestPath=NULL;
    BOOL                            fExists = FALSE;

    _ASSERTE(pAsmItem);

    pszManifestPath = pAsmItem->GetManifestPath();

    hr = CheckFileExistence(pszManifestPath, &fExists, NULL);
    if(FAILED(hr) || fExists) {
        goto Exit;
    }

    CreateFilePathHierarchy(pszManifestPath);

    hr = CheckFileExistence(pszManifestPath, &fExists, NULL);
    if(FAILED(hr)) {
        goto Exit;
    }
    else if(!fExists) {
        WszCopyFile(szPath, pszManifestPath, TRUE);
    }

Exit:

    return hr;
}

HRESULT CAsmDownloadMgr::RecordInfo(BOOL bRecordPolicy, BOOL bAddActivation)
{
    HRESULT                               hr = S_OK;
    IUnknown                             *pAsmActivated = NULL;

    // Insert info into policy cache
   
    // record policy cache for strong named assembly bind excluding custom assembly.
    // Enable it for ngen bind support.
    if (bRecordPolicy && _pPolicyCache && CAssemblyName::IsStronglyNamed(_pNameRefPolicy)) {
        hr = _pPolicyCache->InsertPolicy(_pNameRefSource, _pNameRefPolicy, _pBindHistory, _dwPolicyApplied);
        if (FAILED(hr)) {
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_POLICY_CACHE_INSERT_FAILURE);
        }
    }

    // Add activation to load context

    if (bAddActivation) {
        if (_pAsm) {
#ifdef _DEBUG
            CAssembly *pCAsm = static_cast<CAssembly *>(_pAsm);
            _ASSERTE(pCAsm->GetApplicationContextID()==NULL || pCAsm->GetApplicationContextID() == _pAppCtx);
#endif
            hr = _pLoadContext->AddActivation(_pAsm, &pAsmActivated);
            if (FAILED(hr)) {
                goto Exit;
            }
            else if (hr == S_FALSE) {
                SAFERELEASE(_pAsm);
                hr = pAsmActivated->QueryInterface(IID_IAssembly, (void **)&_pAsm);
                SAFERELEASE(pAsmActivated);
                if (FAILED(hr)) {
                    goto Exit;
                }
                if (_pAsmNI) {
                    _pAsmNI->SetILAssembly(_pAsm);
                }
            }
        }
        else {
            hr = _pLoadContext->AddActivation(_pHostAsm, &pAsmActivated);
            if (FAILED(hr)) {
                goto Exit;
            }
            else if (hr == S_FALSE) {
                SAFERELEASE(_pHostAsm);
                hr = pAsmActivated->QueryInterface(IID_IHostAssembly, (void **)&_pHostAsm);
                SAFERELEASE(pAsmActivated);
            }
        }
    }

Exit:
    return hr;
}

HRESULT CAsmDownloadMgr::LookupDownloadCacheAsm(IAssembly **ppAsm)
{
    HRESULT                                       hr = S_OK;
    LPWSTR                                        wzCodebase=NULL;
    DWORD                                         dwSize;
    DWORD                                         dwFlags;
    DWORD                                         dwCount;
    DWORD                                         i;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");
    
    _ASSERTE(ppAsm);

    hr = _pCodebaseList->GetCount(&dwCount);
    if (FAILED(hr)) {
        hr = S_FALSE;
        goto Exit;
    }

    wzCodebase = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzCodebase)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    for (i = 0; i < dwCount; i++) {
        dwSize = MAX_URL_LENGTH;
        hr = _pCodebaseList->GetCodebase(i, &dwFlags, wzCodebase, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = GetMRUDownloadCacheAsm(wzCodebase, ppAsm);
        if (hr == S_OK) {
            _ASSERTE(ppAsm);
            goto Exit;
        }
    }

    // Missed in download cache.

    hr = S_FALSE;

Exit:
    SAFEDELETEARRAY(wzCodebase);

    return hr;
}

HRESULT CAsmDownloadMgr::GetMRUDownloadCacheAsm(LPCWSTR pwzURL, IAssembly **ppAsm)
{
    HRESULT                                     hr = S_OK;
    IAssemblyName                              *pName = NULL;
    CCache                                     *pCache = NULL;
    CEnumCache                                 *pEnumR = NULL;
    CTransCache                                *pTransCache = NULL;
    CTransCache                                *pTC = NULL;
    CTransCache                                *pTCMax = NULL;
    TRANSCACHEINFO                             *pInfo = NULL;
    TRANSCACHEINFO                             *pInfoMax = NULL;
    IAssemblyManifestImport                    *pManifestImport=NULL;

    _ASSERTE(pwzURL && ppAsm);

    pEnumR = NEW(CEnumCache(FALSE, NULL));
    if (!pEnumR) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = CCache::Create(&pCache, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pCache->CreateTransCacheEntry(CACHE_DOWNLOAD, &pTransCache);
    if (FAILED(hr)) {
        goto Exit;
    }

    pInfo = (TRANSCACHEINFO *)pTransCache->_pInfo;
    pInfo->pwzCodebaseURL = WSTRDupDynamic(pwzURL);
    if (!pInfo->pwzCodebaseURL) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if(FAILED(hr = CreateCacheMutex()))
        goto Exit;

    hr = pEnumR->Init(pTransCache, CTransCache::TCF_SIMPLE_PARTIAL_CODEBASE_URL);
    if (FAILED(hr)) {
        goto Exit;
    }
    else if (hr == S_FALSE) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto Exit;
    }

    while (1) {
        hr = pCache->CreateTransCacheEntry(CACHE_DOWNLOAD, &pTC);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = pEnumR->GetNextRecord(pTC);
        if (FAILED(hr)) {
            SAFERELEASE(pTC);
            goto Exit;
        }
        else if (hr == S_FALSE) {
            // Done iterating
            SAFERELEASE(pTC);
            break;
        }

        if (!pTCMax) {
            pTCMax = pTC;
        }
        else {
            pInfoMax = (TRANSCACHEINFO *)pTCMax->_pInfo;
            pInfo = (TRANSCACHEINFO *)pTC->_pInfo;

            if ((pInfoMax->ftLastModified.dwHighDateTime < pInfo->ftLastModified.dwHighDateTime) ||
                ((pInfoMax->ftLastModified.dwHighDateTime == pInfo->ftLastModified.dwHighDateTime) &&
                 (pInfoMax->ftLastModified.dwLowDateTime < pInfo->ftLastModified.dwLowDateTime))) {

               // New maximum found

               _ASSERTE(pTCMax);

               SAFERELEASE(pTCMax);
               pTCMax = pTC;
            }
            else {
                // Old maximum is fine. Release current, and continue iterating.
                SAFERELEASE(pTC);
            }
        }
    }

    if (pTCMax) {
        if (FAILED(hr = CreateAssemblyManifestImport(pTCMax->_pInfo->pwzPath, &pManifestImport)))
            goto Exit;

        _ASSERTE(pManifestImport);

        if (FAILED(hr = pManifestImport->GetAssemblyNameDef(&pName)))
            goto Exit;

        _ASSERTE(pName);

        if (_pNameRefPolicy->IsEqual(pName, ASM_CMPF_DEFAULT) == S_OK) {
            // Match found!

            hr = CreateAssemblyFromTransCacheEntry(pTCMax, NULL, ppAsm);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = S_OK;
            CAssembly   *pCAsm = static_cast<CAssembly *>(*ppAsm); // dynamic_cast
            pCAsm->SetApplicationContextID(_pAppCtx);

        }
        else {
            hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

Exit:

    SAFERELEASE(pTransCache);
    SAFERELEASE(pCache);
    SAFERELEASE(pTCMax);
    SAFERELEASE(pName);
    SAFERELEASE(pManifestImport);

    SAFEDELETE(pEnumR);

    return hr;
}

HRESULT CAsmDownloadMgr::SetAsmLocation(IAssembly *pAsm, DWORD dwAsmLoc)
{
    HRESULT                             hr = S_OK;
    CAssembly                          *pCAsm = static_cast<CAssembly *>(pAsm); // dynamic_cast

    _ASSERTE(pAsm && pCAsm);

    if (_bCodebaseHintUsed) {
        dwAsmLoc |= ASMLOC_CODEBASE_HINT;
    }

    hr = pCAsm->SetAssemblyLocation(dwAsmLoc);

    return hr;
}

HRESULT CAsmDownloadMgr::GetAppCtx(IApplicationContext **ppAppCtx)
{
    _ASSERTE(ppAppCtx && _pAppCtx);

    *ppAppCtx = _pAppCtx;
    (*ppAppCtx)->AddRef();

    return S_OK;
}

HRESULT CAsmDownloadMgr::DownloadEnabled(BOOL *pbEnabled)
{
    HRESULT                                   hr = S_OK;
    DWORD                                     cbBuf = 0;

    if (!pbEnabled) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (_llFlags & ASM_BINDF_DISABLE_DOWNLOAD) {
        *pbEnabled = FALSE;
        goto Exit;
    }

    hr = _pAppCtx->Get(ACTAG_CODE_DOWNLOAD_DISABLED, NULL, &cbBuf, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        *pbEnabled = FALSE;
        hr = S_OK;
        goto Exit;
    }

    hr = S_OK;
    *pbEnabled = TRUE;

Exit:
    return hr;
}

HRESULT CAsmDownloadMgr::GetBindInfo(FusionBindInfo *pBindInfo)
{
    HRESULT                                 hr = S_OK;

    if (!pBindInfo) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pBindInfo->pdbglog = _pdbglog;

    // Loader wants this post policy assembly name so that they can 
    // invoke the AssemblyResolve event with the right assembly name. 
    // They only need this for loads in the default context, or
    // LoadFrom promoted to the default context.
    //
    if (_pLoadContext && _pLoadContext->GetContextType() == LOADCTX_TYPE_DEFAULT) {
        pBindInfo->pNamePolicy = _pNameRefPolicy;
    }

    pBindInfo->dwPoliciesApplied = _dwPolicyApplied;

    if (pBindInfo->pdbglog) {
        pBindInfo->pdbglog->AddRef();
    }

    if (pBindInfo->pNamePolicy) {
        pBindInfo->pNamePolicy->AddRef();
    }

Exit:
    return hr;
}

HRESULT CAsmDownloadMgr::CacheBindingResult(HRESULT hrResult)
{
    HRESULT hr = S_OK;

    if (!_bInspectionOnly && !DisableCachingBindingFailures()) {
        hr = CacheBindResult(_pBindResultCache, _pBindInput, NULL, _pLoadContext->GetContextType(), hrResult);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

// This method should be called after the assembly is found by non-gac probing.
// These includes:appbase, download cache, dev override.
HRESULT CAsmDownloadMgr::RecordInfoAndProbeNativeImage(BOOL bRecordPolicy, BOOL bAddActivation, BOOL bProbeNativeImage)
{
    HRESULT hr = S_OK;
    CAssembly *pCAsm = NULL;

    _ASSERTE(_pAsm);

    if (_pwzProbingBase) {
        // We must be a child of a LoadFrom, so let's set the probing base
        pCAsm = static_cast<CAssembly *>(_pAsm); // dynamic_cast
        pCAsm->SetProbingBase(_pwzProbingBase);
    }

    hr = RecordInfo(bRecordPolicy, bAddActivation);
    if (FAILED(hr)) {
        goto Exit;
    }


Exit:
    return hr;
}

extern const WCHAR g_szDotDLL[];
extern const WCHAR g_szDotEXE[];

// Check to see if the found assembly should be put into default load context or not. 
HRESULT CAsmDownloadMgr::CheckNeedToPutInDefaultLoadContext(
                        LPCWSTR wzSourceURL,
                        LPCWSTR wzPolicyCodebase,
                        IAssemblyName *pName,
                        BOOL    bWhereRefBind,
                        BOOL    *pbIsInDefaultContext,
                        BOOL    *pbNeedReProbe,
                        IAssembly **ppAsmOut
                    )
{
    HRESULT hr = S_OK;
    LPWSTR  pwzCodebase = NULL;
    DWORD   dwSize = 0;
    DWORD   dwFlags = 0;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwCount = 0;
    BOOL    bUnderAppBase = FALSE;
    IAssemblyName *pNameClone = NULL;
    IHostAssembly *pHostAsm = NULL;

    _ASSERTE(wzSourceURL && wzSourceURL[0]);

    pwzCodebase = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzCodebase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if (!bWhereRefBind) {
        _ASSERTE(_pwzProbingBase);

        // if wzSourceURL is not under _pwzProbingBase, then it should be put in default context. 
        if (!FusionCompareStringNI(wzSourceURL, _pwzProbingBase, lstrlenW(_pwzProbingBase))) {
            // under _pwzProbingBase. We should be able to say it is in loadfrom context. 
            // But there is a weird corner case when _pwzProbingBase is one of the default probing 
            // directory. So we will just check the probing list without probing base. 
            SAFERELEASE(_pCodebaseList);
            hr = ConstructCodebaseList(wzPolicyCodebase, FALSE, FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }

            dwCount = 0;
            hr = _pCodebaseList->GetCount(&dwCount);
            if (FAILED(hr)) {
                goto Exit;
            }

            // check if in default probing list
            for (DWORD i=0; i<dwCount; i++) {
                dwSize = MAX_URL_LENGTH;
                hr = _pCodebaseList->GetCodebase(i, &dwFlags, pwzCodebase, &dwSize);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (!FusionCompareStringI(wzSourceURL, pwzCodebase)) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MATCH);
                    *pbIsInDefaultContext = TRUE;
                    *pbNeedReProbe = FALSE;
                    goto Exit;
                }
            }

            // not in default probing list, should be in loadfrom context.
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MISMATCH);
            *pbIsInDefaultContext = FALSE;
            *pbNeedReProbe = FALSE;
        }
        else {
            // we already probed appbase/codebase. We can just put it in default context. 
            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MATCH);
            *pbIsInDefaultContext = TRUE;
            *pbNeedReProbe = FALSE;
        }
    }
    else {
        // where ref bind.
        _ASSERTE(pName);
        _ASSERTE(ppAsmOut);
        // 1. check if the assembly is in GAC or not.
        if (CAssemblyName::IsStronglyNamed(pName)) {
            hr = CreateAssemblyFromCacheLookup(_pAppCtx, pName, ppAsmOut, _pdbglog);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
                goto Exit;
            }
            if (*ppAsmOut) {
                DEBUGOUT(_pdbglog, 1, ID_FUSLOG_CACHE_LOOKUP_SUCCESS);
                *pbIsInDefaultContext = TRUE;
                *pbNeedReProbe = FALSE;
                goto Exit;
            }
        }

        // can't find in GAC or simply named assembly.
        hr = S_OK;
        
        if (g_bFusionHosted && !g_pHostAsmList) {
            hr = pName->Clone(&pNameClone);
            if (FAILED(hr)){
                goto Exit;
            }

            hr = HostStoreAssemblyLookup(pNameClone, pNameClone, ePolicyLevelNone, _pAppCtx, _pdbglog, &pHostAsm);
            if (FAILED(hr) && (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))) {
                goto Exit;
            }
            hr = S_OK;

            if (pHostAsm) {
                SAFERELEASE(pHostAsm);
                *pbIsInDefaultContext = FALSE;
                *pbNeedReProbe = FALSE;
                goto Exit;
            }
        }

        dwSize = MAX_URL_LENGTH;

        // When this method is called, it should be safe to replace _pCodebaseList. 
        SAFERELEASE(_pCodebaseList);
        hr = ConstructCodebaseList(wzPolicyCodebase, FALSE, FALSE);
        if (FAILED(hr)) {
            goto Exit;
        }

        // check codebase hint
        if (_bCodebaseHintUsed) {
            // if codebase hint used, _pCodebaseList should only have one entry.
            hr = _pCodebaseList->GetCodebase(0, &dwFlags, pwzCodebase, &dwSize);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (!FusionCompareStringI(wzSourceURL, pwzCodebase)) {
                DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MATCH);
                *pbIsInDefaultContext = TRUE;
                *pbNeedReProbe = FALSE;
                goto Exit;
            }
            else {
                DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MISMATCH);
                *pbIsInDefaultContext = FALSE;
                *pbNeedReProbe = FALSE;
                goto Exit;
            }
        }
        else {
            // check if under appbase or dynamic directory
            dwSize = MAX_PATH;
            hr = _pAppCtx->GetDynamicDirectory(wzBuf, &dwSize);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
                goto Exit;
            }
            if (SUCCEEDED(hr)) {
                dwSize = MAX_URL_LENGTH;
                hr = UrlCanonicalizeUnescape(wzBuf, pwzCodebase, &dwSize, 0);
                if (FAILED(hr)) {
                    goto Exit;
                }
                dwSize /= sizeof(WCHAR);
                if (!FusionCompareStringNI(wzSourceURL, pwzCodebase, dwSize - 1)) {
                    bUnderAppBase = TRUE;
                }
            }

            hr = S_OK;

            if (!bUnderAppBase) {
                dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
                hr = _pAppCtx->Get(ACTAG_APP_BASE_URL, pwzCodebase, &dwSize, 0);
                if (FAILED(hr)) {
                    goto Exit;
                }

                dwSize /= sizeof(WCHAR);
                if (FusionCompareStringNI(wzSourceURL, pwzCodebase, dwSize - 1)) {
                    *pbIsInDefaultContext = FALSE;
                    *pbNeedReProbe = FALSE;
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MISMATCH);
                    goto Exit;
                }
                else {
                    bUnderAppBase = TRUE;
                }
            }


            if (bUnderAppBase) {
                // too bad. It is under appbase. We probably will need to reprobe. 
                // But we can do one more check to see if the codebase is in the default
                // probing list. If it is not, we won't need reprobe.
                dwCount = 0;
                hr = _pCodebaseList->GetCount(&dwCount);
                if (FAILED(hr)) {
                    goto Exit;
                }

                // check if in default probing list
                for (DWORD i=0; i<dwCount; i++) {
                    dwSize = MAX_URL_LENGTH;
                    hr = _pCodebaseList->GetCodebase(i, &dwFlags, pwzCodebase, &dwSize);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if (!FusionCompareStringI(wzSourceURL, pwzCodebase)) {
                        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_WHERE_REF_LOAD_CONTEXT_MATCH);
                        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_REPROBE_REQUIRED);
                        *pbIsInDefaultContext = TRUE;
                        *pbNeedReProbe = TRUE;
                        goto Exit;
                    }
                }
                // not in default probing list. Nothing need to be done. 
                *pbIsInDefaultContext = FALSE;
                *pbNeedReProbe = FALSE;
            }
        }
    }

Exit:
    SAFEDELETEARRAY(pwzCodebase);
    SAFERELEASE(pNameClone);
    return hr;
}

HRESULT CAsmDownloadMgr::SwitchLoadContext(BOOL bToDefaultContext)
{    
    HRESULT hr = S_OK;
    CApplicationContext                *pCAppCtx = static_cast<CApplicationContext *>(_pAppCtx); 

    if (bToDefaultContext) {
        _ASSERTE(_pLoadContext->GetContextType() == LOADCTX_TYPE_LOADFROM);
        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_SWITCH_TO_DEFAULT_CONTEXT);
        SAFERELEASE(_pLoadContext);
        hr = pCAppCtx->GetLoadContext(&_pLoadContext, LOADCTX_TYPE_DEFAULT);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        _ASSERTE(_pLoadContext->GetContextType() == LOADCTX_TYPE_DEFAULT);
        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_SWITCH_TO_LOADFROM_CONTEXT);
        SAFERELEASE(_pLoadContext);
        hr = pCAppCtx->GetLoadContext(&_pLoadContext, LOADCTX_TYPE_LOADFROM);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

// We only cache binding failure now. So we only care about the hr. 
// We can easily extend to cache binding success as well. 
HRESULT CAsmDownloadMgr::CheckBindingResultCache(void **ppv)
{
    HRESULT hr = S_OK;
    CBindingOutput *pBindOutput = NULL;
    if (!DisableCachingBindingFailures()) {
        hr = _pBindResultCache->CheckBindingResult(_pBindInput, &pBindOutput);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (pBindOutput) {
            hr = pBindOutput->_hr;
            if (FAILED(hr)) {
                DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_PREVIOUS_FAILED, hr);
                _bPreviousFailed = TRUE;
            }
        }
    }
    else {
        hr = S_FALSE;
    }

Exit:
    SAFERELEASE(pBindOutput);
    return hr;
}

HRESULT ProcessDevOverride(IAssembly **ppAsmOut, IAssemblyName *pName,
                           IApplicationContext *pAppCtx, LONGLONG llFlags,
                           CDebugLog *pdbglog)
{
    HRESULT                             hr = S_OK;
    CAssemblyProbe                     *pAsmProbe = NULL;
    CCodebaseList                      *pCodebaseList = NULL;
    LPWSTR                              pwzDevPath = NULL;
    IAssemblyName                      *pNameCopy = NULL;
    BOOL                                bNeedProcess = g_dwDevOverrideEnable;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");
    
    _ASSERTE(ppAsmOut);

    *ppAsmOut = NULL;

    if (!bNeedProcess) {
        // Check legacy DEVPATH
        hr = ::AppCtxGetWrapper(pAppCtx, ACTAG_DEV_PATH, &pwzDevPath);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            bNeedProcess = TRUE;
        }
    }

    if (!bNeedProcess) {
        hr = S_FALSE;
        goto Exit;
    }
    
    pCodebaseList = NEW(CCodebaseList);
    if (!pCodebaseList) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pNameCopy = pName;
    pNameCopy->AddRef();
    
    hr = CAssemblyProbe::Create(&pAsmProbe, pNameCopy, pAppCtx, 
                                llFlags, pdbglog, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (g_dwDevOverrideEnable) {
        if (g_dwDevOverrideFlags & DEVOVERRIDE_LOCAL) {
            hr = pAsmProbe->SetupDefaultProbeList(g_wzLocalDevOverridePath, NULL,
                                                  pCodebaseList, FALSE, FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }
    
            hr = CheckDevOverridePath(g_wzLocalDevOverridePath, pCodebaseList, pName,
                                      pdbglog, ppAsmOut);
            if (FAILED(hr)) {
                goto Exit;
            }
    
            if (*ppAsmOut) {
                goto Exit;
            }
        }
    
        if (g_dwDevOverrideFlags & DEVOVERRIDE_GLOBAL) {
            pCodebaseList->RemoveAll();
    
            hr = pAsmProbe->SetupDefaultProbeList(g_wzGlobalDevOverridePath, NULL,
                                                  pCodebaseList, FALSE, FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }
    
            hr = CheckDevOverridePath(g_wzGlobalDevOverridePath, pCodebaseList, pName,
                                      pdbglog, ppAsmOut);
            if (FAILED(hr)) {
                goto Exit;
            }
    
            if (*ppAsmOut) {
                goto Exit;
            }
        }
    }
    else {
        // Use legacy devpath
        hr = pAsmProbe->SetupDefaultProbeList(pwzDevPath, NULL, pCodebaseList, FALSE, FALSE);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CheckDevOverridePath(pwzDevPath, pCodebaseList, pName,
                                  pdbglog, ppAsmOut);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (*ppAsmOut) {
            goto Exit;
        }
    }

    hr = S_FALSE;

Exit:
    if (*ppAsmOut) {
        CAssembly   *pCAsm = static_cast<CAssembly *>(*ppAsmOut); // dynamic_cast
        pCAsm->SetApplicationContextID(pAppCtx);
    }

    SAFEDELETE(pCodebaseList);
    SAFEDELETE(pAsmProbe);

    SAFERELEASE(pNameCopy);
    SAFEDELETEARRAY(pwzDevPath);

    return hr;
}

HRESULT CheckDevOverridePath(LPCWSTR pwzDevOverridePath, CCodebaseList *pCodebaseList,
                             IAssemblyName *pName, CDebugLog *pdbglog,
                             IAssembly **ppAsmOut)
{
    HRESULT                              hr = S_OK;
    DWORD                                dwCount;
    DWORD                                dwSize;
    DWORD                                dwFlags;
    DWORD                                i;
    LPWSTR                               pwzCodebase = NULL;
    FILETIME                             ftLastModified;
    BOOL                                 bExists;
    IAssemblyManifestImport             *pManImport = NULL;
    IAssemblyName                       *pNameDef = NULL;
    WCHAR                                wzPath[MAX_PATH + 1];
    DWORD                                dwCmpFlags = ASM_CMPF_NAME |
                                                      ASM_CMPF_PUBLIC_KEY_TOKEN |
                                                      ASM_CMPF_CULTURE;

    _ASSERTE(pCodebaseList);

    hr = pCodebaseList->GetCount(&dwCount);
    if (FAILED(hr)) {
        goto Exit;
    }

    pwzCodebase = NEW(WCHAR[MAX_URL_LENGTH + 1]);
    if (!pwzCodebase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    for (i = 0; i < dwCount; i++) {
        dwSize = MAX_URL_LENGTH;
        hr = pCodebaseList->GetCodebase(i, &dwFlags, pwzCodebase, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }

        dwSize = MAX_PATH;
        hr = PathCreateFromUrlWrap(pwzCodebase, wzPath, &dwSize, 0);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CheckFileExistence(wzPath, &bExists, NULL);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (bExists) {
            DEBUGOUT1(pdbglog, 1, ID_FUSLOG_DEVOVERRIDE_FOUND, wzPath);

            // File exists. Crack the manifest and do ref/def matching
            
            hr = CreateAssemblyManifestImport(wzPath, &pManImport);
            if (FAILED(hr)) {
                goto Exit;
            }
        
            // Get read-only name def from manifest.
            hr = pManImport->GetAssemblyNameDef(&pNameDef);
            if (FAILED(hr)) {
                goto Exit;
            }
        
            hr = pName->IsEqual(pNameDef, dwCmpFlags);
            if (hr != S_OK) {
    
                DEBUGOUT1(pdbglog, 1, ID_FUSLOG_DEVOVERRIDE_REF_DEF_MISMATCH, wzPath);
                continue;
            }
    
            // Matching assembly found in dev path. Create the IAssembly.
        
            dwSize = MAX_URL_LENGTH;
            hr = UrlCanonicalizeUnescape(wzPath, pwzCodebase, &dwSize, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        
            hr = GetFileLastModified(wzPath, &ftLastModified);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            hr = CreateAssemblyFromManifestImport(pManImport, pwzCodebase, &ftLastModified, ppAsmOut);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            goto Exit;
        }
    }

    // No assembly in devoverride path

    hr = S_FALSE;

Exit:
    if (hr != S_OK) {
        DEBUGOUT1(pdbglog, 1, ID_FUSLOG_DEVOVERRIDE_MISS, pwzDevOverridePath);
    }

    SAFEDELETEARRAY(pwzCodebase);

    SAFERELEASE(pManImport);
    SAFERELEASE(pNameDef);

    return hr;
}


HRESULT CheckValidAsmLocation(IAssemblyName *pNameDef, LPCWSTR wzSourceUrl,
                              IApplicationContext *pAppCtx,
                              LPCWSTR pwzParentURL,
                              CDebugLog *pdbglog)
{
    HRESULT                             hr = S_OK;
    LPWSTR                              pwzAppBase = NULL;
    BOOL                                bUnderAppBase;
    DWORD                               dwAppBaseFlags = APPBASE_CHECK_DYNAMIC_DIRECTORY |
                                                         APPBASE_CHECK_PARENT_URL;

    if (!wzSourceUrl || !pNameDef) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    // If you're strongly named, you can be found anywhere, so just succeed.

    if (CAssemblyName::IsStronglyNamed(pNameDef)) {
        goto Exit;
    }

    // Get appbase

    hr = ::AppCtxGetWrapper(pAppCtx, ACTAG_APP_BASE_URL, &pwzAppBase);
    if (FAILED(hr)) {
        goto Exit;
    }
    

    bUnderAppBase = (IsUnderAppBase(pAppCtx, pwzAppBase, pwzParentURL,
                     wzSourceUrl, dwAppBaseFlags) == S_OK);


    if (!bUnderAppBase) {
        DEBUGOUT1(pdbglog, 1, ID_FUSLOG_INVALID_LOCATION_INFO, wzSourceUrl);

        hr = FUSION_E_INVALID_PRIVATE_ASM_LOCATION;
        goto Exit;
    }

Exit:
    SAFEDELETEARRAY(pwzAppBase);
    return hr;
}


HRESULT IsUnderAppBase(IApplicationContext *pAppCtx, 
                       LPCWSTR pwzAppBaseCanonicalized,
                       LPCWSTR pwzParentURLCanonicalized,
                       LPCWSTR pwzSourceCanonicalized,
                       DWORD dwExtendedAppBaseFlags)
{
    HRESULT                                 hr = S_OK;
    LPWSTR                                  pwzSharedPathHintCanonicalized = NULL;
    LPWSTR                                  pwzDynamicDirCanonicalized = NULL;
    WCHAR                                   wzDynamicDir[MAX_PATH];
    BOOL                                    bUnderAppBase = FALSE;
    DWORD                                   cbLen;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");

    if (!pAppCtx || !pwzAppBaseCanonicalized || !pwzSourceCanonicalized) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pwzDynamicDirCanonicalized = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzDynamicDirCanonicalized) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pwzSharedPathHintCanonicalized = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!pwzSharedPathHintCanonicalized) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    // Check if URL is really under appbase

    bUnderAppBase = (!FusionCompareStringNI(pwzSourceCanonicalized, pwzAppBaseCanonicalized, lstrlenW(pwzAppBaseCanonicalized)) != 0);
    
    if (dwExtendedAppBaseFlags & APPBASE_CHECK_DYNAMIC_DIRECTORY) {
        // Check dynamic directory
    
        if (!bUnderAppBase) {
            cbLen = MAX_PATH;
            hr = pAppCtx->GetDynamicDirectory(wzDynamicDir, &cbLen);
            if (SUCCEEDED(hr)) {
                cbLen = MAX_URL_LENGTH;
                hr = UrlCanonicalizeUnescape(wzDynamicDir, pwzDynamicDirCanonicalized, &cbLen, 0);
                if (FAILED(hr)) {
                    goto Exit;
                }
        
                bUnderAppBase = (!FusionCompareStringNI(pwzSourceCanonicalized, pwzDynamicDirCanonicalized, lstrlenW(pwzDynamicDirCanonicalized)) != 0);
            }
        }
    
    }

    if (pwzParentURLCanonicalized && (dwExtendedAppBaseFlags & APPBASE_CHECK_PARENT_URL)) {
        // Check parent URL

        if (!bUnderAppBase) {
            bUnderAppBase = (!FusionCompareStringNI(pwzSourceCanonicalized, pwzParentURLCanonicalized, lstrlenW(pwzParentURLCanonicalized)) != 0);
        }
    }

    hr = (bUnderAppBase) ? (S_OK) : (S_FALSE);

Exit:
    SAFEDELETEARRAY(pwzSharedPathHintCanonicalized);
    SAFEDELETEARRAY(pwzDynamicDirCanonicalized);

    return hr;
}


