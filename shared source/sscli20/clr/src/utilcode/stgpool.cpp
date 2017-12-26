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
// StgPool.cpp
//
// Pools are used to reduce the amount of data actually required in the database.
// This allows for duplicate string and binary values to be folded into one
// copy shared by the rest of the database.  Strings are tracked in a hash
// table when insert/changing data to find duplicates quickly.  The strings
// are then persisted consecutively in a stream in the database format.
//
//*****************************************************************************
#include "stdafx.h"                     // Standard include.
#include <stgpool.h>                    // Our interface definitions.
#include <posterror.h>                  // Error handling.
#include "../md/inc/streamutil.h"

#include "ex.h"


using namespace StreamUtil;

#define MAX_CHAIN_LENGTH 20             // Max chain length before rehashing.

//
//
// StgPool
//
//


//*****************************************************************************
// Free any memory we allocated.
//*****************************************************************************
StgPool::~StgPool()
{
    WRAPPER_CONTRACT;

    Uninit();
} // StgPool::~StgPool()


//*****************************************************************************
// Init the pool for use.  This is called for both the create empty case.
//*****************************************************************************
HRESULT StgPool::InitNew(               // Return code.
    ULONG       cbSize,                 // Estimated size.
    ULONG       cItems)                 // Estimated item count.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    // Make sure we aren't stomping anything and are properly initialized.
    _ASSERTE(m_pSegData == m_zeros);
    _ASSERTE(m_pNextSeg == 0);
    _ASSERTE(m_pCurSeg == this);
    _ASSERTE(m_cbCurSegOffset == 0);
    _ASSERTE(m_cbSegSize == 0);
    _ASSERTE(m_cbSegNext == 0);

    m_bDirty = false;
    m_bReadOnly = false;
    m_bFree = false;

    return (S_OK);
} // HRESULT StgPool::InitNew()

//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
HRESULT StgPool::InitOnMem(             // Return code.
        void        *pData,             // Predefined data.
        ULONG       iSize,              // Size of data.
        int         bReadOnly)          // true if append is forbidden.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Make sure we aren't stomping anything and are properly initialized.
    _ASSERTE(m_pSegData == m_zeros);
    _ASSERTE(m_pNextSeg == 0);
    _ASSERTE(m_pCurSeg == this);
    _ASSERTE(m_cbCurSegOffset == 0);

    // Create case requires no further action.
    if (!pData)
        return (E_INVALIDARG);

    // Might we be extending this heap?
    m_bReadOnly = bReadOnly;


    m_pSegData = reinterpret_cast<BYTE*>(pData);
    m_cbSegSize = iSize;
    m_cbSegNext = iSize;

    m_bFree = false;
    m_bDirty = false;

    return (S_OK);
} // HRESULT StgPool::InitOnMem()


//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
HRESULT StgPool::InitOnMemReadOnly(     // Return code.
        void        *pData,             // Predefined data.
        ULONG       iSize)              // Size of data.
{
    WRAPPER_CONTRACT;

    return InitOnMem(pData, iSize, true);
} // HRESULT StgPool::InitOnMemReadOnly()

//*****************************************************************************
// Called when the pool must stop accessing memory passed to InitOnMem().
//*****************************************************************************
HRESULT StgPool::TakeOwnershipOfInitMem()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // If the pool doesn't have a pointer to non-owned memory, done.
    if (m_bFree)
        return (S_OK);

    // If the pool doesn't have a pointer to memory at all, done.
    if (m_pSegData == m_zeros)
    {
        _ASSERTE(m_cbSegSize == 0);
        return (S_OK);
    }

    // Get some memory to keep.
    BYTE *pData = new (nothrow) BYTE[m_cbSegSize+4];
    if (pData == 0)
        return (PostError(OutOfMemory()));

    // Copy the old data to the new memory.
    memcpy(pData, m_pSegData, m_cbSegSize);
    m_pSegData = pData;
    m_bFree = true;

    return (S_OK);
} // HRESULT StgPool::TakeOwnershipOfInitMem()


//*****************************************************************************
// Clear out this pool.  Cannot use until you call InitNew.
//*****************************************************************************
void StgPool::Uninit()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Free base segment, if appropriate.
    if (m_bFree && (m_pSegData != m_zeros))
    {
        delete [] m_pSegData;
        m_bFree = false;
    }

    // Free chain, if any.
    StgPoolSeg  *pSeg = m_pNextSeg;
    while (pSeg)
    {
        StgPoolSeg *pNext = pSeg->m_pNextSeg;
        delete [] (BYTE*)pSeg;
        pSeg = pNext;
    }

    // Clear vars.
    m_pSegData = (BYTE*)m_zeros;
    m_cbSegSize = m_cbSegNext = 0;
    m_pNextSeg = 0;
    m_pCurSeg = this;
    m_cbCurSegOffset = 0;
    m_State = eNormal;
} // void StgPool::Uninit()

//*****************************************************************************
// Called to copy the pool to writable memory, reset the r/o bit.
//*****************************************************************************
HRESULT StgPool::ConvertToRW()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;                     // A result.
    IfFailRet(TakeOwnershipOfInitMem());

    IfFailRet(SetHash(true));

    m_bReadOnly = false;

    return S_OK;
} // HRESULT StgPool::ConvertToRW()

//*****************************************************************************
// Turn hashing off or on.  Real implementation as required in subclass.
//*****************************************************************************
HRESULT StgPool::SetHash(int bHash)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    return S_OK;
} // HRESULT StgPool::SetHash()

//*****************************************************************************
// Trim any empty final segment.
//*****************************************************************************
void StgPool::Trim()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // If no chained segments, nothing to do.
    if (m_pNextSeg == 0)
        return;

    // Handle special case for a segment that was completely unused.
    if (m_pCurSeg->m_cbSegNext == 0)
    {
        // Find the segment which points to the empty segment.
        StgPoolSeg *pPrev;
        for (pPrev = this; pPrev && pPrev->m_pNextSeg != m_pCurSeg; pPrev = pPrev->m_pNextSeg);
        _ASSERTE(pPrev && pPrev->m_pNextSeg == m_pCurSeg);

        // Free the empty segment.
        delete [] (BYTE*) m_pCurSeg;
        
        // Fix the pCurSeg pointer.
        pPrev->m_pNextSeg = 0;
        m_pCurSeg = pPrev;

        // Adjust the base offset, because the PREVIOUS seg is now current.
        _ASSERTE(m_pCurSeg->m_cbSegNext <= m_cbCurSegOffset);
        m_cbCurSegOffset = m_cbCurSegOffset - m_pCurSeg->m_cbSegNext;
    }
} // void StgPool::Trim()

//*****************************************************************************
// Allocate memory if we don't have any, or grow what we have.  If successful,
// then at least iRequired bytes will be allocated.
//*****************************************************************************
bool StgPool::Grow(                     // true if successful.
    ULONG       iRequired)              // Min required bytes to allocate.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END

    ULONG       iNewSize;               // New size we want.
    StgPoolSeg  *pNew;                  // Temp pointer for malloc.

    _ASSERTE(!m_bReadOnly);

    // Would this put the pool over 2GB?
    if ((m_cbCurSegOffset + iRequired) > INT_MAX)
        return (false);

    // Adjust grow size as a ratio to avoid too many reallocs.
    if ((m_pCurSeg->m_cbSegNext + m_cbCurSegOffset) / m_ulGrowInc >= 3)
        m_ulGrowInc *= 2;

    // If first time, handle specially.
    if (m_pSegData == m_zeros)
    {
        // Allocate the buffer.
        iNewSize = max(m_ulGrowInc, iRequired);
        BYTE *pSegData = new (nothrow) BYTE[iNewSize+4];
        if (pSegData == 0)
            return (false);
        m_pSegData = pSegData;

        // Will need to delete it.
        m_bFree = true;

        // How big is this initial segment?
        m_cbSegSize = iNewSize;

        // Do some validation of var fields.
        _ASSERTE(m_cbSegNext == 0);
        _ASSERTE(m_pCurSeg == this);
        _ASSERTE(m_pNextSeg == 0);

        return (true);
    }

    // Allocate the new space enough for header + data.
    iNewSize = (ULONG) (max(m_ulGrowInc, iRequired) + sizeof(StgPoolSeg));
    pNew = (StgPoolSeg*)new (nothrow) BYTE[iNewSize+4];
    if (pNew == 0)
        return (false);

    // Set the fields in the new segment.
    pNew->m_pSegData = reinterpret_cast<BYTE*>(pNew) + sizeof(StgPoolSeg);
    _ASSERTE(ALIGN4BYTE(reinterpret_cast<ULONG_PTR>(pNew->m_pSegData)) == reinterpret_cast<ULONG_PTR>(pNew->m_pSegData));
    pNew->m_pNextSeg = 0;
    pNew->m_cbSegSize = iNewSize - sizeof(StgPoolSeg);
    pNew->m_cbSegNext = 0;

    // Calculate the base offset of the new segment.
    m_cbCurSegOffset = m_cbCurSegOffset + m_pCurSeg->m_cbSegNext;

    // Handle special case for a segment that was completely unused.
    if (m_pCurSeg->m_cbSegNext == 0)
    {
        // Find the segment which points to the empty segment.
        StgPoolSeg *pPrev;
        for (pPrev = this; pPrev && pPrev->m_pNextSeg != m_pCurSeg; pPrev = pPrev->m_pNextSeg);
        _ASSERTE(pPrev && pPrev->m_pNextSeg == m_pCurSeg);

        // Free the empty segment.
        delete [] (BYTE*) m_pCurSeg;
        
        // Link in the new segment.
        pPrev->m_pNextSeg = pNew;
        m_pCurSeg = pNew;

        return (true);
    }

#ifndef NO_CRT
    // Give back any memory that we won't use.
    if (m_pNextSeg == 0)
    {   // First segment allocated as [header]->[data].
        // Be sure that we are contracting the allocation.
        if (m_pCurSeg->m_cbSegNext < (_msize(m_pCurSeg->m_pSegData)-4))
        {
            // Contract the allocation.
            void *pRealloc = _expand(m_pCurSeg->m_pSegData, m_pCurSeg->m_cbSegNext+4);
            // Shouldn't have moved.
            _ASSERTE(pRealloc == m_pCurSeg->m_pSegData);
        }
    }
    else
    {   // Chained segments are allocated together, [header][data].
        // Be sure that we are contracting the allocation.
        if (m_pCurSeg->m_cbSegNext+sizeof(StgPoolSeg) < (_msize(m_pCurSeg)-4))
        {
            // Contract the allocation.
            void *pRealloc = _expand(m_pCurSeg, m_pCurSeg->m_cbSegNext+sizeof(StgPoolSeg)+4);
            // Shouldn't have moved.
            _ASSERTE(pRealloc == m_pCurSeg);
        }
    }
#endif

    // Fix the size of the old segment.
    m_pCurSeg->m_cbSegSize = m_pCurSeg->m_cbSegNext;

    // Link the new segment into the chain.
    m_pCurSeg->m_pNextSeg = pNew;
    m_pCurSeg = pNew;

    return (true);
} // bool StgPool::Grow()


