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
//*****************************************************************************
// File: request.cpp
//
// CorDataAccess::Request implementation.
//
//*****************************************************************************

#include "stdafx.h"

#ifndef GC_SMP

#include <sigformat.h>
#include <win32threadpool.h>

#include <gceesvr.cpp>


int GCHeapCount()
{
    return SVR::gc_heap::n_heaps;
}

HRESULT GetServerHeapData(CLRDATA_ADDRESS addr, DacpHeapSegmentData *pSegment)
{
    if (!addr)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    SVR::heap_segment *pHeapSegment =
        __DPtr<SVR::heap_segment>(TO_TADDR(addr));

    pSegment->segmentAddr = addr;
    pSegment->allocated = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->allocated;
    pSegment->committed = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->committed;
    pSegment->reserved = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->reserved;
    pSegment->used = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->used;
    pSegment->mem = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->mem;
    pSegment->next = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->next;
    pSegment->gc_heap = (CLRDATA_ADDRESS)(ULONG_PTR) pHeapSegment->heap;

    return S_OK;
}

HRESULT GetServerHeaps(CLRDATA_ADDRESS pGCHeaps[], ICLRDataTarget* pTarget)
{
    TADDR ptr = SVR::gc_heap::g_heaps.GetAddr();
    ULONG32 bytesRead = 0;    
    CLRDATA_ADDRESS pDataAddr = (CLRDATA_ADDRESS) ptr;
    
    for (int i=0;i<GCHeapCount();i++)
    {
        
        LPVOID pGCHeapAddr;
        if (pTarget->ReadVirtual(pDataAddr + (sizeof(LPVOID)*i),
                                 (PBYTE) &pGCHeapAddr, sizeof(LPVOID),
                                 &bytesRead) != S_OK)
        {
            return E_FAIL;
        }
        if (bytesRead != sizeof(LPVOID))
        {
            return E_FAIL;
        }
        pGCHeaps[i] = (CLRDATA_ADDRESS)(ULONG_PTR) pGCHeapAddr;
    }
    return S_OK;
}

#define PTR_CDADDR(ptr)   TO_CDADDR(PTR_TO_TADDR(ptr))
#define HOST_CDADDR(host) TO_CDADDR(PTR_HOST_TO_TADDR(host))

typedef DPTR(class SVR::gc_heap)                        PTR_SVR_gc_heap;

HRESULT ClrDataAccess::ServerGCHeapDetails(CLRDATA_ADDRESS heapAddr, DacpGcHeapDetails *detailsData)
{    
    if (!heapAddr)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    SVR::gc_heap *pHeap = PTR_SVR_gc_heap(TO_TADDR(heapAddr));
    int i;

    detailsData->heapAddr = heapAddr;
    
    detailsData->lowest_address = PTR_CDADDR(g_lowest_address);
    detailsData->highest_address = PTR_CDADDR(g_highest_address);
    detailsData->card_table = PTR_CDADDR(g_card_table);
    
    detailsData->alloc_allocated = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->alloc_allocated;
    detailsData->ephemeral_heap_segment = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->ephemeral_heap_segment;
    for (i=0;i<NUMBERGENERATIONS;i++)
    {
        detailsData->generation_table[i].start_segment = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->generation_table[i].start_segment;
        detailsData->generation_table[i].allocation_start = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->generation_table[i].allocation_start;        
        detailsData->generation_table[i].allocContextPtr = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->generation_table[i].allocation_context.alloc_ptr;
        detailsData->generation_table[i].allocContextLimit = (CLRDATA_ADDRESS)(ULONG_PTR) pHeap->generation_table[i].allocation_context.alloc_limit;
    }

    TADDR pFillPointerArray = TO_TADDR(pHeap->finalize_queue) + offsetof(SVR::CFinalize,m_FillPointers);
    for(i=0;i<(NUMBERGENERATIONS+SVR::CFinalize::ExtraSegCount);i++)
    {
        ULONG32 returned = sizeof(size_t);    
        size_t pValue;
        m_target->ReadVirtual(pFillPointerArray+(i*sizeof(size_t)), (PBYTE)&pValue, sizeof(size_t),&returned);    
        if (returned != sizeof(size_t))
        {
            return E_FAIL;
        }

        detailsData->finalization_fill_pointers[i] = (CLRDATA_ADDRESS) pValue;
    }

    return S_OK;
}

void
ClrDataAccess::EnumSvrGlobalMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    SVR::gc_heap::n_heaps.EnumMem();
    DacEnumMemoryRegion(SVR::gc_heap::g_heaps.GetAddr(),
                    sizeof(TADDR) * SVR::gc_heap::n_heaps);

    SVR::gc_heap::g_heaps.EnumMem();
    
    for (int i=0;i<SVR::gc_heap::n_heaps;i++)
    {
        SVR::gc_heap *pHeap = PTR_SVR_gc_heap(SVR::gc_heap::g_heaps[i]);

        DacEnumMemoryRegion(PTR_HOST_TO_TADDR(pHeap), sizeof(SVR::gc_heap));
        DacEnumMemoryRegion((TADDR)pHeap->finalize_queue,sizeof(SVR::CFinalize));

        // enumerating the generations from max (which is normally gen2) to max+1 gives you
        // the segment list for all the normal segements plus the large heap segment (max+1)
        // this is the convention in the GC so it is repeated here
        for (ULONG i = GCHeap::GetMaxGeneration(); i <= GCHeap::GetMaxGeneration()+1; i++)
        {
            __DPtr<SVR::heap_segment> seg = (TADDR)pHeap->generation_table[i].start_segment;
            while (seg)
            {
                    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(seg), sizeof(SVR::heap_segment));

                    seg = (TADDR)(seg->next);
            }
        }       
    }
}

#endif // GC_SMP
