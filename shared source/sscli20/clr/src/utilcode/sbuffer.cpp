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
// ---------------------------------------------------------------------------
// Buffer.cpp
// ---------------------------------------------------------------------------

#include "stdafx.h"
#include "sbuffer.h"
#include "ex.h"
#include "holder.h"
#include "stdmacros.h"

// Try to minimize the performance impact of contracts on CHK build.
#if defined(_MSC_VER)
#pragma inline_depth (20)
#endif

//----------------------------------------------------------------------------
// ReallocateBuffer
// Low level buffer reallocate routine
//----------------------------------------------------------------------------
void SBuffer::ReallocateBuffer(COUNT_T allocation, Preserve preserve)
{
    CONTRACT_VOID
    {
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckBufferClosed());
        PRECONDITION(CheckAllocation(allocation));
        PRECONDITION(allocation >= m_size);
        POSTCONDITION(m_allocation == allocation);
        if (allocation > 0) THROWS; else NOTHROW;
    } 
    CONTRACT_END;

    BYTE *newBuffer = NULL;
    if (allocation > 0)
    {
        newBuffer = NewBuffer(allocation);

        if (preserve == PRESERVE)
        {
            // Copy the relevant contents of the old buffer over
            DebugMoveBuffer(newBuffer, m_buffer, m_size);
        }
    }

    if (IsAllocated())
        DeleteBuffer(m_buffer, m_allocation);

    m_buffer = newBuffer;
    m_allocation = allocation;

    if (allocation > 0)
        SetAllocated();
    else
        ClearAllocated();

    ClearImmutable();

    RETURN;
}

void SBuffer::Replace(const Iterator &i, COUNT_T deleteSize, COUNT_T insertSize)
{
    CONTRACT_VOID 
    {
        THROWS;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i, deleteSize));
        THROWS;
    } 
    CONTRACT_END;

    COUNT_T startRange = (COUNT_T) (i.m_ptr - m_buffer);
    COUNT_T endRange = startRange + deleteSize;
    COUNT_T end = m_size;

    SCOUNT_T delta = insertSize - deleteSize;

    if (delta < 0)
    {
        // Buffer is shrinking

        DebugDestructBuffer(i.m_ptr, deleteSize);

        DebugMoveBuffer(m_buffer + endRange + delta,
                        m_buffer + endRange, 
                        end - endRange);

        Resize(m_size+delta, PRESERVE);

        i.Resync(this, m_buffer + startRange);

    }
    else if (delta > 0)
    {
        // Buffer is growing

        ResizePadded(m_size+delta);

        i.Resync(this, m_buffer + startRange);

        DebugDestructBuffer(i.m_ptr, deleteSize);

        DebugMoveBuffer(m_buffer + endRange + delta, 
                        m_buffer + endRange, 
                        end - endRange);

    }
    else
    {
        // Buffer stays the same size.  We need to DebugDestruct it first to keep
        // the invariant that the new space is clean.

        DebugDestructBuffer(i.m_ptr, insertSize);
    }

    DebugConstructBuffer(i.m_ptr, insertSize);

    RETURN;
}


