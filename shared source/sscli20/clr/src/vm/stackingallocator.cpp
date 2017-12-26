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
// StackingAllocator.cpp -
//
// Non-thread safe allocator designed for allocations with the following
// pattern:
//      allocate, allocate, allocate ... deallocate all
// There may also be recursive uses of this allocator (by the same thread), so
// the usage becomes:
//      mark checkpoint, allocate, allocate, ..., deallocate back to checkpoint
//
// Allocations come from a singly linked list of blocks with dynamically
// determined size (the goal is to have fewer block allocations than allocation
// requests).
//
// Allocations are very fast (in the case where a new block isn't allocated)
// since blocks are carved up into packets by simply moving a cursor through
// the block.
//
// Allocations are guaranteed to be quadword aligned.


#include "common.h"
#include "excep.h"


#define INC_COUNTER(_name, _amount)
#define MAX_COUNTER(_name, _amount)


StackingAllocator::StackingAllocator()
{
    WRAPPER_CONTRACT;

    _ASSERTE((sizeof(StackBlock) & 7) == 0);
    _ASSERTE((sizeof(Checkpoint) & 7) == 0);

    m_FirstBlock = NULL;
    m_FirstFree = NULL;
    m_InitialBlock = NULL;
    m_DeferredFreeBlock = NULL;

#ifdef _DEBUG
        m_CheckpointDepth = 0;
        m_Allocs = 0;
        m_Checkpoints = 0;
        m_Collapses = 0;
        m_BlockAllocs = 0;
        m_MaxAlloc = 0;
#endif

    Init(true);
}


StackingAllocator::~StackingAllocator()
{
    WRAPPER_CONTRACT;

    Clear(NULL);
 
    if (m_DeferredFreeBlock)
    {
        delete [] (char*)m_DeferredFreeBlock;
        m_DeferredFreeBlock = NULL;
    }

#ifdef _DEBUG
        INC_COUNTER(L"Allocs", m_Allocs);
        INC_COUNTER(L"Checkpoints", m_Checkpoints);
        INC_COUNTER(L"Collapses", m_Collapses);
        INC_COUNTER(L"BlockAllocs", m_BlockAllocs);
        MAX_COUNTER(L"MaxAlloc", m_MaxAlloc);
#endif
}


void *StackingAllocator::GetCheckpoint()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

#ifdef _DEBUG
    m_CheckpointDepth++;
    m_Checkpoints++;
#endif

    // As an optimization, initial checkpoints are lightweight (they just return
    // a special marker, NULL). This is because we know how to restore the
    // allocator state on a Collapse without having to store any additional
    // context info.
    if ((m_InitialBlock == NULL) || (m_FirstFree == m_InitialBlock->m_Data))
        return NULL;

    // Remember the current allocator state.
    StackBlock *pOldBlock = m_FirstBlock;
    unsigned iOldBytesLeft = m_BytesLeft;

    // Allocate a checkpoint block (just like a normal user request).
    Checkpoint *c = (Checkpoint *)Alloc(sizeof(Checkpoint));

    // Record previous allocator state in it.
    c->m_OldBlock = pOldBlock;
    c->m_OldBytesLeft = iOldBytesLeft;

    // Return the checkpoint marker.
    return c;
}


void* StackingAllocator::GetCheckpointSafeThrow()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

#ifdef _DEBUG
    m_CheckpointDepth++;
    m_Checkpoints++;
#endif

    // As an optimization, initial checkpoints are lightweight (they just return
    // a special marker, NULL). This is because we know how to restore the
    // allocator state on a Collapse without having to store any additional
    // context info.
    if ((m_InitialBlock == NULL) || (m_FirstFree == m_InitialBlock->m_Data))
        return NULL;

    // Remember the current allocator state.
    StackBlock *pOldBlock = m_FirstBlock;
    unsigned iOldBytesLeft = m_BytesLeft;

    // Allocate a checkpoint block (just like a normal user request).
    Checkpoint *c = (Checkpoint *)AllocSafeThrow(sizeof(Checkpoint));

    // Record previous allocator state in it.
    c->m_OldBlock = pOldBlock;
    c->m_OldBytesLeft = iOldBytesLeft;

    // Return the checkpoint marker.
    return c;

}

