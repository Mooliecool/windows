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

// SSCOREE_SHIM_IS_PRIVATE is used by the LIBRARY.src file which
// generates the .def file. It tags which symbols are private. Since
// sscoree only has two private symbols, I didn't want to put an
// extra arg in each macro just for them.

#define SSCOREE_SHIM_IS_PRIVATE

// SSCOREE_SHIM_CUSTOM_INIT is used by the functions.cpp file which
// generates the actual forwarders. It allows custom initialization
// to be performed - currenty used by fusion. 

#define SSCOREE_SHIM_CUSTOM_INIT

// This file uses the macros SSCOREE_SHIM_RET and SSCOREE_SHIM_NORET
// to declare shim functions.
//
// SSCOREE_SHIM_RET takes these params in order:
//         - Return_Type    : the return type of the function
//         - Function_Name  : the name of the function
//         - Param_List     : the paramater list, including types, as it
//                            appears in the function prototype
//         - Arg_List       : the argument list to use for the call of the
//                            function in the target library.
//         - Failure_Return : what to return of the symbol can't be loaded
//
// SSCOREE_SHIM_NORET is the same as SSCOREE_SHIM_RET, except without the
//                    Return_Type or Failure_Return params.

// look in functions.cpp to see how they are used to generate functions.

/***************************************************************************/
/* Default definitions of these macros                                     */
/***************************************************************************/

#ifdef SSCOREE_SHIM_FUNC
#define SSCOREE_SHIM_NORET(FUNC,SIG_ARGS,ARGS)                      \
    SSCOREE_SHIM_FUNC(FUNC)
#define SSCOREE_SHIM_RET(SIG_RET,FUNC,SIG_ARGS,ARGS,ONERROR)        \
    SSCOREE_SHIM_FUNC(FUNC)
#endif

#ifndef SSCOREE_SHIM_NORET
#  define SSCOREE_SHIM_NORET(FUNC,SIG_ARGS,ARGS)
#endif

#ifndef SSCOREE_SHIM_RET
#  define SSCOREE_SHIM_RET(SIG_RET,FUNC,SIG_ARGS,ARGS,ONERROR) \
          SSCOREE_SHIM_NORET(FUNC,SIG_ARGS,ARGS)
#endif

#ifndef SSCOREE_LIB_START
#  define SSCOREE_LIB_START(LIB)
#endif

#ifndef SSCOREE_LIB_END
#  define SSCOREE_LIB_END(LIB)
#endif

#ifndef STDMANGLE
#  define STDMANGLE(name,args) name
#endif

/***************************************************************************/
/* from mscorwks.def                                                       */
/***************************************************************************/

SSCOREE_LIB_START (mscorwks)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(MetaDataGetDispenser,12),
                  ( REFCLSID    rclsid,
                    REFIID      riid,
                    LPVOID FAR  *ppv),
                  (rclsid,
                   riid,
                   ppv),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetMetaDataInternalInterface,20),
                  ( LPVOID      pData,
                    ULONG       cbData,
                    DWORD       flags,
                    REFIID      riid,
                    void        **ppv),
                  ( pData,
                    cbData,
                    flags,
                    riid,
                    ppv),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetMetaDataInternalInterfaceFromPublic,12),
                  ( void        *pv,
                    REFIID      riid,
                    void        **ppv),
                  ( pv,
                    riid,
                    ppv),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetMetaDataPublicInterfaceFromInternal,12),
                  ( void        *pv,
                    REFIID      riid,
                    void        **ppv),
                  ( pv,
                    riid,
                    ppv),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  __int32,
                  STDMANGLE(_CorExeMain2,20),
                  ( PBYTE   pUnmappedPE,
                    DWORD   cUnmappedPE,
                    LPWSTR  pImageNameIn,
                    LPWSTR  pLoadersFileName,
                    LPWSTR  pCmdLine),
                  ( pUnmappedPE,
                    cUnmappedPE,
                    pImageNameIn,
                    pLoadersFileName,
                    pCmdLine),
                  -1)

