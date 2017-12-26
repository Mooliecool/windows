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
// StgPool.h
//
// Pools are used to reduce the amount of data actually required in the database.
// This allows for duplicate string and binary values to be folded into one
// copy shared by the rest of the database.  Strings are tracked in a hash
// table when insert/changing data to find duplicates quickly.  The strings
// are then persisted consecutively in a stream in the database format.
//
//*****************************************************************************
#ifndef __StgPool_h__
#define __StgPool_h__

#ifdef _MSC_VER
#pragma warning (disable : 4355)		// warning C4355: 'this' : used in base member initializer list
#endif

#include "stgpooli.h"					// Internal helpers.
#include "corerror.h"					// Error codes.
#include "metadatatracker.h"
#include "metamodelpub.h"
#include "sarray.h"


//*****************************************************************************
// NOTE:
// One limitation with the pools, we have no way to removing strings from
// the pool.  To remove, you need to know the ref count on the string, and
// need the ability to compact the pool and reset all references.
//*****************************************************************************

//********** Constants ********************************************************
const int DFT_STRING_HEAP_SIZE = 2048;
const int DFT_GUID_HEAP_SIZE = 2048;
const int DFT_BLOB_HEAP_SIZE = 1024;
const int DFT_VARIANT_HEAP_SIZE = 512;
const int DFT_CODE_HEAP_SIZE = 8192;



// Forwards.
class StgStringPool;
class StgBlobPool;
class StgCodePool;
struct CorProfileData;


struct HotPoolDataHeader
{
    ULONG DataOffset;
    ULONG DataIndicesOffset;
    ULONG RidsOffset;
};

//  Perform binary search on RID array
//
class RIDBinarySearch : public CBinarySearch<RID>
{
    public:
    RIDBinarySearch(const RID *pBase, int iCount) : CBinarySearch<RID>(pBase, iCount) 
    { 
        LEAF_CONTRACT; 
    }// RIDBinarySearch

    int Compare(RID const *pFirst, RID const *pSecond)
    {
        LEAF_CONTRACT;

        if (*pFirst < *pSecond)
            return -1;

        if (*pFirst > *pSecond)
            return 1;

        return 0;
    }// Compare

};

//*****************************************************************************
// This class provides common definitions for heap segments.  It is both the
//  base class for the heap, and the class for heap extensions (additional
//  memory that must be allocated to grow the heap).
//*****************************************************************************
class StgPoolSeg
{
public:
	StgPoolSeg() : 
		m_pSegData((BYTE*)m_zeros), 
		m_pNextSeg(0), 
		m_cbSegSize(0), 
		m_cbSegNext(0) 
	{LEAF_CONTRACT;  }
	~StgPoolSeg() 
	{ LEAF_CONTRACT; _ASSERTE(m_pSegData == m_zeros);_ASSERTE(m_pNextSeg == 0); }
protected:
	BYTE		*m_pSegData;			// Pointer to the data.
	StgPoolSeg	*m_pNextSeg;			// Pointer to next segment, or 0.
	ULONG		m_cbSegSize;			// Size of the buffer.  Note that this may
										//  be less than the allocated size of the
										//  buffer, if the segment has been filled
										//  and a "next" allocated.
	ULONG		m_cbSegNext;			// Offset of next available byte in segment.
										//  Segment relative.

	friend class StgPool;
    friend class StgStringPool;
	friend class StgGuidPool;
	friend class StgBlobPool;
	friend class RecordPool;

public:
	const BYTE *GetSegData() const { LEAF_CONTRACT; return m_pSegData; }
	const StgPoolSeg* GetNextSeg() const { LEAF_CONTRACT; return m_pNextSeg; }
	ULONG GetSegSize() const { LEAF_CONTRACT; return m_cbSegSize; }

	static const BYTE m_zeros[16];			// array of zeros for "0" indices.
};

//
//
// StgPoolReadOnly
//
// 
//*****************************************************************************
// This is the read only StgPool class
//*****************************************************************************
class StgPoolReadOnly : public StgPoolSeg
{
friend class CBlobPoolHash;

public:
	StgPoolReadOnly() 
        : m_hotDataHeader( NULL )
    { LEAF_CONTRACT; };

