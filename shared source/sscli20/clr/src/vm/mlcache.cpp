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
// MLCACHE.CPP -
//
// Base class for caching ML stubs.
//

#include "common.h"
#include "mlcache.h"
#include "stublink.h"
#include "cgensys.h"
#include "excep.h"
#include "memoryreport.h"

//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
MLStubCache::MLStubCache(LoaderHeap *pHeap) :
    CClosedHashBase(
#ifdef _DEBUG
                      3,
#else
                      17,    // CClosedHashTable will grow as necessary
#endif                      

                      sizeof(MLCHASHENTRY),
                      FALSE
                   ),
    m_crst("MLCache", CrstMLCache, CRST_UNSAFE_ANYMODE),
    m_heap(pHeap)
{
    WRAPPER_CONTRACT;
}


//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
MLStubCache::~MLStubCache()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    MLCHASHENTRY *phe = (MLCHASHENTRY*)GetFirst();
    while (phe)
    {
        _ASSERTE(NULL != phe->m_pMLStub);
        phe->m_pMLStub->DecRef();
        phe = (MLCHASHENTRY*)GetNext((BYTE*)phe);
    }
}



//---------------------------------------------------------
// Callback function for DeleteLoop.
//---------------------------------------------------------
/*static*/ BOOL MLStubCache::DeleteLoopFunc(BYTE *pEntry, LPVOID)
{
    // WARNING: Inside the MLStubCache lock. Be careful what you do.

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pEntry));
    }
    CONTRACTL_END;

    MLCHASHENTRY *phe = (MLCHASHENTRY*)pEntry;
    if (phe->m_pMLStub->HeuristicLooksOrphaned())
    {
        phe->m_pMLStub->DecRef();
        return TRUE;
    }
    return FALSE;
}


//---------------------------------------------------------
// Another callback function for DeleteLoop.
//---------------------------------------------------------
/*static*/ BOOL MLStubCache::ForceDeleteLoopFunc(BYTE *pEntry, LPVOID)
{
    // WARNING: Inside the MLStubCache lock. Be careful what you do.

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pEntry));
    }
    CONTRACTL_END;

    MLCHASHENTRY *phe = (MLCHASHENTRY*)pEntry;
    phe->m_pMLStub->ForceDelete();
    return TRUE;
}


//---------------------------------------------------------
// Call this occasionally to get rid of unused stubs.
//---------------------------------------------------------
VOID MLStubCache::FreeUnusedStubs()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    CrstHolder ch(&m_crst);

    DeleteLoop(DeleteLoopFunc, 0);
}



//---------------------------------------------------------
// Returns the equivalent hashed Stub, creating a new hash
// entry if necessary. If the latter, will call out to CompileMLStub.
//
// Refcounting:
//    The caller is responsible for DecRef'ing the returned stub in
//    order to avoid leaks.
//
//
// On successful exit, *pMode is set to describe
// the compiled nature of the MLStub.
//
// callerContext can be used by the caller to push some context through
// to the compilation routine.
//---------------------------------------------------------
Stub *MLStubCache::Canonicalize(const BYTE * pRawMLStub, MLStubCompilationMode *pMode,
                                void *callerContext)
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    MLCHASHENTRY *phe = NULL;
        
    CrstHolder ch(&m_crst);
    {
        // Try to find the stub
        phe = (MLCHASHENTRY*)Find((LPVOID)pRawMLStub);
        if (phe)
        {
            StubHolder<Stub> pstub;
            pstub = phe->m_pMLStub;

            // IncRef as we're returning a reference to our caller.
            pstub->IncRef();
            
            *pMode = (MLStubCompilationMode) (phe->m_compilationMode);
            pstub.SuppressRelease();
            RETURN pstub;
        }
    }
    ch.Release();

    MEMORY_REPORT_CONTEXT_SCOPE("MLStubCache");

    // Couldn't find it, let's try to compile it.
    CPUSTUBLINKER sl;
    CPUSTUBLINKER slempty;
    CPUSTUBLINKER *psl = &sl;
    MLStubCompilationMode mode;
    mode = CompileMLStub(pRawMLStub, psl, callerContext);
    if (mode == INTERPRETED)
    {
        // CompileMLStub returns INTERPRETED for error cases:
        // in this case, redirect to the empty stublinker so
        // we don't accidentally pick up any crud that
        // CompileMLStub threw into the stublinker before
        // it ran into the error condition.
        psl = &slempty;
    }

    *pMode = mode;

    // Append the raw ML stub to the native stub
    // and link up the stub.
    CodeLabel *plabel = psl->EmitNewCodeLabel();
    psl->EmitBytes(pRawMLStub, Length(pRawMLStub));
    StubHolder<Stub> pstub;
    pstub = psl->Link(m_heap);
    UINT32 offset = psl->GetLabelOffset(plabel);

    if (offset > 0xffff)
        COMPlusThrowOM();



    ch.Acquire();
    {
        bool bNew;
        phe = (MLCHASHENTRY*)FindOrAdd((LPVOID)pRawMLStub, /*modifies*/bNew);
        if (phe)
        {
            if (bNew)
            {
                phe->m_pMLStub = pstub;                
                phe->m_offsetOfRawMLStub = (UINT16)offset;
                phe->m_compilationMode   = mode;

                AddStub(pRawMLStub, pstub);
            }
            else
            {
                // If we got here, some other thread got in
                // and enregistered an identical stub during
                // the window in which we were out of the m_crst.

                //Under DEBUG, two identical ML streams can actually compile
                // to different compiled stubs due to the checked build's
                // toggling between inlined TLSGetValue and api TLSGetValue.
                //_ASSERTE(phe->m_offsetOfRawMLStub == (UINT16)offset);
                _ASSERTE(phe->m_compilationMode == mode);

                //Use the previously created stub
                // This will DecRef the new stub for us.
                pstub = phe->m_pMLStub; 
            }
            // IncRef so that caller has firm ownership of stub.
            pstub->IncRef();
        }
    }
    ch.Release();

    if (!phe)
    {
        // Couldn't grow hash table due to lack of memory.
        COMPlusThrowOM();
    }

    pstub.SuppressRelease();
    RETURN pstub;
}


