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
// File: symread.cpp
// 
// Note: The various SymReader_*, SymDocument_*, SymMethod_*, SymScope_*,
// SymVariable_* and SymNamespace_& are entry points
// called via PInvoke from the managed symbol wrapper used by managed languages
// to emit debug information (such as jscript)
// ===========================================================================
#include "pch.h"
#include "symread.h"
#include "corimage.h"

#define CODE_WITH_NO_SOURCE 0xfeefee
// ------------------------------------------------------------------------- 
// SymReader class
// ------------------------------------------------------------------------- 
extern "C"
HRESULT SymReader_Release(ISymUnmanagedReader *pReader)
{
    if (pReader)
        pReader->Release();;
    return NOERROR;
}

//-----------------------------------------------------------
// NewSymReader
// Static function used to create a new instance of SymReader
//-----------------------------------------------------------
HRESULT
SymReader::NewSymReader(
    REFCLSID clsid,
    void** ppObj
    )
{
    HRESULT hr = S_OK;
    SymReader* pSymReader = NULL;
    
    _ASSERTE(IsValidCLSID(clsid));
    _ASSERTE(IsValidWritePtr(ppObj, IUnknown*));

    if (clsid != IID_ISymUnmanagedReader)
        return (E_UNEXPECTED);

    IfFalseGo(ppObj, E_INVALIDARG);
    
    *ppObj = NULL;
    IfNullGo( pSymReader = NEW(SymReader()));

    *ppObj = pSymReader;
    pSymReader->AddRef();
    pSymReader = NULL;

ErrExit:

    RELEASE( pSymReader );

    return hr;
}


//-----------------------------------------------------------
// ~SymReader
//-----------------------------------------------------------
SymReader::~SymReader()
{
    if (m_pDocs)
    {
        unsigned i;
        for(i = 0; i < m_pPDBInfo->m_CountOfDocuments; i++)
        {
            RELEASE(m_pDocs[i]);
        }
    }

    RELEASE(m_pImporter);
    if (m_pStringPool)
    {
        DELETE(m_pStringPool);
    }

    DELETE(m_pPDBInfo);
    
    // If we loaded from stream, then free the memory we allocated
    if (m_fInitializeFromStream)
    {
        DELETEARRAY(m_DataPointers.m_pBytes);
        DELETEARRAY(m_DataPointers.m_pConstants);
        DELETEARRAY(m_DataPointers.m_pDocuments);
        DELETEARRAY(m_DataPointers.m_pMethods);
        DELETEARRAY(m_DataPointers.m_pScopes);
        DELETEARRAY(m_DataPointers.m_pSequencePoints);
        DELETEARRAY(m_DataPointers.m_pStringsBytes);
        DELETEARRAY(m_DataPointers.m_pUsings);
        DELETEARRAY(m_DataPointers.m_pVars);
    }

    DELETEARRAY(m_pDocs);
}

//-----------------------------------------------------------
// ~QueryInterface
//-----------------------------------------------------------
HRESULT
SymReader::QueryInterface(
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(IsValidIID(riid));
    _ASSERTE(IsValidWritePtr(ppvObject, void*));

    IfFalseGo(ppvObject, E_INVALIDARG);
    if (riid == IID_ISymUnmanagedReader)
    {
        *ppvObject = (ISymUnmanagedReader*) this;
    }
    else 
    if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)this;
    }
    else
    {
        *ppvObject = NULL;
        hr = E_NOINTERFACE;
    }

    if (*ppvObject)
    {
        AddRef();
    }

ErrExit:

    return hr;
}

static HRESULT ReadFromStream(IStream *pIStream, void *pv, ULONG cb)
{
    HRESULT hr = NOERROR;
    ULONG ulBytesRead;

    IfFailGo(pIStream->Read(pv, cb, &ulBytesRead));
    if (ulBytesRead != cb)
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// Initialize
// Pass in the required information to read in the debug info
// If a stream is passed in, it is used, otherwise a filename
// must be passed in
//-----------------------------------------------------------
HRESULT SymReader::Initialize(
    IUnknown *importer,         // Cash it to be consistent with CLR
    const WCHAR* szFileName,    // File name of the ildb
    const WCHAR* szsearchPath,  // Search Path
    IStream *pIStream           // IStream
    )
{
    HRESULT hr = NOERROR;
    GUID GuidVersion;
    BYTE *pSignature;
    _ASSERTE(szFileName || pIStream);
    IfFalseGo(szFileName || pIStream, E_INVALIDARG );

    // If it's passed in, we need to AddRef to be consistent the
    // desktop version since ReleaseImporterFromISymUnmanagedReader (ceeload.cpp)
    // assumes there's an addref
    if (importer)
    {
        m_pImporter = importer;
        m_pImporter->AddRef();
    }

    // If we're reading from a file, just read from it
    if (pIStream == NULL)
    {
        return InitializeFromFile(importer, szFileName, szsearchPath);
    }

    // We're reading in from a stream
    m_fInitializeFromStream = true;

    // Reset the stream to the begining
    LARGE_INTEGER li;
    li.u.HighPart = 0;
    li.u.LowPart = 0;

    // Make sure we're at the beginning of the stream
    IfFailGo(pIStream->Seek(li, STREAM_SEEK_SET, NULL));

    IfNullGo(pSignature = (BYTE *)_alloca(ILDB_SIGNATURE_SIZE));
    IfFailGo(ReadFromStream(pIStream, pSignature, sizeof(ILDB_SIGNATURE_SIZE)));
    
    // Verify that we're looking at an ILDB File
    if (memcmp(pSignature, ILDB_SIGNATURE, ILDB_SIGNATURE_SIZE))
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    IfFailGo(ReadFromStream(pIStream, &GuidVersion, sizeof(GUID)));

    SwapGuid(&GuidVersion);

    if (memcmp(&GuidVersion, &ILDB_VERSION_GUID, sizeof(GUID)))
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    IfNullGo(m_pPDBInfo = NEW(PDBInfo));

    memset(m_pPDBInfo, 0 , sizeof(PDBInfo));
    IfFailGo(ReadFromStream(pIStream, m_pPDBInfo, sizeof(PDBInfo)));

    // Swap the counts
    m_pPDBInfo->ConvertEndianness();

    if (m_pPDBInfo->m_CountOfConstants)
    {
        IfNullGo(m_DataPointers.m_pConstants = NEW(SymConstant[m_pPDBInfo->m_CountOfConstants]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pConstants, m_pPDBInfo->m_CountOfConstants*sizeof(SymConstant)));
    }

    if (m_pPDBInfo->m_CountOfMethods)
    {
        IfNullGo(m_DataPointers.m_pMethods = NEW(SymMethodInfo[m_pPDBInfo->m_CountOfMethods]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pMethods, m_pPDBInfo->m_CountOfMethods*sizeof(SymMethodInfo)));
    }

    if (m_pPDBInfo->m_CountOfScopes)
    {
        IfNullGo(m_DataPointers.m_pScopes = NEW(SymLexicalScope[m_pPDBInfo->m_CountOfScopes]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pScopes, m_pPDBInfo->m_CountOfScopes*sizeof(SymLexicalScope)));
    }

    if (m_pPDBInfo->m_CountOfVars)
    {
        IfNullGo(m_DataPointers.m_pVars = NEW(SymVariable[m_pPDBInfo->m_CountOfVars]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pVars, m_pPDBInfo->m_CountOfVars*sizeof(SymVariable)));
    }

    if (m_pPDBInfo->m_CountOfUsing)
    {
        IfNullGo(m_DataPointers.m_pUsings = NEW(SymUsingNamespace[m_pPDBInfo->m_CountOfUsing]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pUsings, m_pPDBInfo->m_CountOfUsing*sizeof(SymUsingNamespace)));
    }

    if (m_pPDBInfo->m_CountOfSequencePoints)
    {
        IfNullGo(m_DataPointers.m_pSequencePoints = NEW(SequencePoint[m_pPDBInfo->m_CountOfSequencePoints]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pSequencePoints, m_pPDBInfo->m_CountOfSequencePoints*sizeof(SequencePoint)));
    }

    if (m_pPDBInfo->m_CountOfDocuments)
    {
        IfNullGo(m_DataPointers.m_pDocuments = NEW(DocumentInfo[m_pPDBInfo->m_CountOfDocuments]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pDocuments, m_pPDBInfo->m_CountOfDocuments*sizeof(DocumentInfo)));
    }

    if (m_pPDBInfo->m_CountOfBytes)
    {
        IfNullGo(m_DataPointers.m_pBytes = NEW(BYTE[m_pPDBInfo->m_CountOfBytes]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pBytes, m_pPDBInfo->m_CountOfBytes*sizeof(BYTE)));
    }

    
    if (m_pPDBInfo->m_CountOfStringBytes)
    {
        m_pStringPool = NEW(StgStringPool());
        IfNullGo(m_DataPointers.m_pStringsBytes = NEW(BYTE[m_pPDBInfo->m_CountOfStringBytes]));
        IfFailGo(ReadFromStream(pIStream, m_DataPointers.m_pStringsBytes, m_pPDBInfo->m_CountOfStringBytes));
        IfFailGo(m_pStringPool->InitOnMem(m_DataPointers.m_pStringsBytes, m_pPDBInfo->m_CountOfStringBytes, true));
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// VerifyPEDebugInfo
// Verify that the debug info in the PE is the one we want
//-----------------------------------------------------------
HRESULT SymReader::VerifyPEDebugInfo(const WCHAR* szFileName)
{
    HRESULT hr = E_FAIL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = INVALID_HANDLE_VALUE;
    BYTE *pMod = NULL;
    DWORD dwFileSize;
    IMAGE_DEBUG_DIRECTORY *pDebugDir;
    RSDSI *pDebugInfo;
    DWORD dwUtf8Length;
    DWORD dwUnicodeLength;

    // We need to change the .pdb extension to .ildb
    // compatible with VS7
    wchar_t fullpath[_MAX_PATH];
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];

    IMAGE_NT_HEADERS*pNT;

    hFile = CreateFile(szFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Get the last error if we can
        return HrFromWin32(GetLastError());
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize < ILDB_HEADER_SIZE)
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapFile == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    pMod = (BYTE *) MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (pMod == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    pNT = Cor_RtlImageNtHeader(pMod, dwFileSize);

    // If there is no DebugEntry, then just error out
    if (VAL32(pNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress) == 0)
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));

    pDebugDir = (IMAGE_DEBUG_DIRECTORY *)(pMod + Cor_RtlImageRvaToOffset(pNT, VAL32(pNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress), dwFileSize));
    pDebugInfo = (RSDSI *)(pMod + VAL32(pDebugDir->PointerToRawData));

    if (pDebugInfo->dwSig != VAL32(0x53445352)) // "SDSR"
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    
    // Try the returned Stored Name since it might be a fully qualified path
    dwUtf8Length = VAL32(pDebugDir->SizeOfData) - sizeof(RSDSI);
    dwUnicodeLength = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) pDebugInfo->szPDB, dwUtf8Length, fullpath, _MAX_PATH);
    _wsplitpath( fullpath, drive, dir, fname, NULL );
    _wmakepath( m_szStoredSymbolName, drive, dir, fname, L"ildb" );

    _ASSERTE(dwUnicodeLength < _MAX_PATH);
    // Make sure it's NULL terminated
    m_szStoredSymbolName[dwUnicodeLength]='\0';

    // It looks valid, make sure to set the return code
    hr = S_OK;
