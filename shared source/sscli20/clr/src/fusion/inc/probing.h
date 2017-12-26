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
#ifndef __PROBING_H_
#define __PROBING_H_

class CDebugLog;

class CAssemblyProbe {
    public:
        CAssemblyProbe(IAssemblyName *pName, IApplicationContext *pAppCtx,
                       LONGLONG llFlags, CDebugLog *pdbglog);
        virtual ~CAssemblyProbe();

        static HRESULT Create(CAssemblyProbe **ppAsmPrbe, IAssemblyName *pName,
                              IApplicationContext *pAppCtx, LONGLONG llFlags,
                              CDebugLog *pdbglog, LPCWSTR pwzProbingBase);

        HRESULT SetupDefaultProbeList(LPCWSTR wzAppBase,
                                      LPCWSTR wzProbeFileName,
                                      ICodebaseList *pCodebaseList,
                                      BOOL bProbeBinPaths,
                                      BOOL bExtendedAppBaseCheck);


    private:
        HRESULT Init(LPCWSTR pwzProbingBase);

        HRESULT ExtractSubstitutionVars(__out_ecount(NUM_VARS) WCHAR *pwzValues[]);
        HRESULT GenerateProbeUrls(LPCWSTR wzBinPathList,
                                  LPCWSTR wzAppBase,
                                  LPCWSTR wzExt, __out_ecount(NUM_VARS) LPWSTR pwzValues[],
                                  ICodebaseList *pCodebaseList,
                                  DWORD dwExtendedAppBaseFlags,
                                  LONGLONG dwProbingFlags);
        HRESULT ApplyHeuristics(const LPCWSTR pwzHeuristics[],
                                const unsigned int uiNumHeuristics,
                                __out_ecount(NUM_VARS) WCHAR *pwzValues[],
                                LPCWSTR wzPrefix,
                                LPCWSTR wzExtension,
                                LPCWSTR wzAppBaseCanonicalized,
                                ICodebaseList *pCodebaseList,
                                List<CHashNode *> aHashList[],
                                DWORD dwExtendedAppBaseFlags);
        HRESULT ExpandVariables(LPCWSTR pwzHeuristic, __out_ecount(NUM_VARS) WCHAR *pwzValues[],
                                __out_ecount(iMaxLen) LPWSTR wzBuf, int iMaxLen);

        HRESULT CheckProbeUrlDupe(List<CHashNode *> paHashList[],
                                  LPCWSTR pwzSource);
        HRESULT PrepBinPaths(__deref_out LPWSTR *ppwzUserBinPathList);

        HRESULT ConcatenateBinPaths(LPCWSTR pwzPath1, LPCWSTR pwzPath2, __deref_out LPWSTR *ppwzOut);
        HRESULT PrepPrivateBinPath(__deref_out LPWSTR *ppwzPrivateBinPath);

    private:
        IAssemblyName                     *_pName;
        IApplicationContext               *_pAppCtx;
        CDebugLog                         *_pdbglog;
        LONGLONG                           _llFlags;
        LPWSTR                             _pwzProbingBase;
};

#endif  // __PROBING_H_
