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
// LiteWeightStgdb.cpp
//
// This contains definition of class CLiteWeightStgDB. This is light weight
// read-only implementation for accessing compressed meta data format.
//
//*****************************************************************************
#include "stdafx.h"                     // Precompiled header.

#include "metamodelrw.h"
#include "liteweightstgdb.h"

// include stgdatabase.h for GUID_POOL_STREAM definition
// #include "stgdatabase.h"

// include StgTiggerStorage for TiggerStorage definition
#include "stgtiggerstorage.h"
#include "stgio.h"
#include "pedecoder.h"

#include <log.h>


#ifndef TYPELIB_SIG
#define TYPELIB_SIG_MSFT                    0x5446534D  // MSFT
#define TYPELIB_SIG_SLTG                    0x47544C53  // SLTG
#endif

//*****************************************************************************
// Checks the given storage object to see if it is an NT PE image.
//*****************************************************************************
int _IsNTPEImage(                       // true if file is NT PE image.
    StgIO       *pStgIO)                // Storage object.
{
    LONG        lfanew=0;               // Offset in DOS header to NT header.
    ULONG       lSignature=0;           // For NT header signature.
    HRESULT     hr;
    
    // Read DOS header to find the NT header offset.
    if (FAILED(hr = pStgIO->Seek(60, FILE_BEGIN)) ||
        FAILED(hr = pStgIO->Read(&lfanew, sizeof(LONG), 0)))
    {
        return (false);
    }

    // Seek to the NT header and read the signature.
    if (FAILED(hr = pStgIO->Seek(VAL32(lfanew), FILE_BEGIN)) ||
        FAILED(hr = pStgIO->Read(&lSignature, sizeof(ULONG), 0)) ||
        FAILED(hr = pStgIO->Seek(0, FILE_BEGIN)))
    {
        return (false);
    }

    // If the signature is a match, then we have a PE format.
    if (lSignature == VAL32(IMAGE_NT_SIGNATURE))
        return (true);
    else
        return (false);
}

BOOL _GetFileTypeForPathExt(StgIO *pStgIO, FILETYPE *piType)
{
    LPCWSTR     szPath;
    
    // Avoid confusion.
    *piType = FILETYPE_UNKNOWN;

    // If there is a path given, we can default to checking type.
    szPath = pStgIO->GetFileName();
    if (szPath && *szPath)
    {
        WCHAR       rcExt[_MAX_PATH];
        SplitPath(szPath, 0, 0, 0, 0, 0, 0, rcExt, _MAX_PATH);
        if (SString::_wcsicmp(rcExt, L".obj") == 0)
            *piType = FILETYPE_NTOBJ;
        else if (SString::_wcsicmp(rcExt, L".tlb") == 0)
            *piType = FILETYPE_TLB;
    }

    // All file types except .obj have a signature built in.  You should
    // not get to this code for those file types unless that file is corrupt,
    // or someone has changed a format without updating this code.
    _ASSERTE(*piType == FILETYPE_UNKNOWN || *piType == FILETYPE_NTOBJ || *piType == FILETYPE_TLB);

    // If we found a type, then you're ok.
    return (*piType != FILETYPE_UNKNOWN);
}

HRESULT _GetFileTypeForPath(StgIO *pStgIO, FILETYPE *piType)
{
    ULONG       lSignature=0;
    HRESULT     hr;
    
    // Assume native file.
    *piType = FILETYPE_CLB;

    // Need to read signature to see what type it is.
    if (!(pStgIO->GetFlags() & DBPROP_TMODEF_CREATE))
    {
        if (FAILED(hr = pStgIO->Read(&lSignature, sizeof(ULONG), 0)) ||
            FAILED(hr = pStgIO->Seek(0, FILE_BEGIN)))
        {
            return (hr);
        }
        lSignature = VAL32(lSignature);
        if (lSignature == STORAGE_MAGIC_SIG)
            *piType = FILETYPE_CLB;
        else if ((WORD) lSignature ==IMAGE_DOS_SIGNATURE && _IsNTPEImage(pStgIO))
            *piType = FILETYPE_NTPE;
        else if (lSignature == TYPELIB_SIG_MSFT || lSignature == TYPELIB_SIG_SLTG)
            *piType = FILETYPE_TLB;
        else if (!_GetFileTypeForPathExt(pStgIO, piType))
            return CLDB_E_FILE_CORRUPT;
    }
    return S_OK;
}


//*****************************************************************************
// Force generation of specialized versions.  While this may seem to defeat
//  the purpose of templates, it allows us to precisely control the 
//  specializations.  It also keeps the include file smaller.
//*****************************************************************************
void _nullRW()
{
    CLiteWeightStgdb<CMiniMdRW> RW;
        RW.Uninit();
}