	~StgPoolReadOnly();

	
//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
	virtual HRESULT InitOnMemReadOnly(				// Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize);					// Size of data.

//*****************************************************************************
// Prepare to shut down or reinitialize.
//*****************************************************************************
	virtual	void Uninit();

//*****************************************************************************
// Return the size of the pool.
//*****************************************************************************
	virtual ULONG GetPoolSize()
	{ LEAF_CONTRACT; return m_cbSegSize; }

//*****************************************************************************
// Indicate if heap is empty.
//*****************************************************************************
	virtual int IsEmpty()					// true if empty.
	{ LEAF_CONTRACT; return (m_pSegData == m_zeros); }

//*****************************************************************************
// How big is a cookie for this heap.
//*****************************************************************************
	virtual int OffsetSize()
	{
	    LEAF_CONTRACT;

        if (m_cbSegSize < USHRT_MAX)
			return (sizeof(USHORT));
		else
			return (sizeof(ULONG));
	}

//*****************************************************************************
// true if the heap is read only.
//*****************************************************************************
	virtual int IsReadOnly() { LEAF_CONTRACT; return true ;};

//*****************************************************************************
// Is the given cookie a valid offset, index, etc?
//*****************************************************************************
	virtual int IsValidCookie(ULONG ulCookie)
	{ WRAPPER_CONTRACT; return (IsValidOffset(ulCookie)); }


//*****************************************************************************
// Return a pointer to a null terminated string given an offset previously
// handed out by AddString or FindString.
//*****************************************************************************
	FORCEINLINE LPCSTR GetStringReadOnly(	// Pointer to string.
		ULONG		iOffset)				// Offset of string in pool.
	{ WRAPPER_CONTRACT;    return (reinterpret_cast<LPCSTR>(GetDataReadOnly(iOffset))); }

//*****************************************************************************
// Return a pointer to a null terminated string given an offset previously
// handed out by AddString or FindString.
//*****************************************************************************
	FORCEINLINE LPCSTR GetString(			// Pointer to string.
		ULONG		iOffset)				// Offset of string in pool.
	{ WRAPPER_CONTRACT;    return (reinterpret_cast<LPCSTR>(GetData(iOffset))); }

//*****************************************************************************
// Convert a string to UNICODE into the caller's buffer.
//*****************************************************************************
	virtual HRESULT GetStringW(						// Return code.
		ULONG		iOffset,				// Offset of string in pool.
        __out_ecount(cchBuffer) LPWSTR      szOut,                  // Output buffer for string.
		int			cchBuffer);				// Size of output buffer.

//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
	virtual GUID *GetGuid(					// Pointer to guid in pool.
		ULONG		iIndex);				// 1-based index of Guid in pool.



//*****************************************************************************
// Copy a GUID into the caller's buffer.
//*****************************************************************************
	virtual HRESULT GetGuid(				// Return code.
		ULONG		iIndex,					// 1-based index of Guid in pool.
		GUID		*pGuid)					// Output buffer for Guid.
	{
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		*pGuid = *GetGuid(iIndex);
		SwapGuid(pGuid);
		return (S_OK);
	}


//*****************************************************************************
// Return a pointer to a null terminated blob given an offset previously
// handed out by Addblob or Findblob.
//*****************************************************************************
	virtual void *GetBlob(					// Pointer to blob's bytes.
		ULONG		iOffset,				// Offset of blob in pool.
		ULONG		*piSize);				// Return size of blob.

//*****************************************************************************
//  Initialize hot data structures
//*****************************************************************************
    void InitHotData( HotPoolDataHeader * header )
    {
        LEAF_CONTRACT;
        m_hotDataHeader = header;
    }

//*****************************************************************************
//  Retrieve hot data
//*****************************************************************************
    void const * GetHotData( ULONG offset ) const;

protected:

//*****************************************************************************
// Check whether a given offset is valid in the pool.
//*****************************************************************************
    virtual int IsValidOffset(ULONG ulOffset)
    {LEAF_CONTRACT;  return ulOffset == 0 || (m_pSegData != m_zeros && ulOffset < m_cbSegSize); }

//*****************************************************************************
// Get a pointer to an offset within the heap.  Inline for base segment,
//  helper for extension segments.
//*****************************************************************************
    FORCEINLINE BYTE *GetDataReadOnly(ULONG ulOffset)
    {
        LEAF_CONTRACT;
        _ASSERTE(IsReadOnly());

        // If off the end of the heap, return the 'nul' item from the beginning.
        if (ulOffset >= m_cbSegSize)
            ulOffset = 0;

        // try hot data first
        if ( void const * data = GetHotData( ulOffset ) )
            return reinterpret_cast< BYTE * >( const_cast< void * >( data ) );
    
        METADATATRACKER_ONLY( MetaDataTracker::LogBlobAccess( m_pSegData, ulOffset ) );
        PVOID pVal = m_pSegData+ulOffset;

        //_ASSERTE(ulOffset < m_cbSegSize && "Attempt to access past end of heap.");
        METADATATRACKER_ONLY(pVal = MetaDataTracker::NoteAccess(pVal, -1));
        METADATATRACKER_ONLY(pVal = MetaDataTracker::LogHeapAccess(pVal, -1));

        return (BYTE*)pVal;
    }

//*****************************************************************************
// Get a pointer to an offset within the heap.  Inline for base segment,
//  helper for extension segments.
//*****************************************************************************
    virtual BYTE *GetData(ULONG ulOffset)
    {
        WRAPPER_CONTRACT;
        return (GetDataReadOnly(ulOffset));
    }

//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
	GUID *GetGuidi(							// Pointer to guid in pool.
		ULONG		iIndex);				// 0-based index of Guid in pool.

private:
    // hot pool data
    HotPoolDataHeader * m_hotDataHeader;
};

//
//
// StgBlobPoolReadOnly
//
// 
//*****************************************************************************
// This is the read only StgBlobPool class
//*****************************************************************************
class StgBlobPoolReadOnly : public StgPoolReadOnly
{
friend class CBlobPoolHash;

protected:

//*****************************************************************************
// Check whether a given offset is valid in the pool.
//*****************************************************************************
    virtual int IsValidOffset(ULONG ulOffset)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		if(ulOffset)
		{
			if((m_pSegData != m_zeros) && (ulOffset < m_cbSegSize))
			{
				ULONG ulSize = CPackedLen::GetLength(GetData(ulOffset), (void const **)NULL);
				if(ulOffset+ulSize < m_cbSegSize) return true;
			}
			return false;
		}
		else return true;
	}
};

//
//
// StgPool
//
//

//*****************************************************************************
// This base class provides common pool management code, such as allocation
// of dynamic memory.
//*****************************************************************************
class StgPool : public StgPoolReadOnly
{
friend class StgStringPool;
friend class StgBlobPool;
friend class RecordPool;
friend class CBlobPoolHash;

public:
	StgPool(ULONG ulGrowInc=512, ULONG ulAlignment=4) :
		m_State(eNormal),
		m_ulGrowInc(ulGrowInc),
		m_pCurSeg(this),
        m_cbCurSegOffset(0),
		m_bFree(true),
		m_bDirty(false),
		m_bReadOnly(false),
		m_ulAlignment(ulAlignment-1),
		m_cbStartOffsetOfEdit(0),
		m_fValidOffsetOfEdit(0)
	{ LEAF_CONTRACT; }

