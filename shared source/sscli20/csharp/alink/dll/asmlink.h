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
// File: asmlink.h
//
// ===========================================================================

#ifndef __asmlink_h__
#define __asmlink_h__

#include "alink.h"
#include <ole2.h>
#include "assembly.h"

////////////////////////////////////////////////////////////////////////////////
// CAsmLink
//
// This class is the real assembly linker.

class CAsmLink :
    public CComObjectRoot,
    public CComCoClass<CAsmLink, &CLSID_AssemblyLinker>,
    public ISupportErrorInfoImpl< &IID_IALink>,
    public IALink2
{
public:
    BEGIN_COM_MAP(CAsmLink)
        COM_INTERFACE_ENTRY(IALink)
        COM_INTERFACE_ENTRY(IALink2)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    DECLARE_NO_REGISTRY( ) 
    CAsmLink();
    HRESULT Init();
    ~CAsmLink();

    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // Interface methods here
    STDMETHOD(Init)(IMetaDataDispenserEx *pDispenser, IMetaDataError *pError);
    STDMETHOD(SetAssemblyFile)(LPCWSTR pszFilename, IMetaDataEmit *pEmitter, AssemblyFlags afFlags, mdAssembly *pAssemblyID);
    STDMETHOD(SetAssemblyFile2)(LPCWSTR pszFilename, IMetaDataEmit2 *pEmitter, AssemblyFlags afFlags, mdAssembly *pAssemblyID);
    STDMETHOD(SetNonAssemblyFlags)(AssemblyFlags afFlags); 

    // Files and importing
    STDMETHOD(ImportFile)(LPCWSTR pszFilename, LPCWSTR pszTargetName, BOOL fSmartImport,
        mdToken *pImportToken, IMetaDataAssemblyImport **ppAssemblyScope, DWORD *pdwCountOfScopes);
    STDMETHOD(ImportFile2)(LPCWSTR pszFilename, LPCWSTR pszTargetName, IMetaDataAssemblyImport *pAssemblyScopeIn, BOOL fSmartImport,
        mdToken *pImportToken, IMetaDataAssemblyImport **ppAssemblyScope, DWORD *pdwCountOfScopes);
    STDMETHOD(ImportFileEx)(LPCWSTR pszFilename, LPCWSTR pszTargetName, BOOL fSmartImport,
        DWORD dwOpenFlags, mdToken *pImportToken, IMetaDataAssemblyImport **ppAssemblyScope, DWORD *pdwCountOfScopes);
    STDMETHOD(ImportFileEx2)(LPCWSTR pszFilename, LPCWSTR pszTargetName, IMetaDataAssemblyImport *pAssemblyScopeIn, BOOL fSmartImport,
        DWORD dwOpenFlags, mdToken *pImportToken, IMetaDataAssemblyImport **ppAssemblyScope, DWORD *pdwCountOfScopes);
    STDMETHOD(AddFile)(mdAssembly AssemblyID, LPCWSTR pszFilename, DWORD dwFlags,
        IMetaDataEmit *pEmitter, mdFile * pFileToken);
    STDMETHOD(AddFile2)(mdAssembly AssemblyID, LPCWSTR pszFilename, DWORD dwFlags,
        IMetaDataEmit2 *pEmitter, mdFile * pFileToken);
    STDMETHOD(AddImport)(mdAssembly mdAssemblyID, mdToken ImportToken, DWORD dwFlags, mdFile *pFileToken);
    STDMETHOD(GetScope)(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, IMetaDataImport** ppImportScope);
    STDMETHOD(GetScope2)(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, IMetaDataImport2** ppImportScope);
    STDMETHOD(GetAssemblyRefHash)(mdToken FileToken, const void** ppvHash, DWORD* pcbHash);
    STDMETHOD(GetPublicKeyToken)(LPCWSTR pszKeyFile, LPCWSTR pszKeyContainer, void * pvPublicKeyToken, DWORD * pcbPublicKeyToken);
    STDMETHOD(SetPEKind)(mdAssembly AssemblyID, mdToken FileToken, DWORD dwPEKind, DWORD dwMachine);


    STDMETHOD(ImportTypes)(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, HALINKENUM* phEnum,
        IMetaDataImport **ppImportScope, DWORD* pdwCountOfTypes);
    STDMETHOD(ImportTypes2)(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, HALINKENUM* phEnum,
        IMetaDataImport2 **ppImportScope, DWORD* pdwCountOfTypes);
    STDMETHOD(EnumCustomAttributes)(HALINKENUM hEnum, mdToken tkType, mdCustomAttribute rCustomValues[],
        ULONG cMax, ULONG *pcCustomValues);
    STDMETHOD(EnumImportTypes)(HALINKENUM hEnum, DWORD dwMax, mdTypeDef aTypeDefs[], DWORD* pdwCount); 
    STDMETHOD(CloseEnum)(HALINKENUM hEnum);
    
    // Exporting
    STDMETHOD(ExportType)(mdAssembly AssemblyID, mdToken FileToken, mdTypeDef TypeToken,
        LPCWSTR pszTypename, DWORD dwFlags, mdExportedType* pType); 
    STDMETHOD(ExportNestedType)(mdAssembly AssemblyID, mdToken FileToken, mdTypeDef TypeToken, 
        mdExportedType ParentType, LPCWSTR pszTypename, DWORD dwFlags, mdExportedType* pType); 
    STDMETHOD(EmitInternalExportedTypes)(mdAssembly AssemblyID);
    STDMETHOD(ExportTypeForwarder)(mdAssembly AssemblyID, LPCWSTR pszTypename, DWORD dwFlags, mdExportedType* pType); 
    STDMETHOD(ExportNestedTypeForwarder)(mdAssembly AssemblyID, mdToken FileToken, mdTypeDef TypeToken, 
        mdExportedType ParentType, LPCWSTR pszTypename, DWORD dwFlags, mdExportedType* pType); 

    
    // Resources
    STDMETHOD(EmbedResource)(mdAssembly AssemblyID, mdToken FileToken, LPCWSTR pszResourceName, DWORD dwOffset, DWORD dwFlags); 
    STDMETHOD(LinkResource)(mdAssembly AssemblyID, LPCWSTR pszFileName, LPCWSTR pszNewLocation, LPCWSTR pszResourceName, DWORD dwFlags); 
    
    STDMETHOD(GetFileDef)(mdAssembly AssemblyID, mdFile TargetFile, mdFile* pScope);
    STDMETHOD(GetResolutionScope)(mdAssembly AssemblyID, mdToken FileToken, mdToken TargetFile, mdToken* pScope); 
    STDMETHOD(GetWin32ResBlob)(mdAssembly AssemblyID, mdToken FileToken, BOOL fDll, LPCWSTR pszIconFile,
        const void **ppResBlob, DWORD *pcbResBlob);
    STDMETHOD(FreeWin32ResBlob)(const void **ppResBlob);

    
    // Custom Attributes and Assembly properties
    STDMETHOD(SetAssemblyProps)(mdAssembly AssemblyID, mdToken FileToken, AssemblyOptions Option, VARIANT Value); 
    STDMETHOD(EmitAssemblyCustomAttribute)(mdAssembly AssemblyID, mdToken FileToken, mdToken tkType, 
        void const* pCustomValue, DWORD cbCustomValue, BOOL bSecurity, BOOL bAllowMultiple); 
    STDMETHOD(EndMerge)(mdAssembly AssemblyID);
    STDMETHOD(EmitManifest)(mdAssembly AssemblyID, DWORD* pdwReserveSize, mdAssembly* ptkManifest);

    // Emit assembly to the MetaEmit interface
    STDMETHOD(EmitAssembly)(mdAssembly AssemblyID);
    
    // Finish everything off
    STDMETHOD(PreCloseAssembly)(mdAssembly AssemblyID);
    STDMETHOD(CloseAssembly)(mdAssembly AssemblyID); 

private:
    HRESULT FileNameTooLong(LPCWSTR filename);
    HRESULT GetScopeImpl(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, CFile ** ppFile);
    HRESULT ImportTypesImpl(mdAssembly AssemblyID, mdToken FileToken, DWORD dwScope, HALINKENUM* phEnum,
        IMetaDataImport **ppImportScope, IMetaDataImport2 **ppImportScope2, DWORD* pdwCountOfTypes);

    // our implementation
    IMetaDataDispenserEx *m_pDisp;
    IMetaDataError *m_pError;
    CAssembly *m_pAssem;
    CAssembly *m_pImports;
    CAssembly *m_pModules;
    CAssembly *m_pStdLib;
    bool    m_bInited           : 1;
    bool    m_bPreClosed        : 1;
    bool    m_bManifestEmitted  : 1;
    bool    m_bAssemblyEmitted  : 1;
    bool    m_bDontDoHashes     : 1;

protected:
    friend class CFile;
    CAssembly *GetStdLib();

};

#endif // __asmlink_h__
