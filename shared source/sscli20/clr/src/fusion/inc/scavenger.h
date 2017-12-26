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
#ifndef _SCAVENGER_H_
#define _SCAVENGER_H_


#include "fusionp.h"
#include "transprt.h"

// ---------------------------------------------------------------------------
// CScavenger
// static Scavenger class
// ---------------------------------------------------------------------------
class CScavenger : public IAssemblyScavenger
{
public:

    CScavenger();
    ~CScavenger();

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    // scavenging apis...

    STDMETHOD  (ScavengeAssemblyCache)(
               );

    STDMETHOD  (GetCacheDiskQuotas)(
                            /* [out] */ DWORD *pdwZapQuotaInGAC,
                            /* [out] */ DWORD *pdwDownloadQuotaAdmin,
                            /* [out] */ DWORD *pdwDownloadQuotaUser
                   );

        STDMETHOD (SetCacheDiskQuotas)
                   (
                            /* [in] */ DWORD dwZapQuotaInGAC,
                            /* [in] */ DWORD dwDownloadQuotaAdmin,
                            /* [in] */ DWORD dwDownloadQuotaUser
                   );

    STDMETHOD (GetCurrentCacheUsage)
                    (
                    /* [in] */ DWORD *dwZapUsage,
                    /* [in] */ DWORD *dwDownloadUsage
                    );



    static HRESULT DeleteAssembly( DWORD dwCacheFlags, LPCWSTR pszCustomPath, 
                                   LPCWSTR pszManFilePath, BOOL bForceDelete);

    static  HRESULT NukeDownloadedCache();

protected:

private :

    LONG _cRef;
};

HRESULT SetDownLoadUsage(   /* [in] */ BOOL  bUpdate,
                            /* [in] */ int   dwDownloadUsage);

HRESULT DoScavengingIfRequired(BOOL bSynchronous);

HRESULT CreateScavengerThread();

STDAPI CreateScavenger(IUnknown **);

STDAPI NukeDownloadedCache();

HRESULT FlushOldAssembly(LPCWSTR pszCustomPath, LPCWSTR pszAsmDirPath, LPCWSTR pszManifestFileName, BOOL bForceDelete);

HRESULT CleanupTempDir(DWORD dwCacheFlags, LPCWSTR pszCustomPath);

#endif // _SCAVENGER_H_