//*****************************************************************************
// Prepare to go away.
//*****************************************************************************
CLiteWeightStgdbRW::~CLiteWeightStgdbRW()
{
    // Free up this stacks reference on the I/O object.
    if (m_pStgIO)
    {
        m_pStgIO->Release();
        m_pStgIO = NULL;
    }

    if (m_pStreamList) 
        delete m_pStreamList;
}

//*****************************************************************************
// Open an in-memory metadata section for read
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::InitOnMem(
    ULONG       cbData,                 // count of bytes in pData
    LPCVOID     pData,                  // points to meta data section in memory
    int         bReadOnly)              // If true, read-only.
{
    StgIO       *pStgIO = NULL;         // For file i/o.
    HRESULT     hr = NOERROR;

    if ((pStgIO = new (nothrow) StgIO) == 0)
        IfFailGo( E_OUTOFMEMORY);

    // Open the storage based on the pbData and cbData
    IfFailGo( pStgIO->Open(
        NULL,   // filename
        STGIO_READ, 
        pData, 
        cbData, 
        NULL,   // IStream*
        NULL)   // LPSecurityAttributes
         );

    IfFailGo( InitFileForRead(pStgIO, bReadOnly) );

ErrExit:
    if (SUCCEEDED(hr))
    {
        m_pStgIO = pStgIO;
    }
    else
    {
        if (pStgIO)
            pStgIO->Release();
    }
    return hr;
} // HRESULT CLiteWeightStgdbRW::InitOnMem()


//*****************************************************************************
// Given an StgIO, opens compressed streams and do proper initialization.
// This is a helper for other Init functions.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::InitFileForRead(
    StgIO       *pStgIO,                // For file i/o.
    int         bReadOnly)              // If read-only open.
{

    TiggerStorage *pStorage = 0;        // Storage object.
    void        *pvData;
    ULONG       cbData;
    HRESULT     hr = NOERROR;

    // Allocate a new storage object which has IStorage on it.
    if ((pStorage = new (nothrow) TiggerStorage) == 0)
        IfFailGo( E_OUTOFMEMORY);

    // Init the storage object on the backing storage.
    OptionValue ov;
    m_MiniMd.GetOption(&ov);
    IfFailGo( hr = pStorage->Init(pStgIO, ov.m_RuntimeVersion) );

    // Save pointers to header structure for version string.
    _ASSERTE(m_pvMd == NULL && m_cbMd == 0);
    IfFailGo(pStorage->GetHeaderPointer(&m_pvMd, &m_cbMd));

    // Check to see if this is a minimal metadata
    if (SUCCEEDED(pStorage->OpenStream(MINIMAL_MD_STREAM, &cbData, &pvData)))
        m_MiniMd.m_fMinimalDelta = TRUE;

    // Load the string pool.
    if (SUCCEEDED(hr=pStorage->OpenStream(STRING_POOL_STREAM, &cbData, &pvData)))
        IfFailGo( m_MiniMd.InitPoolOnMem(MDPoolStrings, pvData, cbData, bReadOnly) );
    else 
    {
        if (hr != STG_E_FILENOTFOUND)
            IfFailGo(hr);
        IfFailGo(m_MiniMd.InitPoolOnMem(MDPoolStrings, 0, 0, 0));
    }

    // Load the user string blob pool.
    if (SUCCEEDED(hr=pStorage->OpenStream(US_BLOB_POOL_STREAM, &cbData, &pvData)))
        IfFailGo( m_MiniMd.InitPoolOnMem(MDPoolUSBlobs, pvData, cbData, bReadOnly) );
    else 
    {
        if (hr != STG_E_FILENOTFOUND)
            IfFailGo(hr);
        IfFailGo(m_MiniMd.InitPoolOnMem(MDPoolUSBlobs, 0, 0, 0));
    }

    // Load the guid pool.
    if (SUCCEEDED(hr=pStorage->OpenStream(GUID_POOL_STREAM,  &cbData, &pvData)))
        IfFailGo( m_MiniMd.InitPoolOnMem(MDPoolGuids, pvData, cbData, bReadOnly) );
    else 
    {
        if (hr != STG_E_FILENOTFOUND)
            IfFailGo(hr);
        IfFailGo(m_MiniMd.InitPoolOnMem(MDPoolGuids, 0, 0, 0));
    }

    // Load the blob pool.
    if (SUCCEEDED(hr=pStorage->OpenStream(BLOB_POOL_STREAM, &cbData, &pvData)))
        IfFailGo( m_MiniMd.InitPoolOnMem(MDPoolBlobs, pvData, cbData, bReadOnly) );
    else 
    {
        if (hr != STG_E_FILENOTFOUND)
            IfFailGo(hr);
        IfFailGo(m_MiniMd.InitPoolOnMem(MDPoolBlobs, 0, 0, 0));
    }

    // Open the metadata.
    hr = pStorage->OpenStream(COMPRESSED_MODEL_STREAM, &cbData, &pvData);
    if (hr == STG_E_FILENOTFOUND)
    {
        IfFailGo(pStorage->OpenStream(ENC_MODEL_STREAM, &cbData, &pvData) );
    }
    IfFailGo( m_MiniMd.InitOnMem(pvData, cbData, bReadOnly) ); 
    IfFailGo( m_MiniMd.PostInit(0) );
    
ErrExit:
    if (pStorage)
        delete pStorage;
    return hr;
} // HRESULT CLiteWeightStgdbRW::InitFileForRead()

