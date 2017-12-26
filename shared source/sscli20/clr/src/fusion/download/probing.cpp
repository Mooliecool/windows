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
#include "appctx.h"
#include "hashnode.h"
#include "dbglog.h"
#include "list.h"
#include "probing.h"
#include "helpers.h"
#include "util.h"
#include "adlmgr.h"

typedef enum tagIdxVars {
    IDX_VAR_NAME = 0,
    IDX_VAR_CULTURE,
    NUM_VARS
} IdxVars;

// Order of g_pwzVars must follow the enum order above.

const LPCWSTR g_pwzVars[] = {
    L"%NAME%",
    L"%CULTURE%",
};

const LPCWSTR g_pwzRetailHeuristics[] = {
    L"%CULTURE%/%NAME%",
    L"%CULTURE%/%NAME%/%NAME%",
};

const LPCWSTR g_pwzProbeExts[] = {
    L".DLL",
    L".EXE",
#ifdef FEATURE_CASE_SENSITIVE_FILESYSTEM
    L".dll",
    L".exe",
    L".Dll",
    L".Exe",
#endif
};

const unsigned int g_uiNumRetailHeuristics = sizeof(g_pwzRetailHeuristics) / sizeof(g_pwzRetailHeuristics[0]);
const unsigned int g_uiNumProbeExtensions = sizeof(g_pwzProbeExts) / sizeof(g_pwzProbeExts[0]);

#define MAX_HASH_TABLE_SIZE                    127



CAssemblyProbe::CAssemblyProbe(IAssemblyName *pName, IApplicationContext *pAppCtx,
                               LONGLONG llFlags, CDebugLog *pdbglog)
: _pName(pName)
, _pAppCtx(pAppCtx)
, _pdbglog(pdbglog)
, _llFlags(llFlags)
, _pwzProbingBase(NULL)
{
    if (_pName) {
        _pName->AddRef();
    }

    if (_pAppCtx) {
        _pAppCtx->AddRef();
    }

    if (_pdbglog) {
        _pdbglog->AddRef();
    }
}

CAssemblyProbe::~CAssemblyProbe()
{
    SAFERELEASE(_pName);
    SAFERELEASE(_pAppCtx);
    SAFERELEASE(_pdbglog);

    SAFEDELETEARRAY(_pwzProbingBase);
}

HRESULT CAssemblyProbe::Create(CAssemblyProbe **ppAsmProbe, IAssemblyName *pName,
                               IApplicationContext *pAppCtx, LONGLONG llFlags,
                               CDebugLog *pdbglog, LPCWSTR pwzProbingBase)
{
    HRESULT                                   hr = S_OK;
    CAssemblyProbe                           *pAsmProbe = NULL;

    _ASSERTE(ppAsmProbe);

    *ppAsmProbe = NULL;

    pAsmProbe = NEW(CAssemblyProbe(pName, pAppCtx, llFlags, pdbglog));
    if (!pAsmProbe) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pAsmProbe->Init(pwzProbingBase);
    if (FAILED(hr)) {
        SAFEDELETE(pAsmProbe);
        goto Exit;
    }

    *ppAsmProbe = pAsmProbe;

Exit:
    return hr;
}