	virtual ~StgPool();

protected:
	template<class T> T Align(T val) {
	    LEAF_CONTRACT;

	    return (T)((((size_t)val) + m_ulAlignment) & ~m_ulAlignment);
	}

public:
//*****************************************************************************
// Init the pool for use.  This is called for the create empty case.
//*****************************************************************************
	virtual HRESULT InitNew( 				// Return code.
		ULONG		cbSize=0,				// Estimated size.
		ULONG		cItems=0);				// Estimated item count.

//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
	virtual HRESULT InitOnMem(				// Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize,					// Size of data.
		int			bReadOnly);				// true if append is forbidden.

//*****************************************************************************
// Init the pool from existing data.
//*****************************************************************************
	virtual HRESULT InitOnMemReadOnly(		// Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize);					// Size of data.

//*****************************************************************************
// Called when the pool must stop accessing memory passed to InitOnMem().
//*****************************************************************************
	virtual HRESULT TakeOwnershipOfInitMem();

//*****************************************************************************
// Clear out this pool.  Cannot use until you call InitNew.
//*****************************************************************************
	virtual void Uninit();

//*****************************************************************************
// Called to copy the pool to writable memory, reset the r/o bit.
//*****************************************************************************
	virtual HRESULT ConvertToRW();

//*****************************************************************************
// Turn hashing off or on.  Implemented as required in subclass.
//*****************************************************************************
	virtual HRESULT SetHash(int bHash);

//*****************************************************************************
// Allocate memory if we don't have any, or grow what we have.  If successful,
// then at least iRequired bytes will be allocated.
//*****************************************************************************
	bool Grow(								// true if successful.
		ULONG		iRequired);				// Min required bytes to allocate.

//*****************************************************************************
// Add a segment to the chain of segments.
//*****************************************************************************
	virtual HRESULT AddSegment(				// S_OK or error.
		const void	*pData,					// The data.
		ULONG		cbData,					// Size of the data.
		bool		bCopy);					// If true, make a copy of the data.

//*****************************************************************************
// Trim any empty final segment.
//*****************************************************************************
	void Trim();							//

//*****************************************************************************
// Return the size in bytes of the persistent version of this pool.  If
// PersistToStream were the next call, the amount of bytes written to pIStream
// has to be same as the return value from this function.
//*****************************************************************************
	virtual HRESULT GetSaveSize(			// Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		_ASSERTE(pcbSaveSize);
		// Size is offset of last seg + size of last seg.
        ULONG ulSize = m_pCurSeg->m_cbSegNext + m_cbCurSegOffset;
		// Align.
		ulSize = Align(ulSize);

		*pcbSaveSize = ulSize;
		return (S_OK);
	}

//*****************************************************************************
// Return the size in bytes of the edits contained in the persistent version of this pool.
//*****************************************************************************
	virtual HRESULT GetEditSaveSize(			// Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

        _ASSERTE(pcbSaveSize);
        ULONG ulSize = 0;

        if (HaveEdits())
        {
            // Size is offset of last seg + size of last seg.

            // An offset of zero in the pool will give us a zero length blob. The first
            // "real" user string is at offset 1. Wherever this delta gets applied, it will
            // already have this zero length blob. Let's make sure we don't sent it another one.

            _ASSERTE(*(GetData(0)) == 0);
            ULONG ulOffsetOfEdit = GetOffsetOfEdit();
            
            if (ulOffsetOfEdit == 0)
                ulOffsetOfEdit = 1;    
            
            ulSize = m_pCurSeg->m_cbSegNext + m_cbCurSegOffset - ulOffsetOfEdit;
        }

        // Align.
        ulSize = Align(ulSize);

        *pcbSaveSize = ulSize;
        return (S_OK);
        }




//*****************************************************************************
// The entire pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
	virtual HRESULT PersistToStream(		// Return code.
		IStream		*pIStream);				// The stream to write to.

//*****************************************************************************
// The hot data of the pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
    HRESULT PersistHotToStream(
        IStream * pIStream,             // The stream to write to.
        CorProfileData * profileData,   // Profiler data containing hot info
        MDPools poolId,                 // The id of the pool
        ULONG * savedSize );            // Returns size of the saved pool data

//*****************************************************************************
// A portion of the pool is written to the stream.  Must not be optimized.
//*****************************************************************************
	virtual HRESULT PersistPartialToStream(	// Return code.
		IStream		*pIStream,				// The stream to write to.
		ULONG		iOffset);				// Starting byte.

//*****************************************************************************
// Get the size of the data block obtained from the pool.
// Needed for generic persisting of data blocks.
// Override in concrete pool classes to return the correct size.
//*****************************************************************************
    virtual ULONG GetSizeOfData( void const * data )
    {
        LEAF_CONTRACT;
        return 0;
    }

//*****************************************************************************
// Return true if this pool is dirty.
//*****************************************************************************
	virtual int IsDirty()					// true if dirty.
	{LEAF_CONTRACT;  return (m_bDirty); }
	void SetDirty(int bDirty=true)
	{LEAF_CONTRACT;  m_bDirty = bDirty; }

//*****************************************************************************
// Return the size of the pool.
//*****************************************************************************
	virtual ULONG GetPoolSize()
	{LEAF_CONTRACT;  return m_pCurSeg->m_cbSegNext + m_cbCurSegOffset; }

//*****************************************************************************
// Indicate if heap is empty.
//*****************************************************************************
	virtual int IsEmpty()					// true if empty.
	{LEAF_CONTRACT;  return (m_pSegData == m_zeros); }

//*****************************************************************************
// How big is a cookie for this heap.
//*****************************************************************************
	virtual int OffsetSize()
	{
	    LEAF_CONTRACT;

        if (m_pCurSeg->m_cbSegNext + m_cbCurSegOffset < USHRT_MAX)
			return (sizeof(USHORT));
		else
			return (sizeof(ULONG));
	}

//*****************************************************************************
// true if the heap is read only.
//*****************************************************************************
	int IsReadOnly()
	{LEAF_CONTRACT;  return (m_bReadOnly == false); }

//*****************************************************************************
// Is the given cookie a valid offset, index, etc?
//*****************************************************************************
	virtual int IsValidCookie(ULONG ulCookie)
	{ WRAPPER_CONTRACT; return (IsValidOffset(ulCookie)); }

//*****************************************************************************
// Reorganization interface.
//*****************************************************************************
	// Prepare for pool re-organization.
	virtual HRESULT OrganizeBegin();
	// Mark an object as being live in the organized pool.
	virtual HRESULT OrganizeMark(ULONG ulOffset);
	// Organize, based on marked items.
	virtual HRESULT OrganizePool();
	// Remap a cookie from the in-memory state to the persisted state.
	virtual HRESULT OrganizeRemap(ULONG ulOld, ULONG *pulNew);
	// Done with regoranization.  Release any state.
	virtual HRESULT OrganizeEnd();

	enum {eNormal, eMarking, eOrganized} m_State;


//*****************************************************************************
// Get a pointer to an offset within the heap.  Inline for base segment,
//  helper for extension segments.
//*****************************************************************************
	FORCEINLINE BYTE *GetData(ULONG ulOffset)
	{ WRAPPER_CONTRACT; return ((ulOffset < m_cbSegNext) ? (m_pSegData+ulOffset) : GetData_i(ulOffset)); }

//*****************************************************************************
// Copies data from the pool into a buffer. It will correctly walk the different
// segments for the copy
//*****************************************************************************
HRESULT CopyData(ULONG nOffset, BYTE* pBuffer, ULONG cbBuffer, ULONG* cbWritten);

//*****************************************************************************
// Helpers for dump utilities.    
//*****************************************************************************
	HRESULT GetRawSize(			            // Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{
	    LEAF_CONTRACT;

		// Size is offset of last seg + size of last seg.
        *pcbSaveSize = m_pCurSeg->m_cbSegNext + m_cbCurSegOffset;
		return (S_OK);
	}
    
	virtual HRESULT GetNextItem(			// Return code.
        ULONG       ulItem,                 // Current item.
		ULONG		*pulNext)		        // Return offset of next pool item.
	{
	    LEAF_CONTRACT;

        // Must provide an implementation.
        return E_NOTIMPL;
	}

    BOOL HaveEdits() {LEAF_CONTRACT; return m_fValidOffsetOfEdit;}
    ULONG GetOffsetOfEdit() {LEAF_CONTRACT; return m_cbStartOffsetOfEdit;}
    void ResetOffsetOfEdit() {LEAF_CONTRACT; m_fValidOffsetOfEdit=FALSE;}

protected:

//*****************************************************************************
// Check whether a given offset is valid in the pool.
//*****************************************************************************
    virtual int IsValidOffset(ULONG ulOffset)
    { WRAPPER_CONTRACT; return ulOffset == 0 || (m_pSegData != m_zeros && ulOffset < GetNextOffset()); }

	// Following virtual because a) this header included outside the project, and
	//  non-virtual function call (in non-expanded inline function!!) generates
	//  an external def, which causes linkage errors.
	virtual BYTE *GetData_i(ULONG ulOffset);

	// Get pointer to next location to which to write.
	BYTE *GetNextLocation()
	{LEAF_CONTRACT;  return (m_pCurSeg->m_pSegData + m_pCurSeg->m_cbSegNext); }

	// Get pool-relative offset of next location to which to write.
	ULONG GetNextOffset()
	{LEAF_CONTRACT;  return (m_cbCurSegOffset + m_pCurSeg->m_cbSegNext); }

	// Get count of bytes available in tail segment of pool.
	ULONG GetCbSegAvailable()
	{LEAF_CONTRACT;  return (m_pCurSeg->m_cbSegSize - m_pCurSeg->m_cbSegNext); }

    // Allocate space from the segment.

    void SegAllocate(ULONG cb)
    {
        LEAF_CONTRACT;
        _ASSERTE(cb <= GetCbSegAvailable());

        if (!m_fValidOffsetOfEdit)
        {
            m_cbStartOffsetOfEdit = GetNextOffset();
            m_fValidOffsetOfEdit = TRUE;
        }

        m_pCurSeg->m_cbSegNext += cb;
    }// SegAllocate

       

	ULONG		m_ulGrowInc;				// How many bytes at a time.
	StgPoolSeg	*m_pCurSeg;					// Current seg for append -- end of chain.
    ULONG       m_cbCurSegOffset;           // Base offset of current seg.

	unsigned	m_bFree		: 1;			// True if we should free base data.
											//  Extension data is always freed.
	unsigned	m_bDirty	: 1;			// Dirty bit.
	unsigned	m_bReadOnly	: 1;			// True if we shouldn't append.

    size_t         m_ulAlignment;				// Alignment boundary.
    ULONG       m_cbStartOffsetOfEdit;         // Place in the pool where edits started
    BOOL         m_fValidOffsetOfEdit;          // Is the pool edit offset valid

};


//
//
// StgStringPool
//
//



//*****************************************************************************
// This string pool class collects user strings into a big consecutive heap.
// Internally it manages this data in a hash table at run time to help throw
// out duplicates.  The list of strings is kept in memory while adding, and
// finally flushed to a stream at the caller's request.
//*****************************************************************************
class StgStringPool : public StgPool
{
public:
	StgStringPool() :
		StgPool(DFT_STRING_HEAP_SIZE),
		m_Hash(this),
		m_bHash(true)
	{
        LEAF_CONTRACT;
        // force some code in debug.
		_ASSERTE(m_bHash);
	}

//*****************************************************************************
// Create a new, empty string pool.
//*****************************************************************************
	HRESULT InitNew( 						// Return code.
		ULONG		cbSize=0,				// Estimated size.
		ULONG		cItems=0);				// Estimated item count.

//*****************************************************************************
// Load a string heap from persisted memory.  If a copy of the data is made
// (so that it may be updated), then a new hash table is generated which can
// be used to elminate duplicates with new strings.
//*****************************************************************************
	HRESULT InitOnMem(						// Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize,					// Size of data.
		int			bReadOnly);				// true if append is forbidden.

//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
	void Uninit();

//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
	virtual HRESULT SetHash(int bHash);

//*****************************************************************************
// The string will be added to the pool.  The offset of the string in the pool
// is returned in *piOffset.  If the string is already in the pool, then the
// offset will be to the existing copy of the string.
// 
// The first version essentially adds a zero-terminated sequence of bytes
//  to the pool.  MBCS pairs will not be converted to the appropriate UTF8
//  sequence.  The second version converts from Unicode.
//*****************************************************************************
	HRESULT AddString(						// Return code.
		LPCSTR		szString,				// The string to add to pool.
		ULONG		*piOffset,				// Return offset of string here.
		int			iLength=-1);			// chars in string; -1 null terminated.

