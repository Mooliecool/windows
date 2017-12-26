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
// File: parser.h
//
// ===========================================================================

#ifndef __parser_h__
#define __parser_h__

#include "errorids.h"
#include "csiface.h"
#include "tokinfo.h"
#include "alloc.h"
#include "nodes.h"
#include "locks.h"
#include "namemgr.h"
#include "lexer.h"
#include "controller.h"
#include "basenodelookupcache.h"

class CListMaker;
class CSourceData;

namespace ScanTypeFlags { enum _Enum {
    Unknown,
    NotType,
    NonGenericTypeOrExpr,
    GenericTypeOrExpr,
    PointerOrMult,
    MustBeType,
    NullableType,
    AliasQualName           // Namespace or Type
}; };
DECLARE_FLAGS_TYPE(ScanTypeFlags)


namespace ScanNamedTypePart { enum _Enum {
    NotName,
    SimpleName,
    GenericName
}; };
DECLARE_FLAGS_TYPE(ScanNamedTypePart)

namespace MemberName { enum _Enum {
    NotMemberName,                 // anything else
    NotMemberNameWithDot,          // any bad member name which contains a dot
    GenericMethodName,             // I<R>.M<T>
    IndexerName,                   // I<R>.this
    PropertyOrEventOrMethodName,   // I<R>.M
    SimpleName,                    // M
}; };
DECLARE_FLAGS_TYPE(MemberName)

////////////////////////////////////////////////////////////////////////////////
// CParser
//
// This is the base class for the CSharp parser.  Like CLexer, it is abstract;
// you must create a derivation and implement the pure virtuals.

class CParser
{
protected:
    NAMEMGR         *m_pNAMEMGR;
    long            m_iCurToken;
    long            m_iPrevTok;                     // NOTE:  -1 means we must search for it...
    CSTOKEN         *m_pTokens;
    long            m_iTokens;
    PCWSTR          m_pszSource;
    long            *m_piLines;
    long            m_iLines;
    BOOL            m_fErrorOnCurTok;
    long            m_iErrors;
    NAME *          m_pNameSourceFile;

    // The special names - including contextual "pseudo keywords" like get, set, where, etc.
#define SPECNAME(id, str) id,
    enum {
#include "specnames.h"
        SN_LIM
    };
    NAME * m_rgnameSpec[SN_LIM];

    __forceinline NAME * SpecName(int sn) {
        ASSERT(0 <= sn && sn < SN_LIM);
        return m_rgnameSpec[sn];
    }
    __forceinline bool CheckForName(long itok, NAME * name) {
        ASSERT(0 <= itok && itok < m_iTokens);
        return (m_pTokens[itok].Token() == TID_IDENTIFIER && m_pTokens[itok].Name() == name);
    }

    __forceinline bool CheckForSpecName(int sn) {
        return CheckForName(CurTokenIndex(), SpecName(sn));
    }

    enum PARSEDECLFLAGS
    {
        PVDF_LOCAL  = 0x0001,           // Local variable declaration (i.e. not a field of a type)
        PVDF_CONST  = 0x0002,           // Variable is CONST
        PVDF_FIXED  = 0x0004,           // Fixed buffer declaration
    };

    NAMEMGR         *GetNameMgr() { return m_pNAMEMGR; }

    virtual void    ReportFeatureUse(DWORD dwFeatureId) { }

    static long PeekTokenIndexFromInternalEx (CSTOKEN * pTokens, long iTokens, const long iCur, ExtentFlags flags, long iPeek);
    static long PeekTokenIndexFromInternal (CSTOKEN * pTokens, long iTokens, const long iCur, long iPeek);

private:
    bool IsLocalDeclaration(ScanTypeFlagsEnum st);
    void CheckIfGeneric(bool fInExpr, bool* pfDefinitelyGeneric, bool* pfPossiblyGeneric);
    bool PastCloseToken(long iClose);
    bool AtSwitchCase(long iClose);

protected:
    virtual bool SupportsErrorSuppression() { return false; }
    virtual CErrorSuppression GetErrorSuppression() { VSFAIL("Someone subclassed incorrectly"); return CErrorSuppression(); }

public:
    //CParser (CController *pController);
    CParser (NAMEMGR * pNAMEMGR);
    HRESULT Init(NAMEMGR * pNAMEMGR);