SSCOREE_SHIM_RET (
                   BOOL,
                   STDMANGLE(_CorDllMain,12),
                  ( HINSTANCE   hInst,
                    DWORD       dwReason,
                    LPVOID      lpReserved),
                  ( hInst,
                    dwReason,
                    lpReserved),
                  FALSE)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CoInitializeEE,4),
                  (DWORD fFlags),
                  (fFlags),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_NORET (
                    STDMANGLE(CoUninitializeEE,4),
                    (BOOL fFlags),
                    (fFlags))

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CoInitializeCor,4),
                  (DWORD fFlags),
                  (fFlags),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_NORET (
                    STDMANGLE(CoUninitializeCor,0),
                    (void),
                    ())


SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(LoadStringRC,16),
                  ( UINT iResourceID,
                    LPWSTR szBuffer,
                    int iMax,
                    int bQuiet),
                  ( iResourceID,
                    szBuffer,
                    iMax,
                    bQuiet),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(LoadStringRCEx,24),
                  ( LCID lcid,
                    UINT iResourceID,
                    LPWSTR szBuffer,
                    int iMax,
                    int bQuiet,
                    int *pcwchUsed),
                  ( lcid,
                    iResourceID,
                    szBuffer,
                    iMax,
                    bQuiet,
                    pcwchUsed),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(ReOpenMetaDataWithMemory,12),
                  ( void        *pUnk,
                    LPCVOID     pData,
                    ULONG       cbData),
                  ( pUnk,
                    pData,
                    cbData),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(ReOpenMetaDataWithMemoryEx,16),
                  ( void        *pUnk,
                    LPCVOID     pData,
                    ULONG       cbData,
                    DWORD       dwReOpenFlags),
                  ( pUnk,
                    pData,
                    cbData,
                    dwReOpenFlags),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(TranslateSecurityAttributes,24),
                  (void          *pPset,
                   void          *ppbOutput,
                   void          *pcbOutput,
                   void          *ppbNonCasOutput,
                   void          *pcbNonCasOutput,
                   void          *pdwErrorIndex),
                  (pPset,
                   ppbOutput,
                   pcbOutput,
                   ppbNonCasOutput,
                   pcbNonCasOutput,
                   pdwErrorIndex),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetPermissionRequests,28),
                  (LPCWSTR   pwszFileName,
                   BYTE    **ppbMinimal,
                   DWORD    *pcbMinimal,
                   BYTE    **ppbOptional,
                   DWORD    *pcbOptional,
                   BYTE    **ppbRefused,
                   DWORD    *pcbRefused),
                  (pwszFileName,
                   ppbMinimal,
                   pcbMinimal,
                   ppbOptional,
                   pcbOptional,
                   ppbRefused,
                   pcbRefused),
                  HRESULT_FROM_GetLastError())


SSCOREE_SHIM_NORET (
                    STDMANGLE(CorExitProcess,4),
                    (int exitCode),
                    (exitCode))

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(ClrCreateManagedInstance,12),
                  (LPCWSTR pTypeName,
                   REFIID riid,
                   void **ppObject),
                  (pTypeName,
                   riid,
                   ppObject),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_NORET (
                    STDMANGLE(CorMarkThreadInThreadPool,0),
                    (),
                    ())

SSCOREE_SHIM_NORET (
                    STDMANGLE(LogHelp_LogAssert,12),
                    (LPCSTR szFile,
                     int iLine,
                     LPCSTR expr),
                    (szFile,
                     iLine,
                     expr))

SSCOREE_SHIM_RET (
                  BOOL,
                  STDMANGLE(LogHelp_NoGuiOnAssert,0),
                  (),
                  (),
                  FALSE)

SSCOREE_SHIM_NORET (
                    STDMANGLE(LogHelp_TerminateOnAssert,0),
                    (),
                    ())

