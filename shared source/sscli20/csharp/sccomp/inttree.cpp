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
// File: inttree.cpp
//
// ===========================================================================

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::CInteriorTree

CInteriorTree::CInteriorTree () :
    m_pSrcData(NULL),
    m_iRef(0),
    m_pInteriorNode(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::~CInteriorTree

CInteriorTree::~CInteriorTree ()
{
    if (m_pInteriorNode != NULL)
        m_pInteriorNode->Release();

    // NOTE:  Must release this last -- the above might destroy our heap!
    if (m_pSrcData != NULL)
        m_pSrcData->Release();
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::CreateInstance

HRESULT CInteriorTree::CreateInstance (CSourceData *pSrcData, BASENODE *pNode, ICSInteriorTree **ppTree)
{
    CInteriorTree   *pObj = new CInteriorTree ();
    HRESULT         hr;

    if (pObj == NULL)
        return E_OUTOFMEMORY;

    if (FAILED (hr = pObj->Initialize (pSrcData, pNode)) ||
        FAILED (hr = pObj->QueryInterface (IID_ICSInteriorTree, (void **)ppTree)))
    {
        delete pObj;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::Initialize

HRESULT CInteriorTree::Initialize (CSourceData *pSrcData, BASENODE *pNode)
{
    m_pSrcData = pSrcData;
    pSrcData->AddRef();
#ifdef DEBUG
    m_dwThreadId = GetCurrentThreadId();
#endif
    return pSrcData->GetModule()->GetInteriorNode (pSrcData, pNode, &m_pInteriorNode);
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::AddRef

STDMETHODIMP_(ULONG) CInteriorTree::AddRef ()
{
    ASSERT (m_dwThreadId == GetCurrentThreadId());
    return ++m_iRef;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::Release

STDMETHODIMP_(ULONG) CInteriorTree::Release ()
{
    ASSERT (m_dwThreadId == GetCurrentThreadId());

    if (--m_iRef == 0)
    {
        delete this;
        return 0;
    }

    return m_iRef;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::QueryInterface

STDMETHODIMP CInteriorTree::QueryInterface (REFIID riid, void **ppObj)
{
    ASSERT (m_dwThreadId == GetCurrentThreadId());

    *ppObj = NULL;

    if (riid == IID_IUnknown || riid == IID_ICSInteriorTree)
    {
        *ppObj = (ICSInteriorTree *)this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::GetTree

STDMETHODIMP CInteriorTree::GetTree (BASENODE **ppTree)
{
    ASSERT (m_dwThreadId == GetCurrentThreadId());
    *ppTree = m_pInteriorNode->GetRootNode ();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorTree::GetErrors

STDMETHODIMP CInteriorTree::GetErrors (ICSErrorContainer **ppErrors)
{
    ASSERT (m_dwThreadId == GetCurrentThreadId());
    *ppErrors = m_pInteriorNode->GetErrorContainer();
    (*ppErrors)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorNode::~CInteriorNode

CInteriorNode::~CInteriorNode ()
{
    // When the interior node is destroyed, it must clear out the appropriate
    // fields of the container node to indicate that it is no longer parsed.
    switch (m_pContainerNode->kind)
    {
        case NK_CTOR:
        case NK_DTOR:
        case NK_METHOD:
        case NK_OPERATOR:
            {
                WriteToggler allowWrites(ProtectedEntityFlags::ParseTree, m_pContainerNode->asANYMETHOD()->pBody, m_pContainerNode->asANYMETHOD()->pInteriorNode);
                m_pContainerNode->asANYMETHOD()->pBody = NULL;
                m_pContainerNode->asANYMETHOD()->pInteriorNode = NULL;
            }
            break;

        case NK_ACCESSOR:
            {
                WriteToggler allowWrites(ProtectedEntityFlags::ParseTree, m_pContainerNode->asACCESSOR()->pBody, m_pContainerNode->asACCESSOR()->pInteriorNode);
                m_pContainerNode->asACCESSOR()->pBody = NULL;
                m_pContainerNode->asACCESSOR()->pInteriorNode = NULL;
            }
            break;

        default:
            VSFAIL ("Interior node kind not handled in CInteriorNode destruction!");
            break;
    }

    // Get rid of our errors
    if (m_pErrors != NULL)
        m_pErrors->Release();
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorNode::CreateErrorContainer

CErrorContainer *CInteriorNode::CreateErrorContainer ()
{
    ASSERT (m_pErrors == NULL);
    if (FAILED (CErrorContainer::CreateInstance (EC_METHODPARSE, (DWORD_PTR)m_pContainerNode, &m_pErrors)))
    {
        VSFAIL("Out of memory");
    }
    return m_pErrors;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorNode::AddRef

void CInteriorNode::AddRef ()
{
    // Interior nodes can be used by more than one thread.  To serialize access
    // to its ref count and be assured that full creation and destruction of
    // these guys are serialized.
    CTinyGate   gate (m_pModule->GetStateLock());
    m_iRef++;
}

////////////////////////////////////////////////////////////////////////////////
// CInteriorNode::Release

void CInteriorNode::Release ()
{
    CTinyGate   gate (m_pModule->GetStateLock());
    long iRef = --m_iRef;
    if (iRef == 0)
        delete this;
}


////////////////////////////////////////////////////////////////////////////////
// CPrimaryInteriorNode::CPrimaryInteriorNode

CPrimaryInteriorNode::CPrimaryInteriorNode (CSourceModule *pModule, BASENODE *pContainer) 
    : CInteriorNode (pModule, pContainer) 
{
}

////////////////////////////////////////////////////////////////////////////////
// CPrimaryInteriorNode::~CPrimaryInteriorNode

CPrimaryInteriorNode::~CPrimaryInteriorNode ()
{
    // When a primary interiorn node is destroyed, it must notify the module that
    // its internal heap is available for another primary interior node parse.
    ((CSourceModule *)m_pModule)->ResetHeapBusyFlag ();
}
