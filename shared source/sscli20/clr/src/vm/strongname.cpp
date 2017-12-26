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

// ===========================================================================
// File: StrongName.cpp
// 
// Wrappers for signing and hashing functions needed to implement strong names
// ===========================================================================

#include "common.h"

#include <winwrap.h>
#include <windows.h>
#include <wincrypt.h>
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <cor.h>
#include <corimage.h>
#include <metadata.h>
#include <daccess.h>
#include <limits.h>
#include <ecmakey.h>
#include "eventtrace.h"


#include "safegetfilesize.h"

#define SNAPI_INTERNAL
#include "strongname.h"
#include "ex.h"

// The maximum length of CSP name we support (in characters).
#define SN_MAX_CSP_NAME 1024

#ifndef DACCESS_COMPILE


// We cache a couple of things on a per thread basis: the last error encountered
// and (potentially) a CSP context. The following structure tracks these and is
// allocated lazily as needed.
struct SN_THREAD_CTX {
    DWORD       m_dwLastError;
    HCRYPTPROV  m_hProv;
};

// Flag indicating whether the initialization of the strong name APIs has been completed.
BOOLEAN g_bStrongNamesInitialized = FALSE;

// Flag indicating whether it's OK to cache the results of verifying an assembly
// whose file is accessible to users.
BOOLEAN g_fCacheVerify = TRUE;

// Algorithm IDs for hashing and signing. Like the CSP name, these values are
// read from the registry at initialization time.
ALG_ID g_uHashAlgId;
ALG_ID g_uSignAlgId;

// Flag read from the registry at initialization time. It controls the key spec 
// to be used. AT_SIGNATURE will be the default.
DWORD g_uKeySpec;


// Verification Skip Records
//
// These are entries in the registry (usually set up by SN) that control whether
// an assembly needs to pass signature verification to be considered valid (i.e.
// return TRUE from StrongNameSignatureVerification). This is useful during
// development when it's not feasible to fully sign each assembly on each build.
// Assemblies to be skipped can be specified by name and public key token, all
// assemblies with a given public key token or just all assemblies. Each entry
// can be further qualified by a list of user names to which the records
// applies. When matching against an entry, the most specific one wins.
//
// We read these entries at startup time and place them into a global, singly
// linked, NULL terminated list.

// Structure used to represent each record we find in the registry.
struct SN_VER_REC {
    SN_VER_REC     *m_pNext;                    // Pointer to next record (or NULL)
    WCHAR          *m_wszAssembly;              // Assembly name/public key token as a string
    WCHAR          *m_mszUserList;              // Pointer to multi-string list of valid users (or NULL)
    WCHAR          *m_wszTestPublicKey;         // Test public key to use during strong name verification (or NULL)
};

// Head of the list of entries we found in the registry during initialization.
SN_VER_REC *g_pVerificationRecords = NULL;

#endif // #ifndef DACCESS_COMPILE

// We allow a special abbreviated form of the Microsoft public key (16 bytes
// long: 0 for both alg ids, 4 for key length and 4 bytes of 0 for the key
// itself). This allows us to build references to system libraries that are
// platform neutral (so a 3rd party can build mscorlib replacements). The
// special zero PK is just shorthand for the local runtime's real system PK,
// which is always used to perform the signature verification, so no security
// hole is opened by this. Therefore we need to store a copy of the real PK (for
// this platform) here.

// the actual definition of the microsoft key is in separate file to allow custom keys
#include "thekey.h"

#define SN_THE_KEY() ((PublicKeyBlob*)g_rbTheKey)
#define SN_SIZEOF_THE_KEY() sizeof(g_rbTheKey)

#define SN_THE_KEYTOKEN() ((PublicKeyBlob*)g_rbTheKeyToken)

// Determine if the given public key blob is the neutral key.
#define SN_IS_NEUTRAL_KEY(_pk) (SN_SIZEOF_KEY((PublicKeyBlob*)(_pk)) == sizeof(g_rbNeutralPublicKey) && \
                                memcmp((_pk), g_rbNeutralPublicKey, sizeof(g_rbNeutralPublicKey)) == 0)

// Debug logging.
#if !defined(_DEBUG) || defined(DACCESS_COMPILE)
#define SNLOG(args)
#define HexDump(x)
#define DbgCount(x)
#endif // _DEBUG

#ifndef DACCESS_COMPILE

// The subset of crypto APIs is implemented in the PAL - no special initialization necessary
#define SN_CryptAcquireContextA CryptAcquireContextA
#define SN_CryptReleaseContext CryptReleaseContext
#define SN_CryptCreateHash CryptCreateHash
#define SN_CryptDestroyHash CryptDestroyHash
#define SN_CryptHashData CryptHashData
#define SN_CryptGetHashParam CryptGetHashParam
#define SN_CryptImportKey CryptImportKey
#define SN_CryptExportKey CryptExportKey
#define SN_CryptGenKey CryptGenKey
#define SN_CryptGetKeyParam CryptGetKeyParam
#define SN_CryptDestroyKey CryptDestroyKey
#define SN_CryptVerifySignatureA CryptVerifySignatureA
#define SN_CryptSignHashA CryptSignHashA

// Debug logging.
#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
#include <stdarg.h>

BOOLEAN g_fLoggingInitialized = FALSE;
DWORD g_dwLoggingFlags = FALSE;

#define SNLOG(args)   Log args

void Log(__in_z CHAR *szFormat, ...)
{
    if (g_fLoggingInitialized && !g_dwLoggingFlags)
        return;

    DWORD       dwError = GetLastError();

    if (!g_fLoggingInitialized) {
        g_dwLoggingFlags = REGUTIL::GetConfigDWORD(L"MscorsnLogging", 0);
        g_fLoggingInitialized = TRUE;
    }

    if (!g_dwLoggingFlags) {
        SetLastError(dwError);
        return;
    }

    va_list     pArgs;
    CHAR        szBuffer[1024];
    static CHAR szPrefix[] = "SN: ";

    strcpy_s(szBuffer, COUNTOF(szBuffer), szPrefix);

    va_start(pArgs, szFormat);
    _vsnprintf_s(&szBuffer[sizeof(szPrefix) - 1], sizeof(szBuffer) - sizeof(szPrefix), _TRUNCATE, szFormat, pArgs);
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    va_end(pArgs);

    if (g_dwLoggingFlags & 1)
        printf("%s", szBuffer);
    if (g_dwLoggingFlags & 2)
        OutputDebugStringA(szBuffer);

    SetLastError(dwError);
}

void HexDump(BYTE  *pbData,
             DWORD  cbData)
{
    if (g_dwLoggingFlags == 0)
        return;

    DWORD dwRow, dwCol;
    char    szBuffer[4096];
    char   *szPtr = szBuffer;

#define SN_PUSH0(_fmt)          do { szPtr += sprintf_s(szPtr, COUNTOF(szBuffer) - (szPtr - szBuffer), _fmt); } while (false)
#define SN_PUSH1(_fmt, _arg1)   do { szPtr += sprintf_s(szPtr, COUNTOF(szBuffer) - (szPtr - szBuffer), _fmt, _arg1); } while (false)

    szBuffer[0] = '\0';

    for (dwRow = 0; dwRow < ((cbData + 15) / 16); dwRow++) {
        SN_PUSH1("%08X ", pbData + (16 * dwRow));
        for (dwCol = 0; dwCol < 16; dwCol++)
            if (((dwRow * 16) + dwCol) < cbData)
                SN_PUSH1("%02X ", pbData[(dwRow * 16) + dwCol]);
            else
                SN_PUSH0("   ");
        for (dwCol = 0; dwCol < 16; dwCol++)
            if (((dwRow * 16) + dwCol) < cbData) {
                unsigned char c = pbData[(dwRow * 16) + dwCol];
                if ((c >= 32) && (c <= 127))
                    SN_PUSH1("%c", c);
                else
                    SN_PUSH0(".");
            } else
                SN_PUSH0(" ");
        SN_PUSH0("\n");
    }
#undef SN_PUSH1
#undef SN_PUSH0

    _ASSERTE(szPtr < &szBuffer[sizeof(szBuffer)]);

    if (g_dwLoggingFlags & 1)
        printf("%s", szBuffer);
    if (g_dwLoggingFlags & 2)
        OutputDebugStringA(szBuffer);
}


#define DbgCount(x)

#endif // _DEBUG

// Size in bytes of strong name token.
#define SN_SIZEOF_TOKEN     8

// Context structure tracking information for a loaded assembly.
struct SN_LOAD_CTX {
    HANDLE              m_hFile;        // Open file handle
    HANDLE              m_hMap;         // Mapping file handle
    BYTE               *m_pbBase;       // Base address of mapped file
    DWORD               m_dwLength;     // Length of file in bytes
    IMAGE_NT_HEADERS   *m_pNtHeaders;   // Address of NT headers
    IMAGE_COR20_HEADER *m_pCorHeader;   // Address of COM+ 2.0 header
    BYTE               *m_pbSignature;  // Address of signature blob
    DWORD               m_cbSignature;  // Size of signature blob
    BOOLEAN             m_fReadOnly;    // File mapped for read-only access
    BOOLEAN             m_fPreMapped;   // File was already mapped for us
    SN_LOAD_CTX() { ZeroMemory(this, sizeof(*this)); }
};


// The actions that can be performed upon opening a CSP with LocateCSP.
#define SN_OPEN_CONTAINER   0
#define SN_IGNORE_CONTAINER 1
#define SN_CREATE_CONTAINER 2
#define SN_DELETE_CONTAINER 3
#define SN_HASH_SHA1_ONLY   4

// Macro to aid in setting flags for CryptAcquireContext based on container
// actions above.
#define SN_CAC_FLAGS(_act) 0

// Macro containing common code used at the start of most APIs.
#define SN_COMMON_PROLOG() do {                             \
    HRESULT __hr = InitStrongName();                        \
    if (FAILED(__hr)) {                                     \
        SetStrongNameErrorInfo(__hr);                       \
        retVal = FALSE;                                     \
        goto Exit;                                          \
    }                                                       \
    SetStrongNameErrorInfo(S_OK);                           \
} while (0)

// Determine the size of a PublicKeyBlob structure given the size of the key
// portion.
#define SN_SIZEOF_KEY(_pKeyBlob) (offsetof(PublicKeyBlob, PublicKey) + GET_UNALIGNED_VAL32(&(_pKeyBlob)->cbPublicKey))

// Private routine prototypes.
SN_THREAD_CTX *GetThreadContext();
VOID SetStrongNameErrorInfo(DWORD dwStatus);
HCRYPTPROV LocateCSP(LPCWSTR    wszKeyContainer,
                     DWORD      dwAction,
                     ALG_ID     uHashAlgId = 0,
                     ALG_ID     uSignAlgId = 0);
VOID FreeCSP(HCRYPTPROV hProv);
HCRYPTPROV LookupCachedCSP();
VOID CacheCSP(HCRYPTPROV hProv);
BOOLEAN IsCachedCSP(HCRYPTPROV hProv);
HRESULT ReadRegistryConfig();
HRESULT ReadVerificationRecords();
BOOLEAN IsValidUser(__in_z WCHAR *mszUserList);
SN_VER_REC *GetVerificationRecord(__in_z __deref LPWSTR wszAssemblyName, PublicKeyBlob *pPublicKey);
BOOLEAN GetKeyContainerName(LPCWSTR *pwszKeyContainer, BOOLEAN *pbTempContainer);
VOID FreeKeyContainerName(LPCWSTR wszKeyContainer, BOOLEAN bTempContainer);
IMDInternalImport *GetMetadataImport(SN_LOAD_CTX *pLoadCtx, mdAssembly *ptkAssembly);
PublicKeyBlob *FindPublicKey(SN_LOAD_CTX   *pLoadCtx,
                             __out_ecount_opt(cchAssemblyName) LPWSTR         wszAssemblyName,
                             DWORD          cchAssemblyName);
PublicKeyBlob *GetPublicKeyFromHex(LPCWSTR wszPublicKeyHexString);
BOOLEAN RehashModules(SN_LOAD_CTX *pLoadCtx, LPCWSTR szFilePath);
BOOLEAN LoadAssembly(SN_LOAD_CTX *pLoadCtx, LPCWSTR szFilePath, BOOLEAN fRequireSignature = TRUE);
BOOLEAN UnloadAssembly(SN_LOAD_CTX *pLoadCtx);
BOOLEAN VerifySignature(SN_LOAD_CTX *pLoadCtx, DWORD dwInFlags, DWORD *pdwOutFlags);
HRESULT InitStrongNameCriticalSection();
HRESULT InitStrongName();
typedef BOOLEAN (*HashFunc)(HCRYPTHASH hHash, PBYTE start, DWORD length, DWORD flags, void* cookie);
BOOLEAN ComputeHash(SN_LOAD_CTX *pLoadCtx, HCRYPTHASH hHash, HashFunc func, void* cookie);

BOOLEAN CalculateSize(HCRYPTHASH hHash, PBYTE start, DWORD length, DWORD flags, void* cookie)
{
    *(size_t*)cookie += length;
    return TRUE;
}

BOOLEAN CopyData(HCRYPTHASH hHash, PBYTE start, DWORD length, DWORD flags, void* cookie)
{
    PBYTE* pbData = (PBYTE*)cookie;
    memcpy(*pbData, start, length);
    *pbData += length;
    return TRUE;
}

