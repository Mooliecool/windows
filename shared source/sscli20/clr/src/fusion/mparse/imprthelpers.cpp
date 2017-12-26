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

#include "imprthelpers.h"
#include "cacheutils.h"
#include "naming.h"
#include "corpriv.h"
#include "product_version.h"

// Some notes on dwFlags: 
//   We always try to set name, version, culture, publickey/token.
//   If you want more properties to set, 
//   specify them in dwFlags
//   dwFlags should be combination of ASM_IMPORT_NAME_FLAGS
//
STDAPI
GetAssemblyNameDefFromMDImport(IMetaDataAssemblyImport *pMDImport, 
                               DWORD dwFlags,
                               BOOL  bFinalize,
                               IAssemblyName **ppName)
{
    HRESULT                 hr = S_OK;
    mdAssembly              mda;
    ASSEMBLYMETADATA        amd = {0};
    VOID                    *pvPublicKeyToken = NULL;
    DWORD                   dwPublicKeyToken = 0;
    TCHAR                   szAssemblyName[MAX_PATH];
    DWORD                   dwDefFlags = 0;
    DWORD                   dwSize = 0;
    DWORD                   dwHashAlgId = 0;
    DWORD                   cbSigSize = 0;
    ISNAssemblySignature    *pSignature = NULL;
    IMetaDataImport2        *pImport = NULL;
    GUID                    guidMVID;
    BYTE                    abSignature[SIGNATURE_BLOB_LENGTH];
    IAssemblyName           *pName = NULL;
    int                     i;
    BOOL    fLegacyAssembly = FALSE;

    _ASSERTE(pMDImport);
    
    // Get the assembly token
    hr = pMDImport->GetAssemblyFromScope(&mda);
    if (FAILED(hr)) {
        hr = COR_E_ASSEMBLYEXPECTED;
        goto ErrExit;
    }

    // Default allocation sizes
    amd.ulProcessor = 32;
    amd.ulOS = 32;
    amd.cbLocale = MAX_PATH;

        // Loop max 2 (try/retry)
    for (i = 0; i < 2; i++) {
        IfFailGo(AllocateAssemblyMetaData(&amd));

        szAssemblyName[0] = L'\0';

        // Get name and metadata
        hr = pMDImport->GetAssemblyProps(             
            mda,            // [IN] The Assembly for which to get the properties.
            (const void **)&pvPublicKeyToken,  // [OUT] Pointer to the PublicKeyToken blob.
            &dwPublicKeyToken,  // [OUT] Count of bytes in the PublicKeyToken Blob.
            &dwHashAlgId,   // [OUT] Hash Algorithm.
            szAssemblyName, // [OUT] Buffer to fill with name.
            MAX_PATH, // [IN]  Size of buffer in wide chars.
            &dwSize,        // [OUT] Actual # of wide chars in name.
            &amd,           // [OUT] Assembly MetaData.
            &dwDefFlags        // [OUT] Flags.
          );

        if (FAILED(hr)) {
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = FUSION_E_INVALID_NAME;
            }
            goto ErrExit;
        }

        // Check if retry necessary.
        if (!i)
        {
            if (amd.ulProcessor <= 32 && amd.ulOS <= 32) {
                break;
            }            
            else {
                DeAllocateAssemblyMetaData(&amd);
            }
        }
        // Retry with updated sizes
    }

    // Allow for funky null locale convention
    // in metadata - cbLocale == 0 means szLocale ==L'\0'
    if (!amd.cbLocale) {
        _ASSERTE(amd.szLocale && !*(amd.szLocale));
        amd.cbLocale = 1;
    }
    else if (amd.szLocale) {
        WCHAR *ptr;
        ptr = StrChrW(amd.szLocale, L';');
        if (ptr) {
            (*ptr) = L'\0';
            amd.cbLocale = ((DWORD) (ptr - amd.szLocale) + 1);
        }            
    }
    else {
        _ASSERTE(!"ASSEMBLYMETADATA.cbLocal should not be 0!");
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto ErrExit;
    }

    if ( (szAssemblyName[0] == L'\0') || (lstrlenW(szAssemblyName) >= MAX_PATH)) {
        // Name is too long.

        hr = FUSION_E_INVALID_NAME;
        goto ErrExit;
    }

    // Create a name object and hand it out;
    IfFailGo(CreateAssemblyNameObjectFromMetaData(&pName, szAssemblyName, &amd, NULL));

    // See if the assembly[def] is retargetable (ie, for a generic assembly).
    if (IsAfRetargetable(dwDefFlags)) {
        BOOL bTrue = TRUE;
        IfFailGo(pName->SetProperty(ASM_NAME_RETARGET, &bTrue, sizeof(bTrue)));
    }

    // Set Hash Algorithm
    if (dwFlags & ASM_IMPORT_NAME_HASH_ALGID) {
        IfFailGo(pName->SetProperty(ASM_NAME_HASH_ALGID, &dwHashAlgId, sizeof(DWORD)));
    }

    // Set public key or public key token
    if (IsAfPublicKey(dwDefFlags)) {
        IfFailGo(pName->SetProperty(((pvPublicKeyToken && dwPublicKeyToken) ? (ASM_NAME_PUBLIC_KEY) : (ASM_NAME_NULL_PUBLIC_KEY)),
                             pvPublicKeyToken, dwPublicKeyToken * sizeof(BYTE)));
    }
    else {
        IfFailGo(pName->SetProperty(((pvPublicKeyToken && dwPublicKeyToken) ? (ASM_NAME_PUBLIC_KEY_TOKEN) : (ASM_NAME_NULL_PUBLIC_KEY_TOKEN)),
                            pvPublicKeyToken, dwPublicKeyToken * sizeof(BYTE)));
    }

    // Set signature blob

    if (dwFlags & ASM_IMPORT_NAME_SIGNATURE_BLOB) {
        IfFailGo(pMDImport->QueryInterface(IID_ISNAssemblySignature, (void **)&pSignature));

        // It is legitimate for this to fail if the assembly is not strong name signed.  If so,
        // just skip the property and continue.  No need to reset the HR, since it is assigned
        // to in the statement after this block.
        cbSigSize = SIGNATURE_BLOB_LENGTH;
        hr = pSignature->GetSNAssemblySignature(abSignature, &cbSigSize);
        if (SUCCEEDED(hr)) {
            if (!(cbSigSize == SIGNATURE_BLOB_LENGTH || cbSigSize == SIGNATURE_BLOB_LENGTH_HASH)) {
                _ASSERTE(!"Incorrect size for signature blob!");
                hr = E_UNEXPECTED;
                goto ErrExit;
            }
        
            IfFailGo(pName->SetProperty(ASM_NAME_SIGNATURE_BLOB, abSignature, SIGNATURE_BLOB_LENGTH_HASH));
        }
        else
            hr = S_OK;
    }

    // Set MVID
    if (dwFlags & ASM_IMPORT_NAME_MVID) {
        IfFailGo(pMDImport->QueryInterface(IID_IMetaDataImport2, (void **)&pImport));
        IfFailGo(pImport->GetScopeProps(NULL, 0, 0, &guidMVID));
        IfFailGo(pName->SetProperty(ASM_NAME_MVID, &guidMVID, sizeof(guidMVID)));
    }

    if(dwFlags & ASM_IMPORT_NAME_PE_RUNTIME) {
        WCHAR   wzPERuntime[MAX_VERSION_DISPLAY_SIZE];
        DWORD   dwLen = 0;

        wzPERuntime[0] = L'\0';

        if(!pImport) {
            hr = pMDImport->QueryInterface(IID_IMetaDataImport2, (void **)&pImport);
            if(FAILED(hr)) {
                goto ErrExit;
            }
        }

        hr = pImport->GetVersionString(wzPERuntime, MAX_VERSION_DISPLAY_SIZE, &dwLen);
        if(FAILED(hr)) {
            goto ErrExit;
        }

        if( !FusionCompareStringN(wzPERuntime, L"v1.0", 4, FALSE) ||
            !FusionCompareStringN(wzPERuntime, L"v1.1", 4, FALSE) ||
            !FusionCompareStringI(wzPERuntime, g_wzStandardCLI2002) ) {
            fLegacyAssembly = TRUE;
        }

        // reject assemblies built with newer runtime. 
        DWORD dwMajor = 0;
        DWORD dwMinor = 0;
        LPWSTR pwChar = NULL;

        if (wzPERuntime[0]) {
            dwMajor = StrToIntW(&(wzPERuntime[1])); // skip the first 'v'
            pwChar = StrChr(wzPERuntime, L'.');
            if (pwChar) {
                pwChar++;
                dwMinor = StrToIntW(pwChar);
            }
        }
        
        if ((dwMajor > VER_MAJORVERSION) || (dwMajor == VER_MAJORVERSION && dwMinor > VER_MINORVERSION)) {
            hr = COR_E_NEWER_RUNTIME;
            goto ErrExit;
        }
    }
    
    // Set Assembly Architecture
    if(dwFlags & ASM_IMPORT_NAME_ARCHITECTURE) {
        PEKIND      PeKind = peNone;

        if (!fLegacyAssembly) {
            CorPEKind   CORPeKind = peNot;
            DWORD       dwImageType = 0;

            if(!pImport) {
                hr = pMDImport->QueryInterface(IID_IMetaDataImport2, (void **)&pImport);
                if(FAILED(hr)) {
                    goto ErrExit;
                }
            }

            hr = pImport->GetPEKind((LPDWORD) &CORPeKind, &dwImageType);
            if(FAILED(hr)) {
                goto ErrExit;
            }

            hr = TranslatePEToArchitectureType(CORPeKind, dwImageType, &PeKind);
            if(FAILED(hr)) {
                goto ErrExit;
            }
        }

        hr = pName->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &PeKind, sizeof(PeKind));
        if(FAILED(hr)) {
            goto ErrExit;
        }
    }

    // Set name def to read-only.
    // Any subsequent calls to SetProperty on this name
    // will fire an assert.
    if (bFinalize)
        pName->Finalize();

    *ppName = pName;
    pName->AddRef();

