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
////////////////////////////////////////////////////////////////////////////////
// MEMPOOL.H
////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

////////////////////////////////////////////////////////////////////////////////
// CMemPool

class CMemPool
{
private:

    enum
    {
        DEF_BLOCKS_IN_POOL  = 256
    };

    ////////////////////////////////////////////////////////////////////////////
    // CPool

    class CPool
    {
    public:
        CPool       *m_pPrev;       // Previous pool
        long        m_iBlocksLeft;  // Number of blocks this pool has left to allocate (first time)
        BYTE        *m_pNextBlock;  // Next available block in the pool (first time)
    };

    ////////////////////////////////////////////////////////////////////////////
    // CFree

    class CFree
    {
    public:
        CFree       *m_pPrev;       // Previous free block
    };

    CRITICAL_SECTION    m_cs;

    long    m_iBlockSize;       // Size of blocks
    long    m_iBlocksInPool;    // Number of blocks a pool will hold
    CPool   *m_pCurPool;        // Topmost pool
    CFree   *m_pFreeList;       // Free block chain

public:
    CMemPool (long iBlockSize, long iBlocksInPool = DEF_BLOCKS_IN_POOL) :
        m_iBlockSize(iBlockSize),
        m_iBlocksInPool(iBlocksInPool),
        m_pCurPool(NULL),
        m_pFreeList(NULL)
    {
        ASSERT (iBlockSize >= (long)sizeof (CFree *));
        InitializeCriticalSection (&m_cs);
    }
    ~CMemPool ();

    void    *Allocate ();
    void    Free (void *p);
    long    BlockSize () { return m_iBlockSize; }
};

////////////////////////////////////////////////////////////////////////////////
// HELPFUL MACROS:  If you have a class type that you want allocated from a
// memory pool, use these macros:
//
//  DECLARE_MEMORY_POOL()       Put this in your class definition to declare an
//                              overloaded new/delete pair and a pool for your
//                              class.  You MUST UNDEFINE 'new' before this line,
//                              and #define it back to 'vs_new' afterward!
//
//  DEFINE_MEMORY_POOL(class)   Put this in a module to define and construct the
//                              memory pool for the class
////////////////////////////////////////////////////////////////////////////////

#define DECLARE_MEMORY_POOL()\
    static  CMemPool    _m_alloc; \
    DECLARE_CLASS_NEW(dwSize) { ASSERT (dwSize == (UINT)_m_alloc.BlockSize()); return _m_alloc.Allocate (); } \
    void    operator delete (PVOID pv) { _m_alloc.Free (pv); }
	
#define DEFINE_MEMORY_POOL(c) CMemPool  c::_m_alloc(sizeof(c));



#endif // __MEMPOOL_H__