//*****************************************************************************
// Add a segment to the chain of segments.
//*****************************************************************************
HRESULT StgPool::AddSegment(            // S_OK or error.
    const void  *pData,                 // The data.
    ULONG       cbData,                 // Size of the data.
    bool        bCopy)                  // If true, make a copy of the data.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    StgPoolSeg  *pNew;                  // Temp pointer for malloc.

   
    // If we need to copy the data, just grow the heap by enough to take the
    //  the new data, and copy it in.
    if (bCopy)
    {
        void *pDataToAdd = new (nothrow) BYTE[cbData];
        IfNullRet(pDataToAdd);
        memcpy(pDataToAdd, pData, cbData);
        pData = pDataToAdd;
    }

    // If first time, handle specially.
    if (m_pSegData == m_zeros)
    {   // Data was passed in.
        m_pSegData = reinterpret_cast<BYTE*>(const_cast<void*>(pData));
        m_cbSegSize = cbData;
        m_cbSegNext = cbData;
        _ASSERTE(m_pNextSeg == 0);

        // Will not delete it.
        m_bFree = false;

        return S_OK;
    }

    // Not first time.  Handle a completely empty tail segment.
    Trim();

    // Abandon any space past the end of the current live data.
    _ASSERTE( m_pCurSeg->m_cbSegSize >=  m_pCurSeg->m_cbSegNext);
    m_pCurSeg->m_cbSegSize = m_pCurSeg->m_cbSegNext;

    // Allocate a new segment header.
    pNew = (StgPoolSeg *) new (nothrow) BYTE[sizeof(StgPoolSeg)];
    if (pNew == 0)
        return E_OUTOFMEMORY;

    // Set the fields in the new segment.
    pNew->m_pSegData = reinterpret_cast<BYTE*>(const_cast<void*>(pData));
    pNew->m_pNextSeg = 0;
    pNew->m_cbSegSize = cbData;
    pNew->m_cbSegNext = cbData;

    // Calculate the base offset of the new segment.
    m_cbCurSegOffset = m_cbCurSegOffset + m_pCurSeg->m_cbSegNext;

    // Link the segment into the chain.
    _ASSERTE(m_pCurSeg->m_pNextSeg == 0);
    m_pCurSeg->m_pNextSeg = pNew;
    m_pCurSeg = pNew;

    return S_OK;
} // HRESULT StgPool::AddSegment()


//*****************************************************************************
// Prepare for pool reorganization.
//*****************************************************************************
HRESULT StgPool::OrganizeBegin()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate transition.
    _ASSERTE(m_State == eNormal);

    m_State = eMarking;
    return (S_OK);
} // HRESULT StgPool::OrganizeBegin()

//*****************************************************************************
// Mark an object as being live in the organized pool.
//*****************************************************************************
HRESULT StgPool::OrganizeMark(
    ULONG       ulOffset)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END


    // Validate state.
    _ASSERTE(m_State == eMarking);

    return (S_OK);
} // HRESULT StgPool::OrganizeMark()

//*****************************************************************************
// This reorganizes the string pool for minimum size.  This is done by sorting
//  the strings, eliminating any duplicates, and performing tail-merging on
//  any that are left (that is, if "IFoo" is at offset 2, "Foo" will be
//  at offset 3, since "Foo" is a substring of "IFoo").
//
// After this function is called, the only valid operations are RemapOffset and
//  PersistToStream.
//*****************************************************************************
HRESULT StgPool::OrganizePool()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END


    // Validate transition.
    _ASSERTE(m_State == eMarking);

    m_State = eOrganized;
    return (S_OK);
} // HRESULT StgPool::OrganizePool()

//*****************************************************************************
// Given an offset from before the remap, what is the offset after the remap?
//*****************************************************************************
HRESULT StgPool::OrganizeRemap(
    ULONG       ulOld,                  // Old offset.
    ULONG       *pulNew)                // Put new offset here.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END


    // Validate state.
    _ASSERTE(m_State == eOrganized || m_State == eNormal);

    *pulNew = ulOld;
    return (S_OK);
} // HRESULT StgPool::OrganizeRemap()

//*****************************************************************************
// Called to leave the organizing state.  Strings may be added again.
//*****************************************************************************
HRESULT StgPool::OrganizeEnd()
{ 
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END


    // Validate transition.
    _ASSERTE(m_State == eOrganized);

    m_State = eNormal;
    return (S_OK); 
} // HRESULT StgPool::OrganizeEnd()



//*****************************************************************************
// The entire string pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
HRESULT StgPool::PersistToStream(       // Return code.
    IStream     *pIStream)              // The stream to write to.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;
    ULONG       cbTotal;                // Total bytes written.
    StgPoolSeg  *pSeg;                  // A segment being written.

    _ASSERTE(m_pSegData != m_zeros);

    // Start with the base segment.
    pSeg = this;
    cbTotal = 0;

    EX_TRY
    {
        // As long as there is data, write it.
        while (pSeg)
        {   
            // If there is data in the segment . . .
            if (pSeg->m_cbSegNext)
            {   // . . . write and count the data.
                if (FAILED(hr = pIStream->Write(pSeg->m_pSegData, pSeg->m_cbSegNext, 0)))
                    break;
                cbTotal += pSeg->m_cbSegNext;
            }
    
            // Get the next segment.
            pSeg = pSeg->m_pNextSeg;
        }

        if(SUCCEEDED(hr))
        {
            // Align to 4 byte boundary.
            if (Align(cbTotal) != cbTotal)
            {
                _ASSERTE(sizeof(hr) >= 3);
                hr = 0;
                hr = pIStream->Write(&hr, Align(cbTotal)-cbTotal, 0);
            }
        }
    }
    EX_CATCH
    {
        hr = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return (hr);
} // HRESULT StgPool::PersistToStream()


//*****************************************************************************
// The entire string pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
HRESULT StgPool::PersistPartialToStream(// Return code.
    IStream     *pIStream,              // The stream to write to.
    ULONG       iOffset)                // Starting offset.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;              // A result.
    ULONG       cbTotal;                // Total bytes written.
    StgPoolSeg  *pSeg;                  // A segment being written.

    _ASSERTE(m_State == eNormal);
    _ASSERTE(m_pSegData != m_zeros);

    // Start with the base segment.
    pSeg = this;
    cbTotal = 0;

    // As long as there is data, write it.
    while (pSeg)
    {   
        // If there is data in the segment . . .
        if (pSeg->m_cbSegNext)
        {   // If this data should be skipped...
            if (iOffset >= pSeg->m_cbSegNext)
            {   // Skip it
                iOffset -= pSeg->m_cbSegNext;
            }
            else
            {   // At least some data should be written, so write and count the data.
                if (FAILED(hr = pIStream->Write(pSeg->m_pSegData+iOffset, pSeg->m_cbSegNext-iOffset, 0)))
                    return (hr);
                cbTotal += pSeg->m_cbSegNext-iOffset;
                iOffset = 0;
            }
        }

        // Get the next segment.
        pSeg = pSeg->m_pNextSeg;
    }

    // Align to 4 byte boundary.
    if (Align(cbTotal) != cbTotal)
    {
        _ASSERTE(sizeof(hr) >= 3);
        hr = 0;
        hr = pIStream->Write(&hr, Align(cbTotal)-cbTotal, 0);
    }

    return (hr);
} // HRESULT StgPool::PersistPartialToStream()


namespace
{
    // compare function for qsort
    int __cdecl UlongComp( void const * x, void const * y )
    {
        return *reinterpret_cast< ULONG const * >( x ) - *reinterpret_cast< ULONG const * >( y );
    }
}


HRESULT StgPool::PersistHotToStream( IStream * strm,
                                     CorProfileData * profileData,
                                     MDPools poolId,
                                     ULONG * savedSize )
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION( strm != NULL );
        PRECONDITION( profileData != NULL );
        PRECONDITION( savedSize != NULL );
    }
    CONTRACTL_END
    

    return S_OK;
}


HRESULT StgPool::CopyData(
    ULONG   nOffset,
    BYTE*   pBuffer,
    ULONG   cbBuffer,
    ULONG*  cbWritten)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(pBuffer));
        PRECONDITION(CheckPointer(cbWritten));
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;              // A result.
    StgPoolSeg  *pSeg;                  // A segment being written.

    _ASSERTE(m_State == eNormal);
    _ASSERTE(m_pSegData != m_zeros);

    // Start with the base segment.
    pSeg = this;
    *cbWritten = 0;

    // As long as there is data, write it.
    while (pSeg)
    {   
        // If there is data in the segment . . .
        if (pSeg->m_cbSegNext)
        {   // If this data should be skipped...
            if (nOffset >= pSeg->m_cbSegNext)
            {   // Skip it
                nOffset -= pSeg->m_cbSegNext;
            }
            else
            {
                ULONG nNumBytesToCopy = pSeg->m_cbSegNext-nOffset;
                if (nNumBytesToCopy > (cbBuffer-*cbWritten))
                {
                    _ASSERTE(!"Buffer isn't big enough to copy everything!");
                    nNumBytesToCopy = cbBuffer-*cbWritten;
                }

                memcpy(pBuffer+*cbWritten, pSeg->m_pSegData+nOffset, nNumBytesToCopy);

                *cbWritten += nNumBytesToCopy;
                nOffset = 0;
            }
        }

        // Get the next segment.
        pSeg = pSeg->m_pNextSeg;
    }

    return (hr);
} // HRESULT StgPool::CopyData()

//*****************************************************************************
// Get a pointer to the data at some offset.  May require traversing the
//  chain of extensions.  It is the caller's responsibility not to attempt
//  to access data beyond the end of a segment.
// This is an internal accessor, and should only be called when the data
//  is not in the base segment.
//*****************************************************************************
BYTE *StgPool::GetData_i(               // pointer to data or NULL.
    ULONG       ulOffset)               // Offset of data within pool.
{

    LEAF_CONTRACT;

    // Shouldn't be called on base segment.
    _ASSERTE(ulOffset >= m_cbSegNext);
    StgPoolSeg  *pSeg = this;

    while (ulOffset && ulOffset >= pSeg->m_cbSegNext)
    {
        // If we are chaining through this segment, it should be fixed (size == next).
        _ASSERTE(pSeg->m_cbSegNext == pSeg->m_cbSegSize);

        // On to next segment.
        ulOffset -= pSeg->m_cbSegNext;
        pSeg = pSeg->m_pNextSeg;

        // Is there a next?
        if (pSeg == 0)
        {
#ifdef _DEBUG
            if(REGUTIL::GetConfigDWORD(L"AssertOnBadImageFormat", 0))
            _ASSERTE(!"Offset past end-of-chain passed to GetData_i()");
#endif
            return (BYTE*)m_zeros;
        }
    }

    // For the case where we want to read the first item and the pool is empty.
//    if (ulOffset == 0 && pSeg->m_cbSegNext==0)
    if (ulOffset == pSeg->m_cbSegNext) // can only be if both == 0
        return (BYTE*)m_zeros;

    return (pSeg->m_pSegData + ulOffset);
} // BYTE *StgPool::GetData_i()


//
//
// StgStringPool
//
//


//*****************************************************************************
// Create a new, empty string pool.
//*****************************************************************************
HRESULT StgStringPool::InitNew(         // Return code.
    ULONG       cbSize,                 // Estimated size.
    ULONG       cItems)                 // Estimated item count.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    HRESULT     hr;                     // A result.
    ULONG       i;                      // Offset of empty string.

    // Let base class intialize.
    if (FAILED(hr = StgPool::InitNew()))
        return (hr);

    _ASSERTE(m_Remap.Count() == 0);
    _ASSERTE(m_RemapIndex.Count() == 0);

    // Set initial table sizes, if specified.
    if (cbSize)
        if (!Grow(cbSize))
            return E_OUTOFMEMORY;
    if (cItems)
        m_Hash.SetBuckets(cItems);

    // Init with empty string.
    if (FAILED(hr = AddString("", &i, 0)))
        return hr;
    
    // Empty string had better be at offset 0.
    _ASSERTE(i == 0);
    SetDirty(false);
    return (hr);
} // HRESULT StgStringPool::InitNew()


//*****************************************************************************
// Load a string heap from persisted memory.  If a copy of the data is made
// (so that it may be updated), then a new hash table is generated which can
// be used to elminate duplicates with new strings.
//*****************************************************************************
HRESULT StgStringPool::InitOnMem(       // Return code.
    void        *pData,                 // Predefined data.
    ULONG       iSize,                  // Size of data.
    int         bReadOnly)              // true if append is forbidden.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END

    HRESULT     hr;

    // There may be up to three extra '\0' characters appended for padding.  Trim them.
    char *pchData = reinterpret_cast<char*>(pData);
    while (iSize > 1 && pchData[iSize-1] == 0 && pchData[iSize-2] == 0)
        --iSize;

    // Let base class init our memory structure.
    if (FAILED(hr = StgPool::InitOnMem(pData, iSize, bReadOnly)))
        return (hr);

    if (!bReadOnly)
        TakeOwnershipOfInitMem();

    _ASSERTE(m_Remap.Count() == 0);
    _ASSERTE(m_RemapIndex.Count() == 0);

    // If might be updated, build the hash table.
    if (!bReadOnly)
        hr = RehashStrings();

    return (hr);
} // HRESULT StgStringPool::InitOnMem()

