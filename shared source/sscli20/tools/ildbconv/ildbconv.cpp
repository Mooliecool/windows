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
//*****************************************************************************
// File: ildbconv.cpp
//*****************************************************************************

#include "stdafx.h"
#include "..\..\palrt\inc\sscli_version.h"

// Revert the if'def in rotor_palrt.h so we can use the Windows version
// of CoCreateInstance.
#undef CoCreateInstance

// Globals
HINSTANCE  g_OLE32Dll = NULL; // ole32 Dll handle

// Array and count of document writers
DocumentEntry *g_ppDocumentWriter = NULL;
ULONG32 g_cDocuments = 0;
MapFile g_FileInput;

#ifndef CLSCTX_INPROC_SERVER
#define CLSCTX_INPROC_SERVER 0x01  // server dll (runs in same process as caller)
#endif

typedef HRESULT __stdcall FN_COINITIALIZE(LPVOID);
typedef void    __stdcall FN_COUNINITIALIZE(void);
typedef HRESULT __stdcall FN_COCREATEINSTANCE(REFCLSID rclsid,
                                              LPUNKNOWN pUnkOuter,
                                              DWORD dwClsContext,
                                              REFIID riid,
                                              LPVOID * ppv
                                             );

HRESULT CoInitialize(LPVOID pvReserved)
{
    HRESULT hr = NOERROR;    
    FN_COINITIALIZE * fnCoInitialize = (FN_COINITIALIZE*)GetProcAddress(g_OLE32Dll, "CoInitialize");
    
    if (fnCoInitialize == NULL) {
        goto Win32Error;
    }

    hr = (*fnCoInitialize)(pvReserved);
    return hr;
    
Win32Error:
    return HrLastError(); 
}

void CoUninitialize()
{
    FN_COUNINITIALIZE * fnCoUninitialize = (FN_COUNINITIALIZE*)GetProcAddress(g_OLE32Dll, "CoUninitialize");

    if (fnCoUninitialize != NULL) {
        (*fnCoUninitialize)();
    }
}

