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
// MEMPOOL.CPP
////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "dbgout.h"
#include "mempool.h"

////////////////////////////////////////////////////////////////////////////////
// CMemPool::~CMemPool

CMemPool::~CMemPool ()
{
    while (m_pCurPool != NULL)
    {
        CPool   *pTmp = m_pCurPool->m_pPrev;
        free (m_pCurPool);
        m_pCurPool = pTmp;
    }

    DeleteCriticalSection (&m_cs);
}

////////////////////////////////////////////////////////////////////////////////
// CMemPool::Allocate

void *CMemPool::Allocate ()
{
    EnterCriticalSection (&m_cs);

    if (m_pFreeList != NULL)
    {
        // use the free blocks first
        void    *pMem = m_pFreeList;
        m_pFreeList = m_pFreeList->m_pPrev;

        LeaveCriticalSection (&m_cs);
        return pMem;
    }

    if (m_pCurPool == NULL || m_pCurPool->m_iBlocksLeft == 0)
    {
        CPool   *pPool = (CPool *)malloc (sizeof (CPool) + (m_iBlocksInPool * m_iBlockSize));
        if (pPool == NULL)
        {
            LeaveCriticalSection (&m_cs);
            return NULL;
        }

        pPool->m_pPrev = m_pCurPool;
        pPool->m_iBlocksLeft = m_iBlocksInPool;
        pPool->m_pNextBlock = (BYTE *)(pPool + 1);
        m_pCurPool = pPool;
    }

    void    *pMem = m_pCurPool->m_pNextBlock;
    m_pCurPool->m_pNextBlock += m_iBlockSize;
    m_pCurPool->m_iBlocksLeft--;

    LeaveCriticalSection (&m_cs);
    return pMem;
}

////////////////////////////////////////////////////////////////////////////////
// CMemPool::Free

void CMemPool::Free (void *p)
{
    EnterCriticalSection (&m_cs);

    // Simply place the block on the free list
    CFree   *pBlock = (CFree *)p;
    pBlock->m_pPrev = m_pFreeList;
    m_pFreeList = pBlock;

    LeaveCriticalSection (&m_cs);
}