//*****************************************************************************
// Open a metadata section for read
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::OpenForRead(
    LPCWSTR     szDatabase,             // Name of database.
    void        *pbData,                // Data to open on top of, 0 default.
    ULONG       cbData,                 // How big is the data.
    DWORD       dwFlags)                // Flags for the open.
{
    LPCWSTR     pNoFile=L"";            // Constant for empty file name.
    StgIO       *pStgIO = NULL;         // For file i/o.
    HRESULT     hr;

    m_pImage = NULL;
    m_dwImageSize = 0;
    m_eFileType = FILETYPE_UNKNOWN;
    // szDatabase, and pbData are mutually exclusive.  Only one may be
    // non-NULL.  Having both NULL means empty stream creation.
    //
    _ASSERTE(!(szDatabase && (pbData)));
    _ASSERTE(!(pbData && (szDatabase)));

    // Open on memory needs there to be something to work with.
    if (pbData && cbData == 0)
        IfFailGo(CLDB_E_NO_DATA);

    // Make sure we have a path to work with.
    if (!szDatabase)
        szDatabase = pNoFile;

    // Sanity check the name.
    if (lstrlenW(szDatabase) >= _MAX_PATH)
        IfFailGo( E_INVALIDARG );

    // If we have storage to work with, init it and get type.
    if (*szDatabase || pbData)
    {
        // Allocate a storage instance to use for i/o.
        if ((pStgIO = new (nothrow) StgIO) == 0)
            IfFailGo( E_OUTOFMEMORY );

        DBPROPMODE dmOpenFlags = DBPROP_TMODEF_READ;

        // If we're taking ownership of this memory.....
        if (IsOfTakeOwnership(dwFlags))
            dmOpenFlags = (DBPROPMODE)(dmOpenFlags | DBPROP_TMODEF_SHAREDMEM);

        // Open the storage so we can read the signature if there is already data.
        IfFailGo( pStgIO->Open(szDatabase, 
                               dmOpenFlags, 
                               pbData, 
                               cbData, 
                               0, // IStream*
                               NULL) );         

        // Determine the type of file we are working with.
        IfFailGo( _GetFileTypeForPath(pStgIO, &m_eFileType) );
    }

    // Check for default type.
    if (m_eFileType == FILETYPE_CLB)
    {
        // If user wanted us to make a local copy of the data, do that now.
        if (IsOfCopyMemory(dwFlags))
            IfFailGo(pStgIO->LoadFileToMemory());

        // Try the native .clb file.
        IfFailGo( InitFileForRead(pStgIO, IsOfRead(dwFlags)) );
    }
    // PE/COFF executable/object format.  This requires us to find the .clb 
    // inside the binary before doing the Init.
    else if (m_eFileType == FILETYPE_NTPE || m_eFileType == FILETYPE_NTOBJ)
    {
        void        *ptr;
        ULONG       cbSize;

        // Map the entire binary for the FindImageMetaData function.
        IfFailGo( pStgIO->MapFileToMem(ptr, &cbSize) );

        // Find the .clb inside of the content.
        if (m_eFileType == FILETYPE_NTPE)
        {
            m_pImage = ptr;
            m_dwImageSize = cbSize;
            hr = FindImageMetaData(ptr, 
                                   cbSize, 
                                   pStgIO->GetMemoryMappedType() == MTYPE_IMAGE, 
                                   IsOfManifestMetadata(dwFlags), 
                                   &ptr, 
                                   &cbSize);
        }
        else
        {
            _ASSERTE(pStgIO->GetMemoryMappedType() != MTYPE_IMAGE);
            hr = FindObjMetaData(ptr, cbSize, &ptr, &cbSize);
        }
        // Was the metadata found inside the PE file?
        if (FAILED(hr))
        {   
            if (hr == E_OUTOFMEMORY)
                IfFailGo(E_OUTOFMEMORY);
        
            // No clb in the PE, assume it is a type library.
            m_eFileType = FILETYPE_TLB;

            // Let the caller deal with a TypeLib.
            IfFailGo(hr);
        }
        else
        {   
            // Metadata was found inside the file.
            // Now reset the base of the stg object so that all memory accesses
            // are relative to the .clb content.
            //
            IfFailGo( pStgIO->SetBaseRange(ptr, cbSize) );

            // If user wanted us to make a local copy of the data, do that now.
            if (IsOfCopyMemory(dwFlags))
            {
                // Cache the PEKind, Machine.
                GetPEKind(pStgIO->GetMemoryMappedType(), NULL, NULL);  
                // Copy the file into memory; releases the file.
                IfFailGo(pStgIO->LoadFileToMemory());
                // No longer have the image.
                m_pImage = NULL;
                m_dwImageSize = 0;
            }

            // Defer to the normal lookup.
            IfFailGo( InitFileForRead(pStgIO, IsOfRead(dwFlags)) );
        }
    }
    else if (m_eFileType == FILETYPE_TLB)
    {
        // Let the caller deal with a TypeLib.
        IfFailGo(CLDB_E_NO_DATA);
    }
    // This spells trouble, we need to handle all types we might find.
    else
    {
        _ASSERTE(!"Unknown file type.");
        IfFailGo( E_FAIL );
    }

    // Save off everything.
    wcscpy_s(m_rcDatabase, _MAX_PATH, szDatabase);

    // If this was a file...
    if (pbData == NULL)
    {
        WIN32_FILE_ATTRIBUTE_DATA faData;
        if (!WszGetFileAttributesEx(szDatabase, GetFileExInfoStandard, &faData))
            IfFailGo(E_FAIL);
        m_dwDatabaseLFS = faData.nFileSizeLow;
        m_dwDatabaseLFT = faData.ftLastWriteTime.dwLowDateTime;
    }
    
ErrExit:
 
    if (SUCCEEDED(hr))
    {
        m_pStgIO = pStgIO;
    }
    else
    {
        if (pStgIO)
            pStgIO->Release();
    }
    return (hr);
}