ErrExit:
    if (pMod)
      UnmapViewOfFile(pMod);
    if (hMapFile != INVALID_HANDLE_VALUE)
      CloseHandle(hMapFile);
    if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    return hr;

}

//-----------------------------------------------------------
// InitializeFromFile
// Initialize the reader using the passed in file name
//-----------------------------------------------------------
HRESULT
SymReader::InitializeFromFile(
    IUnknown *importer,
    const WCHAR* szFileName,
    const WCHAR* szsearchPath)
{
    HRESULT hr = S_OK;
    wchar_t fullpath[_MAX_PATH];
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = INVALID_HANDLE_VALUE;
    HMODULE hMod = NULL;
    BYTE *CurrentOffset;
    DWORD dwFileSize;
    DWORD dwDataSize;
    GUID VersionInfo;
   
    _ASSERTE(szFileName);
    IfFalseGo(szFileName, E_INVALIDARG );

    IfFailGo(VerifyPEDebugInfo(szFileName));
    // We need to open the exe and check to see if the DebugInfo matches

    _wsplitpath( szFileName, drive, dir, fname, NULL );
    _wmakepath( fullpath, drive, dir, fname, L"ildb" );
    wcsncpy( m_szPath, fullpath, _MAX_PATH );
    
    hFile = CreateFile(m_szPath,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {

        // If the stored string is empty, don't do anything
        if (m_szStoredSymbolName[0] == '\0')
        {
            return HrFromWin32(GetLastError());
        }

        _wsplitpath( m_szStoredSymbolName, drive, dir, fname, NULL );
        _wmakepath( fullpath, drive, dir, fname, L"ildb" );
        wcsncpy( m_szPath, fullpath, _MAX_PATH );

        hFile = CreateFile(m_szPath,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            return HrFromWin32(GetLastError());
        }
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize < ILDB_HEADER_SIZE)
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapFile == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    hMod = (HMODULE) MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (hMod == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    // We've opened the file, now let's get the pertinent info
    CurrentOffset = (BYTE *)hMod;

    // Verify that we're looking at an ILDB File
    if (memcmp(CurrentOffset, ILDB_SIGNATURE, ILDB_SIGNATURE_SIZE))
    {
        IfFailGo(E_FAIL);
    }
    CurrentOffset += ILDB_SIGNATURE_SIZE;

    memcpy( &VersionInfo, CurrentOffset, sizeof(GUID));
    SwapGuid( &VersionInfo );
    CurrentOffset += sizeof(GUID);

    if (memcmp(&VersionInfo, &ILDB_VERSION_GUID, sizeof(GUID)))
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    IfNullGo(m_pPDBInfo = NEW(PDBInfo));

    memcpy(m_pPDBInfo, CurrentOffset, sizeof(PDBInfo));

    // Swap the counts
    m_pPDBInfo->ConvertEndianness();

    // Check to make sure we have enough data to be read in.
    dwDataSize = ILDB_HEADER_SIZE +
                 (m_pPDBInfo->m_CountOfConstants*sizeof(SymConstant)) +
                 (m_pPDBInfo->m_CountOfMethods * sizeof(SymMethodInfo)) +
                 (m_pPDBInfo->m_CountOfScopes*sizeof(SymLexicalScope)) +
                 (m_pPDBInfo->m_CountOfVars*sizeof(SymVariable)) +
                 (m_pPDBInfo->m_CountOfUsing*sizeof(SymUsingNamespace)) +
                 (m_pPDBInfo->m_CountOfSequencePoints*sizeof(SequencePoint)) +
                 (m_pPDBInfo->m_CountOfDocuments*sizeof(DocumentInfo)) +
                 (m_pPDBInfo->m_CountOfBytes*sizeof(BYTE));

    dwDataSize += (m_pPDBInfo->m_CountOfStringBytes*sizeof(BYTE));

    if (dwFileSize < dwDataSize)
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    CurrentOffset += sizeof(PDBInfo);

    if (m_pPDBInfo->m_CountOfConstants)
    {
        m_DataPointers.m_pConstants = (SymConstant*)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfConstants*sizeof(SymConstant));
    }

    if (m_pPDBInfo->m_CountOfMethods)
    {
        m_DataPointers.m_pMethods = (SymMethodInfo *)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfMethods*sizeof(SymMethodInfo));
    }

    if (m_pPDBInfo->m_CountOfScopes)
    {
        m_DataPointers.m_pScopes = (SymLexicalScope *)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfScopes*sizeof(SymLexicalScope));
    }

    if (m_pPDBInfo->m_CountOfVars)
    {
        m_DataPointers.m_pVars = (SymVariable *)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfVars*sizeof(SymVariable));
    }

    if (m_pPDBInfo->m_CountOfUsing)
    {
        m_DataPointers.m_pUsings = (SymUsingNamespace *)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfUsing*sizeof(SymUsingNamespace));
    }

    if (m_pPDBInfo->m_CountOfSequencePoints)
    {
        m_DataPointers.m_pSequencePoints = (SequencePoint*)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfSequencePoints*sizeof(SequencePoint));
    }

    if (m_pPDBInfo->m_CountOfDocuments)
    {
        m_DataPointers.m_pDocuments = (DocumentInfo*)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfDocuments*sizeof(DocumentInfo));
    }

    if (m_pPDBInfo->m_CountOfBytes)
    {
        m_DataPointers.m_pBytes = (BYTE*)CurrentOffset;
        CurrentOffset += (m_pPDBInfo->m_CountOfBytes*sizeof(BYTE));
    }

    if (m_pPDBInfo->m_CountOfStringBytes)
    {
        m_pStringPool = NEW(StgStringPool());
        m_DataPointers.m_pStringsBytes = (BYTE*)CurrentOffset;
        IfFailGo(m_pStringPool->InitOnMem(m_DataPointers.m_pStringsBytes, m_pPDBInfo->m_CountOfStringBytes, true));
    }
        
ErrExit:
    
    return hr;
}