bool StackingAllocator::AllocNewBlockForBytes(unsigned n)
{
    CONTRACT (bool)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(m_CheckpointDepth > 0);
    }
    CONTRACT_END;

    // already aligned and in the hard case
    
    _ASSERTE(n % 8 == 0);
    _ASSERTE(n > m_BytesLeft);

    StackBlock* b = NULL;
    

    if (m_DeferredFreeBlock != NULL && m_DeferredFreeBlock->m_Length >= n) 
    {
        b =  m_DeferredFreeBlock;
        m_DeferredFreeBlock = NULL;

        // b->m_Length doesn't need init because its value is still valid
        // from the original allocation
    }
    else
    {
        // Allocate a block four times as large as the request but with a lower
        // limit of MinBlockSize and an upper limit of MaxBlockSize. If the
        // request is larger than MaxBlockSize then allocate exactly that
        // amount.
        // Additionally, if we don't have an initial block yet, use an increased
        // lower bound for the size, since we intend to cache this block.
        unsigned lower = m_InitialBlock ? MinBlockSize : InitBlockSize;
        size_t allocSize = sizeof(StackBlock) + max(n, min(max(n * 4, lower), MaxBlockSize));

        // Allocate the block.
        b = (StackBlock *)new (nothrow) char[allocSize];
        if (b == NULL)
            RETURN false;

        // reserve space for the Block structure and then link it in
        b->m_Length = (unsigned) (allocSize - sizeof(StackBlock));

#ifdef _DEBUG
        m_BlockAllocs++;
#endif
     }

     // If this is the first block allocated, we record that fact since we
     // intend to cache it.
     if (m_InitialBlock == NULL)
     {
         _ASSERTE((m_FirstBlock == NULL) && (m_FirstFree == NULL) && (m_BytesLeft == 0));
         m_InitialBlock = b;
     }

     // Link new block to head of block chain and update internal state to
     // start allocating from this new block.
     b->m_Next = m_FirstBlock;
     m_FirstBlock = b;
     m_FirstFree = b->m_Data;
     m_BytesLeft = b->m_Length;
     INDEBUG(b->m_Sentinal = 0);
     
     RETURN true;
}


void* StackingAllocator::AllocSafeThrow(unsigned Size)
{
    CONTRACT (void*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
        PRECONDITION(m_CheckpointDepth > 0);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // OOM fault injection in AllocNoThrow

    void* retval = AllocNoThrow(Size);
    if (retval == NULL)
        ENCLOSE_IN_EXCEPTION_HANDLER ( ThrowOutOfMemory );

    RETURN retval;
}

void *StackingAllocator::Alloc(unsigned Size)
{
    CONTRACT (void*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
        PRECONDITION(m_CheckpointDepth > 0);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // OOM fault injection in AllocNoThrow

    void* retval = AllocNoThrow(Size);
    if (retval == NULL)
        ThrowOutOfMemory();

    RETURN retval;
}


void StackingAllocator::Collapse(void *CheckpointMarker)
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_CheckpointDepth > 0);

#ifdef _DEBUG
    m_CheckpointDepth--;
    m_Collapses++;
#endif

    Checkpoint *c = (Checkpoint *)CheckpointMarker;

    // Special case collapsing back to the initial checkpoint.
    if (c == NULL || c->m_OldBlock == NULL) {
        Clear(m_InitialBlock);
        Init(false);

        // confirm no buffer overruns
        INDEBUG(Validate(m_FirstBlock, m_FirstFree));
        
        return;
    }

    // Cache contents of checkpoint, we can potentially deallocate it in the
    // next step (if a new block had to be allocated to accomodate the
    // checkpoint).
    StackBlock *pOldBlock = c->m_OldBlock;
    unsigned iOldBytesLeft = c->m_OldBytesLeft;

    // Start deallocating blocks until the block that was at the head on the
    // chain when the checkpoint is taken is there again.
    Clear(pOldBlock);

    // Restore former allocator state.
    m_FirstBlock = pOldBlock;
    m_FirstFree = &pOldBlock->m_Data[pOldBlock->m_Length - iOldBytesLeft];
    m_BytesLeft = iOldBytesLeft;

    // confirm no buffer overruns
    INDEBUG(Validate(m_FirstBlock, m_FirstFree));
}
