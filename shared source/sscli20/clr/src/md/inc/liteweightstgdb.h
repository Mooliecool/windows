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
// LiteWeightStgdb.h
//
// This contains definition of class CLiteWeightStgDB. This is light weight
// read-only implementation for accessing compressed meta data format.
//
//*****************************************************************************
#ifndef __LiteWeightStgdb_h__
#define __LiteWeightStgdb_h__

#include "metamodelro.h"
#include "metamodelrw.h"

#include "stgtiggerstorage.h"

class StgIO;

#include "mdcommon.h"

class TiggerStorage;

//*****************************************************************************
// This class provides common definitions for heap segments.  It is both the
//  base class for the heap, and the class for heap extensions (additional
//  memory that must be allocated to grow the heap).
//*****************************************************************************
template <class MiniMd>
class CLiteWeightStgdb
{
public:
    CLiteWeightStgdb() : m_pvMd(NULL), m_cbMd(0)
    {}

    ~CLiteWeightStgdb() 
    { Uninit(); }

    // open an in-memory metadata section for read.
    HRESULT InitOnMem(  
        ULONG cbData,
        LPCVOID pbData);

    void InitHotPools( ULONG * hotMetadataDir );

    void Uninit();

protected:
    MiniMd      m_MiniMd;               // embedded compress meta data schemas definition
    const void  *m_pvMd;                // Pointer to meta data.
    ULONG       m_cbMd;                 // Size of the meta data.

    friend class CorMetaDataScope;
    friend class COR;
    friend class RegMeta;
    friend class MERGER;
    friend class NEWMERGER;
    friend class MDInternalRO;
    friend class MDInternalRW;
};

//*****************************************************************************
// Open an in-memory metadata section for read
//*****************************************************************************
template <class MiniMd>
void CLiteWeightStgdb<MiniMd>::Uninit()
{
    m_MiniMd.m_Strings.Uninit();
    m_MiniMd.m_USBlobs.Uninit();
    m_MiniMd.m_Guids.Uninit();
    m_MiniMd.m_Blobs.Uninit();
    m_pvMd = NULL;
    m_cbMd = 0;
}

class CLiteWeightStgdbRW : public CLiteWeightStgdb<CMiniMdRW>
{
    friend class CImportTlb;
    friend class RegMeta;
public:
    CLiteWeightStgdbRW() : m_cbSaveSize(0), m_pStreamList(0), m_pNextStgdb(NULL), m_pStgIO(NULL)
    { *m_rcDatabase= 0; m_pImage = NULL; m_dwImageSize = 0; m_dwPEKind = (DWORD)(-1); m_dwDatabaseLFS=0; m_dwDatabaseLFT=0;}
    ~CLiteWeightStgdbRW();

    HRESULT InitNew();

    // open an in-memory metadata section for read.
    HRESULT InitOnMem(  
        ULONG cbData,
        LPCVOID pbData,
        int     bReadOnly);

    HRESULT GetSaveSize(
        CorSaveSize fSize,
        ULONG       *pulSaveSize,
        CorProfileData *profileData = NULL);// optional IBC profile data for working set optimization

    HRESULT SaveToStream(
        IStream     *pIStream,              // Stream to which to write
        CorProfileData *profileData = NULL);// optional IBC profile data for working set optimization

    HRESULT Save(
        LPCWSTR     szFile, 
        DWORD       dwSaveFlags);

    // Open a metadata section for read/write
    HRESULT OpenForRead(
        LPCWSTR     szDatabase,             // Name of database.
        void        *pbData,                // Data to open on top of, 0 default.
        ULONG       cbData,                 // How big is the data.
        DWORD       dwFlags);               // Flags for the open.


    HRESULT FindImageMetaData(
        PVOID pImage,                       // Pointer to head of a file
        DWORD dwFileLength,                 // length of a flat file
        BOOL  bMappedImage,                 // Is the file mapped
        BOOL  bGetManifestMetadata,         // Should read manifest metadata
        PVOID *ppMetaData,                  // [out] pointer to the metadata
        ULONG *pcbMetaData);                // [out] size of the metadata

    
    HRESULT FindObjMetaData(
        PVOID pImage,                       // Pointer to an OBJ file
        DWORD dwFileLength,                 // Length of the file
        PVOID *ppMetaData,                  // [out] pointer to the metadata
        ULONG *pcbMetaData);                // [out] size of the metadata



    HRESULT CLiteWeightStgdbRW::GetPEKind(  // S_OK or error.
        MAPPINGTYPE mtMapping,              // The type of mapping the image has
        DWORD* pdwPEKind,                   // [OUT] The kind of PE (0 - not a PE)
        DWORD* pdwMachine);                 // [OUT] Machine as defined in NT header

    // Low level data access; not useful for most clients.
    HRESULT GetRawData(
        const void **ppvMd,                 // [OUT] put pointer to MD section here (aka, 'BSJB').
        ULONG   *pcbMd);                    // [OUT] put size of the stream here.
    STDMETHODIMP GetRawStreamInfo(          // Get info about the MD stream.
        ULONG   ix,                         // [IN] Stream ordinal desired.
        const char **ppchName,              // [OUT] put pointer to stream name here.
        const void **ppv,                   // [OUT] put pointer to MD stream here.
        ULONG   *pcb);                      // [OUT] put size of the stream here.

    ULONG       m_cbSaveSize;               // Size of the saved streams.
    int         m_bSaveCompressed;          // If true, save as compressed stream (#-, not #~)
    VOID*       m_pImage;                   // Set in OpenForRead, NULL for anything but PE files
    DWORD       m_dwImageSize;              // On-disk size of image

protected:
    DWORD       m_dwPEKind;                 // The kind of PE - 0: not a PE.
    DWORD       m_dwMachine;                // Machine as defined in NT header.

    HRESULT CLiteWeightStgdbRW::GetPoolSaveSize(
        LPCWSTR     szHeap,                 // Name of the heap stream.
        int         iPool,                  // The pool whose size to get.
        ULONG       *pcbSaveSize);           // Add pool data to this value.
    HRESULT CLiteWeightStgdbRW::GetTablesSaveSize(
        CorSaveSize fSave,
        ULONG       *pcbSaveSize,
        CorProfileData *profileData = NULL);// Add pool data to this value.
    HRESULT CLiteWeightStgdbRW::AddStreamToList(
        ULONG       cbSize,                 // Size of the stream data.
        LPCWSTR     szName);                // Name of the stream.
    HRESULT SaveToStorage(TiggerStorage *pStorage, CorProfileData *profileData = NULL);
    HRESULT SavePool(LPCWSTR szName, TiggerStorage *pStorage, int iPool);

    STORAGESTREAMLST *m_pStreamList;
    
    HRESULT InitFileForRead(            
        StgIO       *pStgIO,            // For file i/o.
        int         bReadOnly=true);    // If read-only.

    CLiteWeightStgdbRW *m_pNextStgdb;

public:
    FORCEINLINE FILETYPE GetFileType() { return m_eFileType; }

private:
    FILETYPE    m_eFileType;
    WCHAR       m_rcDatabase[_MAX_PATH];// Name of this database.
    DWORD       m_dwDatabaseLFT; // Low bytes of the database file's last write time
    DWORD       m_dwDatabaseLFS; // Low bytes of the database file's size  
    StgIO       *m_pStgIO;              // For file i/o.
};

#endif // __LiteWeightStgdb_h__
