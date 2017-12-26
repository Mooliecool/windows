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
// --------------------------------------------------------------------------------
// SBuffer.h  (Safe Buffer)
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// SBuffer is a relatively safe way to manipulate a dynamically
// allocated data buffer.  An SBuffer is conceptually a simple array
// of bytes.  It maintains both a conceptual size and an actual allocated size.
// 
// SBuffer provides safe access to the data buffer by providing rich high 
// level functionality (like insertion, deleteion, copying,  comparison, and 
// iteration) without exposing direct pointers to its buffers. 
// 
// For interoperability, SBuffers can expose their buffers - either as readonly
// by BYTE * or void * cases, or as writable by the OpenRawBuffer/CloseRawBuffer
// entry points.  Use of these should be limited wherever possible though; as there
// is always a possibilility of buffer overrun.
//
// To mimimize heap allocations, the InlineSBuffer template will preallocate a fixed
// size buffer inline with the SBuffer object itself.  It will use this buffer unless
// it needs a bigger one, in which case it transparently moves on to using the heap.
// The StackSBuffer class instatiates the InlineSBuffer with a standard heuristic 
// stack preallocation size.
//
// SBuffer is "subclassable" to add content typeing to the buffer. See SArray and
// SString for examples.
// --------------------------------------------------------------------------------

#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#include "clrtypes.h"
#include "iterator.h"
#include "check.h"
#include "daccess.h"

// ================================================================================
// Macros for computing padding
// ================================================================================

#define ALIGNMENT(size) \
    (( (size^(size-1)) >> 1) +1)

#define ALIGN(size, align) \
    (((size)+(align)-1) & ~((align)-1))

#define PAD(size, align) \
    (ALIGN((size), (align)) - (size))

// ================================================================================
// SBuffer : base class for safe buffers
// ================================================================================

class SBuffer
{
  public:
    //--------------------------------------------------------------------
    // Flags and constants
    //--------------------------------------------------------------------

    enum ImmutableFlag
    {
        Immutable
    };
    
    enum PreallocFlag
    {
        Prealloc
    };
    
    //--------------------------------------------------------------------
    // Types
    //--------------------------------------------------------------------

  public:
    class CIterator;
    friend class CIterator;

    class Iterator;
    friend class Iterator;

    //--------------------------------------------------------------------
    // Initializers and constructors
    //--------------------------------------------------------------------

  public:
    // Constructors
    SBuffer();
    SBuffer(COUNT_T size);
    SBuffer(const BYTE *buffer, COUNT_T size);
    explicit SBuffer(const SBuffer &buffer);

    // Immutable constructor should ONLY be used if buffer will 
    // NEVER BE FREED OR MODIFIED. PERIOD. .
    SBuffer(ImmutableFlag immutable, const BYTE *buffer, COUNT_T size);

    // Prealloc should be allocated inline with SBuffer - it must have the same
    // lifetime as SBuffer's memory.
    SBuffer(PreallocFlag prealloc, void *buffer, COUNT_T size);

    ~SBuffer();

    void Clear();

    void Set(const SBuffer &buffer);
    void Set(const BYTE *buffer, COUNT_T size);
    void SetImmutable(const BYTE *buffer, COUNT_T size);

    //--------------------------------------------------------------------
    // Buffer size routines.  A buffer has an externally visible size, but
    // it also has an internal allocation size which may be larger.
    //--------------------------------------------------------------------

    // Get and set size of buffer.  Note that the actual size of the 
    // internally allocated memory block may be bigger.
    COUNT_T GetSize() const;
    void SetSize(COUNT_T count);

    // Grow size of buffer to maximum amount without reallocating.
    void MaximizeSize();

    //--------------------------------------------------------------------
    // Buffer allocation routines
    //--------------------------------------------------------------------

    // Return the current available allocation space of the buffer.
    COUNT_T GetAllocation() const;

