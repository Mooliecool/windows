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
// StackingAllocator.h -
//

#ifndef __stacking_allocator_h__
#define __stacking_allocator_h__

#include "util.hpp"
#include "eecontract.h"


// We use zero sized arrays, disable the non-standard extension warning.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
#endif

#ifdef _DEBUG
    struct Sentinal
    {
		enum { marker1Val = 0xBAD00BAD };
		Sentinal(Sentinal* next) : m_Marker1(marker1Val), m_Next(next) { LEAF_CONTRACT; }

        unsigned  m_Marker1;        // just some data bytes 
        Sentinal* m_Next;           // linked list of these
    };
#endif

    // Blocks from which allocations are carved. Size is determined dynamically,
    // with upper and lower bounds of MinBlockSize and MaxBlockSize respectively
    // (though large allocation requests will cause a block of exactly the right
    // size to be allocated).
    struct StackBlock
    {
        StackBlock     *m_Next;         // Next oldest block in list
        DWORD_PTR   m_Length;       // Length of block excluding header  (needs to be pointer-sized for alignment on IA64)
        INDEBUG(Sentinal*   m_Sentinal;)    // insure that we don't fall of the end of the buffer
        INDEBUG(void**      m_Pad;)    		// keep the size a multiple of 8
        char        m_Data[];       // Start of user allocation space
    };

    // Whenever a checkpoint is requested, a checkpoint structure is allocated
    // (as a normal allocation) and is filled with information about the state
    // of the allocator prior to the checkpoint. When a Collapse request comes
    // in we can therefore restore the state of the allocator.
    // It is the address of the checkpoint structure that we hand out to the
    // caller of GetCheckpoint as an opaque checkpoint marker.
    struct Checkpoint
    {
        StackBlock *m_OldBlock;     // Head of block list before checkpoint
        unsigned    m_OldBytesLeft; // Number of free bytes before checkpoint
    };



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
class StackingAllocator
{
public:

    enum
    {
        MinBlockSize    = 128,
        MaxBlockSize    = 4096,
        InitBlockSize   = 512      
    };

#ifndef DACCESS_COMPILE
    StackingAllocator();
    ~StackingAllocator();
#else
    StackingAllocator() { LEAF_CONTRACT; }
#endif

    void StoreCheckpoint(Checkpoint *c)
    {
        LEAF_CONTRACT;

#ifdef _DEBUG
        m_CheckpointDepth++;
        m_Checkpoints++;
#endif

        // Record previous allocator state in it.
        c->m_OldBlock = m_FirstBlock;
        c->m_OldBytesLeft = m_BytesLeft;
    }

    void* GetCheckpoint();
    void* GetCheckpointSafeThrow();

    
    FORCEINLINE void* AllocNoThrow(unsigned Size)
    {
        CONTRACT (void*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            SO_TOLERANT;
            INJECT_FAULT(CONTRACT_RETURN NULL;);
            PRECONDITION(m_CheckpointDepth > 0);
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;
        
#ifdef _DEBUG
        m_Allocs++;
        m_MaxAlloc = max(Size, m_MaxAlloc);
#endif

        //special case, 0 size alloc, return non-null but invalid pointer
        if (Size == 0)
            RETURN (void*)-1;

        // Round size up to ensure alignment.
        unsigned n = (Size + 7) & ~7;

        // leave room for sentinal
        INDEBUG(n += sizeof(Sentinal));

        // Is the request too large for the current block?
        if (n > m_BytesLeft)
        {
            if (!AllocNewBlockForBytes(n))
                RETURN NULL;
        }

        // Once we get here we know we have enough bytes left in the block at the
        // head of the chain.
        _ASSERTE(n <= m_BytesLeft);

        void *ret = m_FirstFree;
        m_FirstFree += n;
        m_BytesLeft -= n;

#ifdef _DEBUG
        // Add sentinal to the end
        m_FirstBlock->m_Sentinal = new(m_FirstFree - sizeof(Sentinal)) Sentinal(m_FirstBlock->m_Sentinal);
#endif

        RETURN ret;
    }    
    
    void* AllocSafeThrow(unsigned Size);
    void* Alloc(unsigned Size);
    void  Collapse(void* CheckpointMarker);

private:
    bool AllocNewBlockForBytes(unsigned n);
    
    StackBlock      *m_FirstBlock;       // Pointer to head of allocation block list
    char       *m_FirstFree;        // Pointer to first free byte in head block
    unsigned    m_BytesLeft;        // Number of free bytes left in head block
    StackBlock      *m_InitialBlock;     // The first block is special, we never free it
    StackBlock      *m_DeferredFreeBlock; // Avoid going to the OS too often by deferring one free

#ifdef _DEBUG
    unsigned    m_CheckpointDepth;
    unsigned    m_Allocs;
    unsigned    m_Checkpoints;
    unsigned    m_Collapses;
    unsigned    m_BlockAllocs;
    unsigned    m_MaxAlloc;
#endif

    void Init(bool bResetInitBlock)
    {
        WRAPPER_CONTRACT;

        if (bResetInitBlock || (m_InitialBlock == NULL))
        {
			Clear(NULL);
            m_FirstBlock = NULL;
            m_FirstFree = NULL;
            m_BytesLeft = 0;
            m_InitialBlock = NULL;
        }
        else
        {
            m_FirstBlock = m_InitialBlock;
            m_FirstFree = m_InitialBlock->m_Data;
            m_BytesLeft = m_InitialBlock->m_Length;
        }
    }

#ifdef _DEBUG
    void Validate(StackBlock *block, void* spot)
    {
        LEAF_CONTRACT;

        if (!block) 
            return;
        Sentinal* ptr = block->m_Sentinal;
        _ASSERTE(spot);
        while(ptr >= spot)
        {
				// If this assert goes off then someone overwrote their buffer!
                // A common candidate is PINVOKE buffer run.  To confirm look
                // up on the stack for NDirect.* Look for the MethodDesc
                // associated with it.  Be very suspicious if it is one that
                // has a return string buffer!.  This usually means the end
                // programmer did not allocate a big enough buffer before passing
                // it to the PINVOKE method.
            if (ptr->m_Marker1 != Sentinal::marker1Val)
                _ASSERTE(!"Memory overrun!! May be bad buffer passed to PINVOKE. turn on logging LF_STUBS level 6 to find method");
            ptr = ptr->m_Next;
        }
        block->m_Sentinal = ptr;
	}
#endif

    void Clear(StackBlock *ToBlock)
    {
        LEAF_CONTRACT;

        StackBlock *p = m_FirstBlock;
        StackBlock *q;

        while (p != ToBlock)
        {
            q = p;
            p = p->m_Next;
            INDEBUG(Validate(q, q));

            if (m_DeferredFreeBlock != NULL)
            {
                delete [] (char *)m_DeferredFreeBlock;
            }

            m_DeferredFreeBlock = q;
            m_DeferredFreeBlock->m_Next = NULL;
        }
    }

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