SSCOREE_SHIM_RET (
                  void *,
                  STDMANGLE(GetPrivateContextsPerfCounters,0),
                  (),
                  (),
                  NULL)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetAssemblyMDImport,12),
                  ( LPCWSTR     szFileName,
                    REFIID      riid,
                    void       *ppIUnk),
                  ( szFileName,
                    riid,
                    ppIUnk),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(InitializeFusion,0),
                  (),
                  (),
                  HRESULT_FROM_GetLastError())

#undef SSCOREE_SHIM_CUSTOM_INIT
#define SSCOREE_SHIM_CUSTOM_INIT \
    { HRESULT hrFusion = InitializeFusion(); if (FAILED(hrFusion)) return hrFusion; }

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetCachePath,12),
                  ( ASM_CACHE_FLAGS dwCacheFlags,
                    LPWSTR pwzCachePath,
                    PDWORD pcchPath),
                  ( dwCacheFlags,
                    pwzCachePath,
                    pcchPath),
                  HRESULT_FROM_GetLastError())


SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateAssemblyNameObject,16),
                  (
                    LPASSEMBLYNAME    *ppAssemblyName,
                    LPCOLESTR          szAssemblyName,
                    DWORD              dwFlags,
                    LPVOID             pvReserved),
                  (
                    ppAssemblyName,
                    szAssemblyName,
                    dwFlags,
                    pvReserved),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateApplicationContext,8),
                  (
                    IAssemblyName *pName,
                    LPAPPLICATIONCONTEXT *ppCtx),
                  (
                    pName,
                    ppCtx),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateAssemblyCache,8),
                  (
                    IAssemblyCache **ppAsmCache,
                    DWORD dwReserved
                  ),
                  (
                    ppAsmCache,
                    dwReserved),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateAssemblyEnum,20),
                  (
                    IAssemblyEnum** ppEnum,
                    IUnknown *pUnkAppCtx,
                    IAssemblyName *pName,
                    DWORD dwFlags,
                    LPVOID pvReserved),
                  (
                    ppEnum,
                    pUnkAppCtx,
                    pName,
                    dwFlags,
                    pvReserved),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateHistoryReader,8),
                  (
                    LPCWSTR wzFilePath,
                    IHistoryReader **ppHistoryReader),
                  (
                    wzFilePath,
                    ppHistoryReader),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(LookupHistoryAssembly,28),
                  (
                    LPCWSTR pwzFilePath,
                    FILETIME *pftActivationDate,
                    LPCWSTR pwzAsmName,
                    LPCWSTR pwzPublicKeyToken,
                    LPCWSTR pwzCulture,
                    LPCWSTR pwzVerRef,
                    IHistoryAssembly **ppHistAsm),
                  (
                    pwzFilePath,
                    pftActivationDate,
                    pwzAsmName,
                    pwzPublicKeyToken,
                    pwzCulture,
                    pwzVerRef,
                    ppHistAsm),
                    HRESULT_FROM_GetLastError())


SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetHistoryFileDirectory,8),
                  (
                    LPWSTR wzDir,
                    DWORD *pdwSize),
                  (
                    wzDir,
                    pdwSize),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(PreBindAssembly,20),
                  (
                    IApplicationContext *pAppCtx,
                    IAssemblyName *pName,
                    IAssembly *pAsmParent,
                    IAssemblyName **ppNamePostPolicy,
                    LPVOID pvReserved),
                  (
                    pAppCtx,
                    pName,
                    pAsmParent,
                    ppNamePostPolicy,
                    pvReserved),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(PreBindAssemblyEx,24),
                  (
                    IApplicationContext *pAppCtx,
                    IAssemblyName *pName,
                    IAssembly *pAsmParent,
                    LPCWSTR pwzRuntimeVersion,
                    IAssemblyName **ppNamePostPolicy,
                    LPVOID pvReserved),
                  (
                    pAppCtx,
                    pName,
                    pAsmParent,
                    pwzRuntimeVersion,
                    ppNamePostPolicy,
                    pvReserved),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(NukeDownloadedCache,0),
                  (),
                  (),
                  HRESULT_FROM_GetLastError())