ErrExit:
    DeAllocateAssemblyMetaData(&amd);

    SAFERELEASE(pImport);
    SAFERELEASE(pSignature);
    SAFERELEASE(pName);

    return hr;
}

// dwFlags same meaning as in GetAssemblyNameDefFromMDImport
STDAPI
GetAssemblyNameRefFromMDImport(IMetaDataAssemblyImport *pMDImport, 
                                mdAssemblyRef mdar, 
                                DWORD dwFlags,
                                IAssemblyName **ppName)
{
    HRESULT             hr = S_OK;
    IMetaDataImport2    *pImport = NULL;
    TCHAR               szAssemblyName[MAX_PATH];
    const VOID*         pvPublicKeyToken = 0;
    const VOID*         pvHashValue = NULL;

    DWORD               ccAssemblyName = MAX_PATH,
                        cbPublicKeyToken   = 0,
                        cbHashValue    = 0,
                        dwRefFlags     = 0;
    INT                 i;
    ASSEMBLYMETADATA    amd = {0};


    // Default allocation sizes.
    amd.ulProcessor = amd.ulOS = 32;
    amd.cbLocale = MAX_PATH;
    
    // Loop max 2 (try/retry)
    for (i = 0; i < 2; i++)
    {
        // Allocate ASSEMBLYMETADATA instance.
        IfFailGo(AllocateAssemblyMetaData(&amd));
   
        // Get the properties for the refrenced assembly.
        IfFailGo(pMDImport->GetAssemblyRefProps(
            mdar,              // [IN] The AssemblyRef for which to get the properties.
            &pvPublicKeyToken,      // [OUT] Pointer to the PublicKeyToken blob.
            &cbPublicKeyToken,      // [OUT] Count of bytes in the PublicKeyToken Blob.
            szAssemblyName,     // [OUT] Buffer to fill with name.
            MAX_PATH,     // [IN] Size of buffer in wide chars.
            &ccAssemblyName,    // [OUT] Actual # of wide chars in name.
            &amd,               // [OUT] Assembly MetaData.
            &pvHashValue,       // [OUT] Hash blob.
            &cbHashValue,       // [OUT] Count of bytes in the hash blob.
/*
            NULL,               // [OUT] Token for Execution Location.
*/
            &dwRefFlags         // [OUT] Flags.
            ));

        // Check if retry necessary.
        if (!i)
        {   
            if (amd.ulProcessor <= 32 
                && amd.ulOS <= 32)
            {
                break;
            }            
            else
                DeAllocateAssemblyMetaData(&amd);
        }

    // Retry with updated sizes
    }

    // Allow for funky null locale convention
    // in metadata - cbLocale == 0 means szLocale ==L'\0'
    if (!amd.cbLocale)
    {
        _ASSERTE(amd.szLocale && !*(amd.szLocale));
        amd.cbLocale = 1;
    }
    else if (amd.szLocale)
    {
        WCHAR *ptr;
        ptr = StrChrW(amd.szLocale, L';');
        if (ptr)
        {
            (*ptr) = L'\0';
            amd.cbLocale = ((DWORD) (ptr - amd.szLocale) + 1);
        }            
    }
    else
    {
        _ASSERTE(!"ASSEMBLYMETADATA.cbLocal should not be 0!");
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto ErrExit;
    }
    
    // Create the assembly name object.
    IfFailGo(CreateAssemblyNameObjectFromMetaData(ppName, szAssemblyName, &amd, NULL));

    // Set the rest of the properties.
    // PublicKeyToken
    if (IsAfPublicKey(dwRefFlags)) {
        IfFailGo((*ppName)->SetProperty((pvPublicKeyToken && cbPublicKeyToken) ?
                ASM_NAME_PUBLIC_KEY : ASM_NAME_NULL_PUBLIC_KEY,
                (LPVOID) pvPublicKeyToken, cbPublicKeyToken));
    }
    else {
        IfFailGo((*ppName)->SetProperty((pvPublicKeyToken && cbPublicKeyToken) ?
            ASM_NAME_PUBLIC_KEY_TOKEN : ASM_NAME_NULL_PUBLIC_KEY_TOKEN,
            (LPVOID) pvPublicKeyToken, cbPublicKeyToken));
    }

    // Hash value
    if (dwFlags & ASM_IMPORT_NAME_HASH_VALUE) {
        IfFailGo((*ppName)->SetProperty(ASM_NAME_HASH_VALUE, 
            (LPVOID) pvHashValue, cbHashValue));
    }

        
    // See if the assembly[ref] is retargetable (ie, for a generic assembly).
    if (IsAfRetargetable(dwRefFlags)) {
        BOOL bTrue = TRUE;
        IfFailGo((*ppName)->SetProperty(ASM_NAME_RETARGET, &bTrue, sizeof(bTrue)));

    }

ErrExit:

    SAFERELEASE(pImport);

    if (FAILED(hr)) {
        SAFERELEASE(*ppName);
    }
    
    DeAllocateAssemblyMetaData(&amd);
        
    return hr;
}