    // Preallocate some memory you expect to use.  This can prevent
    // multiple reallocations.  Note this does not change the visible
    // size of the buffer.
    void Preallocate(COUNT_T allocation) const;
    
    // Shrink memory usage of buffer to minimal amount.  Note that
    // this does not change the visible size of the buffer.
    void Trim() const;

    //--------------------------------------------------------------------
    // Content manipulation routines
    //--------------------------------------------------------------------

    void Zero();
    void Fill(BYTE value);
    void Fill(const Iterator &to, BYTE value, COUNT_T size);

    // Internal copy. "Copy" leaves from range as is; "Move"
    // leaves from range in uninitialized state. 
    // (This distinction is more important when using from a 
    // typed wrapper than in the base SBuffer class.)
    // 
    // NOTE: Copy vs Move is NOT based on whether ranges overlap
    // or not.  Ranges may overlap in either case.
    // 
    // Note that both Iterators must be on THIS buffer.
    void Copy(const Iterator &to, const CIterator &from, COUNT_T size);
    void Move(const Iterator &to, const CIterator &from, COUNT_T size);

    // External copy. 
    void Copy(const Iterator &i, const SBuffer &source);
    void Copy(const Iterator &i, const void *source, COUNT_T size);
    void Copy(void *dest, const CIterator &i, COUNT_T size);

    // Insert bytes at the given iterator location.
    void Insert(const Iterator &i, const SBuffer &source);
    void Insert(const Iterator &i, COUNT_T size);

    // Delete bytes at the given iterator location
    void Delete(const Iterator &i, COUNT_T size);

    // Replace bytes at the given iterator location
    void Replace(const Iterator &i, COUNT_T deleteSize, const SBuffer &insert);
    void Replace(const Iterator &i, COUNT_T deleteSize, COUNT_T insertSize);

    // Compare entire buffer; return -1, 0, 1
    int Compare(const SBuffer &compare) const;
    int Compare(const BYTE *match, COUNT_T size) const;

    // Compare entire buffer; return TRUE or FALSE
    BOOL Equals(const SBuffer &compare) const;
    BOOL Equals(const BYTE *match, COUNT_T size) const;

    // Match portion of this buffer to given bytes; return TRUE or FALSE
    BOOL Match(const CIterator &i, const SBuffer &match) const;
    BOOL Match(const CIterator &i, const BYTE *match, COUNT_T size) const;

    //--------------------------------------------------------------------
    // Iterators
    //
    // Note that any iterator returned is not
    // valid after any operation which may resize the buffer, unless
    // the operation was performed on that particular iterator.
    //--------------------------------------------------------------------

    CIterator Begin() const;
    CIterator End() const;

    Iterator Begin();
    Iterator End();

    BYTE & operator[] (int index);
    const BYTE & operator[] (int index) const;

    //--------------------------------------------------------------------
    // Raw buffer access
    //
    // Accessing a raw buffer via pointer is inherently more dangerous than
    // other uses of this API, and should be avoided if at all possible. 
    // It is primarily provided for compatibility with existing APIs.
    //
    // Note that any buffer pointer returned is not
    // valid after any operation which may resize the buffer.
    //--------------------------------------------------------------------

    // Casting operators return the existing buffer as
    // a raw const pointer.  Note that the pointer is valid only
    // until the buffer is modified via an API.
    operator const void *() const;
    operator const BYTE *() const;

    // To write directly to the SString's underlying buffer: 
    // 1) Call OpenRawBuffer() and pass it the count of bytes 
    // you need.
    // 2) That returns a pointer to the raw buffer which you can write to.
    // 3) When you are done writing to the pointer, call CloseBuffer()
    // and pass it the count of bytes you actually wrote. 
    // The pointer from step 1 is now invalid. 

    // example usage:
    // void GetInfo(SBuffer &buf) 
    // {
    //      BYTE *p = buf.OpenRawBuffer(3);
    //      OSGetSomeInfo(p, 3);
    //      buf.CloseRawBuffer();
    // }