	HRESULT AddStringW(						// Return code.
		LPCWSTR		szString,				// The string to add to pool.
		ULONG		*piOffset,				// Return offset of string here.
		int			iLength=-1);			// chars in string; -1 null terminated.

//*****************************************************************************
// Look for the string and return its offset if found.
//*****************************************************************************
	HRESULT FindString(						// S_OK, S_FALSE.
		LPCSTR		szString,				// The string to find in pool.
		ULONG		*piOffset)				// Return offset of string here.
	{
        WRAPPER_CONTRACT;

		STRINGHASH	*pHash;					// Hash item for lookup.
        if ((pHash = m_Hash.Find(szString)) == 0)
			return (S_FALSE);
		*piOffset = pHash->iOffset;
		return (S_OK);
	}

//*****************************************************************************
// How many objects are there in the pool?  If the count is 0, you don't need
// to persist anything at all to disk.
//*****************************************************************************
	int Count()
	{
        WRAPPER_CONTRACT;
        _ASSERTE(m_bHash);
		return (m_Hash.Count()); }

//*****************************************************************************
// String heap is considered empty if the only thing it has is the initial
// empty string, or if after organization, there are no strings.
//*****************************************************************************
	int IsEmpty()						// true if empty.
    {
        WRAPPER_CONTRACT;

		if (m_State == eNormal)
			return (GetNextOffset() <= 1); 
		else
			return (m_cbOrganizedSize == 0);
	}

//*****************************************************************************
// Reorganization interface.
//*****************************************************************************
	// Prepare for pool re-organization.
	virtual HRESULT OrganizeBegin();
	// Mark an object as being live in the organized pool.
	virtual HRESULT OrganizeMark(ULONG ulOffset);
	// Organize, based on marked items.
	virtual HRESULT OrganizePool();
	// Remap a cookie from the in-memory state to the persisted state.
	virtual HRESULT OrganizeRemap(ULONG ulOld, ULONG *pulNew);
	// Done with regoranization.  Release any state.
	virtual HRESULT OrganizeEnd();

//*****************************************************************************
// How big is a cookie for this heap.
//*****************************************************************************
	int OffsetSize()
	{
        WRAPPER_CONTRACT;

		ULONG		ulOffset;

		// Pick an offset based on whether we've been organized.
		if (m_State == eOrganized)
			ulOffset = m_cbOrganizedOffset;
		else
			ulOffset = GetNextOffset();

        if (ulOffset< USHRT_MAX)
			return (sizeof(USHORT));
		else
			return (sizeof(ULONG));
	}

//*****************************************************************************
// Return the size in bytes of the persistent version of this pool.  If
// PersistToStream were the next call, the amount of bytes written to pIStream
// has to be same as the return value from this function.
//*****************************************************************************
	virtual HRESULT GetSaveSize(			// Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{
        LEAF_CONTRACT;

		ULONG		ulSize;					// The size.
		_ASSERTE(pcbSaveSize);

		if (m_State == eOrganized)
			ulSize = m_cbOrganizedSize;
		else
		{	// Size is offset of last seg + size of last seg.
			ulSize = m_pCurSeg->m_cbSegNext + m_cbCurSegOffset;
		}
		// Align.
		ulSize = ALIGN4BYTE(ulSize);

		*pcbSaveSize = ulSize;
		return (S_OK);
	}

//*****************************************************************************
// The entire string pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
	virtual HRESULT PersistToStream(		// Return code.
		IStream		*pIStream);				// The stream to write to.

//*****************************************************************************
// Get the size of the string obtained from the pool.
// Needed for generic persisting of data blocks.
//*****************************************************************************
    virtual ULONG GetSizeOfData( void const * data )
    {
        LEAF_CONTRACT;
        return ULONG( strlen( reinterpret_cast< LPCSTR >( data ) ) + 1 ); // using strlen since the string is UTF8
    }

	
//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
    virtual HRESULT GetNextItem(			// Return code.
        ULONG       ulItem,                 // Current item.
		ULONG		*pulNext);		        // Return offset of next pool item.

private:
	HRESULT RehashStrings();

private:
	CStringPoolHash m_Hash;					// Hash table for lookups.
	int			m_bHash;					// true to keep hash table.
	ULONG		m_cbOrganizedSize;			// Size of the optimized pool.
	ULONG		m_cbOrganizedOffset;		// Highest offset.

