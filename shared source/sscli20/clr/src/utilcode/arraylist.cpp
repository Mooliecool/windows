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
#include "stdafx.h"

#include "arraylist.h"
#include "utilcode.h"

//
// ArrayList is a simple class which is used to contain a growable
// list of pointers, stored in chunks.  Modification is by appending
// only currently.  Access is by index (efficient if the number of
// elements stays small) and iteration (efficient in all cases).
//
// An important property of an ArrayList is that the list remains
// coherent while it is being modified (appended to). This means that readers
// never need to lock when accessing it. (Locking is necessary among multiple
// writers, however.)
//

void ArrayListBase::Clear()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    ArrayListBlock *block = m_firstBlock.m_next;
    while (block != NULL)
    {
        ArrayListBlock *next = block->m_next;
        delete [] block;
        block = next;
    }
    m_firstBlock.m_next = 0;
    m_count = 0;
}

void **ArrayListBase::GetPtr(DWORD index)
{
	
	STATIC_CONTRACT_NOTHROW;
	STATIC_CONTRACT_FORBID_FAULT;
	STATIC_CONTRACT_SO_TOLERANT;

    _ASSERTE(index < m_count);

    ArrayListBlock *b = (ArrayListBlock*)&m_firstBlock;

    while (index >= b->m_blockSize)
    {
        PREFIX_ASSUME(b->m_next != NULL);
        index -= b->m_blockSize;
        b = b->m_next;
    }

    return b->m_array + index;
}

#ifndef DACCESS_COMPILE

HRESULT ArrayListBase::Append(void *element)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    ArrayListBlock *b = (ArrayListBlock*)&m_firstBlock;
    DWORD           count = m_count;

    while (count >= b->m_blockSize)
    {
        count -= b->m_blockSize;

        if (b->m_next == NULL)
        {
            _ASSERTE(count == 0);

            DWORD nextSize = b->m_blockSize * 2;

            ArrayListBlock *bNew = (ArrayListBlock *)
              new (nothrow) BYTE [sizeof(ArrayListBlock) + nextSize * sizeof(void*)];

            if (bNew == NULL)
                return E_OUTOFMEMORY;

            bNew->m_next = NULL;
            bNew->m_blockSize = nextSize;

            b->m_next = bNew;
        }

        b = b->m_next;
    }

    b->m_array[count] = element;

    m_count++;

    return S_OK;
}

#endif // #ifndef DACCESS_COMPILE

DWORD ArrayListBase::FindElement(DWORD start, void *element)
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    DWORD index = start;

    _ASSERTE(index <= m_count);

    ArrayListBlock *b = (ArrayListBlock*)&m_firstBlock;

    //
    // Skip to the block containing start.
    // index should be the index of start in the block.
    //

    while (b != NULL && index >= b->m_blockSize)
    {
        index -= b->m_blockSize;
        b = b->m_next;
    }

    //
    // Adjust start to be the index of the start of the block
    //

    start -= index;

    //
    // Compute max number of entries from the start of the block
    //

    DWORD max = m_count - start;

    while (b != NULL)
    {
        //
        // Compute end of search in this block - either end of the block
        // or end of the array
        //

        DWORD blockMax;
        if (max < b->m_blockSize)
            blockMax = max;
        else
            blockMax = b->m_blockSize;

        //
        // Scan for element, until the end.
        //

        while (index < blockMax)
        {
            if (b->m_array[index] == element)
                return start + index;
            index++;
        }

        //
        // Otherwise, increment block start index, decrement max count,
        // reset index, and go to the next block (if any)
        //

        start += b->m_blockSize;
        max -= b->m_blockSize;
        index = 0;
        b = b->m_next;
    }

    return (DWORD) NOT_FOUND;
}

BOOL ArrayListBase::Iterator::Next()
{
    LEAF_CONTRACT;

    ++m_index;

    if (m_index >= m_remaining)
        return FALSE;

    if (m_index >= m_block->m_blockSize)
    {
        m_remaining -= m_block->m_blockSize;
        m_index -= m_block->m_blockSize;
        m_total += m_block->m_blockSize;
        m_block = m_block->m_next;
    }

    return TRUE;
}

#ifdef DACCESS_COMPILE

void
ArrayListBase::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // Assume that 'this' is enumerated, either explicitly
    // or because this class is embedded in another.

    PTR_ArrayListBlock block = m_firstBlock.m_next;
    while (block.IsValid())
    {
        block.EnumMem();
        block = block->m_next;
    }
}

#endif // #ifdef DACCESS_COMPILE


void StructArrayListBase::Destruct (FreeProc *pfnFree)
{
    WRAPPER_CONTRACT;
    
    StructArrayListEntryBase *pList = m_pChunkListHead;
    while (pList)
    {
        StructArrayListEntryBase *pTrash = pList;
        pList = pList->pNext;
        pfnFree(this, pTrash);
    }
}


void StructArrayListBase::CreateNewChunk (SIZE_T InitialChunkLength, SIZE_T ChunkLengthGrowthFactor, SIZE_T cbElement, AllocProc *pfnAlloc)
{
    CONTRACTL {
        THROWS;
        PRECONDITION(!m_pChunkListHead || m_nItemsInLastChunk == m_nLastChunkCapacity);
    } CONTRACTL_END;

    SIZE_T nChunkCapacity;
    if (!m_pChunkListHead)
        nChunkCapacity = InitialChunkLength;
    else
        nChunkCapacity = m_nLastChunkCapacity * ChunkLengthGrowthFactor;

    SIZE_T cbChunk = sizeof(StructArrayListEntryBase) + cbElement * nChunkCapacity;

    StructArrayListEntryBase *pNewChunk = (StructArrayListEntryBase*)pfnAlloc(this, cbChunk);

    if (m_pChunkListTail)
    {
        _ASSERTE(m_pChunkListHead);
        m_pChunkListTail->pNext = pNewChunk;
    }
    else
    {
        _ASSERTE(!m_pChunkListHead);
        m_pChunkListHead = pNewChunk;
    }
    
    pNewChunk->pNext = NULL;
    m_pChunkListTail = pNewChunk;

    m_nItemsInLastChunk = 0;
    m_nLastChunkCapacity = nChunkCapacity;
}


void StructArrayListBase::ArrayIteratorBase::SetCurrentChunk (StructArrayListEntryBase *pChunk, SIZE_T nChunkCapacity)
{
    LEAF_CONTRACT;

    m_pCurrentChunk = pChunk;

    if (pChunk)
    {
        if (pChunk == m_pArrayList->m_pChunkListTail)
            m_nItemsInCurrentChunk = m_pArrayList->m_nItemsInLastChunk;
        else
            m_nItemsInCurrentChunk = nChunkCapacity;

        m_nCurrentChunkCapacity = nChunkCapacity;
    }
}