#undef SSCOREE_SHIM_CUSTOM_INIT
#define SSCOREE_SHIM_CUSTOM_INIT

#undef SSCOREE_SHIM_IS_PRIVATE
#define SSCOREE_SHIM_IS_PRIVATE private

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CopyPDBs,4),
                  (
                    IAssembly *pAsm),
                  (
                    pAsm),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(DeleteShadowCache,8),
                  (
                    LPWSTR pwzCachePath,
                    LPWSTR pwzAppName),
                  (
                    pwzCachePath,
                    pwzAppName),
                  HRESULT_FROM_GetLastError())


SSCOREE_SHIM_RET (
                  void *,
                  STDMANGLE(IEE,0),
                  (),
                  (),
                  NULL)

#undef SSCOREE_SHIM_IS_PRIVATE
#define SSCOREE_SHIM_IS_PRIVATE

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetCORSystemDirectory,12),
                  (LPWSTR pbuffer,
                   DWORD  cchBuffer,
                   DWORD* dwlength),
                  (pbuffer,
                   cchBuffer,
                   dwlength),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetCORVersion,12),
                  (LPWSTR pbuffer,
                   DWORD cchBuffer,
                   DWORD* dwlength),
                  (pbuffer,
                   cchBuffer,
                   dwlength),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetCORRequiredVersion,12),
                  (LPWSTR pbuffer,
                   DWORD cchBuffer,
                   DWORD* dwlength),
                  (pbuffer,
                   cchBuffer,
                   dwlength),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(LoadLibraryShim,16),
                  (LPCWSTR szDllName,
                   LPCWSTR szVersion,
                   LPVOID pvReserved,
                   HMODULE *phModDll),
                  (szDllName,
                   szVersion,
                   pvReserved,
                   phModDll),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(GetXMLObject,4),
                  (void *ppv),
                  (ppv),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateConfigStream,8),
                  (LPCWSTR pszFileName,
                   void* ppStream),
                  (pszFileName,
                   ppStream),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  DWORD,
                  STDMANGLE(StrongNameErrorInfo,0),
                  (VOID),
                  (),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_NORET (
                    STDMANGLE(StrongNameFreeBuffer,4),
                    (BYTE *pbMemory),
                    (pbMemory))

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameKeyGen,16),
                  (LPCWSTR  wszKeyContainer,
                   DWORD    dwFlags,
                   BYTE   **ppbKeyBlob,
                   ULONG   *pcbKeyBlob),
                  (wszKeyContainer,
                   dwFlags,
                   ppbKeyBlob,
                   pcbKeyBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameKeyGenEx,20),
                  (LPCWSTR  wszKeyContainer,
                   DWORD    dwFlags,
                   DWORD    dwKeySize,
                   BYTE   **ppbKeyBlob,
                   ULONG   *pcbKeyBlob),
                  (wszKeyContainer,
                   dwFlags,
                   dwKeySize,
                   ppbKeyBlob,
                   pcbKeyBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameKeyInstall,12),
                  (LPCWSTR  wszKeyContainer,
                   BYTE    *pbKeyBlob,
                   ULONG    cbKeyBlob),
                  (wszKeyContainer,
                   pbKeyBlob,
                   cbKeyBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameKeyDelete,4),
                  (LPCWSTR wszKeyContainer),
                  (wszKeyContainer),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameGetPublicKey,20),
                  (LPCWSTR   wszKeyContainer,
                   BYTE     *pbKeyBlob,
                   ULONG     cbKeyBlob,
                   BYTE    **ppbPublicKeyBlob,
                   ULONG    *pcbPublicKeyBlob),
                  (wszKeyContainer,
                   pbKeyBlob,
                   cbKeyBlob,
                   ppbPublicKeyBlob,
                   pcbPublicKeyBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureGeneration,24),
                  (LPCWSTR     wszFilePath,
                   LPCWSTR     wszKeyContainer,
                   BYTE       *pbKeyBlob,
                   ULONG       cbKeyBlob,
                   BYTE      **ppbSignatureBlob,
                   ULONG      *pcbSignatureBlob),
                  (wszFilePath,
                   wszKeyContainer,
                   pbKeyBlob,
                   cbKeyBlob,
                   ppbSignatureBlob,
                   pcbSignatureBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureGenerationEx,28),
                  (LPCWSTR     wszFilePath,
                   LPCWSTR     wszKeyContainer,
                   BYTE       *pbKeyBlob,
                   ULONG       cbKeyBlob,
                   BYTE      **ppbSignatureBlob,
                   ULONG      *pcbSignatureBlob,
                   DWORD       dwFlags),
                  (wszFilePath,
                   wszKeyContainer,
                   pbKeyBlob,
                   cbKeyBlob,
                   ppbSignatureBlob,
                   pcbSignatureBlob,
                   dwFlags),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameTokenFromAssembly,12),
                  (LPCWSTR   wszFilePath,
                   BYTE    **ppbStrongNameToken,
                   ULONG    *pcbStrongNameToken),
                  (wszFilePath,
                   ppbStrongNameToken,
                   pcbStrongNameToken),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameTokenFromAssemblyEx,20),
                  (LPCWSTR   wszFilePath,
                   BYTE    **ppbStrongNameToken,
                   ULONG    *pcbStrongNameToken,
                   BYTE    **ppbPublicKeyBlob,
                   ULONG    *pcbPublicKeyBlob),
                  (wszFilePath,
                   ppbStrongNameToken,
                   pcbStrongNameToken,
                   ppbPublicKeyBlob,
                   pcbPublicKeyBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameTokenFromPublicKey,16),
                  (BYTE    *pbPublicKeyBlob,
                   ULONG    cbPublicKeyBlob,
                   BYTE   **ppbStrongNameToken,
                   ULONG   *pcbStrongNameToken),
                  (pbPublicKeyBlob,
                   cbPublicKeyBlob,
                   ppbStrongNameToken,
                   pcbStrongNameToken),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureVerification,12),
                  (LPCWSTR wszFilePath,
                   DWORD   dwInFlags,
                   DWORD  *pdwOutFlags),
                  (wszFilePath,
                   dwInFlags,
                   pdwOutFlags),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameCompareAssemblies,12),
                  (LPCWSTR   wszAssembly1,
                   LPCWSTR   wszAssembly2,
                   DWORD    *pdwResult),
                  (wszAssembly1,
                   wszAssembly2,
                   pdwResult),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameHashSize,8),
                  (ULONG  ulHashAlg,
                   DWORD *pcbSize),
                  (ulHashAlg,
                   pcbSize),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureSize,12),
                  (BYTE    *pbPublicKeyBlob,
                   ULONG    cbPublicKeyBlob,
                   DWORD   *pcbSize),
                  (pbPublicKeyBlob,
                   cbPublicKeyBlob,
                   pcbSize),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureVerificationEx,12),
                  (LPCWSTR     wszFilePath,
                   BOOLEAN     fForceVerification,
                   BOOLEAN    *pfWasVerified),
                  (wszFilePath,
                   fForceVerification,
                   pfWasVerified),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromAssemblyFile,20),
                  (LPCSTR szFilePath,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (szFilePath,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromAssemblyFileW,20),
                  (LPCWSTR wszFilePath,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (wszFilePath,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromBlob,24),
                  (BYTE   *pbBlob,
                   DWORD  cchBlob,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (pbBlob,
                   cchBlob,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromFile,20),
                  (LPCSTR szFilePath,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (szFilePath,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromFileW,20),
                  (LPCWSTR wszFilePath,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (wszFilePath,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI_(DWORD),
                  STDMANGLE(GetHashFromHandle,20),
                  (HANDLE hFile,
                   unsigned int *piHashAlg,
                   BYTE   *pbHash,
                   DWORD  cchHash,
                   DWORD  *pchHash),
                  (hFile,
                   piHashAlg,
                   pbHash,
                   cchHash,
                   pchHash),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameSignatureVerificationFromImage,16),
                  (BYTE     *pbBase,
                   DWORD     dwLength,
                   DWORD     dwInFlags,
                   DWORD    *pdwOutFlags),
                  (pbBase,
                   dwLength,
                   dwInFlags,
                   pdwOutFlags),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameGetBlob,12),
                  (LPCWSTR  wszFilePath,
                   PBYTE    pbBlob,
                   DWORD    *cbBlob),
                  (wszFilePath,
                   pbBlob,
                   cbBlob),
                  FALSE)

SSCOREE_SHIM_RET (
                  SNAPI,
                  STDMANGLE(StrongNameGetBlobFromImage,16),
                  (BYTE     *pbBase,
                   DWORD     dwLength,
                   PBYTE     pbBlob,
                   DWORD    *cbBlob),
                  (pbBase,
                   dwLength,
                   pbBlob,
                   cbBlob),
                  FALSE)

SSCOREE_LIB_END (mscorwks)

/***************************************************************************/
/* from mscorpe.def                                                        */
/***************************************************************************/

SSCOREE_LIB_START (mscorpe)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateICeeFileGen,4),
                  (void* pCeeFileGen),
                  (pCeeFileGen),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(DestroyICeeFileGen,4),
                  (void* pCeeFileGen),
                  (pCeeFileGen),
                  HRESULT_FROM_GetLastError())

SSCOREE_LIB_END (mscorpe)

/***************************************************************************/
/* from mscordbi.def                                                       */
/***************************************************************************/

SSCOREE_LIB_START (mscordbi)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateCordbObject,8),
                  (int iDebuggerVersion,
                   IUnknown ** ppCordb),
                  (iDebuggerVersion,
                   ppCordb),
                  HRESULT_FROM_GetLastError())