    //*************************************************************************
	// Private classes used in optimization.
    //*************************************************************************
	struct StgStringRemap
	{
		ULONG	ulOldOffset;
		ULONG	ulNewOffset;
		ULONG	cbString;
	};

	CDynArray<StgStringRemap> m_Remap;		// For use in reorganization.
	ULONGARRAY	m_RemapIndex;				// For use in reorganization.

	// Sort by reversed strings.
	friend class SortReversedName;
	class BinarySearch : public CBinarySearch<StgStringRemap>
	{
	public:
		BinarySearch(StgStringRemap *pBase, int iCount) : CBinarySearch<StgStringRemap>(pBase, iCount) 
        { 
            LEAF_CONTRACT; 
        }

		int Compare(StgStringRemap const *pFirst, StgStringRemap const *pSecond)
		{
            LEAF_CONTRACT;

			if (pFirst->ulOldOffset < pSecond->ulOldOffset)
				return -1;
			if (pFirst->ulOldOffset > pSecond->ulOldOffset)
				return 1;
			return 0;
		}
	};
};

class SortReversedName : public CQuickSort<ULONG>
{
public:
	SortReversedName(ULONG *pBase, int iCount, StgStringPool &Pool) 
		:  CQuickSort<ULONG>(pBase, iCount),
		m_Pool(Pool)
	{ LEAF_CONTRACT; }
	
	int Compare(ULONG *pUL1, ULONG *pUL2)
	{
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		StgStringPool::StgStringRemap *pRM1 = m_Pool.m_Remap.Get(*pUL1);
		StgStringPool::StgStringRemap *pRM2 = m_Pool.m_Remap.Get(*pUL2);
		LPCSTR p1 = m_Pool.GetString(pRM1->ulOldOffset) + pRM1->cbString - 1;
		LPCSTR p2 = m_Pool.GetString(pRM2->ulOldOffset) + pRM2->cbString - 1;
		while (*p1 == *p2 && *p1)
			--p1, --p2;
		if (*p1 < *p2)
			return -1;
		if (*p1 > *p2)
			return 1;
		return 0;
	}
	
