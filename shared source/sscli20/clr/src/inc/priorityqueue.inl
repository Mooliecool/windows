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
// PriorityQueue.inl:  PriorityQueue or "Heap"
// --------------------------------------------------------------------------------

#ifndef _PRIORITYQUEUE_INL_
#define _PRIORITYQUEUE_INL_

#include "priorityqueue.h"
#include "check.h"
#include "contract.h"

template <typename ELEMENT, COUNT_T ALLOC>
COUNT_T PriorityQueue<ELEMENT, ALLOC>::Parent(COUNT_T index)
{
    CONTRACT(COUNT_T)
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(index > 0);
        PRECONDITION(index < m_array.GetCount());
        POSTCONDITION(ChildLeft(RETVAL) == index || ChildLeft(RETVAL)+1 == index);
    }
    CONTRACT_END;


    RETURN (index-1)>>1;
}

// ChildRight is ChildLeft + 1
template <typename ELEMENT, COUNT_T ALLOC>
COUNT_T PriorityQueue<ELEMENT, ALLOC>::ChildLeft(COUNT_T index)
{
    CONTRACT(COUNT_T)
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(index < m_array.GetCount());
        POSTCONDITION(Parent(RETVAL) == index);
    }
    CONTRACT_END;

    RETURN (index<<1)+1;
}

template <typename ELEMENT, COUNT_T ALLOC>
COUNT_T PriorityQueue<ELEMENT, ALLOC>::Sibling(COUNT_T index)
{
    CONTRACT(COUNT_T)
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(index > 0);
        PRECONDITION(index < m_array.GetCount());
        POSTCONDITION(index != RETVAL);
        POSTCONDITION(Parent(RETVAL) == Parent(index));
    }
    CONTRACT_END;

    return (((index+1)^1)-1);
}

template <typename ELEMENT, COUNT_T ALLOC>
void PriorityQueue<ELEMENT, ALLOC>::Add(const ELEMENT &e)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(!IsEmpty());
    }
    CONTRACT_END;

    (*m_array.Append()) = e;
    SortUp(m_array.GetCount()-1);

    RETURN;
}

template <typename ELEMENT, COUNT_T ALLOC>
BOOL PriorityQueue<ELEMENT, ALLOC>::IsEmpty() const
{
    WRAPPER_CONTRACT;

    return m_array.GetCount() == 0;
}

template <typename ELEMENT, COUNT_T ALLOC>
const ELEMENT &PriorityQueue<ELEMENT, ALLOC>::Peek() const
{
    LEAF_CONTRACT;

    return m_array[0];
}

template <typename ELEMENT, COUNT_T ALLOC>
ELEMENT PriorityQueue<ELEMENT, ALLOC>::Remove()
{
    WRAPPER_CONTRACT;

    ELEMENT temp = m_array[0];
    FillGapDown(0);
    return temp;
}

template <typename ELEMENT, COUNT_T ALLOC>
CHECK PriorityQueue<ELEMENT, ALLOC>::ElementInvariant(COUNT_T index)
{
    WRAPPER_CONTRACT;

    if (index > 0)
        CHECK(m_array[Parent(index)].Priority() >= m_array[index].Priority());

    COUNT_T left = ChildLeft(index);
    if (left < m_array.GetCount())
        CHECK(m_array[index].Priority() >= m_array[left].Priority());            

    COUNT_T right = left+1;
    if (right < m_array.GetCount())
        CHECK(m_array[index].Priority() >= m_array[right].Priority());            

    CHECK_OK;
}

template <typename ELEMENT, COUNT_T ALLOC>
CHECK PriorityQueue<ELEMENT, ALLOC>::IncrementalInvariant(COUNT_T index)
{
    WRAPPER_CONTRACT;

    while (TRUE)
    {
        CHECK(ElementInvariant(index));
        if (index == 0)
            break;
        index = Parent(index);
    }

    CHECK_OK;
}

template <typename ELEMENT, COUNT_T ALLOC>
CHECK PriorityQueue<ELEMENT, ALLOC>::Invariant()
{
    for (COUNT_T i = 0; i < m_array.GetCount(); i++)
    {
        CHECK(ElementInvariant(index));
    }

    CHECK_OK;
}

template <typename ELEMENT, COUNT_T ALLOC>
void PriorityQueue<ELEMENT, ALLOC>::Swap(COUNT_T index1, COUNT_T index2)
{
    LEAF_CONTRACT;

    ELEMENT temp = m_array[index1];
    m_array[index1] = m_array[index2];
    m_array[index2] = temp;
}


template <typename ELEMENT, COUNT_T ALLOC>
void PriorityQueue<ELEMENT, ALLOC>::SortUp(COUNT_T index)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(IncrementalInvariant(index));
    }
    CONTRACT_END;

    COUNT_T i = index;

    while (i > 0)
    {
        COUNT_T parent = Parent(i);

        if (m_array[parent].Priority() >= m_array[i].Priority())
            break;

        Swap(i, parent);

        i = parent;
    }

    RETURN;
}

template <typename ELEMENT, COUNT_T ALLOC>
void PriorityQueue<ELEMENT, ALLOC>::FillGapDown(COUNT_T index)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(IncrementalInvariant(index));
    }
    CONTRACT_END;

    while (TRUE)
    {
        COUNT_T left = ChildLeft(index);
        COUNT_T right = left+1;

        if (right >= m_array.GetCount())
        {
            // End of tree. We need to shrink the size of the array by one.
            // Take the last element and put it in the gap we are filling down.
            // Then sort it back up as necessary.

            // Check for the special case that we got lucky and the gap was already the
            // last element.
            if (index != m_array.GetCount()-1)
            {
                m_array[index] = m_array[m_array.GetCount()-1];
                SortUp(index);
            }

            m_array.SetCount(m_array.GetCount()-1);

            break;
        }
        else if (m_array[left].Priority() > m_array[right].Priority())
        {
            Swap(index, left);

            index = left;
        }
        else
        {
            Swap(index, right);

            index = right;
        }

    }

    RETURN;
}

#endif  // _PRIORITYQUEUE_INL_
