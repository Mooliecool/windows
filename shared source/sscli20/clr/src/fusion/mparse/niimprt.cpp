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
#include "niimprt.h"
#include "util.h"
#include "corcompile.h"

extern const WCHAR g_szDotDLL[];

HRESULT
CreateNativeImageManifestImport(
        LPCWSTR pwzNIManifestFilePath,
        CNativeImageManifestImport **ppImport)
{
    HRESULT hr = S_OK;
    CNativeImageManifestImport *pImport = NULL;

    IfNullGo(pImport = NEW(CNativeImageManifestImport));

    IfFailGo(pImport->Init(pwzNIManifestFilePath));

    *ppImport = pImport;

ErrExit:
    if (FAILED(hr)) {
        SAFEDELETE(pImport);
    }

    return hr;
}

HRESULT
CreateNativeImageManifestImport(
        LPCWSTR pwzNIManifestFilePath,
        IMetaDataAssemblyImport *pMDImport,
        CNativeImageManifestImport **ppImport)
{
    HRESULT hr = S_OK;
    CNativeImageManifestImport *pImport = NULL;

    IfNullGo(pImport = NEW(CNativeImageManifestImport));
    IfFailGo(pImport->Init(pwzNIManifestFilePath,pMDImport));

    *ppImport = pImport;

ErrExit:
    if (FAILED(hr)) {
        SAFEDELETE(pImport);
    }
    return hr;
}

CNativeImageManifestImport::CNativeImageManifestImport()
{
    _dwSig = 0x4954414e; // "ITAN"
    _pMDImport = NULL;
    _pNIInstallInfo = NULL;
    _pNgenEntry = NULL;
    _pos = NULL;
    _pwzConfigString = NULL;
    _dwConfigMask = 0;
}

CNativeImageManifestImport::~CNativeImageManifestImport()
{
    SAFERELEASE(_pMDImport);
    SAFERELEASE(_pNIInstallInfo);
    SAFERELEASE(_pNgenEntry);

    SAFEDELETEARRAY(_pwzManifestFilePath);
    SAFEDELETEARRAY(_pwzConfigString);

    LISTNODE pos = NULL;
    CNgenDependentEntry *pDepEntry = NULL;

    pos = _listDeps.GetHeadPosition();
    while (pos) {
        pDepEntry = _listDeps.GetNext(pos);
        SAFERELEASE(pDepEntry);
    }

    _listDeps.RemoveAll();
}


HRESULT CNativeImageManifestImport::Init(LPCWSTR pwzNIManifestFilePath)
{
    HRESULT hr = S_OK;
  
    IfNullGo(_pwzManifestFilePath = WSTRDupDynamic(pwzNIManifestFilePath));
    IfFailGo(CreateMetaDataImport(pwzNIManifestFilePath, &_pMDImport));
    IfFailGo(_pMDImport->QueryInterface(IID_INativeImageInstallInfo, (void **)&_pNIInstallInfo));
    
    IfFailGo(CopyProperties());

    IfFailGo(CopyDependencies());

    _pos = _listDeps.GetHeadPosition();
    
ErrExit:
    return hr; 
}

HRESULT CNativeImageManifestImport::Init(LPCWSTR pwzNIManifestFilePath, IMetaDataAssemblyImport *pMDImport)
{
    HRESULT hr = S_OK;
    _ASSERTE(pMDImport);

    IfNullGo(_pwzManifestFilePath = WSTRDupDynamic(pwzNIManifestFilePath));

    _pMDImport = pMDImport;
    _pMDImport->AddRef();

    IfFailGo(_pMDImport->QueryInterface(IID_INativeImageInstallInfo, (void **)&_pNIInstallInfo));

    IfFailGo(CopyProperties());

    IfFailGo(CopyDependencies());

    _pos = _listDeps.GetHeadPosition();

ErrExit:
    return hr;
}