HRESULT 
GetAssemblyModuleListFromMDImport(IMetaDataAssemblyImport *pMDImport,
                                   __deref_out LPWSTR **pppwzModules,
                                   LPDWORD  pdwNumModules)
{
    HRESULT              hr = S_OK;
    HCORENUM             hEnum = NULL;
    mdFile               mdf;
    TCHAR                szModuleName[MAX_PATH];
    DWORD                ccModuleName = MAX_PATH;
    const VOID           *pvHashValue = NULL;
    DWORD                cbHashValue = 0;
    DWORD                dwFlags = 0;
    mdFile               *rAssemblyModuleTokens;
    DWORD                cAssemblyModuleTokens = ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE;
    DWORD                i;
    LPWSTR              *ppModules = NULL;
    DWORD                dwNumModules = 0;

    _ASSERTE(pMDImport);
   
    IfNullGo(rAssemblyModuleTokens = NEW(mdFile[ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE]));

    // get the total number of modules
    while (cAssemblyModuleTokens > 0) {
        IfFailGo(pMDImport->EnumFiles(&hEnum, rAssemblyModuleTokens,
                                  ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE,
                                  &cAssemblyModuleTokens));
        dwNumModules += cAssemblyModuleTokens;
    }

    // reset hEnum
    pMDImport->CloseEnum(hEnum);
    hEnum = NULL;

    // Copy modules only if there are some
    if (dwNumModules) {
        // re-size if necessary
        if (dwNumModules > ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE) {
            SAFEDELETEARRAY(rAssemblyModuleTokens);
            IfNullGo(rAssemblyModuleTokens = NEW(mdFile[dwNumModules]));
        }

        IfFailGo(pMDImport->EnumFiles(&hEnum, rAssemblyModuleTokens, dwNumModules, &dwNumModules));

        // we don't need hEnum anymore
        pMDImport->CloseEnum(hEnum);
        hEnum = NULL;

        IfNullGo(ppModules = NEW(LPWSTR[dwNumModules]));
        ZeroMemory(ppModules, dwNumModules*sizeof(LPWSTR)); 

        // copy modules
        for (i = 0; i < dwNumModules; i++) {
            mdf = rAssemblyModuleTokens[i];
        
            hr = pMDImport->GetFileProps(
                mdf,            // [IN] The File for which to get the properties.
                szModuleName,   // [OUT] Buffer to fill with name.
                MAX_PATH,       // [IN] Size of buffer in wide chars.
                &ccModuleName,  // [OUT] Actual # of wide chars in name.
                &pvHashValue,   // [OUT] Pointer to the Hash Value Blob.
                &cbHashValue,   // [OUT] Count of bytes in the Hash Value Blob.
                &dwFlags);      // [OUT] Flags.

            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = FUSION_E_INVALID_NAME;
            }
            if (hr == CLDB_S_TRUNCATION) {
                // Cannot have a name greater than MAX_PATH
                hr = FUSION_E_ASM_MODULE_MISSING;
            }
            if (FAILED(hr)) {
                goto ErrExit;
            }

            IfNullGo(ppModules[i] = NEW(WCHAR[ccModuleName]));

            hr = StringCchCopy(ppModules[i], ccModuleName, szModuleName);
            if (FAILED(hr)) {
                goto ErrExit;
            }
        }
    }

    *pppwzModules = ppModules;
    *pdwNumModules = dwNumModules;

    hr = S_OK;