// Read/Write versions.
//*****************************************************************************
// Init the Stgdb and its subcomponents.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::InitNew()
{ 
    InitializeLogging();
    LOG((LF_METADATA, LL_INFO10, "Metadata logging enabled\n"));

    return m_MiniMd.InitNew();
}

//*****************************************************************************
// Determine what the size of the saved data will be.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::GetSaveSize(// S_OK or error.
    CorSaveSize fSave,                  // Quick or accurate?
    ULONG       *pulSaveSize,           // Put the size here.
    CorProfileData *profileData)        // Profile data for working set optimization
{
    HRESULT     hr = S_OK;              // A result.
    ULONG       cbTotal = 0;            // The total size.
    ULONG       cbSize = 0;             // Size of a component.

    m_cbSaveSize = 0;

    // Allocate stream list if not already done.
    if (!m_pStreamList)
        IfNullGo(m_pStreamList = new (nothrow) STORAGESTREAMLST);
    else
        m_pStreamList->Clear();

    // Make sure the user string pool is not empty. An empty user string pool causes
    // problems with edit and continue

    if(m_MiniMd.m_USBlobs.IsEmpty())
    {
        if(!(IsENCDelta(m_MiniMd.m_OptionValue.m_UpdateMode)||m_MiniMd.IsMinimalDelta()))
        {
            BYTE pbData[] = {' ', 0, 0};
            ULONG ulOffset;
            m_MiniMd.PutUserString(pbData,sizeof(pbData),&ulOffset);
        }
    }

    // If we're saving a delta metadata, figure out how much space it will take to
    // save the minimal metadata stream (used only to identify that we have a delta
    // metadata... nothing should be in that stream.
    if ((m_MiniMd.m_OptionValue.m_UpdateMode & MDUpdateMask) == MDUpdateDelta)
    {
        IfFailGo(AddStreamToList(0, MINIMAL_MD_STREAM));
        // Ask the storage system to add stream fixed overhead.
        IfFailGo(TiggerStorage::GetStreamSaveSize(MINIMAL_MD_STREAM, 0, &cbSize));
        cbTotal += cbSize;
    }


    // Query the MiniMd for its size.
    IfFailGo(GetTablesSaveSize(fSave, &cbSize, profileData));
    cbTotal += cbSize;

    // Get the pools' sizes.
    IfFailGo(GetPoolSaveSize(STRING_POOL_STREAM, MDPoolStrings, &cbSize));
    cbTotal += cbSize;
    IfFailGo(GetPoolSaveSize(US_BLOB_POOL_STREAM, MDPoolUSBlobs, &cbSize));
    cbTotal += cbSize;
    IfFailGo(GetPoolSaveSize(GUID_POOL_STREAM, MDPoolGuids, &cbSize));
    cbTotal += cbSize;
    IfFailGo(GetPoolSaveSize(BLOB_POOL_STREAM, MDPoolBlobs, &cbSize));
    cbTotal += cbSize;

    // Finally, ask the storage system to add fixed overhead it needs for the
    // file format.  The overhead of each stream has already be calculated as
    // part of GetStreamSaveSize.  What's left is the signature and header
    // fixed size overhead.
    IfFailGo(TiggerStorage::GetStorageSaveSize(&cbTotal, 0, m_MiniMd.m_OptionValue.m_RuntimeVersion));

    // Log the size info.
    LOG((LF_METADATA, LL_INFO10, "Metadata: GetSaveSize total is %d.\n", cbTotal));
    
    // The list of streams that will be saved are now in the stream save list.
    // Next step is to walk that list and fill out the correct offsets.  This is 
    // done here so that the data can be streamed without fixing up the header.
    TiggerStorage::CalcOffsets(m_pStreamList, 0, m_MiniMd.m_OptionValue.m_RuntimeVersion);

    if (pulSaveSize)
        *pulSaveSize = cbTotal;

    // Don't cache the value for the EnC case
    if (!IsENCDelta(m_MiniMd.m_OptionValue.m_UpdateMode))
        m_cbSaveSize = cbTotal;

ErrExit:
    return hr;
} // HRESULT CLiteWeightStgdbRW::GetSaveSize()