SSCOREE_LIB_END (mscordbi)

/***************************************************************************/
/* from alink.def                                                          */
/***************************************************************************/

SSCOREE_LIB_START (alink)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateALink,8),
                  (REFIID riid,
                  IUnknown** ppInterface),
                  (riid,
                  ppInterface),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HINSTANCE,
                  STDMANGLE(GetALinkMessageDll,0),
                  (),
                  (),
                  NULL)

SSCOREE_LIB_END (alink)

/***************************************************************************/
/* from csc.def                                                            */
/***************************************************************************/

SSCOREE_LIB_START (cscomp)

SSCOREE_SHIM_RET (
                  HINSTANCE,
                  STDMANGLE(GetMessageDll,0),
                  (),
                  (),
                  NULL)

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(CreateCompilerFactory,4),
                  (ICSCompilerFactory **ppFactory),
                  (ppFactory),
                  HRESULT_FROM_GetLastError())

SSCOREE_SHIM_RET (
                  HRESULT,
                  STDMANGLE(InMemoryCompile,8),
                  (LPCWSTR * pszText, 
                  const COMPILEPARAMS * params),
                  (pszText, 
                  params),
                  HRESULT_FROM_GetLastError())

SSCOREE_LIB_END (cscomp)

#ifdef SSCOREE_SHIM_FUNC
#  undef SSCOREE_SHIM_FUNC
#endif

#ifdef SSCOREE_SHIM_NORET
#  undef SSCOREE_SHIM_NORET
#endif

#ifdef SSCOREE_SHIM_RET
#  undef SSCOREE_SHIM_RET
#endif

#ifdef SSCOREE_LIB_START
#  undef SSCOREE_LIB_START
#endif

#ifdef SSCOREE_LIB_END
#  undef SSCOREE_LIB_END
#endif

#ifdef STDMANGLE
#  undef STDMANGLE
#endif