    virtual ~CParser ();

    // Initialization
    void            SetInputData (PCWSTR pszFileName, CSTOKEN *pTokens, long iTokens, PCWSTR pszSource, long *piLines, long iLines);
    void            SetInputData (PCWSTR pszFileName, LEXDATA *pLexData);

    // Simple parsing/token indexing helpers
    TOKENID         CurToken () { return m_pTokens[m_iCurToken].Token(); }
    long            CurTokenIndex () { return m_iCurToken; }
    void            Rewind (long iIndex) { m_iCurToken = iIndex; m_iPrevTok = -1; m_fErrorOnCurTok = FALSE; }
    POSDATA         &CurTokenPos () { return m_pTokens[m_iCurToken]; }
    TOKENID         TokenAt (long iIndex) { return m_pTokens[iIndex].Token(); }
    TOKENID         NextToken () 
    { 
        long    iPrev = m_iCurToken;
        while (m_iCurToken < m_iTokens - 1) 
        { 
            m_iCurToken++; 
            m_fErrorOnCurTok = FALSE; 
            if ((m_rgTokenInfo[m_pTokens[m_iCurToken].Token()].dwFlags & TFF_NOISE) == 0)
            {
                m_iPrevTok = iPrev;
                return CurToken();
            }
        } 
        
        return CurToken(); 
    }

    TOKENID         PrevToken () 
    { 
        if (m_iPrevTok != -1)
        {
            // Previous token is known...
            m_iCurToken = m_iPrevTok;
        }
        else
        {
            // Previous token isn't known, must search for it
            while (m_iCurToken > 0)
                if ((m_rgTokenInfo[m_pTokens[--m_iCurToken].Token()].dwFlags & TFF_NOISE) == 0)
                    break;
        }

        // Must search for the next "previous" token
        m_iPrevTok = -1;
        m_fErrorOnCurTok = FALSE; 
        return CurToken(); 
    }

    TOKENID         PeekToken (long iPeek = 1) { return m_pTokens[PeekTokenIndexFrom (m_iCurToken, iPeek)].Token(); }
    long            PeekTokenIndex (long iPeek = 1) { return PeekTokenIndexFrom (m_iCurToken, iPeek); }
    long            PeekTokenIndexFrom (long iCur, long iPeek = 1) { return PeekTokenIndexFromInternal (m_pTokens, m_iTokens, iCur, iPeek); }
    long            PeekTokenIndexFromEx (long iCur, ExtentFlags flags, long iPeek = 1) { return PeekTokenIndexFromInternalEx (m_pTokens, m_iTokens, iCur, flags, iPeek); }


    // More advanced (non-inlined) parsing/token indexing helpers

    //returns true if it successfully ate that token
    bool            Eat (TOKENID iToken);
    bool            CheckToken (TOKENID iToken);

    // Error reporting
    void ErrorArgs(ERRORIDS id, int carg, ErrArg * prgarg);
    void Error(ERRORIDS id) { ErrorArgs(id, 0, NULL); }
    void Error(ERRORIDS id, ErrArg a);
    void Error(ERRORIDS id, ErrArg a, ErrArg b);

    void ErrorAtTokenArgs(long iTokenIdx, ERRORIDS id, int carg, ErrArg * prgarg);
    void ErrorAtToken(long iTokenIdx, ERRORIDS id) { ErrorAtTokenArgs(iTokenIdx, id, 0, NULL); }
    void ErrorAtToken(long iTokenIdx, ERRORIDS id, ErrArg a);
    void ErrorAtToken(long iTokenIdx, ERRORIDS id, ErrArg a, ErrArg b);

    void ErrorAfterPrevTokenArgs(ERRORIDS id, int carg, ErrArg * prgarg);
    void ErrorAfterPrevToken(ERRORIDS id) { ErrorAfterPrevTokenArgs(id, 0, NULL); }
    void ErrorAfterPrevToken(ERRORIDS id, ErrArg a);
    void ErrorAfterPrevToken(ERRORIDS id, ErrArg a, ErrArg b);

