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
// File: srcdata.cpp
//
// ===========================================================================

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// CSourceData::CSourceData

CSourceData::CSourceData (CSourceModuleBase *pModule) :
    m_spModule(pModule),
    m_iRef(0)
{
    m_spModule->AddDataRef ();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::~CSourceData

CSourceData::~CSourceData ()
{
    ASSERT (m_spModule != NULL);
    m_spModule->ReleaseDataRef ();
    m_spModule.Release();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::CreateInstance

HRESULT CSourceData::CreateInstance (CSourceModuleBase *pModule, ICSSourceData **ppData)
{
    CSourceData *pNew = new CSourceData (pModule);

    if (pNew == NULL)
        return E_OUTOFMEMORY;

    (*ppData) = pNew;
    (*ppData)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::AddRef

STDMETHODIMP_(ULONG) CSourceData::AddRef ()
{
    return InterlockedIncrement (&m_iRef);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::Release

STDMETHODIMP_(ULONG) CSourceData::Release ()
{
    ULONG   iNew = InterlockedDecrement (&m_iRef);
    if (iNew == 0)
        delete this;

    return iNew;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::QueryInterface

STDMETHODIMP CSourceData::QueryInterface (REFIID riid, void **ppObj)
{
    *ppObj = NULL;

    if (riid == IID_IUnknown || riid == IID_ICSSourceData)
    {
        *ppObj = (ICSSourceData *)this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetSourceModule

STDMETHODIMP CSourceData::GetSourceModule (ICSSourceModule **ppModule)
{
    return m_spModule->GetICSSourceModule(ppModule);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetLexResults

STDMETHODIMP CSourceData::GetLexResults (LEXDATA *pLexData)
{
    return m_spModule->GetLexResults (this, pLexData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetSingleTokenData

STDMETHODIMP CSourceData::GetSingleTokenData (long iToken, TOKENDATA *pTokenData)
{
    return m_spModule->GetSingleTokenData (this, iToken, pTokenData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetSingleTokenPos

STDMETHODIMP CSourceData::GetSingleTokenPos (long iToken, POSDATA *pposToken)
{
    return m_spModule->GetSingleTokenPos (this, iToken, pposToken);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetTokenText

STDMETHODIMP CSourceData::GetTokenText (long iTokenId, PCWSTR *ppszText, long *piLength)
{
    return m_spModule->GetTokenText (iTokenId, ppszText, piLength);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::IsInsideComment

STDMETHODIMP CSourceData::IsInsideComment (const POSDATA pos, BOOL *pfInComment)
{
    return m_spModule->IsInsideComment (this, pos, pfInComment);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::ParseTopLevel

STDMETHODIMP CSourceData::ParseTopLevel (BASENODE **ppTree)
{
    return m_spModule->ParseTopLevel (this, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::ParseTopLevel2

STDMETHODIMP CSourceData::ParseTopLevel2 (MIDL_BASENODE **ppTree, BOOL fCreateParseDiffs)
{
    return m_spModule->ParseTopLevel (this, ppTree, fCreateParseDiffs);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetErrors

STDMETHODIMP CSourceData::GetErrors (ERRORCATEGORY iCategory, ICSErrorContainer **ppErrors)
{
    return m_spModule->GetErrors (this, iCategory, ppErrors);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetInteriorParseTree

STDMETHODIMP CSourceData::GetInteriorParseTree (BASENODE *pNode, ICSInteriorTree **ppTree)
{
    return m_spModule->GetInteriorParseTree (this, pNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::LookupNode

STDMETHODIMP CSourceData::LookupNode (NAME *pKey, long iOrdinal, BASENODE **ppNode, long *piGlobalOrdinal)
{
    return m_spModule->LookupNode (this, pKey, iOrdinal, ppNode, piGlobalOrdinal);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetNodeKeyOrdinal

STDMETHODIMP CSourceData::GetNodeKeyOrdinal (BASENODE *pNode, NAME **ppKey, long *piKeyOrdinal)
{
    return m_spModule->GetNodeKeyOrdinal (this, pNode, ppKey, piKeyOrdinal);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetGlobalKeyArray

STDMETHODIMP CSourceData::GetGlobalKeyArray (KEYEDNODE *pKeyedNodes, long iSize, long *piCopied)
{
    return m_spModule->GetGlobalKeyArray (this, pKeyedNodes, iSize, piCopied);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::ParseForErrors

STDMETHODIMP CSourceData::ParseForErrors ()
{
    return m_spModule->ParseForErrors (this);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::FindLeafNode

STDMETHODIMP CSourceData::FindLeafNode (const POSDATA pos, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return FindLeafNodeEx(pos, EF_FULL, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::FindLeafNodeEx

STDMETHODIMP CSourceData::FindLeafNodeEx (const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return m_spModule->FindLeafNodeEx (this, pos, flags, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::FindLeafNodeForToken

STDMETHODIMP CSourceData::FindLeafNodeForToken (long iToken, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return FindLeafNodeForTokenEx(iToken, EF_FULL, ppNode, ppTree);
}


////////////////////////////////////////////////////////////////////////////////
// CSourceData::FindLeafNodeForTokenEx

STDMETHODIMP CSourceData::FindLeafNodeForTokenEx (long iToken, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return m_spModule->FindLeafNodeForTokenEx (this, iToken, flags, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetExtent

STDMETHODIMP CSourceData::GetExtentEx (BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags)
{
    return m_spModule->GetExtent (pNode, pposStart, pposEnd, flags);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetTokenExtent

STDMETHODIMP CSourceData::GetTokenExtent (BASENODE *pNode, long *piFirstToken, long *piLastToken)
{
    return m_spModule->GetTokenExtent (pNode, piFirstToken, piLastToken);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetDocComment

STDMETHODIMP CSourceData::GetDocComment (BASENODE *pNode, long *piComment, long *piCount)
{
    return m_spModule->GetDocComment (pNode, piComment, piCount);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetDocCommentXML

STDMETHODIMP CSourceData::GetDocCommentXML (BASENODE *pNode, BSTR *pbstrDoc)
{
    return m_spModule->GetDocCommentXML (pNode, pbstrDoc);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::IsUpToDate

STDMETHODIMP CSourceData::IsUpToDate (BOOL *pfTokenized, BOOL *pfTopParsed)
{
    return m_spModule->IsUpToDate (pfTokenized, pfTopParsed);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::MapSourceLine

STDMETHODIMP CSourceData::MapSourceLine(long iLine, long *piMappedLine, PCWSTR *ppszFilename, BOOL *pbIsHidden)
{
	return m_spModule->MapSourceLine(this, iLine, piMappedLine, ppszFilename, pbIsHidden);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::GetLastRenamedTokenIndex

STDMETHODIMP CSourceData::GetLastRenamedTokenIndex(long *piTokIdx, NAME **ppPreviousName)
{
	return m_spModule->GetLastRenamedTokenIndex(piTokIdx, ppPreviousName);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::ResetRenamedTokenData

STDMETHODIMP CSourceData::ResetRenamedTokenData()
{
	return m_spModule->ResetRenamedTokenData();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::CloneToNonLocking

STDMETHODIMP CSourceData::CloneToNonLocking(ICSSourceData **ppData)
{
    HRESULT hr;
    CSourceModuleBasePtr spClone;
    if (FAILED (hr = m_spModule->Clone (this, &spClone)))
        return hr;

    return CSourceData::CreateInstance (spClone, ppData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceData::OnRename

STDMETHODIMP CSourceData::OnRename(NAME * pNewName)
{
    return m_spModule->OnRename(pNewName);
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CSourceData::GetExtent(BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd) {
    return GetExtentEx( pNode, pposStart, pposEnd, EF_FULL);
};