//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
void StgStringPool::Uninit()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Clear the hash table.
    m_Hash.Clear();

    // Let base class clean up.
    StgPool::Uninit();

    // Clean up any remapping state.
    m_State = eNormal;
    m_Remap.Clear();
    m_RemapIndex.Clear();
} // void StgStringPool::Uninit()

//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
HRESULT StgStringPool::SetHash(int bHash)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;

    _ASSERTE(m_State == eNormal);

    // If turning on hash again, need to rehash all strings.
    if (bHash)
        hr = RehashStrings();

    m_bHash = bHash;
    return (hr);
} // HRESULT StgStringPool::SetHash()

//*****************************************************************************
// The string will be added to the pool.  The offset of the string in the pool
// is returned in *piOffset.  If the string is already in the pool, then the
// offset will be to the existing copy of the string.
//*****************************************************************************
HRESULT StgStringPool::AddString(       // Return code.
    LPCSTR      szString,               // The string to add to pool.
    ULONG       *piOffset,              // Return offset of string here.
    int         iLength)                // -1 null terminated.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    STRINGHASH  *pHash;                 // Hash item for add.
    ULONG       iLen;                   // To handle non-null strings.
    LPSTR       pData;                  // Pointer to location for new string.

    _ASSERTE(!m_bReadOnly);

    // Can't add during a reorganization.
    _ASSERTE(m_State == eNormal);

    // Null pointer is an error.
    if (szString == 0)
        return (PostError(E_INVALIDARG));

    // Find the real length we need in buffer.
    if (iLength == -1)
        iLen = (ULONG)(strlen(szString) + 1);
    else
        iLen = iLength + 1;

    // Where to put the new string?
    if (iLen > GetCbSegAvailable())
    {
        if (!Grow(iLen))
            return (PostError(OutOfMemory()));
    }
    pData = reinterpret_cast<LPSTR>(GetNextLocation());

    // Copy the data into the buffer.
    if (iLength == -1)
        strcpy_s(pData, iLen, szString);
    else
    {
        strncpy_s(pData, iLen, szString, iLength);
        pData[iLength] = '\0';
    }

    // If the hash table is to be kept built (default).
    if (m_bHash)
    {
        // Find or add the entry.
        pHash = m_Hash.Find(pData, true);
        if (!pHash)
            return (PostError(OutOfMemory()));

        // If the entry was new, keep the new string.
        if (pHash->iOffset == 0xffffffff)
        {
            *piOffset = pHash->iOffset = GetNextOffset();
            SegAllocate(iLen);
            SetDirty();

            if (m_Hash.MaxChainLength() > MAX_CHAIN_LENGTH)
                RehashStrings();
        }
        // Else use the old one.
        else
            *piOffset = pHash->iOffset;
    }
    // Probably an import which defers the hash table for speed.
    else
    {
        *piOffset = GetNextOffset();
        SegAllocate(iLen);
        SetDirty();
    }
    return (S_OK);
} // HRESULT StgStringPool::AddString()

//*****************************************************************************
// Add a string to the pool with Unicode to UTF8 conversion.
//*****************************************************************************
HRESULT StgStringPool::AddStringW(      // Return code.
    LPCWSTR     szString,               // The string to add to pool.
    ULONG       *piOffset,              // Return offset of string here.
    int         iLength)                // -1 null terminated.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    STRINGHASH  *pHash;                 // Hash item for add.
    ULONG       iLen;                   // Correct length after conversion.
    LPSTR       pData;                  // Pointer to location for new string.

    _ASSERTE(!m_bReadOnly);
    
    // Can't add during a reorganization.
    _ASSERTE(m_State == eNormal);

    // Null pointer is an error.
    if (szString == 0)
        return (PostError(E_INVALIDARG));

    // Special case empty string.
    if (iLength == 0 || (iLength == -1 && *szString == '\0'))
    {
        *piOffset = 0;
        return (S_OK);
    }

    // How many bytes will be required in the heap?
    iLen = ::WszWideCharToMultiByte(CP_UTF8, 0, szString, iLength, 0, 0, 0, 0);
    // WCTMB includes trailing 0 if iLength==-1, doesn't otherwise.
    if (iLength >= 0)
        ++iLen;

    // Check for room.
    if (iLen > GetCbSegAvailable())
    {
        if (!Grow(iLen))
            return (PostError(OutOfMemory()));
    }
    pData = reinterpret_cast<LPSTR>(GetNextLocation());

    // Convert the data in place to the correct location.
    iLen = ::WszWideCharToMultiByte(CP_UTF8, 0, szString, iLength,
        pData, GetCbSegAvailable(), 0, 0);
    if (iLen == 0)
        return (BadError(HRESULT_FROM_NT(GetLastError())));
    // If the conversion didn't, null terminate; count the null.
    if (iLength >= 0)
        pData[iLen++] = '\0';

    // If the hash table is to be kept built (default).
    if (m_bHash)
    {
        // Find or add the entry.
        pHash = m_Hash.Find(pData, true);
        if (!pHash)
            return (PostError(OutOfMemory()));

        // If the entry was new, keep the new string.
        if (pHash->iOffset == 0xffffffff)
        {
            *piOffset = pHash->iOffset = GetNextOffset();
            SegAllocate(iLen);
            SetDirty();
        }
        // Else use the old one.
        else
            *piOffset = pHash->iOffset;
    }
    // Probably an import which defers the hash table for speed.
    else
    {
        *piOffset = GetNextOffset();
        SegAllocate(iLen);
        SetDirty();
    }
    return (S_OK);
} // HRESULT StgStringPool::AddStringW()


//*****************************************************************************
// Clears out the existing hash table used to eliminate duplicates.  Then
// rebuilds the hash table from scratch based on the current data.
//*****************************************************************************
HRESULT StgStringPool::RehashStrings()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    ULONG       iOffset;                // Loop control.
    ULONG       iMax;                   // End of loop.
    ULONG       iSeg;                   // Location within segment.
    StgPoolSeg  *pSeg = this;           // To loop over segments.
    STRINGHASH  *pHash;                 // Hash item for add.
    LPCSTR      pString;                // A string;
    ULONG       iLen;                   // The string's length.
    int         iBuckets;               // Buckets in the hash.
    int         iCount;                 // Items in the hash.
    int         iNewBuckets;            // New count of buckets in the hash.

    // Determine the new bucket size.
    iBuckets = m_Hash.Buckets();
    iCount = m_Hash.Count();
    iNewBuckets = max(iCount, iBuckets+iBuckets/2+1);
        
    // Remove any stale data.
    m_Hash.Clear();
    m_Hash.SetBuckets(iNewBuckets);

    // How far should the loop go.
    iMax = GetNextOffset();

    // Go through each string, skipping initial empty string.
    for (iSeg=iOffset=1;  iOffset < iMax;  )
    {
        // Get the string from the pool.
        pString = reinterpret_cast<LPCSTR>(pSeg->m_pSegData + iSeg);
        // Add the string to the hash table.
        if ((pHash = m_Hash.Add(pString)) == 0)
            return (PostError(OutOfMemory()));
        pHash->iOffset = iOffset;

        // Move to next string.
        iLen = (ULONG)(strlen(pString) + 1);
        iOffset += iLen;
        iSeg += iLen;
        if (iSeg >= pSeg->m_cbSegNext)
        {
            pSeg = pSeg->m_pNextSeg;
            iSeg = 0;
        }
    }
    return (S_OK);
} // HRESULT StgStringPool::RehashStrings()

//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
HRESULT StgStringPool::GetNextItem(     // Return code.
    ULONG       ulItem,                 // Current item.
    ULONG       *pulNext)               // Return offset of next pool item.
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    LPCSTR      pString;                // A string;

    // Outside of heap?
    if (ulItem >= GetNextOffset())
    {
        *pulNext = 0;
        return S_FALSE;
    }
    
    pString = reinterpret_cast<LPCSTR>(GetData(ulItem));
    ulItem += (ULONG)(strlen(pString) + 1);
    
    // Was it the last item in heap?
    if (ulItem >= GetNextOffset())
    {
        *pulNext = 0;
        return S_FALSE;
    }
    
    *pulNext = ulItem;
    return (S_OK);
} // HRESULT StgStringPool::GetNextItem()


//*****************************************************************************
// Prepare for string reorganization.
//*****************************************************************************
HRESULT StgStringPool::OrganizeBegin()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    ULONG       iOffset;                // Loop control.
    ULONG       iMax;                   // End of loop.
    ULONG       iSeg;                   // Location within segment.
    StgPoolSeg  *pSeg = this;           // To loop over segments.
    LPCSTR      pString;                // A string;
    ULONG       iLen;                   // The string's length.
    StgStringRemap *pRemap;             // A new remap element.

    _ASSERTE(m_State == eNormal);
    _ASSERTE(m_Remap.Count() == 0);


    // How far should the loop go.
    iMax = GetNextOffset();

    // Go through each string, skipping initial empty string.
    for (iSeg=iOffset=1;  iOffset < iMax;  )
    {
        // Get the string from the pool.
        pString = reinterpret_cast<LPCSTR>(pSeg->m_pSegData + iSeg);
        iLen = (ULONG)(strlen(pString));

        // Add the string to the remap list.
        pRemap = m_Remap.Append();
        if (pRemap == 0)
        {
            m_Remap.Clear();
            return (PostError(OutOfMemory()));
        }
        pRemap->ulOldOffset = iOffset;
        pRemap->cbString = iLen;
        pRemap->ulNewOffset = ULONG_MAX;

        // Move to next string.
        iOffset += iLen + 1;
        iSeg += iLen + 1;
        if (iSeg >= pSeg->m_cbSegNext)
        {
            _ASSERTE(iSeg == pSeg->m_cbSegNext);
            pSeg = pSeg->m_pNextSeg;
            iSeg = 0;
        }
    }

    m_State = eMarking;
    return (S_OK);
} // HRESULT StgStringPool::OrganizeBegin()

//*****************************************************************************
// Mark an object as being live in the organized pool.
//*****************************************************************************
HRESULT StgStringPool::OrganizeMark(
    ULONG       ulOffset)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    int         iContainer;             // Index for insert, if not already in list.
    StgStringRemap  *pRemap;            // Found entry.

    // Validate state.
    _ASSERTE(m_State == eMarking);

    // Treat (very common) null string specially.
    // Some columns use 0xffffffff as a null flag.
    if (ulOffset == 0 || ulOffset == 0xffffffff)
        return (S_OK);
    
    StgStringRemap  sTarget = {ulOffset};// For the search, only contains ulOldOffset.
    BinarySearch Searcher(m_Remap.Ptr(), m_Remap.Count()); // Searcher object

    // Do the search.  If exact match, set the ulNewOffset to 0;
    if ((pRemap = const_cast<StgStringRemap*>(Searcher.Find(&sTarget, &iContainer))))
    {
        pRemap->ulNewOffset = 0;
        return (S_OK);
    }

    // Found a tail string.  Get the remap record for the containing string.
    _ASSERTE(iContainer > 0);
    pRemap = m_Remap.Get(iContainer-1);

    // If this is the longest tail so far, set ulNewOffset to the delta from the 
    //  heap's string.
    _ASSERTE(ulOffset > pRemap->ulOldOffset);
    ULONG cbDelta = ulOffset - pRemap->ulOldOffset;
    if (cbDelta < pRemap->ulNewOffset)
        pRemap->ulNewOffset = cbDelta;

    return (S_OK);
} // HRESULT StgStringPool::OrganizeMark()