// Call into the palrt to CoCreate the clsid
HRESULT CoCreateInstance(REFCLSID   rclsid,
                         IUnknown*  pUnkOuter,
                         DWORD      dwClsContext,
                         REFIID     riid,
                         void     **ppv)
{
    HRESULT             hr = NOERROR;
    FN_COCREATEINSTANCE * fnCoCreateInstance = (FN_COCREATEINSTANCE*)GetProcAddress(g_OLE32Dll, "CoCreateInstance");

    if (fnCoCreateInstance == NULL) {
        goto Win32Error;
    }

    hr = (*fnCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    return hr;

Win32Error:
    return HrLastError();
}

//
// Find a matching document writer for the passed in document
//
// Note, this doesn't return an addref'd DocumentWriter
HRESULT FindDocumentWriter(
    ISymUnmanagedDocument *pDocument,
    ISymUnmanagedDocumentWriter **ppDocumentWriter)
{
    HRESULT hr = NOERROR;
    _ASSERTE(g_ppDocumentWriter);
    IfFalseGo(g_ppDocumentWriter, E_INVALIDARG);
    ULONG32 cUrlLength;
    WCHAR *wcsURL;

    // Init out parameter:
    *ppDocumentWriter = NULL;

    // Get the URL for the document
    IfFailGo(pDocument->GetURL(0, &cUrlLength, NULL));
    _ASSERTE(cUrlLength);

    if (cUrlLength)
    {
        unsigned UrlLengthDummy;
        unsigned i;
        wcsURL = (WCHAR *)_alloca((cUrlLength + 1) * sizeof(WCHAR));
        IfFailGo(pDocument->GetURL(cUrlLength+1, &UrlLengthDummy, wcsURL));
        _ASSERTE(UrlLengthDummy == cUrlLength);

        // Match the URL with one of the document writers we already loaded
        for (i = 0; i < g_cDocuments; i++)
        {
            if (wcscmp(g_ppDocumentWriter[i].wcsUrl, wcsURL) == 0)
            {
                *ppDocumentWriter = g_ppDocumentWriter[i].pDocumentWriter;
                break;
            }
        }
    }

Error:
    if (SUCCEEDED(hr) && *ppDocumentWriter == NULL)
    {
        hr = E_FAIL;
    }
    return hr;
}

//
// Map an RVA to a section
//
PIMAGE_SECTION_HEADER MapFile::ImageRvaToSection(ULONG Rva)
{
    ULONG i, dwStop;
    PIMAGE_SECTION_HEADER NtSection;

    if (m_bIs64) {
        NtSection = IMAGE_FIRST_SECTION( m_pNT64 );
        dwStop = m_pNT64->FileHeader.NumberOfSections;
    } else {
        NtSection = IMAGE_FIRST_SECTION( m_pNT32 );
        dwStop = m_pNT32->FileHeader.NumberOfSections;
    }
    for (i=0; i < dwStop; i++) {
        if (Rva >= NtSection->VirtualAddress &&
            Rva < NtSection->VirtualAddress + NtSection->SizeOfRawData)
            return NtSection;
        
        ++NtSection;
    }

    return NULL;
}

//
// Map an RVA to a Virtual address
//
PVOID MapFile::ImageRvaToVa(ULONG Rva)
{
    PIMAGE_SECTION_HEADER NtSection = ImageRvaToSection(Rva);

    if (NtSection != NULL) {
        return (PVOID)((PCHAR)m_pbMapAddress +
                       (Rva - NtSection->VirtualAddress) +
                       NtSection->PointerToRawData);
    }
    else
        return NULL;
}

HRESULT MapFile::FindHeaders()
{
    HRESULT hr = NOERROR;
    IMAGE_DOS_HEADER   *pDosHeader;

    if (m_pNT32 == NULL && m_pNT64 == NULL)
    {
        pDosHeader = (IMAGE_DOS_HEADER *) m_pbMapAddress;

        if ((pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) &&
            (pDosHeader->e_lfanew != 0))
        {
            m_pNT32 = (IMAGE_NT_HEADERS32*) (pDosHeader->e_lfanew + (BYTE*) pDosHeader);
            m_pNT64 = (IMAGE_NT_HEADERS64*)m_pNT32;

            // NOTE: at this point, it might be either a 32-bit or 64-bit image
            // but the headers are the same upto the Magic
            if ((m_pNT32->Signature != IMAGE_NT_SIGNATURE)) {
                m_pNT32 = NULL;
                IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
            }
            else if ((m_pNT32->FileHeader.SizeOfOptionalHeader ==
                IMAGE_SIZEOF_NT_OPTIONAL32_HEADER) &&
                (m_pNT32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)) {
                m_bIs64 = false;
            } 
            else if ((m_pNT64->FileHeader.SizeOfOptionalHeader ==
                IMAGE_SIZEOF_NT_OPTIONAL64_HEADER) &&
                (m_pNT64->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
                m_bIs64 = true;
            } else {
                m_pNT32 = NULL;
                IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
            }

        }
        else
        {
            IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
        }
    }

Error:
    return hr;
}
//
// Find the Cor Header
//
IMAGE_COR20_HEADER *MapFile::FindCorHeader()
{

    IMAGE_COR20_HEADER         *pICH = NULL;

    if (SUCCEEDED(FindHeaders()))
    {
        // Get the image header from the image, then get the directory location
        // of the COM+ header which may or may not be filled out.
        if (m_bIs64)
            pICH = (IMAGE_COR20_HEADER *) ImageRvaToVa(m_pNT64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress);
        else
            pICH = (IMAGE_COR20_HEADER *) ImageRvaToVa(m_pNT32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress);
    }

    return pICH;

}

IMAGE_DEBUG_DIRECTORY *MapFile::FindDebugEntry()
{
    IMAGE_DEBUG_DIRECTORY *pDebugDirectory = NULL;

    if (SUCCEEDED(FindHeaders()))
    {
        if (m_bIs64)
            pDebugDirectory = (IMAGE_DEBUG_DIRECTORY *)(ImageRvaToVa(m_pNT64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress));
        else
            pDebugDirectory = (IMAGE_DEBUG_DIRECTORY *)(ImageRvaToVa(m_pNT32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress));
    }

    return pDebugDirectory;
}

RSDSI * MapFile::FindDebugInfo()
{
    IMAGE_DEBUG_DIRECTORY *pDebugEntry = FindDebugEntry();
    if (pDebugEntry)
    {
        return (RSDSI *)(m_pbMapAddress + pDebugEntry->PointerToRawData);
    }
    return NULL;
}

HRESULT MapFile::FileMapping(
    HANDLE hFileMappingObject,   // handle to file-mapping object
    DWORD dwDesiredAccess,       // access mode
    SIZE_T dwNumberOfBytesToMap  // number of bytes to map
    )
{
    HRESULT hr = NOERROR;
    m_pbMapAddress = (BYTE*)MapViewOfFile(hFileMappingObject, dwDesiredAccess, 0, 0, dwNumberOfBytesToMap);
    if (!m_pbMapAddress ) {
        IfFailGo(HrLastError());
    }
Error:
    return hr;
}

//
//  Return the IMetaDataImport for the passed file.
//
HRESULT GetMetaFromFile(LPCWSTR wcsFileName, IMetaDataImport **ppMetaImport)
{
    IMAGE_COR20_HEADER         *pICH;
    HRESULT hr = NOERROR;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    DWORD dwLength;
    IMetaDataDispenserEx *pDisp = NULL;

    // Create a dispenser
    IfFailGo(CoCreateInstance(CLSID_CorMetaDataDispenser,
                              NULL,
                              CLSCTX_INPROC_SERVER, 
                              IID_IMetaDataDispenserEx,
                              (void **) &pDisp));

    // Open and map
    hFile = CreateFileW( wcsFileName, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        IfFailGo(HrLastError());
    }

    dwLength = GetFileSize( hFile, NULL);

    // Map it into memory
    hMap = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, dwLength, NULL);
    if (hMap == NULL) {
        IfFailGo(HrLastError());
    }

    IfFailGo(g_FileInput.FileMapping(hMap, FILE_MAP_READ, dwLength));

    // Find the COR Header
    pICH = g_FileInput.FindCorHeader();
    if (pICH == NULL)
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    // Open the scope on memory
    IfFailGo(pDisp->OpenScopeOnMemory(
            g_FileInput.ImageRvaToVa(pICH->MetaData.VirtualAddress),
            pICH->MetaData.Size,
            ofRead, 
            IID_IMetaDataImport, 
            (IUnknown **)ppMetaImport));

Error:
    RELEASE(pDisp);
    if (hMap)
    {
        CloseHandle(hMap);
    }
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    if (FAILED(hr))
    {
        return hr;
    }
    // Could return S_FALSE
    if (hr != S_OK)
    {
        return E_FAIL;
    }
    return hr;
}

//
// Transfer the Documents from the Reader to the Writer
//
HRESULT TransferDocuments(ISymUnmanagedReader *pSymReader, ISymUnmanagedWriter *pSymWriter)
{
    HRESULT hr = NOERROR;
    ULONG32 cDocs;
    unsigned i;

    ISymUnmanagedDocumentWriter *pDocumentWriter = NULL;

    ISymUnmanagedDocument **ppDocuments;

    // Get the count of documents
    IfFailGo(pSymReader->GetDocuments(0, &cDocs, NULL));
    if (cDocs)
    {
        // Allocate temp memory to read the documents
        ULONG32 cDocsDummy;
        ppDocuments = (ISymUnmanagedDocument **)_alloca(cDocs * sizeof(void *));

        // Allocate memory into a global to store the Document Writer interfaces
        g_ppDocumentWriter = (DocumentEntry *)malloc (cDocs * sizeof(DocumentEntry));
        IfNullGo(g_ppDocumentWriter);
        memset(g_ppDocumentWriter, 0, cDocs * sizeof(DocumentEntry));
        g_cDocuments = cDocs;

        memset(ppDocuments, 0, cDocs * sizeof(void *));
        IfFailGo(pSymReader->GetDocuments(cDocs, &cDocsDummy, ppDocuments));
        _ASSERTE(cDocs == cDocsDummy);

        // For each doc, get the document information and create a DocumentWriter
        for (i = 0; i < cDocs; i++)
        {
            unsigned UrlLength;
            WCHAR *wcsURL = NULL;
            unsigned CheckSumLength = 0;
            BYTE *CheckSum = NULL;
            GUID Language;
            GUID LanguageVendor;
            GUID DocumentType;
            GUID CheckSumId;

            // Get the Document information
            IfFailGo(ppDocuments[i]->GetURL(0, &UrlLength, NULL));
            if (UrlLength)
            {
                unsigned UrlLengthDummy;
                wcsURL = (WCHAR *)malloc((UrlLength + 1) * sizeof(WCHAR));
                IfNullGo(wcsURL);
                IfFailGo(ppDocuments[i]->GetURL(UrlLength+1, &UrlLengthDummy, wcsURL));
                _ASSERTE(UrlLengthDummy == UrlLength);
            }

            IfFailGo(ppDocuments[i]->GetLanguage(&Language));
            IfFailGo(ppDocuments[i]->GetLanguageVendor(&LanguageVendor));
            IfFailGo(ppDocuments[i]->GetDocumentType(&DocumentType));
            IfFailGo(ppDocuments[i]->GetCheckSumAlgorithmId(&CheckSumId));
            // It's possible that GetCheckSum return E_NOTIMPL
            ppDocuments[i]->GetCheckSum(0, &CheckSumLength, NULL);
            if (CheckSumLength)
            {
                unsigned CheckSumLengthDummy;
                CheckSum = (BYTE *)malloc(CheckSumLength+1);
                IfNullGo(CheckSum);
                IfFailGo(ppDocuments[i]->GetCheckSum(CheckSumLength, &CheckSumLengthDummy, CheckSum));
            }

            // Define a document in the writer
            IfFailGo(pSymWriter->DefineDocument(wcsURL,
                                                &Language,
                                                &LanguageVendor,
                                                &DocumentType,
                                                &pDocumentWriter));

            // Fill in Document writer information
            IfFailGo(pDocumentWriter->SetCheckSum(CheckSumId, CheckSumLength, CheckSum));
            g_ppDocumentWriter[i].wcsUrl = (WCHAR *)malloc ((wcslen(wcsURL) + 1) * sizeof(WCHAR));
            IfNullGo(g_ppDocumentWriter[i].wcsUrl);
            wcscpy(g_ppDocumentWriter[i].wcsUrl, wcsURL);
            g_ppDocumentWriter[i].pDocumentWriter = pDocumentWriter;
            pDocumentWriter = NULL;
            RELEASE(pDocumentWriter);
            if (wcsURL)
            {
                free(wcsURL);
                wcsURL = NULL;
            }
            if (CheckSum)
            {
                free(CheckSum);
                CheckSum = NULL;
            }
        }
    }

    // Fall through
Error:
    // Cleanup
    RELEASE(pDocumentWriter);
    if (ppDocuments)
    {
        for (i = 0; i < cDocs; i++)
        {
            RELEASE(ppDocuments[i]);
        }
    }
    return hr;
}

//
// Write out the pVariable information into the writer
//
HRESULT TransferVariable(
    ISymUnmanagedWriter *pSymWriter,
    ISymUnmanagedVariable *pVariable
)
{
    HRESULT hr;
    WCHAR *wcsName;
    ULONG32 cchName;
    ULONG32 Attributes;
    ULONG32 cSig, cSigDummy;
    ULONG32 startOffset = 0, endOffset = 0;
    BYTE *pSignature;
    ULONG32 addrKind, addr1, addr2 = 0 , addr3 = 0;

    // Get the information from the variable
    IfFailGo(pVariable->GetName(0, &cchName, NULL));
    wcsName = (WCHAR *)_alloca((cchName + 1) * sizeof (WCHAR));
    IfNullGo(wcsName);
    IfFailGo(pVariable->GetName(cchName, NULL, wcsName));
    IfFailGo(pVariable->GetSignature(0, &cSig, NULL));
    pSignature = (BYTE *)_alloca((cSig + 1) * sizeof (BYTE));
    IfNullGo(pSignature);
    IfFailGo(pVariable->GetSignature(cSig, &cSigDummy, pSignature));
    _ASSERTE(cSig == cSigDummy);
    IfFailGo(pVariable->GetAttributes(&Attributes));
    IfFailGo(pVariable->GetAddressKind(&addrKind));
    IfFailGo(pVariable->GetAddressField1(&addr1));

    // The Reader might not support these
    pVariable->GetAddressField2(&addr2);
    pVariable->GetAddressField3(&addr3);
    pVariable->GetStartOffset(&startOffset);
    pVariable->GetEndOffset(&endOffset);

    // Create the Variable in the SymWriter
    IfFailGo(pSymWriter->DefineLocalVariable(wcsName,
                                              Attributes,
                                              cSig,
                                              pSignature,
                                              addrKind,
                                              addr1, addr2, addr3,
                                              startOffset, endOffset));

Error:
    return hr;
}

//
// Write out the pNameSpace information into the writer
//
HRESULT TransferNameSpace(
    ISymUnmanagedWriter *pSymWriter,
    ISymUnmanagedNamespace *pNameSpace
)
{
    HRESULT hr;
    ULONG32 cNameLength, cNameLengthDummy;
    IfFailGo(pNameSpace->GetName(0, &cNameLength, NULL));
    _ASSERTE(cNameLength);
    if (cNameLength)
    {
        WCHAR *wszNameSpace = (WCHAR *)_alloca((cNameLength+1) * sizeof(WCHAR));
        IfNullGo(wszNameSpace);
        IfFailGo(pNameSpace->GetName(cNameLength+1, &cNameLengthDummy, wszNameSpace));
        _ASSERTE(cNameLengthDummy == cNameLength);
        IfFailGo(pSymWriter->UsingNamespace(wszNameSpace));
    }
Error:
    return hr;
}

//
// Transfer the pScope into the Writer
//
HRESULT TransferScope(
    ISymUnmanagedWriter *pSymWriter,
    ISymUnmanagedScope *pScope
)
{
    HRESULT hr;
    ISymUnmanagedVariable **ppVariables = NULL;
    ISymUnmanagedScope **ppScopes = NULL;
    ISymUnmanagedNamespace **ppNameSpaces = NULL;
    ULONG32 StartOffset;
    ULONG32 EndOffset;
    ULONG32 cNameSpaces, cNameSpacesDummy;
    ULONG32 cLocals, cLocalsDummy;
    ULONG32 cChildScopes, cDummyChildren;
    unsigned int uiMethodScope;
    unsigned i;

    IfFailGo(pScope->GetStartOffset(&StartOffset));
    IfFailGo(pScope->GetEndOffset(&EndOffset));
    IfFailGo(pSymWriter->OpenScope(StartOffset, &uiMethodScope));
    IfFailGo(pScope->GetLocals(0, &cLocals, NULL));

    if (cLocals)
    {
        ppVariables = (ISymUnmanagedVariable **) _alloca(cLocals * sizeof(void *));
        IfNullGo(ppVariables);
        memset(ppVariables, 0, cLocals * sizeof(void *));
        IfFailGo(pScope->GetLocals(cLocals, &cLocalsDummy, ppVariables));
        _ASSERTE(cLocals == cLocalsDummy);

        for (i = 0; i < cLocals; i++)
        {
            IfFailGo(TransferVariable(pSymWriter, ppVariables[i]));
        }
    }


    IfFailGo(pScope->GetNamespaces(0, &cNameSpaces, NULL));
    if (cNameSpaces)
    {
        ppNameSpaces = (ISymUnmanagedNamespace **) _alloca(cNameSpaces * sizeof(void *));
        IfNullGo(ppNameSpaces);
        memset(ppNameSpaces, 0, cNameSpaces * sizeof(void *));
        IfFailGo(pScope->GetNamespaces(cNameSpaces, &cNameSpacesDummy, ppNameSpaces));
        _ASSERTE(cNameSpacesDummy == cNameSpaces);
        for (i = 0; i < cNameSpaces; i++)
        {
            IfFailGo(TransferNameSpace(pSymWriter, ppNameSpaces[i]));
        }
    }

    IfFailGo(pScope->GetChildren(0, &cChildScopes, NULL));
    if (cChildScopes)
    {
        ppScopes = (ISymUnmanagedScope **) _alloca(cChildScopes * sizeof(void *));
        IfNullGo(ppScopes);
        memset(ppScopes, 0, cChildScopes * sizeof(void *));
        IfFailGo(pScope->GetChildren(cChildScopes, &cDummyChildren, ppScopes));
        _ASSERTE(cDummyChildren == cChildScopes);
        for (i = 0; i < cChildScopes; i++)
        {
            IfFailGo(TransferScope(pSymWriter, ppScopes[i]));
        }
    }

    IfFailGo(pSymWriter->CloseScope(EndOffset));    

Error:
    if (ppVariables)
    {
        for (i =0; i < cLocals; i++)
        {
            RELEASE(ppVariables[i]);
        }
    }
    if (ppScopes)
    {
        for (i =0; i < cChildScopes; i++)
        {
            RELEASE(ppScopes[i]);
        }
    }
    return hr;
}

//
// Transfer the sequence points for the method into the Writer
//
HRESULT TransferSequencePoints(
    ISymUnmanagedWriter *pSymWriter,
    ISymUnmanagedMethod *pMethod
)
{
    HRESULT hr;
    ULONG32 cSequencePoints, cSequencePointsDummy;
    unsigned i;
    ISymUnmanagedDocumentWriter *pDocumentWriter = NULL;
    ISymUnmanagedDocument **ppDocuments;

    IfFailGo(pMethod->GetSequencePointCount(&cSequencePoints));
    if (cSequencePoints)
    {
        ULONG32 *pStartLines = (ULONG32 *)_alloca(cSequencePoints * sizeof(ULONG32));
        ULONG32 *pEndLines = (ULONG32 *)_alloca(cSequencePoints * sizeof(ULONG32));
        ULONG32 *pStartColumns = (ULONG32 *)_alloca(cSequencePoints * sizeof(ULONG32));
        ULONG32 *pEndColumns = (ULONG32 *)_alloca(cSequencePoints * sizeof(ULONG32));
        ULONG32 *pOffsets = (ULONG32 *)_alloca(cSequencePoints * sizeof(ULONG32));
        IfNullGo(pStartLines);
        IfNullGo(pEndLines);
        IfNullGo(pStartColumns);
        IfNullGo(pEndColumns);
        IfNullGo(pOffsets);
    
        ppDocuments = (ISymUnmanagedDocument **)_alloca(cSequencePoints * sizeof(void *));
        IfNullGo(ppDocuments);
        memset(ppDocuments, 0, cSequencePoints * sizeof(void *));

        IfFailGo(pMethod->GetSequencePoints(cSequencePoints,
                                            &cSequencePointsDummy,
                                            pOffsets,
                                            ppDocuments,
                                            pStartLines,
                                            pStartColumns,
                                            pEndLines,
                                            pEndColumns));

        IfFailGo(FindDocumentWriter(ppDocuments[0], &pDocumentWriter));
        IfFailGo(pSymWriter->DefineSequencePoints(pDocumentWriter,
                                                  cSequencePoints,
                                                  pOffsets,
                                                  pStartLines,
                                                  pStartColumns,
                                                  pEndLines,
                                                  pEndColumns));
    }
Error:
    if (ppDocuments)
    {
        for (i = 0; i < cSequencePoints; i++)
        {
            RELEASE(ppDocuments[i]);
        }
    }
    return hr;
}

//
// Transfer the method into the writer
//
HRESULT TransferMethod(
    ISymUnmanagedReader *pSymReader,
    ISymUnmanagedWriter *pSymWriter,
    IMetaDataImport *pMetaImport,
    mdMethodDef MethodDef
)
{
    HRESULT hr;
    ISymUnmanagedMethod *pMethod = NULL;
    ISymUnmanagedScope *pScope = NULL;
    bool fOpened = false;

#if _DEBUG
    WCHAR szMethodName[MAX_PATH];
    IfFailGo(pMetaImport->GetMethodProps(MethodDef,
                                         NULL, // Class token
                                         szMethodName,
                                         MAX_PATH,
                                         NULL, NULL, NULL, NULL, NULL, NULL));
#endif

    if (SUCCEEDED(pSymReader->GetMethod(MethodDef, &pMethod)))
    {
        IfFailGo(pSymWriter->OpenMethod(MethodDef));
        fOpened = true;

        IfFailGo(pMethod->GetRootScope(&pScope));
        IfFailGo(TransferScope(pSymWriter, pScope));
        IfFailGo(TransferSequencePoints(pSymWriter, pMethod));
    }

Error:
    if (fOpened)
    {
        pSymWriter->CloseMethod();
    }
    RELEASE(pScope);
    RELEASE(pMethod);
    return hr;
}

//
// Transfer all the methods into the writer
//
HRESULT TransferMethods(
    ISymUnmanagedReader *pSymReader,
    ISymUnmanagedWriter *pSymWriter,
    IMetaDataImport *pMetaImport
)
{
    HRESULT hr = NOERROR;
    HCORENUM hEnumTypes = 0;
    HCORENUM hEnumMethods = 0;
    mdTypeDef typedefs[32];
    ULONG cTypedefs, iTypedef;

    do
    {
        // Get next batch of types.
        IfFailGo(pMetaImport->EnumTypeDefs(&hEnumTypes, typedefs, lengthof(typedefs), &cTypedefs));

        // Process each type.
        for (iTypedef = 0; iTypedef < cTypedefs; ++iTypedef) 
        {
#if _DEBUG
            mdToken BaseClass;
            WCHAR szTypeDef[MAX_PATH];
            IfFailGo(pMetaImport->GetTypeDefProps(typedefs[iTypedef],
                                                  szTypeDef,
                                                  MAX_PATH,
                                                  NULL, NULL,
                                                  &BaseClass));
#endif

            ULONG cMethodDefs;
            do {
                mdMethodDef methoddefs[32];
                ULONG iMethodDef;

                // Get next batch of types.
                IfFailGo(pMetaImport->EnumMethods(&hEnumMethods,
                                                  typedefs[iTypedef],
                                                  methoddefs,
                                                  lengthof(methoddefs),
                                                  &cMethodDefs));

                for (iMethodDef = 0; iMethodDef < cMethodDefs; iMethodDef++) 
                {
                    IfFailGo(TransferMethod(pSymReader, pSymWriter, pMetaImport, methoddefs[iMethodDef]));
                }

            } while (cMethodDefs > 0 && SUCCEEDED(hr));
            if (hEnumMethods)
            {
                pMetaImport->CloseEnum(hEnumMethods);
                hEnumMethods = NULL;
            }
        }
    } while (cTypedefs > 0 && SUCCEEDED(hr));

    // In case it's set to S_FALSE;
    hr = NOERROR;
Error:
    if (hEnumMethods)
    {
        pMetaImport->CloseEnum(hEnumMethods);
    }
    if (hEnumTypes)
    {
        pMetaImport->CloseEnum(hEnumTypes);
    }
    return hr;
}

//
// MapFromReaderToWriter
//
HRESULT MapFromReaderToWriter(
    ISymUnmanagedReader *pSymReader,
    ISymUnmanagedWriter *pSymWriter,
    IMetaDataImport *pMetaImport
)
{
  HRESULT hr;

  mdMethodDef mdEntryPoint;

  // Transfer the user entry point (might not be one if it's a DLL)
  if (SUCCEEDED(pSymReader->GetUserEntryPoint(&mdEntryPoint)))
  {
      IfFailGo(pSymWriter->SetUserEntryPoint(mdEntryPoint));
  }

  IfFailGo(TransferDocuments(pSymReader, pSymWriter));

  IfFailGo(TransferMethods(pSymReader, pSymWriter, pMetaImport));

Error:
  return hr;
}

HRESULT TrimDebugInfo(MapFile *pFile,
                      IMAGE_DEBUG_DIRECTORY *pNewDebugDirectory)
{

    HRESULT hr = NOERROR;

    IfFalseGo(pNewDebugDirectory, E_INVALIDARG);
    IfFalseGo(pFile, E_INVALIDARG);

    WCHAR wcsFileName[_MAX_FNAME],
          wcsExt[_MAX_EXT];
    WCHAR fullpath[_MAX_PATH];
    WCHAR wszDebugFileName[_MAX_PATH];
    DWORD dwUtf8Length;
    DWORD dwUnicodeLength;
    IMAGE_DEBUG_DIRECTORY *pDebugEntry;

    RSDSI *pDebugInfo;
    RSDSI *pNewDebugInfo;

    pNewDebugInfo = (RSDSI *)((BYTE *)pNewDebugDirectory+sizeof(IMAGE_DEBUG_DIRECTORY));

    pDebugEntry = pFile->FindDebugEntry();
    pDebugInfo = pFile->FindDebugInfo();

    if (pDebugInfo == NULL || pDebugInfo->dwSig != 0x53445352 || 
        pNewDebugInfo->dwSig != 0x53445352) // "SDSR"
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    // Try the returned Stored Name since it might be a fully qualified path
    dwUtf8Length = pNewDebugDirectory->SizeOfData - sizeof(RSDSI);
    dwUnicodeLength = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) pNewDebugInfo->szPdb, dwUtf8Length, fullpath, _MAX_PATH);
    _wsplitpath( fullpath, NULL, NULL, wcsFileName, wcsExt );
    _wmakepath( wszDebugFileName, NULL, NULL, wcsFileName, wcsExt );

    dwUtf8Length = UnicodeToUTF8( wszDebugFileName, NULL, 0 );
    if (dwUtf8Length <= pDebugEntry->SizeOfData - sizeof(RSDSI))
    {
        // There's room for the name
        UnicodeToUTF8( wszDebugFileName, pDebugInfo->szPdb, dwUtf8Length );
    }
    else
    {
        // There should always be room
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }
    pDebugInfo->guidSig = pNewDebugInfo->guidSig;
    pDebugInfo->age = pNewDebugInfo->age;