HRESULT CAssemblyProbe::Init(LPCWSTR pwzProbingBase)
{
    HRESULT                                   hr = S_OK;

    if (pwzProbingBase) {
        _pwzProbingBase = WSTRDupDynamic(pwzProbingBase);
        if (!_pwzProbingBase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT CAssemblyProbe::SetupDefaultProbeList(LPCWSTR pwzAppBase,
                                              LPCWSTR wzProbeFileName,
                                              ICodebaseList *pCodebaseList,
                                              BOOL bProbeBinPaths,
                                              BOOL bExtendedAppBaseCheck) 
{
    HRESULT                   hr = S_OK;
    WCHAR                    *pwzValues[NUM_VARS];
    WCHAR                     wzAppBase[MAX_URL_LENGTH];
    LPWSTR                    wzBinPathList = NULL;
    int                       iLen;
    unsigned int              i;
    DWORD                     dwAppBaseFlags = 0;

    memset(pwzValues, 0, sizeof(pwzValues));

    if (!_pAppCtx || !pCodebaseList) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (pwzAppBase) {
        _ASSERTE(pwzAppBase[0]);
        iLen = lstrlenW(pwzAppBase);
        
        hr = StringCbCopy(wzAppBase, sizeof(wzAppBase), pwzAppBase);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (pwzAppBase[iLen - 1] != L'\\' && pwzAppBase[iLen - 1] != L'/') {
            PathAddBackslashWrap(wzAppBase, MAX_URL_LENGTH);
        }
    }

    // Grab data from app context and name reference

    hr = ExtractSubstitutionVars(pwzValues);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (wzProbeFileName) {
        // Over-ride for probing filename specified
        SAFEDELETEARRAY(pwzValues[IDX_VAR_NAME]);
        pwzValues[IDX_VAR_NAME] = WSTRDupDynamic(wzProbeFileName);
    }

    // If there is no name, we can't probe.

    if (!pwzValues[IDX_VAR_NAME]) {
        hr = S_FALSE;
        goto Exit;
    }

    // Prepare binpaths

    if (bProbeBinPaths) {
        hr = PrepBinPaths(&wzBinPathList);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Probe for each of the listed extensions

    DWORD dwProbeExt;
    dwProbeExt = g_uiNumProbeExtensions;

    // Set appbase check flags

    // Generate probing URLs for appbase only if appbase is given. 
    // When DISALLOW_APP_BASE_PROBING is set, we won't want to probe app base. But we still want 
    // probe parent directory for loadfrom. In this case, we pass appbase as NULL in. 
    if (pwzAppBase) {
        if (bExtendedAppBaseCheck) {
            dwAppBaseFlags = APPBASE_CHECK_DYNAMIC_DIRECTORY | APPBASE_CHECK_SHARED_PATH_HINT;
        }

        for (i = 0; i < dwProbeExt; i++) {
            hr = GenerateProbeUrls(wzBinPathList, wzAppBase,
                                   g_pwzProbeExts[i], pwzValues,
                                   pCodebaseList, dwAppBaseFlags, _llFlags);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    // If this is a dependency of an assembly loaded through a where-ref
    // (ie. Assembly.LoadFrom) bind, then probe the parent assembly location
    // and the end. Do not probe binpaths, or dynamic dir in this case.

    if (bExtendedAppBaseCheck && _pwzProbingBase) {
        LPWSTR pwzProbingBaseClean;
        
        // Set appbase check flags
    
        pwzProbingBaseClean = StripFilePrefix(_pwzProbingBase);

        dwAppBaseFlags = APPBASE_CHECK_PARENT_URL;

        for (i = 0; i < dwProbeExt; i++) {
            hr = GenerateProbeUrls(NULL, pwzProbingBaseClean,
                                   g_pwzProbeExts[i], pwzValues,
                                   pCodebaseList, dwAppBaseFlags, (_llFlags & ~ASM_BINDF_BINPATH_PROBE_ONLY));
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    
Exit:
    // Free memory allocated by extract vars, now that the codebase list
    // has been constructed.

    for (i = 0; i < NUM_VARS; i++) {
        SAFEDELETEARRAY(pwzValues[i]);
    }

    SAFEDELETEARRAY(wzBinPathList);

    return hr;    
}

HRESULT CAssemblyProbe::GenerateProbeUrls(LPCWSTR wzBinPathList,
                                          LPCWSTR wzAppBase,
                                          LPCWSTR wzExt, __out_ecount(NUM_VARS) LPWSTR pwzValues[],
                                          ICodebaseList *pCodebaseList,
                                          DWORD dwExtendedAppBaseFlags,
                                          LONGLONG dwProbingFlags) 
{
    HRESULT                              hr = S_OK;
    LPWSTR                               wzBinPathCopy = NULL;
    LPWSTR                               wzCurBinPath = NULL;
    LPWSTR                               wzCurPos = NULL;
    DWORD                                cbLen = 0;
    DWORD                                dwSize = 0;
    DWORD                                dwLen = 0;
    LPWSTR                               wzPrefix = NULL;
    LPWSTR                               wzPrefixTmp = NULL;
    WCHAR                                wzDynamicDir[MAX_PATH];
    LPWSTR                               wzAppBaseCanonicalized = NULL;
    List<CHashNode *>                    *aHashList = NULL;
    LISTNODE                             pos = NULL;
    CHashNode                           *pHashNode = NULL;
    unsigned int                         i;

    aHashList = NEW(List<CHashNode *>[MAX_HASH_TABLE_SIZE]);
    if (!aHashList) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzAppBaseCanonicalized = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!wzAppBaseCanonicalized) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzPrefix = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!wzPrefix) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzPrefixTmp = NEW(WCHAR[MAX_URL_LENGTH]);
    if (!wzPrefix) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwSize = MAX_URL_LENGTH;
    hr = UrlCanonicalizeUnescape(wzAppBase, wzAppBaseCanonicalized, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Probe the appbase first

    if (!(dwProbingFlags & ASM_BINDF_BINPATH_PROBE_ONLY)) {
        hr = ApplyHeuristics(g_pwzRetailHeuristics, g_uiNumRetailHeuristics,
                             pwzValues, wzAppBase, wzExt,
                             wzAppBaseCanonicalized, pCodebaseList, aHashList,
                             dwExtendedAppBaseFlags);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    if (dwExtendedAppBaseFlags & APPBASE_CHECK_DYNAMIC_DIRECTORY) {
        // Add dynamic directory to the mix.
        
        cbLen = MAX_PATH;
        hr = _pAppCtx->GetDynamicDirectory(wzDynamicDir, &cbLen);
        if (SUCCEEDED(hr)) {
            hr = PathAddBackslashWrap(wzDynamicDir, MAX_PATH);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = ApplyHeuristics(g_pwzRetailHeuristics, g_uiNumRetailHeuristics,
                                 pwzValues, wzDynamicDir, wzExt, wzAppBaseCanonicalized,
                                 pCodebaseList, aHashList, dwExtendedAppBaseFlags);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        else {
            // Ignore if dynamic dir is not set.
            hr = S_OK;
        }
    }

    if (!wzBinPathList) {
        // No binpaths, we're done.
        goto Exit;
    }

    // Now probe the binpaths
         
    wzBinPathCopy = WSTRDupDynamic(wzBinPathList);
    if (!wzBinPathCopy) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzCurPos = wzBinPathCopy;
    while (wzCurPos) {
        wzCurBinPath = ::GetNextDelimitedString(&wzCurPos, BINPATH_LIST_DELIMITER);

        // "." is not a valid binpath! Ignore this.
        
        if (!FusionCompareString(wzCurBinPath, L".")) {
            continue;
        }

        // Build the prefix (canonicalization of appbase and binpath)

        // UrlCombineW will return the 'relative' URL part if it is an
        // absolute URL. However, the returned URL is in the canonicalized
        // form. To tell if the binpath was actually full-qualified (disallowed
        // for private probing), we canonicalize the binpath first, can
        // compare this with the combined form afterwards (if equal, then it
        // is absolute).

        // The function that calls us guarantees that if the appbase is
        // file://, the file:// will be stripped off (ie. we either have
        // an URL, or raw filepath).

        if (!PathIsURLW(wzAppBase) && !PathIsURLW(wzCurBinPath)) {
            cbLen = MAX_URL_LENGTH;
            hr = UrlCombineUnescape(wzAppBase, wzCurBinPath, wzPrefixTmp, &cbLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }

            // Get the literal file path back, to pass into ApplyHeuristics.

            cbLen = MAX_URL_LENGTH;
            hr = PathCreateFromUrlWrap(wzPrefixTmp, wzPrefix, &cbLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        else {
            // This is http:// so no special treatment necessary. Just
            // UrlCombine, and we're golden.

            cbLen = MAX_URL_LENGTH;
            hr = UrlCombineW(wzAppBase, wzCurBinPath, wzPrefix, &cbLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        dwLen = lstrlenW(wzPrefix);
        _ASSERTE(wzPrefix);

        if (wzPrefix[dwLen - 1] != L'/' && wzPrefix[dwLen - 1] != L'\\') {
            hr = StringCchCat(wzPrefix, MAX_URL_LENGTH, L"/");
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        // Now we have built a prefix. Apply the heuristics.

        hr = ApplyHeuristics(g_pwzRetailHeuristics, g_uiNumRetailHeuristics,
                             pwzValues, wzPrefix, wzExt, wzAppBaseCanonicalized,
                             pCodebaseList, aHashList, dwExtendedAppBaseFlags);

        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(wzBinPathCopy);
    
    // Clear dupe check hash table

    if (aHashList) {
        for (i = 0; i < MAX_HASH_TABLE_SIZE; i++) {
            if (aHashList[i].GetCount()) {
                pos = aHashList[i].GetHeadPosition();
                _ASSERTE(pos);

                while (pos) {
                    pHashNode = aHashList[i].GetNext(pos);
                    SAFEDELETE(pHashNode);
                }
            }
        }
    }

    SAFEDELETEARRAY(wzPrefix);
    SAFEDELETEARRAY(wzPrefixTmp);
    SAFEDELETEARRAY(wzAppBaseCanonicalized);
    SAFEDELETEARRAY(aHashList);

    return hr;    
}

//
// ApplyHeuristics takes a prefix, and will perform an UrlCombineUnescape
// to build the actual probe URL. This means that the prefix coming in
// needs to be escaped file:// URL (the UrlCombine won't touch the
// already-escaped characters), or a simple file path (the combine will
// escape the characters, which will subsequently get unescaped inside
// UrlCombineUnescape). If you pass in an *unescaped* file:// URL, then
// you will get double-unescaping (the already unescaped URL will pass
// through untouched by the UrlCombine, but will get hit by the explict
// UrlUnescape).
//
// For http:// URLs, as long as everything is always unescaped, we don't
// do explicit unescaping in our wrappers for UrlCombine/UrlCanonicalize
// so we can just pass these through.
//

HRESULT CAssemblyProbe::ApplyHeuristics(const LPCWSTR pwzHeuristics[],
                                         const unsigned int uiNumHeuristics,
                                         __out_ecount(NUM_VARS) WCHAR *pwzValues[],
                                         LPCWSTR wzPrefix,
                                         LPCWSTR wzExtension,
                                         LPCWSTR wzAppBaseCanonicalized,
                                         ICodebaseList *pCodebaseList,
                                         List<CHashNode *> aHashList[],
                                         DWORD dwExtendedAppBaseFlags) 
{
    HRESULT                         hr = S_OK;
    DWORD                           dwSize = 0;
    BOOL                            bUnderAppBase;
    LPWSTR                          pwzBuf=NULL;
    LPWSTR                          pwzNewCodebase=NULL;
    LPWSTR                          pwzCanonicalizedDir=NULL;
    unsigned int                    i;
    
    if (!pwzHeuristics || !uiNumHeuristics || !pwzValues || !pCodebaseList || !wzAppBaseCanonicalized) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pwzBuf = NEW(WCHAR[MAX_URL_LENGTH*3+3]);
    if (!pwzBuf)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pwzNewCodebase = pwzBuf + MAX_URL_LENGTH +1;
    pwzCanonicalizedDir = pwzNewCodebase + MAX_URL_LENGTH +1;

    pwzBuf[0] = L'\0';
    pwzCanonicalizedDir[0] = L'\0';

    for (i = 0; i < uiNumHeuristics; i++) {
        hr = ExpandVariables(pwzHeuristics[i], pwzValues, pwzBuf, MAX_URL_LENGTH);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCchPrintf(pwzNewCodebase, MAX_URL_LENGTH, L"%ws%ws%ws", wzPrefix,
                   pwzBuf, wzExtension);
        if (FAILED(hr)) {
            goto Exit;
        }


        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(pwzNewCodebase, pwzCanonicalizedDir, &dwSize, 0);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CheckProbeUrlDupe(aHashList, pwzCanonicalizedDir);
        if (SUCCEEDED(hr)) {
            bUnderAppBase = (IsUnderAppBase(_pAppCtx, wzAppBaseCanonicalized,
                                            _pwzProbingBase, pwzCanonicalizedDir,
                                            dwExtendedAppBaseFlags) == S_OK);

            if (bUnderAppBase) {
                hr = pCodebaseList->AddCodebase(pwzCanonicalizedDir, 0);
            }
            else {
                DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_IGNORE_INVALID_PROBE, pwzCanonicalizedDir);
            }
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) {
            hr = S_OK;
            continue;
        }
        else {
            // Fatal error
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(pwzBuf);
    return hr;
}                                       

HRESULT CAssemblyProbe::ExpandVariables(LPCWSTR pwzHeuristic, __out_ecount(NUM_VARS) WCHAR *pwzValues[],
                                         __out_ecount(iMaxLen) LPWSTR wzBuf, int iMaxLen) 
{
    HRESULT                         hr = S_OK;
    BOOL                            bCmp;
    LPCWSTR                         pwzCurPos = NULL;
    LPCWSTR                         pwzVarHead = NULL;
    unsigned int                    i;

    if (!pwzHeuristic || !wzBuf || !iMaxLen) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    wzBuf[0] = L'\0';

    pwzCurPos = pwzHeuristic;
    pwzVarHead = NULL;

    while (*pwzCurPos) {
        if (*pwzCurPos == L'%') {
            if (pwzVarHead) {
                // This % is the trailing delimiter of a variable

                for (i = 0; i < NUM_VARS; i++) {
                    bCmp = FusionCompareStringNI(pwzVarHead, g_pwzVars[i], lstrlenW(g_pwzVars[i]));
                    if (!bCmp) {
                        if (pwzValues[i]) {
                            hr = StringCchCat(wzBuf, iMaxLen, pwzValues[i]);
                            if (FAILED(hr)) {
                                goto Exit;
                            }
                            break;
                        }
                        else {
                            // No value specified. If next character
                            // is a backslash, don't bother concatenating
                            // it.
                            if (*(pwzCurPos + 1) == L'/') {
                                pwzCurPos++;
                                break;
                            }
                        }
                    }
                }
                        
                // Reset head
                pwzVarHead = NULL;
            }
            else {
                // This is the leading % for a variable

                pwzVarHead = pwzCurPos;
            }
        }
        else if (!pwzVarHead) {
            hr = StringCchCatN(wzBuf, iMaxLen, pwzCurPos, 1);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        pwzCurPos++;
    }

    _ASSERTE(lstrlenW(wzBuf) < iMaxLen);

Exit:
    return hr;
}

HRESULT CAssemblyProbe::CheckProbeUrlDupe(List<CHashNode *> paHashList[],
                                          LPCWSTR pwzSource) 
{
    HRESULT                                    hr = S_OK;
    DWORD                                      dwHash;
    DWORD                                      dwCount;
    LISTNODE                                   pos = NULL;
    CHashNode                                 *pHashNode = NULL;
    CHashNode                                 *pHashNodeCur = NULL;

    if (!pwzSource || !paHashList) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwHash = HashString(pwzSource, 0, MAX_HASH_TABLE_SIZE, FALSE);
    dwCount = paHashList[dwHash].GetCount();

    if (!dwCount) {
        // Empty hash cell. This one is definitely unique.

        hr = CHashNode::Create(pwzSource, &pHashNode);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (!paHashList[dwHash].AddTail(pHashNode))
        {
            hr = E_OUTOFMEMORY;
            SAFEDELETE(pHashNode);
            goto Exit;
        }
    }
    else {
        // Encountered hash table collision.

        // Check if we hit a duplicate, or if this was just a cell collision.

        pos = paHashList[dwHash].GetHeadPosition();
        _ASSERTE(pos);

        while (pos) {
            pHashNodeCur = paHashList[dwHash].GetNext(pos);
            _ASSERTE(pHashNodeCur);

            if (pHashNodeCur->IsDuplicate(pwzSource)) {
                // Duplicate found!
                
                hr = HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
                goto Exit;
            }
        }

        // If we get here, there was no duplicate (and we just had a
        // cell collision). Insert the new node.

        hr = CHashNode::Create(pwzSource, &pHashNode);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (!paHashList[dwHash].AddTail(pHashNode))
        {
            hr = E_OUTOFMEMORY;
            SAFEDELETE(pHashNode);
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT CAssemblyProbe::ExtractSubstitutionVars(__out_ecount(NUM_VARS) WCHAR *pwzValues[]) 
{
    HRESULT                         hr = S_OK;
    DWORD                           cbBuf = 0;
    LPWSTR                          wzBuf = NULL;
    unsigned int                    i;

    if (!pwzValues) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    for (i = 0; i < NUM_VARS; i++) {
        pwzValues[i] = NULL;
    }

    // The following properties are retrieved from the name object itself

    // Assembly Name
    cbBuf = 0;
    hr = _pName->GetName(&cbBuf, wzBuf);

    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        wzBuf = NEW(WCHAR[cbBuf]);
        if (!wzBuf) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    
        hr = _pName->GetName(&cbBuf, wzBuf);
        if (FAILED(hr)) {
            SAFEDELETEARRAY(wzBuf);
            goto Exit;
        }
        else {
            pwzValues[IDX_VAR_NAME] = wzBuf;
        }
    }
    else {
        pwzValues[IDX_VAR_NAME] = NULL;
    }

    // Culture

    cbBuf = 0;
    hr = _pName->GetProperty(ASM_NAME_CULTURE, NULL, &cbBuf);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        pwzValues[IDX_VAR_CULTURE] = NEW(WCHAR[cbBuf / sizeof(WCHAR)]);
    
        if (!pwzValues[IDX_VAR_CULTURE]) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = _pName->GetProperty(ASM_NAME_CULTURE, pwzValues[IDX_VAR_CULTURE], &cbBuf);
        if (FAILED(hr)) {
            goto Exit;
        }

        // If we have "\0" as Culture (default Culture), this is the same as
        // no Culture.

        if (!lstrlenW(pwzValues[IDX_VAR_CULTURE])) {
            SAFEDELETEARRAY(pwzValues[IDX_VAR_CULTURE]);
        }
    }
    
Exit:
    if (FAILED(hr) && pwzValues) {
        // reset everything to NULL and free memory

        for (i = 0; i < NUM_VARS; i++) {
            SAFEDELETEARRAY(pwzValues[i]);
        }
    }

    return hr;
}

HRESULT CAssemblyProbe::PrepBinPaths(__deref_out LPWSTR *ppwzUserBinPathList)
{
    HRESULT                                  hr = S_OK;
    LPWSTR                                   wzPrivate = NULL;

    if (!ppwzUserBinPathList) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = PrepPrivateBinPath(&wzPrivate);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ConcatenateBinPaths(wzPrivate, NULL, ppwzUserBinPathList);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFEDELETEARRAY(wzPrivate);

    return hr;
}

HRESULT CAssemblyProbe::PrepPrivateBinPath(__deref_out LPWSTR *ppwzPrivateBinPath)
{
    HRESULT                                     hr = S_OK;
    LPWSTR                                      wzPrivatePath = NULL;
    LPWSTR                                      wzCfgPrivatePath = NULL;

    _ASSERTE(ppwzPrivateBinPath);

    hr = ::AppCtxGetWrapper(_pAppCtx, ACTAG_APP_PRIVATE_BINPATH, &wzPrivatePath);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ::AppCtxGetWrapper(_pAppCtx, ACTAG_APP_CFG_PRIVATE_BINPATH, &wzCfgPrivatePath);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ConcatenateBinPaths(wzPrivatePath, wzCfgPrivatePath, ppwzPrivateBinPath);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Private binpath is always probed (considered path of the appbase).

Exit:
    SAFEDELETEARRAY(wzPrivatePath);
    SAFEDELETEARRAY(wzCfgPrivatePath);

    return hr;
}

HRESULT CAssemblyProbe::ConcatenateBinPaths(LPCWSTR pwzPath1, LPCWSTR pwzPath2,
                                            __deref_out LPWSTR *ppwzOut)
{
    HRESULT                                 hr = S_OK;
    DWORD                                   dwLen = 0;

    if (!ppwzOut) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (pwzPath1 && pwzPath2) {
        // +1 for delimiter, +1 for NULL;
        dwLen = lstrlenW(pwzPath1) + lstrlenW(pwzPath2) + 2;

        *ppwzOut = NEW(WCHAR[dwLen]);
        if (!*ppwzOut) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCchPrintf(*ppwzOut, dwLen, L"%ws%wc%ws", pwzPath1, BINPATH_LIST_DELIMITER,
                   pwzPath2);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else if (pwzPath1) {
        *ppwzOut = WSTRDupDynamic(pwzPath1);
    }
    else if (pwzPath2) {
        *ppwzOut = WSTRDupDynamic(pwzPath2);
    }
    else {
        *ppwzOut = NULL;
    }

Exit:
    return hr;
}                                          

