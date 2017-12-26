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

#ifndef __IMPRTHELPERS_H__
#define __IMPRTHELPERS_H__

#include "fusionp.h"
#include "corpriv.h"

#define ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE 32

// This enum is use to determine which properties we need to copy
// out when we get assembly information from IMetaDataImport
typedef enum 
{
    ASM_IMPORT_NONE                 = 0x0,
    ASM_IMPORT_NAME_HASH_ALGID      = 0x1,
    ASM_IMPORT_NAME_HASH_VALUE      = 0x2,
    ASM_IMPORT_NAME_MVID            = 0x4,
    ASM_IMPORT_NAME_SIGNATURE_BLOB  = 0x8,
    ASM_IMPORT_NAME_PE_RUNTIME      = 0x10,
    ASM_IMPORT_NAME_ARCHITECTURE    = 0x20
}ASM_IMPORT_NAME_FLAGS;

inline HRESULT CreateMetaDataImport(
        LPCOLESTR pszFilename,
        MDInternalImportFlags dwFlags,
        IMetaDataAssemblyImport **ppImport)
{
    HRESULT hr = S_OK;

    hr = GetAssemblyMDInternalImportEx(pszFilename, IID_IMetaDataAssemblyImport,dwFlags, (IUnknown**)ppImport);
    if (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    return hr;
}

inline HRESULT CreateMetaDataImport(
        LPCOLESTR pszFilename, 
        IMetaDataAssemblyImport **ppImport)
{
    return CreateMetaDataImport(pszFilename, MDInternalImport_Default, ppImport);
}

inline HRESULT CreateMetaDataImport(
        IStream *pStream, 
        UINT64 AsmId,
        MDInternalImportFlags dwFlags,
        IMetaDataAssemblyImport **ppImport)
{
    return GetAssemblyMDInternalImportByStreamEx(pStream, 
                                               AsmId,
                                               IID_IMetaDataAssemblyImport, 
                                               dwFlags,
                                               (IUnknown **)ppImport);
}

inline HRESULT CreateMetaDataImport(
        IStream *pStream, 
        UINT64 AsmId,
        IMetaDataAssemblyImport **ppImport)
{
    return CreateMetaDataImport(pStream, AsmId, MDInternalImport_Default, ppImport);
}

STDAPI
GetAssemblyNameDefFromMDImport(IMetaDataAssemblyImport *pMDImport, 
                               DWORD dwFlags,
                               BOOL  bFinalize,
                               IAssemblyName **ppName);

STDAPI
GetAssemblyNameRefFromMDImport(IMetaDataAssemblyImport *pMDImport, 
                               mdAssemblyRef mdar, 
                               DWORD dwFlags,
                               IAssemblyName **ppName);

HRESULT 
GetAssemblyModuleListFromMDImport(IMetaDataAssemblyImport *pMDImport,
                                  __deref_out LPWSTR **pppwzModules,
                                  __out LPDWORD  pdwNumModules);

STDAPI DeAllocateAssemblyMetaData(ASSEMBLYMETADATA *pamd);
STDAPI AllocateAssemblyMetaData(ASSEMBLYMETADATA *pamd);

HRESULT TranslatePEToArchitectureType(CorPEKind CLRPeKind, DWORD dwImageType, PEKIND *PEKind);

HRESULT GetAssemblyRefTokens(IMetaDataAssemblyImport *pImport, mdAssembly **ppTokens, DWORD *pcTokens);

#endif