Error:
    return hr;

}
//
// ConvertFromIldbToPdb
//
HRESULT ConvertFromIldbToPdb(char *szExeFileName)
{
    HRESULT hr;
    IMetaDataImport *pMetaImport = NULL;
    ISymUnmanagedWriter *pSymWriter = NULL;
    ISymUnmanagedBinder *pSymBinder = NULL;
    ISymUnmanagedReader *pSymReader = NULL;
    int cchWideLength;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    IMAGE_DEBUG_DIRECTORY *pDebugDirectory;
    IMAGE_DEBUG_DIRECTORY *pNewDebugDirectory;
    BYTE *pbMapAddressWrite = NULL;
    DWORD cDataSize;
    BYTE *pData;
    DWORD dwLength;
    MapFile FileExe;

    WCHAR *wcsExeFileName;
    WCHAR *wcsILDBFileName;
    WCHAR wcsFileName[_MAX_FNAME],
          wcsExt[_MAX_EXT],
          wcsDir[_MAX_DIR],
          wcsDrive[_MAX_DRIVE];

    cchWideLength = MultiByteToWideChar(CP_ACP, 0, szExeFileName, -1, NULL, 0);
    wcsExeFileName = (WCHAR *)_alloca(cchWideLength * sizeof (WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szExeFileName, -1, wcsExeFileName, cchWideLength);

    _wsplitpath(wcsExeFileName, wcsDrive, wcsDir, wcsFileName, wcsExt);

    // .ildb + Null terminator
    size_t OutputFileNameLength = wcslen(wcsDrive) + 
                                  wcslen(wcsDir) +
                                  wcslen(wcsFileName)
                                  + 6;

    wcsILDBFileName = (WCHAR *)_alloca(OutputFileNameLength * sizeof (WCHAR));
    wcscpy(wcsILDBFileName, wcsDrive);
    wcscat(wcsILDBFileName, wcsDir);
    wcscat(wcsILDBFileName, wcsFileName);
    wcscat(wcsILDBFileName, L".ildb");

    IfFailGo(GetMetaFromFile(wcsExeFileName, &pMetaImport));

    IfFailGo(CoCreateInstance(CLSID_CorSymWriter_SxS,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_ISymUnmanagedWriter,
                              (void**)&pSymWriter));

    IfFailGo(pSymWriter->Initialize(pMetaImport,
                                    wcsILDBFileName, 
                                    NULL,
                                    true));  

    IfFailGo(PAL_CoCreateInstance(CLSID_CorSymBinder_SxS, IID_ISymUnmanagedBinder, (void **)&pSymBinder));
    IfFailGo(pSymBinder->GetReaderForFile(pMetaImport, wcsExeFileName, NULL, &pSymReader));

    IfFailGo(MapFromReaderToWriter(pSymReader, pSymWriter, pMetaImport));

    // If we're converting from ildb to pdb, we also need to update the debug info
    // in the exe
    RELEASE(pMetaImport);

    // Open and map
    hFile = CreateFileW( wcsExeFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        IfFailGo(HrLastError());
    }

    dwLength = GetFileSize( hFile, NULL);

    // Map it into memory
    hMap = CreateFileMappingA( hFile, NULL, PAGE_READWRITE, 0, dwLength, NULL);
    if (hMap == NULL) {
        IfFailGo(HrLastError());
    }

    IfFailGo(FileExe.FileMapping(hMap, FILE_MAP_WRITE, dwLength));

    IfFailGo(pSymWriter->GetDebugInfo(NULL, 0, &cDataSize, NULL));
      
    pNewDebugDirectory = (IMAGE_DEBUG_DIRECTORY *)malloc(sizeof(IMAGE_DEBUG_DIRECTORY) + cDataSize);
    pData = (BYTE *)pNewDebugDirectory + sizeof(IMAGE_DEBUG_DIRECTORY);

    IfFailGo(pSymWriter->GetDebugInfo(pNewDebugDirectory, cDataSize, &cDataSize, pData));

    pDebugDirectory = FileExe.FindDebugEntry();
    if (pDebugDirectory == NULL)
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    IfFailGo(TrimDebugInfo(&FileExe, pNewDebugDirectory));
    pDebugDirectory->Type = pNewDebugDirectory->Type;

    pSymWriter->Close();

Error:

    if (hMap)
    {
        CloseHandle(hMap);
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    RELEASE(pMetaImport);
    RELEASE(pSymBinder);
    RELEASE(pSymReader);
    RELEASE(pSymWriter);
    return hr;
}

//
// ConvertFromPdbToIldb
//
HRESULT ConvertFromPdbToIldb(char *szExeFileName)
{
    HRESULT hr;
    IMetaDataImport *pMetaImport = NULL;
    ISymUnmanagedWriter *pSymWriter = NULL;
    ISymUnmanagedBinder *pSymBinder = NULL;
    ISymUnmanagedReader *pSymReader = NULL;
    int cchWideLength;

    WCHAR *wcsExeFileName;
    WCHAR *wcsPdbFileName;
    WCHAR wcsFileName[_MAX_FNAME],
          wcsExt[_MAX_EXT],
          wcsDir[_MAX_DIR],
          wcsDrive[_MAX_DRIVE];

    cchWideLength = MultiByteToWideChar(CP_ACP, 0, szExeFileName, -1, NULL, 0);
    wcsExeFileName = (WCHAR *)_alloca(cchWideLength * sizeof (WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szExeFileName, -1, wcsExeFileName, cchWideLength);

    _wsplitpath(wcsExeFileName, wcsDrive, wcsDir, wcsFileName, wcsExt);

    // .pdb + Null terminator
    size_t OutputFileNameLength = wcslen(wcsDrive) + 
                                  wcslen(wcsDir) +
                                  wcslen(wcsFileName)
                                  + 5;

    wcsPdbFileName = (WCHAR *)_alloca(OutputFileNameLength * sizeof (WCHAR));
    wcscpy(wcsPdbFileName, wcsDrive);
    wcscat(wcsPdbFileName, wcsDir);
    wcscat(wcsPdbFileName, wcsFileName);
    wcscat(wcsPdbFileName, L".pdb");

    IfFailGo(GetMetaFromFile(wcsExeFileName, &pMetaImport));

    IfFailGo(CoCreateInstance(CLSID_CorSymBinder_SxS,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_ISymUnmanagedBinder,
                              (void**)&pSymBinder));

    IfFailGo(pSymBinder->GetReaderForFile(pMetaImport, wcsExeFileName, NULL, &pSymReader));

    IfFailGo(PAL_CoCreateInstance(CLSID_CorSymWriter_SxS, IID_ISymUnmanagedWriter, (void **)&pSymWriter));

    IfFailGo(pSymWriter->Initialize(pMetaImport,
                                    wcsExeFileName, 
                                    NULL,
                                    true));  

    IfFailGo(MapFromReaderToWriter(pSymReader, pSymWriter, pMetaImport));

    pSymWriter->Close();

Error:
    RELEASE(pMetaImport);
    RELEASE(pSymBinder);
    RELEASE(pSymReader);
    RELEASE(pSymWriter);
    return hr;
}

//
//  Print out the usage information
//
void Usage()
{
    printf("Microsoft (R) Shared Source CLI Debug Information converter.  Version %s\n", SSCLI_VERSION_STR);
    printf("Copyright (C) Microsoft Corporation 1998-2002. All rights reserved.\n");
    printf("\n");
    printf("Usage: ildbconv [options]  <assembly>\n");
    printf("\n");
    printf("Options:\n");
    printf("     /topdb will create a pdb from the associated ildb file\n");
    printf("     /toildb will create an ildb from the associated pdb file (default)\n");
    printf("\n");
}

//
//  Main entry point
//
int __cdecl
main(int argc, char* argv[])
{
    HRESULT hr = NOERROR;

    g_OLE32Dll = LoadLibraryW(L"ole32.dll");
    if (g_OLE32Dll == NULL) {
        printf("Failed to load ole32.dll library.\n");
        return 1;
    }
    
    CoInitialize(NULL);

    // Make sure there's the correct number of arguments
    if (argc == 3 && (argv[1][0]=='-' || argv[1][0]=='/'))
    {
        if (_stricmp(&argv[1][1], "topdb") == 0)
        {
            IfFailGo(ConvertFromIldbToPdb(argv[2]));
        }
        else if (_stricmp(&argv[1][1], "toildb") == 0)
        {
            IfFailGo(ConvertFromPdbToIldb(argv[2]));
        }
        else
        {
            Usage();
        }
    }
    else if (argc == 2)
    {
        if ((argv[1][0] == '/' || argv[1][0] == '-') &&
            (argv[1][1] == '?'))
        {
            Usage();
        }
        else
        {
            // If there's only 1 additional argument assume it's
            // the filename and we want to convert to ILDB
            IfFailGo(ConvertFromPdbToIldb(argv[1]));
        }
    }
    else
    {
        Usage();
    }

    // Fall through
Error:

    // Cleanup
    if (g_OLE32Dll)
    {
        FreeLibrary(g_OLE32Dll);
    }

    if (g_ppDocumentWriter)
    {
        unsigned i;
        for (i = 0; i < g_cDocuments; i++)
        {
            RELEASE(g_ppDocumentWriter[i].pDocumentWriter);
            free(g_ppDocumentWriter[i].wcsUrl);
        }
        free(g_ppDocumentWriter);
    }

    CoUninitialize();

    if (FAILED(hr))
    {
        IErrorInfo *pErrorInfo;
        printf("Error 0x%X: ", hr);
        if (GetErrorInfo(0, &pErrorInfo) == S_OK)
        {    
            BSTR ErrorString;
            if (pErrorInfo->GetDescription(&ErrorString) == S_OK)
            {
                printf("%S\n", ErrorString);
                SysFreeString(ErrorString);
                hr = NOERROR;
            }
        }
        else
        {
            WCHAR wszDescription[1024];
            // Get the description from the system
            wszDescription[0] = '\0';
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
            hr, 0, wszDescription, sizeof(wszDescription) / sizeof(WCHAR), NULL);
            // If the system has a string for this error
            if (*wszDescription != '\0')
            {
                printf("%S\n", wszDescription);
                hr = NOERROR;
            }
        }

        // Make sure something was printed
        if (FAILED(hr))
        {
            printf("Terminating\n", hr);
        }
        return 1;
    }

    return 0;
}