HRESULT CNativeImageManifestImport::CopyProperties()
{
    HRESULT hr = S_OK;
    DWORD   dwSize;
    DWORD   dwConfigLen = 0;
    LPBYTE  pbEvaluationData = NULL; 
    DWORD   dwEvaDataSize = 0;
    IAssemblyName *pName = NULL;
    BOOL    isDll = FALSE;
    CORCOMPILE_NGEN_SIGNATURE ngenSign;
    CORCOMPILE_ASSEMBLY_SIGNATURE ilSign;
    CILEntry *pILEntry = NULL;
    IAssemblyName *pNameCopy = NULL;

    LPWSTR  pwzTmp = NULL;
    
    _ASSERTE(_pMDImport &&_pNIInstallInfo);

    // determine if the native image is a dll or exe.
    _ASSERT(_pwzManifestFilePath);
    pwzTmp = PathFindExtensionW(_pwzManifestFilePath);
    if(pwzTmp && !FusionCompareStringI(pwzTmp, g_szDotDLL)) {
        isDll = TRUE;
    }
    
    IfFailGo(_pNIInstallInfo->GetSignature(&ngenSign));
    
    IfFailGo(_pNIInstallInfo->GetILSignature(&ilSign));

    _pNIInstallInfo->GetConfigString(NULL, &dwConfigLen);
    _ASSERTE(dwConfigLen);
    
    IfNullGo(_pwzConfigString = NEW(WCHAR[dwConfigLen]));
    
    IfFailGo(_pNIInstallInfo->GetConfigString(_pwzConfigString, &dwConfigLen));

    IfFailGo(_pNIInstallInfo->GetConfigMask(&_dwConfigMask));

    _pNIInstallInfo->GetEvaluationDataToCache(NULL, &dwEvaDataSize);
    // we may encounter no data case
    if (dwEvaDataSize) {
        IfNullGo(pbEvaluationData = NEW(BYTE[dwEvaDataSize]));
        
        IfFailGo(_pNIInstallInfo->GetEvaluationDataToCache(pbEvaluationData, &dwEvaDataSize));
    }

    IfFailGo(GetAssemblyNameDefFromMDImport(_pMDImport, ASM_IMPORT_NONE, FALSE, &pName));
    
    IfFailGo(pName->Clone(&pNameCopy));

    IfFailGo(pName->SetProperty(ASM_NAME_MVID, (LPVOID)&ngenSign, sizeof(ngenSign)));
    
    IfFailGo(pName->SetProperty(ASM_NAME_CUSTOM, _pwzConfigString, dwConfigLen*sizeof(WCHAR)));
    
    IfFailGo(pName->SetProperty(ASM_NAME_CONFIG_MASK, (LPVOID)&_dwConfigMask, sizeof(DWORD)));

    IfFailGo(pNameCopy->SetProperty(ASM_NAME_MVID, (LPBYTE)&(ilSign.mvid), sizeof(GUID)));

    IfFailGo(pNameCopy->SetProperty(ASM_NAME_SIGNATURE_BLOB, ilSign.rgbSNHash, ilSign.wcbSNHash));

    IfFailGo(CILEntry::Create(pNameCopy, isDll, &pILEntry));
    
    if (CAssemblyName::IsSystem(pNameCopy)) {
        pILEntry->SetIsSystem();
    }

    IfFailGo(CNgenEntry::Create(pName, pbEvaluationData, dwEvaDataSize, &_pNgenEntry));
   
    _pNgenEntry->SetILEntry(pILEntry);

    // we need to know all the modules
    _pNgenEntry->CheckExistsInGAC();
    if (_pNgenEntry->IsInGAC()) {
        PEKIND pe = MapCacheTypeToPEKIND(pILEntry->GetCacheType());
        IfFailGo(pNameCopy->SetProperty(ASM_NAME_ARCHITECTURE, &pe, sizeof(pe)));
        IfFailGo(_pNgenEntry->PopulateModules());
    }

ErrExit:
    SAFEDELETEARRAY(pbEvaluationData);
    SAFERELEASE(pName);
    SAFERELEASE(pNameCopy);
    SAFERELEASE(pILEntry);

    return hr;
}