BOOLEAN CalcHash(HCRYPTHASH hHash, PBYTE start, DWORD length, DWORD flags, void* cookie)
{
    return SN_CryptHashData(hHash, start, length, flags);
}

VOID
WINAPI Fls_Callback (
    IN PVOID lpFlsData
    )
{
    STATIC_CONTRACT_SO_TOLERANT;
    SN_THREAD_CTX *pThreadCtx = (SN_THREAD_CTX*)lpFlsData;
    if (pThreadCtx != NULL) {
        if (pThreadCtx->m_hProv)
            SN_CryptReleaseContext(pThreadCtx->m_hProv, 0);
        delete pThreadCtx;
    }
}

HRESULT InitStrongNameCriticalSection()
{
    return S_OK;
}

HRESULT InitStrongName()
{
    HRESULT hr = S_OK;
    if (g_bStrongNamesInitialized)
        return hr;

    // Initialize Wsz wrappers.
    OnUnicodeSystem();


    // Read CSP configuration info from the registry (if provided).
    hr = ReadRegistryConfig();
    if (FAILED(hr))
        return hr;

    // Associate a callback for freeing our TLS data.
    ClrFlsAssociateCallback(TlsIdx_StrongName, Fls_Callback);

    g_bStrongNamesInitialized = TRUE;

    return hr;
}

#endif // #ifndef DACCESS_COMPILE

// Return last error.
SNAPI_(DWORD) StrongNameErrorInfo(VOID)
{
    HRESULT hr = E_FAIL;
    
    BEGIN_ENTRYPOINT_NOTHROW;

#ifndef DACCESS_COMPILE
    SN_THREAD_CTX *pThreadCtx = GetThreadContext();
    if (pThreadCtx == NULL)
        hr = E_OUTOFMEMORY;
    else
        hr = pThreadCtx->m_dwLastError;
#else
    hr = E_FAIL;
#endif // #ifndef DACCESS_COMPILE
    END_ENTRYPOINT_NOTHROW;

    return hr;

}

// Free buffer allocated by routines below.
SNAPI_(VOID) StrongNameFreeBuffer(BYTE *pbMemory)            // [in] address of memory to free
{
    BEGIN_ENTRYPOINT_VOIDRET;

    SNLOG(("StrongNameFreeBuffer(%08X)\n", pbMemory));

    if (pbMemory != (BYTE*)SN_THE_KEY() && pbMemory != g_rbNeutralPublicKey)
        delete [] pbMemory;
    END_ENTRYPOINT_VOIDRET;

}

#ifndef DACCESS_COMPILE

// Generate a new key pair for strong name use.
SNAPI StrongNameKeyGen(LPCWSTR  wszKeyContainer,    // [in] desired key container name, must be a non-empty string
                       DWORD    dwFlags,            // [in] flags (see below)
                       BYTE   **ppbKeyBlob,         // [out] public/private key blob
                       ULONG   *pcbKeyBlob)
{

    BOOLEAN retVal = FALSE;
    BEGIN_ENTRYPOINT_VOIDRET;

    SN_COMMON_PROLOG();

    DWORD dwKeySize;

    // Calculate a key size mask. The key size in bits is encoded in the upper
    // 16-bits of a DWORD (to be OR'd together with other flags for the
    // CryptGenKey call). We set a key size of 1024 in we're using the default
    // signing algorithm (RSA), otherwise we leave it at the default.
    if (g_uSignAlgId == CALG_RSA_SIGN)
        dwKeySize = 1024;
    else
        dwKeySize = 0;

    retVal = StrongNameKeyGenEx(wszKeyContainer, dwFlags, dwKeySize, ppbKeyBlob, pcbKeyBlob);

Exit:
    END_ENTRYPOINT_VOIDRET;
    return retVal;
}

// Generate a new key pair with the specified key size for strong name use.
SNAPI StrongNameKeyGenEx(LPCWSTR  wszKeyContainer,    // [in] desired key container name, must be a non-empty string
                         DWORD    dwFlags,            // [in] flags (see below)
                         DWORD    dwKeySize,          // [in] desired key size.
                         BYTE   **ppbKeyBlob,         // [out] public/private key blob
                         ULONG   *pcbKeyBlob)
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    HCRYPTPROV  hProv = NULL;
    HCRYPTKEY   hKey = NULL;
    BOOLEAN     bTempContainer = FALSE;

    SNLOG(("StrongNameKeyGenEx(\"%S\", %08X, %08X, %08X, %08X)\n", wszKeyContainer, dwFlags, dwKeySize, ppbKeyBlob, pcbKeyBlob));

    SN_COMMON_PROLOG();

    // Check to see if a temporary container name is needed.
    _ASSERTE((wszKeyContainer != NULL) || !(dwFlags & SN_LEAVE_KEY));
    if (!GetKeyContainerName(&wszKeyContainer, &bTempContainer))
    {
        goto Exit;
    }

    // Open a CSP and container.
    hProv = LocateCSP(wszKeyContainer, SN_CREATE_CONTAINER);
    if (!hProv)
        goto Error;

    // Generate the new key pair, try for exportable first.
    if (!SN_CryptGenKey(hProv, g_uKeySpec, (dwKeySize << 16) | CRYPT_EXPORTABLE, &hKey)) {
        SNLOG(("Couldn't create exportable key, trying for non-exportable: %08X\n", GetLastError()));
        if (!SN_CryptGenKey(hProv, g_uKeySpec, dwKeySize << 16, &hKey)) {
            SNLOG(("Couldn't create key pair: %08X\n", GetLastError()));
            goto Error;
        }
    }


    // If the user wants the key pair back, attempt to export it.
    if (ppbKeyBlob) {

        // Calculate length of blob first;
        if (!SN_CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, NULL, pcbKeyBlob)) {
            SNLOG(("Couldn't export key pair: %08X\n", GetLastError()));
            goto Error;
        }

        // Allocate a buffer of the right size.
        *ppbKeyBlob = new (nothrow) BYTE[*pcbKeyBlob];
        if (*ppbKeyBlob == NULL) {
            SetLastError(E_OUTOFMEMORY);
            goto Error;
        }

        // Export the key pair.
        if (!SN_CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, *ppbKeyBlob, pcbKeyBlob)) {
            SNLOG(("Couldn't export key pair: %08X\n", GetLastError()));
            delete[] *ppbKeyBlob;
            *ppbKeyBlob = NULL;
            goto Error;
        }
    }

    // Destroy the key handle (but not the key pair itself).
    SN_CryptDestroyKey(hKey);
    hKey = NULL;

    // Release the CSP.
    FreeCSP(hProv);

    // If the user didn't explicitly want to keep the key pair around, delete the
    // key container.
    if (!(dwFlags & SN_LEAVE_KEY) || bTempContainer)
        LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);

    // Free temporary key container name if allocated.
    FreeKeyContainerName(wszKeyContainer, bTempContainer);
    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (hKey)
        SN_CryptDestroyKey(hKey);
    if (hProv) {
        FreeCSP(hProv);
        if (!(dwFlags & SN_LEAVE_KEY) || bTempContainer)
            LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);
    }
    FreeKeyContainerName(wszKeyContainer, bTempContainer);

 Exit:
    END_ENTRYPOINT_VOIDRET;
    return retVal;
}


// Import key pair into a key container.
SNAPI StrongNameKeyInstall(LPCWSTR  wszKeyContainer,// [in] desired key container name, must be a non-empty string
                           BYTE    *pbKeyBlob,      // [in] public/private key pair blob
                           ULONG    cbKeyBlob)
{
    BOOLEAN retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;
    _ASSERTE(false); // Sorry, no key containers support in PALRT
    SetStrongNameErrorInfo(CORSEC_E_CRYPTOAPI_CALL_FAILED);

    END_ENTRYPOINT_VOIDRET;
    return retVal;

}


// Delete a key pair.
SNAPI StrongNameKeyDelete(LPCWSTR wszKeyContainer)  // [in] desired key container name
{
    BOOLEAN retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    _ASSERTE(false); // Sorry, no key containers support in PALRT
    SetStrongNameErrorInfo(CORSEC_E_CRYPTOAPI_CALL_FAILED);

    END_ENTRYPOINT_VOIDRET;
    return retVal;

}


// Retrieve the public portion of a key pair.
SNAPI StrongNameGetPublicKey (LPCWSTR   wszKeyContainer,    // [in] desired key container name
                              BYTE     *pbKeyBlob,          // [in] public/private key blob (optional)
                              ULONG     cbKeyBlob,
                              BYTE    **ppbPublicKeyBlob,   // [out] public key blob
                              ULONG    *pcbPublicKeyBlob)
{
    BOOLEAN         retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    HCRYPTPROV      hProv = NULL;
    HCRYPTKEY       hKey = NULL;
    DWORD           dwKeyLen;
    PublicKeyBlob  *pKeyBlob;
    DWORD           dwSigAlgIdLen;
    BOOLEAN         bTempContainer = FALSE;

    SNLOG(("StrongNameGetPublicKey(\"%S\", %08X, %08X, %08X, %08X)\n", wszKeyContainer, pbKeyBlob, cbKeyBlob, ppbPublicKeyBlob, pcbPublicKeyBlob));

    SN_COMMON_PROLOG();

    // If we're handed a platform neutral public key, just hand it right back to
    // the user. Well, hand back a copy at least.
    if (pbKeyBlob && cbKeyBlob && SN_IS_NEUTRAL_KEY(pbKeyBlob)) {
        *pcbPublicKeyBlob = sizeof(g_rbNeutralPublicKey);
        *ppbPublicKeyBlob = (BYTE*)g_rbNeutralPublicKey;
        retVal = TRUE;
        goto Exit;
    }

    // Check to see if a temporary container name is needed.
    if (!GetKeyContainerName(&wszKeyContainer, &bTempContainer))
        goto Exit;

    // Open a CSP. Create a key container if a public/private key blob is
    // provided, otherwise we assume a key container already exists.
    if (pbKeyBlob)
        hProv = LocateCSP(wszKeyContainer, SN_CREATE_CONTAINER);
    else
        hProv = LocateCSP(wszKeyContainer, SN_OPEN_CONTAINER);
    if (!hProv)
        goto Error;

    // If a key blob was provided, import the key pair into the container.
    if (pbKeyBlob) {
        if (!SN_CryptImportKey(hProv,
                               pbKeyBlob,
                               cbKeyBlob,
                               0, 0, &hKey))
            goto Error;
    } else {
        SetLastError(E_NOTIMPL);
        goto Error;
    }

    // Determine the length of the public key part as a blob.
    if (!SN_CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, NULL, &dwKeyLen))
        goto Error;

    // And then the length of the PublicKeyBlob structure we return to the
    // caller.
    *pcbPublicKeyBlob = offsetof(PublicKeyBlob, PublicKey) + dwKeyLen;

    // Allocate a large enough buffer.
    *ppbPublicKeyBlob = new (nothrow) BYTE[*pcbPublicKeyBlob];
    if (*ppbPublicKeyBlob == NULL) {
        SetLastError(E_OUTOFMEMORY);
        goto Error;
    }

    pKeyBlob = (PublicKeyBlob*)*ppbPublicKeyBlob;

    // Extract the public part as a blob.
    if (!SN_CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, pKeyBlob->PublicKey, &dwKeyLen)) {
        delete[] *ppbPublicKeyBlob;
        *ppbPublicKeyBlob = NULL;
        goto Error;
    }

    // Extract key's signature algorithm and store it in the key blob.
    dwSigAlgIdLen = sizeof(unsigned int);
    ALG_ID SigAlgID;
    if (!SN_CryptGetKeyParam(hKey, KP_ALGID, (BYTE*)&SigAlgID, &dwSigAlgIdLen, 0)) {
        delete[] *ppbPublicKeyBlob;
        *ppbPublicKeyBlob = NULL;
        goto Error;
    }
    SET_UNALIGNED_VAL32(&pKeyBlob->SigAlgID, SigAlgID);

    // Fill in the other public key blob fields.
    SET_UNALIGNED_VAL32(&pKeyBlob->HashAlgID, g_uHashAlgId);
    SET_UNALIGNED_VAL32(&pKeyBlob->cbPublicKey, dwKeyLen);
    
    SN_CryptDestroyKey(hKey);
    FreeCSP(hProv);

    // If the caller provided a key blob, delete the temporary key container we
    // created.
    if (pbKeyBlob)
        LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);

    // Free temporary key container name if allocated.
    FreeKeyContainerName(wszKeyContainer, bTempContainer);
    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (hKey)
        SN_CryptDestroyKey(hKey);
    if (hProv) {
        FreeCSP(hProv);
        if (pbKeyBlob)
            LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);
    }
    FreeKeyContainerName(wszKeyContainer, bTempContainer);

Exit:

    END_ENTRYPOINT_VOIDRET;
    return retVal;
}

// Hash and sign a manifest.
SNAPI StrongNameSignatureGeneration(LPCWSTR     wszFilePath,        // [in] valid path to the PE file for the assembly
                                    LPCWSTR     wszKeyContainer,    // [in] desired key container name
                                    BYTE       *pbKeyBlob,          // [in] public/private key blob (optional)
                                    ULONG       cbKeyBlob,
                                    BYTE      **ppbSignatureBlob,   // [out] signature blob
                                    ULONG      *pcbSignatureBlob)
{
    BOOL fRetVal = FALSE;
    BEGIN_ENTRYPOINT_VOIDRET;
    fRetVal = StrongNameSignatureGenerationEx(wszFilePath, wszKeyContainer, pbKeyBlob, cbKeyBlob, ppbSignatureBlob, pcbSignatureBlob, 0);
    END_ENTRYPOINT_VOIDRET;
    return fRetVal;
}