    void ErrorAtPositionArgs(long iLine, long iCol, long iExtent, ERRORIDS id, int carg, ErrArg * prgarg);
    void ErrorAtPosition(long iLine, long iCol, long iExtent, ERRORIDS id) { ErrorAtPositionArgs(iLine, iCol, iExtent, id, 0, NULL); }
    // void __cdecl    ErrorAtPosition (long iLine, long iCol, long iExtent, ERRORIDS iErrorId, ...);

    ERRORIDS        ExpectedErrorFromToken (TOKENID iToken, TOKENID iTokenActual);
    PCWSTR          GetTokenText (long iTokenIdx);
    void            ErrorInvalidMemberDecl(long iTokenIdx);

    // Top-level parse/skip/scan helpers
    BOOL            SkipToNamespaceElement ();
    BOOL            SkipToMember ();
    DWORD           SkipBlock (DWORD dwFlags, long *piClose);
    DWORD           ScanMemberDeclaration (DWORD dwFlags);
    BOOL            ScanParameterList (BOOL fIndexer, BOOL *pfHadParms);
    BOOL            ScanCast ();
    ScanTypeFlagsEnum ScanType ();
    ScanNamedTypePartEnum ScanNamedTypePart();
    bool            ScanOptionalInstantiation();
    MemberNameEnum  ScanMemberName();
    
    // Literals
    STRCONST        *ScanStringLiteral (long iTokenIndex);
    WCHAR           ScanCharLiteral (long iTokenIndex);
    void            ScanNumericLiteral (long iTokenIndex, CONSTVALNODE *pNode);

    // Node allocation
    BASENODE        *AllocNode (NODEKIND kind, BASENODE * nodePar, long itok);
    BASENODE        *AllocNode (NODEKIND kind, BASENODE * nodePar)
        { return AllocNode(kind, nodePar, CurTokenIndex()); }