ErrExit:
    if (hEnum) {
        pMDImport->CloseEnum(hEnum);
    }

    SAFEDELETEARRAY(rAssemblyModuleTokens);

    if (FAILED(hr) && ppModules) {
        for (i = 0; i < dwNumModules; i++) {
            SAFEDELETEARRAY(ppModules[i]);
        }
        SAFEDELETEARRAY(ppModules);
    }

    return hr;
}


// Creates an ASSEMBLYMETADATA struct for write.
STDAPI
AllocateAssemblyMetaData(ASSEMBLYMETADATA *pamd)
{
    HRESULT hr = S_OK;
    
    // Re/Allocate Locale array
    SAFEDELETEARRAY(pamd->szLocale);

    if (pamd->cbLocale) {
        IfNullGo(pamd->szLocale = NEW(WCHAR[pamd->cbLocale]));
    }

    // Re/Allocate Processor array
    SAFEDELETEARRAY(pamd->rProcessor);
    if (pamd->ulProcessor) {
        IfNullGo(pamd->rProcessor = NEW(DWORD[pamd->ulProcessor]));
    }

    // Re/Allocate OS array
    SAFEDELETEARRAY(pamd->rOS);
    if (pamd->ulOS) {
        IfNullGo(pamd->rOS = NEW(OSINFO[pamd->ulOS]));
    }

/*
    // Re/Allocate configuration
    if (pamd->szConfiguration)
        delete [] pamd->szConfiguration;
    pamd->szConfiguration = NEW(TCHAR[pamd->cbConfiguration = MAX_CLASS_NAME]);
    if (!pamd->szConfiguration)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
*/

ErrExit:
    if (FAILED(hr) && pamd)
        DeAllocateAssemblyMetaData(pamd);

    return hr;
}