void MLStubCache::AddStub(const BYTE* pRawMLStub, Stub* pNewStub)
{
    LEAF_CONTRACT;

    // By default, don't do anything.
    return;
}


//*****************************************************************************
// Hash is called with a pointer to an element in the table.  You must override
// this method and provide a hash algorithm for your element type.
//*****************************************************************************
unsigned long MLStubCache::Hash(             // The key value.
    void const  *pData)                      // Raw data to hash.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    const BYTE *pRawMLStub = (const BYTE *)pData;

    UINT cb = Length(pRawMLStub);
    long   hash = 0;
    while (cb--)
        hash = _rotl(hash,1) + *(pRawMLStub++);

    return hash;
}

//*****************************************************************************
// Compare is used in the typical memcmp way, 0 is eqaulity, -1/1 indicate
// direction of miscompare.  In this system everything is always equal or not.
//*****************************************************************************
unsigned long MLStubCache::Compare(          // 0, -1, or 1.
    void const  *pData,                 // Raw key data on lookup.
    BYTE        *pElement)            // The element to compare data against.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    const BYTE *pRawMLStub1  = (const BYTE *)pData;
    const BYTE *pRawMLStub2  = (const BYTE *)GetKey(pElement);
    UINT cb1 = Length(pRawMLStub1);
    UINT cb2 = Length(pRawMLStub2);

    if (cb1 != cb2)
        return 1; // not equal
    else
    {
        while (cb1--)
        {
            if (*(pRawMLStub1++) != *(pRawMLStub2++))
                return 1; // not equal
        }
        return 0;
    }
}

//*****************************************************************************
// Return true if the element is free to be used.
//*****************************************************************************
CClosedHashBase::ELEMENTSTATUS MLStubCache::Status(           // The status of the entry.
    BYTE        *pElement)           // The element to check.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    Stub *pStub = ((MLCHASHENTRY*)pElement)->m_pMLStub;
    
    if (pStub == NULL)
        return FREE;
    else if (pStub == (Stub*)(-1))
        return DELETED;
    else
        return USED;
}

//*****************************************************************************
// Sets the status of the given element.
//*****************************************************************************
void MLStubCache::SetStatus(
    BYTE        *pElement,              // The element to set status for.
    ELEMENTSTATUS eStatus)            // New status.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    MLCHASHENTRY *phe = (MLCHASHENTRY*)pElement;
    
    switch (eStatus)
    {
        case FREE:    phe->m_pMLStub = NULL;   break;
        case DELETED: phe->m_pMLStub = (Stub*)(-1); break;
        default:
            _ASSERTE(!"MLCacheEntry::SetStatus(): Bad argument.");
    }
}

//*****************************************************************************
// Returns the internal key value for an element.
//*****************************************************************************
void *MLStubCache::GetKey(                   // The data to hash on.
    BYTE        *pElement)           // The element to return data ptr for.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    MLCHASHENTRY *phe = (MLCHASHENTRY*)pElement;
    return (void *)( ((BYTE*)(phe->m_pMLStub->GetEntryPoint())) + phe->m_offsetOfRawMLStub ); 
}
