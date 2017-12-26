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

#ifndef NAMING_H_
#define NAMING_H_

#include "fusionp.h"

// Define in naming.h
extern const WCHAR      g_wzRTMCorVersion[];
extern const WCHAR      g_wzEverettCorVersion[];
extern const WCHAR      g_wzStandardCLI2002[];

extern IAssemblyName *g_pSystemAssemblyName;
extern IAssembly *g_pSystemAssembly;
extern BOOL g_bSystemAssemblyVerified;

#define SYSTEM_ASSEMBLY_NAME  L"mscorlib"

class CDebugLog;
class CAssemblyDownload;
class CAsmDownloadMgr;
class CDebugLog;
class CAssembly;
class CLoadContext;

STDAPI
CreateAssemblyNameObjectFromMetaData(
    LPASSEMBLYNAME    *ppAssemblyName,
    LPCOLESTR          szAssemblyName,
    ASSEMBLYMETADATA  *pamd,
    LPVOID             pvReserved);

STDAPI PreBindAssemblyExInternal(
                IApplicationContext *pAppCtx, IAssemblyName *pName, 
                CLoadContext *pLoadContext, LPCWSTR pwzRuntimeVersion,
                IAssemblyName **ppNamePostPolicy, LPDWORD pdwPoliciesApplied, LPVOID pvReserved);

// classes invisible to 'C'

struct FusionProperty
{
    LPVOID pv;
    DWORD  cb;
};

class CPropertyArray
{
    friend class CAssemblyName;
private:

    DWORD    _dwSig;
    FusionProperty _rProp[ASM_NAME_MAX_PARAMS];

public:

    CPropertyArray();
    ~CPropertyArray();

    inline HRESULT Set(DWORD PropertyId, LPVOID pvProperty, DWORD  cbProperty);
    inline HRESULT Get(DWORD PropertyId, LPVOID pvProperty, LPDWORD pcbProperty);
    inline FusionProperty operator [] (DWORD dwPropId);
};

class CAssemblyName : public IAssemblyName, public IAssemblyNameBinder
{
private:

    DWORD        _dwSig;
    DWORD        _cRef;
    CPropertyArray _rProp;
    BOOL         _fIsFinalized;
    BOOL         _fPublicKeyToken;
    BOOL         _fCustom;
    LPWSTR       _pwzPathModifier;
    LPWSTR       _pwzTextualIdentity;
    LPWSTR       _pwzTextualIdentityILFull;

    DWORD _dw;

public:

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    // IAssemblyName methods
    STDMETHOD(SetProperty)(
        /* in */ DWORD  PropertyId,
        /* in */ LPVOID pvProperty,
        /* in */ DWORD  cbProperty);


    STDMETHOD(GetProperty)(
        /* in      */  DWORD    PropertyId,
        /*     out */  LPVOID   pvProperty,
        /* in  out */  LPDWORD  pcbProperty);


    STDMETHOD(Finalize)();

    STDMETHOD(GetDisplayName)(
        /* [out]   */   LPOLESTR  szDisplayName,
        /* in  out */   LPDWORD   pccDisplayName,
        /* [in]    */   DWORD     dwDisplayFlags);
   
    STDMETHOD(GetName)( 
        /* [out][in] */ LPDWORD lpcwBuffer,
        /* [out] */ LPOLESTR pwzBuffer);

    STDMETHOD(GetVersion)( 
        /* [out] */ LPDWORD pwVersionHi,
        /* [out] */ LPDWORD pwVersionLow);
    
    STDMETHOD (IsEqual)(
        /* [in] */ LPASSEMBLYNAME pName,
        /* [in] */ DWORD dwCmpFlags);
        
    STDMETHOD (IsEqualLogging)(
        /* [in] */ LPASSEMBLYNAME pName,
        /* [in] */ DWORD dwCmpFlags,
        /* [in] */ CDebugLog *pdbglog);

    STDMETHOD(Reserved)(
        /* in      */  REFIID               refIID,
        /* in      */  IUnknown            *pUnkBindSink,
        /* in      */  IUnknown            *pUnkAppCtx,
        /* in      */  LPCOLESTR            szCodebase,
        /* in      */  LONGLONG             llFlags,
        /* in      */  LPVOID               pvReserved,
        /* in      */  DWORD                cbReserved,
        /*     out */  VOID               **ppv);

