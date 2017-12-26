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
#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include <daccess.h>
#include <contract.h>

//
// ArrayList is a simple class which is used to contain a growable
// list of pointers, stored in chunks.  Modification is by appending
// only currently.  Access is by index (efficient if the number of
// elements stays small) and iteration (efficient in all cases).
// 
// An important property of an ArrayList is that the list remains
// coherent while it is being modified. This means that readers
// never need to lock when accessing it.
//

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200) // Disable zero-sized array warning
#endif

class ArrayListBase
{
 public:

    enum
    {
        ARRAY_BLOCK_SIZE_START = 5,
    };

  private:

    struct ArrayListBlock
    {
        SPTR(ArrayListBlock)    m_next;
        DWORD                   m_blockSize;
        void                    *m_array[0];

#ifdef DACCESS_COMPILE
        static ULONG32 DacSize(TADDR addr)
        {
            LEAF_CONTRACT;
            return sizeof(ArrayListBlock) +
                (*PTR_DWORD(addr + FIELD_OFFSET(ArrayListBlock, m_blockSize)) * sizeof(void*));
        }
#endif
    };
    typedef SPTR(ArrayListBlock) PTR_ArrayListBlock;

    struct FirstArrayListBlock
    {
        PTR_ArrayListBlock      m_next;
        DWORD                   m_blockSize;
        void                    *m_array[ARRAY_BLOCK_SIZE_START];
    };

    DWORD               m_count;
    FirstArrayListBlock m_firstBlock;

  public:

    void **GetPtr(DWORD index);
    void *Get(DWORD index) 
    { 
        WRAPPER_CONTRACT; 
        
        return *GetPtr(index); 
    }
    
    void Set(DWORD index, void *element) 
    { 
        WRAPPER_CONTRACT; 
        STATIC_CONTRACT_SO_INTOLERANT;
        *GetPtr(index) = element; 
    }

    DWORD GetCount() { LEAF_CONTRACT; return m_count; }

    HRESULT Append(void *element);

    enum { NOT_FOUND = -1 };
    DWORD FindElement(DWORD start, void *element);

    void Clear();

    void Init()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_INTOLERANT;
        
        m_count = 0;
        m_firstBlock.m_next = NULL;
        m_firstBlock.m_blockSize = ARRAY_BLOCK_SIZE_START;
    }

    void Destroy()
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_INTOLERANT;
        Clear();
    }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
    class Iterator 
    {
        friend class ArrayListBase;

      public:
        BOOL Next();

        void SetEmpty()
        {
            LEAF_CONTRACT;
            
            m_block = NULL;
            m_index = (DWORD)-1;
            m_remaining = 0;
            m_total = 0;
        }
        
        void *GetElement() {LEAF_CONTRACT; return m_block->m_array[m_index]; }
        void **GetElementPtr() {LEAF_CONTRACT; return m_block->m_array + m_index; }
        DWORD GetIndex() {LEAF_CONTRACT; return m_index + m_total; }

      private:
        ArrayListBlock*     m_block;
        DWORD               m_index;
        DWORD               m_remaining;
        DWORD               m_total;
        static Iterator Create(ArrayListBlock* block, DWORD remaining)
          {
              LEAF_CONTRACT;
              STATIC_CONTRACT_SO_INTOLERANT;
              Iterator i;
              i.m_block = block;
              i.m_index = (DWORD) -1;
              i.m_remaining = remaining;
              i.m_total = 0;
              return i;
          }
    };

    Iterator Iterate()
    {
        STATIC_CONTRACT_SO_INTOLERANT;
        WRAPPER_CONTRACT;
        return Iterator::Create((ArrayListBlock*)&m_firstBlock, m_count);
    }


    // BlockIterator is used for only memory walking, such as prejit save/fixup.
    // It is not appropriate for other more typical ArrayList use.
    class BlockIterator
    {
      private:

        ArrayListBlock *m_block;

        friend class ArrayListBase;
        BlockIterator(ArrayListBlock *block)
          : m_block(block)
        {
        }

      public:
        
        BOOL Next()
        { 
            if (m_block != NULL)
                m_block = m_block->m_next;
            return m_block != NULL;
        }

        void **GetNextPtr()
        {
            return (void **) &m_block->m_next;
        }

        void *GetBlock()
        {
            return m_block;
        }
        
        SIZE_T GetBlockSize()
        {
            return sizeof(ArrayListBlock) + (m_block->m_blockSize * sizeof(void*));
        }
    };

    void **GetInitialNextPtr()
    {
        return (void **) &m_firstBlock.m_next;
    }

    BlockIterator IterateBlocks()
    {
        return BlockIterator((ArrayListBlock *) &m_firstBlock);
    }

};

class ArrayList : public ArrayListBase
{
public:
#ifndef DACCESS_COMPILE
    ArrayList()
    {
        STATIC_CONTRACT_SO_INTOLERANT;
        WRAPPER_CONTRACT;
        Init();
    }

    ~ArrayList()
    {
        STATIC_CONTRACT_SO_INTOLERANT;
        WRAPPER_CONTRACT;
        Destroy();
    }
#endif
};

/* to be used as static variable - no constructor/destructor, assumes zero 
   initialized memory */
