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
// PriorityQueue.h:  PriorityQueue or "Heap"
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// PriorityQueue is a relatively efficient way to store a set of prioritized
// elements, which provides fast operations for adding a new element, or extracting
// the highest prioirty element.
//
// Note that this is sometimes called a "heap" in literature.
// --------------------------------------------------------------------------------

#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

#include "clrtypes.h"
#include "sarray.h"
#include "check.h"

// ================================================================================
// PriorityQueue
// ================================================================================

template <typename ELEMENT, COUNT_T ALLOC = 0>
class PriorityQueue
{
  private:
    InlineSArray<ELEMENT, ALLOC> m_array;

    // Node traversal

    COUNT_T Parent(COUNT_T index);
    COUNT_T ChildLeft(COUNT_T index);
    // ChildRight is ChildLeft + 1
    COUNT_T Sibling(COUNT_T index);

    // Invariants

    CHECK ElementInvariant(COUNT_T index);
    CHECK IncrementalInvariant(COUNT_T index);
    CHECK Invariant();

    // Utility routines

    void Swap(COUNT_T index1, COUNT_T index2);

    void SortUp(COUNT_T index);
    void FillGapDown(COUNT_T index);

  public:

    BOOL IsEmpty() const;
    const ELEMENT &Peek() const;

    void Add(const ELEMENT &e);
    ELEMENT Remove();
};

// ================================================================================
// Inline definitions
// ================================================================================

#include "priorityqueue.inl"

#endif  // _PRIORITYQUEUE_H_