//*****************************************************************************
// This reorganizes the string pool for minimum size.  This is done by sorting
//  the strings, eliminating any duplicates, and performing tail-merging on
//  any that are left (that is, if "IFoo" is at offset 2, "Foo" will be
//  at offset 3, since "Foo" is a substring of "IFoo").
//
// After this function is called, the only valid operations are RemapOffset and
//  PersistToStream.
//*****************************************************************************
HRESULT StgStringPool::OrganizePool()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    StgStringRemap  *pRemap;            // An entry in the Remap array.
    LPCSTR      pszSaved;               // Pointer to most recently saved string.
    LPCSTR      pszNext;                // Pointer to string under consideration.
    ULONG       cbSaved;                // Size of most recently saved string.
    ULONG       cbDelta;                // Delta in sizes between saved and current strings.
    ULONG       ulOffset;               // Current offset as we loop through the strings.
    int         i;                      // Loop control.
    int         iCount;                 // Count of live strings.


    // Validate state.
    _ASSERTE(m_State == eMarking);
    m_State = eOrganized;

    // Allocate enough indices for the entire remap array.
    if (!m_RemapIndex.AllocateBlock(m_Remap.Count()))
        return (PostError(OutOfMemory()));
    iCount = 0;

    // Add the live strings to the index map.  Discard any unused heads
    //  at this time.
    for (i=0; i<m_Remap.Count(); ++i)
    {
        pRemap = m_Remap.Get(i);
        if (pRemap->ulNewOffset != ULONG_MAX)
        {
            _ASSERTE(pRemap->ulNewOffset < pRemap->cbString);
            m_RemapIndex[iCount++] = i;
            // Discard head of the string?
            if (pRemap->ulNewOffset)
            {
                pRemap->ulOldOffset += pRemap->ulNewOffset;
                pRemap->cbString -= pRemap->ulNewOffset;
                pRemap->ulNewOffset = 0;
            }
        }
    }
    // Clear unused entries from the index map. 
    // Note: AllocateBlock a negative number.
    m_RemapIndex.AllocateBlock(iCount - m_RemapIndex.Count());

    // If no strings marked, nothing to save.
    if (iCount == 0)
    {
        m_cbOrganizedSize = 0;
        m_cbOrganizedOffset = 0;
        return (S_OK);
    }


    //*****************************************************************
    // Phase 1: Sort decending by reversed string value.
    SortReversedName NameSorter(m_RemapIndex.Ptr(), m_RemapIndex.Count(), *this);
    NameSorter.Sort();

#if defined(_DEBUG)
    {
        LPCSTR  pString;
        ULONG   ulOld;
        int     ix;
        for (ix=0; ix<iCount; ++ix)
        {
            ulOld = m_Remap[m_RemapIndex[ix]].ulOldOffset;
            pString = GetString(ulOld);
        }
    }
#endif
    //*****************************************************************
    // Search for duplicates and potential tail-merges.

    // Build the pool from highest to lowest offset.  Since we don't 
    //  know yet how big the pool will be, start with the end at 
    //  ULONG_MAX; then shift the whole set down to start at 1 (right
    //  after the empty string).

    // Map the highest entry first string.  Save length and pointer.
    int ix = iCount - 1;
    pRemap = m_Remap.Get(m_RemapIndex[ix]);
    pszSaved = GetString(pRemap->ulOldOffset);
    cbSaved = pRemap->cbString;
    ulOffset = ULONG_MAX - (cbSaved + 1);
    pRemap->ulNewOffset = ulOffset;

    // For each item in array (other than the highest entry)...
    for (--ix; ix>=0; --ix)
    {
        // Get the remap entry.
        pRemap = m_Remap.Get(m_RemapIndex[ix]);
        pszNext = GetString(pRemap->ulOldOffset);
        _ASSERTE(strlen(pszNext) == pRemap->cbString);
        // If the length is less than or equal to saved length, it might be a substring.
        if (pRemap->cbString <= cbSaved)
        {
             // delta = len(saved) - len(next) [saved is not shorter].  Compare (szOld+delta, szNext)
            cbDelta = cbSaved - pRemap->cbString;
            if (strcmp(pszNext, pszSaved + cbDelta) == 0)
            {   // Substring: save just the offset
                pRemap->ulNewOffset = ulOffset + cbDelta;
                continue;
            }
        }
        // Unique string.  Map string.  Save length and pointer.
        cbSaved = pRemap->cbString;
        ulOffset -= cbSaved + 1;
        pRemap->ulNewOffset = ulOffset;
        pszSaved = pszNext;
    }

    // How big is the optimized pool?
    m_cbOrganizedSize = ULONG_MAX - ulOffset + 1;

    // Shift each entry so that the lowest one starts at 1.
    for (ix=0; ix<iCount; ++ix)
        m_Remap[m_RemapIndex[ix]].ulNewOffset -= ulOffset - 1;
    // Find the highest offset in the pool.
    m_cbOrganizedOffset = m_Remap[m_RemapIndex[--ix]].ulNewOffset;
    for (--ix; ix >= 0 && m_Remap[m_RemapIndex[ix]].ulNewOffset >= m_cbOrganizedOffset ; --ix)
        m_cbOrganizedOffset = m_Remap[m_RemapIndex[ix]].ulNewOffset;
    m_cbOrganizedSize = ALIGN4BYTE(m_cbOrganizedSize);

    return (S_OK);
} // HRESULT StgStringPool::OrganizePool()

//*****************************************************************************
// Given an offset from before the remap, what is the offset after the remap?
//*****************************************************************************
HRESULT StgStringPool::OrganizeRemap(
    ULONG       ulOld,                  // Old offset.
    ULONG       *pulNew)                // Put new offset here.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate state.
    _ASSERTE(m_State == eOrganized || m_State == eNormal);

    // If not reorganized, new == old.
    // Treat (very common) null string specially.
    // Some columns use 0xffffffff as a null flag.
    if (m_State == eNormal || ulOld == 0 || ulOld == 0xffffffff)
    {
        *pulNew = ulOld;
        return (S_OK);
    }

    // Search for old index.  May not be in the map, since the pool may have
    //  been optimized previously.  In that case, find the string that this 
    //  one was the tail of, get the new location of that string, and adjust
    //  by the length deltas.
    int         iContainer;                 // Index of containing string, if not in map.
    StgStringRemap const *pRemap;               // Found entry.
    StgStringRemap  sTarget = {ulOld};          // For the search, only contains ulOldOffset.
    BinarySearch Searcher(m_Remap.Ptr(), m_Remap.Count()); // Searcher object

    // Do the search.
    pRemap = Searcher.Find(&sTarget, &iContainer);
    // Found?
    if (pRemap)
    {   // Yes.
        _ASSERTE(pRemap->ulNewOffset > 0);
        *pulNew = static_cast<ULONG>(pRemap->ulNewOffset);
        return (S_OK);
    }

    // Not Found; this is a persisted tail-string.  New offset is to containing
    //  string's new offset as old offset is to containing string's old offset.
    // This string wasn't found; it is a tail of the previous entry.
    _ASSERTE(iContainer > 0);
    pRemap = m_Remap.Get(iContainer-1);
    // Make sure that the offset really is contained within the previous entry.
    _ASSERTE(ulOld >= pRemap->ulOldOffset && ulOld < pRemap->ulOldOffset + pRemap->cbString);
    *pulNew = pRemap->ulNewOffset + ulOld-pRemap->ulOldOffset;

    return (S_OK);
} // HRESULT StgStringPool::OrganizeRemap()

//*****************************************************************************
// Called to leave the organizing state.  Strings may be added again.
//*****************************************************************************
HRESULT StgStringPool::OrganizeEnd()
{ 
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate state.
    _ASSERTE(m_State == eOrganized);

    m_Remap.Clear(); 
    m_RemapIndex.Clear();
    m_State = eNormal;
    m_cbOrganizedSize = 0;

    return (S_OK); 
} // HRESULT StgStringPool::OrganizeEnd()

//*****************************************************************************
// The entire string pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
HRESULT StgStringPool::PersistToStream( // Return code.
    IStream     *pIStream)              // The stream to write to.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;              // A result.
    StgStringRemap  *pRemap;            // A remap entry.
    ULONG       ulOffset;               // Offset within the pool.
    DEBUG_STMT(ULONG ulOffsetDbg;)      // For debugging offsets.
    int         i;                      // Loop control.
    LPCSTR      pszString;              // Pointer to a string.

    // If not reorganized, just let the base class write the data.
    if (m_State == eNormal)
    {
        return StgPool::PersistToStream(pIStream);
    }

    // Validate state.
    _ASSERTE(m_State == eOrganized);

    // If there is any string data at all, then start pool with empty string.
    if (m_RemapIndex.Count())
    {
        hr = 0; // cheeze -- use hr as a buffer for 0
        if (FAILED(hr = pIStream->Write(&hr, 1, 0)))
            return (hr);
        ulOffset = 1;
    }
    else
        ulOffset = 0;

    // Iterate over the map writing unique strings.  We will detect a unique string
    //  because it will start just past the end of the previous string; ie the next
    //  offset.
    DEBUG_STMT(ulOffsetDbg = 0;)
    for (i=0; i<m_RemapIndex.Count(); ++i)
    {
        // Get the remap entry.
        pRemap = m_Remap.Get(m_RemapIndex[i]);

        // The remap array is sorted by strings.  A given entry may be a tail-string of a higer
        //  indexed string.  Thus, each new unique string will start at ulOffset, just past the
        //  previous unique string.  Tail matched strings will be destined for an offset higher
        //  than ulOffset, and should be skipped.  Finally, in the case of duplicate copies of 
        //  otherwise unique strings, the first copy will appear to be the unique string; the 
        //  offset will be advanced, and subsequent strings will start before ulOffset.
        //  or equal to what we've already written.
        _ASSERTE(pRemap->ulNewOffset >= ulOffset || pRemap->ulNewOffset == ulOffsetDbg);

        // If this string starts past ulOffset, it must be a tail string, and needn't be
        //  written.
        if (static_cast<ULONG>(pRemap->ulNewOffset) > ulOffset)
        {
            // Better be at least one more string, for this one to be the tail of.
            _ASSERTE(i < (m_RemapIndex.Count() - 1));

            // Better end at same point as next string, which this one is a tail of.
            DEBUG_STMT(StgStringRemap *pRemapDbg = m_Remap.Get(m_RemapIndex[i+1]);)
            _ASSERTE(pRemap->ulNewOffset + pRemap->cbString == pRemapDbg->ulNewOffset + pRemapDbg->cbString);

            // This string better really be a tail of the next one.
            DEBUG_STMT(int delta = pRemapDbg->cbString - pRemap->cbString;)
            DEBUG_STMT(const char *p1 = GetString(pRemap->ulOldOffset);)
            DEBUG_STMT(const char *p2 = GetString(pRemapDbg->ulOldOffset) + delta;)
            _ASSERTE(strcmp(p1, p2) == 0);
            continue;
        }

        // If this string starts before ulOffset, it is a duplicate of a previous string.
        if (static_cast<ULONG>(pRemap->ulNewOffset) < ulOffset)
        {
            // There had better be some string before this one.
            _ASSERTE(i > 0);

            // Better end just before where the next string is supposed to start.
            _ASSERTE(pRemap->ulNewOffset + pRemap->cbString + 1 == ulOffset);

            // This string better really match up with the one it is supposed to be a duplicate of.
            DEBUG_STMT(StgStringRemap *pRemapDbg = m_Remap.Get(m_RemapIndex[i-1]);)
            DEBUG_STMT(int delta = pRemapDbg->cbString - pRemap->cbString;)
            DEBUG_STMT(const char *p1 = GetString(pRemap->ulOldOffset);)
            DEBUG_STMT(const char *p2 = GetString(pRemapDbg->ulOldOffset) + delta;)
            _ASSERTE(strcmp(p1, p2) == 0);
            continue;
        }

        // New unique string.  (It starts exactly where we expect it to.)

        // Get the string data, and write it.
        pszString = GetString(pRemap->ulOldOffset);
        _ASSERTE(pRemap->cbString == strlen(pszString));
        if (FAILED(hr=pIStream->Write(pszString, pRemap->cbString+1, 0)))
            return (hr);

        // Save this offset for debugging duplicate strings.
        DEBUG_STMT(ulOffsetDbg = ulOffset);

        // Shift up for the next one.
        ulOffset += pRemap->cbString + 1;
        _ASSERTE(ulOffset <= m_cbOrganizedSize);
        _ASSERTE(ulOffset > 0);
    }

    // Align.
    if (ulOffset != ALIGN4BYTE(ulOffset))
    {
        hr = 0;
        if (FAILED(hr = pIStream->Write(&hr, ALIGN4BYTE(ulOffset)-ulOffset, 0)))
            return (hr);
        ulOffset += ALIGN4BYTE(ulOffset)-ulOffset;
    }

    // Should have written exactly what we expected.
    _ASSERTE(ulOffset == m_cbOrganizedSize);

    return (S_OK);
} // HRESULT StgStringPool::PersistToStream()


//
//
// StgGuidPool
//
//

