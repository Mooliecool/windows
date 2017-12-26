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
// ===========================================================================
// File: inttree.h
//
// ===========================================================================

#ifndef __inttree_h__
#define __inttree_h__

#include "csiface.h"
#include "alloc.h"
#include "nodes.h"

class CSourceData;
class CSourceModule;
class CInteriorNode;
class CErrorContainer;

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree
//
// This is the COM object handed out to people requesting interior parses thru
// ISCSourceData::GetInteriorTree().  It holds a reference on the originating
// source data object, and a reference on the underlying parse tree (which is
// shared by potentially more than one of these objects).

class CInteriorTree : public ICSInteriorTree
{
private:
    CSourceData         *m_pSrcData;        // NOTE:  This is a ref'd pointer!
    long                m_iRef;             // Ref count for this object
    CInteriorNode       *m_pInteriorNode;   // The interior node we refer to
#ifdef DEBUG
    DWORD               m_dwThreadId;       // These should only be used by one thread!
#endif

    CInteriorTree ();
    ~CInteriorTree ();

public:
    static  HRESULT CreateInstance (CSourceData *pSrcData, BASENODE *pNode, ICSInteriorTree **ppTree);

    HRESULT         Initialize (CSourceData *pSrcData, BASENODE *pNode);

    // IUnknown
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID riid, void **ppObj);

    // ICSInteriorTree
    STDMETHOD(GetTree)(BASENODE **ppTree);
    STDMETHOD(GetErrors)(ICSErrorContainer **ppErrors);
};


////////////////////////////////////////////////////////////////////////////////
// CInteriorNode
//
// This is the base class for the internal objects that "hold" interior parse
// trees.  One of these exists for each interior node that is parsed.

class CInteriorNode
{
protected:
    CSourceModuleBase * m_pModule;                 // NOTE:  this is NOT a ref'd pointer (unnecessary)
    BASENODE *          m_pContainerNode;          // This is the interior node "container" whose child tree we contain
    CErrorContainer *   m_pErrors;                 // Parse errors
    long                m_iRef;                     // Ref count

public:
    CInteriorNode (CSourceModuleBase *pModule, BASENODE *pContainer) : m_pModule(pModule), m_pContainerNode(pContainer), m_pErrors(NULL), m_iRef(0) {}
    virtual ~CInteriorNode ();
    virtual NRHEAP      *GetAllocationHeap () { return NULL; }
    BASENODE        *GetRootNode () { return m_pContainerNode; }
    CErrorContainer *GetErrorContainer () { return m_pErrors; }
    CErrorContainer *CreateErrorContainer ();
    void            AddRef ();
    void            Release ();
};

////////////////////////////////////////////////////////////////////////////////
// CPrimaryInteriorNode
//
// This is a CInteriorNode that holds a parsed interior node in the NRHEAP built
// into the corresponding source module.  This is the most-often-used one; it
// is created when no other interior node is parsed whose node memory is allocated
// from the main module heap.

class CPrimaryInteriorNode : public CInteriorNode
{
public:
    CPrimaryInteriorNode (CSourceModule *pModule, BASENODE *pContainer);
    ~CPrimaryInteriorNode ();
    static  CPrimaryInteriorNode *CreateInstance (CSourceModule *pModule, BASENODE *pContainer) { return new CPrimaryInteriorNode (pModule, pContainer); }
};

////////////////////////////////////////////////////////////////////////////////
// CSecondaryInteriorNode
//
// This CInteriorNode derivation is used when a CPrimaryInteriorNode already
// exists, and is thus using the "second half" of the module's node allocation
// heap.  This object has its own such heap from which the interior nodes are
// allocated.

class CSecondaryInteriorNode : public CInteriorNode
{
private:
    NRHEAP      m_heap;         // Heap from which nodes are allocated

public:
    CSecondaryInteriorNode (CSourceModuleBase *pModule, ALLOCHOST *pHost, BASENODE *pContainer) : CInteriorNode (pModule, pContainer), m_heap(pHost, false, ProtectedEntityFlags::ParseTree) {}
    ~CSecondaryInteriorNode() {}
    static CSecondaryInteriorNode *CreateInstance (CSourceModuleBase *pModule, ALLOCHOST *pHost, BASENODE *pContainer) { return new CSecondaryInteriorNode (pModule, pHost, pContainer); }

    NRHEAP      *GetAllocationHeap () { return &m_heap; }
};

#endif //__inttree_h__