    // You should open the buffer, write the data, and immediately close it.
    // No sbuffer operations are valid while the buffer is opened.
    //                                                              
    // In a debug build, Open/Close will do lots of little checks to make sure
    // you don't buffer overflow while it's opened. In a retail build, this
    // is a very streamlined action.

    // Open the raw buffer for writing count bytes
    BYTE *OpenRawBuffer(COUNT_T maxCount);
    
    // Call after OpenRawBuffer().
    
    // Provide the count of bytes actually used. This will make sure the 
    // SBuffer's size is correct.
    void CloseRawBuffer(COUNT_T actualCount);

    // Close the buffer. Assumes that we completely filled the buffer
    // that OpenRawBuffer() gave back. 
    void CloseRawBuffer();

    //--------------------------------------------------------------------
    // Check routines. These are typically used internally, but may be
    // called externally if desired.
    //--------------------------------------------------------------------

    CHECK CheckBufferClosed() const;
    static CHECK CheckSize(COUNT_T size);
    static CHECK CheckAllocation(COUNT_T allocation);
    CHECK CheckIteratorRange(const CIterator &i) const;
    CHECK CheckIteratorRange(const CIterator &i, COUNT_T size) const;

    CHECK Check() const;
    CHECK Invariant() const;
    CHECK InternalInvariant() const;

  protected:

    //--------------------------------------------------------------------
    // Internal helper routines
    //--------------------------------------------------------------------

    // Preserve = preserve contents while reallocating
    typedef enum
    {
        DONT_PRESERVE = 0,
        PRESERVE = 1,
    } Preserve;

    void Resize(COUNT_T size, Preserve preserve = PRESERVE);
    void ResizePadded(COUNT_T size, Preserve preserve = PRESERVE);
    void TweakSize(COUNT_T size);
    void ReallocateBuffer(COUNT_T allocation, Preserve preserve);
    void EnsureMutable() const;

    //--------------------------------------------------------------------
    // We define some extra flags and fields for subclasses (these are specifically
    // designed for SString, but use otherwise if desired.)
    //--------------------------------------------------------------------

    BOOL IsFlag1() const;
    void SetFlag1();
    void ClearFlag1();

    BOOL IsFlag2() const;
    void SetFlag2();
    void ClearFlag2();

    BOOL IsFlag3() const;
    void SetFlag3();
    void ClearFlag3();

    INT GetRepresentationField() const;
    void SetRepresentationField(int value);

  protected:

    //--------------------------------------------------------------------
    // Flag access
    //--------------------------------------------------------------------

    BOOL IsAllocated() const;
    void SetAllocated();
    void ClearAllocated();

    BOOL IsImmutable() const;
    void SetImmutable();
    void ClearImmutable();

#if _DEBUG
    BOOL IsOpened() const;
    void SetOpened();
    void ClearOpened();
#endif

    //--------------------------------------------------------------------
    // Buffer management routines
    //--------------------------------------------------------------------

    // Allocate and free a memory buffer
    BYTE *NewBuffer(COUNT_T allocation);
    void DeleteBuffer(BYTE *buffer, COUNT_T allocation);

    // Use existing buffer
    BYTE *UseBuffer(BYTE *buffer, COUNT_T *allocation);

    CHECK CheckBuffer(const BYTE* buffer, COUNT_T allocation) const;

    // Manipulates contents of the buffer via the plugins below, but 
    // adds some debugging checks.  Should always call through here rather
    // than directly calling the extensibility points.
    void DebugMoveBuffer(BYTE *to, BYTE *from, COUNT_T size);
    void DebugCopyConstructBuffer(BYTE *to, const BYTE *from, COUNT_T size);
    void DebugConstructBuffer(BYTE *buffer, COUNT_T size);
    void DebugDestructBuffer(BYTE *buffer, COUNT_T size);

