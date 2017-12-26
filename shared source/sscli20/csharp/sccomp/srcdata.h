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
// File: srcdata.h
//
// ===========================================================================

#ifndef __srcdata_h__
#define __srcdata_h__

#include "csiface.h"
#include "tokinfo.h"
#include "alloc.h"
#include "nodes.h"
#include "srcmod.h"

////////////////////////////////////////////////////////////////////////////////
// CSourceData

class CSourceData : 
    public ICSSourceData
{
private:
    CSourceModuleBasePtr    m_spModule;
    LONG                    m_iRef;

    CSourceData (CSourceModuleBase *pModule);
    ~CSourceData ();

public:
    //void    *operator new (size_t size);
    //void    operator delete (void *pv);

    static  HRESULT CreateInstance (CSourceModuleBase *pModule, ICSSourceData **ppData);

    CSourceModuleBase   *GetModule() { return m_spModule; }

    // IUnknown
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID riid, void **ppObj);

    // ICSSourceData
    STDMETHOD(GetSourceModule)(ICSSourceModule **ppModule);
    STDMETHOD(GetLexResults)(LEXDATA *pLexData);
    STDMETHOD(GetSingleTokenPos)(long iToken, POSDATA *pposToken);
    STDMETHOD(GetSingleTokenData)(long iToken, TOKENDATA *pTokenData);
    STDMETHOD(GetTokenText)(long iTokenId, PCWSTR *ppszText, long *piLength);
    STDMETHOD(IsInsideComment)(const POSDATA pos, BOOL *pfInComment);
    STDMETHOD(ParseTopLevel)(BASENODE **ppTree);
    STDMETHOD(ParseTopLevel2)(BASENODE **ppTree, BOOL fCreateParseDiffs);
    STDMETHOD(GetErrors)(ERRORCATEGORY iCategory, ICSErrorContainer **ppErrors);
    STDMETHOD(GetInteriorParseTree)(BASENODE *pNode, ICSInteriorTree **ppTree);
    STDMETHOD(LookupNode)(NAME *pKey, long iOrdinal, BASENODE **ppNode, long *piGlobalOrdinal);
    STDMETHOD(GetNodeKeyOrdinal)(BASENODE *pNode, NAME **ppKey, long *piKeyOrdinal);
    STDMETHOD(GetGlobalKeyArray)(KEYEDNODE *pKeyedNodes, long iSize, long *piCopied);
    STDMETHOD(ParseForErrors)();
    STDMETHOD(FindLeafNode)(const POSDATA pos, BASENODE **ppNode, ICSInteriorTree **ppTree);
    STDMETHOD(FindLeafNodeForToken)(long iToken, BASENODE **ppNode, ICSInteriorTree **ppTree);
    STDMETHOD(FindLeafNodeEx)(const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree);
    STDMETHOD(FindLeafNodeForTokenEx)(long iToken, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree);
    STDMETHOD(GetExtent)(BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd);
    STDMETHOD(GetTokenExtent)(BASENODE *pNode, long *piFirstToken, long *piLastToken);
    STDMETHOD(GetDocComment)(BASENODE *pNode, long *piComment, long *piCount);
    STDMETHOD(GetDocCommentXML)(BASENODE *pNode, BSTR * pbstrDoc);
    STDMETHOD(IsUpToDate)(BOOL *pfTokenized, BOOL *pfTopParsed);
    STDMETHOD(GetExtentEx)(BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags);
    STDMETHOD(MapSourceLine)(long iline, long *piMappedLine, PCWSTR *ppszFilename, BOOL *pbIsHidden);
    STDMETHOD(GetLastRenamedTokenIndex)(long *piTokIdx, NAME **ppPreviousName);
    STDMETHOD(ResetRenamedTokenData)();
    STDMETHOD(CloneToNonLocking)(ICSSourceData **ppData);
    STDMETHOD(OnRename)(NAME * pNewName);
};


#endif //__srcdata_h__