SNAPI StrongNameSignatureGenerationEx(LPCWSTR     wszFilePath,        // [in] valid path to the PE file for the assembly
                                      LPCWSTR     wszKeyContainer,    // [in] desired key container name
                                      BYTE       *pbKeyBlob,          // [in] public/private key blob (optional)
                                      ULONG       cbKeyBlob,
                                      BYTE      **ppbSignatureBlob,   // [out] signature blob
                                      ULONG      *pcbSignatureBlob,
                                      DWORD       dwFlags)            // [in] modifer flags
{
    BOOLEAN         retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;
    HCRYPTPROV      hProv = NULL;
    HCRYPTHASH      hHash = NULL;
    HCRYPTKEY       hKey = NULL;
    BOOLEAN         bImageLoaded = FALSE;
    BYTE           *pbSig = NULL;
    ULONG           cbSig = 0;
    SN_LOAD_CTX     sLoadCtx;
    BOOLEAN         bTempContainer = FALSE;

    SNLOG(("StrongNameSignatureGenerationEx(\"%S\", \"%S\", %08X, %08X, %08X, %08X, %08X)\n", wszFilePath, wszKeyContainer, pbKeyBlob, cbKeyBlob, ppbSignatureBlob, pcbSignatureBlob, dwFlags));

    SN_COMMON_PROLOG();

    // Check to see if a temporary container name is needed.
    if (wszKeyContainer || pbKeyBlob) {
        if (!GetKeyContainerName(&wszKeyContainer, &bTempContainer))
        {
            goto Exit;
        }

        // Open a CSP. Create a key container if a public/private key blob is
        // provided, otherwise we assume a key container already exists.
        if (pbKeyBlob)
            hProv = LocateCSP(wszKeyContainer, SN_CREATE_CONTAINER);
        else
            hProv = LocateCSP(wszKeyContainer, SN_OPEN_CONTAINER);
        if (!hProv)
            goto Error;

        // If a key blob was provided, import the key pair into the container.
        if (pbKeyBlob) {
            if (!SN_CryptImportKey(hProv,
                                   pbKeyBlob,
                                   cbKeyBlob,
                                   0, 0, &hKey))
                goto Error;
            // We don't need to keep the key object open (the key is found
            // implicitly by the signing code).
            SN_CryptDestroyKey(hKey);
        }

        // Create a hash object.
        if (!SN_CryptCreateHash(hProv, g_uHashAlgId, 0, 0, &hHash))
            goto Error;

        // Compute size of the signature blob.
        if (!SN_CryptSignHashA(hHash, g_uKeySpec, NULL, 0, NULL, &cbSig))
            goto Error;

        // If the caller only wants the size of the signature, return it now and
        // exit.
        if (wszFilePath == NULL) {
            *pcbSignatureBlob = cbSig;
            SN_CryptDestroyHash(hHash);
            FreeCSP(hProv);
            if (pbKeyBlob)
                LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);
            FreeKeyContainerName(wszKeyContainer, bTempContainer);
            retVal = TRUE;
            goto Exit;
        }
    }

    // Map the assembly into memory.
    sLoadCtx.m_fReadOnly = FALSE;
    if (!LoadAssembly(&sLoadCtx, wszFilePath))
        goto Error;
    bImageLoaded = TRUE;

    // If we've asked to recalculate the file hashes of linked modules we have
    // to load the metadata engine and search for file references.
    if (dwFlags & SN_SIGN_ALL_FILES)
        if (!RehashModules(&sLoadCtx, wszFilePath))
            goto Error;

    // If no key pair is provided, then we were only called to re-compute the hashes of
    // linked modules in the assembly.
    if (!wszKeyContainer && !pbKeyBlob)
    {
        retVal = TRUE;
        goto Exit;
    }

    // We set a bit in the header to indicate we're fully signing the assembly.
    if (!(dwFlags & SN_TEST_SIGN))
        sLoadCtx.m_pCorHeader->Flags |= VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED);
    else
        sLoadCtx.m_pCorHeader->Flags &= ~VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED);

    // Destroy the old hash object and create a new one
    // because CryptoAPI says you can't reuse a hash once you've signed it
    // Note that this seems to work with MS-based CSPs but breaks on
    // at least newer nCipher CSPs.
    if (hHash)
        SN_CryptDestroyHash(hHash);
    hHash = NULL;
    if (!SN_CryptCreateHash(hProv, g_uHashAlgId, 0, 0, &hHash))
        goto Error;

    // Compute a hash over the image.
    if (!ComputeHash(&sLoadCtx, hHash, CalcHash, NULL))
        goto Error;

    // Allocate the blob.
    pbSig = new (nothrow) BYTE[cbSig];
    if (pbSig == NULL) {
        SetLastError(E_OUTOFMEMORY);
        goto Error;
    }

    // Compute a signature blob over the hash of the manifest.
    if (!SN_CryptSignHashA(hHash, g_uKeySpec, NULL, 0, pbSig, &cbSig))
        goto Error;

    // Check the signature size
    if (sLoadCtx.m_cbSignature != cbSig) {
        SetLastError(CORSEC_E_SIGNATURE_MISMATCH);
        goto Error;
    }

    // Write the signature into file or return it to the user so they can do it.
    if (!ppbSignatureBlob)
        memcpy(sLoadCtx.m_pbSignature, pbSig, cbSig);

    // Unmap the image (automatically recalculates and updates the image
    // checksum).
    bImageLoaded = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    SN_CryptDestroyHash(hHash);
    FreeCSP(hProv);

    // If a temporary key container was created, delete it now.
    if (pbKeyBlob)
        LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);

    // Free temporary key container name if allocated.
    FreeKeyContainerName(wszKeyContainer, bTempContainer);

    if (ppbSignatureBlob) {
        *ppbSignatureBlob = pbSig;
        *pcbSignatureBlob = cbSig;
    }
    else
        delete[] pbSig;

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (pbSig)
        delete[] pbSig;
    if (bImageLoaded)
        UnloadAssembly(&sLoadCtx);
    if (hHash)
        SN_CryptDestroyHash(hHash);
    if (hProv) {
        FreeCSP(hProv);
        if (pbKeyBlob)
            LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);
    }
    FreeKeyContainerName(wszKeyContainer, bTempContainer);
Exit:
    END_ENTRYPOINT_VOIDRET;
    return retVal;
}

// Create a strong name token from an assembly file.
SNAPI StrongNameTokenFromAssembly(LPCWSTR   wszFilePath,            // [in] valid path to the PE file for the assembly
                                  BYTE    **ppbStrongNameToken,     // [out] strong name token 
                                  ULONG    *pcbStrongNameToken)
{
    BOOL fRetValue = FALSE;
    BEGIN_ENTRYPOINT_VOIDRET;
    fRetValue = StrongNameTokenFromAssemblyEx(wszFilePath,
                                        ppbStrongNameToken,
                                         pcbStrongNameToken,
                                         NULL,
                                         NULL);
    END_ENTRYPOINT_VOIDRET;
    return fRetValue;
}

// Create a strong name token from an assembly file and additionally return the full public key.
SNAPI StrongNameTokenFromAssemblyEx(LPCWSTR   wszFilePath,            // [in] valid path to the PE file for the assembly
                                    BYTE    **ppbStrongNameToken,     // [out] strong name token 
                                    ULONG    *pcbStrongNameToken,
                                    BYTE    **ppbPublicKeyBlob,       // [out] public key blob
                                    ULONG    *pcbPublicKeyBlob)
{
    BOOLEAN         retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;
    SN_LOAD_CTX     sLoadCtx;
    BOOLEAN         fMapped = FALSE;
    BOOLEAN         fSetErrorInfo = TRUE;
    PublicKeyBlob  *pPublicKey = NULL;

    SNLOG(("StrongNameTokenFromAssemblyEx(\"%S\", %08X, %08X, %08X, %08X)\n", wszFilePath, ppbStrongNameToken, pcbStrongNameToken, ppbPublicKeyBlob, pcbPublicKeyBlob));

    SN_COMMON_PROLOG();

    // Map the assembly into memory.
    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, wszFilePath))
        goto Error;
    fMapped = TRUE;

    // Read the public key used to sign the assembly from the assembly metadata.
    pPublicKey = FindPublicKey(&sLoadCtx, NULL, 0);
    if (pPublicKey == NULL)
        goto Error;

    // Unload the assembly.
    fMapped = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    // Now we have a public key blob, we can call our more direct API to do the
    // actual work.
    if (!StrongNameTokenFromPublicKey((BYTE*)pPublicKey,
                                      SN_SIZEOF_KEY(pPublicKey),
                                      ppbStrongNameToken,
                                      pcbStrongNameToken)) {
        fSetErrorInfo = FALSE;
        goto Error;
    }

    if (pcbPublicKeyBlob)
        *pcbPublicKeyBlob = SN_SIZEOF_KEY(pPublicKey);
 
    // Return public key information.
    if (ppbPublicKeyBlob)
        *ppbPublicKeyBlob = (BYTE*)pPublicKey;
    else
        delete [] (BYTE*)pPublicKey;

    retVal = TRUE;
    goto Exit;

 Error:
    if (fSetErrorInfo)
        SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (pPublicKey)
        delete [] (BYTE*)pPublicKey;
    if (fMapped)
        UnloadAssembly(&sLoadCtx);

Exit:
    END_ENTRYPOINT_VOIDRET;
        
    return retVal;
}

#endif // #ifndef DACCESS_COMPILE

// Create a strong name token from a public key blob.
SNAPI StrongNameTokenFromPublicKey(BYTE    *pbPublicKeyBlob,        // [in] public key blob
                                   ULONG    cbPublicKeyBlob,
                                   BYTE   **ppbStrongNameToken,     // [out] strong name token 
                                   ULONG   *pcbStrongNameToken)
{
    BOOLEAN         retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

#ifndef DACCESS_COMPILE
    HCRYPTPROV      hProv = NULL;
    HCRYPTHASH      hHash = NULL;
    HCRYPTKEY       hKey  = NULL;
    DWORD           dwHashLen;
    DWORD           dwRetLen;
    BYTE            *pHash;
    DWORD           i;
    DWORD           cbKeyBlob;
    PublicKeyBlob   *pPublicKey = NULL;

    SNLOG(("StrongNameTokenFromPublicKey(%08X, %08X, %08X, %08X)\n", pbPublicKeyBlob, cbPublicKeyBlob, ppbStrongNameToken, pcbStrongNameToken));

    ETWTraceStartup trace(ETW_TYPE_STARTUP_SECURITYCATCHALL);

    SN_COMMON_PROLOG();

    // Allocate a buffer for the output token.
    *ppbStrongNameToken = new (nothrow) BYTE[SN_SIZEOF_TOKEN];
    if (*ppbStrongNameToken == NULL) {
        SetStrongNameErrorInfo(E_OUTOFMEMORY);
        goto Exit;
    }
    *pcbStrongNameToken = SN_SIZEOF_TOKEN;

    // We cache a couple of common cases.
    if (SN_IS_NEUTRAL_KEY(pbPublicKeyBlob)) {
        memcpy(*ppbStrongNameToken, g_rbNeutralPublicKeyToken, SN_SIZEOF_TOKEN);
        retVal = TRUE;
        goto Exit;
    }
    if (cbPublicKeyBlob == SN_SIZEOF_THE_KEY() &&
        memcmp(pbPublicKeyBlob, SN_THE_KEY(), cbPublicKeyBlob) == 0) {
        memcpy(*ppbStrongNameToken, SN_THE_KEYTOKEN(), SN_SIZEOF_TOKEN);
        retVal = TRUE;
        goto Exit;
    }

    // Look for a CSP to hash the public key.
    hProv = LocateCSP(NULL, SN_HASH_SHA1_ONLY);
    if (!hProv)
        goto Error;

    // To compute the correct public key token, we need to make sure the public key blob
    // was not padded with extra bytes that CAPI CryptImportKey would've ignored.
    // Without this round trip, we would blindly compute the hash over the padded bytes
    // which could make finding a public key token collision a significantly easier task
    // since an attacker wouldn't need to work hard on generating valid key pairs before hashing.
    if (cbPublicKeyBlob <= sizeof(PublicKeyBlob)) {
        SetLastError(CORSEC_E_INVALID_PUBLICKEY);
        goto Error;
    }

    // Check that the blob type is PUBLICKEYBLOB.
    pPublicKey = (PublicKeyBlob*) pbPublicKeyBlob;
    if (*(BYTE*) pPublicKey->PublicKey /* PUBLICKEYSTRUC->bType */ != PUBLICKEYBLOB) {
        SetLastError(CORSEC_E_INVALID_PUBLICKEY);
        goto Error;
    }

    if (!SN_CryptImportKey(hProv, 
                           pPublicKey->PublicKey, 
                           GET_UNALIGNED_VAL32(&pPublicKey->cbPublicKey),
                           0, 
                           0, 
                           &hKey))
        goto Error;

    cbKeyBlob = sizeof(DWORD);
    if (!SN_CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, NULL, &cbKeyBlob))
        goto Error;

    if ((offsetof(PublicKeyBlob, PublicKey) + cbKeyBlob) != cbPublicKeyBlob) {
        SetLastError(CORSEC_E_INVALID_PUBLICKEY);
        goto Error;
    }

    // Create a hash object.
    if (!SN_CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
        goto Error;

    // Compute a hash over the public key.
    if (!SN_CryptHashData(hHash, pbPublicKeyBlob, cbPublicKeyBlob, 0))
        goto Error;

    // Get the length of the hash.
    dwRetLen = sizeof(dwHashLen);
    if (!SN_CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashLen, &dwRetLen, 0))
        goto Error;

    // Allocate a temporary block to hold the hash.
    pHash = (BYTE*)_alloca(dwHashLen);

    // Read the hash value.
    if (!SN_CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwHashLen, 0))
        goto Error;

    // We no longer need the hash object or the provider.
    SN_CryptDestroyHash(hHash);
    SN_CryptDestroyKey(hKey);
    FreeCSP(hProv);

    // Take the last few bytes of the hash value for our token. (These are the
    // low order bytes from a network byte order point of view). Reverse the
    // order of these bytes in the output buffer to get host byte order.
    _ASSERTE(dwHashLen >= SN_SIZEOF_TOKEN);
    for (i = 0; i < SN_SIZEOF_TOKEN; i++)
        (*ppbStrongNameToken)[SN_SIZEOF_TOKEN - (i + 1)] = pHash[i + (dwHashLen - SN_SIZEOF_TOKEN)];

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (hHash)
        SN_CryptDestroyHash(hHash);
    if (hKey)
        SN_CryptDestroyKey(hKey);
    if (hProv)
        FreeCSP(hProv);
    if (*ppbStrongNameToken) {
        delete [] *ppbStrongNameToken;
        *ppbStrongNameToken = NULL;
    }
