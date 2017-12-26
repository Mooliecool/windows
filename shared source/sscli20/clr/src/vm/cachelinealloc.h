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
//---------------------------------------------------------------------------
// CCacheLineAllocator
//
// @doc
// @module	cachelineAlloc.h
//
//		This file defines the CacheLine Allocator class.
//
// @comm
//
//    
// <nl> Definitions.:
// <nl>	Class Name						Header file
// <nl>	---------------------------		---------------
// <nl>	<c CCacheLineAllocator>			BAlloc.h
//
// <nl><nl>
//  Notes:
//		The CacheLineAllocator maintains a pool of free CacheLines
//		
//		The CacheLine Allocator provides static member functions 
//		GetCacheLine and FreeCacheLine,
//		
// <nl><nl>
//---------------------------------------------------------------------------
#ifndef _H_CACHELINE_ALLOCATOR_
#define _H_CACHELINE_ALLOCATOR_

#include "list.h"

#include <pshpack1.h>

class CacheLine
{
public:
    enum
    {
        numEntries       = 15,
        numValidBytes    = numEntries * sizeof(void *)
    };

    // store next pointer and the entries
    SLink   m_link;
    union
    {
        void*   m_pAddr[numEntries];
        BYTE    m_xxx[numValidBytes];
    };

    // init
    void Init32()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        // initialize cacheline
        memset(&m_link,0,32); 
    }

    void Init64()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        // initialize cacheline
        memset(&m_link,0,64); 
    }

    CacheLine()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        // initialize cacheline
        memset(&m_link,0,sizeof(CacheLine)); 
    }
};
#include <poppack.h>

typedef CacheLine* LPCacheLine;

/////////////////////////////////////////////////////////
//		class CCacheLineAllocator
//		Handles Allocation/DeAllocation of cache lines
//		used for hash table overflow buckets
///////////////////////////////////////////////////////
class CCacheLineAllocator 
{
    typedef SList<CacheLine, offsetof(CacheLine,m_link), true> REGISTRYLIST;
    typedef SList<CacheLine, offsetof(CacheLine,m_link), true> FREELIST32;
    typedef SList<CacheLine, offsetof(CacheLine,m_link), true> FREELIST64;

public:

    //constructor
    CCacheLineAllocator ();
    //destructor
    ~CCacheLineAllocator ();
   
    // free cacheline blocks
    FREELIST32         m_freeList32; //32 byte 
    FREELIST64         m_freeList64; //64 byte

    // registry for virtual free
    REGISTRYLIST     m_registryList;
    
    void *VAlloc(ULONG cbSize);

    void VFree(void* pv);

	// GetCacheLine, 
	void *	GetCacheLine32();
    
    // GetCacheLine, 
	void *	GetCacheLine64();

	// FreeCacheLine, 
	void FreeCacheLine32(void *pCacheLine);

	// FreeCacheLine, 
	void FreeCacheLine64(void *pCacheLine);

};
#endif