	StgStringPool	&m_Pool;
};


//
//
// StgGuidPool
//
//



//*****************************************************************************
// This Guid pool class collects user Guids into a big consecutive heap.
// Internally it manages this data in a hash table at run time to help throw
// out duplicates.  The list of Guids is kept in memory while adding, and
// finally flushed to a stream at the caller's request.
//*****************************************************************************
class StgGuidPool : public StgPool
{
public:
	StgGuidPool() :
		StgPool(DFT_GUID_HEAP_SIZE),
		m_Hash(this),
		m_bHash(true)
	{ LEAF_CONTRACT; }

//*****************************************************************************
// Init the pool for use.  This is called for the create empty case.
//*****************************************************************************
	HRESULT InitNew( 						// Return code.
		ULONG		cbSize=0,				// Estimated size.
		ULONG		cItems=0);				// Estimated item count.

//*****************************************************************************
// Load a Guid heap from persisted memory.  If a copy of the data is made
// (so that it may be updated), then a new hash table is generated which can
// be used to elminate duplicates with new Guids.
//*****************************************************************************
    HRESULT InitOnMem(                      // Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize,					// Size of data.
        int         bReadOnly);             // true if append is forbidden.

//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
	void Uninit();

//*****************************************************************************
// Add a segment to the chain of segments.
//*****************************************************************************
	virtual HRESULT AddSegment(				// S_OK or error.
		const void	*pData,					// The data.
		ULONG		cbData,					// Size of the data.
		bool		bCopy);					// If true, make a copy of the data.

//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
	virtual HRESULT SetHash(int bHash);

//*****************************************************************************
// The Guid will be added to the pool.  The index of the Guid in the pool
// is returned in *piIndex.  If the Guid is already in the pool, then the
// index will be to the existing copy of the Guid.
//*****************************************************************************
	HRESULT AddGuid(						// Return code.
		REFGUID		guid,					// The Guid to add to pool.
		ULONG		*piIndex);				// Return index of Guid here.


//*****************************************************************************
// Return a pointer to a Guid given an index previously handed out by
// AddGuid or FindGuid.
//*****************************************************************************
	virtual GUID *GetGuid(					// Pointer to guid in pool.
		ULONG		iIndex);				// 1-based index of Guid in pool.

//*****************************************************************************
// Copy a GUID into the caller's buffer.
//*****************************************************************************
	HRESULT GetGuid(						// Return code.
		ULONG		iIndex,					// 1-based index of Guid in pool.
		GUID		*pGuid)					// Output buffer for Guid.
	{
        WRAPPER_CONTRACT;

		*pGuid = *GetGuid(iIndex);
		SwapGuid(pGuid);
		return (S_OK);
	}

//*****************************************************************************
// Get the size of the GUID obtained from the pool.
// Needed for generic persisting of data blocks.
//*****************************************************************************
    virtual ULONG GetSizeOfData( void const * data )
    {
        LEAF_CONTRACT;
        return sizeof( GUID );
    }

//*****************************************************************************
// How many objects are there in the pool?  If the count is 0, you don't need
// to persist anything at all to disk.
//*****************************************************************************
	int Count()
	{
      WRAPPER_CONTRACT;
      _ASSERTE(m_bHash);
		return (m_Hash.Count()); }

//*****************************************************************************
// Indicate if heap is empty.  This has to be based on the size of the data
// we are keeping.  If you open in r/o mode on memory, there is no hash
// table.  
//*****************************************************************************
	virtual int IsEmpty()					// true if empty.
	{
        WRAPPER_CONTRACT;

		if (m_State == eNormal)
			return (GetNextOffset() == 0);
		else
			return (m_cbOrganizedSize == 0);
	}

//*****************************************************************************
// Is the index valid for the GUID?
//*****************************************************************************
    virtual int IsValidCookie(ULONG ulCookie)
	{ WRAPPER_CONTRACT; return (ulCookie == 0 || IsValidOffset((ulCookie-1) * sizeof(GUID))); }

//*****************************************************************************
// Return the size of the heap.
//*****************************************************************************
    ULONG GetNextIndex()
    { LEAF_CONTRACT; return (GetNextOffset() / sizeof(GUID)); }

//*****************************************************************************
// How big is an offset in this heap.
//*****************************************************************************
	int OffsetSize()
	{
        WRAPPER_CONTRACT;

		ULONG cbSaveSize;
		GetSaveSize(&cbSaveSize);
        ULONG iIndex = cbSaveSize / sizeof(GUID);
		if (iIndex < 0xffff)
			return (sizeof(short));
		else
			return (sizeof(long));
	}

//*****************************************************************************
// Reorganization interface.
//*****************************************************************************
	// Prepare for pool re-organization.
	virtual HRESULT OrganizeBegin();
	// Mark an object as being live in the organized pool.
	virtual HRESULT OrganizeMark(ULONG ulOffset);
	// Organize, based on marked items.
	virtual HRESULT OrganizePool();
	// Remap a cookie from the in-memory state to the persisted state.
	virtual HRESULT OrganizeRemap(ULONG ulOld, ULONG *pulNew);
	// Done with regoranization.  Release any state.
	virtual HRESULT OrganizeEnd();

//*****************************************************************************
// Return the size in bytes of the persistent version of this pool.  If
// PersistToStream were the next call, the amount of bytes written to pIStream
// has to be same as the return value from this function.
//*****************************************************************************
	virtual HRESULT GetSaveSize(			// Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{

        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		ULONG		ulSize;					// The size.

		_ASSERTE(pcbSaveSize);

		if (m_State == eNormal)
			// Size is offset of last seg + size of last seg.
		    ulSize = m_pCurSeg->m_cbSegNext + m_cbCurSegOffset;
		else
			ulSize = m_cbOrganizedSize;

		// Should be aligned.
		_ASSERTE(ulSize == ALIGN4BYTE(ulSize));

		*pcbSaveSize = ulSize;
		return (S_OK);
	}

//*****************************************************************************
// The entire string pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
	virtual HRESULT PersistToStream(		// Return code.
		IStream		*pIStream);				// The stream to write to.


//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
    virtual HRESULT GetNextItem(			// Return code.
        ULONG       ulItem,                 // Current item.
		ULONG		*pulNext);		        // Return offset of next pool item.


private:

	HRESULT RehashGuids();


private:
	ULONGARRAY	m_Remap;					// For remaps.
	ULONG		m_cbOrganizedSize;			// Size after organization.
	CGuidPoolHash m_Hash;					// Hash table for lookups.
	int			m_bHash;					// true to keep hash table.
};



//
//
// StgBlobPool
//
//


//*****************************************************************************
// Just like the string pool, this pool manages a list of items, throws out
// duplicates using a hash table, and can be persisted to a stream.  The only
// difference is that instead of saving null terminated strings, this code
// manages binary values of up to 64K in size.  Any data you have larger than
// this should be stored someplace else with a pointer in the record to the
// external source.
//*****************************************************************************
class StgBlobPool : public StgPool
{
public:
	StgBlobPool(ULONG ulGrowInc=DFT_BLOB_HEAP_SIZE) :
        StgPool(ulGrowInc),
        m_Hash(this),
		m_bAlign(false)
	{ LEAF_CONTRACT; }

//*****************************************************************************
// Init the pool for use.  This is called for the create empty case.
//*****************************************************************************
	HRESULT InitNew( 						// Return code.
		ULONG		cbSize=0,				// Estimated size.
		ULONG		cItems=0,               // Estimated item count.
		BOOL        fAddEmptryItem=TRUE);        // Should we add an empty item at offset 0

//*****************************************************************************
// Init the blob pool for use.  This is called for both create and read case.
// If there is existing data and bCopyData is true, then the data is rehashed
// to eliminate dupes in future adds.
//*****************************************************************************
    HRESULT InitOnMem(                      // Return code.
		void		*pData,					// Predefined data.
		ULONG		iSize,					// Size of data.
        int         bReadOnly);             // true if append is forbidden.

//*****************************************************************************
// Clears the hash table then calls the base class.
//*****************************************************************************
	void Uninit();

//*****************************************************************************
// The blob will be added to the pool.  The offset of the blob in the pool
// is returned in *piOffset.  If the blob is already in the pool, then the
// offset will be to the existing copy of the blob.
//*****************************************************************************
	HRESULT AddBlob(						// Return code.
		ULONG		iSize,					// Size of data item.
		const void	*pData,					// The data.
		ULONG		*piOffset);				// Return offset of blob here.

//*****************************************************************************
// Return a pointer to a null terminated blob given an offset previously
// handed out by Addblob or Findblob.
//*****************************************************************************
	virtual void *GetBlob(					// Pointer to blob's bytes.
		ULONG		iOffset,				// Offset of blob in pool.
		ULONG		*piSize);				// Return size of blob.

	virtual void *GetBlobNext(				// Pointer to blob's bytes.
		ULONG		iOffset,				// Offset of blob in pool.
		ULONG		*piSize,				// Return size of blob.
		ULONG		*piNext);				// Return offset of next blob.



//*****************************************************************************
// Turn hashing off or on.  If you turn hashing on, then any existing data is
// thrown away and all data is rehashed during this call.
//*****************************************************************************
	virtual HRESULT SetHash(int bHash);

//*****************************************************************************
// Get the size of the blob obtained from the pool.
// Needed for generic persisting of data blocks.
//*****************************************************************************
    virtual ULONG GetSizeOfData( void const * data )
    {
        WRAPPER_CONTRACT;
        
        void const * blobdata = 0;
        ULONG blobsize = CPackedLen::GetLength( data, & blobdata ); // the size is encoded at the beginning of the block
        return blobsize + static_cast< ULONG >( reinterpret_cast< BYTE const * >( blobdata ) - reinterpret_cast< BYTE const * >( data ) );
    }

//*****************************************************************************
// How many objects are there in the pool?  If the count is 0, you don't need
// to persist anything at all to disk.
//*****************************************************************************
	int Count()
	{ WRAPPER_CONTRACT; return (m_Hash.Count()); }

//*****************************************************************************
// String heap is considered empty if the only thing it has is the initial
// empty string, or if after organization, there are no strings.
//*****************************************************************************
	virtual int IsEmpty()					// true if empty.
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		if (m_State == eNormal)
			return (GetNextOffset() <= 1); 
		else
			return (m_Remap.Count() == 0);
	}

//*****************************************************************************
// Reorganization interface.
//*****************************************************************************
	// Prepare for pool re-organization.
	virtual HRESULT OrganizeBegin();
	// Mark an object as being live in the organized pool.
	virtual HRESULT OrganizeMark(ULONG ulOffset);
	// Organize, based on marked items.
	virtual HRESULT OrganizePool();
	// Remap a cookie from the in-memory state to the persisted state.
	virtual HRESULT OrganizeRemap(ULONG ulOld, ULONG *pulNew);
	// Done with regoranization.  Release any state.
	virtual HRESULT OrganizeEnd();

//*****************************************************************************
// How big is a cookie for this heap.
//*****************************************************************************
	int OffsetSize()
	{
        WRAPPER_CONTRACT;

		ULONG		ulOffset;

		// Pick an offset based on whether we've been organized.
		if (m_State == eOrganized)
			ulOffset = m_cbOrganizedOffset;
		else
			ulOffset = GetNextOffset();

        if (ulOffset< USHRT_MAX)
			return (sizeof(USHORT));
		else
			return (sizeof(ULONG));
	}

//*****************************************************************************
// Return the size in bytes of the persistent version of this pool.  If
// PersistToStream were the next call, the amount of bytes written to pIStream
// has to be same as the return value from this function.
//*****************************************************************************
	virtual HRESULT GetSaveSize(			// Return code.
		ULONG		*pcbSaveSize)			// Return save size of this pool.
	{
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		_ASSERTE(pcbSaveSize);

		if (m_State == eOrganized)
		{
			*pcbSaveSize = m_cbOrganizedSize;
			return (S_OK);
		}

		return (StgPool::GetSaveSize(pcbSaveSize));
	}

//*****************************************************************************
// The entire blob pool is written to the given stream. The stream is aligned
// to a 4 byte boundary.
//*****************************************************************************
	virtual HRESULT PersistToStream(		// Return code.
		IStream		*pIStream);				// The stream to write to.

    //*************************************************************************
	// Private classes used in optimization.
    //*************************************************************************
	struct StgBlobRemap
	{
		ULONG	ulOldOffset;
		int		iNewOffset;
	};
	class BinarySearch : public CBinarySearch<StgBlobRemap>
	{
	public:
		BinarySearch(StgBlobRemap *pBase, int iCount) : CBinarySearch<StgBlobRemap>(pBase, iCount) { LEAF_CONTRACT; }

		int Compare(StgBlobRemap const *pFirst, StgBlobRemap const *pSecond)
		{
            LEAF_CONTRACT;
			if (pFirst->ulOldOffset < pSecond->ulOldOffset)
				return -1;
			if (pFirst->ulOldOffset > pSecond->ulOldOffset)
				return 1;
			return 0;
		}
	};

	const void *GetBuffer() {LEAF_CONTRACT; return (m_pSegData);}