STDAPI
DeAllocateAssemblyMetaData(ASSEMBLYMETADATA *pamd)
{
    // NOTE - do not 0 out counts
    // since struct may be reused.

    SAFEDELETEARRAY(pamd->szLocale);
    SAFEDELETEARRAY(pamd->rProcessor);
    SAFEDELETEARRAY(pamd->rOS);
/*
    if (pamd->szConfiguration)
    {
        delete [] pamd->szConfiguration;
        pamd->szConfiguration = NULL;
    }
*/
    return S_OK;
}


HRESULT TranslatePEToArchitectureType(CorPEKind CLRPeKind, DWORD dwImageType, PEKIND *PeKind)
{
    HRESULT     hr = S_OK;

    _ASSERTE(PeKind);

    *PeKind = peNone;

    if(CLRPeKind == peNot) 
    {        
        // Not a PE. Shouldn't ever get here.
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto Exit;
    }
    else 
    {
        if ((CLRPeKind & peILonly) && !(CLRPeKind & pe32Plus) &&
            !(CLRPeKind & pe32BitRequired) && dwImageType == IMAGE_FILE_MACHINE_I386) 
        {
            // Processor-agnostic (MSIL)
            *PeKind = peMSIL;
        }
        else if (CLRPeKind & pe32Plus) 
        {
            // 64-bit
            
            if (CLRPeKind & pe32BitRequired) 
            {
                // Invalid
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }

            // Regardless of whether ILONLY is set or not, the architecture
            // is the machine type.

            if(dwImageType == IMAGE_FILE_MACHINE_IA64) 
                *PeKind = peIA64;
            else if(dwImageType == IMAGE_FILE_MACHINE_AMD64) 
                *PeKind = peAMD64;
            else 
            {
                // We don't support other architectures
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }
        }
        else 
        {
            // 32-bit, non-agnostic

            if (dwImageType != IMAGE_FILE_MACHINE_I386) 
            {
                // Not supported
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }

            *PeKind = peI386;
        }
    }

Exit:
    return hr;
}