    void DebugStompUnusedBuffer(BYTE *buffer, COUNT_T size);
    CHECK CheckUnusedBuffer(const BYTE *buffer, COUNT_T size) const;

#ifdef DACCESS_COMPILE
    void* DacGetRawContent(void) const
    {
        return DacInstantiateTypeByAddress((TADDR)m_buffer, m_size, true);
    }

    void EnumMemoryRegions(void) const
    {
        DacEnumMemoryRegion((TADDR)m_buffer, m_size);
    }
#endif

    //----------------------------------------------------------------------------
    // Iterator base class
    //----------------------------------------------------------------------------

    friend class CheckedIteratorBase<SBuffer>;

    class Index : public CheckedIteratorBase<SBuffer>
    {
        friend class SBuffer;

        friend class CIterator;
        friend class Indexer<const BYTE, CIterator>;

        friend class Iterator;
        friend class Indexer<BYTE, Iterator>;

      protected:
        BYTE* m_ptr;
        
        Index();
        Index(SBuffer *container, SCOUNT_T index);
        BYTE &GetAt(SCOUNT_T delta) const;
        void Skip(SCOUNT_T delta);
        SCOUNT_T Subtract(const Index &i) const;

        CHECK DoCheck(SCOUNT_T delta) const;

        void Resync(const SBuffer *container, BYTE *value) const;
    };

  public:

    class CIterator : public Index, public Indexer<const BYTE, CIterator>
    {
        friend class SBuffer;

    public:
        CIterator()
        {
        }

        CIterator(const SBuffer *buffer, int index)
          : Index(const_cast<SBuffer*>(buffer), index)
        {
        }
    };

    class Iterator : public Index, public Indexer<BYTE, Iterator>
    {
        friend class SBuffer;

    public:
        operator const CIterator &() const
        {
            return *(const CIterator *)this;
        }

        operator CIterator &()
        {
            return *(CIterator *)this;
        }

        Iterator()
        {
        }

        Iterator(SBuffer *buffer, int index)
          : Index(buffer, index)
        {
        }

    };


    //----------------------------------------------------------------------------
    // Member and data declarations
    //----------------------------------------------------------------------------

  private:
    enum
    {
        REPRESENTATION_MASK     = 0x07,
        ALLOCATED               = 0x08,
        IMMUTABLE               = 0x10,
        OPENED                  = 0x20,
        FLAG1                   = 0x40,
        FLAG2                   = 0x80,
        FLAG3                   = 0x100,
    };

    COUNT_T   m_size;           // externally visible size
    COUNT_T   m_allocation;     // actual allocated size
    UINT32    m_flags;

  protected:
    BYTE     *m_buffer;
    
#if _DEBUG
  protected:
    // We will update the "revision" of the buffer every time it is potentially reallocation, 
    // so we can tell when iterators are no longer valid.
    int m_revision;
#endif
};

// ================================================================================
// InlineSBuffer : Tlempate for an SBuffer with preallocated buffer space
// ================================================================================

#define BUFFER_ALIGNMENT 4

template <COUNT_T size>
class InlineSBuffer : public SBuffer
{
 private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200) // zero sized array
#pragma warning(disable:4324) // don't complain if DECLSPEC_ALIGN actually pads
    DECLSPEC_ALIGN(BUFFER_ALIGNMENT) BYTE m_prealloc[size];
#pragma warning(pop)
#else
     // use UINT64 to get maximum alignment of the memory
     UINT64 m_prealloc[ALIGN(size,sizeof(UINT64))/sizeof(UINT64)];
#endif // _MSC_VER

 public:
    InlineSBuffer()
      : SBuffer((BYTE*)m_prealloc, size)
    {
        WRAPPER_CONTRACT;
    }
};

// ================================================================================
// StackSBuffer : SBuffer with relatively large preallocated buffer for stack use
// ================================================================================

#define STACK_ALLOC 256

typedef InlineSBuffer<STACK_ALLOC> StackSBuffer;

// ================================================================================
// Inline definitions
// ================================================================================

#include <sbuffer.inl>

#endif  // _SBUFFER_H_