HRESULT StgGuidPool::InitNew(           // Return code.
    ULONG       cbSize,                 // Estimated size.
    ULONG       cItems)                 // Estimated item count.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;                     // A result.

    if (FAILED(hr = StgPool::InitNew()))
        return (hr);

    // Set initial table sizes, if specified.
    if (cbSize)
        if (!Grow(cbSize))
            return E_OUTOFMEMORY;
    if (cItems)
        m_Hash.SetBuckets(cItems);

    return (S_OK);
} // HRESULT StgGuidPool::InitNew()

//*****************************************************************************
// Load a Guid heap from persisted memory.  If a copy of the data is made
// (so that it may be updated), then a new hash table is generated which can
// be used to elminate duplicates with new Guids.
//*****************************************************************************
HRESULT StgGuidPool::InitOnMem(         // Return code.
    void        *pData,                 // Predefined data.
    ULONG       iSize,                  // Size of data.
    int         bReadOnly)              // true if append is forbidden.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;

    // Let base class init our memory structure.
    if (FAILED(hr = StgPool::InitOnMem(pData, iSize, bReadOnly)))
        return (hr);

    // For init on existing mem case.
    if (pData && iSize)
    {
        // If we cannot update, then we don't need a hash table.
        if (bReadOnly)
            return (S_OK);

        TakeOwnershipOfInitMem();

        // Build the hash table on the data.
        if (FAILED(hr = RehashGuids()))
        {
            Uninit();
            return (hr);
        }
    }

    return (S_OK);
} // HRESULT StgGuidPool::InitOnMem()

//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
void StgGuidPool::Uninit()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Clear the hash table.
    m_Hash.Clear();

    // Let base class clean up.
    StgPool::Uninit();
} // void StgGuidPool::Uninit()

//*****************************************************************************
// Add a segment to the chain of segments.
//*****************************************************************************
HRESULT StgGuidPool::AddSegment(            // S_OK or error.
    const void  *pData,                 // The data.
    ULONG       cbData,                 // Size of the data.
    bool        bCopy)                  // If true, make a copy of the data.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Want an integeral number of GUIDs.
    _ASSERTE((cbData % sizeof(GUID)) == 0);

    return StgPool::AddSegment(pData, cbData, bCopy);

} // HRESULT StgGuidPool::AddSegment()

//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
HRESULT StgGuidPool::SetHash(int bHash)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;

    // Can't do any updates during a reorganization.
    _ASSERTE(m_State == eNormal);

    // If turning on hash again, need to rehash all guids.
    if (bHash)
        hr = RehashGuids();

    m_bHash = bHash;
    return (hr);
} // HRESULT StgGuidPool::SetHash()

//*****************************************************************************
// The Guid will be added to the pool.  The index of the Guid in the pool
// is returned in *piIndex.  If the Guid is already in the pool, then the
// index will be to the existing copy of the Guid.
//*****************************************************************************
HRESULT StgGuidPool::AddGuid(           // Return code.
    REFGUID     pguid,                  // The Guid to add to pool.
    ULONG       *piIndex)               // Return 1-based index of Guid here.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    GUIDHASH    *pHash = NULL;                  // Hash item for add.

    GUID guid = pguid;
    SwapGuid(&guid);
    // Can't do any updates during a reorganization.
    _ASSERTE(m_State == eNormal);

    // Special case for GUID_NULL
    if (guid == GUID_NULL)
    {
        *piIndex = 0;
        return (S_OK);
    }

    // If the hash table is to be kept built (default).
    if (m_bHash)
    {
        // Find or add the entry.
        pHash = m_Hash.Find(&guid, true);
        if (!pHash)
            return (PostError(OutOfMemory()));

        // If the guid was found, just use it.
        if (pHash->iIndex != 0xffffffff)
        {   // Return 1-based index.
            *piIndex = pHash->iIndex;
            return (S_OK);
        }
    }

    // Space on heap for new guid?
    if (sizeof(GUID) > GetCbSegAvailable())
    {
        if (!Grow(sizeof(GUID)))
            return (PostError(OutOfMemory()));
    }

    // Copy the guid to the heap.
    *reinterpret_cast<GUID*>(GetNextLocation()) = guid;
    SetDirty();

    // Give the 1-based index back to caller.
    *piIndex = (GetNextOffset() / sizeof(GUID)) + 1;

    // If hashing, save the 1-based index in the hash.
    if (m_bHash)
        pHash->iIndex = *piIndex;

    // Update heap counters.
    SegAllocate(sizeof(GUID));

    return (S_OK);
} // HRESULT StgGuidPool::AddGuid()

//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
GUID *StgGuidPool::GetGuid(             // Pointer to guid in pool.
    ULONG       iIndex)                 // 1-based index of Guid in pool.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    if (iIndex == 0)
        return (reinterpret_cast<GUID*>(const_cast<BYTE*>(m_zeros)));

    // Convert to 0-based internal form, defer to implementation.
    return (GetGuidi(iIndex-1));
} // GUID *StgGuidPool::GetGuid()



//*****************************************************************************
// Recompute the hashes for the pool.
//*****************************************************************************
HRESULT StgGuidPool::RehashGuids()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    ULONG       iOffset;                // Loop control.
    ULONG       iMax;                   // End of loop.
    ULONG       iSeg;                   // Location within segment.
    StgPoolSeg  *pSeg = this;           // To loop over segments.
    GUIDHASH    *pHash;                 // Hash item for add.
    GUID        *pGuid;                 // A guid;

    // Remove any stale data.
    m_Hash.Clear();

    // How far should the loop go.
    iMax = GetNextOffset();

    // Go through each guid.
    for (iSeg=iOffset=0;  iOffset < iMax;  )
    {
        // Get a pointer to the guid.
        pGuid = reinterpret_cast<GUID*>(pSeg->m_pSegData + iSeg);
        // Add the guid to the hash table.
        if ((pHash = m_Hash.Add(pGuid)) == 0)
            return (PostError(OutOfMemory()));
        pHash->iIndex = iOffset / sizeof(GUID);

        // Move to next Guid.
        iOffset += sizeof(GUID);
        iSeg += sizeof(GUID);
        if (iSeg > pSeg->m_cbSegNext)
        {
            pSeg = pSeg->m_pNextSeg;
            iSeg = 0;
        }
    }
    return (S_OK);
} // HRESULT StgGuidPool::RehashGuids()

//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
HRESULT StgGuidPool::GetNextItem(       // Return code.
    ULONG       ulItem,                 // Current item.
    ULONG       *pulNext)               // Return offset of next pool item.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    ++ulItem;
    
    // Outside of heap, or last item in heap?
    if ((ulItem*sizeof(GUID)) >= GetNextOffset())
    {
        *pulNext = 0;
        return S_FALSE;
    }
    
    *pulNext = ulItem;
    return (S_OK);
} // HRESULT StgGuidPool::GetNextItem()

//*****************************************************************************
// Prepare for pool reorganization.
//*****************************************************************************
HRESULT StgGuidPool::OrganizeBegin()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    int         cRemap;

    // Validate transition.
    _ASSERTE(m_State == eNormal);

    _ASSERTE(m_Remap.Count() == 0);
    cRemap = GetNextIndex();
    if (cRemap == 0)
    {
        m_State = eMarking;
        m_cbOrganizedSize = 0;
        return (S_OK);
    }

    if (!m_Remap.AllocateBlock(cRemap))
        return (PostError(OutOfMemory()));

    memset(m_Remap.Ptr(), 0xff, cRemap * sizeof(m_Remap.Ptr()[0]));
    // Be sure we recognize the "not mapped" value.
    _ASSERTE(m_Remap[0] == ULONG_MAX);

    m_State = eMarking;
    return (S_OK);
} // HRESULT StgGuidPool::OrganizeBegin()

//*****************************************************************************
// Mark an object as being live in the organized pool.
//*****************************************************************************
HRESULT StgGuidPool::OrganizeMark(
    ULONG       ulOffset)               // 1-based index of guid.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate state.
    _ASSERTE(m_State == eMarking);

    // Don't mark special NULL-GUID. Some columns use 0xffffffff as NULL.
    if (ulOffset == 0 || ulOffset == 0xffffffff)
        return (S_OK);

    // Convert to 0-based internal format.
    --ulOffset;

    _ASSERTE(ulOffset < static_cast<ULONG>(m_Remap.Count()));
    m_Remap[ulOffset] = 1;

    return (S_OK);
} // HRESULT StgGuidPool::OrganizeMark()

//*****************************************************************************
// This reorganizes the string pool for minimum size.  This is done by sorting
//  the strings, eliminating any duplicates, and performing tail-merging on
//  any that are left (that is, if "IFoo" is at offset 2, "Foo" will be
//  at offset 3, since "Foo" is a substring of "IFoo").
//
// After this function is called, the only valid operations are RemapOffset and
//  PersistToStream.
//*****************************************************************************
HRESULT StgGuidPool::OrganizePool()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    int         i;                      // Loop control.
    int         iIndex;                 // New index.

    // Validate transition.
    _ASSERTE(m_State == eMarking);
    m_State = eOrganized;

    iIndex = 0;
    for (i=0; i<m_Remap.Count(); ++i)
    {
        if (m_Remap[i] != ULONG_MAX)
            m_Remap[i] = iIndex++;
    }

    // Remember how big the pool will be.
    m_cbOrganizedSize = iIndex * sizeof(GUID);

    return (S_OK);
} // HRESULT StgGuidPool::OrganizePool()

//*****************************************************************************
// Given an offset from before the remap, what is the offset after the remap?
//*****************************************************************************
HRESULT StgGuidPool::OrganizeRemap(
    ULONG       ulOld,                  // Old 1-based offset.
    ULONG       *pulNew)                // Put new 1-based offset here.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate state.
    _ASSERTE(m_State == eOrganized || m_State == eNormal);

    if (ulOld == 0 || ulOld == 0xffffffff || m_State == eNormal)
    {
        *pulNew = ulOld;
        return (S_OK);
    }

    // Convert to 0-based internal form.
    --ulOld;

    // Valid index?
    _ASSERTE(ulOld < static_cast<ULONG>(m_Remap.Count()));
    // Did they map this one?
    _ASSERTE(m_Remap[ulOld] != ULONG_MAX);

    // Give back 1-based external form.
    *pulNew = m_Remap[ulOld] + 1;

    return (S_OK);
} // HRESULT StgGuidPool::OrganizeRemap()

//*****************************************************************************
// Called to leave the organizing state.  Strings may be added again.
//*****************************************************************************
HRESULT StgGuidPool::OrganizeEnd()
{ 
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate transition.
    _ASSERTE(m_State == eOrganized);

    m_Remap.Clear();
    m_cbOrganizedSize = 0;

    m_State = eNormal;
    return (S_OK); 
} // HRESULT StgGuidPool::OrganizeEnd()

//*****************************************************************************
// Save the pool data into the given stream.
//*****************************************************************************
HRESULT StgGuidPool::PersistToStream(// Return code.
    IStream     *pIStream)              // The stream to write to.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    int         i;                      // Loop control.
    GUID        *pGuid;                 // data to write.
    ULONG       cbTotal;                // Size written.
    HRESULT     hr = S_OK;

    // If not reorganized, just let the base class write the data.
    if (m_State == eNormal)
    {
        return StgPool::PersistToStream(pIStream);
    }

    // Verify state.
    _ASSERTE(m_State == eOrganized);

    cbTotal = 0;
    for (i=0; i<m_Remap.Count(); ++i)
    {
        if (m_Remap[i] != ULONG_MAX)
        {   // Use internal form, GetGuidi, to get 0-based index.
            pGuid = GetGuidi(i);
            if (FAILED(hr = pIStream->Write(pGuid, sizeof(GUID), 0)))
                return (hr);
            cbTotal += sizeof(GUID);
        }
    }
    _ASSERTE(cbTotal == m_cbOrganizedSize);

    return (S_OK);
} // HRESULT StgGuidPool::PersistToStream()
//
//
// StgBlobPool
//
//



//*****************************************************************************
// Create a new, empty blob pool.
//*****************************************************************************
HRESULT StgBlobPool::InitNew(           // Return code.
    ULONG       cbSize,                 // Estimated size.
    ULONG       cItems,                 // Estimated item count.
    BOOL        fAddEmptryItem)        // Should we add an empty item at offset 0