Exit:
#else
    DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
    END_ENTRYPOINT_VOIDRET;

    return retVal;

}

#ifndef DACCESS_COMPILE

// Verify a strong name/manifest against a public key blob.
SNAPI StrongNameSignatureVerificationEx(LPCWSTR     wszFilePath,        // [in] valid path to the PE file for the assembly
                                        BOOLEAN     fForceVerification, // [in] verify even if settings in the registry disable it
                                        BOOLEAN    *pfWasVerified)      // [out] set to false if verify succeeded due to registry settings
{
    BOOL bRet = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;
    DWORD dwOutFlags;
    bRet = StrongNameSignatureVerification(wszFilePath,
                                           SN_INFLAG_INSTALL|SN_INFLAG_ALL_ACCESS|(fForceVerification ? SN_INFLAG_FORCE_VER : 0),
                                           &dwOutFlags);
    if (bRet && pfWasVerified)
        *pfWasVerified = (dwOutFlags & SN_OUTFLAG_WAS_VERIFIED) != 0;
    END_ENTRYPOINT_VOIDRET;
    return bRet;
}


// Verify a strong name/manifest against a public key blob.
SNAPI StrongNameSignatureVerification(LPCWSTR wszFilePath,      // [in] valid path to the PE file for the assembly
                                      DWORD   dwInFlags,        // [in] flags modifying behaviour
                                      DWORD  *pdwOutFlags)      // [out] additional output info
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    SN_LOAD_CTX sLoadCtx;
    BOOLEAN     fMapped = FALSE;

    SNLOG(("StrongNameSignatureVerification(\"%S\", %08X, %08X, %08X)\n", wszFilePath, dwInFlags, pdwOutFlags));

    // Map the assembly into memory.
    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, wszFilePath))
        goto Error;
    fMapped = TRUE;

    SN_COMMON_PROLOG();

    // Go to common code to process the verification.
    if (!VerifySignature(&sLoadCtx, dwInFlags, pdwOutFlags))
        goto Error;

    // Unmap the image.
    fMapped = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (fMapped)
        UnloadAssembly(&sLoadCtx);
Exit:

    END_ENTRYPOINT_VOIDRET;
    return retVal;
}


// Verify a strong name/manifest against a public key blob when the assembly is
// already memory mapped.
SNAPI StrongNameSignatureVerificationFromImage(BYTE     *pbBase,             // [in] base address of mapped manifest file
                                               DWORD     dwLength,           // [in] length of mapped image in bytes
                                               DWORD     dwInFlags,          // [in] flags modifying behaviour
                                               DWORD    *pdwOutFlags)        // [out] additional output info
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET

    SN_LOAD_CTX sLoadCtx;
    BOOLEAN     fMapped = FALSE;

    SNLOG(("StrongNameSignatureVerificationFromImage(%08X, %08X, %08X, %08X)\n", pbBase, dwLength, dwInFlags, pdwOutFlags));

    // We don't need to map the image, it's already in memory. But we do need to
    // set up a load context for some of the following routines. LoadAssembly
    // copes with this case for us.
    sLoadCtx.m_pbBase = pbBase;
    sLoadCtx.m_dwLength = dwLength;
    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, NULL))
        goto Error;
    fMapped = TRUE;

    SN_COMMON_PROLOG();

    // Go to common code to process the verification.
    if (!VerifySignature(&sLoadCtx, dwInFlags, pdwOutFlags))
        goto Error;

    // Unmap the image.
    fMapped = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (fMapped)
        UnloadAssembly(&sLoadCtx);
Exit:

    END_ENTRYPOINT_VOIDRET;
    return retVal;
}

// The common code used to verify a signature (taking into account whether skip
// verification is enabled for the given assembly).
BOOLEAN CollectBlob(SN_LOAD_CTX *pLoadCtx, PBYTE pbBlob, DWORD* pcbBlob)
{
    // Calculate the required size
    DWORD cbRequired = 0;
    BOOLEAN bRetval = ComputeHash(pLoadCtx, (HCRYPTHASH)INVALID_HANDLE_VALUE, CalculateSize, &cbRequired);
    if (!bRetval)
        return FALSE;
    if (*pcbBlob < cbRequired) {
        *pcbBlob = cbRequired;
        SetLastError( E_INVALIDARG );
        return FALSE;
    }
    PBYTE pbBlobCopy = pbBlob;
    if (!ComputeHash(pLoadCtx, (HCRYPTHASH)INVALID_HANDLE_VALUE, CopyData, &pbBlobCopy))
        return FALSE;
    
    *pcbBlob = cbRequired;
    return TRUE;
}

// ensure that the symbol will be exported properly
extern "C" SNAPI StrongNameGetBlob(LPCWSTR wszFilePath,
                        PBYTE     pbBlob,
                        DWORD    *cbBlob);

// Verify a strong name/manifest against a public key blob when the assembly is
// already memory mapped.
SNAPI StrongNameGetBlob(LPCWSTR wszFilePath,      // [in] valid path to the PE file for the assembly
                        PBYTE     pbBlob,         // [in] buffer to fill with blob
                        DWORD    *cbBlob)         // [in/out] size of buffer/number of bytes put into buffer
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    SN_LOAD_CTX sLoadCtx;
    BOOLEAN     fMapped = FALSE;

    SNLOG(("StrongNameGetBlob(\"%S\", %08X, %08X)\n", wszFilePath, pbBlob, cbBlob));

    SN_COMMON_PROLOG();

    // Map the assembly into memory.
    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, wszFilePath, FALSE))
        goto Error;
    fMapped = TRUE;

    // Go to common code to process the verification.
    if (!CollectBlob(&sLoadCtx, pbBlob, cbBlob))
        goto Error;

    // Unmap the image.
    fMapped = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (fMapped)
        UnloadAssembly(&sLoadCtx);
Exit:
    END_ENTRYPOINT_VOIDRET;
    return retVal;
}

// ensure that the symbol will be exported properly
extern "C" SNAPI StrongNameGetBlobFromImage(BYTE     *pbBase,
                                 DWORD     dwLength,
                                 PBYTE     pbBlob,
                                 DWORD    *cbBlob);

SNAPI StrongNameGetBlobFromImage(BYTE     *pbBase,             // [in] base address of mapped manifest file
                                 DWORD     dwLength,           // [in] length of mapped image in bytes
                                 PBYTE     pbBlob,             // [in] buffer to fill with blob
                                 DWORD    *cbBlob)             // [in/out] size of buffer/number of bytes put into buffer
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    SN_LOAD_CTX sLoadCtx;
    BOOLEAN     fMapped = FALSE;


    SNLOG(("StrongNameGetBlobFromImage(%08X, %08X, %08X, %08X)\n", pbBase, dwLength, pbBlob, cbBlob));

    SN_COMMON_PROLOG();

    // We don't need to map the image, it's already in memory. But we do need to
    // set up a load context for some of the following routines. LoadAssembly
    // copes with this case for us.
    sLoadCtx.m_pbBase = pbBase;
    sLoadCtx.m_dwLength = dwLength;
    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, NULL, FALSE))
        goto Error;
    fMapped = TRUE;

    // Go to common code to process the verification.
    if (!CollectBlob(&sLoadCtx, pbBlob, cbBlob))
        goto Error;

    // Unmap the image.
    fMapped = FALSE;
    if (!UnloadAssembly(&sLoadCtx))
        goto Error;

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (fMapped)
        UnloadAssembly(&sLoadCtx);

Exit:
    END_ENTRYPOINT_VOIDRET;

    return retVal;
}


// Verify that two assemblies differ only by signature blob.
SNAPI StrongNameCompareAssemblies(LPCWSTR   wszAssembly1,           // [in] file name of first assembly
                                  LPCWSTR   wszAssembly2,           // [in] file name of second assembly
                                  DWORD    *pdwResult)              // [out] result of comparison
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;


    SN_LOAD_CTX sLoadCtx1;
    SN_LOAD_CTX sLoadCtx2;
    size_t      dwSkipOffsets[3];
    size_t      dwSkipLengths[3];
    BOOLEAN     bMappedAssem1 = FALSE;
    BOOLEAN     bMappedAssem2 = FALSE;
    BOOLEAN     bIdentical;
    BOOLEAN     bSkipping;
    DWORD       i, j;



    SNLOG(("StrongNameCompareAssemblies(\"%S\", \"%S\", %08X)\n", wszAssembly1, wszAssembly2, pdwResult));

    SN_COMMON_PROLOG();

    // Map each assembly.
    sLoadCtx1.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx1, wszAssembly1))
        goto Error;
    bMappedAssem1 = TRUE;

    sLoadCtx2.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx2, wszAssembly2))
        goto Error;
    bMappedAssem2 = TRUE;

    // If the files aren't even the same length then they must be different.
    if (sLoadCtx1.m_dwLength != sLoadCtx2.m_dwLength)
        goto ImagesDiffer;

    // Check that the signatures are located at the same offset and are the same
    // length in each assembly.
    if (sLoadCtx1.m_pCorHeader->StrongNameSignature.VirtualAddress !=
        sLoadCtx2.m_pCorHeader->StrongNameSignature.VirtualAddress)
        goto ImagesDiffer;
    if (sLoadCtx1.m_pCorHeader->StrongNameSignature.Size !=
        sLoadCtx2.m_pCorHeader->StrongNameSignature.Size)
        goto ImagesDiffer;

    // Set up list of image ranges to skip in the upcoming comparison.
    // First there's the signature blob.
    dwSkipOffsets[0] = sLoadCtx1.m_pbSignature - sLoadCtx1.m_pbBase;
    dwSkipLengths[0] = sLoadCtx1.m_cbSignature;

    // Then there's the checksum.
    if (sLoadCtx1.m_pNtHeaders->OptionalHeader.Magic != sLoadCtx2.m_pNtHeaders->OptionalHeader.Magic)
        goto ImagesDiffer;
    if (sLoadCtx1.m_pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
        dwSkipOffsets[1] = (BYTE*)&((IMAGE_NT_HEADERS32*)sLoadCtx1.m_pNtHeaders)->OptionalHeader.CheckSum - sLoadCtx1.m_pbBase;
    else if (sLoadCtx1.m_pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
        dwSkipOffsets[1] = (BYTE*)&((IMAGE_NT_HEADERS64*)sLoadCtx1.m_pNtHeaders)->OptionalHeader.CheckSum - sLoadCtx1.m_pbBase;
    else {
        SetLastError(CORSEC_E_INVALID_IMAGE_FORMAT);
        goto Error;
    }
    dwSkipLengths[1] = sizeof(DWORD);

    // Skip the COM+ 2.0 PE header extension flags field. It's updated by the
    // signing operation.
    dwSkipOffsets[2] = (BYTE*)&sLoadCtx1.m_pCorHeader->Flags - sLoadCtx1.m_pbBase;
    dwSkipLengths[2] = sizeof(DWORD);

    // Compare the two mapped images, skipping the ranges we defined above.
    bIdentical = TRUE;
    for (i = 0; i < sLoadCtx1.m_dwLength; i++) {

        // Determine if we're skipping the check on the current byte.
        bSkipping = FALSE;
        for (j = 0; j < (sizeof(dwSkipOffsets) / sizeof(dwSkipOffsets[0])); j++)
            if ((i >= dwSkipOffsets[j]) && (i < (dwSkipOffsets[j] + dwSkipLengths[j]))) {
                bSkipping = TRUE;
                break;
            }

        // Perform comparisons as desired.
        if (sLoadCtx1.m_pbBase[i] != sLoadCtx2.m_pbBase[i])
            if (bSkipping)
                bIdentical = FALSE;
            else
                goto ImagesDiffer;
    }

    // The assemblies are the same.
    *pdwResult = bIdentical ? SN_CMP_IDENTICAL : SN_CMP_SIGONLY;

    UnloadAssembly(&sLoadCtx1);
    UnloadAssembly(&sLoadCtx2);

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (bMappedAssem1)
        UnloadAssembly(&sLoadCtx1);
    if (bMappedAssem2)
        UnloadAssembly(&sLoadCtx2);
    goto Exit;

 ImagesDiffer:
    if (bMappedAssem1)
        UnloadAssembly(&sLoadCtx1);
    if (bMappedAssem2)
        UnloadAssembly(&sLoadCtx2);
    *pdwResult = SN_CMP_DIFFERENT;
    retVal = TRUE;

Exit:
    END_ENTRYPOINT_VOIDRET;

    return retVal;
}


// Compute the size of buffer needed to hold a hash for a given hash algorithm.
SNAPI StrongNameHashSize(ULONG  ulHashAlg,  // [in] hash algorithm
                         DWORD *pcbSize)    // [out] size of the hash in bytes
{
    BOOLEAN     retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    HCRYPTPROV  hProv = NULL;
    HCRYPTHASH  hHash = NULL;
    DWORD       dwSize;


    SNLOG(("StrongNameHashSize(%08X, %08X)\n", ulHashAlg, pcbSize));

    SN_COMMON_PROLOG();

    // Default hashing algorithm ID if necessary.
    if (ulHashAlg == 0)
        ulHashAlg = CALG_SHA1;

    // Find a CSP supporting the required algorithm.
    hProv = LocateCSP(NULL, SN_IGNORE_CONTAINER, ulHashAlg);
    if (!hProv)
        goto Error;

    // Create a hash object.
    if (!SN_CryptCreateHash(hProv, ulHashAlg, 0, 0, &hHash))
        goto Error;

    // And ask for the size of the hash.
    dwSize = sizeof(DWORD);
    if (!SN_CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)pcbSize, &dwSize, 0))
        goto Error;

    // Cleanup and exit.
    SN_CryptDestroyHash(hHash);
    FreeCSP(hProv);

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (hHash)
        SN_CryptDestroyHash(hHash);
    if (hProv)
        FreeCSP(hProv);

 Exit:

    END_ENTRYPOINT_VOIDRET;

    return retVal;
}