//*****************************************************************************
// Get the save size of one of the pools.  Also adds the pool's stream to
//  the list of streams to be saved.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::GetPoolSaveSize(
    LPCWSTR     szHeap,                 // Name of the heap stream.
    int         iPool,                  // The pool of which to get size.
    ULONG       *pcbSaveSize)           // Add pool data to this value.
{
    ULONG       cbSize = 0;             // Size of pool data.
    ULONG       cbStream;               // Size of just the stream.
    HRESULT     hr;

    *pcbSaveSize = 0;

    // If there is no data, then don't bother.
    if (m_MiniMd.IsPoolEmpty(iPool))
        return (S_OK);

    // Ask the pool to size its data.
    IfFailGo(m_MiniMd.GetPoolSaveSize(iPool, &cbSize));
    cbStream = cbSize;

    // Add this item to the save list.
    IfFailGo(AddStreamToList(cbSize, szHeap));


    // Ask the storage system to add stream fixed overhead.
    IfFailGo(TiggerStorage::GetStreamSaveSize(szHeap, cbSize, &cbSize));

    // Log the size info.
    LOG((LF_METADATA, LL_INFO10, "Metadata: GetSaveSize for %ls: %d data, %d total.\n",
        szHeap, cbStream, cbSize));

    // Give the size of the pool to the caller's total.  
    *pcbSaveSize = cbSize;

ErrExit:
    return hr;
}

//*****************************************************************************
// Get the save size of the metadata tables.  Also adds the tables stream to
//  the list of streams to be saved.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::GetTablesSaveSize(
    CorSaveSize fSave,
    ULONG       *pcbSaveSize,
    CorProfileData *profileData)           // Add pool data to this value.
{
    ULONG       cbSize = 0;             // Size of pool data.
    ULONG       cbHotSize = 0;          // Size of pool data.
    ULONG       cbStream;               // Size of just the stream.
    ULONG       bCompressed;            // Will the stream be compressed data?
    LPCWSTR     szName;                 // What will the name of the pool be?
    HRESULT     hr;

    *pcbSaveSize = 0;

    if (profileData != NULL)
    {
        // Ask the metadata to size its hot data.
        IfFailGo(m_MiniMd.GetSaveSize(fSave, &cbHotSize, &bCompressed, profileData));
        cbStream = cbHotSize;
        m_bSaveCompressed = bCompressed;

        if (cbHotSize != 0)
        {
            szName = HOT_MODEL_STREAM;

            // Add this item to the save list.
            IfFailGo(AddStreamToList(cbHotSize, szName));
    
            // Ask the storage system to add stream fixed overhead.
            IfFailGo(TiggerStorage::GetStreamSaveSize(szName, cbHotSize, &cbHotSize));

            // Log the size info.
            LOG((LF_METADATA, LL_INFO10, "Metadata: GetSaveSize for %ls: %d data, %d total.\n",
                szName, cbStream, cbHotSize));
        }
    }
    // Ask the metadata to size its data.
    IfFailGo(m_MiniMd.GetSaveSize(fSave, &cbSize, &bCompressed));
    cbStream = cbSize;
    m_bSaveCompressed = bCompressed;
    szName = m_bSaveCompressed ? COMPRESSED_MODEL_STREAM : ENC_MODEL_STREAM;

    // Add this item to the save list.
    IfFailGo(AddStreamToList(cbSize, szName));
    
    // Ask the storage system to add stream fixed overhead.
    IfFailGo(TiggerStorage::GetStreamSaveSize(szName, cbSize, &cbSize));

    // Log the size info.
    LOG((LF_METADATA, LL_INFO10, "Metadata: GetSaveSize for %ls: %d data, %d total.\n",
        szName, cbStream, cbSize));

    // Give the size of the pool to the caller's total.  
    *pcbSaveSize = cbHotSize + cbSize;

ErrExit:
    return hr;
} // HRESULT CLiteWeightStgdbRW::GetTablesSaveSize()

