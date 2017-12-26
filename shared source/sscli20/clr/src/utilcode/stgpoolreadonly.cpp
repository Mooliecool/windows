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
// StgPoolReadOnly.cpp
//
// Read only pools are used to reduce the amount of data actually required in the database.
// 
//*****************************************************************************
#include "stdafx.h"                     // Standard include.
#include <stgpool.h>                    // Our interface definitions.
#include "metadatatracker.h"
//
//
// StgPoolReadOnly
//
//


const BYTE StgPoolSeg::m_zeros[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


//*****************************************************************************
// Free any memory we allocated.
//*****************************************************************************
StgPoolReadOnly::~StgPoolReadOnly()
{
    LEAF_CONTRACT;
}


//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
HRESULT StgPoolReadOnly::InitOnMemReadOnly(// Return code.
        void        *pData,             // Predefined data.
        ULONG       iSize)              // Size of data.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    // Make sure we aren't stomping anything and are properly initialized.
    _ASSERTE(m_pSegData == m_zeros);

    // Create case requires no further action.
    if (!pData)
        return (E_INVALIDARG);

    m_pSegData = reinterpret_cast<BYTE*>(pData);
    m_cbSegSize = iSize;
    m_cbSegNext = iSize;
    return (S_OK);
}

//*****************************************************************************
// Prepare to shut down or reinitialize.
//*****************************************************************************
void StgPoolReadOnly::Uninit()
{
    LEAF_CONTRACT;

	m_pSegData = (BYTE*)m_zeros;
	m_pNextSeg = 0;
}


//*****************************************************************************
// Convert a string to UNICODE into the caller's buffer.
//*****************************************************************************
HRESULT StgPoolReadOnly::GetStringW(      // Return code.
    ULONG       iOffset,                // Offset of string in pool.
    __out_ecount(cchBuffer) LPWSTR      szOut,                  // Output buffer for string.
    int         cchBuffer)              // Size of output buffer.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT;

    LPCSTR      pString;                // The string in UTF8.
    int         iChars;
    pString = GetString(iOffset);
    iChars = ::WszMultiByteToWideChar(CP_UTF8, 0, pString, -1, szOut, cchBuffer);
    if (iChars == 0)
        return (BadError(HRESULT_FROM_NT(GetLastError())));
    return (S_OK);
}


//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
GUID *StgPoolReadOnly::GetGuid(			// Pointer to guid in pool.
	ULONG		iIndex)					// 1-based index of Guid in pool.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    if (iIndex == 0)
        return (reinterpret_cast<GUID*>(const_cast<BYTE*>(m_zeros)));

	// Convert to 0-based internal form, defer to implementation.
	return (GetGuidi(iIndex-1));
}


//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
GUID *StgPoolReadOnly::GetGuidi(		// Pointer to guid in pool.
	ULONG		iIndex)					// 0-based index of Guid in pool.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

	ULONG iOffset = iIndex * sizeof(GUID);
	if(!IsValidOffset(iOffset))
	{
#ifdef _DEBUG
        if(REGUTIL::GetConfigDWORD(L"AssertOnBadImageFormat", 1))
		    _ASSERTE(!"Invalid GUID Offset");
#endif
		iOffset = 0;
	}
    return (reinterpret_cast<GUID*>(GetData(iOffset)));
}


//*****************************************************************************
// Return a pointer to a null terminated blob given an offset previously
// handed out by Addblob or Findblob.
//*****************************************************************************
void *StgPoolReadOnly::GetBlob(             // Pointer to blob's bytes. Valid only if *piSize is non-zero
    ULONG       iOffset,                // Offset of blob in pool.
    ULONG       *piSize)                // Return size of blob.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    void const  *pData = NULL;          // Pointer to blob's bytes.

	// This should not be a necessary special case.  The zero byte at the 
	//  start of the pool will code for a length of zero.  We will return
	//  a pointer to the next length byte, but the caller should notice that
	//  the size is zero, and should not look at any bytes.
    // [SL] Yes, but we don't need all further computations and checks if iOffset==0
    
    if (iOffset == 0)
    {
        *piSize = 0;
        return (const_cast<BYTE*>(m_zeros));
    }

#ifdef _DEBUG
    // Is the offset within this heap?
	if(!IsValidOffset(iOffset))
	{
        if(REGUTIL::GetConfigDWORD(L"AssertOnBadImageFormat", 0))
            _ASSERTE(!"Invalid Blob Offset");

        *piSize = 0;
        return (const_cast<BYTE*>(m_zeros));
	}
#endif

    // Get size of the blob (and pointer to data).
    *piSize = CPackedLen::GetLength(GetData(iOffset), &pData);

    // Return pointer to data.
    return (const_cast<void*>(pData));
}


void const * StgPoolReadOnly::GetHotData( ULONG offset ) const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    if ( !m_hotDataHeader )
        return NULL;
    
    ULONG const * endAddr = reinterpret_cast< ULONG const * >( m_hotDataHeader );
    ULONG const * ridBegin = endAddr - m_hotDataHeader->RidsOffset / sizeof( ULONG );

    // binary search for rid
    RIDBinarySearch ridsearch( ridBegin, static_cast< int >( endAddr - ridBegin ) );

    ULONG const * foundRid = ridsearch.Find( & offset );

    if ( !foundRid )
        return NULL;

    ptrdiff_t indPos = foundRid - ridBegin;
    ULONG const * indices = endAddr - m_hotDataHeader->DataIndicesOffset / sizeof( ULONG );
    BYTE const * data = reinterpret_cast< BYTE const * >( endAddr ) - m_hotDataHeader->DataOffset;

    // compute data pointer
    // indices[ indPos ] points past the data
    // and the index of the first data block (intPos==0) is 0
    BYTE const * foundData = data + ( indPos > 0 ? indices[ indPos - 1 ] : 0 );

    return reinterpret_cast< void const * >( foundData );
}