STDAPI GetAssemblyTextualIdentity(LPCWSTR pwzAssemblyPath, DWORD dwFlags, 
    __out_ecount(*pdwSize) LPWSTR pwzDisplayName, __inout LPDWORD pdwSize)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;
    IMetaDataAssemblyImport *pImport = NULL;
    DWORD dwImportFlags = ASM_IMPORT_NAME_ARCHITECTURE|ASM_IMPORT_NAME_PE_RUNTIME;
    DWORD dwDispFlags = ASM_DISPLAYF_FULL;

    if (!pwzAssemblyPath || !pdwSize || dwFlags) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = CreateMetaDataImport(pwzAssemblyPath, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetAssemblyNameDefFromMDImport(pImport, dwImportFlags, TRUE, &pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pName->GetDisplayName(pwzDisplayName, pdwSize, dwDispFlags);
    
Exit:
    SAFERELEASE(pName);
    SAFERELEASE(pImport);
    return hr;
} 

STDAPI GetCanonicalTextualIdentityFromStream(IStream *pStream, DWORD dwFlags,
    __out_ecount(*pccDisplayName) LPWSTR pwzDisplayName, __inout LPDWORD pccDisplayName)
{
    HRESULT                         hr = S_OK;
    IAssemblyName                   *pName = NULL;
    IMetaDataAssemblyImport         *pImport = NULL;
    DWORD                           dwImportFlags = ASM_IMPORT_NAME_ARCHITECTURE|ASM_IMPORT_NAME_PE_RUNTIME;
    DWORD                           dwDispFlags = ASM_DISPLAYF_FULL;

    if (!pStream || !pccDisplayName || dwFlags) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = CreateMetaDataImport(pStream, 0, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetAssemblyNameDefFromMDImport(pImport, dwImportFlags, TRUE, &pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pName->GetDisplayName(pwzDisplayName, pccDisplayName, dwDispFlags);
    if (FAILED(hr)) {
        goto Exit;
    }


    hr = CanonicalizeIdentity(pwzDisplayName);
    if (FAILED(hr)) {
        goto Exit;
    }
    
Exit:
    if (pImport) {
    }

    SAFERELEASE(pName);
    SAFERELEASE(pImport);

    return hr;
} 

HRESULT GetAssemblyRefTokens(IMetaDataAssemblyImport *pImport, mdAssembly **ppTokens, DWORD *pcTokens) {
    HRESULT     hr = S_OK;
    HCORENUM  hEnum = 0;  
    DWORD       cTokensMax = 0;
    mdAssembly  *rAssemblyRefTokens = NULL;
    DWORD        cAssemblyRefTokens = 0;
    mdAssembly  *rNewTokens = NULL;

    _ASSERTE(pImport);
    _ASSERTE(ppTokens);
    _ASSERTE(pcTokens);

    *ppTokens = NULL;
    *pcTokens = 0;
    
    rAssemblyRefTokens = NEW(mdAssemblyRef[ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE]);
    if (!rAssemblyRefTokens) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    rNewTokens = rAssemblyRefTokens;
    cAssemblyRefTokens = 0;
    
    // Attempt to get token array. If we have insufficient space
    // in the default array we will re-allocate it.
    while (1) {        
        cTokensMax = 0;
        hr = pImport->EnumAssemblyRefs(
            &hEnum, 
            rNewTokens, 
            ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE, 
            &cTokensMax);
        if (FAILED(hr)) {
            goto Exit;
        }
        _ASSERTE(cTokensMax >= 0 && cTokensMax <= ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE);
        cAssemblyRefTokens += cTokensMax;
        if (cTokensMax == ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE) {
            rNewTokens = NEW(mdAssemblyRef[cAssemblyRefTokens + ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE]);
            if (!rNewTokens) {
                hr = E_OUTOFMEMORY;
                goto Exit;
            }
            memcpy(rNewTokens, rAssemblyRefTokens, cAssemblyRefTokens*sizeof(mdAssemblyRef));
            SAFEDELETEARRAY(rAssemblyRefTokens);
            rAssemblyRefTokens = rNewTokens;
            rNewTokens = rAssemblyRefTokens + cAssemblyRefTokens;
        } else {
            break;
        }
    }

    *ppTokens = rAssemblyRefTokens;
    rAssemblyRefTokens = NULL;
    *pcTokens = cAssemblyRefTokens;
    hr = S_OK;
    
Exit:
    if (hEnum) 
        pImport->CloseEnum(hEnum);
    SAFEDELETEARRAY(rAssemblyRefTokens);    
    return hr;
}