//*****************************************************************************
// Add a stream, and its size, to the list of streams to be saved.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::AddStreamToList(
    ULONG       cbSize,
    LPCWSTR     szName)
{
    HRESULT     hr = S_OK;
    PSTORAGESTREAM pItem;               // New item to allocate & fill.

    // Add a new item to the end of the list.
    IfNullGo(pItem = m_pStreamList->Append());

    // Fill out the data.
    pItem->SetOffset(0);
    pItem->SetSize(cbSize);
    pItem->SetName(szName);

ErrExit:
    return hr;
}

//*****************************************************************************
// Save the data to a stream.  A TiggerStorage sub-allocates streams within
//   the stream.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::SaveToStream(
    IStream     *pIStream,
    CorProfileData *profileData)
{
    HRESULT     hr = S_OK;              // A result.
    StgIO       *pStgIO = 0;
    TiggerStorage *pStorage = 0;

    // Allocate a storage subsystem and backing store.
    IfNullGo(pStgIO = new (nothrow) StgIO);
    IfNullGo(pStorage = new (nothrow) TiggerStorage);


    // Open around this stream for write.
    IfFailGo(pStgIO->Open(L"", 
        DBPROP_TMODEF_DFTWRITEMASK, 
        0, 0,                           // pbData, cbData
        pIStream,
        0));                            // LPSecurityAttributes
    OptionValue ov;
    m_MiniMd.GetOption(&ov);
    IfFailGo( pStorage->Init(pStgIO, ov.m_RuntimeVersion) );

    // Save worker will do tables, pools.
    IfFailGo(SaveToStorage(pStorage, profileData));

ErrExit:

    if (pStgIO)
        pStgIO->Release();
    if (pStorage)
        delete pStorage;
    return hr;
} // HRESULT CLiteWeightStgdbRW::PersistToStream()

//*****************************************************************************
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::SaveToStorage(
    TiggerStorage *pStorage,
    CorProfileData *profileData)
{
    HRESULT     hr;                     // A result.
    LPCWSTR     szName;                 // Name of the tables stream.
    IStream     *pIStreamTbl = 0;
    ULONG       cb;
    ULONG       cbSaveSize = m_cbSaveSize;

    // Must call GetSaveSize to cache the streams up front.
    // Don't trust cached values in the delta case... if there was a previous call to get 
    // a non-delta size, it will be incorrect.
    if (!m_cbSaveSize || IsENCDelta(m_MiniMd.m_OptionValue.m_UpdateMode))
        IfFailGo(GetSaveSize(cssAccurate, &cbSaveSize));

    // Save the header of the data file.
    IfFailGo(pStorage->WriteHeader(m_pStreamList, 0, NULL));

    // If this is a minimal delta, write a stream marker 
    if (IsENCDelta(m_MiniMd.m_OptionValue.m_UpdateMode))
    {
        IfFailGo(pStorage->CreateStream(MINIMAL_MD_STREAM, 
            STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
            0, 0, &pIStreamTbl));
        pIStreamTbl->Release();
        pIStreamTbl = 0;
    }
    
    if (profileData != NULL)
    {
        ULONG       bCompressed;
        ULONG       cbHotSize;
        // Will the stream be compressed data?
        
        // Only create this additional stream if it will be non-empty
        IfFailGo(m_MiniMd.GetSaveSize(cssAccurate, &cbHotSize, &bCompressed, profileData));

        if (cbHotSize > 0)
        {
            // Create a stream and save the hot tables.
            szName = HOT_MODEL_STREAM;
            IfFailGo(pStorage->CreateStream(szName, 
                    STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
                    0, 0, &pIStreamTbl));
            IfFailGo(m_MiniMd.SaveTablesToStream(pIStreamTbl, profileData));
            pIStreamTbl->Release();
            pIStreamTbl = 0;
        }
    }

    // Create a stream and save the tables.
    szName = m_bSaveCompressed ? COMPRESSED_MODEL_STREAM : ENC_MODEL_STREAM;
    IfFailGo(pStorage->CreateStream(szName, 
            STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
            0, 0, &pIStreamTbl));
    IfFailGo(m_MiniMd.SaveTablesToStream(pIStreamTbl, NULL));
    pIStreamTbl->Release();
    pIStreamTbl = 0;

    // Save the pools.
    IfFailGo(SavePool(STRING_POOL_STREAM, pStorage, MDPoolStrings));
    IfFailGo(SavePool(US_BLOB_POOL_STREAM, pStorage, MDPoolUSBlobs));
    IfFailGo(SavePool(GUID_POOL_STREAM, pStorage, MDPoolGuids));
    IfFailGo(SavePool(BLOB_POOL_STREAM, pStorage, MDPoolBlobs));

    // Write the header to disk.
    OptionValue ov;
    m_MiniMd.GetOption(&ov);
    
    IfFailGo(pStorage->WriteFinished(m_pStreamList, &cb, IsENCDelta(ov.m_UpdateMode)));

    _ASSERTE(cbSaveSize == cb);

    // Let the Storage release some memory.
    pStorage->ResetBackingStore();

    m_MiniMd.SaveDone();

ErrExit:
    if (pIStreamTbl)
        pIStreamTbl->Release();
    delete m_pStreamList;
    m_pStreamList = 0;
    m_cbSaveSize = 0;
    return hr;
} // HRESULT CLiteWeightStgdbRW::SaveToStorage()