//-----------------------------------------------------------
// GetDocument
// Get the document for the passed in information
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetDocument(
    ISymUnmanagedReader *pReader,
    WCHAR *wcsUrl,
    GUID language,
    GUID languageVendor,
    GUID documentType,
    ISymUnmanagedDocument **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetDocument(wcsUrl, language, languageVendor, documentType, ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetDocument(
    WCHAR *wcsUrl,        // URL of the document
    GUID language,        // Language for the file
    GUID languageVendor,  // Language vendor
    GUID documentType,    // Type of document
    ISymUnmanagedDocument **ppRetVal  // [out] Document
    )
{
    HRESULT hr = S_OK;
    unsigned i;
    SymDocument* pDoc = NULL;
    WCHAR *wcsDocumentUrl = NULL;
    WCHAR *wcsDocumentUrlAlloc = NULL;

    _ASSERTE(ppRetVal && wcsUrl);
    IfFalseGo(ppRetVal, E_INVALIDARG);
    IfFalseGo(wcsUrl, E_INVALIDARG);

    // Init Out Parameter
    *ppRetVal = NULL;

    for (i = 0; i < m_pPDBInfo->m_CountOfDocuments; i++)
    {
        int cchName;

        // Convert the UTF8 string to Wide
        cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(m_DataPointers.m_pStringsBytes[m_DataPointers.m_pDocuments[i].UrlEntry()]),
                                                -1,
                                                0,
                                                NULL);
        IfNullGo( wcsDocumentUrlAlloc = NEW(WCHAR[cchName]) );

        cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(m_DataPointers.m_pStringsBytes[m_DataPointers.m_pDocuments[i].UrlEntry()]),
                                                -1,
                                                wcsDocumentUrlAlloc,
                                                cchName);
        wcsDocumentUrl = wcsDocumentUrlAlloc;

        // Compare the url
        if (wcscmp(wcsUrl, wcsDocumentUrl) == 0)
        {
            IfFailGo(GetDocument(i, &pDoc));
            break;
        }
        DELETEARRAY(wcsDocumentUrlAlloc);
        wcsDocumentUrlAlloc = NULL;
    }
  
    if (pDoc)
    {
        IfFailGo( pDoc->QueryInterface( IID_ISymUnmanagedDocument,
                                        (void**) ppRetVal ) );
    }

ErrExit:
    DELETEARRAY(wcsDocumentUrlAlloc);

    RELEASE( pDoc );

    return hr;
}