{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;                     // A result.
    ULONG       i;                      // Offset of empty blob.

    // Let base class intialize.
    if (FAILED(hr = StgPool::InitNew()))
        return (hr);

    _ASSERTE(m_Remap.Count() == 0);

    // Set initial table sizes, if specified.
    if (cbSize)
        if (!Grow(cbSize))
            return E_OUTOFMEMORY;
    if (cItems)
        m_Hash.SetBuckets(cItems);

    // Init with empty blob.
    
    // Normally must do this, regardless if we currently have anything in the pool.
    // If we don't do this, the first blob that gets added to the pool will
    // have an offset of 0. This will cause this blob to have a token of
    // 0x70000000, which is considered a nil string token.
    //
    // By inserting a zero length blob into the pool the being with, we're
    // assured that the first blob added to the pool will have an offset
    // of 1 and a token of 0x70000001, which is a valid token.
    //
    // The only time we wouldn't want to do this is if we're reading in a delta metadata.
    // Then, we don't care if the first string is at offset 0... when the delta gets applied,
    // the string will get moved to the appropriate offset.
    if (fAddEmptryItem)
    {
        IfFailRet(hr = AddBlob(0, NULL, &i));
        // Empty blob better be at offset 0.
        _ASSERTE(i == 0);
        SetDirty(false);
    }
    return (hr);
} // HRESULT StgBlobPool::InitNew()


//*****************************************************************************
// Init the blob pool for use.  This is called for both create and read case.
// If there is existing data and bCopyData is true, then the data is rehashed
// to eliminate dupes in future adds.
//*****************************************************************************
HRESULT StgBlobPool::InitOnMem(         // Return code.
    void        *pBuf,                  // Predefined data.
    ULONG       iBufSize,               // Size of data.
    int         bReadOnly)              // true if append is forbidden.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    BLOBHASH    *pHash;                 // Hash item for add.
    ULONG       iOffset;                // Loop control.
    void const  *pBlob;                 // Pointer to a given blob.
    ULONG       cbBlob;                 // Length of a blob.
    int         iSizeLen = 0;           // Size of an encoded length.
    HRESULT     hr;

    // Let base class init our memory structure.
    if (FAILED(hr = StgPool::InitOnMem(pBuf, iBufSize, bReadOnly)))
        return (hr);

    // Init hash table from existing data.
    // If we cannot update, we don't need a hash table.
    if (bReadOnly)
        return (S_OK);

    TakeOwnershipOfInitMem();

    // Go through each blob.
    ULONG       iMax;                   // End of loop.
    ULONG       iSeg;                   // Location within segment.
    StgPoolSeg  *pSeg = this;           // To loop over segments.

    // How far should the loop go.
    iMax = GetNextOffset();

    // Go through each string, skipping initial empty string.
    for (iSeg=iOffset=0; iOffset < iMax; )
    {
        // Get the string from the pool.
        pBlob = pSeg->m_pSegData + iSeg;

        // Add the blob to the hash table.
        if ((pHash = m_Hash.Add(pBlob)) == 0)
        {
            Uninit();
            return (E_OUTOFMEMORY);
        }
        pHash->iOffset = iOffset;

        // Move to next blob.
        cbBlob = CPackedLen::GetLength(pBlob, &iSizeLen);
        ULONG       cbCur;                  // Size of length + data.
        cbCur = cbBlob + iSizeLen;
        if (cbCur == 0)
        {
            Uninit();
            return CLDB_E_FILE_CORRUPT;
        }
        iOffset += cbCur;
        iSeg += cbBlob + iSizeLen;
        if (iSeg > pSeg->m_cbSegNext)
        {
            pSeg = pSeg->m_pNextSeg;
            iSeg = 0;
        }
    }
    return (S_OK);
} // HRESULT StgBlobPool::InitOnMem()


//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
void StgBlobPool::Uninit()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Clear the hash table.
    m_Hash.Clear();

    // Let base class clean up.
    StgPool::Uninit();
} // void StgBlobPool::Uninit()


//*****************************************************************************
// The blob will be added to the pool.  The offset of the blob in the pool
// is returned in *piOffset.  If the blob is already in the pool, then the
// offset will be to the existing copy of the blob.
//*****************************************************************************
HRESULT StgBlobPool::AddBlob(           // Return code.
    ULONG       iSize,                  // Size of data item.
    const void  *pData,                 // The data.
    ULONG       *piOffset)              // Return offset of blob here.
{
    BLOBHASH    *pHash;                 // Hash item for add.
    void        *pBytes;                // Working pointer.
    BYTE        *pStartLoc;             // Location to write real blob
    ULONG       iRequired;              // How much buffer for this blob?
    ULONG       iFillerLen;             // space to fill to make byte-aligned

    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Can't do any updates during a reorganization.
    _ASSERTE(m_State == eNormal);

    // Can we handle this blob?
    if (iSize > CPackedLen::MAX_LEN)
        return (PostError(CLDB_E_TOO_BIG));

    // worst case is we need three more bytes to ensure byte-aligned, hence the 3
    iRequired = iSize + CPackedLen::Size(iSize) + 3;
    if (iRequired > GetCbSegAvailable())
    {
        if (!Grow(iRequired))
            return (PostError(OutOfMemory()));
    }

    
    // unless changed due to alignment, the location of the blob is just
    // the value returned by GetNextLocation(), which is also a iFillerLen of
    // 0

    pStartLoc = (BYTE *) GetNextLocation();
    iFillerLen = 0;

    // technichally, only the data portion must be DWORD-aligned.  So, if the
    // data length is zero, we don't need to worry about alignment.

    if (m_bAlign && iSize > 0)
    {
        // figure out how many bytes are between the current location and
        // the position to write the size of the real blob.
        ULONG_PTR   iStart      = (ULONG_PTR)GetNextLocation();
        ULONG iLenSize  = CPackedLen::Size(iSize);
        ULONG       iSum        = ((ULONG)(iStart % sizeof(DWORD))) + iLenSize;
        iFillerLen = (sizeof(DWORD)-((iSum)%sizeof(DWORD)))%sizeof(DWORD);

        // if there is a difference between where we are now and we want to
        // start, put in a filler blob.
        if (iFillerLen > 0)
        {
            // Pack in "filler blob" length.
            pStartLoc = (BYTE *) CPackedLen::PutLength(GetNextLocation(), iFillerLen - 1);

            // Write iFillerLen - 1 bytes of zeros after the length indicator.
            for (ULONG i = 0; i < iFillerLen - 1; i++)
            {
                *pStartLoc++ = 0;
            }
        }       
    } 
    
    // Pack in the length at pStartLoc (the start location)
    pBytes = CPackedLen::PutLength(pStartLoc, iSize);

#if defined(_DEBUG)
    if (m_bAlign && iSize > 0)
        // check to make sure blob write will be DWORD-aligned
        _ASSERTE( ( ( (ULONG_PTR) pBytes ) % sizeof(DWORD) ) == 0);
#endif

    // Put the bytes themselves.
    memcpy(pBytes, pData, iSize);

    // Find or add the entry.
    if ((pHash = m_Hash.Find(GetNextLocation() + iFillerLen, true)) == 0)
        return (PostError(OutOfMemory()));

    // If the entry was new, keep the new blob.
    if (pHash->iOffset == 0xffffffff)
    {
        // this blob's offset is increased by iFillerLen bytes
        pHash->iOffset = *piOffset = GetNextOffset() + iFillerLen;
        // only SegAllocate what we actually used, rather than what we requested
        SegAllocate(iSize + CPackedLen::Size(iSize) + iFillerLen);
        SetDirty();
        
        if (m_Hash.MaxChainLength() > MAX_CHAIN_LENGTH)
            RehashBlobs();
    }
    // Else use the old one.
    else
        *piOffset = pHash->iOffset;
    return (S_OK);
} // HRESULT StgBlobPool::AddBlob()

//*****************************************************************************
// Return a pointer to a blob, and the size of the blob.
//*****************************************************************************
void *StgBlobPool::GetBlob(             // Pointer to blob's bytes. Valid only if *piSize is non-zero
    ULONG       iOffset,                // Offset of blob in pool.
    ULONG       *piSize)                // Return size of blob.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    void const  *pData;                 // Pointer to blob's bytes.

    // This should not be a necessary special case.  The zero byte at the 
    //  start of the pool will code for a length of zero.  We will return
    //  a pointer to the next length byte, but the caller should notice that
    //  the size is zero, and should not look at any bytes.

    // Is the offset within this heap?
    //_ASSERTE(IsValidOffset(iOffset));
    if(!IsValidOffset(iOffset))
    {
#ifdef _DEBUG
        if(REGUTIL::GetConfigDWORD(L"AssertOnBadImageFormat", 0))
            _ASSERTE(!"Invalid Blob Offset");
#endif
        iOffset = 0;
    }

    // Get size of the blob (and pointer to data).
    pData = CPackedLen::GetData(GetData(iOffset), piSize);

    // Sanity check the return alignment.
    _ASSERTE(!IsAligned() || (((UINT_PTR)(pData) % sizeof(DWORD)) == 0));

    // Return pointer to data.
    return (const_cast<void*>(pData));
} // void *StgBlobPool::GetBlob()

//*****************************************************************************
// Return a pointer to a blob, the size of the blob, and the offset of 
//  the next blob (-1 at end).
//*****************************************************************************
void *StgBlobPool::GetBlobNext(         // Pointer to blob's bytes.
    ULONG       iOffset,                // Offset of blob in pool.
    ULONG       *piSize,                // Return size of blob.
    ULONG       *piNext)                // Return offset of next blob.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    const BYTE  *pData;                 // Pointer to blob's bytes.
    int         iLen = 0;               // Length of the length field.
    ULONG       ulNext;                 // Offset of next blob.

    if (iOffset == 0)
    {
        *piSize = 0;
        if (1 < GetNextOffset())
            *piNext = 1;
        else
            *piNext = (ULONG) -1;
        return (const_cast<BYTE*>(m_zeros));
    }
    else
    if (iOffset == (ULONG) -1)
    {
        *piSize = 0;
        *piNext = (ULONG) -1;
        return (const_cast<BYTE*>(m_zeros));
    }

    // Is the offset within this heap?
    _ASSERTE(IsValidOffset(iOffset));

    // Get size of the blob, and the size of the size.
    pData = GetData(iOffset);
    *piSize = CPackedLen::GetLength(pData, &iLen);

    // Get the blob itself.
    pData += iLen;

    // Get the offset of the next blob.
    ulNext = iOffset + *piSize + iLen;
    if (ulNext < GetNextOffset())
        *piNext = ulNext;
    else
        *piNext = (ULONG) -1;

    // Sanity check the return alignment.
    _ASSERTE(!IsAligned() || (((UINT_PTR)(pData) % sizeof(DWORD)) == 0));

    // Return pointer to data.
    return (const_cast<BYTE*>(pData));
} // void *StgBlobPool::GetBlobNext()

//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
HRESULT StgBlobPool::SetHash(int bHash)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr = S_OK;

    // Can't do any updates during a reorganization.
    _ASSERTE(m_State == eNormal);

    // If turning on hash again, need to rehash all Blobs.
    if (bHash)
        hr = RehashBlobs();

    return (hr);
} // HRESULT StgBlobPool::SetHash()