    BINOPNODE       *AllocDotNode (long iTokIdx, BASENODE *pParent, BASENODE *p1, BASENODE *p2);
    BINOPNODE       *AllocBinaryOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent, BASENODE *p1, BASENODE *p2);
    UNOPNODE        *AllocUnaryOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent, BASENODE *pArg);
    BASENODE        *AllocOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent);
    NAMENODE        *AllocNameNode (NAME *pName, long iTokIdx);
    GENERICNAMENODE *AllocGenericNameNode (NAME *pName, long iTokIdx);
    NAMENODE        *InitNameNode (NAMENODE *pNameNode, NAME *pName);

    // Top-level parsing methods
    virtual BASENODE *ParseSourceModule ();
    NAMESPACENODE   *ParseRootNamespace ();
    void            ParseNamespaceBody (NAMESPACENODE *pNS);
    BASENODE        *ParseUsingClause (BASENODE *pParent);
    BASENODE        *ParseExternAliasClause (BASENODE *pParent);
    BASENODE        *ParseGlobalAttributes (BASENODE *pParent);
    BASENODE        *ParseNamespace (BASENODE *pParent);
    BASENODE        *ParseTypeDeclaration (BASENODE *pParent, BASENODE *pAttr);
    BASENODE        *ParseAggregate (BASENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttrs, unsigned iMods);
    BASENODE        *ParseDelegate (BASENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttrs, unsigned iMods);
    BASENODE        *ParseEnum (BASENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttrs, unsigned iMods);
    BASENODE        *ParseTypeDef (BASENODE *pParent, long iTokIdx, BASENODE *pAttrs, unsigned iMods);


    MEMBERNODE      *ParseMember (AGGREGATENODE *pParent);
    MEMBERNODE      *ParseConstructor (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods);
    MEMBERNODE      *ParseDestructor (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods);
    MEMBERNODE      *ParseConstant (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods);
    MEMBERNODE      *ParseMethod (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType);
    MEMBERNODE      *ParseProperty (NODEKIND nodekind, AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType, bool isEvent);
    MEMBERNODE      *ParseField (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType, bool isEvent, bool isFixed);
    MEMBERNODE      *ParseOperator (AGGREGATENODE *pParent, long iTokIdx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType);

    TYPEBASENODE    *ParseType (BASENODE *pParent, bool fIsOrAs = false);
    PREDEFINEDTYPENODE *    ParsePredefinedType(BASENODE *pParent);
    TYPEBASENODE    *ParseUnderlyingType(BASENODE *pParent, bool *pfHadError);
    TYPEBASENODE    *ParsePointerTypeMods (BASENODE *pParent, TYPEBASENODE *pBaseType);
    NAMEDTYPENODE   *ParseNamedType (BASENODE *pParent);
    TYPEBASENODE    *ParseClassType (BASENODE *pParent);
    TYPEBASENODE    *ParseReturnType (BASENODE *pParent);
    BASENODE        *ParseBaseTypesClause (BASENODE *pParent);
    TYPEBASENODE    *ParseTypeName (BASENODE *pParent, BOOL fConstraint);
    BASENODE        *ParseInstantiation (BASENODE *pParent, bool fAllowAttrs, long * pitokClose);
    int             ScanOpenType ();
    TYPEBASENODE    *ParseOpenType (BASENODE *pParent);
    CONSTRAINTNODE  *ParseConstraint(BASENODE *pParent);
    BASENODE        *ParseConstraintClause(BASENODE *pParent, BOOL fAllowed);
    NAMENODE        *ParseIdentifier (BASENODE *pParent);
    NAMENODE        *ParseIdentifierOrKeyword (BASENODE *pParent);
    BASENODE        *ParseDottedName (BASENODE *pParent, bool fAllowQualifier);
    BASENODE        *ParseMethodName (BASENODE *pParent);
    BASENODE        *ParseQualifiedName(BASENODE* pParent, bool fInExpr);
    NAMENODE        *ParseGenericQualifiedNamePart(BASENODE *pParent, bool fInExpr, bool fTypeVarList);
    BASENODE        *ParseGenericQualifiedNameList(BASENODE *pParent, bool fInExpr);
    NAMENODE        *ParseMissingName (BASENODE *pParent, long iTokIndex);
    BASENODE        *ParseMissingType (BASENODE* pParent, int iTokIndex, int iStart, unsigned iMods, BASENODE* pAttr);
    BASENODE        *ParseIndexerName (BASENODE *pParent);
    unsigned        ParseModifiers (bool fReportErrors);
    void CheckForAlias(BASENODE * node);

    enum {
        kppoAllowAll = 0x00,
        kppoSquare = 0x01, // Square brackets
        kppoNoNames = 0x02,
        kppoNoParams = 0x04,
        kppoNoAttrs = 0x08,
        kppoNoVarargs = 0x10,
        kppoNoRefOrOut = 0x20,
    };

    BASENODE        *ParseParameter (BASENODE *pParent, int flags);
    void             ParseParameterList (BASENODE *pParent, BASENODE ** ppParams, long *piOpen, long *piClose, int flags);
    BASENODE        *ParseAttributeSection (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations, bool* pfAttributeHadError);
    BASENODE        *ParseAttributes (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations);
    BASENODE        *ParseAttributes (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations, bool* pfAttributeHadError);
    BASENODE        *ParseAttribute (BASENODE *pParent, bool* pfAttributeHadError);
    BASENODE        *ParseAttributeArgument (BASENODE *pParent, BOOL *pfNamed);
    BASENODE        *SetDefaultAttributeLocation(BASENODE *pAttrList, ATTRLOC defaultLocation, ATTRLOC validLocations);

    // For XML or EE member parsing.
    METHODNODE * ParseMemberRefSpecial();
    void ParseDottedNameSpecial(METHODNODE * meth);
    void AddToDottedListSpecial(BASENODE ** pnodeAll, BASENODE * node, int itokDot);
    int ParseParamListSpecial(METHODNODE * meth, bool fThis);
    void CheckEofSpecial(METHODNODE * meth, bool fThis);
    NAMENODE * ParseSingleNameSpecial(BASENODE * nodePar);
    BASENODE * ParseOperatorSpecial(METHODNODE * meth);

    enum PARSERTYPE {
        EParseNone,
#define PARSERDEF(name) EParse##name,
#include "parsertype.h"
    };

    // Interior node parsing methods