//*****************************************************************************
// Save a pool of data out to a stream.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::SavePool(   // Return code.
    LPCWSTR     szName,                 // Name of stream on disk.
    TiggerStorage *pStorage,            // The storage to put data in.
    int         iPool)                  // The pool to save.
{
    IStream     *pIStream=0;            // For writing.
    HRESULT     hr;

    // If there is no data, then don't bother.
    if (m_MiniMd.IsPoolEmpty(iPool))
        return (S_OK);

    // Create the new stream to hold this table and save it.
    IfFailGo(pStorage->CreateStream(szName, 
            STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
            0, 0, &pIStream));
    IfFailGo(m_MiniMd.SavePoolToStream(iPool, pIStream));

ErrExit:
    if (pIStream)
        pIStream->Release();
    return hr;
} // HRESULT CLiteWeightStgdbRW::SavePool()


//*****************************************************************************
// Save the metadata to a file.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::Save(       // S_OK or error.
    LPCWSTR     szDatabase,             // Name of file to which to save.
    DWORD       dwSaveFlags)            // Flags for the save.
{
    TiggerStorage *pStorage=0;          // IStorage object.
    StgIO       *pStgIO=0;              // Backing storage.
    HRESULT     hr = S_OK;

    if (!*m_rcDatabase)
    {
        if (!szDatabase)
        {
            // Make sure that a NULL is not passed in the first time around.
            _ASSERTE(!"Not allowed to pass a NULL for filename on the first call to Save.");
            return (E_INVALIDARG);
        }
        else
        {
            // Save the file name.
            wcscpy_s(m_rcDatabase, _MAX_PATH, szDatabase);
        }
    }
    else if (szDatabase && SString::_wcsicmp(szDatabase, m_rcDatabase) != 0)
    {
        // Allow for same name, case of multiple saves during session.
        // Changing the name on a scope which is already opened is not allowed.
        // Save the file name.
        wcscpy_s(m_rcDatabase, _MAX_PATH, szDatabase);
    }

    // Sanity check the name.
    if (lstrlenW(m_rcDatabase) >= _MAX_PATH)
        IfFailGo(E_INVALIDARG);

    m_eFileType = FILETYPE_CLB;

    // Allocate a new storage object.
    IfNullGo(pStgIO = new (nothrow) StgIO);

    // Create the output file.
    IfFailGo(pStgIO->Open(m_rcDatabase, 
        DBPROP_TMODEF_DFTWRITEMASK,
        0,0,                // pbData, cbData
        0,                  // IStream*
        0));                // LPSecurityAttributes

    // Allocate an IStorage object to use.
    IfNullGo(pStorage = new (nothrow) TiggerStorage);

    // Init the storage object on the i/o system.
    OptionValue ov;
    m_MiniMd.GetOption(&ov);
    IfFailGo( pStorage->Init(pStgIO, ov.m_RuntimeVersion) );

    // Save the data.
    IfFailGo(SaveToStorage(pStorage));

ErrExit:
    if (pStgIO)
        pStgIO->Release();
    if (pStorage)
        delete pStorage;
    return (hr);
} // HRESULT CLiteWeightStgdbRW::Save()