    STDMETHODIMP Clone(IAssemblyName **ppName);

    // IAssemblyNameBinder 
    STDMETHOD(BindToObject)(
        /* in      */  REFIID               refIID,
        /* in      */  IUnknown            *pUnkBindSink,
        /* in      */  IUnknown            *pUnkAppCtx,
        /* in      */  LPCOLESTR            szCodebase,
        /* in      */  LONGLONG             llFlags,
        /* in      */  LPVOID               pvReserved,
        /* in      */  DWORD                cbReserved,
        /*     out */  VOID               **ppv,
        /*     out */  VOID               **ppvNI);

    static 
    HRESULT GetHash(
        IAssemblyName *pName, 
        DWORD dwFlags,
        DWORD dwHashSize,
        LPDWORD pdwHash);

    CAssemblyName();
    ~CAssemblyName();
    
    HRESULT GetVersion(DWORD dwMajorVersionEnumValue, 
                       LPDWORD pdwVersionHi,
                       LPDWORD pdwVersionLow);

    HRESULT GetFileVersion( LPDWORD pdwVersionHi, LPDWORD pdwVersionLow);

    HRESULT Init(LPCTSTR pszAssemblyName, ASSEMBLYMETADATA *pamd);

    HRESULT SetPathModifier(LPCWSTR pwzPathModifyer);
    HRESULT GetPathModifier(
            __out_ecount_opt(*pcchPathModifierSize) LPWSTR pwzPathModifier, 
            __inout LPDWORD pcchPathModifierSize);

    HRESULT Parse(LPCWSTR szDisplayName);
    HRESULT GetPublicKeyToken(LPDWORD cbBuf, LPBYTE pbBuf, BOOL fDisplay);

    BOOL IsLegacyAssembly();

    static HRESULT GetVersion(IAssemblyName *pName, BOOL fFailIfIncomplete, ULONGLONG *ullVer);
    static BOOL IsPartial(IAssemblyName *pName, LPDWORD pdwCmpMask = NULL);
    static BOOL IsCustom(IAssemblyName *pName);
    static BOOL IsStronglyNamed(IAssemblyName *pName);
    static BOOL IsSystem(IAssemblyName *pName);
    static HRESULT ParseCustomDisplayName(LPCWSTR pwzDispName, 
                                        IAssemblyName **ppName);
    static HRESULT GetCustomDisplayName(
                            IAssemblyName *pName, 
                            __out_ecount_opt(*pdwSize) LPWSTR pwzBuffer, 
                            __inout LPDWORD pdwSize, 
                            DWORD dwFlags);
    static HRESULT CloneForBind(IAssemblyName *pNameSource, 
                                IAssemblyName **ppNameClone);

 public:
     STDMETHODIMP ParseLegacy(LPCWSTR szDisplayName);


    STDMETHODIMP GetDisplayNameLegacy(
        __out_ecount_opt(*pccDisplayName) LPOLESTR  szDisplayName,
        __inout LPDWORD   pccDisplayName,
        DWORD     dwDisplayFlags);


    // Generate PublicKeyToken from public key blob.
    HRESULT GetPublicKeyTokenFromPKBlob(LPBYTE pbPublicKeyToken, 
                                        DWORD cbPublicKeyToken,
                                        LPBYTE *ppbSN, LPDWORD pcbSN);
   
    HRESULT DescribeBindInfo(CDebugLog *pdbglog, 
                            IApplicationContext *pAppCtx, 
                            LPCWSTR wzCodebase, 
                            LPCWSTR pwzCallingAsm);
    static HRESULT CopyProperties(CAssemblyName *pSource, 
                                CAssemblyName *pTarget, 
                                const DWORD properties[], 
                                DWORD dwSize);
};


// Binding methods
HRESULT DownloadAppCfg(IApplicationContext *pAppCtx, CAssemblyDownload *padl,
                       IAssemblyBindSink *pbindsink, CDebugLog *pdbglog, BOOL bAsyncAllowed);

HRESULT GetAssemblyRefOrDefIdentityFromFile(LPCWSTR pwzFilePath, REFIID riid, IUnknown **ppIdentity);
#endif  // NAMING_H_