#define PARSERDEF(name) STATEMENTNODE   *Parse##name (BASENODE *pParent, long iClose = -1);
#include "parsertype.h"

    STATEMENTNODE   *ParseEmbeddedStatement (BASENODE *pParent, BOOL fComplexCheck);
    BASENODE        *ParseVariableDeclarator (BASENODE *pParent, BASENODE *pParentDecl, DWORD dwPVDFlags, BOOL fFirst, long iClose = -1);
    BASENODE        *ParseVariableInitializer (BASENODE *pParent, BOOL fAllowStackAlloc, long iClose = -1);
    BASENODE        *ParseArrayInitializer (BASENODE *pParent);
    BASENODE        *ParseStackAllocExpression (BASENODE *pParent);
    BASENODE        *ParseSwitchCase (BASENODE *pParent, long iClose);
    BASENODE        *ParseCatchClause (BASENODE *pParent, BOOL *pfEmpty);
    BASENODE        *ParseExpression (BASENODE *pParent, long iClose = -1);
    BASENODE        *ParseSubExpression (BASENODE *pParent, UINT iPrecedence, long iClose = -1);
    BASENODE        *ParseTerm (BASENODE *pParent, long iClose = -1);
    BASENODE        *ParsePostFixOperator (BASENODE *pParent, BASENODE *pExpr, long iClose = -1);
    BASENODE        *ParseCastOrExpression (BASENODE *pParent);
    BASENODE        *ParseAnonymousMethodExpr (BASENODE * pParent);
    BASENODE        *ParseNumber (BASENODE *pParent);
    BASENODE        *ParseStringLiteral (BASENODE *pParent);
    BASENODE        *ParseCharLiteral (BASENODE *pParent);
    BASENODE        *ParseNewExpression (BASENODE *pParent);
    BASENODE        *ParseArgument (BASENODE *pParent);
    BASENODE        *ParseArgumentList (BASENODE *pParent);
    BASENODE        *ParseArrayRankSpecifier (BASENODE *pParent, bool fIsDeclaration, int *nDim = NULL);
    BASENODE        *ParseDimExpressionList (BASENODE *pParent, int *nDim = NULL);
    BASENODE        *ParseExpressionList (BASENODE *pParent);



    // Extent/Leaf calculators
    HRESULT         GetExtent (CBasenodeLookupCache& lastTokenCache, BASENODE *pNode, ExtentFlags flags, POSDATA *pposStart, POSDATA *pposEnd);
    void            GetTokenExtent (BASENODE *pNode, long *piFirst, long *piLast, ExtentFlags flags);
    BASENODE*       FindContainingNodeInChain(CBasenodeLookupCache& lastTokenCache, POSDATA pos, BASENODE *pChain, ExtentFlags flags);
    BASENODE*       FindLeaf (const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ICSInteriorTree **ppTree);

    BASENODE*       FindLeafIgnoringTokenStream(const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ICSInteriorTree **ppTree);
    BASENODE*       FindLeafEx (CBasenodeLookupCache& lastTokenCache, const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ExtentFlags flags, ICSInteriorTree **ppTree);
    HRESULT         FindLeafNodeForTokenEx(long iToken, BASENODE* pNode, CSourceData* pData, ExtentFlags flags, BASENODE** ppNode, ICSInteriorTree** ppTree);
    long            GetFirstToken (BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName);
    long            GetLastToken       (CBasenodeLookupCache& lastTokenCache, BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName);
    long            GetLastTokenWorker (CBasenodeLookupCache& lastTokenCache, BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName);

    // Token processing
    long            IncludeNextToken                (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName, TOKENID iTok1);
    long            IncludeOneOfNextToken           (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName, TOKENID iTok1, TOKENID iTok2);
    long            IncludeSemiColon                (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeColon                    (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeCloseParen               (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeCloseCurly               (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeCloseSquare              (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeCloseParenOrCloseSquare  (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeSemiColonOrComma         (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeQuestion                 (long iTokIdx, ExtentFlags flags, BOOL* pfMissingName);
    long            IncludeComma                    (long iTokIdx, ExtentFlags flags, BOOL *pfMissingName);


    // Here are the pure virtuals that must be implemented by parser implementations
    virtual void    *MemAlloc (long iSize) = 0;
    virtual BOOL    CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd) = 0;
    virtual void    AddToNodeTable (BASENODE *pNode) = 0;
    virtual void    GetInteriorTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree) = 0;
    virtual HRESULT GetLexData(LEXDATA* pLexData) = 0;

    // Static token/operator information grabbers
    static  const   TOKINFO m_rgTokenInfo[TID_NUMTOKENS];
    static  const   OPINFO  m_rgOpInfo[];
    static  const   TOKINFO         *GetTokenInfo (TOKENID iTok) { return m_rgTokenInfo + iTok; }
    static  const   OPINFO          *GetOperatorInfo (OPERATOR iOp) { return m_rgOpInfo + iOp; }
    static          BOOL            IsRightAssociative (OPERATOR iOp) { return m_rgOpInfo[iOp].fRightAssoc; }
    static          UINT            GetOperatorPrecedence (OPERATOR iOp) { return m_rgOpInfo[iOp].iPrecedence; }
    static          UINT            GetCastPrecedence () { return GetOperatorPrecedence(OP_CAST); }
    static          bool            IsUnaryOperator (TOKENID iTok, OPERATOR *piOp, UINT *piPrecedence);
    static          bool            IsUnaryOperator (TOKENID iTok);
    static          BOOL            IsBinaryOperator (TOKENID iTok, OPERATOR *piOp, UINT *piPrecedence);
    static          STATEMENTNODE   *GetLastStatement (STATEMENTNODE *pStmt);
    static          BOOL            IsNameStart(TOKENID iTok) { return iTok == TID_IDENTIFIER; }
    static          BOOL            IsAnyTypeOrExpr(ScanTypeFlagsEnum st) { return st == ScanTypeFlags::GenericTypeOrExpr || st == ScanTypeFlags::NonGenericTypeOrExpr; }
    static          ScanTypeFlagsEnum PartScanToTypeScan(ScanNamedTypePartEnum st) { ASSERT(st != ScanNamedTypePart::NotName); return st == ScanNamedTypePart::GenericName ? ScanTypeFlags::GenericTypeOrExpr : ScanTypeFlags::NonGenericTypeOrExpr; }

    // This should really be on LEXDATA, but it makes it easier to share the token info table
    // and share the implementation.
    static          long            PeekTokenIndexFrom (const LEXDATA & ld, long iCur, long iPeek = 1) 
    { 
        return ld.PeekTokenIndexFrom (iCur, iPeek);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CSimpleParser

class CSimpleParser : public CParser
{
private:
    NRHEAP      m_heap;

public:
    CSimpleParser (CController *pController) : CParser (pController->GetNameMgr()), m_heap(pController) {}

    // CParser
    void    *MemAlloc (long iSize) { return m_heap.Alloc (iSize); }
    BOOL    CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd) { return FALSE; }
    void    AddToNodeTable (BASENODE *pNode) {}
    void    GetInteriorTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree) {}
    HRESULT GetLexData(LEXDATA* pLexData);
};

////////////////////////////////////////////////////////////////////////////////
// CTextParser
//
// This is the object that exposes ICSParser for external clients

class CTextParser :
    public CComObjectRootMT,
    public ICSParser
{
private:
    CSimpleParser       *m_pParser;
    CComPtr<ICSLexer>   m_spLexer;

public:
    BEGIN_COM_MAP(CTextParser)
        COM_INTERFACE_ENTRY(ICSParser)
    END_COM_MAP()

    CTextParser ();
    ~CTextParser ();

    HRESULT     Initialize (CController *pController, CompatibilityMode cm);

    // ICSParser
    STDMETHOD(SetInputText)(PCWSTR pszInputText, long iLen);
    STDMETHOD(CreateParseTree)(ParseTreeScope iScope, BASENODE *pParent, BASENODE **ppNode);
    STDMETHOD(AllocateNode)(long iKind, BASENODE **ppNode);
    STDMETHOD(FindLeafNode)(const POSDATA pos, BASENODE *pNode, BASENODE **ppLeafNode);    
    STDMETHOD(FindLeafNodeForToken)(long iToken, BASENODE* pNode, BASENODE** ppNode);
    STDMETHOD(GetLexResults)(LEXDATA *pLexData);
    STDMETHOD(GetExtent)(BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags);
    STDMETHOD(GetTokenExtent)(BASENODE *pNode, long *piFirstToken, long *piLastToken, ExtentFlags flags);
};

#endif //__parser_h__