// Compute the size that needs to be allocated for a signature in an assembly.
SNAPI StrongNameSignatureSize(BYTE    *pbPublicKeyBlob,    // [in] public key blob
                              ULONG    cbPublicKeyBlob,
                              DWORD   *pcbSize)            // [out] size of the signature in bytes
{
    BOOLEAN         retVal = FALSE;

    BEGIN_ENTRYPOINT_VOIDRET;

    PublicKeyBlob  *pPublicKey = (PublicKeyBlob*)pbPublicKeyBlob;
    ALG_ID          uHashAlgId;
    ALG_ID          uSignAlgId;
    HCRYPTPROV      hProv = NULL;
    HCRYPTHASH      hHash = NULL;
    HCRYPTKEY       hKey = NULL;
    LPCWSTR         wszKeyContainer = NULL;
    BOOLEAN         bTempContainer = FALSE;
    DWORD           dwKeyLen;
    DWORD           dwBytes;

    SNLOG(("StrongNameSignatureSize(%08X, %08X, %08X)\n", pbPublicKeyBlob, cbPublicKeyBlob, pcbSize));

    SN_COMMON_PROLOG();

    // Special case neutral key.
    if (SN_IS_NEUTRAL_KEY(pPublicKey))
        pPublicKey = SN_THE_KEY();

    // Determine hashing/signing algorithms.
    uHashAlgId = GET_UNALIGNED_VAL32(&pPublicKey->HashAlgID);
    uSignAlgId = GET_UNALIGNED_VAL32(&pPublicKey->SigAlgID);

    // Default hashing and signing algorithm IDs if necessary.
    if (uHashAlgId == 0)
        uHashAlgId = CALG_SHA1;
    if (uSignAlgId == 0)
        uSignAlgId = CALG_RSA_SIGN;

    // Create a temporary key container name.
    if (!GetKeyContainerName(&wszKeyContainer, &bTempContainer))
        goto Exit;

    // Find a CSP supporting the required algorithms and create a temporary key
    // container.
    hProv = LocateCSP(wszKeyContainer, SN_CREATE_CONTAINER, uHashAlgId, uSignAlgId);
    if (!hProv)
        goto Error;

    // Import the public key (we need to do this in order to determine the key
    // length reliably).
    if (!SN_CryptImportKey(hProv,
                           pPublicKey->PublicKey,
                           GET_UNALIGNED_VAL32(&pPublicKey->cbPublicKey),
                           0, 0, &hKey))
        goto Error;

    // Query the key attributes (it's the length we're interested in).
    dwBytes = sizeof(dwKeyLen);
    if (!SN_CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)&dwKeyLen, &dwBytes, 0))
        goto Error;

    // Delete the key container.
    if (LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER) == NULL) {
        SetLastError(CORSEC_E_CONTAINER_NOT_FOUND);
        goto Error;
    }

    // Take shortcut for the typical case
    if ((uSignAlgId == CALG_RSA_SIGN) && (dwKeyLen % 8 == 0)) {
        // The signature size is same as the key length for CALG_RSA_SIGN
        *pcbSize = dwKeyLen / 8;
    }
    else {
        // We should not get here - the only supported signing algorithm for PALRT is CALG_RSA_SIGN
        _ASSERTE(false);
        SetLastError(CORSEC_E_CRYPTOAPI_CALL_FAILED);
        goto Error;
    }

    SNLOG(("Signature size for %08X key (%08X bits) is %08X bytes\n", uSignAlgId, dwKeyLen, *pcbSize));

    SN_CryptDestroyKey(hKey);
    FreeCSP(hProv);
    FreeKeyContainerName(wszKeyContainer, bTempContainer);

    retVal = TRUE;
    goto Exit;

 Error:
    SetStrongNameErrorInfo(HRESULT_FROM_GetLastError());
    if (hHash)
        SN_CryptDestroyHash(hHash);
    if (hKey)
        SN_CryptDestroyKey(hKey);
    if (hProv)
        FreeCSP(hProv);
    if (bTempContainer)
        LocateCSP(wszKeyContainer, SN_DELETE_CONTAINER);
    FreeKeyContainerName(wszKeyContainer, bTempContainer);

 Exit:
    END_ENTRYPOINT_VOIDRET;

    return retVal;
}


// Retrieve per-thread context, lazily allocating it if necessary.
SN_THREAD_CTX *GetThreadContext()
{
    SN_THREAD_CTX *pThreadCtx = (SN_THREAD_CTX*)ClrFlsGetValue(TlsIdx_StrongName);
    if (pThreadCtx == NULL) {
        pThreadCtx = new (nothrow) SN_THREAD_CTX;
        if (pThreadCtx == NULL)
            return NULL;
        pThreadCtx->m_dwLastError = S_OK;
        pThreadCtx->m_hProv = NULL;
        EX_TRY {
            ClrFlsSetValue(TlsIdx_StrongName, pThreadCtx);
        }
        EX_CATCH {
            delete pThreadCtx;
            pThreadCtx = NULL;
        }
        EX_END_CATCH (SwallowAllExceptions);
    }
    return pThreadCtx;
}


// Set the per-thread last error code.
VOID SetStrongNameErrorInfo(DWORD dwStatus)
{
    SN_THREAD_CTX *pThreadCtx = GetThreadContext();
    if (pThreadCtx == NULL)
        // We'll return E_OUTOFMEMORY when we attempt to get the error.
        return;
    pThreadCtx->m_dwLastError = dwStatus;
}


// Locate CSP based on criteria specified in the registry (CSP name etc).
// Optionally create or delete a named key container within that CSP.
HCRYPTPROV LocateCSP(LPCWSTR    wszKeyContainer,
                     DWORD      dwAction,
                     ALG_ID     uHashAlgId,
                     ALG_ID     uSignAlgId)
{
    HCRYPTPROV hProv;

    // If we're not trying to create/open/delete a key container, see if a
    // CSP is cached.
    if (dwAction != SN_DELETE_CONTAINER) {
        if ((hProv = LookupCachedCSP())) {
            SNLOG(("Found provider in cache\n"));
            return hProv;
        }
    }
    if (!SN_CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, SN_CAC_FLAGS(dwAction))) {
        SNLOG(("Failed to open: %08X\n", GetLastError()));
        return NULL;
    }

    // If we're not trying to create/open/delete a key container, cache
    // the CSP returned.
    if (dwAction != SN_DELETE_CONTAINER)
        CacheCSP(hProv);
    return (dwAction == SN_DELETE_CONTAINER) ? (HCRYPTPROV)~0 : hProv;
}


// Release a CSP acquired through LocateCSP.
VOID FreeCSP(HCRYPTPROV hProv)
{
    // If the CSP is the one currently cached, don't release it yet.
    if (!IsCachedCSP(hProv))
        SN_CryptReleaseContext(hProv, 0);
}


// Locate a cached CSP for this thread.
HCRYPTPROV LookupCachedCSP()
{
    SN_THREAD_CTX *pThreadCtx = GetThreadContext();
    if (pThreadCtx == NULL)
        return NULL;
    return pThreadCtx->m_hProv;
}


// Update the CSP cache for this thread (freeing any CSP displaced).
VOID CacheCSP(HCRYPTPROV hProv)
{
    SN_THREAD_CTX *pThreadCtx = GetThreadContext();
    if (pThreadCtx == NULL)
        return;
    if (pThreadCtx->m_hProv)
        SN_CryptReleaseContext(pThreadCtx->m_hProv, 0);
    pThreadCtx->m_hProv = hProv;
}


// Determine whether a given CSP is currently cached.
BOOLEAN IsCachedCSP(HCRYPTPROV hProv)
{
    SN_THREAD_CTX *pThreadCtx = GetThreadContext();
    if (pThreadCtx == NULL)
        return FALSE;
    return pThreadCtx->m_hProv == hProv;
}


// rehash all files in a multi-module assembly
BOOLEAN RehashModules (SN_LOAD_CTX *pLoadCtx, LPCWSTR wszFilePath) {
    HRESULT             hr;
    ULONG               ulHashAlg;
    mdAssembly          tkAssembly;
    HENUMInternal       hFileEnum;
    mdFile              tkFile;
    LPCSTR              pszFile;
    BYTE               *pbFileHash;
    DWORD               cbFileHash;
    NewArrayHolder<BYTE> pbNewFileHash(NULL);
    DWORD               cbNewFileHash = 0;
    DWORD               cchDirectory;
    DWORD               cchFullFile;
    CHAR                szFullFile[MAX_PATH + 1];
    WCHAR               wszFullFile[MAX_PATH + 1];
    LPCWSTR             pszSlash;
    DWORD               cchFile;
    IMDInternalImport  *pMetaDataImport = NULL;

    // Determine the directory the assembly lives in (this is where we'll
    // look for linked files).
    if (((pszSlash = wcsrchr(wszFilePath, L'\\')) != NULL) || ((pszSlash = wcsrchr(wszFilePath, L'/')) != NULL)) {
        cchDirectory = (DWORD) (pszSlash - wszFilePath + 1);
        cchDirectory = WszWideCharToMultiByte(CP_UTF8, 0, wszFilePath, cchDirectory, szFullFile, MAX_PATH, NULL, NULL);
        if (cchDirectory >= MAX_PATH) {
            SNLOG(("Assembly directory name too long\n"));
            hr = ERROR_BUFFER_OVERFLOW;
            goto Error;
        }
    } else
        cchDirectory = 0;

    // Open the scope on the mapped image.
    if ((pMetaDataImport = GetMetadataImport(pLoadCtx, &tkAssembly)) == NULL) {
        hr = E_FAIL;
        goto Error;
    }

    // Determine the hash algorithm used for file references.
    pMetaDataImport->GetAssemblyProps(tkAssembly,           // [IN] The Assembly for which to get the properties
                                      NULL,                 // [OUT] Pointer to the Originator blob
                                      NULL,                 // [OUT] Count of bytes in the Originator Blob
                                      &ulHashAlg,           // [OUT] Hash Algorithm
                                      NULL,                 // [OUT] Buffer to fill with name
                                      NULL,                 // [OUT] Assembly MetaData
                                      NULL);                // [OUT] Flags

    // Enumerate all file references.
    if (FAILED(hr = pMetaDataImport->EnumInit(mdtFile, mdTokenNil, &hFileEnum))) {
        SNLOG(("Failed to enumerate linked files, %08X\n", hr));
        goto Error;
    }

    for (; pMetaDataImport->EnumNext(&hFileEnum, &tkFile); ) {

        // Determine the file name and the location of the hash.
        pMetaDataImport->GetFileProps(tkFile,
                                      &pszFile,
                                      (const void **)&pbFileHash,
                                      &cbFileHash,
                                      NULL);

        // Build the full filename by appending to the assembly directory we
        // calculated earlier.
        cchFile = (DWORD) strlen(pszFile);
        if ((cchFile + cchDirectory) >= MAX_PATH) {
            pMetaDataImport->EnumClose(&hFileEnum);
            SNLOG(("Linked file name too long (%s)\n", pszFile));
            hr = ERROR_BUFFER_OVERFLOW;
            goto Error;
        }
        memcpy(&szFullFile[cchDirectory], pszFile, cchFile + 1);

        // Allocate enough buffer for the new hash.
        if (cbNewFileHash < cbFileHash) {
            pbNewFileHash = new (nothrow) BYTE[cbFileHash];
            if (pbNewFileHash == NULL) {
                hr = E_OUTOFMEMORY;
                goto Error;
            }
            cbNewFileHash = cbFileHash;
        }

        cchFullFile = WszMultiByteToWideChar(CP_UTF8, 0, szFullFile, -1, wszFullFile, MAX_PATH);
        if (cchFullFile == 0 || cchFullFile >= MAX_PATH) {
            pMetaDataImport->EnumClose(&hFileEnum);
            SNLOG(("Assembly directory name too long\n"));
            hr = ERROR_BUFFER_OVERFLOW;
            goto Error;
        }

        // Compute a new hash for the file.
        if (FAILED(hr = GetHashFromFileW(wszFullFile,
                                         (unsigned*)&ulHashAlg,
                                         pbNewFileHash,
                                         cbNewFileHash,
                                         &cbNewFileHash))) {
            pMetaDataImport->EnumClose(&hFileEnum);
            SNLOG(("Failed to get compute file hash, %08X\n", hr));
            goto Error;
        }

        // The new hash has to be the same size (since we used the same
        // algorithm).
        _ASSERTE(cbNewFileHash == cbFileHash);

        // We make the assumption here that the pointer to the file hash
        // handed to us by the metadata is a direct pointer and not a
        // buffered copy. If this changes, we'll need a new metadata API to
        // support updates of this type.
        memcpy(pbFileHash, pbNewFileHash, cbFileHash);
    }

    pMetaDataImport->EnumClose(&hFileEnum);
    pMetaDataImport->Release();
    return TRUE;

Error:
    if (pMetaDataImport)
        pMetaDataImport->Release();
    if (pbNewFileHash)
        pbNewFileHash.Release();
    SetLastError(hr);
    return FALSE;
}

