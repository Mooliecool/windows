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
/*============================================================
**
** Class: COMMemoryFailPoint
**
** Purpose: Native methods for System.Runtime.MemoryFailPoint.
** These are to implement memory gates to limit allocations
** when progress will likely result in an OOM.
**
===========================================================*/
#include "common.h"

#include "frames.h"
#include "commemoryfailpoint.h"

// Need to know the maximum segment size for both the normal GC heap and the
// large object heap, as well as the top user-accessible address within the
// address space (ie, theoretically 2^31 - 1 on a 32 bit machine, but a tad 
// lower in practice).  This will help out with 32 bit machines running in 
// 3 GB mode.
FCIMPL2(void, COMMemoryFailPoint::GetMemorySettings, UINT32* pMaxGCSegmentSize, UINT64* pTopOfMemory)
{
    WRAPPER_CONTRACT;

    GCHeap * pGC = GCHeap::GetGCHeap();
    size_t segment_size = pGC->GetValidSegmentSize(FALSE);
    size_t large_segment_size = pGC->GetValidSegmentSize(TRUE);
    _ASSERTE(segment_size < 0xffffffff && large_segment_size < 0xffffffff);
    if (segment_size > large_segment_size)
        *pMaxGCSegmentSize = (UINT32) segment_size;
    else
        *pMaxGCSegmentSize = (UINT32) large_segment_size;

    // GetTopMemoryAddress returns a void*, which can't be cast
    // directly to a UINT64 without causing an error from GCC.
    void * topOfMem = GetTopMemoryAddress();
    *pTopOfMemory = (UINT64) (size_t) topOfMem;
}
FCIMPLEND