//*****************************************************************************
// Clears out the existing hash table used to eliminate duplicates.  Then
// rebuilds the hash table from scratch based on the current data.
//*****************************************************************************
HRESULT StgBlobPool::RehashBlobs()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    void const  *pBlob;                 // Pointer to a given blob.
    ULONG       cbBlob;                 // Length of a blob.
    int         iSizeLen = 0;           // Size of an encoded length.
    ULONG       iOffset;                // Location within iteration.
    ULONG       iMax;                   // End of loop.
    ULONG       iSeg;                   // Location within segment.
    StgPoolSeg  *pSeg = this;           // To loop over segments.
    BLOBHASH    *pHash;                 // Hash item for add.
    int         iBuckets;               // Buckets in the hash.
    int         iCount;                 // Items in the hash.
    int         iNewBuckets;            // New count of buckets in the hash.

    // Determine the new bucket size.
    iBuckets = m_Hash.Buckets();
    iCount = m_Hash.Count();
    iNewBuckets = max(iCount, iBuckets+iBuckets/2+1);
        
    // Remove any stale data.
    m_Hash.Clear();
    m_Hash.SetBuckets(iNewBuckets);
    
    // How far should the loop go.
    iMax = GetNextOffset();

    // Go through each string, skipping initial empty string.
    for (iSeg=iOffset=0; iOffset < iMax; )
    {
        // Get the string from the pool.
        pBlob = pSeg->m_pSegData + iSeg;

        // Add the blob to the hash table.
        if ((pHash = m_Hash.Add(pBlob)) == 0)
        {
            Uninit();
            return (E_OUTOFMEMORY);
        }
        pHash->iOffset = iOffset;

        // Move to next blob.
        cbBlob = CPackedLen::GetLength(pBlob, &iSizeLen);
        cbBlob = CPackedLen::GetLength(pBlob, &iSizeLen);
        ULONG       cbCur;                  // Size of length + data.
        cbCur = cbBlob + iSizeLen;
        if (cbCur == 0)
        {
            Uninit();
            return CLDB_E_FILE_CORRUPT;
        }
        iOffset += cbCur;
        iSeg += cbBlob + iSizeLen;
        if (iSeg >= pSeg->m_cbSegNext)
        {
            pSeg = pSeg->m_pNextSeg;
            iSeg = 0;
        }
    }
    return (S_OK);
} // HRESULT StgBlobPool::RehashBlobs()

//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
HRESULT StgBlobPool::GetNextItem(       // Return code.
    ULONG       ulItem,                 // Current item.
    ULONG       *pulNext)               // Return offset of next pool item.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    void const  *pBlob;                 // Pointer to a given blob.
    ULONG       cbBlob;                 // Length of a blob.
    int         iSizeLen = 0;           // Size of an encoded length.

    // Outside of heap?
    if (ulItem >= GetNextOffset())
    {
        *pulNext = 0;
        return S_FALSE;
    }
    
    pBlob = GetData(ulItem);
    // Move to next blob.
    cbBlob = CPackedLen::GetLength(pBlob, &iSizeLen);
    ulItem += cbBlob + iSizeLen;
    
    // Was it the last item in heap?
    if (ulItem >= GetNextOffset())
    {
        *pulNext = 0;
        return S_FALSE;
    }
    
    *pulNext = ulItem;
    return (S_OK);
} // HRESULT StgBlobPool::GetNextItem()

//*****************************************************************************
// Prepare for pool reorganization.
//*****************************************************************************
HRESULT StgBlobPool::OrganizeBegin()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    m_cbOrganizedOffset = 0;
    return (StgPool::OrganizeBegin());
} // HRESULT StgBlobPool::OrganizeBegin()

//*****************************************************************************
// Mark an object as being live in the organized pool.
//*****************************************************************************
HRESULT StgBlobPool::OrganizeMark(
    ULONG       ulOffset)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    int         iContainer;             // Index for insert, if not already in list.
    StgBlobRemap  *psRemap;             // Found entry.

    // Validate state.
    _ASSERTE(m_State == eMarking);

    // Don't mark 0 (empty) entry.  Some columns use 0xffffffff as a null flag.
    if (ulOffset == 0 || ulOffset == 0xffffffff)
        return (S_OK);
    
    // Is the offset within this heap?
    _ASSERTE(IsValidOffset(ulOffset));

    StgBlobRemap    sTarget = {ulOffset};// For the search, only contains ulOldOffset.
    BinarySearch Searcher(m_Remap.Ptr(), m_Remap.Count()); // Searcher object

    // Do the search, done if found.
    if ((psRemap = const_cast<StgBlobRemap*>(Searcher.Find(&sTarget, &iContainer))))
        return (S_OK);

    // Add the entry to the remap array.
    if ((psRemap = m_Remap.Insert(iContainer)) == 0)
        return (PostError(OutOfMemory()));

    psRemap->ulOldOffset = ulOffset;
    psRemap->iNewOffset = -1;
    return (S_OK);
} // HRESULT StgBlobPool::OrganizeMark()

//*****************************************************************************
// This reorganizes the blob pool for minimum size. 
//
// After this function is called, the only valid operations are RemapOffset and
//  PersistToStream.
//*****************************************************************************
HRESULT StgBlobPool::OrganizePool()
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    ULONG       ulOffset;               // New offset of a blob.
    int         i;                      // Loop control.
    ULONG       iFillerLen;             // Size of pre-blob filler to maintain alignment
    ULONG       cbBlob;                 // Size of a blob.
    int         cbLen = 0;              // Size of a length.
    // Validate transition.
    _ASSERTE(m_State == eMarking);

    m_State = eOrganized;

    // If nothing to save, we're done.
    if (m_Remap.Count() == 0)
    {
        m_cbOrganizedSize = 0;
        return (S_OK);
    }

    // Start past the empty blob.
    ulOffset = 1;

    // Go through the remap array, and assign each item it's new offset.
    for (i=0; i<m_Remap.Count(); ++i)
    {
        // Still at a valid offset within this heap?
        _ASSERTE(IsValidOffset(ulOffset));

        // Get size of the blob and of length.
        cbBlob = CPackedLen::GetLength(GetData(m_Remap[i].ulOldOffset), &cbLen);

        // For alignment case, need to add in expected padding.
        if (m_bAlign)
        {
            ULONG iSum = (ulOffset % sizeof(DWORD)) + cbLen;
            iFillerLen = (sizeof(DWORD)-((iSum)%sizeof(DWORD)))%sizeof(DWORD);
        }
        else
            iFillerLen = 0;

        // Set the mapping values.
        m_Remap[i].iNewOffset = ulOffset + iFillerLen;
        m_cbOrganizedOffset = m_Remap[i].iNewOffset;

        // Adjust offset to next blob.
        ulOffset += cbBlob + cbLen + iFillerLen;
    }

    // How big is the whole thing?
    m_cbOrganizedSize = ALIGN4BYTE(ulOffset);

    return (S_OK);
} // HRESULT StgBlobPool::OrganizePool()

//*****************************************************************************
// Given an offset from before the remap, what is the offset after the remap?
//*****************************************************************************
HRESULT StgBlobPool::OrganizeRemap(
    ULONG       ulOld,                  // Old offset.
    ULONG       *pulNew)                // Put new offset here.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    // Validate state.
    _ASSERTE(m_State == eOrganized || m_State == eNormal);

    // If not reorganized, new == old.
    if (m_State == eNormal)
    {
        *pulNew = ulOld;
        return (S_OK);
    }

    // Empty blob translates to self.  Some columns use 0xffffffff as a null flag.
    if (ulOld == 0 || ulOld == 0xffffffff)
    {
        *pulNew = ulOld;
        return (S_OK);
    }

    // Search for old index.  
    int         iContainer;                 // Index of containing Blob, if not in map.
    StgBlobRemap const *psRemap;                // Found entry.
    StgBlobRemap    sTarget = {ulOld};          // For the search, only contains ulOldOffset.
    BinarySearch Searcher(m_Remap.Ptr(), m_Remap.Count()); // Searcher object

    // Do the search.
    psRemap = Searcher.Find(&sTarget, &iContainer);
    // Found?
    if (psRemap)
    {   // Yes.
        _ASSERTE(psRemap->iNewOffset >= 0);
        *pulNew = static_cast<ULONG>(psRemap->iNewOffset);
        return (S_OK);
    }

    // Not Found, translate to SQL-style NULL.
    _ASSERTE(!"Remap a non-marked blob.");
    *pulNew = 0xffffffff;

    return (S_OK);
} // HRESULT StgBlobPool::OrganizeRemap()

//*****************************************************************************
// Called to leave the organizing state.  Blobs may be added again.
//*****************************************************************************
HRESULT StgBlobPool::OrganizeEnd()
{ 
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    _ASSERTE(m_State == eOrganized);

    m_Remap.Clear(); 
    m_State = eNormal;
    m_cbOrganizedSize = 0;

    return (S_OK); 
} // HRESULT StgBlobPool::OrganizeEnd()

//*****************************************************************************
// The entire Blob pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
HRESULT StgBlobPool::PersistToStream(   // Return code.
    IStream     *pIStream)              // The stream to write to.
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;                     // A result.
    StgBlobRemap *psRemap;              // A remap entry.
    ULONG       ulTotal;                // Bytes written so far.
    int         i;                      // Loop control.
    ULONG       cbBlob;                 // Size of a blob.
    int         cbLen = 0;              // Size of a length.
    BYTE        *pBlob;                 // Pointer to a blob.
    ULONG       iFillerLen;             // Size of pre-blob filler to maintain alignment

    // If not reorganized, just let the base class write the data.
    if (m_State == eNormal)
    {
        return StgPool::PersistToStream(pIStream);
    }

    // Validate state.
    _ASSERTE(m_State == eOrganized);

    // If there is any blob data at all, then start pool with empty blob.
    if (m_Remap.Count())
    {
        hr = 0; // cheeze -- use hr as a buffer for 0
        if (FAILED(hr = pIStream->Write(&hr, 1, 0)))
            return (hr);
        ulTotal = 1;
    }
    else
        ulTotal = 0;

    // Iterate over the map writing Blobs.  
    for (i=0; i<m_Remap.Count(); ++i)
    {
        // Get the remap entry.
        psRemap = m_Remap.Get(i);

        // Get size of the blob and of length.
        pBlob = GetData(psRemap->ulOldOffset);
        cbBlob = CPackedLen::GetLength(pBlob, &cbLen);

        if (m_bAlign)
        {
            ULONG iSum = (ulTotal % sizeof(DWORD)) + cbLen;
            iFillerLen = (sizeof(DWORD)-((iSum)%sizeof(DWORD)))%sizeof(DWORD);

            // if there is a difference between where we are now and we want to
            // start, put in a filler blob.
            if (iFillerLen > 0)
            {
                BYTE    rgFillBlob[sizeof(DWORD)];

                // Zero out buffer.             
                *(DWORD *) rgFillBlob = 0;

                // Pack in "filler blob" length, we know it will only be 1 byte
                CPackedLen::PutLength(&rgFillBlob[0], iFillerLen - 1);
                if (FAILED(hr = pIStream->Write(rgFillBlob, iFillerLen, 0)))
                    return (hr);

                ulTotal += iFillerLen;
            }
        }
        else
            iFillerLen = 0;

        // Is this what we expected?
        _ASSERTE(ulTotal == static_cast<ULONG>(psRemap->iNewOffset));

#if defined (_DEBUG)
    // check to make sure that we are writing aligned if desired
    if (m_bAlign)
        _ASSERTE( (ulTotal + cbLen) % sizeof(DWORD) == 0 );
#endif

        // Write the data.
        if (FAILED(hr = pIStream->Write(pBlob, cbBlob+cbLen, 0)))
            return (hr);

        // Accumulate the bytes.
        ulTotal += cbBlob + cbLen;
    }

    // Align.
    if (ulTotal != ALIGN4BYTE(ulTotal))
    {
        hr = 0;
        if (FAILED(hr = pIStream->Write(&hr, ALIGN4BYTE(ulTotal)-ulTotal, 0)))
            return (hr);
        ulTotal += ALIGN4BYTE(ulTotal)-ulTotal;
    }

    // Should have written exactly what we expected.
    _ASSERTE(ulTotal == m_cbOrganizedSize);

    return (S_OK);
} // HRESULT StgBlobPool::PersistToStream()


//
// CInMemoryStream
//


ULONG STDMETHODCALLTYPE CInMemoryStream::Release()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    ULONG       cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        if (m_dataCopy != NULL)
            delete [] m_dataCopy;
        
        delete this;
    }
    return (cRef);
} // ULONG STDMETHODCALLTYPE CInMemoryStream::Release()

HRESULT STDMETHODCALLTYPE CInMemoryStream::QueryInterface(REFIID riid, PVOID *ppOut)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    if (!ppOut)
    {
        return E_POINTER;
    }

    *ppOut = NULL;
    if (riid == IID_IStream || riid == IID_ISequentialStream || riid == IID_IUnknown)
    {
        *ppOut = this;
        AddRef();
        return (S_OK);
    }

    return E_NOINTERFACE;

} // HRESULT STDMETHODCALLTYPE CInMemoryStream::QueryInterface()