	int IsAligned() {LEAF_CONTRACT;  return (m_bAlign); };
	void SetAligned(int bAlign) { LEAF_CONTRACT; m_bAlign = bAlign; };
    

//*****************************************************************************
// Helper gets the next item, given an input item.    
//*****************************************************************************
    virtual HRESULT GetNextItem(			// Return code.
        ULONG       ulItem,                 // Current item.
		ULONG		*pulNext);		        // Return offset of next pool item.

protected:

//*****************************************************************************
// Check whether a given offset is valid in the pool.
//*****************************************************************************
    virtual int IsValidOffset(ULONG ulOffset)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

		if(ulOffset)
		{
			if(m_pSegData != m_zeros && (ulOffset < GetNextOffset()))
			{
				ULONG ulSize = CPackedLen::GetLength(GetData(ulOffset), (void const **)NULL);
				if(ulOffset+ulSize < GetNextOffset()) return true;
			}
			return false;
		}
		else return true;
	}


private:
	HRESULT RehashBlobs();

	CBlobPoolHash m_Hash;					// Hash table for lookups.
	CDynArray<StgBlobRemap> m_Remap;		// For use in reorganization.
	ULONG		m_cbOrganizedSize;			// Size of the optimized pool.
	ULONG		m_cbOrganizedOffset;		// Highest offset.
	unsigned	m_bAlign : 1;				// if blob data should be aligned on DWORDs
};

#ifdef _MSC_VER
#pragma warning (default : 4355)
#endif

class CInMemoryStream : public IStream
{
public:
    CInMemoryStream() :
        m_pMem(0),
        m_cbSize(0),
        m_cbCurrent(0),
        m_cRef(1),
        m_noHacks(false),
        m_dataCopy(NULL)
    { LEAF_CONTRACT; }

    void InitNew(
        void        *pMem,
        ULONG       cbSize)
    {
        LEAF_CONTRACT;

        m_pMem = pMem;
        m_cbSize = cbSize;
        m_cbCurrent = 0;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        LEAF_CONTRACT;
        return InterlockedIncrement(&m_cRef);
    }


    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, PVOID *ppOut);

    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);

    HRESULT STDMETHODCALLTYPE Write(const void  *pv, ULONG cb, ULONG *pcbWritten);

    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream     *pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead,
        ULARGE_INTEGER *pcbWritten);

    HRESULT STDMETHODCALLTYPE Commit(
        DWORD       grfCommitFlags)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE Revert()
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE Stat(
        STATSTG     *pstatstg,
        DWORD       grfStatFlag)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        pstatstg->cbSize.QuadPart = m_cbSize;
        return (S_OK);
    }

    HRESULT STDMETHODCALLTYPE Clone(
        IStream     **ppstm)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FAULT; //E_OUTOFMEMORY;

        return (BadError(E_NOTIMPL));
    }

    static HRESULT CreateStreamOnMemory(           // Return code.
                                 void        *pMem,                  // Memory to create stream on.
                                 ULONG       cbSize,                 // Size of data.
                                 IStream     **ppIStream,			// Return stream object here.
								 BOOL		fDeleteMemoryOnRelease = FALSE
								 );            

    static HRESULT CreateStreamOnMemoryNoHacks(
                                 void        *pMem,
                                 ULONG       cbSize,
                                 IStream     **ppIStream);

    static HRESULT CreateStreamOnMemoryCopy(
                                 void        *pMem,
                                 ULONG       cbSize,
                                 IStream     **ppIStream);

private:
    void        *m_pMem;                // Memory for the read.
    ULONG       m_cbSize;               // Size of the memory.
    ULONG       m_cbCurrent;            // Current offset.
    LONG        m_cRef;                 // Ref count.
    bool        m_noHacks;              // Don't use any hacks.
    BYTE       *m_dataCopy;             // Optional copy of the data.
};

//*****************************************************************************
// CGrowableStream is a simple IStream implementation that grows as
// its written to. All the memory is contigious, so read access is
// fast. A grow does a realloc, so be aware of that if you're going to
// use this.
//*****************************************************************************
class CGrowableStream : public IStream
{
public:
    CGrowableStream();
	~CGrowableStream();

    void *GetBuffer(void)
    {
        LEAF_CONTRACT;
        return (void*)m_swBuffer;
    }

private:
    char *  m_swBuffer;
    DWORD   m_dwBufferSize;
    DWORD   m_dwBufferIndex;
    LONG    m_cRef;

    // IStream methods
public:
    ULONG STDMETHODCALLTYPE AddRef() {
        LEAF_CONTRACT;
        return InterlockedIncrement(&m_cRef);
    }


    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, PVOID *ppOut);

    STDMETHOD(Read)(
         void * pv,
         ULONG cb,
         ULONG * pcbRead);

    STDMETHOD(Write)(
         const void * pv,
         ULONG cb,
         ULONG * pcbWritten);

    STDMETHOD(Seek)(
         LARGE_INTEGER dlibMove,
         DWORD dwOrigin,
         ULARGE_INTEGER * plibNewPosition);

    STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);

    STDMETHOD(CopyTo)(
         IStream * pstm,
         ULARGE_INTEGER cb,
         ULARGE_INTEGER * pcbRead,
         ULARGE_INTEGER * pcbWritten) { STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT; return E_NOTIMPL; }

    STDMETHOD(Commit)(
         DWORD grfCommitFlags) { STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT; return NOERROR; }

    STDMETHOD(Revert)( void) {STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT;  return E_NOTIMPL; }

    STDMETHOD(LockRegion)(
         ULARGE_INTEGER libOffset,
         ULARGE_INTEGER cb,
         DWORD dwLockType) { STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT; return E_NOTIMPL; }

    STDMETHOD(UnlockRegion)(
         ULARGE_INTEGER libOffset,
         ULARGE_INTEGER cb,
         DWORD dwLockType) {STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT;   return E_NOTIMPL; }

    STDMETHOD(Stat)(
         STATSTG * pstatstg,
         DWORD grfStatFlag);

    STDMETHOD(Clone)(
         IStream ** ppstm) { STATIC_CONTRACT_NOTHROW; STATIC_CONTRACT_FAULT; return E_NOTIMPL; }

}; // Class CGrowableStream

#endif // __StgPool_h__
