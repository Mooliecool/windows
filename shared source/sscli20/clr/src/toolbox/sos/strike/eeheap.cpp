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
#include <assert.h>
#include "strike.h"
#include "util.h"
#include "safemath.h"


#define CCH_STRING_PREFIX_SUMMARY 64

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to update GC heap statistics.             *  
*                                                                      *
\**********************************************************************/
void HeapStat::Add(DWORD_PTR aData, DWORD aSize)
{
    if (head == 0)
    {
        head = (Node*)malloc(sizeof(Node));
        if (head == NULL)
        {
            ReportOOM();
            ControlC = TRUE;
            return;
        }
        head = new (head) Node;
        
        if (bHasStrings)
        {
            size_t capacity_pNew = wcslen((wchar_t*)aData) + 1;
            wchar_t *pNew = (wchar_t*)malloc(capacity_pNew*sizeof(wchar_t));
            if (pNew == NULL)
            {
               ReportOOM();               
               ControlC = TRUE;
               return;
            }
            wcscpy_s(pNew, capacity_pNew, (wchar_t*)aData);
            aData = (DWORD_PTR)pNew;            
        }

        head->data = aData;
    }
    Node *walk = head;
    int cmp = 0;

    for (;;)
    {
        if (IsInterrupt())
            return;
        
        cmp = CompareData(aData, walk->data);            

        if (cmp == 0)
            break;
        
        if (cmp < 0)
        {
            if (walk->left == NULL)
                break;
            walk = walk->left;
        }
        else
        {
            if (walk->right == NULL)
                break;
            walk = walk->right;
        }
    }

    if (cmp == 0)
    {
        walk->count ++;
        walk->totalSize += aSize;
    }
    else
    {
        Node *node = (Node*)malloc(sizeof(Node));
        if (node == NULL)
        {
            ReportOOM();                
            ControlC = TRUE;
            return;
        }

        if (bHasStrings)
        {
            size_t capacity_pNew = wcslen((wchar_t*)aData) + 1;
            wchar_t *pNew = (wchar_t*)malloc(capacity_pNew*sizeof(wchar_t));
            if (pNew == NULL)
            {
               ReportOOM();
               ControlC = TRUE;
               return;
            }
            wcscpy_s(pNew, capacity_pNew, (wchar_t*)aData);
            aData = (DWORD_PTR)pNew;            
        }
        
        node = new (node) Node;
        node->data = aData;       
        node->totalSize = aSize;
        node->count ++;

        if (cmp < 0)
        {
            walk->left = node;
        }
        else
        {
            walk->right = node;
        }
    }
}
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function compares two nodes in the tree.     *  
*                                                                      *
\**********************************************************************/
int HeapStat::CompareData(DWORD_PTR d1, DWORD_PTR d2)
{
    if (bHasStrings)
        return wcscmp((wchar_t*)d1, (wchar_t*)d2);

    if (d1 > d2)
        return 1;

    if (d1 < d2)
        return -1;

    return 0;   
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to sort all entries in the heap stat.     *  
*                                                                      *
\**********************************************************************/
void HeapStat::Sort ()
{
    Node *root = head;
    head = NULL;
    ReverseLeftMost (root);

    Node *sortRoot = NULL;
    while (head)
    {
        Node *tmp = head;
        head = head->left;
        if (tmp->right)
            ReverseLeftMost (tmp->right);
        // add tmp
        tmp->right = NULL;
        tmp->left = NULL;
        SortAdd (sortRoot, tmp);
    }
    head = sortRoot;

    // Change binary tree to a linear tree
    root = head;
    head = NULL;
    ReverseLeftMost (root);
    sortRoot = NULL;
    while (head)
    {
        Node *tmp = head;
        head = head->left;
        if (tmp->right)
            ReverseLeftMost (tmp->right);
        // add tmp
        tmp->right = NULL;
        tmp->left = NULL;
        LinearAdd (sortRoot, tmp);
    }
    head = sortRoot;

    //reverse the order
    root = head;
    head = NULL;
    sortRoot = NULL;
    while (root)
    {
        Node *tmp = root->right;
        root->left = NULL;
        root->right = NULL;
        LinearAdd (sortRoot, root);
        root = tmp;
    }
    head = sortRoot;
}

void HeapStat::ReverseLeftMost (Node *root)
{
    while (root)
    {
        Node *tmp = root->left;
        root->left = head;
        head = root;
        root = tmp;
    }
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to help to sort heap stat.                *  
*                                                                      *
\**********************************************************************/
void HeapStat::SortAdd (Node *&root, Node *entry)
{
    if (root == NULL)
    {
        root = entry;
    }
    else
    {
        Node *parent = root;
        Node *ptr = root;
        while (ptr)
        {
            parent = ptr;
            if (ptr->totalSize < entry->totalSize)
                ptr = ptr->right;
            else
                ptr = ptr->left;
        }
        if (parent->totalSize < entry->totalSize)
            parent->right = entry;
        else
            parent->left = entry;
    }
}

void HeapStat::LinearAdd(Node *&root, Node *entry)
{
    if (root == NULL)
    {
        root = entry;
    }
    else
    {
        entry->right = root;
        root = entry;
    }
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to print GC heap statistics.              *  
*                                                                      *
\**********************************************************************/
void HeapStat::Print()
{
    dprintf("Statistics:\n");
    if (bHasStrings)
        dprintf("%8s %12s %s\n", "Count", "TotalSize", "String Value");
    else
        dprintf("%" POINTERSIZE "s %8s %12s %s\n","MT", "Count", "TotalSize", "Class Name");

    Node *root = head;
    int ncount = 0;
    while (root)
    {
        if (IsInterrupt())
            return;
        
        ncount += root->count;

        if (bHasStrings)
        {
            dprintf ("%8d %12" POINTERSIZE_TYPE "u \"%S\"\n", root->count, root->totalSize, root->data);
        }
        else
        {
            dprintf ("%p %8d %12" POINTERSIZE_TYPE "u ", (ULONG64) root->data, root->count, root->totalSize);
            if (IsMTForFreeObj(root->data))
            {
                dprintf ("%9s\n","Free");
            }
            else
            {
                wcscpy_s (g_mdName, mdNameLen, L"UNKNOWN");
                NameForMT_s ((DWORD_PTR) root->data, g_mdName, mdNameLen);
                dprintf ("%S\n", g_mdName);
            }
        }
        root = root->right;
        
    }
    dprintf ("Total %d objects\n", ncount);
}

void HeapStat::Delete()
{
    Node *root = head;
    head = NULL;
    ReverseLeftMost (root);

    while (root)
    {
        Node *tmp = root;
        root = root->left;
        if (tmp->right)
            ReverseLeftMost (tmp->right);
        // free tmp
        if (bHasStrings)
            free((void*)tmp->data);
        free (tmp);
    }

    // return to default state
    bHasStrings = FALSE;
}

// -----------------------------------------------------------------------
//
// MethodTableCache implementation
//
// Used during heap traversals for quick object size computation
//
BOOL MethodTableCache::Lookup (DWORD_PTR aData, DWORD& BaseSize, DWORD& ComponentSize, BOOL& bContainsPointers)
{
    if (head == 0)
    {
        return FALSE;
    }

    Node *walk = head;
    int cmp = 0;

    for (;;)
    {
        if (IsInterrupt())
            return FALSE;        

        cmp = CompareData(aData, walk->data);            

        if (cmp == 0)
            break;
        
        if (cmp < 0)
        {
            if (walk->left == NULL)
                break;
            walk = walk->left;
        }
        else
        {
            if (walk->right == NULL)
                break;
            walk = walk->right;
        }
    }

    if (cmp == 0)
    {
        BaseSize = walk->BaseSize;
        ComponentSize = walk->ComponentSize;
        bContainsPointers = walk->bContainsPointers;
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to update GC heap statistics.             *  
*                                                                      *
\**********************************************************************/
void MethodTableCache::Add(DWORD_PTR aData, DWORD BaseSize, DWORD ComponentSize, BOOL bContainsPointers)
{
    if (head == 0)
    {
        head = (Node*)malloc(sizeof(Node));
        if (head == NULL)
        {
            ReportOOM();
            ControlC = TRUE;
            return;
        }
        head = new (head) Node;        
        head->data = aData;
        head->BaseSize = BaseSize;
        head->ComponentSize = ComponentSize;
        head->bContainsPointers = bContainsPointers;

    }
    Node *walk = head;
    int cmp = 0;

    for (;;)
    {
        if (IsInterrupt())
            return;

        cmp = CompareData(aData, walk->data);            

        if (cmp == 0)
            break;
        
        if (cmp < 0)
        {
            if (walk->left == NULL)
                break;
            walk = walk->left;
        }
        else
        {
            if (walk->right == NULL)
                break;
            walk = walk->right;
        }
    }

    if (cmp != 0)
    {
        Node *node = (Node*)malloc(sizeof(Node));
        if (node == NULL)
        {
            ReportOOM();            
            ControlC = TRUE;
            return;
        }
        
        node = new (node) Node;
        node->data = aData;       
        node->BaseSize = BaseSize;
        node->ComponentSize = ComponentSize;
        node->bContainsPointers = bContainsPointers;
        
        if (cmp < 0)
        {
            walk->left = node;
        }
        else
        {
            walk->right = node;
        }
    }
}
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function compares two nodes in the tree.     *  
*                                                                      *
\**********************************************************************/
int MethodTableCache::CompareData(DWORD_PTR d1, DWORD_PTR d2)
{
    if (d1 > d2)
        return 1;

    if (d1 < d2)
        return -1;

    return 0;   
}

void MethodTableCache::ReverseLeftMost (Node *root)
{
    if (root)
    {
        if (root->left) ReverseLeftMost(root->left);
        if (root->right) ReverseLeftMost(root->right);
        free (root);
    }
}

void MethodTableCache::Clear()
{
    Node *root = head;
    head = NULL;
    ReverseLeftMost (root);
}

// -----------------------------------------------------------------------
//
// Fragmentation report infrastructure
//
FragmentationBlock *g_pBlocks = NULL;
int g_BlocksLength = 0;

#define BLOCK_INCREMENTS 100
int BlocksAllocated = 0;

BOOL InitializeBlocks()
{
    g_BlocksLength = 0;
    if (g_pBlocks == NULL)
    {
        g_pBlocks = (FragmentationBlock *) malloc(sizeof(FragmentationBlock) * BLOCK_INCREMENTS);
        if (g_pBlocks == NULL)
        {
            ReportOOM();            
            return FALSE;
        }
        
        BlocksAllocated = BLOCK_INCREMENTS;
    }
    return TRUE;
}

BOOL AddBlock(FragmentationBlock *pBlock)
{
    if (g_pBlocks == NULL)
    {
        // We didn't have enough memory for this report
        return FALSE;
    }

    if (g_BlocksLength == BlocksAllocated)
    {
        FragmentationBlock *pNewBlocks = (FragmentationBlock *) malloc(sizeof(FragmentationBlock) * (BlocksAllocated + BLOCK_INCREMENTS));
        if (pNewBlocks == NULL)
        {
            return FALSE;
        }

        memcpy(pNewBlocks, g_pBlocks, sizeof(FragmentationBlock) * BlocksAllocated);
        BlocksAllocated += BLOCK_INCREMENTS;
        free(g_pBlocks);
        g_pBlocks = pNewBlocks;
    }

    g_pBlocks[g_BlocksLength++] = *pBlock;        
    return TRUE;
}

//Alignment constant for allocation
#ifdef _X86_
#define ALIGNCONST 3
#else
#define ALIGNCONST 7
#endif

//The large object heap uses a different alignment
#define ALIGNCONSTLARGE 7


#define SIZEOF_OBJHEADER    4
 
#define plug_skew           SIZEOF_OBJHEADER
#define min_obj_size        (sizeof(BYTE*)+plug_skew+sizeof(size_t))
size_t Align (size_t nbytes)
{
    return (nbytes + ALIGNCONST) & ~ALIGNCONST;
}

size_t AlignLarge(size_t nbytes)
{
    return (nbytes + ALIGNCONSTLARGE) & ~ALIGNCONSTLARGE;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Print the gc heap info.                                           *  
*                                                                      *
\**********************************************************************/
void GCPrintGenerationInfo(DacpGcHeapDetails &heap)
{
    UINT n;
    for (n = 0; n <= GetMaxGeneration(); n ++)
    {
        if (IsInterrupt())
            return;
        dprintf ("generation %d starts at 0x%p\n",
                 n, (ULONG64)heap.generation_table[n].allocation_start);
    }

    // We also need to look at the gen0 alloc context.
    dprintf ("ephemeral segment allocation context: ");
    if (heap.generation_table[0].allocContextPtr)
    {
        dprintf ("(0x%p, 0x%p)\n",
            (ULONG64) heap.generation_table[0].allocContextPtr,
            (ULONG64) (heap.generation_table[0].allocContextLimit + Align(min_obj_size)));       
    }
    else
    {
        dprintf ("none\n");
    }
}



void GCPrintLargeHeapSegmentInfo(DacpGcHeapDetails &heap, DWORD_PTR &total_size)
{
    DWORD_PTR dwAddrSeg;
    DacpHeapSegmentData segment;
    int n;
    dwAddrSeg = (DWORD_PTR)heap.generation_table[GetMaxGeneration()+1].start_segment;

    // total_size = 0;
    n = 0;
    while (dwAddrSeg != NULL)
    {
        if (IsInterrupt())
            return;
        if (segment.Request(g_clrData, dwAddrSeg, heap) != S_OK)
        {
            dprintf("Error requesting heap segment %p\n",(ULONG64)dwAddrSeg);
            return;
        }
        dprintf ("%p %p  %p 0x%p(%d)\n", (ULONG64)dwAddrSeg,
                 (ULONG64)segment.mem, (ULONG64)segment.allocated,
                 (ULONG64)(segment.allocated - segment.mem),
                 segment.allocated - segment.mem);
        total_size += (DWORD_PTR) (segment.allocated - segment.mem);
        dwAddrSeg = (DWORD_PTR)segment.next;
        n ++;
        if (n > 100)
            break;
    }
}


BOOL TypeNameSubstrMatch(DWORD_PTR dwAddrMethTable,const WCHAR *TypeName)
{
    NameForMT_s (dwAddrMethTable, g_mdName, mdNameLen);
    return (wcsstr(g_mdName,TypeName) != NULL);
}
    
// PRECONDITION: dwAddrArrayObj is a System.Object[] pointer
BOOL ArrayElementSubstrMatch(DWORD_PTR dwAddrArrayObj, const WCHAR *TypeName)
{    
    // Get the object, get the methodtable of internal type.
    // Get the string name of that method table and compare.
    DacpObjectData objdata;

    if (FAILED(objdata.Request(g_clrData, dwAddrArrayObj)))
    {
        return FALSE;
    }

    NameForMT_s ((DWORD_PTR)objdata.ElementTypeHandle, g_mdName, mdNameLen);
    wcscat (g_mdName, L"[]");
    return (wcsstr(g_mdName, TypeName) != NULL);
}

BOOL HasThinLock(DWORD_PTR objAddr, ThinLockInfo *pOut)
{
    DWORD headerVal = 0;

    if (FAILED(MOVE(headerVal, objAddr-4)))
    {
        ExtOut("Unable to read memory at %p\n",(ULONG64)objAddr-4);
        return FALSE;
    }
            
    if (headerVal & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_SPIN_LOCK))
    {
        return FALSE;
    }

    pOut->ThreadId = headerVal & SBLK_MASK_LOCK_THREADID;
    pOut->Recursion = (headerVal & SBLK_MASK_LOCK_RECLEVEL) >> SBLK_RECLEVEL_SHIFT;

    if (pOut->ThreadId)
    {
        if (DacpThreadData::ThreadFromThinlockId (g_clrData, pOut->ThreadId, 
            &(pOut->threadPtr)) != S_OK)
        {
            pOut->threadPtr = NULL;
        }                
    }
    
    return pOut->ThreadId;
}

MethodTableCache g_special_mtCache;

DWORD GetNumComponents(DWORD_PTR addr)
{
    // For an object pointer, this attempts to read the number of
    // array components.
    addr+=sizeof(size_t);    
    DWORD Value = NULL;
    MOVE(Value, addr);
    return Value;
}

BOOL GetSizeEfficient(DWORD_PTR dwAddrCurrObj, 
    DWORD_PTR dwAddrMethTable, BOOL bLarge, size_t& s, BOOL& bContainsPointers)
{
    DWORD bs, cs;
    
    // Remove lower bits in case we are in mark phase
    dwAddrMethTable = dwAddrMethTable & ~3;
    if (!(g_special_mtCache.Lookup (dwAddrMethTable, bs, cs, bContainsPointers)))
    {
        DacpMethodTableData dmtd;
        if (dmtd.Request(g_clrData,dwAddrMethTable) != S_OK)
        {
            return FALSE;
        }

        bs = dmtd.BaseSize;
        cs = dmtd.ComponentSize;
        bContainsPointers = dmtd.bContainsPointers;
        g_special_mtCache.Add (dwAddrMethTable, bs, cs, bContainsPointers);
    }
        
    s = bs;
    if (cs)
    {
        s += cs*GetNumComponents(dwAddrCurrObj);
    }            
    s = (bLarge ? AlignLarge(s) : Align (s));
    return TRUE;
}

// This function expects stat to be valid, and ready to get statistics.
void GatherOneHeapFinalization(DacpGcHeapDetails& heapDetails, HeapStat *stat)
{
    DWORD_PTR dwAddr=0;    
    UINT m;

    for (m = 0; m <= GetMaxGeneration(); m ++)
    {
        if (IsInterrupt())
        {
            return;
        }
        
        ExtOut ("generation %d has %d finalizable objects ",
            m, 
            (SegQueueLimit(heapDetails,gen_segment(m)) - SegQueue(heapDetails,gen_segment(m))) / sizeof(size_t));
        
        ExtOut ("(%p->%p)\n",
            (ULONG64) SegQueue(heapDetails,gen_segment(m)),
            (ULONG64) SegQueueLimit(heapDetails,gen_segment(m)));    
    }

    ExtOut ("Ready for finalization %d objects ",
            (SegQueueLimit(heapDetails,FinalizerListSeg)-SegQueue(heapDetails,CriticalFinalizerListSeg)) / sizeof(size_t));
    ExtOut ("(%p->%p)\n",                    
            (ULONG64) SegQueue(heapDetails,CriticalFinalizerListSeg),
            (ULONG64) SegQueueLimit(heapDetails,FinalizerListSeg));            

    for (dwAddr = (DWORD_PTR)SegQueue(heapDetails, gen_segment(2));
         dwAddr < (DWORD_PTR)SegQueueLimit(heapDetails, FinalizerListSeg);
         dwAddr += sizeof (dwAddr)) 
    {
        if (IsInterrupt())
        {
            return;
        }
        
        DWORD_PTR objAddr, MTAddr=NULL;
        if (!FAILED(MOVE(objAddr, dwAddr)) && !FAILED(MOVE(MTAddr,objAddr)) && MTAddr) 
        {
            size_t s = ObjectSize (objAddr);
            stat->Add (MTAddr, (DWORD)s);
        }
    }
}


GCHeapSnapshot::GCHeapSnapshot() 
{ 
    m_isBuilt = FALSE; 
    m_heapDetails = NULL;    
}

///////////////////////////////////////////////////////////

SegmentLookup::SegmentLookup() 
{ 
    m_iSegmentsSize = m_iSegmentCount = 0; 

    m_segments = new DacpHeapSegmentData[100];
    if (m_segments == NULL)
    {
        ReportOOM();
    }
    else 
    {
        m_iSegmentsSize = 100;
    }
}

BOOL SegmentLookup::AddSegment(DacpHeapSegmentData *pData)
{
    if (m_iSegmentCount >= m_iSegmentsSize)
    {
        // expand buffer
        DacpHeapSegmentData *pNewBuffer = new DacpHeapSegmentData[m_iSegmentsSize+100];
        if (pNewBuffer==NULL)
            return FALSE;

        memcpy(pNewBuffer, m_segments, sizeof(DacpHeapSegmentData)*m_iSegmentsSize);
        
        m_iSegmentsSize+=100;
        m_segments = pNewBuffer;
    }
     
    m_segments[m_iSegmentCount++] = *pData;        
    
    return TRUE;
}

SegmentLookup::~SegmentLookup()
{
    if (m_segments)
    {
        delete [] m_segments;
        m_segments = NULL;
    }
}

void SegmentLookup::Clear()
{
    m_iSegmentCount = 0;
}

CLRDATA_ADDRESS SegmentLookup::GetHeap(CLRDATA_ADDRESS object, BOOL& bFound)
{
    CLRDATA_ADDRESS ret = NULL;
    bFound = FALSE;
    
    // Visit our segments
    for (int i=0; i<m_iSegmentCount; i++)
    {
        if (m_segments[i].mem <= object && m_segments[i].highAllocMark > object)
        {
            ret = m_segments[i].gc_heap;
            bFound = TRUE;
            break;
        }
    }    

    return ret;
}

///////////////////////////////////////////////////////////////////////////

BOOL GCHeapSnapshot::Build()
{    
    Clear();
    
    m_isBuilt = FALSE;
    if (m_gcheap.Request(g_clrData) != S_OK)
    {
        ExtOut("Error requesting GC Heap data\n");
        return FALSE;
    }

    CLRDATA_ADDRESS *heapAddrs = NULL;
    ToDestroy des((void **) &heapAddrs);
    if (m_gcheap.bServerMode)
    {
        UINT AllocSize;
        if (!ClrSafeInt<UINT>::multiply(sizeof(CLRDATA_ADDRESS), m_gcheap.HeapCount, AllocSize) ||
            (heapAddrs = (CLRDATA_ADDRESS*)malloc(AllocSize)) == NULL)
        {
            ReportOOM();                
            return FALSE;
        }

        if (DacpGcHeapData::GetHeaps(g_clrData, m_gcheap.HeapCount, heapAddrs) != S_OK)
        {
            ExtOut("Failed to get GCHeaps\n");
            return FALSE;
        }
    }

    // Get the heap too.
    m_heapDetails = new DacpGcHeapDetails[m_gcheap.HeapCount];

    if (m_heapDetails == NULL)
    {
        ReportOOM();        
        return FALSE;
    }
    
    for (UINT n = 0; n < m_gcheap.HeapCount; n ++)
    {        
        if (m_gcheap.bServerMode)
        {
            if (m_heapDetails[n].Request(g_clrData, heapAddrs[n]) != S_OK)
            {
                ExtOut("Error requesting details\n");
                return FALSE;
            }
        }
        else
        {
            if (m_heapDetails[n].Request(g_clrData) != S_OK)
            {
                ExtOut("Error requesting details\n");
                return FALSE;
            }
        }

        if (!AddSegments(m_heapDetails[n]))
        {
            ExtOut("Failed to retrieve segments for gc heap\n");
            return FALSE;
        }
    }

    m_isBuilt = TRUE;
    return TRUE; 
}

BOOL GCHeapSnapshot::AddSegments(DacpGcHeapDetails& details)
{
    int n = 0;
    DacpHeapSegmentData segment;
    CLRDATA_ADDRESS AddrSegs[] =
    {
        details.generation_table[GetMaxGeneration()].start_segment,
        details.generation_table[GetMaxGeneration()+1].start_segment
    };

    for (int i = 0; i < sizeof(AddrSegs)/sizeof(AddrSegs[0]); ++i)
    {
        CLRDATA_ADDRESS AddrSeg = AddrSegs[i];
        
        while (AddrSeg != NULL)
        {
            if (IsInterrupt())
            {
                return FALSE;
            }
            if (segment.Request(g_clrData, AddrSeg, details) != S_OK)
            {
                dprintf("Error requesting heap segment %p\n",(ULONG64)AddrSeg);
                return FALSE;
            }
            if (n++ > 1000) // that would be insane
            {
                ExtOut("More than 1000 heap segments, there must be an error\n");
                return FALSE;
            }
            if (!m_segments.AddSegment(&segment))
            {
                ExtOut("strike: Failed to store segment\n");
                return FALSE;
            }        
            AddrSeg = segment.next;
        }
    }
    
    return TRUE;
}

void GCHeapSnapshot::Clear()
{
    if (m_heapDetails != NULL)
    {
        delete [] m_heapDetails;
        m_heapDetails = NULL;
    }

    m_segments.Clear();
    
    m_isBuilt = FALSE;
}

GCHeapSnapshot g_snapshot;

DacpGcHeapDetails *GCHeapSnapshot::GetHeap(CLRDATA_ADDRESS objectPointer)
{
    // We need bFound because heap will be NULL if we are Workstation Mode.
    // We still need a way to know if the address was found in our segment 
    // list.
    BOOL bFound = FALSE;
    CLRDATA_ADDRESS heap = m_segments.GetHeap(objectPointer, bFound);
    if (heap)
    {
        for (UINT i=0; i<m_gcheap.HeapCount; i++)
        {
            if (m_heapDetails[i].heapAddr == heap)
                return m_heapDetails + i;
        }    
    }
    else if (!m_gcheap.bServerMode)
    {
        if (bFound)
        {
            return m_heapDetails;
        }
    }
    
    // Not found
    return NULL;
}

int GCHeapSnapshot::GetGeneration(CLRDATA_ADDRESS objectPointer)
{
    DacpGcHeapDetails *pDetails = GetHeap(objectPointer);
    if (pDetails == NULL)
    {
        ExtOut("Object %p has no generation\n", (ULONG64) objectPointer);
        return 0;
    }

    if (objectPointer >= pDetails->generation_table[0].allocation_start 
#ifndef GC_SMP
        && objectPointer <= pDetails->alloc_allocated 
#endif
        )
        return 0;

    if (objectPointer >= pDetails->generation_table[1].allocation_start &&
        objectPointer <= pDetails->generation_table[0].allocation_start)
        return 1;
    
    return 2;
}

DWORD_PTR g_trav_totalSize = 0;
DWORD_PTR g_trav_wastedSize = 0;

void LoaderHeapTraverse(CLRDATA_ADDRESS blockData,size_t blockSize,BOOL blockIsCurrentBlock)
{
    DWORD_PTR dwAddr1;
    DWORD_PTR curSize = 0;
    char ch;
    for (dwAddr1 = (DWORD_PTR)blockData;
         dwAddr1 < (DWORD_PTR)blockData + blockSize;
         dwAddr1 += OSPageSize())
    {
        if (IsInterrupt())
            break;
        if (SafeReadMemory(dwAddr1, &ch, sizeof(ch), NULL))
        {
            curSize += OSPageSize();
        }
        else
            break;
    }
    if (!blockIsCurrentBlock)
    {
        g_trav_wastedSize  += blockSize  - curSize;
    }
        
    g_trav_totalSize += curSize;
    ExtOut ("%p(%x", (ULONG64)blockData,blockSize);
    ExtOut (":%x) ", curSize);
}

DWORD_PTR LoaderHeapInfo (CLRDATA_ADDRESS pLoaderHeapAddr)
{
    g_trav_totalSize = 0; g_trav_wastedSize = 0;
    if (pLoaderHeapAddr)
    {
        DacpLoaderHeapTraverse::DoTraverse(g_clrData,pLoaderHeapAddr,LoaderHeapTraverse);
    }

    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize, g_trav_wastedSize );    
    return g_trav_totalSize;
}
    
DWORD_PTR JitHeapInfo ()
{
    // walk ExecutionManager__m_pJitList
    DacpJitListInfo jlInfo;

    if (jlInfo.Request(g_clrData) != S_OK)
    {
        dprintf("Unable to get JIT info\n");
        return 0;
    }

    DacpJitManagerInfo *pArray = new DacpJitManagerInfo[jlInfo.JitCount];
    if (pArray==NULL)
    {
        ReportOOM();        
        return 0;
    }

    if (DacpJitListInfo::GetJitManagers(g_clrData,jlInfo.JitCount,pArray) != S_OK)
    {
        ExtOut("Unable to get array of JIT Managers\n");
        delete [] pArray;
        return 0;
    }

    DWORD_PTR totalSize = 0;

    for (int n=0;n<jlInfo.JitCount;n++)
    {
        if (IsInterrupt())
            break;

#define   miManaged_IL_EJIT             (miMaxMethodImplVal + 1)

        if (IsMiIL(pArray[n].codeType)) // JIT
        {
            DacpJitHeapList jhl;
            jhl.eeJitManager = pArray[n].managerAddr;
            if (jhl.Request(g_clrData) != S_OK)
            {
                dprintf ("Error getting EEJitManager code heaps\n");
                break;
            }

            DacpJitCodeHeapInfo *codeHeapInfo = new DacpJitCodeHeapInfo[jhl.heapListCount];
            if (codeHeapInfo == NULL)
            {
                ReportOOM();                        
                break;
            }

            if (jhl.GetCodeHeaps(g_clrData,jhl.heapListCount,codeHeapInfo) != S_OK)
            {
                dprintf("Unable to get code heap info\n");
                delete [] codeHeapInfo;
                break;
            }

            for (int iHeaps = 0; iHeaps < jhl.heapListCount; iHeaps++)
            {
                if (IsInterrupt())
                    break;

                if (codeHeapInfo[iHeaps].codeHeapType == CODEHEAP_LOADER)
                {
                    dprintf ("LoaderCodeHeap: ");
                    totalSize += LoaderHeapInfo (codeHeapInfo[iHeaps].LoaderHeap);
                }
                else if (codeHeapInfo[iHeaps].codeHeapType == CODEHEAP_HOST)
                {
                    dprintf ("HostCodeHeap: ");
                    dprintf ("%p ", (ULONG64) codeHeapInfo[iHeaps].HostData.baseAddr);
                    DWORD dwSize = (DWORD) (codeHeapInfo[iHeaps].HostData.currentAddr - codeHeapInfo[iHeaps].HostData.baseAddr);
                    dprintf ("Size: 0x%x(%d)bytes.\n", dwSize, dwSize);
                    totalSize += dwSize;                    
                }
            }

            delete [] codeHeapInfo;
        }
        else if (!IsMiNative(pArray[n].codeType)) // ignore native heaps for now
        {
            dprintf ("Unknown Jit encountered, ignored\n");
        }
    }

    delete [] pArray;

    ExtOut ("Total size: 0x%x(%d)bytes\n", totalSize, totalSize);
    return totalSize;
}

DWORD_PTR VSDHeapInfo(CLRDATA_ADDRESS AppDomainPtr)
{
    if (!AppDomainPtr)
        return 0;
    DWORD_PTR totalSize = 0;
    g_trav_totalSize = 0; g_trav_wastedSize = 0;
    ExtOut("  IndcellHeap: ");
    DacpVirtualCallStubHeapTraverse::DoTraverse(g_clrData, AppDomainPtr, IndcellHeap, LoaderHeapTraverse);
    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("  Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize,  g_trav_wastedSize);    

    totalSize += g_trav_totalSize;    
    g_trav_totalSize = 0; g_trav_wastedSize = 0;

    ExtOut("  LookupHeap: ");
    DacpVirtualCallStubHeapTraverse::DoTraverse(g_clrData, AppDomainPtr, LookupHeap, LoaderHeapTraverse);
    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("  Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize,  g_trav_wastedSize);    

    totalSize += g_trav_totalSize;
    g_trav_totalSize = 0; g_trav_wastedSize = 0;

    ExtOut("  ResolveHeap: ");
    DacpVirtualCallStubHeapTraverse::DoTraverse(g_clrData, AppDomainPtr, ResolveHeap, LoaderHeapTraverse);
    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("  Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize,  g_trav_wastedSize);    

    totalSize += g_trav_totalSize;
    g_trav_totalSize = 0; g_trav_wastedSize = 0;

    ExtOut("  DispatchHeap: ");
    DacpVirtualCallStubHeapTraverse::DoTraverse(g_clrData, AppDomainPtr, DispatchHeap, LoaderHeapTraverse);
    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("  Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize,  g_trav_wastedSize);    

    totalSize += g_trav_totalSize;
    g_trav_totalSize = 0; g_trav_wastedSize = 0;

    ExtOut("  CacheEntryHeap: ");
    DacpVirtualCallStubHeapTraverse::DoTraverse(g_clrData, AppDomainPtr, CacheEntryHeap, LoaderHeapTraverse);
    ExtOut ("Size: 0x%x(%lu)bytes.\n", g_trav_totalSize, g_trav_totalSize);
    if (g_trav_wastedSize )
        ExtOut ("  Wasted: 0x%x(%lu)bytes.\n", g_trav_wastedSize,  g_trav_wastedSize);    
    
    return totalSize;
}