// Map an assembly into memory.
BOOLEAN LoadAssembly(SN_LOAD_CTX *pLoadCtx, LPCWSTR wszFilePath, BOOLEAN fRequireSignature)
{
    DWORD dwError = S_OK;

    // If a filename is not supplied, the image has already been mapped (and the
    // image base and length fields set up correctly).
    if (wszFilePath == NULL)
        pLoadCtx->m_fPreMapped = TRUE;
    else {

        pLoadCtx->m_hMap = INVALID_HANDLE_VALUE;
        pLoadCtx->m_pbBase = NULL;

        // Open the file for reading or writing.
        pLoadCtx->m_hFile = WszCreateFile(wszFilePath,
                                          GENERIC_READ | (pLoadCtx->m_fReadOnly ? 0 : GENERIC_WRITE),
                                          pLoadCtx->m_fReadOnly ? FILE_SHARE_READ : FILE_SHARE_WRITE,
                                          NULL,
                                          OPEN_EXISTING,
                                          0,
                                          NULL);
        if (pLoadCtx->m_hFile == INVALID_HANDLE_VALUE) {
            dwError = HRESULT_FROM_GetLastError();
            goto Error;
        }

        pLoadCtx->m_dwLength = SafeGetFileSize(pLoadCtx->m_hFile, NULL);
        if (pLoadCtx->m_dwLength == 0xffffffff) {
            dwError = HRESULT_FROM_GetLastError();
            goto Error;
        }

        // Create a mapping handle for the file.
        pLoadCtx->m_hMap = WszCreateFileMapping(pLoadCtx->m_hFile, NULL, pLoadCtx->m_fReadOnly ? PAGE_READONLY : PAGE_READWRITE, 0, 0, NULL);
        if (pLoadCtx->m_hMap == NULL) {
            dwError = HRESULT_FROM_GetLastError();
            goto Error;
        }

        // And map it into memory.
        pLoadCtx->m_pbBase = (BYTE*)CLRMapViewOfFile(pLoadCtx->m_hMap, pLoadCtx->m_fReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0);
        if (pLoadCtx->m_pbBase == NULL) {
            dwError = HRESULT_FROM_GetLastError();
            goto Error;
        }
    }

    // Locate standard NT image header.
    pLoadCtx->m_pNtHeaders = Cor_RtlImageNtHeader(pLoadCtx->m_pbBase, pLoadCtx->m_dwLength);
    if (pLoadCtx->m_pNtHeaders == NULL) {
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    pLoadCtx->m_pCorHeader = (IMAGE_COR20_HEADER*)Cor_RtlImageDirToVa(PTR_IMAGE_NT_HEADERS(pLoadCtx->m_pNtHeaders),
                                                   pLoadCtx->m_pbBase,
                                                   IMAGE_DIRECTORY_ENTRY_COMHEADER,
                                                   pLoadCtx->m_dwLength);

    if (pLoadCtx->m_pCorHeader == NULL) {
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    // Set up signature pointer (if we require it).
    if (fRequireSignature) {
        pLoadCtx->m_pbSignature = (BYTE*)Cor_RtlImageRvaToVa(PTR_IMAGE_NT_HEADERS(pLoadCtx->m_pNtHeaders),
                                                             pLoadCtx->m_pbBase,
                                                             VAL32(pLoadCtx->m_pCorHeader->StrongNameSignature.VirtualAddress),
                                                             pLoadCtx->m_dwLength);
        pLoadCtx->m_cbSignature = VAL32(pLoadCtx->m_pCorHeader->StrongNameSignature.Size);
    }

    return TRUE;

 Error:
    if (!pLoadCtx->m_fPreMapped) {
    if (pLoadCtx->m_pbBase)
        CLRUnmapViewOfFile(pLoadCtx->m_pbBase);
    if (pLoadCtx->m_hMap != INVALID_HANDLE_VALUE)
        CloseHandle(pLoadCtx->m_hMap);
        if (pLoadCtx->m_hFile != INVALID_HANDLE_VALUE)
            CloseHandle(pLoadCtx->m_hFile);
    }
    SetLastError(dwError);
    return FALSE;
}


// Unload an assembly loaded with LoadAssembly (recomputing checksum if
// necessary).
BOOLEAN UnloadAssembly(SN_LOAD_CTX *pLoadCtx)
{
    BOOLEAN             bResult = TRUE;

    if (!pLoadCtx->m_fReadOnly) {

        IMAGE_NT_HEADERS   *pNtHeaders = NULL;
        DWORD               dwCheckSum = 0;

        pNtHeaders = Cor_RtlImageNtHeader(pLoadCtx->m_pbBase, pLoadCtx->m_dwLength);

        if (pNtHeaders != NULL) {
            if (pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
                ((IMAGE_NT_HEADERS32*)pNtHeaders)->OptionalHeader.CheckSum = VAL32(dwCheckSum);
            else
                if (pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
                    ((IMAGE_NT_HEADERS64*)pNtHeaders)->OptionalHeader.CheckSum = VAL32(dwCheckSum);
        } else
            bResult = FALSE;

    }

    if (!pLoadCtx->m_fPreMapped) {
        if (!CLRUnmapViewOfFile(pLoadCtx->m_pbBase))
            bResult = FALSE;

        if (!CloseHandle(pLoadCtx->m_hMap))
            bResult = FALSE;

        if (!CloseHandle(pLoadCtx->m_hFile))
            bResult = FALSE;
    }

    return bResult;
}

// Reads CSP configuration info (name of CSP to use, IDs of hashing/signing
// algorithms) from the registry.
HRESULT ReadRegistryConfig()
{
    g_uKeySpec = AT_SIGNATURE;
    g_uHashAlgId = CALG_SHA1;
    g_uSignAlgId = CALG_RSA_SIGN;
    g_fCacheVerify = TRUE;

    // Read verify disable records.
    return ReadVerificationRecords();
}


#define CONFIG_FILE_NAME   L"strongname.ini"
#define DIRECTORYSEPARATOR L"\\"
#define DIRECTORYSEPARATORLENGTH 1

static BOOL GetStrongNameConfigPath(__out_ecount(cbFileName) __out_z WCHAR* lpFileName, DWORD cbFileName)
{
    if (!PAL_GetMachineConfigurationDirectoryW(lpFileName, cbFileName))
        return FALSE;

    if (wcslen(lpFileName) + sizeof(CONFIG_FILE_NAME)/sizeof(WCHAR) + DIRECTORYSEPARATORLENGTH >= 
        cbFileName) {
        return FALSE;
    }

    wcscat_s(lpFileName, cbFileName, DIRECTORYSEPARATOR);
    wcscat_s(lpFileName, cbFileName, CONFIG_FILE_NAME);

    return TRUE;
}

static DWORD PAL_IniReadStringEx(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             __out LPWSTR* ppszValue)
{
    DWORD       dwRet;
    LPWSTR      pwzBuf = NULL;
    DWORD       dwSize = 128;

    for (;;) {
        pwzBuf = new (nothrow) WCHAR[dwSize];
        if (!pwzBuf) {
            *ppszValue = NULL;
            return 0;
        }

        dwRet = PAL_IniReadString(hIni, pszSection, pszKey, 
                                        pwzBuf, dwSize);
        if ( (pszSection && pszKey && dwRet == dwSize - 1) ||
            ((!pszSection || !pszKey) && dwRet == dwSize - 2) )
        {
            delete [] pwzBuf;
            dwSize *= 2;
            continue;
        }

        if (dwRet == 0)
        {
            delete [] pwzBuf;
            *ppszValue = NULL;
            return 0;
        }

        break;
    }

    *ppszValue = pwzBuf;
    return dwRet;
}

HRESULT ReadVerificationRecords()
{
    WCHAR      *pwzBuffer = NULL;
    HINI        hIni = NULL;
    WCHAR      *p;
    DWORD       dw;
    SN_VER_REC *pVerificationRecords = NULL;
    WCHAR       wszConfigPath[MAX_PATH+1];
    WCHAR       wszSubKey[MAX_PATH+1];
    HRESULT     hr = S_OK;
    SN_VER_REC *pVerRec = NULL;

    if (!GetStrongNameConfigPath(wszConfigPath, MAX_PATH+1))
        return hr;

    hIni = PAL_IniCreate();
    if (!hIni)
        return hr;

    if (!PAL_IniLoad(hIni, wszConfigPath)) {
        PAL_IniClose(hIni);
        return hr;
    }

    if (!PAL_IniReadStringEx(hIni, SN_CONFIG_VERIFICATION_W, NULL, &pwzBuffer)) {
        PAL_IniClose(hIni);
        return hr;
    }

    p = pwzBuffer;
    while (*p != '\0')
    {
        NewArrayHolder<WCHAR> mszUserList(NULL);
        NewArrayHolder<WCHAR> wszTestPublicKey(NULL);
        NewArrayHolder<WCHAR> wszAssembly(NULL);

        dw = PAL_IniReadString(hIni, SN_CONFIG_VERIFICATION_W, p, wszSubKey, MAX_PATH+1);
        if ((dw == 0) || (dw == MAX_PATH)) {
            hr = E_OUTOFMEMORY;
            goto FreeListExit;
        }

        PAL_IniReadStringEx(hIni, SN_CONFIG_USERLIST_W, p, &mszUserList);
        PAL_IniReadStringEx(hIni, SN_CONFIG_TESTPUBLICKEY_W, p, &wszTestPublicKey);

        size_t dwSubKeyLen = wcslen(wszSubKey);
        wszAssembly = new (nothrow) WCHAR[dwSubKeyLen+1];
        if (!wszAssembly) {
            hr = E_OUTOFMEMORY;
            goto FreeListExit;
        }
        wcsncpy_s(wszAssembly, dwSubKeyLen+1, wszSubKey, _TRUNCATE);
        wszAssembly[dwSubKeyLen] =  L'\0';

        // We've found a valid entry, add it to the local list.
        pVerRec = new (nothrow) SN_VER_REC;
        if (!pVerRec) {
            hr = E_OUTOFMEMORY;
            goto FreeListExit;
        }

        pVerRec->m_mszUserList = mszUserList;
        pVerRec->m_wszTestPublicKey = wszTestPublicKey;
        pVerRec->m_wszAssembly = wszAssembly;

        mszUserList.SuppressRelease();
        wszTestPublicKey.SuppressRelease();
        wszAssembly.SuppressRelease();

        pVerRec->m_pNext = pVerificationRecords;
        pVerificationRecords = pVerRec;
        SNLOG(("Verification record for '%S' found in registry\n", wszSubKey));

        p += wcslen(p) + 1;
    }

    // Initialize the global list of verification records.
    PVOID pv;
    pv = InterlockedCompareExchangePointer((PVOID *) &g_pVerificationRecords, (PVOID) pVerificationRecords, NULL);
    if (pv == NULL)
        goto LExit;

FreeListExit:
    // Iterate over local list of verification records and free allocated memory.
    pVerRec = pVerificationRecords;
    while (pVerRec) {
        delete [] pVerRec->m_mszUserList;
        delete [] pVerRec->m_wszTestPublicKey;
        delete [] pVerRec->m_wszAssembly;
        SN_VER_REC *tmp = pVerRec->m_pNext;
        delete pVerRec;
        pVerRec = tmp;
    }

LExit:
    delete [] pwzBuffer;
    if (hIni != NULL)
        PAL_IniClose(hIni);

    return hr;
}

// Check current user name against a multi-string user name list. Return true if
// the name is found (or the list is empty).
BOOLEAN IsValidUser(__in_z WCHAR *mszUserList)
{
    // No usernames in FEATURE_PAL
    return TRUE;
}


// See if there's a verification records for the given assembly.
SN_VER_REC *GetVerificationRecord(__in_z __deref LPWSTR wszAssemblyName, PublicKeyBlob *pPublicKey)
{
    SN_VER_REC *pVerRec;
    SN_VER_REC *pWildcardVerRec = NULL;
    LPWSTR      pAssembly = NULL;
    BYTE       *pbToken;
    DWORD       cbToken;
    WCHAR       wszStrongName[(SN_SIZEOF_TOKEN * 2) + 1];
    DWORD       i;

    // Compress the public key to make for a shorter assembly name.
    if (!StrongNameTokenFromPublicKey((BYTE*)pPublicKey,
                                      SN_SIZEOF_KEY(pPublicKey),
                                      &pbToken,
                                      &cbToken))
        return NULL;

    if (cbToken > SN_SIZEOF_TOKEN)
        return NULL;

    // Turn the token into hex.
    for (i = 0; i < cbToken; i++) {
        static WCHAR *wszHex = L"0123456789ABCDEF";
        wszStrongName[(i * 2) + 0] = wszHex[(pbToken[i] >> 4)];
        wszStrongName[(i * 2) + 1] = wszHex[(pbToken[i] & 0x0F)];
    }
    wszStrongName[i * 2] = L'\0';
    delete[] pbToken;

    // Build the full assembly name.

    size_t nLen = wcslen(wszAssemblyName) + wcslen(L",") + wcslen(wszStrongName);
    pAssembly = new (nothrow) WCHAR[nLen +1]; // +1 for NULL
    if (pAssembly == NULL)
            return NULL;
    wcscpy_s(pAssembly, nLen + 1, wszAssemblyName);
    wcscat_s(pAssembly, nLen + 1, L",");
    wcscat_s(pAssembly, nLen + 1, wszStrongName);

    // Iterate over global list of verification records.
    for (pVerRec = g_pVerificationRecords; pVerRec; pVerRec = pVerRec->m_pNext) {
        // Look for matching assembly name.
        if (!SString::_wcsicmp(pAssembly, pVerRec->m_wszAssembly)) {
            delete[] pAssembly;
            // Check current user against allowed user name list.
            if (IsValidUser(pVerRec->m_mszUserList))
                return pVerRec;
            else
                return NULL;
        } else if (!wcscmp(L"*,*", pVerRec->m_wszAssembly)) {
            // Found a wildcard record, it'll do if we don't find something more
            // specific.
            if (pWildcardVerRec == NULL)
                pWildcardVerRec = pVerRec;
        } else if (!wcsncmp(L"*,", pVerRec->m_wszAssembly, 2)) {
            // Found a wildcard record (with a specific strong name). If the
            // strong names match it'll do unless we find something more
            // specific (it overrides "*,*" wildcards though).
            if (!SString::_wcsicmp(wszStrongName, &pVerRec->m_wszAssembly[2]))
                pWildcardVerRec = pVerRec;
        }
    }

    delete[] pAssembly;

    // No match on specific assembly name, see if there's a wildcard entry.
    if (pWildcardVerRec)
        // Check current user against allowed user name list.
        if (IsValidUser(pWildcardVerRec->m_mszUserList))
            return pWildcardVerRec;
        else
            return NULL;

    return NULL;
}


// Load metadata engine and return an importer.
IMDInternalImport *GetMetadataImport(SN_LOAD_CTX *pLoadCtx, mdAssembly *ptkAssembly)
{
    BYTE                   *pMetaData;
    HRESULT                 hr;
    IMDInternalImport      *pMetaDataImport = NULL;
    HMODULE                 hLibrary = NULL;
    HRESULT                 (__stdcall *GetMetaDataInternalInterface)(LPVOID, ULONG, DWORD, REFIID, void**);
    DWORD                   dwError = 0;

    // Locate the COM+ meta data within the header.
    pMetaData = (BYTE*)Cor_RtlImageRvaToVa(PTR_IMAGE_NT_HEADERS(pLoadCtx->m_pNtHeaders), pLoadCtx->m_pbBase,
        VAL32(pLoadCtx->m_pCorHeader->MetaData.VirtualAddress), pLoadCtx->m_dwLength);

    if (pMetaData == NULL) {
        SNLOG(("Couldn't locate the COM+ header\n"));
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    // We late bind the metadata function to avoid having a direct dependence on
    // mscoree.dll unless we absolutely need to.
    if ((hLibrary = WszLoadLibrary(MSCOREE_SHIM_W)) == NULL) {
        SNLOG(("WszLoadLibrary(\"" MSCOREE_SHIM_A "\") failed with %08x\n", GetLastError()));
        goto Error;
    }

    if ((*(FARPROC*)&GetMetaDataInternalInterface = GetProcAddress(hLibrary, "GetMetaDataInternalInterface")) == NULL) {
        SNLOG(("Couldn't find GetMetaDataInternalInterface() in mscoree.dll\n"));
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    // Open a metadata scope on the memory directly.
    if (FAILED(hr = GetMetaDataInternalInterface(pMetaData,
                                                 VAL32(pLoadCtx->m_pCorHeader->MetaData.Size),
                                                 ofRead,
                                                 IID_IMDInternalImport,
                                                 (void**)&pMetaDataImport))) {
        SNLOG(("GetMetaDataInternalInterface() failed with %08x\n", hr));
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    // Determine the metadata token for the assembly from the scope.
    if (FAILED(hr = pMetaDataImport->GetAssemblyFromScope(ptkAssembly))) {
        SNLOG(("pMetaData->GetAssemblyFromScope() failed with %08x\n", hr));
        dwError = CORSEC_E_INVALID_IMAGE_FORMAT;
        goto Error;
    }

    return pMetaDataImport;

 Error:
    if (pMetaDataImport)
        pMetaDataImport->Release();
    if (hLibrary)
        FreeLibrary(hLibrary);
    SetLastError(dwError);
    return NULL;
}


// Locate the public key blob located within the metadata of an assembly file
// and return a copy (use delete to deallocate). Optionally get the assembly
// name as well.
PublicKeyBlob *FindPublicKey(SN_LOAD_CTX   *pLoadCtx,
                             __out_ecount_opt(cchAssemblyName) LPWSTR         wszAssemblyName,
                             DWORD          cchAssemblyName)
{
    IMDInternalImport      *pMetaDataImport = NULL;
    PublicKeyBlob          *pKey;
    DWORD                   dwKeyLen;
    mdAssembly              tkAssembly;
    BYTE                   *pKeyCopy = NULL;
    LPCSTR                  szAssemblyName;
    DWORD                   dwError;

    // Open a metadata scope on the image.
    if ((pMetaDataImport = GetMetadataImport(pLoadCtx, &tkAssembly)) == NULL)
        goto Error;

    // Read the public key location from the assembly properties (it's known as
    // the originator property).
    pMetaDataImport->GetAssemblyProps(tkAssembly,           // [IN] The Assembly for which to get the properties
                                      (const void **)&pKey, // [OUT] Pointer to the Originator blob
                                      &dwKeyLen,            // [OUT] Count of bytes in the Originator Blob
                                      NULL,                 // [OUT] Hash Algorithm
                                      &szAssemblyName,      // [OUT] Buffer to fill with name
                                      NULL,                 // [OUT] Assembly MetaData
                                      NULL);                // [OUT] Flags

    if (dwKeyLen == 0) {
        SNLOG(("No public key stored in metadata\n"));
        SetLastError(CORSEC_E_MISSING_STRONGNAME);
        goto Error;
    }

    // Make a copy of the key blob (because we're going to close the metadata
    // scope).
    pKeyCopy = new (nothrow) BYTE[dwKeyLen];
    if (pKeyCopy == NULL) {
        SetLastError(E_OUTOFMEMORY);
        goto Error;
    }
    memcpy(pKeyCopy, pKey, dwKeyLen);

    // Copy the assembly name as well (if it was asked for). We also convert
    // from UTF8 to UNICODE while we're at it.
    if (wszAssemblyName)
        WszMultiByteToWideChar(CP_UTF8, 0, szAssemblyName, -1, wszAssemblyName, cchAssemblyName);

    pMetaDataImport->Release();

    return (PublicKeyBlob*)pKeyCopy;

 Error:
    dwError = GetLastError();
    if (pKeyCopy)
        delete [] pKeyCopy;
    if (pMetaDataImport)
        pMetaDataImport->Release();
    SetLastError(dwError);
    return NULL;
}

BYTE HexToByte (WCHAR wc) {
    if (!iswxdigit(wc)) return (BYTE) 0xff;
    if (iswdigit(wc)) return (BYTE) (wc - L'0');
    if (iswupper(wc)) return (BYTE) (wc - L'A' + 10);
    return (BYTE) (wc - L'a' + 10);
}

// Read the hex string into a PublicKeyBlob structure.
PublicKeyBlob *GetPublicKeyFromHex(LPCWSTR wszPublicKeyHexString) {
    size_t cchHex = wcslen(wszPublicKeyHexString);
    size_t cbHex = cchHex / 2;
    if (cchHex % 2 != 0)
        return NULL;

    BYTE *pKey = new (nothrow) BYTE[cbHex];
    if (!pKey)
        return NULL;
    for (size_t i = 0; i < cbHex; i++) {
        pKey[i] = (BYTE) ((HexToByte(*wszPublicKeyHexString) << 4) | HexToByte(*(wszPublicKeyHexString + 1)));
        wszPublicKeyHexString += 2;
    }
    return (PublicKeyBlob*) pKey;
}

BOOLEAN GetKeyContainerName(LPCWSTR *pwszKeyContainer, BOOLEAN *pbTempContainer)
{
    *pbTempContainer = TRUE;

    if (*pwszKeyContainer != NULL) {
        _ASSERTE(false); // Sorry, no key containers support in PALRT
        SetStrongNameErrorInfo(CORSEC_E_CRYPTOAPI_CALL_FAILED);
        return FALSE;
    }

    return TRUE;
}

VOID FreeKeyContainerName(LPCWSTR wszKeyContainer, BOOLEAN bTempContainer)
{
}

// The common code used to verify a signature (taking into account whether skip
// verification is enabled for the given assembly).
BOOLEAN VerifySignature(SN_LOAD_CTX *pLoadCtx, DWORD dwInFlags, DWORD *pdwOutFlags)
{
    PublicKeyBlob  *pPublicKey;
    PublicKeyBlob  *pRealPublicKey;
    ALG_ID          uHashAlgId;
    ALG_ID          uSignAlgId;
    HCRYPTPROV      hProv = NULL;
    HCRYPTHASH      hHash = NULL;
    HCRYPTKEY       hKey = NULL;
    WCHAR           wszAssemblyName[MAX_PATH + 1];
    DWORD           dwError;
    BYTE           *pbRealPublicKey = NULL;
    DWORD           cbRealPublicKey = NULL;
    SN_VER_REC     *pVerRec = NULL;

    if (pdwOutFlags)
        *pdwOutFlags = 0;

    // Read the public key used to sign the assembly from the assembly metadata.
    // Also get the assembly name, we might need this if we fail the
    // verification and need to look up a verification disablement entry.
    pPublicKey = FindPublicKey(pLoadCtx,
                               wszAssemblyName,
                               sizeof(wszAssemblyName) / sizeof(WCHAR));
    if (pPublicKey == NULL)
        goto Error;

    // If this isn't the first time we've been called for this assembly and we
    // know it was fully signed and we're confident it couldn't have been
    // tampered with in the meantime, we can just skip the verification.
    if (!(dwInFlags & SN_INFLAG_FORCE_VER) &&
        !(dwInFlags & SN_INFLAG_INSTALL) &&
        (pLoadCtx->m_pCorHeader->Flags & VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED)) &&
        ((dwInFlags & SN_INFLAG_ADMIN_ACCESS) || g_fCacheVerify)) {
        SNLOG(("Skipping verification due to cached result\n"));
        DbgCount(dwInFlags & SN_INFLAG_RUNTIME ? L"RuntimeSkipCache" : L"FusionSkipCache");
        delete [] (BYTE*)pPublicKey;
        return TRUE;
    }

    // If we're not forcing verification, let's see if there's a skip
    // verification entry for this assembly. If there is we can skip all the
    // hard work and just lie about the strong name now. The exception is if the
    // assembly is marked as fully signed, in which case we have to force a
    // verification to see if they're telling the truth.
    pVerRec = GetVerificationRecord(wszAssemblyName, pPublicKey);
    if (!(dwInFlags & SN_INFLAG_FORCE_VER) && !(pLoadCtx->m_pCorHeader->Flags & VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED))) {
        if (pVerRec) {
            if (pVerRec->m_wszTestPublicKey) {
                // substitute the public key with the test public key.
                PublicKeyBlob *pKey = GetPublicKeyFromHex(pVerRec->m_wszTestPublicKey);
                if (pKey) {
                    SNLOG(("Using test public key for verification due to registry entry\n"));
                    DbgCount(dwInFlags & SN_INFLAG_RUNTIME ? L"RuntimeSkipDelay" : L"FusionSkipDelay");

                    delete [] (BYTE*)pPublicKey;
                    pPublicKey = pKey;
                }
            }
            else {
                SNLOG(("Skipping verification due to registry entry\n"));
                DbgCount(dwInFlags & SN_INFLAG_RUNTIME ? L"RuntimeSkipDelay" : L"FusionSkipDelay");
                delete [] (BYTE*)pPublicKey;
                return TRUE;
            }
        }
    }

    uHashAlgId = GET_UNALIGNED_VAL32(&pPublicKey->HashAlgID);
    uSignAlgId = GET_UNALIGNED_VAL32(&pPublicKey->SigAlgID);

    // Default hashing and signing algorithm IDs if necessary.
    if (uHashAlgId == 0)
        uHashAlgId = CALG_SHA1;
    if (uSignAlgId == 0)
        uSignAlgId = CALG_RSA_SIGN;

    // Find a CSP supporting the required algorithms.
    hProv = LocateCSP(NULL, SN_IGNORE_CONTAINER, uHashAlgId, uSignAlgId);
    if (!hProv)
        goto Error;

    // Import the public key used to check the signature. If we're handed the
    // ECMA key, we translate it to the real (MS) key at this point.
    pRealPublicKey = SN_IS_NEUTRAL_KEY(pPublicKey) ? SN_THE_KEY() : pPublicKey;

    pbRealPublicKey = pRealPublicKey->PublicKey;
    cbRealPublicKey = GET_UNALIGNED_VAL32(&pRealPublicKey->cbPublicKey);
    if (!SN_CryptImportKey(hProv,
                           pbRealPublicKey,
                           cbRealPublicKey,
                           0, 0, &hKey))
        goto Error;

    // Create a hash object.
    if (!SN_CryptCreateHash(hProv, uHashAlgId, 0, 0, &hHash))
        goto Error;

    // Compute a hash over the image.
    if (!ComputeHash(pLoadCtx, hHash, CalcHash, NULL))
        goto Error;

    // Verify the hash against the signature.
    DbgCount(dwInFlags & SN_INFLAG_RUNTIME ? L"RuntimeVerify" : L"FusionVerify");
    if (pLoadCtx->m_pbSignature != NULL && pLoadCtx->m_cbSignature != 0 && 
        SN_CryptVerifySignatureA(hHash, pLoadCtx->m_pbSignature, pLoadCtx->m_cbSignature, hKey, NULL, 0)) {
        SNLOG(("Verification succeeded (for real)\n"));
        if (pdwOutFlags)
            *pdwOutFlags |= SN_OUTFLAG_WAS_VERIFIED;
    } else {
        SNLOG(("Verification failed\n"));
        SetLastError(CORSEC_E_INVALID_STRONGNAME);
        goto Error;
    }

    SN_CryptDestroyHash(hHash);
    SN_CryptDestroyKey(hKey);
    FreeCSP(hProv);
    delete [] (BYTE*)pPublicKey;

    return TRUE;

 Error:
    dwError = GetLastError();
    if (pPublicKey)
        delete [] (BYTE*)pPublicKey;
    if (hKey)
        SN_CryptDestroyKey(hKey);
    if (hHash)
        SN_CryptDestroyHash(hHash);
    if (hProv)
        FreeCSP(hProv);
    SetLastError(dwError);
    return FALSE;
}


// Compute a hash over the elements of an assembly manifest file that should
// remain static (skip checksum, Authenticode signatures and strong name
// signature blob).
BOOLEAN ComputeHash(SN_LOAD_CTX *pLoadCtx, HCRYPTHASH hHash, HashFunc func, void* cookie)
{
    union {
        IMAGE_NT_HEADERS32  m_32;
        IMAGE_NT_HEADERS64  m_64;
    }                       sHeaders;
    IMAGE_SECTION_HEADER   *pSections;
    ULONG                   i;
    BYTE                   *pbSig = pLoadCtx->m_pbSignature;
    DWORD                   cbSig = pLoadCtx->m_cbSignature;

#define SN_HASH(_start, _length) do { if (!func(hHash, (_start), (_length), 0, cookie)) return FALSE; } while (false)

    // Hash the DOS header if it exists.
    if ((BYTE*)pLoadCtx->m_pNtHeaders != pLoadCtx->m_pbBase)
        SN_HASH(pLoadCtx->m_pbBase, (DWORD)((BYTE*)pLoadCtx->m_pNtHeaders - pLoadCtx->m_pbBase));

    // Add image headers minus the checksum and security data directory.
    if (pLoadCtx->m_pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC)) {
        sHeaders.m_32 = *((IMAGE_NT_HEADERS32*)pLoadCtx->m_pNtHeaders);
        sHeaders.m_32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress = 0;
        sHeaders.m_32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size = 0;
        sHeaders.m_32.OptionalHeader.CheckSum = 0;
        SN_HASH((BYTE*)&sHeaders.m_32, sizeof(sHeaders.m_32));
    } else if (pLoadCtx->m_pNtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
        sHeaders.m_64 = *((IMAGE_NT_HEADERS64*)pLoadCtx->m_pNtHeaders);
        sHeaders.m_64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress = 0;
        sHeaders.m_64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size = 0;
        sHeaders.m_64.OptionalHeader.CheckSum = 0;
        SN_HASH((BYTE*)&sHeaders.m_64, sizeof(sHeaders.m_64));
    } else {
        SetLastError(CORSEC_E_INVALID_IMAGE_FORMAT);
        return FALSE;
    }

    // Then the section headers.
    pSections = IMAGE_FIRST_SECTION(pLoadCtx->m_pNtHeaders);
    SN_HASH((BYTE*)pSections, VAL16(pLoadCtx->m_pNtHeaders->FileHeader.NumberOfSections) * sizeof(IMAGE_SECTION_HEADER));

    // Finally, add data from each section.
    for (i = 0; i < VAL16(pLoadCtx->m_pNtHeaders->FileHeader.NumberOfSections); i++) {
        BYTE   *pbData = pLoadCtx->m_pbBase + VAL32(pSections[i].PointerToRawData);
        DWORD   cbData = VAL32(pSections[i].SizeOfRawData);

        // We need to exclude the strong name signature blob from the hash. The
        // blob could intersect the section in a number of ways.

        if ((pbSig + cbSig) <= pbData || pbSig >= (pbData + cbData))
            // No intersection at all. Hash all data.
            SN_HASH(pbData, cbData);
        else if (pbSig == pbData && cbSig == cbData)
            // Signature consumes entire block. Hash no data.
            ;
        else if (pbSig == pbData)
            // Signature at start. Hash end.
            SN_HASH(pbData + cbSig, cbData - cbSig);
        else if ((pbSig + cbSig) == (pbData + cbData))
            // Signature at end. Hash start.
            SN_HASH(pbData, cbData - cbSig);
        else {
            // Signature in the middle. Hash head and tail.
            SN_HASH(pbData, (DWORD)(pbSig - pbData));
            SN_HASH(pbSig + cbSig, cbData - (DWORD)(pbSig + cbSig - pbData));
        }
    }

#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
    if (hHash != (HCRYPTHASH)INVALID_HANDLE_VALUE) {
        BYTE   *pbHash;
        DWORD   cbHash;
        DWORD   dwRetLen = sizeof(cbHash);
        if (SN_CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&cbHash, &dwRetLen, 0)) {
            pbHash = (BYTE*)_alloca(cbHash);
            if (SN_CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &cbHash, 0)) {
                SNLOG(("Computed Hash Value (%u bytes):\n", cbHash));
                HexDump(pbHash, cbHash);
            } else
                SNLOG(("CryptGetHashParam() failed with %08X\n", GetLastError()));
        } else
            SNLOG(("CryptGetHashParam() failed with %08X\n", GetLastError()));
    }
#endif // _DEBUG

    return TRUE;
}


SNAPI_(DWORD) GetHashFromAssemblyFile(LPCSTR szFilePath, // [IN] location of file to be hashed
                                      unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                      BYTE   *pbHash,    // [OUT] hash buffer
                                      DWORD  cchHash,    // [IN]  max size of buffer
                                      DWORD  *pchHash)   // [OUT] length of hash byte array
{
    BOOL retVal = FALSE;

    BEGIN_ENTRYPOINT_NOTHROW;
    // Convert filename to wide characters and call the W version of this
    // function.

    MAKE_WIDEPTR_FROMANSI(wszFilePath, szFilePath);
    retVal = GetHashFromAssemblyFileW(wszFilePath, piHashAlg, pbHash, cchHash, pchHash);
    END_ENTRYPOINT_NOTHROW;
    return retVal;
}

SNAPI_(DWORD) GetHashFromAssemblyFileW(LPCWSTR wszFilePath, // [IN] location of file to be hashed
                                       unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                       BYTE   *pbHash,    // [OUT] hash buffer
                                       DWORD  cchHash,    // [IN]  max size of buffer
                                       DWORD  *pchHash)   // [OUT] length of hash byte array
{
    HRESULT         hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    SN_LOAD_CTX     sLoadCtx;
    BYTE           *pbMetaData;
    DWORD           cbMetaData;

    sLoadCtx.m_fReadOnly = TRUE;
    if (!LoadAssembly(&sLoadCtx, wszFilePath, FALSE))
        IfFailGo(HRESULT_FROM_GetLastError());

    pbMetaData = (BYTE*)Cor_RtlImageRvaToVa(PTR_IMAGE_NT_HEADERS(sLoadCtx.m_pNtHeaders),
                                        sLoadCtx.m_pbBase,
                                        VAL32(sLoadCtx.m_pCorHeader->MetaData.VirtualAddress),
                                        sLoadCtx.m_dwLength);
    if (pbMetaData == NULL) {
        UnloadAssembly(&sLoadCtx);
        IfFailGo(E_INVALIDARG);
    }
    cbMetaData = VAL32(sLoadCtx.m_pCorHeader->MetaData.Size);

    hr = GetHashFromBlob(pbMetaData, cbMetaData, piHashAlg, pbHash, cchHash, pchHash);

    UnloadAssembly(&sLoadCtx);
ErrExit:
    
    END_ENTRYPOINT_NOTHROW;
    return hr;
}
    
SNAPI_(DWORD) GetHashFromFile(LPCSTR szFilePath, // [IN] location of file to be hashed
                              unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                              BYTE   *pbHash,    // [OUT] hash buffer
                              DWORD  cchHash,    // [IN]  max size of buffer
                              DWORD  *pchHash)   // [OUT] length of hash byte array
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;
    
    HANDLE hFile = CreateFileA(szFilePath,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_GetLastError();
    }
    else
    {
        hr = GetHashFromHandle(hFile, piHashAlg, pbHash, cchHash, pchHash);
        CloseHandle(hFile);
    }
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

SNAPI_(DWORD) GetHashFromFileW(LPCWSTR wszFilePath, // [IN] location of file to be hashed
                               unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                               BYTE   *pbHash,    // [OUT] hash buffer
                               DWORD  cchHash,    // [IN]  max size of buffer
                               DWORD  *pchHash)   // [OUT] length of hash byte array
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    HANDLE hFile = WszCreateFile(wszFilePath,
                                 GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                 NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        IfFailGo(HRESULT_FROM_GetLastError());

    hr = GetHashFromHandle(hFile, piHashAlg, pbHash, cchHash, pchHash);
    CloseHandle(hFile);
ErrExit:
    
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

SNAPI_(DWORD) GetHashFromHandle(HANDLE hFile,      // [IN] handle of file to be hashed
                                unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                BYTE   *pbHash,    // [OUT] hash buffer
                                DWORD  cchHash,    // [IN]  max size of buffer
                                DWORD  *pchHash)   // [OUT] length of hash byte array
{
    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    PBYTE pbBuffer = NULL;
    DWORD dwFileLen = SafeGetFileSize(hFile, 0);
    if (dwFileLen == 0xffffffff)
        IfFailGo(HRESULT_FROM_GetLastError());

    if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
        IfFailGo(HRESULT_FROM_GetLastError());

    DWORD dwResultLen;
    pbBuffer = new (nothrow) BYTE[dwFileLen];
    IfNullGo(pbBuffer);

    if (ReadFile(hFile, pbBuffer, dwFileLen, &dwResultLen, NULL))
        hr = GetHashFromBlob(pbBuffer, dwResultLen, piHashAlg, pbHash, cchHash, pchHash);
    else
        hr = HRESULT_FROM_GetLastError();

    delete[] pbBuffer;

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

SNAPI_(DWORD) GetHashFromBlob(BYTE   *pbBlob,       // [IN] pointer to memory block to hash
                              DWORD  cchBlob,       // [IN] length of blob
                              unsigned int *piHashAlg,  // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                              BYTE   *pbHash,       // [OUT] hash buffer
                              DWORD  cchHash,       // [IN]  max size of buffer
                              DWORD  *pchHash)      // [OUT] length of hash byte array
{
    HRESULT    hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;


    if (!piHashAlg || !pbHash || !pchHash)
        IfFailGo(E_INVALIDARG);

    if (!(*piHashAlg))
        *piHashAlg = CALG_SHA1;

    *pchHash = cchHash;

    if ((!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) ||
        (!CryptCreateHash(hProv, *piHashAlg, 0, 0, &hHash)) ||
        (!CryptHashData(hHash, pbBlob, cchBlob, 0)) ||
        (!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, pchHash, 0)))
        hr = HRESULT_FROM_GetLastError();

    if (hHash)
        CryptDestroyHash(hHash);
    if (hProv)
        CryptReleaseContext(hProv, 0);

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

#endif // #ifndef DACCESS_COMPILE