//-----------------------------------------------------------
// GetDocuments
// Get the documents for this reader
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetDocuments(
    ISymUnmanagedReader *pReader,
    ULONG32 cDocs,
    ULONG32 *pcDocs,
    ISymUnmanagedDocument *pDocs[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetDocuments(cDocs, pcDocs, pDocs);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetDocuments(
    ULONG32 cDocs,
    ULONG32 *pcDocs,
    ISymUnmanagedDocument *pDocs[]
    )
{
    HRESULT hr = S_OK;
    unsigned cDocCount = 0;

    _ASSERTE(pDocs || pcDocs);
    IfFalseGo(pDocs || pcDocs, E_INVALIDARG);

    cDocs = min(cDocs, m_pPDBInfo->m_CountOfDocuments);

    for (cDocCount = 0; cDocCount < cDocs; cDocCount++)
    {
        if (pDocs)
        {
            SymDocument *pDoc;
            IfFailGo(GetDocument(cDocCount, &pDoc));
            pDocs[cDocCount] = pDoc;
        }
    }
    if (pcDocs)
    {
        *pcDocs = (ULONG32)m_pPDBInfo->m_CountOfDocuments;
    }

ErrExit:
    if (FAILED(hr))
    {
        unsigned i;
        for (i = 0; i < cDocCount; i++)
        {
            RELEASE(pDocs[cDocCount]);
        }
    }
    return hr;
}

//-----------------------------------------------------------
// GetUserEntryPoint
// Get the entry point for the pe
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetUserEntryPoint(
    ISymUnmanagedReader *pReader,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetUserEntryPoint(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetUserEntryPoint(
    mdMethodDef *pRetVal
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    // If it wasn't set then return E_FAIL
    if (m_pPDBInfo->m_userEntryPoint == mdTokenNil)
    {
        hr = E_FAIL;
    }
    else
    {
        *pRetVal = m_pPDBInfo->m_userEntryPoint;
    }
ErrExit:    
    return hr;
}

// Compare the method token with the SymMethodInfo Entry and return the
// value expected by bsearch
int __cdecl CompareMethodToToken(const void *pMethodToken, const void *pMethodInfoEntry)
{
    mdMethodDef MethodDef = *(mdMethodDef *)pMethodToken;
    SymMethodInfo *pMethodInfo = (SymMethodInfo *)pMethodInfoEntry;

    return MethodDef - pMethodInfo->MethodToken();
}

//-----------------------------------------------------------
// GetMethod
// Get the method for the methoddef
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetMethod(
    ISymUnmanagedReader *pReader,
    mdMethodDef method,
    ISymUnmanagedMethod **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetMethod(method, ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetMethod(
    mdMethodDef method,   // MethodDef
    ISymUnmanagedMethod **ppRetVal  // [out] Method
    )
{
    HRESULT hr = E_FAIL;
    UINT32 MethodEntry = 0;
    SymMethodInfo *pMethodInfo;

    _ASSERTE(ppRetVal);
    IfFalseGo(ppRetVal, E_INVALIDARG);

    pMethodInfo = (SymMethodInfo *)bsearch(&method, m_DataPointers.m_pMethods, m_pPDBInfo->m_CountOfMethods, sizeof(SymMethodInfo), CompareMethodToToken);
    if (pMethodInfo)
    {
        MethodEntry = UINT32 (pMethodInfo - m_DataPointers.m_pMethods);
        _ASSERTE(m_DataPointers.m_pMethods[MethodEntry].MethodToken() == method);
        SymMethod *pMethod = NULL;
        IfNullGo( pMethod = NEW(SymMethod(this, &m_DataPointers, MethodEntry)) );
        *ppRetVal = pMethod;
        pMethod->AddRef();
        hr = S_OK;
    }
     
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetMethodByVersion
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetMethodByVersion(
    ISymUnmanagedReader *pReader,
    mdMethodDef method,
    int version,
    ISymUnmanagedMethod **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetMethodByVersion(method, version, ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetMethodByVersion(
    mdMethodDef method,
    int version,
    ISymUnmanagedMethod **ppRetVal
    )
{
    // Don't support multiple version of the same Method so just
    // call GetMethod
    return GetMethod(method, ppRetVal);
}


//-----------------------------------------------------------
// GetMethodFromDocumentPosition
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetMethodFromDocumentPosition(
    ISymUnmanagedReader *pReader,
    ISymUnmanagedDocument *document,
    ULONG32 line,
    ULONG32 column,
    ISymUnmanagedMethod **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetMethodFromDocumentPosition(document, line, column, ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetMethodFromDocumentPosition(
    ISymUnmanagedDocument *document,
    ULONG32 line,
    ULONG32 column,
    ISymUnmanagedMethod **ppRetVal
)
{
    HRESULT hr = S_OK;
    UINT32 DocumentEntry;
    UINT32 Method;
    UINT32 point;
    SequencePoint *pSequencePointBefore;
    SequencePoint *pSequencePointAfter;

    _ASSERTE(document && ppRetVal);
    IfFalseGo(document, E_INVALIDARG);
    IfFalseGo(ppRetVal, E_INVALIDARG);

    DocumentEntry = ((SymDocument *)document)->GetDocumentEntry();

    // Init out parameter
    *ppRetVal = NULL;

    // Walk all Methods, check their Document and SequencePoints to see if it's in this doc
    // and the line/column

    for (Method = 0; Method < m_pPDBInfo->m_CountOfMethods; Method++)
    {
        pSequencePointBefore = NULL;
        pSequencePointAfter = NULL;

        // Walk the sequence points
        for (point = m_DataPointers.m_pMethods[Method].StartSequencePoints();
             point < m_DataPointers.m_pMethods[Method].EndSequencePoints();
             point++)
        {
            // Check to see if this sequence point is in this doc
            if (m_DataPointers.m_pSequencePoints[point].Document() == DocumentEntry)
            {
                // If the point is position is within the sequence point then
                // we're done.
                if (m_DataPointers.m_pSequencePoints[point].IsWithin(line, column))
                {
                    IfFailGo(GetMethod(m_DataPointers.m_pMethods[Method].MethodToken(), ppRetVal));
                    break;
                }

                // If the sequence is before the point then just remember the point
                if (m_DataPointers.m_pSequencePoints[point].IsUserLine() &&
                    m_DataPointers.m_pSequencePoints[point].IsLessThan(line, column))
                {
                    pSequencePointBefore = &m_DataPointers.m_pSequencePoints[point];
                }

                // If the sequence is before the point then just remember the point
                if (m_DataPointers.m_pSequencePoints[point].IsUserLine() && 
                    m_DataPointers.m_pSequencePoints[point].IsGreaterThan(line, column))
                {
                    pSequencePointAfter = &m_DataPointers.m_pSequencePoints[point];
                }
            }
        }

        // If we found sequence points within the method before and after
        // the line/column then we found the method!
        if (pSequencePointBefore && pSequencePointAfter)
        {
            IfFailGo(GetMethod(m_DataPointers.m_pMethods[Method].MethodToken(), ppRetVal));
            break;
        }
    }

    if (*ppRetVal == NULL)
    {
        hr = E_FAIL;
    }

ErrExit:
    return hr;
}


HRESULT
SymReader::GetMethodsFromDocumentPosition(
    ISymUnmanagedDocument *document,
    ULONG32 line,
    ULONG32 column,
    ULONG32 cMethod,
    ULONG32* pcMethod,  //[Optional]: How many method actually returned
    ISymUnmanagedMethod** ppRetVal
    )
{

    HRESULT hr = S_OK;

    _ASSERTE(document && ppRetVal && cMethod);
    IfFalseGo(document, E_INVALIDARG);
    IfFalseGo(ppRetVal, E_INVALIDARG);

    // Init out parameter
    *ppRetVal = NULL;

    IfFailGo(GetMethodFromDocumentPosition(document, line, column, ppRetVal));

    // Since we only support one version for a file then we only return one method
    if (*ppRetVal && pcMethod)
    {
        *pcMethod = 1;
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetSymbolStoreFileName
//-----------------------------------------------------------
HRESULT
SymReader::GetSymbolStoreFileName(
    ULONG32 cchName,    // Length of szName
    ULONG32 *pcchName,  // [Optional]
    WCHAR szName[]      // [Optional]
    )
{
    if (pcchName)
    {
        *pcchName = (ULONG32)(wcslen(m_szPath)+1);
    }

    if( szName )
    {
        wcsncpy( szName, m_szPath, cchName );
    }

    return NOERROR;
}

//-----------------------------------------------------------
// GetMethodVersion
//-----------------------------------------------------------
HRESULT
SymReader::GetMethodVersion(
    ISymUnmanagedMethod * pMethod,
    int* pVersion
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pMethod && pVersion);
    IfFalseGo( pMethod && pVersion, E_INVALIDARG);    
    // This symbol store only supports one version of a method
    *pVersion = 0;
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetDocumentVersion
//-----------------------------------------------------------
HRESULT
SymReader::GetDocumentVersion(
    ISymUnmanagedDocument* pDoc,
    int* pVersion,
    BOOL* pbCurrent // [Optional]
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pVersion && pDoc);
    IfFalseGo(pVersion, E_INVALIDARG);
    IfFalseGo(pDoc, E_INVALIDARG);

    // This symbol store only supports one version of a document
    *pVersion = 0;
    if (pbCurrent)
    {
        *pbCurrent = TRUE;
    }
ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetDocument
// Return the document for the given entry
//-----------------------------------------------------------
HRESULT SymReader::GetDocument(
    UINT32 DocumentEntry,   
    SymDocument **ppDocument)
{
    HRESULT hr = NOERROR;
    _ASSERTE(ppDocument);
    _ASSERTE(DocumentEntry < m_pPDBInfo->m_CountOfDocuments);
    IfFalseGo(ppDocument, E_INVALIDARG);

    if (m_pDocs == NULL)
    {
        IfNullGo(m_pDocs = NEW(SymDocument *[m_pPDBInfo->m_CountOfDocuments]));
        memset(m_pDocs, 0, m_pPDBInfo->m_CountOfDocuments * sizeof(void *));
    }

    if (m_pDocs[DocumentEntry] == NULL)
    {
        m_pDocs[DocumentEntry] = NEW(SymDocument(this, &m_DataPointers, m_pPDBInfo->m_CountOfMethods, DocumentEntry));
        IfNullGo(m_pDocs[DocumentEntry]);
        // AddRef the table version
        m_pDocs[DocumentEntry]->AddRef();

    }

    //Set and AddRef the Out Parameter
    *ppDocument = m_pDocs[DocumentEntry];
    (*ppDocument)->AddRef();

ErrExit:
    return hr;
}

HRESULT
SymReader::UpdateSymbolStore(
    const WCHAR *filename,
    IStream *pIStream
    )
{
    // This symbol store doesn't support updating the symbol store.
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

HRESULT
SymReader::ReplaceSymbolStore(
    const WCHAR *filename,
    IStream *pIStream
    )
{
    // This symbol store doesn't support updating the symbol store.
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetVariables
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetVariables(
    ISymUnmanagedReader *pReader,
    mdToken parent,
    ULONG32 cVars,
    ULONG32 *pcVars,
    ISymUnmanagedVariable *pVars[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetVariables(parent, cVars, pcVars, pVars);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetVariables(
    mdToken parent,
    ULONG32 cVars,
    ULONG32 *pcVars,
    ISymUnmanagedVariable *pVars[]
    )
{
    //
    // This symbol reader doesn't support non-local variables.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetGlobalVariables
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetGlobalVariables(
    ISymUnmanagedReader *pReader,
    ULONG32 cVars,
    ULONG32 *pcVars,
    ISymUnmanagedVariable *pVars[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetGlobalVariables(cVars, pcVars, pVars);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetGlobalVariables(
    ULONG32 cVars,
    ULONG32 *pcVars,
    ISymUnmanagedVariable *pVars[]
    )
{
    //
    // This symbol reader doesn't support non-local variables.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetSymAttribute
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetSymAttribute(
    ISymUnmanagedReader *pReader,
    mdToken parent,
    WCHAR *name,
    ULONG32 cBuffer,
    ULONG32 *pcBuffer,
    BYTE buffer[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetSymAttribute(parent, name, cBuffer, pcBuffer, buffer);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetSymAttribute(
    mdToken parent,
    WCHAR *name,
    ULONG32 cBuffer,
    ULONG32 *pcBuffer,
    BYTE buffer[]
    )
{
    // This symbol store doesn't support this
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetNamespaces
//-----------------------------------------------------------
extern "C"
HRESULT SymReader_GetNamespaces(
    ISymUnmanagedReader *pReader,
    ULONG32 cNameSpaces,
    ULONG32 *pcNameSpaces,
    ISymUnmanagedNamespace *namespaces[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pReader, E_INVALIDARG);
    hr = pReader->GetNamespaces(cNameSpaces, pcNameSpaces, namespaces);
ErrExit:
    return hr;
}
HRESULT
SymReader::GetNamespaces(
    ULONG32 cNameSpaces,
    ULONG32 *pcNameSpaces,
    ISymUnmanagedNamespace *namespaces[]
    )
{
    // This symbol store doesn't support this
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

/* ------------------------------------------------------------------------- *
 * SymDocument class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT SymDocument_Release(ISymUnmanagedDocument *pDocument)
{
    if (pDocument)
        pDocument->Release();;
    return NOERROR;
}

HRESULT
SymDocument::QueryInterface(
    REFIID riid,
    void **ppInterface
    )
{
    if (riid == IID_ISymUnmanagedDocument)
        *ppInterface = (ISymUnmanagedDocument*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedDocument*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


//-----------------------------------------------------------
// GetURL
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetUrl(
    ISymUnmanagedDocument *pDocument,
    ULONG32 cchUrl,
    ULONG32 *pcchUrl,
    WCHAR szUrl[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetURL(cchUrl, pcchUrl, szUrl);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetURL(
    ULONG32 cchUrl,   // The allocated size of the buffer
    ULONG32 *pcchUrl, // [optional,out] The number of characters available for return
    WCHAR szUrl[]     // [optional,out] The string buffer.
    )
{


    if (pcchUrl)
    {
        // Convert the UTF8 string to Wide
        *pcchUrl = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pDocuments[m_DocumentEntry].UrlEntry()]),
                                                -1,
                                                0,
                                                NULL);
    }

    if( szUrl )
    {
        // Convert the UTF8 string to Wide
        MultiByteToWideChar(CP_UTF8,
                            0,
                            (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pDocuments[m_DocumentEntry].UrlEntry()]),
                            -1,
                            szUrl,
                            cchUrl);
    }
    return NOERROR;
}

//-----------------------------------------------------------
// GetDocumentType
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetDocumentType(
    ISymUnmanagedDocument *pDocument,
    GUID *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetDocumentType(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetDocumentType(
    GUID *pRetVal
    )
{
    HRESULT hr = NOERROR;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pDocuments[m_DocumentEntry].DocumentType();
    SwapGuid(pRetVal);
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetLanguage
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetLanguage(
    ISymUnmanagedDocument *pDocument,
    GUID *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetLanguage(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetLanguage(
    GUID *pRetVal
    )
{
    HRESULT hr = NOERROR;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    *pRetVal = m_pData->m_pDocuments[m_DocumentEntry].Language();
    SwapGuid(pRetVal);
ErrExit:        
    return hr;
}

//-----------------------------------------------------------
// GetLanguageVendor
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetLanguageVendor(
    ISymUnmanagedDocument *pDocument,
    GUID *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetLanguageVendor(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymDocument::GetLanguageVendor(
    GUID *pRetVal
    )
{
    HRESULT hr = NOERROR;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pDocuments[m_DocumentEntry].LanguageVendor();
    SwapGuid(pRetVal);
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetCheckSumAlgorithmId
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetCheckSumAlgorithmId(
    ISymUnmanagedDocument *pDocument,
    GUID *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetCheckSumAlgorithmId(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetCheckSumAlgorithmId(
    GUID *pRetVal
    )
{
    HRESULT hr = NOERROR;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pDocuments[m_DocumentEntry].AlgorithmId();
    SwapGuid(pRetVal);
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetCheckSum
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetCheckSum(
    ISymUnmanagedDocument *pDocument,
    ULONG32 cData,
    ULONG32 *pcData,
    BYTE data[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetCheckSum(cData, pcData, data);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetCheckSum(
    ULONG32 cData,    // The allocated size of the buffer.
    ULONG32 *pcData,  // [optional] The number of bytes available for return
    BYTE data[])      // [optional] The buffer to receive the checksum.
{
    BYTE *pCheckSum = &m_pData->m_pBytes[m_pData->m_pDocuments[m_DocumentEntry].CheckSumEntry()];
    ULONG32 CheckSumSize = m_pData->m_pDocuments[m_DocumentEntry].CheckSumSize();
    if (pcData)
    {
        *pcData = CheckSumSize;
    }
    if(data)
    {
        memcpy(data, pCheckSum, min(CheckSumSize, cData));
    }
    return NOERROR;
}

//-----------------------------------------------------------
// FindClosestLine
// Search the sequence points looking a line that is closest
// line following this one that is a sequence point
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_FindClosestLine(
    ISymUnmanagedDocument *pDocument,
    ULONG32 line,
    ULONG32 *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->FindClosestLine(line, pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::FindClosestLine(
    ULONG32 line,
    ULONG32 *pRetVal
    )
{
    HRESULT hr = NOERROR;
    UINT32 Method;
    UINT32 point;
    ULONG32 RetVal = 0xffffffff;
    
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    // Walk all Methods, check their Document and SequencePoints to see if it's in this doc
    // and the line/column
    for (Method = 0; Method < m_CountOfMethods; Method++)
    {
        // Walk the sequence points
        for (point = m_pData->m_pMethods[Method].StartSequencePoints();
             point < m_pData->m_pMethods[Method].EndSequencePoints();
             point++)
        {
            // Check to see if this sequence point is in this doc
            if (m_pData->m_pSequencePoints[point].Document() == m_DocumentEntry)
            {
                if (m_pData->m_pSequencePoints[point].IsUserLine() &&
                    m_pData->m_pSequencePoints[point].IsGreaterThan(line, 0))
                {
                    if (m_pData->m_pSequencePoints[point].StartLine() < RetVal)
                    {
                        RetVal = m_pData->m_pSequencePoints[point].StartLine();
                    }
                }
            }
        }
    }

    *pRetVal = RetVal;

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymDocument HasEmbeddedSource
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_HasEmbeddedSource(
    ISymUnmanagedDocument *pDocument,
    BOOL *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->HasEmbeddedSource(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::HasEmbeddedSource(
    BOOL *pRetVal
    )
{
    //
    // This symbol reader doesn't support embedded source.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// SymDocument GetSourceLength
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetSourceLength(
    ISymUnmanagedDocument *pDocument,
    ULONG32 *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetSourceLength(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetSourceLength(
    ULONG32 *pRetVal
    )
{
    //
    // This symbol reader doesn't support embedded source.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// SymDocument GetSourceRange
//-----------------------------------------------------------
extern "C"
HRESULT SymDocument_GetSourceRange(
    ISymUnmanagedDocument *pDocument,
    ULONG32 startLine,
    ULONG32 startColumn,
    ULONG32 endLine,
    ULONG32 endColumn,
    ULONG32 cSourceBytes,
    ULONG32 *pcSourceBytes,
    BYTE source[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocument, E_INVALIDARG);
    hr = pDocument->GetSourceRange(startLine, startColumn,
                                                    endLine, endColumn,
                                                    cSourceBytes, pcSourceBytes, source);

ErrExit:
    return hr;
}
HRESULT
SymDocument::GetSourceRange(
    ULONG32 startLine,
    ULONG32 startColumn,
    ULONG32 endLine,
    ULONG32 endColumn,
    ULONG32 cSourceBytes,
    ULONG32 *pcSourceBytes,
    BYTE source[]
    )
{
    //
    // This symbol reader doesn't support embedded source.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

/* ------------------------------------------------------------------------- *
 * SymMethod class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT SymMethod_Release(ISymUnmanagedMethod *pMethod)
{
    if (pMethod)
        pMethod->Release();;
    return NOERROR;
}

HRESULT
SymMethod::QueryInterface(
    REFIID riid,
    void **ppInterface
    )
{
    if (riid == IID_ISymUnmanagedMethod)
        *ppInterface = (ISymUnmanagedMethod*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedMethod*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// GetToken
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetToken(
    ISymUnmanagedMethod *pMethod,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetToken(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetToken(
    mdMethodDef *pRetVal
)
{
    HRESULT hr = S_OK;
    
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pMethods[m_MethodEntry].MethodToken();
ErrExit:
    return hr;
}


//-----------------------------------------------------------
// GetSequencePointCount
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetSequencePointCount(
    ISymUnmanagedMethod *pMethod,
    ULONG32 *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetSequencePointCount(pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetSequencePointCount(
    ULONG32* pRetVal
    )
{

    HRESULT hr = S_OK;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    *pRetVal = (ULONG32)(m_pData->m_pMethods[m_MethodEntry].EndSequencePoints() - 
                         m_pData->m_pMethods[m_MethodEntry].StartSequencePoints());
ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetSequencePoints
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetSequencePoints(
    ISymUnmanagedMethod *pMethod,
    ULONG32 cPoints,
    ULONG32* pcPoints,
    ULONG32 offsets[],
    ISymUnmanagedDocument *documents[],
    ULONG32 lines[],
    ULONG32 columns[],
    ULONG32 endLines[],
    ULONG32 endColumns[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetSequencePoints(cPoints, pcPoints,
                                                   offsets, documents,
                                                   lines, columns,
                                                   endLines, endColumns);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetSequencePoints(
    ULONG32 cPoints,    // The size of the allocated arrays. 
    ULONG32* pcPoints,  // [optional] The number of sequence points available for return.
    ULONG32 offsets[],  // [optional]
    ISymUnmanagedDocument *documents[], // [Optional]
    ULONG32 lines[],      // [Optional]
    ULONG32 columns[],    // [Optional]
    ULONG32 endLines[],   // [Optional]
    ULONG32 endColumns[]  // [Optional]
    )
{

    HRESULT hr = NOERROR;
    UINT32 i = 0;
    ULONG32 Points = 0;
    // Other parameters are optional
    _ASSERTE(pcPoints && offsets);
    IfFalseGo(pcPoints, E_INVALIDARG);
    IfFalseGo(offsets, E_INVALIDARG);
    
    for (i = m_pData->m_pMethods[m_MethodEntry].StartSequencePoints();
         (i < m_pData->m_pMethods[m_MethodEntry].EndSequencePoints());
         i++, Points++)
    {
        if (Points < cPoints)
        {
            if (documents)
            {
                SymDocument *pDoc;
                IfFailGo(m_pReader->GetDocument(m_pData->m_pSequencePoints[i].Document(), &pDoc));
                documents[Points] = pDoc;
            }

            if (offsets)
            {
                offsets[Points] = m_pData->m_pSequencePoints[i].Offset();
            }

            if (lines)
            {
                lines[Points] = m_pData->m_pSequencePoints[i].StartLine();
            }
            if (columns)
            {
                columns[Points] = m_pData->m_pSequencePoints[i].StartColumn();
            }
            if (endLines)
            {
                endLines[Points] = m_pData->m_pSequencePoints[i].EndLine();
            }
            if (endColumns)
            {
                endColumns[Points] = m_pData->m_pSequencePoints[i].EndColumn();
            }
        }
    }

    if (pcPoints)
    {
        *pcPoints = Points;
    }

ErrExit:
    if (FAILED(hr))
    {
        if (documents)
        {
            unsigned j;
            for (j = 0; j < i; j++)
            {
                RELEASE(documents[i]);
            }
        }
    }
    return hr;
}

//-----------------------------------------------------------
// GetRootScope
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetRootScope(
    ISymUnmanagedMethod *pMethod,
    ISymUnmanagedScope **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetRootScope(ppRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetRootScope(
    ISymUnmanagedScope **ppRetVal
    )
{
    HRESULT hr = S_OK;
    SymScope *pScope = NULL;
    _ASSERTE(ppRetVal);
    IfFalseGo(ppRetVal, E_INVALIDARG);

    // Init Out Param
    *ppRetVal = NULL;
    if (m_pData->m_pMethods[m_MethodEntry].EndScopes() - m_pData->m_pMethods[m_MethodEntry].StartScopes())
    {
        IfNullGo(pScope = NEW(SymScope(this, m_pData, NULL, m_MethodEntry, m_pData->m_pMethods[m_MethodEntry].StartScopes())));
        pScope->AddRef();
        *ppRetVal = pScope;
    }
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetOffset
// Given a position in a document, gets the offset within the 
// method that corresponds to the position.
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetOffset(
    ISymUnmanagedMethod *pMethod,
    ISymUnmanagedDocument *document,
    ULONG32 line,
    ULONG32 column,
    ULONG32 *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetOffset(document, line, column, pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetOffset(
    ISymUnmanagedDocument *document,
    ULONG32 line,
    ULONG32 column,
    ULONG32 *pRetVal
    )
{
    HRESULT hr = E_NOTIMPL;
    bool fFound = false;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    UINT32 point;
    UINT32 DocumentEntry;

    DocumentEntry = ((SymDocument *)document)->GetDocumentEntry();

    // Walk the sequence points
    for (point = m_pData->m_pMethods[m_MethodEntry].StartSequencePoints();
         point < m_pData->m_pMethods[m_MethodEntry].EndSequencePoints();
         point++)
    {
        // Check to see if this sequence point is in this doc
        if (m_pData->m_pSequencePoints[point].Document() == DocumentEntry)
        {
            // Check to see if it's within the sequence point
            if (m_pData->m_pSequencePoints[point].IsWithin(line, column))
            {
                *pRetVal = m_pData->m_pSequencePoints[point].Offset();               
                fFound = true;
                break;
            }
        }
    }
    if (!fFound)
    {
        hr = E_FAIL;
    }
ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetRanges
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetRanges(
    ISymUnmanagedMethod *pMethod,
    ISymUnmanagedDocument *pDocument,
    ULONG32 line,
    ULONG32 column,
    ULONG32 cRanges,
    ULONG32 *pcRanges,
    ULONG32 ranges[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetRanges(pDocument, line, column, cRanges, pcRanges, ranges);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetRanges(
    ISymUnmanagedDocument *pDocument, // [in] Document we're working on
    ULONG32 line,                     // [in] The document line corresponding to the ranges.
    ULONG32 column,                   // [in] Ignored
    ULONG32 cRanges,                  // [in] The size of the allocated ranges[] array.
    ULONG32 *pcRanges,                // [out] The number of ranges available for return
    ULONG32 ranges[]                  // [out] The range array.
    )
{
    HRESULT hr = NOERROR;
    DWORD iRange = 0;
    UINT32 DocumentEntry;
    UINT32 point;
    bool fFound = false;

    // Validate some of the parameters
    _ASSERTE(pDocument && (cRanges % 2) == 0);
    IfFalseGo(pDocument, E_INVALIDARG);
    IfFalseGo((cRanges % 2) == 0, E_INVALIDARG);

    // Init out parameter
    if (pcRanges)
    {
        *pcRanges=0;
    }

    DocumentEntry = ((SymDocument *)pDocument)->GetDocumentEntry();

    // Walk the sequence points
    for (point = m_pData->m_pMethods[m_MethodEntry].StartSequencePoints();
         point < m_pData->m_pMethods[m_MethodEntry].EndSequencePoints();
         point++)
    {
        // Check to see if this sequence point is in this doc
        if (m_pData->m_pSequencePoints[point].Document() == DocumentEntry)
        {
            // Check to see if the line is within this sequence
            // Note, to be compatible with VS7, ignore the column information
            if (line >= m_pData->m_pSequencePoints[point].StartLine() &&
                line <= m_pData->m_pSequencePoints[point].EndLine())
            {
                fFound = true;
                break;
            }
        }
    }

    if (fFound)
    {
        for (;point < m_pData->m_pMethods[m_MethodEntry].EndSequencePoints(); point++)
        {

            // Search through all the sequence points since line might have there
            // IL spread accross multiple ranges (for loops for example)
            if (m_pData->m_pSequencePoints[point].Document() == DocumentEntry &&
                line >= m_pData->m_pSequencePoints[point].StartLine() &&
                line <= m_pData->m_pSequencePoints[point].EndLine())
            {
                if (iRange < cRanges)
                {
                    ranges[iRange] = m_pData->m_pSequencePoints[point].Offset();
                }
                iRange++;
                if (iRange < cRanges)
                {
                    if (point+1 < m_pData->m_pMethods[m_MethodEntry].EndSequencePoints())
                    {
                        ranges[iRange] = m_pData->m_pSequencePoints[point+1].Offset();
                    }
                    else
                    {
                        // Then it must be till the end of the function which is the root scope's endoffset
                        ranges[iRange] = m_pData->m_pScopes[m_pData->m_pMethods[m_MethodEntry].StartScopes()].EndOffset()+1;
                    }
                }
                iRange++;
            }
        }
        if (pcRanges)
        {
            // If cRanges passed in, return the number
            // of elements actually filled in
            if (cRanges)
            {
                *pcRanges = min(iRange, cRanges);
            }
            else
            {
                // Otherwise return the max number
                *pcRanges = iRange;
            }
        }
    }
    else
    {
        return E_FAIL;
    }

ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetScopeFromOffset
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetScopeFromOffset(
    ISymUnmanagedMethod *pMethod,
    ULONG32 offset,
    ISymUnmanagedScope **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetScopeFromOffset(offset, ppRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetScopeFromOffset(
    ULONG32 offset,
    ISymUnmanagedScope **pRetVal
    )
{
    //
    // This symbol reader doesn't support this functionality
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetParameters
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetParameters(
    ISymUnmanagedMethod *pMethod,
    ULONG32 cParams,
    ULONG32 *pcParams,
    ISymUnmanagedVariable *params[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetParameters(cParams, pcParams, params);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetParameters(
    ULONG32 cParams,
    ULONG32 *pcParams,
    ISymUnmanagedVariable *params[]
    )
{
    //
    // This symbol reader doesn't support parameter access. Parameters
    // can be found in the normal metadata.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetNamespace
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetNamespace(
    ISymUnmanagedMethod *pMethod,
    ISymUnmanagedNamespace **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetNamespace(ppRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetNamespace(
    ISymUnmanagedNamespace **ppRetVal
    )
{
    //
    // This symbol reader doesn't support namespaces
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetSourceStartEnd
//-----------------------------------------------------------
extern "C"
HRESULT SymMethod_GetSourceStartEnd(
    ISymUnmanagedMethod *pMethod,
    ISymUnmanagedDocument *docs[2],
    ULONG32 lines[2],
    ULONG32 columns[2],
    BOOL *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pMethod, E_INVALIDARG);
    hr = pMethod->GetSourceStartEnd(docs, lines, columns, pRetVal);

ErrExit:
    return hr;
}
HRESULT
SymMethod::GetSourceStartEnd(
    ISymUnmanagedDocument *docs[2],
    ULONG32 lines[2],
    ULONG32 columns[2],
    BOOL *pRetVal
    )
{
    //
    // This symbol reader doesn't support source start/end for methods.
    //
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

/* ------------------------------------------------------------------------- *
 * SymScope class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT SymScope_Release(ISymUnmanagedScope* pScope)
{
    if (pScope)
        pScope->Release();;
    return NOERROR;
}

//-----------------------------------------------------------
// QueryInterface
//-----------------------------------------------------------
HRESULT
SymScope::QueryInterface(
    REFIID riid,
    void **ppInterface
    )
{
    if (riid == IID_ISymUnmanagedScope)
        *ppInterface = (ISymUnmanagedScope*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedScope*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// GetMethod
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetMethod(
    ISymUnmanagedScope *pScope,
    ISymUnmanagedMethod **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetMethod(ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetMethod(
    ISymUnmanagedMethod **ppRetVal
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(ppRetVal);
    IfFalseGo(ppRetVal, E_INVALIDARG);
    
    *ppRetVal = m_pSymMethod;
    m_pSymMethod->AddRef();

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetParent
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetParent(
    ISymUnmanagedScope *pScope,
    ISymUnmanagedScope **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetParent(ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetParent(
    ISymUnmanagedScope **ppRetVal
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(ppRetVal);
    IfFalseGo(ppRetVal, E_INVALIDARG);
    *ppRetVal = m_pParent;
    if (m_pParent)
    {
        m_pParent->AddRef();
    }
ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetChildren
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetChildren(
    ISymUnmanagedScope *pScope,
    ULONG32 cChildren,
    ULONG32 *pcChildren,
    ISymUnmanagedScope *children[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetChildren(cChildren, pcChildren, children);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetChildren(
    ULONG32 cChildren,    // [optional] Number of entries in children
    ULONG32 *pcChildren,  // [optional, out] Number of Children available for retur
    ISymUnmanagedScope *children[] // [optional] array to store children into
    )
{
    HRESULT hr = S_OK;
    ULONG32 ChildrenCount = 0;
    _ASSERTE(pcChildren || (children && cChildren));
    IfFalseGo((pcChildren || (children && cChildren)), E_INVALIDARG);

    if (m_pData->m_pScopes[m_ScopeEntry].HasChildren())
    {
        UINT32 ScopeEntry;
        for(ScopeEntry = m_pData->m_pMethods[m_MethodEntry].StartScopes();
            (ScopeEntry < m_pData->m_pMethods[m_MethodEntry].EndScopes());
            ScopeEntry++)
        {
            if (m_pData->m_pScopes[ScopeEntry].ParentScope() == m_ScopeEntry)
            {
                if (children && ChildrenCount < cChildren)
                {
                    SymScope *pScope;
                    // Found a child
                    IfNullGo(pScope = NEW(SymScope(m_pSymMethod, m_pData, this, m_MethodEntry, ScopeEntry)));
                    children[ChildrenCount] = pScope;
                    pScope->AddRef();
                }
                ChildrenCount++;
            }
        }
    }

    if (pcChildren)
    {
        *pcChildren = ChildrenCount;
    }

ErrExit:    
    if (FAILED(hr) && ChildrenCount)
    {
        unsigned i;
        for (i =0; i< ChildrenCount; i++)
        {
            RELEASE(children[i]);
        }
    }
    return hr;
}

//-----------------------------------------------------------
// GetStartOffset
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetStartOffset(
    ISymUnmanagedScope *pScope,
    ULONG32* pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetStartOffset(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetStartOffset(
    ULONG32* pRetVal
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pScopes[m_ScopeEntry].StartOffset();
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetEndOffset
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetEndOffset(
    ISymUnmanagedScope *pScope,
    ULONG32* pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetEndOffset(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetEndOffset(
    ULONG32* pRetVal
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);
    *pRetVal = m_pData->m_pScopes[m_ScopeEntry].EndOffset();
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetLocalCount
//-----------------------------------------------------------
HRESULT
SymScope::GetLocalCount(
    ULONG32 *pRetVal
    )
{
    HRESULT hr = S_OK;
    ULONG32 LocalCount = 0;
    _ASSERTE(pRetVal);
    IfFalseGo(pRetVal, E_INVALIDARG);

    // Init out parameter
    *pRetVal = 0;
    if (m_pData->m_pScopes[m_ScopeEntry].HasVars())
    {
        UINT32 var;
        // Walk and get the locals for this Scope
        for (var = m_pData->m_pMethods[m_MethodEntry].StartVars();
             var < m_pData->m_pMethods[m_MethodEntry].EndVars();
             var++)
        {
            if (m_pData->m_pVars[var].Scope() == m_ScopeEntry &&
                m_pData->m_pVars[var].IsParam() == false)
            {
                LocalCount++;
            }
        }       
    }

    *pRetVal = LocalCount;
ErrExit:    
    return hr;
}

//-----------------------------------------------------------
// GetLocals
// Input: either pcLocals or
//        cLocals and pLocals 
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetLocals(
    ISymUnmanagedScope *pScope,
    ULONG32 cLocals,
    ULONG32 *pcLocals,
    ISymUnmanagedVariable *pLocals[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetLocals(cLocals, pcLocals, pLocals);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetLocals(
    ULONG32 cLocals,    // [optional] available entries in pLocals
    ULONG32 *pcLocals,  // [optional, out] Number of locals returned
    ISymUnmanagedVariable *pLocals[] // [optional] array to store locals into
    )
{
    HRESULT hr = S_OK;

    ULONG32 LocalCount = 0;
    _ASSERTE(pcLocals || pLocals);
    IfFalseGo(pcLocals || pLocals, E_INVALIDARG);

    if (m_pData->m_pScopes[m_ScopeEntry].HasVars())
    {
        UINT32 var;
        // Walk and get the locals for this Scope
        for (var = m_pData->m_pMethods[m_MethodEntry].StartVars();
             var < m_pData->m_pMethods[m_MethodEntry].EndVars();
             var++)
        {
            if (m_pData->m_pVars[var].Scope() == m_ScopeEntry &&
                m_pData->m_pVars[var].IsParam() == false)
            {
                if (pLocals && LocalCount < cLocals)
                {
                    SymReaderVar *pVar;
                    IfNullGo( pVar = NEW(SymReaderVar(this, m_pData, var)));
                    pLocals[LocalCount] = pVar;
                    pVar->AddRef();
                }
                LocalCount++;
            }
        }       
    }
    if (pcLocals)
    {
        *pcLocals = LocalCount;
    }
ErrExit:
    if (FAILED(hr) && LocalCount != 0)
    {
        unsigned i;
        for (i =0; i < LocalCount; i++)
        {
            RELEASE(pLocals[i]);
        }
    }
    return hr;
}

//-----------------------------------------------------------
// GetNamespaces
// Input: either pcNameSpaces or
//        cNameSpaces and pNameSpaces 
//-----------------------------------------------------------
extern "C"
HRESULT SymScope_GetNamespaces(
    ISymUnmanagedScope *pScope,
    ULONG32 cNameSpaces,
    ULONG32 *pcNameSpaces,
    ISymUnmanagedNamespace *pNameSpaces[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pScope, E_INVALIDARG);
    hr = pScope->GetNamespaces(cNameSpaces, pcNameSpaces, pNameSpaces);
ErrExit:
    return hr;
}
HRESULT
SymScope::GetNamespaces(
    ULONG32 cNameSpaces,    // [optional] number of entries pNameSpaces
    ULONG32 *pcNameSpaces,  // [optional, out] Maximum number of Namespace
    ISymUnmanagedNamespace *pNameSpaces[] // [optinal] array to store namespaces into
    )
{
    HRESULT hr = NOERROR;
    unsigned i;
    UINT32 NameSpace;
    unsigned NameSpaceCount = 0;

    _ASSERTE(pcNameSpaces || (pNameSpaces && cNameSpaces));
    IfFalseGo(pcNameSpaces || (pNameSpaces && cNameSpaces), E_INVALIDARG);

    for (NameSpace = m_pData->m_pMethods[m_MethodEntry].StartUsing();
         NameSpace < m_pData->m_pMethods[m_MethodEntry].EndUsing();
         NameSpace++)
    {
        if (m_pData->m_pUsings[NameSpace].ParentScope() == m_ScopeEntry)
        {
            if (pNameSpaces)
            {
                IfNullGo(pNameSpaces[NameSpaceCount] = NEW(SymReaderNamespace(this, m_pData, NameSpace)));
                pNameSpaces[NameSpaceCount]->AddRef();
            }
            NameSpaceCount++;
        }
    }
    if (pcNameSpaces)
    {
       *pcNameSpaces = NameSpaceCount;
    }
ErrExit:
    if (FAILED(hr))
    {
        for (i = 0; i < NameSpaceCount; i++)
        {
            RELEASE(pNameSpaces[i]);
        }
    }
    return hr;
}

/* ------------------------------------------------------------------------- *
 * SymReaderVar class
 * ------------------------------------------------------------------------- */

extern "C"
HRESULT SymVariable_Release(ISymUnmanagedVariable *pVariable)
{
    if (pVariable)
        pVariable->Release();;
    return NOERROR;
}

//-----------------------------------------------------------
// QueryInterface
//-----------------------------------------------------------
HRESULT
SymReaderVar::QueryInterface(
    REFIID riid,
    void **ppInterface
    )
{
    if (riid == IID_ISymUnmanagedVariable)
        *ppInterface = (ISymUnmanagedVariable*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedVariable*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// GetName
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetName(
    ISymUnmanagedVariable *pVariable,
    ULONG32 cchName,
    ULONG32 *pcchName,
    WCHAR szName[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetName(cchName, pcchName, szName);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetName(
    ULONG32 cchName,    // [optional] Length of szName buffer
    ULONG32 *pcchName,  // [optional, out] Total size needed to return the name
    WCHAR szName[]      // [optional] Buffer to store the name into.
    )
{
    HRESULT hr = S_OK;

    // We must have at least one combination
    _ASSERTE(pcchName || (szName && cchName));
    IfFalseGo( (pcchName || (szName && cchName)), E_INVALIDARG );

    if (pcchName)
    {
        // Convert the UTF8 string to Wide
        *pcchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pVars[m_VarEntry].Name()]),
                                                -1,
                                                0,
                                                NULL);

    }
    if (szName)
    {
        // Convert the UTF8 string to Wide
        MultiByteToWideChar(CP_UTF8,
                            0,
                            (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pVars[m_VarEntry].Name()]),
                            -1,
                            szName,
                            cchName);
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetAttributes
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetAttributes(
    ISymUnmanagedVariable *pVariable,
    ULONG32 *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetAttributes(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetAttributes(
    ULONG32 *pRetVal // [out]
    )
{
    *pRetVal = m_pData->m_pVars[m_VarEntry].Attributes();
    return S_OK;
}

//-----------------------------------------------------------
// GetSignature
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetSignature(
    ISymUnmanagedVariable *pVariable,
    ULONG32 cSig,
    ULONG32 *pcSig,
    BYTE sig[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetSignature(cSig, pcSig, sig);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetSignature(
    ULONG32 cSig,   // Size of allocated buffer passed in (sig)
    ULONG32 *pcSig, // [optional, out] Total size needed to return the signature
    BYTE sig[] // [Optional] Signature
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(pcSig || sig);
    IfFalseGo( pcSig || sig, E_INVALIDARG );
    if (pcSig)
    {
        *pcSig = m_pData->m_pVars[m_VarEntry].SignatureSize();
    }
    if (sig)
    {
        cSig = min(m_pData->m_pVars[m_VarEntry].SignatureSize(), cSig);
        memcpy(sig, &m_pData->m_pBytes[m_pData->m_pVars[m_VarEntry].Signature()],cSig);
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetAddressKind
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetAddressKind(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetAddressKind(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetAddressKind(
    ULONG32 *pRetVal // [out]
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pRetVal);
    IfFalseGo( pRetVal, E_INVALIDARG );
    *pRetVal = m_pData->m_pVars[m_VarEntry].AddrKind();
ErrExit:
    return S_OK;
}

//-----------------------------------------------------------
// GetAddressField1
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetAddressField1(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetAddressField1(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetAddressField1(
    ULONG32 *pRetVal // [out]
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(pRetVal);
    IfFalseGo( pRetVal, E_INVALIDARG );
    
    *pRetVal = m_pData->m_pVars[m_VarEntry].Addr1();

ErrExit:
    
    return hr;
}

//-----------------------------------------------------------
// GetAddressField2
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetAddressField2(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetAddressField2(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetAddressField2(
    ULONG32 *pRetVal // [out]
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(pRetVal);
    IfFalseGo( pRetVal, E_INVALIDARG );
    
    *pRetVal = m_pData->m_pVars[m_VarEntry].Addr2();

ErrExit:
    
    return hr;
}

//-----------------------------------------------------------
// GetAddressField3
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetAddressField3(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetAddressField3(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetAddressField3(
    ULONG32 *pRetVal // [out]
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(pRetVal);
    IfFalseGo( pRetVal, E_INVALIDARG );
    
    *pRetVal = m_pData->m_pVars[m_VarEntry].Addr3();

ErrExit:
    
    return hr;
}

//-----------------------------------------------------------
// GetStartOffset
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetStartOffset(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetStartOffset(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetStartOffset(
    ULONG32 *pRetVal
    )
{
    //
    // This symbol reader doesn't support variable sub-offsets.
    //
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetEndOffset
//-----------------------------------------------------------
extern "C"
HRESULT SymVariable_GetEndOffset(
    ISymUnmanagedVariable *pVariable,
    mdMethodDef *pRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pVariable, E_INVALIDARG);
    hr = pVariable->GetEndOffset(pRetVal);
ErrExit:
    return hr;
}
HRESULT
SymReaderVar::GetEndOffset(
    ULONG32 *pRetVal
    )
{
    //
    // This symbol reader doesn't support variable sub-offsets.
    //
    return E_NOTIMPL;
}


/* ------------------------------------------------------------------------- *
 * SymReaderNamespace class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT SymNamespace_Release(ISymUnmanagedNamespace *pNamespace)
{
    if (pNamespace)
        pNamespace->Release();;
    return NOERROR;
}

//-----------------------------------------------------------
// QueryInterface
//-----------------------------------------------------------
HRESULT
SymReaderNamespace::QueryInterface(
    REFIID riid,
    void** ppInterface
    )
{
    if (riid == IID_ISymUnmanagedNamespace)
        *ppInterface = (ISymUnmanagedNamespace*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedNamespace*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// GetName
//-----------------------------------------------------------
extern "C"
HRESULT SymNamespace_GetName(
    ISymUnmanagedNamespace *pNamespace,
    ULONG32 cchName,
    ULONG32 *pcchName,
    WCHAR szName[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pNamespace, E_INVALIDARG);
    hr = pNamespace->GetName(cchName, pcchName, szName);
ErrExit:
    return hr;
}
HRESULT
SymReaderNamespace::GetName(
    ULONG32 cchName,    // [optional] Chars available in szName
    ULONG32 *pcchName,  // [optional] Total size needed to return the name
    WCHAR szName[]      // [optional] Location to store the name into.
    )
{
    HRESULT hr = S_OK;
    _ASSERTE(pcchName || (szName && cchName));
    IfFalseGo( (pcchName || (szName && cchName)), E_INVALIDARG );

    if (pcchName)
    {
        *pcchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pUsings[m_NamespaceEntry].Name()]),
                                                -1,
                                                0,
                                                NULL);
    }
    if (szName)
    {
        MultiByteToWideChar(CP_UTF8,
                            0,
                            (LPCSTR)&(m_pData->m_pStringsBytes[m_pData->m_pUsings[m_NamespaceEntry].Name()]),
                            -1,
                            szName,
                            cchName);
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// GetNamespaces
//-----------------------------------------------------------
extern "C"
HRESULT SymNamespace_GetNamespaces(
    ISymUnmanagedNamespace *pNamespace,
    ULONG32 cNameSpaces,
    ULONG32 *pcNameSpaces,
    ISymUnmanagedNamespace *namespaces[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pNamespace, E_INVALIDARG);
    hr = pNamespace->GetNamespaces(cNameSpaces, pcNameSpaces, namespaces);
ErrExit:
    return hr;
}
HRESULT
SymReaderNamespace::GetNamespaces(
    ULONG32 cNamespaces,
    ULONG32 *pcNamespaces,
    ISymUnmanagedNamespace* namespaces[]
    )
{
    // This symbol store doesn't support namespaces.
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// GetVariables
//-----------------------------------------------------------
extern "C"
HRESULT SymNamespace_GetVariables(
    ISymUnmanagedNamespace *pNamespace,
    ULONG32 cVars,
    ULONG32 *pcVars,
    ISymUnmanagedVariable *pVars[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pNamespace, E_INVALIDARG);
    hr = pNamespace->GetVariables(cVars, pcVars, pVars);
ErrExit:
    return hr;
}
HRESULT
SymReaderNamespace::GetVariables(
    ULONG32 cVariables,
    ULONG32 *pcVariables,
    ISymUnmanagedVariable *pVars[])
{
    // This symbol store doesn't support namespaces.
    _ASSERTE(!"NYI");
    return E_NOTIMPL;
}


/* ------------------------------------------------------------------------- *
 * SequencePoint struct functions
 * ------------------------------------------------------------------------- */

//-----------------------------------------------------------
// IsWithin - Is the point given within this sequence point
//-----------------------------------------------------------
bool SequencePoint::IsWithin(
    ULONG32 line,
    ULONG32 column)
{
    // If the sequence point starts on the same line
    // Check the start column (if present)
    if (StartLine() == line)
    {
        if (0 < column && StartColumn() > column)
        {
            return false;
        }
    }

    // If the sequence point ends on the same line
    // Check the end column
    if (EndLine() == line)
    {
        if (EndColumn() < column)
        {
            return false;
        }
    }

    // Make sure the line is within this sequence point
    if (!((StartLine() <= line) && (EndLine() >= line)))
    {
        return false;
    }

    // Yep it's within this sequence point
    return true;

}

//-----------------------------------------------------------
// IsGreaterThan - Is the sequence point greater than the position
//-----------------------------------------------------------
bool SequencePoint::IsGreaterThan(
    ULONG32 line,
    ULONG32 column)
{
    return (StartLine() > line) || 
           (StartLine() == line && StartColumn() > column);
}

//-----------------------------------------------------------
// IsLessThan - Is the sequence point less than the position
//-----------------------------------------------------------
bool SequencePoint::IsLessThan
(   
    ULONG32 line,
    ULONG32 column
)
{
    return (StartLine() < line) || 
           (StartLine() == line && StartColumn() < column);
}

//-----------------------------------------------------------
// IsUserLine - Is the sequence part of user code
//-----------------------------------------------------------
bool SequencePoint::IsUserLine()
{
    return StartLine() != CODE_WITH_NO_SOURCE;
}
