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
// BitMask.inl
// --------------------------------------------------------------------------------

#include <bitmask.h>

#ifndef _BITMASK_INL_
#define _BITMASK_INL_

inline BOOL BitMask::IsArray()
{
    LEAF_CONTRACT;
    return (m_mask&1) == 0;
}

// Indexing computations
inline COUNT_T BitMask::BitToIndex(int bit)
{
    LEAF_CONTRACT;
    // First word has one less bit due to tag
    return (bit+1) >> BIT_SIZE_SHIFT;
}

inline COUNT_T BitMask::BitToShift(int bit)
{
    LEAF_CONTRACT;
    // First word has one less bit due to tag
    return (bit+1) & BIT_SIZE_MASK;
}

// Array access.  Note the first array element is the count of the 
// rest of the elements

inline COUNT_T *BitMask::GetMaskArray()
{
    LEAF_CONTRACT;
    if (IsArray())
    {
        CONSISTENCY_CHECK(CheckPointer(m_maskArray));
        return m_maskArray+1;
    }
    else
        return &m_mask;
}
    
inline COUNT_T BitMask::GetMaskArraySize()
{
    LEAF_CONTRACT;
    if (IsArray())
        return *m_maskArray;
    else
        return 1;
}

inline void BitMask::GrowArray(COUNT_T newSize)
{
    CONTRACTL
    {
          THROWS;
    }
    CONTRACTL_END;

    // Ensure we don't grow too often

    COUNT_T oldSize = GetMaskArraySize();
    if (newSize <= oldSize)
        return;

    if (newSize < oldSize*2)
        newSize = oldSize*2;
    if (newSize < MIN_ARRAY_ALLOCATION)
        newSize = MIN_ARRAY_ALLOCATION;

    // Allocate new array

    COUNT_T *newArray = new COUNT_T [newSize+1];
    *newArray = newSize;
        
    CopyMemory(newArray+1, GetMaskArray(), oldSize * sizeof(COUNT_T));
    ZeroMemory(newArray+oldSize+1, (newSize - oldSize) * sizeof(COUNT_T));

    if (IsArray())
        delete [] m_maskArray;

    m_maskArray = newArray;
}
    
inline BitMask::BitMask()
  : m_mask(1)
{
    LEAF_CONTRACT;
}

inline BitMask::~BitMask()
{
    LEAF_CONTRACT;

    if (IsArray())
        delete [] m_maskArray;
}

inline BOOL BitMask::TestBit(int bit)
{
    LEAF_CONTRACT;

    COUNT_T index = BitToIndex(bit);

    if (index >= GetMaskArraySize())
        return FALSE;

    return ( GetMaskArray()[index] >> BitToShift(bit) ) & 1;
}

inline void BitMask::SetBit(int bit)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;

    COUNT_T index = BitToIndex(bit);

    if (index >= GetMaskArraySize())
        GrowArray(index+1);
            
    GetMaskArray()[index] |= (1 << BitToShift(bit));
}

inline void BitMask::ClearBit(int bit)
{
    LEAF_CONTRACT;

    COUNT_T index = BitToIndex(bit);

    if (index >= GetMaskArraySize())
        return;
            
    GetMaskArray()[index] &= ~(1 << BitToShift(bit));
}

inline BOOL BitMask::TestAnyBit()
{
    LEAF_CONTRACT;

    if (IsArray())
    {
        COUNT_T *mask = m_maskArray+1;
        COUNT_T *maskEnd = mask + m_maskArray[0];

        while (mask < maskEnd)
        {
            if (*mask != 0)
                return TRUE;
            mask++;
        }

        return FALSE;
    }
    else
        return m_mask != (COUNT_T) 1;
}

inline void BitMask::ClearAllBits()
{
    LEAF_CONTRACT;

    if (IsArray())
        delete [] m_maskArray;

    m_mask = 1;
}

inline void **BitMask::GetAllocatedBlockPtr()
{
    LEAF_CONTRACT;

	if (IsArray())
        return (void **) &m_maskArray;
    else
        return NULL;
}

inline void *BitMask::GetAllocatedBlock()
{
    LEAF_CONTRACT;

	if (IsArray())
        return m_maskArray;
    else
        return NULL;
}

inline COUNT_T BitMask::GetAllocatedBlockSize()
{
    LEAF_CONTRACT;

	if (IsArray())
        return (GetMaskArraySize()+1) * sizeof(COUNT_T);
    else
        return 0;
}

#endif // _BITMASK_INL_

