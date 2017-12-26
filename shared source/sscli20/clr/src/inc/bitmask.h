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
// BitMask.h
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// BitMask is an arbitrarily large sized bitfield which has optimal storage 
// for 32 bits or less.  
// Storage is proportional to the highest index which is set. 
// --------------------------------------------------------------------------------

#include <clrtypes.h>

#ifndef _BITMASK_H_
#define _BITMASK_H_

class BitMask
{
 public:

    BitMask();
    ~BitMask();

    BOOL TestBit(int bit);
    void SetBit(int bit);
    void ClearBit(int bit);

    // returns true if any bit is set
    BOOL TestAnyBit();

    void ClearAllBits();

    // Allocation exposed for ngen save/fixup
    void **GetAllocatedBlockPtr();
    void *GetAllocatedBlock();
    COUNT_T GetAllocatedBlockSize();

 private:

    static const int BIT_SIZE_SHIFT = 5;
    static const int BIT_SIZE = (1<<BIT_SIZE_SHIFT);
    static const int BIT_SIZE_MASK = BIT_SIZE-1;

    static const COUNT_T MIN_ARRAY_ALLOCATION = 3;

    // The first bit is used to indicate whether we've got a flat mask or
    // an array of mask elements
    BOOL IsArray();

    // Indexing computations
    COUNT_T BitToIndex(int bit);
    COUNT_T BitToShift(int bit);

    // Generic mask array access.  Works for either case (array or non-array).
    COUNT_T *GetMaskArray();
    COUNT_T GetMaskArraySize();

    // Need more bits...
    void GrowArray(COUNT_T newSize);
    
    union
    {
        COUNT_T     m_mask;
        COUNT_T     *m_maskArray; // first array element is size of rest of array
    };
};

#include <bitmask.inl>

#endif // _BITMASK_H_
