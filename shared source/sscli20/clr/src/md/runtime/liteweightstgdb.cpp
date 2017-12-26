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
#include "stdafx.h" 					// Precompiled header.
#include "mdfileformat.h"
#include "metamodelro.h"
#include "liteweightstgdb.h"
#include "metadatatracker.h"


HRESULT _CallInitOnMemHelper(CLiteWeightStgdb<CMiniMd> *pStgdb, ULONG cbData, LPCVOID pData)
{
    return pStgdb->InitOnMem(cbData,pData);
}

//*****************************************************************************
// Open an in-memory metadata section for read
//*****************************************************************************
template <class MiniMd>
HRESULT CLiteWeightStgdb<MiniMd>::InitOnMem(
	ULONG		cbData,					// count of bytes in pData
	LPCVOID 	pData)					// points to meta data section in memory
{
	STORAGEHEADER sHdr;					// Header for the storage.
	PSTORAGESTREAM pStream;				// Pointer to each stream.
	int			bFoundMd = false;		// true when compressed data found.
	int			i;						// Loop control.
	HRESULT		hr = S_OK;

	// Don't double open.
	_ASSERTE(m_pvMd == NULL && m_cbMd == 0);

	// Validate the signature of the format, or it isn't ours.
	if (FAILED(hr = MDFormat::VerifySignature((PSTORAGESIGNATURE) pData, cbData)))
		goto ErrExit;

    m_MiniMd.m_hotTableDirectory = NULL;

	// Get back the first stream.
	pStream = MDFormat::GetFirstStream(&sHdr, pData);
    if (pStream == NULL)
        IfFailGo(CLDB_E_FILE_CORRUPT);

	// Loop through each stream and pick off the ones we need.
	for (i=0;  i<sHdr.GetiStreams();  i++)
	{        
		// Pick off the location and size of the data.
		void *pvCurrentData = (void *) ((BYTE *) pData + pStream->GetOffset());
		ULONG cbCurrentData = pStream->GetSize();


        // Get next stream 
        PSTORAGESTREAM pNext = pStream->NextStream();


        // Range check
        if ((LPBYTE) pStream >= (LPBYTE) pData + cbData ||
            (LPBYTE) pNext   >  (LPBYTE) pData + cbData )
        {
            IfFailGo( CLDB_E_FILE_CORRUPT );
        }

        if ( (LPBYTE) pvCurrentData                 >= (LPBYTE) pData + cbData ||
             (LPBYTE) pvCurrentData + cbCurrentData >  (LPBYTE) pData + cbData )
        {
            IfFailGo( CLDB_E_FILE_CORRUPT );
        }

        
        // String pool.
        if (strcmp(pStream->GetName(), STRING_POOL_STREAM_A) == 0)
        {
            METADATATRACKER_ONLY(MetaDataTracker::NoteSection(TBL_COUNT + MDPoolStrings, pvCurrentData, cbCurrentData, 1));
            IfFailGo( m_MiniMd.m_Strings.InitOnMemReadOnly(pvCurrentData, cbCurrentData) );
        }

        // Literal String Blob pool.
        else if (strcmp(pStream->GetName(), US_BLOB_POOL_STREAM_A) == 0)
        {
            METADATATRACKER_ONLY(MetaDataTracker::NoteSection(TBL_COUNT + MDPoolUSBlobs, pvCurrentData, cbCurrentData, 1));
            IfFailGo( m_MiniMd.m_USBlobs.InitOnMemReadOnly(pvCurrentData, cbCurrentData) );
        }

        // GUID pool.
        else if (strcmp(pStream->GetName(), GUID_POOL_STREAM_A) == 0)
        {
            METADATATRACKER_ONLY(MetaDataTracker::NoteSection(TBL_COUNT + MDPoolGuids, pvCurrentData, cbCurrentData, 1));
            IfFailGo( m_MiniMd.m_Guids.InitOnMemReadOnly(pvCurrentData, cbCurrentData) );
        }

        // Blob pool.
        else if (strcmp(pStream->GetName(), BLOB_POOL_STREAM_A) == 0)
        {
            METADATATRACKER_ONLY(MetaDataTracker::NoteSection(TBL_COUNT + MDPoolBlobs, pvCurrentData, cbCurrentData, 1));
            IfFailGo( m_MiniMd.m_Blobs.InitOnMemReadOnly(pvCurrentData, cbCurrentData) );
        }

        // Found the compressed meta data stream.
        else if (strcmp(pStream->GetName(), COMPRESSED_MODEL_STREAM_A) == 0)
        {
            IfFailGo( m_MiniMd.InitOnMem(pvCurrentData, cbCurrentData) );
            bFoundMd = true;
        }

        // Found the hot meta data stream
        else if (strcmp(pStream->GetName(), HOT_MODEL_STREAM_A) == 0)
        {
            BYTE * hotStreamEnd = reinterpret_cast< BYTE * >( pvCurrentData ) + cbCurrentData;
            ULONG * hotMetadataDir = reinterpret_cast< ULONG * >( hotStreamEnd ) - 2;
            ULONG hotPoolsSize = *hotMetadataDir;
            
            m_MiniMd.m_hotTableDirectory = (CMiniMd::HotTableDirectory *)
                ( reinterpret_cast< BYTE* >( hotMetadataDir ) - hotPoolsSize - sizeof(CMiniMd::HotTableDirectory) );
            m_MiniMd.m_hotTableDirectory->CheckTables();

            InitHotPools( hotMetadataDir );
        }
		// Pick off the next stream if there is one.
		pStream = pNext;
	}

	// If the meta data wasn't found, we can't handle this file.
	if (!bFoundMd)
	{
		IfFailGo( CLDB_E_FILE_CORRUPT );
	}
    else
    {   // Validate sensible heaps.
        IfFailGo( m_MiniMd.PostInit(0) );
    }

	// Save off the location.
	m_pvMd = pData;
	m_cbMd = cbData;

ErrExit:
	return (hr);
}


template <class MiniMd>
void CLiteWeightStgdb<MiniMd>::InitHotPools( ULONG * hotMetadataDir )
{
    ULONG * poolDataBase = hotMetadataDir - hotMetadataDir[ 1 ] / sizeof( ULONG );
    
    for ( ULONG * hotPoolsDir = hotMetadataDir - hotMetadataDir[ 1 ] / sizeof( ULONG )
            ; hotPoolsDir != hotMetadataDir
            ; hotPoolsDir += 2 )
    {
        HotPoolDataHeader * poolHeader
            = reinterpret_cast< HotPoolDataHeader * >( poolDataBase - hotPoolsDir[ 1 ] / sizeof( ULONG ) );
            
        switch ( hotPoolsDir[ 0 ] )  // this is pool id
        {
            case MDPoolStrings:
                m_MiniMd.m_Strings.InitHotData( poolHeader );
                break;
                
            case MDPoolGuids:
                m_MiniMd.m_Guids.InitHotData( poolHeader );
                break;

            case MDPoolUSBlobs:
                m_MiniMd.m_USBlobs.InitHotData( poolHeader );
                break;

            case MDPoolBlobs:
                m_MiniMd.m_Blobs.InitHotData( poolHeader );
                break;
        }
    }
}