HRESULT STDMETHODCALLTYPE CInMemoryStream::Read(
                               void        *pv,
                               ULONG       cb,
                               ULONG       *pcbRead)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

    ULONG       cbRead = min(cb, m_cbSize - m_cbCurrent);

    if (cbRead == 0)
        return (S_FALSE);
    memcpy(pv, (void *) ((ULONG_PTR) m_pMem + m_cbCurrent), cbRead);
    if (pcbRead)
        *pcbRead = cbRead;
    m_cbCurrent += cbRead;
    return (S_OK);
} // HRESULT STDMETHODCALLTYPE CInMemoryStream::Read()

HRESULT STDMETHODCALLTYPE CInMemoryStream::Write(
                                const void  *pv,
                                ULONG       cb,
                                ULONG       *pcbWritten)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

    if (ovadd_gt(m_cbCurrent, cb, m_cbSize))
        return (OutOfMemory());

    memcpy((BYTE *) m_pMem + m_cbCurrent, pv, cb);
    m_cbCurrent += cb;
    if (pcbWritten) *pcbWritten = cb;
    return (S_OK);
} // HRESULT STDMETHODCALLTYPE CInMemoryStream::Write()

HRESULT STDMETHODCALLTYPE CInMemoryStream::Seek(LARGE_INTEGER dlibMove,
                               DWORD       dwOrigin,
                               ULARGE_INTEGER *plibNewPosition)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

    _ASSERTE(dwOrigin == STREAM_SEEK_SET || dwOrigin == STREAM_SEEK_CUR);
    _ASSERTE(dlibMove.QuadPart <= ULONG_MAX);

    if (dwOrigin == STREAM_SEEK_SET)
    {
        m_cbCurrent = (ULONG) dlibMove.QuadPart;
    }
    else
    if (dwOrigin == STREAM_SEEK_CUR)
    {
        m_cbCurrent+= (ULONG)dlibMove.QuadPart;
    }
    //
    //
    //
    if (plibNewPosition)
    {
        if (m_noHacks)
            plibNewPosition->QuadPart = m_cbCurrent;
        else
            plibNewPosition->u.LowPart=0;
    }

    return (m_cbCurrent < m_cbSize) ? (S_OK) : E_FAIL;
} // HRESULT STDMETHODCALLTYPE CInMemoryStream::Seek()

HRESULT STDMETHODCALLTYPE CInMemoryStream::CopyTo(
                                 IStream     *pstm,
                                 ULARGE_INTEGER cb,
                                 ULARGE_INTEGER *pcbRead,
                                 ULARGE_INTEGER *pcbWritten)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

    HRESULT     hr;
    // We don't handle pcbRead or pcbWritten.
    _ASSERTE(pcbRead == 0);
    _ASSERTE(pcbWritten == 0);

    _ASSERTE(cb.QuadPart <= ULONG_MAX);
    ULONG       cbTotal = min(static_cast<ULONG>(cb.QuadPart), m_cbSize - m_cbCurrent);
    ULONG       cbRead=min(1024, cbTotal);
    CQuickBytes rBuf;
    void        *pBuf = rBuf.AllocNoThrow(cbRead);
    if (pBuf == 0)
        return (PostError(OutOfMemory()));

    while (cbTotal)
        {
            if (cbRead > cbTotal)
                cbRead = cbTotal;
            if (FAILED(hr=Read(pBuf, cbRead, 0)))
                return (hr);
            if (FAILED(hr=pstm->Write(pBuf, cbRead, 0)))
                return (hr);
            cbTotal -= cbRead;
        }

    // Adjust seek pointer to the end.
    m_cbCurrent = m_cbSize;

    return (S_OK);
} // HRESULT STDMETHODCALLTYPE CInMemoryStream::CopyTo()

HRESULT CInMemoryStream::CreateStreamOnMemory(           // Return code.
                                    void        *pMem,                  // Memory to create stream on.
                                    ULONG       cbSize,                 // Size of data.
                                    IStream     **ppIStream,                      // Return stream object here.
                                    BOOL        fDeleteMemoryOnRelease
                                    )  
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    CInMemoryStream *pIStream;          // New stream object.
    if ((pIStream = new (nothrow) CInMemoryStream) == 0)
        return (PostError(OutOfMemory()));
    pIStream->InitNew(pMem, cbSize);
    if (fDeleteMemoryOnRelease)
    {
        // make sure this memory is allocated using new
        pIStream->m_dataCopy = (BYTE *)pMem;
    }
    *ppIStream = pIStream;
    return (S_OK);
} // HRESULT CInMemoryStream::CreateStreamOnMemory()

HRESULT CInMemoryStream::CreateStreamOnMemoryNoHacks(void *pMem,
                                                     ULONG cbSize,
                                                     IStream **ppIStream)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    CInMemoryStream *pIStream;          // New stream object.
    if ((pIStream = new (nothrow) CInMemoryStream) == 0)
        return (PostError(OutOfMemory()));
    pIStream->InitNew(pMem, cbSize);
    pIStream->m_noHacks = true;
    *ppIStream = pIStream;
    return (S_OK);
}

HRESULT CInMemoryStream::CreateStreamOnMemoryCopy(void *pMem,
                                                  ULONG cbSize,
                                                  IStream **ppIStream)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    CInMemoryStream *pIStream;          // New stream object.
    if ((pIStream = new (nothrow) CInMemoryStream) == 0)
        return (PostError(OutOfMemory()));

    // Init the stream.
    pIStream->m_cbCurrent = 0;
    pIStream->m_noHacks = true;
    pIStream->m_cbSize = cbSize;

    // Copy the data.
    pIStream->m_dataCopy = new (nothrow) BYTE[cbSize];

    if (pIStream->m_dataCopy == NULL)
    {
        delete pIStream;
        return (PostError(OutOfMemory()));
    }
    
    pIStream->m_pMem = pIStream->m_dataCopy;
    memcpy(pIStream->m_dataCopy, pMem, cbSize);

    *ppIStream = pIStream;
    return (S_OK);
}

//---------------------------------------------------------------------------
// CGrowableStream is a simple IStream implementation that grows as
// its written to. All the memory is contigious, so read access is
// fast. A grow does a realloc, so be aware of that if you're going to
// use this.
//---------------------------------------------------------------------------

CGrowableStream::CGrowableStream() 
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_swBuffer = NULL;
    m_dwBufferSize = 0;
    m_dwBufferIndex = 0;
    m_cRef = 1;
}

CGrowableStream::~CGrowableStream() 
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Destroy the buffer.
    if (m_swBuffer != NULL)
        delete [] m_swBuffer;

    m_swBuffer = NULL;
    m_dwBufferSize = 0;
}

ULONG STDMETHODCALLTYPE CGrowableStream::Release()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    ULONG       cRef = InterlockedDecrement(&m_cRef);

    if (cRef == 0)
        delete this;

    return (cRef);
}

HRESULT STDMETHODCALLTYPE CGrowableStream::QueryInterface(REFIID riid,
                                                          PVOID *ppOut)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    *ppOut = this;
    AddRef();
    return (S_OK);
}

HRESULT CGrowableStream::Read(void * pv,
                              ULONG cb,
                              ULONG * pcbRead)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    HRESULT hr = S_OK;
    DWORD dwCanReadBytes = 0;

    if (NULL == pv)
        return E_POINTER;

    // short-circuit a zero-length read or see if we are at the end
    if (cb == 0 || m_dwBufferIndex >= m_dwBufferSize)
    {
        if (pcbRead != NULL)
            *pcbRead = 0;

        return S_OK;
    }

    // Figure out if we have enough room in the buffer
    dwCanReadBytes = cb;

    if ((dwCanReadBytes + m_dwBufferIndex) > m_dwBufferSize)
        dwCanReadBytes = (m_dwBufferSize - m_dwBufferIndex);

    // copy from our buffer to caller's buffer
    memcpy(pv, &m_swBuffer[m_dwBufferIndex], dwCanReadBytes);

    // adjust our current position
    m_dwBufferIndex += dwCanReadBytes;

    // if they want the info, tell them how many byte we read for them
    if (pcbRead != NULL)
        *pcbRead = dwCanReadBytes;

    return hr;
}

HRESULT CGrowableStream::Write(const void * pv,
                               ULONG cb,
                               ULONG * pcbWritten)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    HRESULT hr = S_OK;
    DWORD dwActualWrite = 0;

    // avoid NULL write
    if (cb == 0)
    {
        hr = S_OK;
        goto Error;
    }

    // Check if our buffer is large enough
    _ASSERTE(m_dwBufferIndex <= m_dwBufferSize);
    
    if (cb > (m_dwBufferSize - m_dwBufferIndex))
    {
        // Grow at least a page at a time.
        DWORD size = m_dwBufferSize + max(cb, 4096);

        char *tmp = new (nothrow) char[size];
        if (m_swBuffer) {
            memcpy (tmp, m_swBuffer, m_dwBufferSize);
            delete [] m_swBuffer;
        }
        m_swBuffer = tmp;
            
        if (m_swBuffer == NULL)
        {
            m_dwBufferSize = 0;
            return E_OUTOFMEMORY;
        }
        
        m_dwBufferSize = size;
    }
    
    if ((pv != NULL) && (cb > 0))
    {
        // write to current position in the buffer
        memcpy(&m_swBuffer[m_dwBufferIndex], pv, cb);

        // now update our current index
        m_dwBufferIndex += cb;

        // in case they want to know the number of bytes written
        dwActualWrite = cb;
    }

Error:
    if (pcbWritten)
        *pcbWritten = dwActualWrite;

    return hr;
}

STDMETHODIMP CGrowableStream::Seek(LARGE_INTEGER dlibMove,
                                   DWORD dwOrigin,
                                   ULARGE_INTEGER * plibNewPosition)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    // a Seek() call on STREAM_SEEK_CUR and a dlibMove == 0 is a
    // request to get the current seek position.
    if ((dwOrigin == STREAM_SEEK_CUR && dlibMove.u.LowPart == 0) &&
        (dlibMove.u.HighPart == 0) &&
        (NULL != plibNewPosition))
    {
        goto Error;        
    }

    // we don't support STREAM_SEEK_SET (beginning of buffer)
    if (dwOrigin != STREAM_SEEK_SET)
        return E_NOTIMPL;

    // did they ask to seek past end of buffer?
    if (dlibMove.u.LowPart > m_dwBufferSize)
        return E_UNEXPECTED;

    // we ignore the high part of the large integer
    m_dwBufferIndex = dlibMove.u.LowPart;

Error:
    if (NULL != plibNewPosition)
    {
        plibNewPosition->u.HighPart = 0;
        plibNewPosition->u.LowPart = m_dwBufferIndex;
    }

    return S_OK;
}
STDMETHODIMP CGrowableStream::SetSize(ULARGE_INTEGER libNewSize)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    DWORD dwNewSize = libNewSize.u.LowPart;

    _ASSERTE(libNewSize.u.HighPart == 0);

    // we don't support large allocations
    if (libNewSize.u.HighPart > 0)
        return E_OUTOFMEMORY;

    char *tmp = new (nothrow) char[dwNewSize];
    if (m_swBuffer) {               // existing allocation, must realloc
        memcpy (tmp, m_swBuffer, m_dwBufferSize);
        delete [] m_swBuffer;
    }
    m_swBuffer = tmp;
        
    if (m_swBuffer == NULL)
    {
        m_dwBufferSize = 0;
        return E_OUTOFMEMORY;
    }
    else
        m_dwBufferSize = dwNewSize;
        
    return S_OK;
}

STDMETHODIMP CGrowableStream::Stat(STATSTG * pstatstg, DWORD grfStatFlag)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY

    if (NULL == pstatstg)
        return E_POINTER;

    // this is the only useful information we hand out - the size of the stream
    pstatstg->cbSize.u.HighPart = 0;
    pstatstg->cbSize.u.LowPart = m_dwBufferSize;
    pstatstg->type = STGTY_STREAM;

    // we ignore the grfStatFlag - we always assume STATFLAG_NONAME
    pstatstg->pwcsName = NULL;

    pstatstg->grfMode = 0;
    pstatstg->grfLocksSupported = 0;
    pstatstg->clsid = CLSID_NULL;
    pstatstg->grfStateBits = 0;

    return S_OK;
}