//*****************************************************************************
// Pull the PEKind and Machine out of PE headers -- if we have PE headers.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::GetPEKind(  // S_OK or error.
    MAPPINGTYPE mtMapping,              // The type of mapping the image has
    DWORD       *pdwPEKind,             // [OUT] The kind of PE (0 - not a PE)
    DWORD       *pdwMachine)            // [OUT] Machine as defined in NT header
{
    HRESULT     hr = NOERROR;
    DWORD       dwKind=0;               // Working copy of pe kind.
    DWORD       dwMachine=0;            // Working copy of machine.

#ifndef DACCESS_COMPILE
    // Do we already have cached information?
    if (m_dwPEKind != (DWORD)(-1))
    {
        dwKind = m_dwPEKind;
        dwMachine = m_dwMachine;
    }
    else if (m_pImage)
    {
        NewHolder<PEDecoder> pe;

        EX_TRY
        {
            // We need to use different PEDecoder constructors based on the type of data we give it.
            // We use the one with a 'bool' as the second argument when dealing with a mapped file,
            // and we use the one that takes a COUNT_T as the second argument when dealing with a
            // flat file.

            if (mtMapping == MTYPE_IMAGE)
                pe = new (nothrow) PEDecoder(m_pImage, false);
            else
                pe = new (nothrow) PEDecoder(m_pImage, (COUNT_T)(m_dwImageSize));
        
        }
        EX_CATCH
        {
            hr = COR_E_BADIMAGEFORMAT;
        }
        EX_END_CATCH(SwallowAllExceptions)

        IfFailRet(hr);
        IfNullRet(pe);


        if (pe->HasContents() && pe->HasNTHeaders())
        {
            pe->GetPEKindAndMachine(&dwKind,&dwMachine);
            // Cache entries.
            m_dwPEKind = dwKind;
            m_dwMachine = dwMachine;
        }
        else // if (pe.HasContents()...
            hr = COR_E_BADIMAGEFORMAT;
    } // if (m_pImage)
    else 
        hr = S_FALSE;
#endif
    if (pdwPEKind) 
        *pdwPEKind = dwKind;
    if (pdwMachine) 
        *pdwMachine = dwMachine;

    return hr;
} // HRESULT CLiteWeightStgdbRW::GetPEKind()

//*****************************************************************************
// Low level access to the data.  Intended for metainfo, and such.
//*****************************************************************************
HRESULT CLiteWeightStgdbRW::GetRawData(
    const void **ppvMd,                 // [OUT] put pointer to MD section here (aka, 'BSJB').
    ULONG   *pcbMd)                     // [OUT] put size of the stream here.
{
    *ppvMd = (const void*) m_pStgIO->m_pData;
    *pcbMd = m_pStgIO->m_cbData;
    return S_OK;
} // HRESULT CLiteWeightStgdbRW::GetRawData()

//*****************************************************************************
// Low level access to stream data.  Intended for metainfo, and such.
//*****************************************************************************
STDMETHODIMP CLiteWeightStgdbRW::GetRawStreamInfo(          // Get info about the MD stream.
    ULONG   ix,                         // [IN] Stream ordinal desired.
    const char **ppchName,              // [OUT] put pointer to stream name here.
    const void **ppv,                   // [OUT] put pointer to MD stream here.
    ULONG   *pcb)                       // [OUT] put size of the stream here.
{
    void *pData = m_pStgIO->m_pData;
    ULONG cbData = m_pStgIO->m_cbData;
    
    HRESULT     hr = NOERROR;
    STORAGEHEADER sHdr;                 // Header for the storage.
    PSTORAGESTREAM pStream;             // Pointer to each stream.
    ULONG       i;                      // Loop control.

    // Validate the signature of the format, or it isn't ours.
    IfFailGo(MDFormat::VerifySignature((PSTORAGESIGNATURE) pData, cbData));

    // Get back the first stream.
    pStream = MDFormat::GetFirstStream(&sHdr, pData);
    if (pStream == NULL)
        IfFailGo(CLDB_E_FILE_CORRUPT);

    // Check that the requested stream exists.
    if (ix >= sHdr.GetiStreams())
        return S_FALSE;

    // Skip to the desired stream.
    for (i=0;  i<ix;  i++)
    {
        PSTORAGESTREAM pNext = pStream->NextStream();

        // Check that stream header is within the buffer.
        if ((LPBYTE) pStream >= (LPBYTE) pData + cbData ||
            (LPBYTE) pNext   >  (LPBYTE) pData + cbData )
        {
            hr = CLDB_E_FILE_CORRUPT;
            goto ErrExit;
        }

        // Check that the stream data starts and fits within the buffer.
        //  need two checks on size because of wraparound.
        if (pStream->GetOffset() > cbData   ||
            pStream->GetSize() > cbData     ||
            (pStream->GetSize() + pStream->GetOffset()) > cbData)
        {
            hr = CLDB_E_FILE_CORRUPT;
            goto ErrExit;
        }

        // Pick off the next stream if there is one.
        pStream = pNext;
    }
    
    if (pStream)
    {
        *ppv = (const void *)((const BYTE *)pData + pStream->GetOffset());
        *pcb = pStream->GetSize();
        *ppchName = pStream->GetName();
    }
    else
    {
        *ppv = NULL;
        *pcb = 0;
        *ppchName = NULL;
        hr = CLDB_E_FILE_CORRUPT;
    }

ErrExit:
    return hr;
} // STDMETHODIMP CLiteWeightStgdbRW::GetRawStreamInfo()


// eof ------------------------------------------------------------------------