class ArrayListStatic : public ArrayListBase
{
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif



//*****************************************************************************
// StructArrayList is similar to ArrayList, but the element type can be any
// arbitrary type.  Elements can only be accessed sequentially.  This is
// basically just a more efficient linked list - it's useful for accumulating
// lots of small fixed-size allocations into larger chunks, which would
// otherwise have an unnecessarily high ratio of heap overhead.
//
// The allocator provided must throw an exception on failure.
//*****************************************************************************

struct StructArrayListEntryBase
{
    StructArrayListEntryBase *pNext;  // actually StructArrayListEntry<ELEMENT_TYPE>*
};

template<class ELEMENT_TYPE>
struct StructArrayListEntry : StructArrayListEntryBase
{
    ELEMENT_TYPE rgItems[1];
};

class StructArrayListBase
{
protected:

    typedef void *AllocProc (void *pvContext, SIZE_T cb);
    typedef void FreeProc (void *pvContext, void *pv);

    StructArrayListBase ()
    {
        LEAF_CONTRACT;

        m_pChunkListHead = NULL;
        m_pChunkListTail = NULL;
        m_nTotalItems = 0;
    }

    void Destruct (FreeProc *pfnFree);

    void CreateNewChunk (SIZE_T InitialChunkLength, SIZE_T ChunkLengthGrowthFactor, SIZE_T cbElement, AllocProc *pfnAlloc);

    class ArrayIteratorBase
    {
    protected:

        void SetCurrentChunk (StructArrayListEntryBase *pChunk, SIZE_T nChunkCapacity);

        StructArrayListEntryBase *m_pCurrentChunk;
        SIZE_T m_nItemsInCurrentChunk;
        SIZE_T m_nCurrentChunkCapacity;
        StructArrayListBase *m_pArrayList;
    };
    friend class ArrayIteratorBase;

    StructArrayListEntryBase *m_pChunkListHead;  // actually StructArrayListEntry<ELEMENT_TYPE>*
    StructArrayListEntryBase *m_pChunkListTail;  // actually StructArrayListEntry<ELEMENT_TYPE>*
    SIZE_T m_nItemsInLastChunk;
    SIZE_T m_nTotalItems;
    SIZE_T m_nLastChunkCapacity;
};

template <class                ELEMENT_TYPE,
          SIZE_T               INITIAL_CHUNK_LENGTH,
          SIZE_T               CHUNK_LENGTH_GROWTH_FACTOR,
          class                ALLOCATOR>
class StructArrayList : public StructArrayListBase
{
private:

    CPP_ASSERT(1, INITIAL_CHUNK_LENGTH > 0);
    CPP_ASSERT(2, CHUNK_LENGTH_GROWTH_FACTOR > 0);

    friend class ArrayIterator;
    friend class ElementIterator;
    
public:

    StructArrayList ()
    {
        LEAF_CONTRACT;
    }

    ~StructArrayList ()
    {
        WRAPPER_CONTRACT;

        Destruct(&ALLOCATOR::Free);
    }

    ELEMENT_TYPE *AppendThrowing ()
    {
        CONTRACTL {
            THROWS;
        } CONTRACTL_END;

        if (!m_pChunkListTail || m_nItemsInLastChunk == m_nLastChunkCapacity)
            CreateNewChunk(INITIAL_CHUNK_LENGTH, CHUNK_LENGTH_GROWTH_FACTOR, sizeof(ELEMENT_TYPE), &ALLOCATOR::Alloc);

        m_nTotalItems++;
        m_nItemsInLastChunk++;
        return &((StructArrayListEntry<ELEMENT_TYPE>*)m_pChunkListTail)->rgItems[m_nItemsInLastChunk-1];
    }

    SIZE_T Count ()
    {
        LEAF_CONTRACT;
        
        return m_nTotalItems;
    }

    VOID CopyTo (ELEMENT_TYPE *pDest)
    {
        ArrayIterator iter(this);
        ELEMENT_TYPE *pSrc;
        SIZE_T nSrc;

        while (pSrc = iter.GetNext(&nSrc))
        {
            memcpy(pDest, pSrc, nSrc * sizeof(ELEMENT_TYPE));
            pDest += nSrc;
        }
    }

    class ArrayIterator : public ArrayIteratorBase
    {
    public:

        ArrayIterator (StructArrayList *pArrayList)
        {
            WRAPPER_CONTRACT;
            
            m_pArrayList = pArrayList;
            SetCurrentChunk(pArrayList->m_pChunkListHead, INITIAL_CHUNK_LENGTH);
        }

        ELEMENT_TYPE *GetCurrent (SIZE_T *pnElements)
        {
            LEAF_CONTRACT;
            
            ELEMENT_TYPE *pRet = NULL;
            SIZE_T nElements = 0;

            if (m_pCurrentChunk)
            {
                pRet = &((StructArrayListEntry<ELEMENT_TYPE>*)m_pCurrentChunk)->rgItems[0];
                nElements = m_nItemsInCurrentChunk;
            }

            *pnElements = nElements;
            return pRet;
        }

        // Returns NULL when there are no more items.
        ELEMENT_TYPE *GetNext (SIZE_T *pnElements)
        {
            WRAPPER_CONTRACT;

            ELEMENT_TYPE *pRet = GetCurrent(pnElements);

            if (pRet)
                SetCurrentChunk(m_pCurrentChunk->pNext, m_nCurrentChunkCapacity * CHUNK_LENGTH_GROWTH_FACTOR);

            return pRet;
        }
    };
};


#endif