HRESULT CNativeImageManifestImport::CopyDependencies()
{
    HRESULT     hr = S_OK;
    HCORENUM    hEnum = NULL;
    INativeImageDependency* rAssemblyDeps[ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE];
    DWORD  cAssemblyDeps = ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE;
    CNgenDependentEntry *pDepEntry = NULL;
    CNgenEntry *pNgenEntry = NULL;
    CILEntry   *pILEntry = NULL;
    mdAssemblyRef mdRef = 0;
    mdAssemblyRef mdDef = 0;
    IAssemblyName *pNameRef = NULL;
    IAssemblyName *pNameDef = NULL;
    IAssemblyName *pNameSelf = NULL;
    IAssemblyName *pNameCopy = NULL;
    CORCOMPILE_NGEN_SIGNATURE ngenSign;
    CORCOMPILE_ASSEMBLY_SIGNATURE ilSign;
    DWORD i = 0;
    WORD    wVers[4];
    DWORD   dwSize;

    IfFailGo(_pNgenEntry->GetNameDef(&pNameSelf));

    do{
        IfFailGo(_pNIInstallInfo->EnumDependencies(
                &hEnum,
                rAssemblyDeps,
                ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE,
                &cAssemblyDeps));
        
        for (i = 0; i< cAssemblyDeps; i++) {
            // Get ref
            IfFailGo(rAssemblyDeps[i]->GetILAssemblyRef(&mdRef));
            _ASSERTE(mdRef);
            
            IfFailGo(GetAssemblyNameRefFromMDImport(_pMDImport, mdRef, 0, &pNameRef));

            dwSize = sizeof(WORD);
            IfFailGo(pNameRef->GetProperty(ASM_NAME_MAJOR_VERSION, &wVers[0], &dwSize));

            dwSize = sizeof(WORD);
            IfFailGo(pNameRef->GetProperty(ASM_NAME_MINOR_VERSION, &wVers[1], &dwSize));

            dwSize = sizeof(WORD);
            IfFailGo(pNameRef->GetProperty(ASM_NAME_BUILD_NUMBER, &wVers[2], &dwSize));

            dwSize = sizeof(WORD);
            IfFailGo(pNameRef->GetProperty(ASM_NAME_REVISION_NUMBER, &wVers[3], &dwSize));

            // Get def
            IfFailGo(rAssemblyDeps[i]->GetNativeAssemblyDef(&ngenSign));
            if (ngenSign != INVALID_NGEN_SIGNATURE) { // hard dependency
                IfFailGo(rAssemblyDeps[i]->GetILAssemblyDef(&mdDef, &ilSign));
                _ASSERTE(mdDef);
                IfFailGo(GetAssemblyNameRefFromMDImport(_pMDImport, mdDef, 0, &pNameDef));
                IfFailGo(pNameDef->Clone(&pNameCopy));
                // hard dependency will have same config string as its parent
                IfFailGo(pNameDef->SetProperty(ASM_NAME_CUSTOM, _pwzConfigString, (lstrlenW(_pwzConfigString) + 1 )* sizeof(WCHAR)));
                IfFailGo(pNameDef->SetProperty(ASM_NAME_CONFIG_MASK, (LPVOID)&_dwConfigMask, sizeof(DWORD)));
                C_ASSERT(sizeof(CORCOMPILE_NGEN_SIGNATURE) == sizeof(GUID));
                IfFailGo(pNameDef->SetProperty(ASM_NAME_MVID, (LPBYTE)&ngenSign, sizeof(CORCOMPILE_NGEN_SIGNATURE)));

                IfFailGo(pNameCopy->SetProperty(ASM_NAME_MVID, (LPBYTE)&(ilSign.mvid), sizeof(GUID)));
                IfFailGo(pNameCopy->SetProperty(ASM_NAME_SIGNATURE_BLOB, ilSign.rgbSNHash, ilSign.wcbSNHash));

                IfFailGo(CILEntry::Create(pNameCopy, TRUE, &pILEntry));
                
                if (CAssemblyName::IsSystem(pNameCopy)) {
                    pILEntry->SetIsSystem();
                }

                IfFailGo(CNgenEntry::Create(pNameDef, NULL, 0, &pNgenEntry));

                pNgenEntry->SetILEntry(pILEntry);
                
                IfFailGo(CNgenDependentEntry::Create(wVers, pNgenEntry, &pDepEntry));

                SAFERELEASE(pILEntry);
                SAFERELEASE(pNameCopy);
            }
            else { // soft dependency
                IfFailGo(rAssemblyDeps[i]->GetILAssemblyDef(&mdDef, &ilSign));
                if (mdDef == mdAssemblyRefNil) {
                    _pNgenEntry->SetDepMissing();
                    
                    IfFailGo(pNameRef->Clone(&pNameDef));

                    IfFailGo(CILEntry::Create(pNameDef, TRUE, &pILEntry));

                    pILEntry->SetDefMissing();

                }
                else {
                    IfFailGo(GetAssemblyNameRefFromMDImport(_pMDImport, mdDef, 0, &pNameDef));
                    
                    IfFailGo(pNameSelf->IsEqual(pNameDef, ASM_CMPF_NAME|ASM_CMPF_VERSION|ASM_CMPF_PUBLIC_KEY_TOKEN|ASM_CMPF_CULTURE));
                    if (hr == S_OK) {
                        SAFERELEASE(pNameRef);
                        SAFERELEASE(pNameDef);
                        continue;
                    }

                    IfFailGo(pNameDef->SetProperty(ASM_NAME_MVID, &(ilSign.mvid), sizeof(GUID)));                
                    IfFailGo(pNameDef->SetProperty(ASM_NAME_SIGNATURE_BLOB, &(ilSign.rgbSNHash), ilSign.wcbSNHash));

                    IfFailGo(CILEntry::Create(pNameDef, TRUE, &pILEntry));
                    
                    pILEntry->CheckExistsInGAC();
                    if (pILEntry->IsInGAC()) {
                        PEKIND pe = MapCacheTypeToPEKIND(pILEntry->GetCacheType());
                        
                        IfFailGo(pNameDef->SetProperty(ASM_NAME_ARCHITECTURE, &pe, sizeof(pe)));

                        IfFailGo(pILEntry->PopulateModules());
                    }

                    if (CAssemblyName::IsSystem(pNameDef)) {
                        pILEntry->SetIsSystem();
                    }
                }
                    
                IfFailGo(CNgenDependentEntry::Create(wVers, pILEntry, &pDepEntry));
            }

            if (pDepEntry) {
                IfNullGo(_listDeps.AddHead(pDepEntry));
            }

            // prepare for the next dependency
            SAFERELEASE(pNameRef);
            SAFERELEASE(pNameDef);
            SAFERELEASE(pNgenEntry);
            SAFERELEASE(pILEntry);
            pDepEntry = NULL;
        }
    }while(hEnum);
    
ErrExit:
    SAFERELEASE(pNameRef);
    SAFERELEASE(pNameDef);
    SAFERELEASE(pNameSelf);
    SAFERELEASE(pNameCopy);
    SAFERELEASE(pNgenEntry);
    SAFERELEASE(pILEntry);
    SAFERELEASE(pDepEntry);

    return hr;
}   
    
HRESULT CNativeImageManifestImport::GetAssemblyNameDef(IAssemblyName **ppName)
{
    _ASSERTE(_pNgenEntry && ppName);
    if (!ppName) {
       return E_INVALIDARG;
    }

    return _pNgenEntry->GetNameDef(ppName);
}

HRESULT CNativeImageManifestImport::GetNgenEntry(CNgenEntry **ppNgenEntry)
{
    _ASSERTE(_pNgenEntry && ppNgenEntry);

    if (!ppNgenEntry) {
        return E_INVALIDARG;
    }

    *ppNgenEntry = _pNgenEntry;
    _pNgenEntry->AddRef();

    return S_OK;
}

HRESULT CNativeImageManifestImport::GetNextDependentAssembly(CNgenDependentEntry **ppDepEntry)
{
    _ASSERTE(ppDepEntry);
    
    if (!ppDepEntry) {
        return E_INVALIDARG;
    }

    if (_pos == NULL) {
        return S_FALSE;
    }

    *ppDepEntry = _listDeps.GetNext(_pos);
    _ASSERTE(*ppDepEntry);
    (*ppDepEntry)->AddRef();

    return S_OK;
}

        

    

