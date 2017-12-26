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
// File: parser.cpp
//
// ===========================================================================

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// CListMaker

class CListMaker
{
private:
    BASENODE        **ppDest;
    CParser         *pParser; 

public:
    CListMaker (CParser *p, BASENODE **pp) : ppDest(pp), pParser(p) { *pp = NULL; }
    void Restart (BASENODE **pp) { ppDest = pp; *pp = NULL; }
    BASENODE *Add (BASENODE *pNew, long iTokIdx = -1)
    {
        // Handle NULL -- this is not an error case; some functions return NULL after
        // reporting an error, which should not corrupt the list.
        if (pNew == NULL)
            return pNew;

        ASSERT (pNew->pParent != NULL && pNew->pParent != (BASENODE *)I64(0xcccccccccccccccc));

        // If NULL, *ppDest just gets the value of pNew, which should already have
        // it's parent relationship set up (we assert that above).
        if (*ppDest == NULL)
        {
            // Do not update *ppDest -- this isn't a list yet!
            *ppDest = pNew;
            return pNew;
        }

        BINOPNODE   *pList = pParser->AllocNode(NK_LIST, (*ppDest)->pParent, iTokIdx)->asLIST();

        // Create the new list node
        pList->p1 = *ppDest;
        pList->p2 = pNew;
        ASSERT(pList->pParent == pList->p1->pParent);    // Use (*ppDest)'s old parent
        pList->p1->pParent = pList->p2->pParent = pList;

        // Update the destination pointer for the next list node
        *ppDest = pList;
        ppDest = &pList->p2;

        ASSERT ((*ppDest)->kind != NK_LIST);

        return pList;       // Return the new node in case the caller wants to store flags/position, etc.
    }
};


//
class CEnsureParserProgress
{
private:
    CParser* m_pParser;
    long m_iStart;

public:
    CEnsureParserProgress(CParser* pParser) : m_pParser(pParser)
    {
        m_iStart = m_pParser->CurTokenIndex();
    }

    ~CEnsureParserProgress()
    {
        //if the cursor wasn't advanced then this wasn't a valid
        //Statement.  Move forward one and start again.
        if (m_pParser->CurTokenIndex() == m_iStart) {
            m_pParser->NextToken();
        }
    }
};


////////////////////////////////////////////////////////////////////////////////
// CParser::m_rgTokenInfo

#define TOK(name, id, flags, type, stmtfunc, binop, unop, selfop, color) { name, flags, sizeof(name)/sizeof(WCHAR)-1, stmtfunc, type, binop, unop, selfop },
#define PARSEFN(func) CParser::EParse##func
#define NOPARSEFN CParser::EParseNone
const TOKINFO   CParser::m_rgTokenInfo[TID_NUMTOKENS] = {
    #include "tokens.h"
};
#undef  PARSEFN
#undef  NOPARSEFN

////////////////////////////////////////////////////////////////////////////////
// CParser::m_rgOpInfo

#define OP(n,p,a,stmt,t,pn,e) {t,p,a},
const OPINFO    CParser::m_rgOpInfo[] = {
    #include "ops.h"
    { TID_UNKNOWN, 0, 0}
};

////////////////////////////////////////////////////////////////////////////////
// CParser::CParser

CParser::CParser (NAMEMGR *pNAMEMGR) :
    m_pNAMEMGR(NULL),
    m_iCurToken(0),
    m_iPrevTok(-1),
    m_pTokens(NULL),    
    m_iTokens(0),
    m_pszSource(NULL),
    m_piLines(NULL),
    m_iLines(0),
    m_fErrorOnCurTok(FALSE),
    m_iErrors(0)
{
    ASSERT(pNAMEMGR);

    if (pNAMEMGR)
        Init(pNAMEMGR);
}

HRESULT CParser::Init(NAMEMGR * pNAMEMGR)
{
    m_pNAMEMGR = pNAMEMGR;
#define SPECNAME(id, str) m_rgnameSpec[id] = pNAMEMGR->AddString(str);
#include "specnames.h"
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::~CParser

CParser::~CParser ()
{
}

////////////////////////////////////////////////////////////////////////////////

long CParser::PeekTokenIndexFromInternalEx (CSTOKEN * pTokens, long iTokens, const long iCur, ExtentFlags flags, long iPeek)
{
    if (flags & EF_IGNORE_TOKEN_STREAM)
    {
        return iCur + iPeek;
    }
    else
    {
        return min (iTokens - 1, PeekTokenIndexFromInternal (pTokens, iTokens, iCur, iPeek));
    }
}

////////////////////////////////////////////////////////////////////////////////

long CParser::PeekTokenIndexFromInternal (CSTOKEN * pTokens, long iTokens, const long iCur, long iPeek)     
{
    if (iPeek > 0)
    {
        long i;
        for (i=iCur + 1; i<iTokens-1; i++)
        {
            if ((m_rgTokenInfo[pTokens[i].Token()].dwFlags & TFF_NOISE) == 0)
            {
                if (--iPeek == 0)
                    break;
            }
        }

        return min (i, iTokens - 1);
    }
    else if (iPeek < 0)
    {
        // We need to adjust iPeek when iCur is out of bound of the token stream
        iPeek += max(0, iCur - iTokens);    // Adjust iPeek to ignore out of stream indices
        long i = min(iTokens, iCur) - 1;    // Ensure starting index in in bound

        // If iPeek is still negative, go for it
        if (iPeek < 0)
        {
            for (; i >= 0; i--)
            {
                if ((m_rgTokenInfo[pTokens[i].Token()].dwFlags & TFF_NOISE) == 0)
                {
                    if (++iPeek == 0)
                        break;
                }
            }
        }
        return max (i, 0);
    }

    return iCur;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::SetInputData

void CParser::SetInputData (PCWSTR pszFileName, CSTOKEN *pTokens, long iTokens, PCWSTR pszSource, long *piLines, long iLines)
{
    m_pTokens = pTokens;
    m_iTokens = iTokens;
    m_pszSource = pszSource;
    m_piLines = piLines;
    m_iLines = iLines;
    m_pNameSourceFile = m_pNAMEMGR->AddString(pszFileName);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::SetInputData

void CParser::SetInputData (PCWSTR pszFileName, LEXDATA *pLexData)
{
    pLexData->UnsafeExposeTokens(&m_pTokens, &m_iTokens);
    pLexData->UnsafeExposeLineOffsets(&m_piLines, &m_iLines);
    pLexData->UnsafeExposeSource(&m_pszSource);
    m_pNameSourceFile = m_pNAMEMGR->AddString(pszFileName);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::Eat

bool CParser::Eat (TOKENID iTok)
{
    TOKENID iTokActual;
    if ((iTokActual = CurToken()) == iTok) {
        NextToken();
        return true;
    } else {
        ErrorAfterPrevToken (ExpectedErrorFromToken (iTok, iTokActual), GetTokenInfo(iTok)->pszText, GetTokenInfo(iTokActual)->pszText);
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CParser::CheckToken

inline bool CParser::CheckToken (TOKENID iTok)
{
    TOKENID iTokActual;
    if ((iTokActual = CurToken()) != iTok)
    {
        ErrorAfterPrevToken (ExpectedErrorFromToken (iTok, iTokActual), GetTokenInfo(iTok)->pszText, GetTokenInfo(iTokActual)->pszText);
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::Error
//
// Produce the given error/warning at the current token.

void CParser::ErrorArgs(ERRORIDS iErrorId, int carg, ErrArg * prgarg)
{
    if (!m_fErrorOnCurTok)
    {
        POSDATA pos = m_pTokens[CurTokenIndex()];
        POSDATA posEnd = m_pTokens[CurTokenIndex()].StopPosition();

        m_fErrorOnCurTok = CreateNewError(iErrorId, carg, prgarg, pos, posEnd);
    }

    m_iErrors++;
}

void CParser::Error(ERRORIDS id, ErrArg a)           {                           ErrorArgs(id, 1, &a); }
void CParser::Error(ERRORIDS id, ErrArg a, ErrArg b) { ErrArg args[] = { a, b }; ErrorArgs(id, 2, args); }

////////////////////////////////////////////////////////////////////////////////
// CParser::ErrorAtToken

void CParser::ErrorAtTokenArgs(long iTokenIdx, ERRORIDS id, int carg, ErrArg * prgarg)
{
    POSDATA pos = m_pTokens[iTokenIdx];
    POSDATA posEnd = m_pTokens[iTokenIdx].StopPosition();

    CreateNewError(id, carg, prgarg, pos, posEnd);
    m_iErrors++;
}

void CParser::ErrorAtToken(long iTokenIdx, ERRORIDS id, ErrArg a)           {                           ErrorAtTokenArgs(iTokenIdx, id, 1, &a); }
void CParser::ErrorAtToken(long iTokenIdx, ERRORIDS id, ErrArg a, ErrArg b) { ErrArg args[] = { a, b }; ErrorAtTokenArgs(iTokenIdx, id, 2, args); }

////////////////////////////////////////////////////////////////////////////////
// CParser::ErrorAfterPrevToken
//
// If the current token is on the same line as the previous token, then behave
// just like Error. Otherwise, report the error 1 character beyond the
// previous token. Useful for "expected semicolon" type messages.

void CParser::ErrorAfterPrevTokenArgs(ERRORIDS id, int carg, ErrArg * prgarg)
{
    long    iTokenIdx = CurTokenIndex();
    long    iPrevTokIdx = PeekTokenIndex (-1);
    POSDATA posCurToken = m_pTokens[iTokenIdx];
    POSDATA posPrevToken = m_pTokens[iPrevTokIdx];
    POSDATA pos, posEnd;
    BOOL    fCurTokError = FALSE;

    if (posPrevToken.iLine == posCurToken.iLine && CurToken() != TID_ENDFILE) 
    {
        pos = posCurToken;
        posEnd = m_pTokens[iTokenIdx].StopPosition();
        fCurTokError = TRUE;
    }
    else 
    {        
        pos = posEnd = m_pTokens[iPrevTokIdx].StopPosition();
        posEnd.iChar++;
    }

    if (!m_fErrorOnCurTok)
    {
        fCurTokError = (CreateNewError(id, carg, prgarg, pos, posEnd) & fCurTokError);
        m_iErrors++;
        // We only set this if we actually reported an error, not just a warning
        // And we're reporting it on the current token
        if (fCurTokError) {
            m_fErrorOnCurTok = TRUE;
        }
    }
}

void CParser::ErrorAfterPrevToken(ERRORIDS id, ErrArg a)           {                           ErrorAfterPrevTokenArgs(id, 1, &a); }
void CParser::ErrorAfterPrevToken(ERRORIDS id, ErrArg a, ErrArg b) { ErrArg args[] = { a, b }; ErrorAfterPrevTokenArgs(id, 2, args); }

////////////////////////////////////////////////////////////////////////////////
// CParser::ErrorAtPosition

void CParser::ErrorAtPositionArgs(long iLine, long iCol, long iExtent, ERRORIDS id, int carg, ErrArg * prgarg)
{
    POSDATA pos(iLine, iCol);
    POSDATA posEnd(iLine, iCol + max (iExtent, 1));
    CreateNewError(id, carg, prgarg, pos, posEnd);
    m_iErrors++;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ExpectedErrorFromToken

ERRORIDS CParser::ExpectedErrorFromToken (TOKENID iTok, TOKENID iTokActual)
{
    ERRORIDS   iError;

    switch (iTok)
    {
        case TID_IDENTIFIER:    
            if (iTokActual < TID_IDENTIFIER) 
                iError = ERR_IdentifierExpectedKW;   // A keyword -- use special message.
            else
                iError = ERR_IdentifierExpected;     
            break;
        case TID_SEMICOLON:     iError = ERR_SemicolonExpected;      break;
        //case TID_COLON:         iError = ERR_ColonExpected;          break;
        //case TID_OPENPAREN:     iError = ERR_LparenExpected;         break;
        case TID_CLOSEPAREN:    iError = ERR_CloseParenExpected;     break;
        case TID_OPENCURLY:     iError = ERR_LbraceExpected;         break;
        case TID_CLOSECURLY:    iError = ERR_RbraceExpected;         break;
        //case TID_CLOSESQUARE:   iError = ERR_CloseSquareExpected;    break;
        default:                iError = ERR_SyntaxError;
    }

    return iError;
}

//

PCWSTR CParser::GetTokenText (long iTokenIdx)
{
    HRESULT hr;
    switch (m_pTokens[iTokenIdx].Token())
    {
        case TID_IDENTIFIER:
        case TID_UNKNOWN:
            return m_pTokens[iTokenIdx].Name()->text;

        case TID_NUMBER:
            return m_pTokens[iTokenIdx].Literal()->szText;

        case TID_CHARLIT:
        {
            static const size_t cchBuf = 9;
            PWSTR   pszBuf = (PWSTR)MemAlloc (cchBuf * sizeof (WCHAR));

            if (m_pTokens[iTokenIdx].Length() == 3)
            {
                hr = StringCchPrintfW(pszBuf, cchBuf, L"'%c'", m_pTokens[iTokenIdx].CharValue ());
            }
            else 
            {
                hr = StringCchPrintfW (pszBuf, cchBuf, L"'\\u%04x'", m_pTokens[iTokenIdx].CharValue ());
            }
            ASSERT (SUCCEEDED (hr));
            
            return pszBuf;
        }

        case TID_VSLITERAL:
        case TID_STRINGLIT:
        {
            const int cchBuf = 32;
            PWSTR   pszBuf = (PWSTR)MemAlloc (cchBuf * sizeof (WCHAR)), pszStr;
            long    iLen;
            bool    fTooLong = false;

            if (m_pTokens[iTokenIdx].Token() == TID_VSLITERAL)
            {
                iLen = m_pTokens[iTokenIdx].VSLiteral()->str.length;
                pszStr = m_pTokens[iTokenIdx].VSLiteral()->str.text;
            }
            else
            {   
                iLen = m_pTokens[iTokenIdx].StringLiteral()->str.length;
                pszStr = m_pTokens[iTokenIdx].StringLiteral()->str.text;
            }

            if (iLen >= 29)
            {
                fTooLong = true;
                iLen = 26;
            }

            ASSERT (iLen < cchBuf);
            *pszBuf = '\"';
            wcsncpy_s (pszBuf + 1, cchBuf - 1, pszStr, iLen);
            StringCchCopyW (
                pszBuf + iLen + 1,
                cchBuf - iLen - 1,
                fTooLong ? L"..." : L"\"");
            return pszBuf;
        }

        default:
            return GetTokenInfo (m_pTokens[iTokenIdx].Token())->pszText;
    }

    ASSERT( !"How did we get here?");
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ErrorInvalidMemberDecl

void CParser::ErrorInvalidMemberDecl(long iTokenIdx)
{
    // An invalid token was found in a member declaration. If the token is a modifier, give a special
    // error message that seems better                                              
    if (GetTokenInfo (m_pTokens[iTokenIdx].Token())->dwFlags & TFF_MODIFIER)
        Error (ERR_BadModifierLocation, GetTokenText(iTokenIdx));
    else
        Error (ERR_InvalidMemberDecl, GetTokenText(iTokenIdx));
}

////////////////////////////////////////////////////////////////////////////////
// CParser::SkipToNamespaceElement
//
// Skip ahead to something that looks like it could be the start of a namespace
// element.  Return TRUE if one is found; or FALSE if end-of-file or unmatched
// close-curly is found first.

BOOL CParser::SkipToNamespaceElement ()
{
    long    iCurlyCount = 0;

    while (TRUE)
    {
        TOKENID iTok = NextToken();

        if (m_rgTokenInfo[iTok].dwFlags & TFF_NSELEMENT)
            return TRUE;

        switch (iTok)
        {
            case TID_IDENTIFIER:
                if (m_pTokens[CurTokenIndex()].Name() == SpecName(SN_PARTIAL) &&
                    (m_rgTokenInfo[PeekToken()].dwFlags & TFF_NSELEMENT))
                {
                    return TRUE;
                }
                break;

            case TID_NAMESPACE:
                return TRUE;

            case TID_OPENCURLY:
                iCurlyCount++;
                break;

            case TID_CLOSECURLY:
                if (iCurlyCount-- == 0)
                    return FALSE;
                break;

            case TID_ENDFILE:
                return FALSE;

            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CParser::SkipToMember
//
// Skip ahead to something that looks like it could be the start of a member.
// Return TRUE if one is found; or FALSE if end-of-file or unmatched close-curly
// is found first.

BOOL CParser::SkipToMember ()
{
    long    iCurlyCount = 0;

    while (TRUE)
    {
        TOKENID iTok = NextToken();

        // If this token can start a member, we're done
        if ((m_rgTokenInfo[iTok].dwFlags & TFF_MEMBER) && 
            !(iTok == TID_DELEGATE && (PeekToken() == TID_OPENCURLY || PeekToken() == TID_OPENPAREN))) {
            return TRUE;
        }


        // Watch curlies and look for end of file/close curly
        switch (iTok)
        {
            case TID_OPENCURLY:
                iCurlyCount++;
                break;

            case TID_CLOSECURLY:
                if (iCurlyCount-- == 0)
                    return FALSE;
                break;

            case TID_ENDFILE:
                return FALSE;

            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CParser::SkipBlock
//
// The purpose of this function is to skip method/constructor/destructor/accessor
// bodies during the top-level parse.  The trick is that while scanning for the
// matching close-curly, we check for patterns that indicate a missing close-
// curly situation.  These patterns include class member declarations, as well
// as namespace member declarations.

DWORD CParser::SkipBlock (DWORD dwFlags, long *piClose)
{
    // We're always called with the open curly as the current token...
    Eat (TID_OPENCURLY);

    long    iCurlyCount = 1;

    for (;;NextToken())
    {
        TOKENID iTok = CurToken();
        DWORD   dwTokFlags = m_rgTokenInfo[iTok].dwFlags;

        if (iTok == TID_OPENCURLY)
        {
            // Increment curly nesting
            iCurlyCount++;
            continue;
        }

        if (iTok == TID_CLOSECURLY)
        {
            // Decrement curly nesting, and bail if 0
            iCurlyCount--;
            if (iCurlyCount == 0)
            {
                // Here's the end of our block.  Remember the close token
                // index, and indicate to the caller that this was a normal skip.
                if (piClose != NULL)
                    *piClose = CurTokenIndex();
                NextToken();
                if (CurToken() == TID_SEMICOLON) {
                    Error (ERR_UnexpectedSemicolon);
                }
                return SB_NORMALSKIP;
            }
            continue;
        }

        if (iTok == TID_ENDFILE)
        {
            // Whoops!  We hit the end of the file without finding our match.
            // Report the error and indicate what happened to the caller.
            if (piClose != NULL)
                *piClose = CurTokenIndex();
            CheckToken (TID_CLOSECURLY);
            return SB_ENDOFFILE;
        }

        if (iTok == TID_NAMESPACE)
        {
            if (piClose != NULL)
                *piClose = CurTokenIndex();
            CheckToken (TID_CLOSECURLY);
            return SB_NAMESPACEMEMBER;
        }

        if (dwTokFlags & TFF_MEMBER)
        {
            DWORD       dwScan;
            long        iMark = CurTokenIndex();

            // This token can start a type member declaration.  Scan it to see
            // if it really looks like a member declaration that is NOT also a
            // valid statement (such as a local variable declaration)
            dwScan = ScanMemberDeclaration (dwFlags);
            Rewind (iMark);
            if (dwScan != SB_NOTAMEMBER)
            {
                // This is a member declaration, so it's out of place here.
                // Assume a close-curly is missing, and indicate what we found
                // to the caller
                if (piClose != NULL)
                    *piClose = CurTokenIndex();
                CheckToken (TID_CLOSECURLY);
                return dwScan;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanMemberName
//
// Scans a member name. Does NOT handle:
//      operators
//      conversions
//      constructors
//      destructors
//      nested types
//
// Return Value:
//        GENERIC_METHOD_NAME,                // alias::I<R>.M<T>
//        INDEXER_NAME,                       // alias::I<R>.this
//        PROPERTY_OR_EVENT_OR_METHOD_NAME,   // alias::I<R>.M
//        SIMPLE_NAME,                        // M
//        NOT_MEMBER_NAME,                    // anything else
//
// Leaves the current token after the member name.

MemberNameEnum CParser::ScanMemberName()
{
    if (!IsNameStart(CurToken()))
    {
        if (CurToken() == TID_THIS)
        {
            NextToken();
            return MemberName::IndexerName;
        }
        return MemberName::NotMemberName;
    }

    ASSERT(CurToken() == TID_IDENTIFIER);
    // simplest case of a single id
    TOKENID tok = PeekToken();
    if (tok != TID_DOT && tok != TID_OPENANGLE && tok != TID_COLONCOLON)
    {
        NextToken();
        return MemberName::SimpleName;
    }

    while (true)
    {
        if (CurToken() == TID_THIS)
        {
            NextToken();
            return MemberName::IndexerName;
        }

        ScanNamedTypePartEnum typePart = ScanNamedTypePart();
        if (typePart == ScanNamedTypePart::NotName)
        {
            return MemberName::NotMemberNameWithDot;
        }

        if (CurToken() != TID_DOT && CurToken() != TID_COLONCOLON)
        {
            if (typePart == ScanNamedTypePart::SimpleName)
                return MemberName::PropertyOrEventOrMethodName;
            else
                return MemberName::GenericMethodName;
        }
        NextToken();
    }
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanMemberDeclaration
//
// This function is used by SkipBlock to check to see if the current token is
// the start of a valid type member declaration that is NOT a valid statement.
// Note that field declarations look like local variable declarations, so only
// certain kinds of fields will trigger this (such as const, or those with an
// access modifier such as 'public').  But methods, constructors, properties,
// etc., will always return TRUE provided that they are correct/complete enough
// (i.e. up to their own block or semicolon).  If we're inside a property
// accessor (indicated by dwSkipFlags) then we also check for another accessor.
//
// Note that the current token is UNDEFINED after calling this function...

DWORD CParser::ScanMemberDeclaration (DWORD dwSkipFlags)
{
    // First, check to see if we're in an accessor and handle detection of
    // another one.
    if (dwSkipFlags & SBF_INACCESSOR)
    {
        long    iMark = CurTokenIndex();

        // The situation we're trying to detect here is a missing close-curly
        // before the second accessor in a property declaration.
        while (m_rgTokenInfo[CurToken()].dwFlags & TFF_MODIFIER)
            NextToken();

        if (CurToken() == TID_IDENTIFIER && PeekToken() == TID_OPENCURLY)
        {
            NAME    *pName = m_pTokens[CurTokenIndex()].Name();

            if (pName == SpecName(SN_GET) || pName == SpecName(SN_SET) || pName == SpecName(SN_ADD) || pName == SpecName(SN_REMOVE))
                return SB_ACCESSOR;
        }

        // Not an accessor...
        Rewind (iMark);
    }

    // The remaining logic in this function should closely match that in
    // ParseMember; it just doesn't construct a parse tree.

    // The [ token will land us here, but we don't look at attributes.  That means
    // if a member DOES has attributes, we'll miss them -- but we don't care, since
    // that only means an error occurred so we won't do anything with them anyway.
    // If we cared enough, we could scan backwards for attributes if we found a
    // member declaration -- but I don't think it's worth it.
    if (CurToken() == TID_OPENSQUARE)
        return SB_NOTAMEMBER;

    BOOL        fHadModifiers = FALSE;

    // Skip any modifiers, remembering if there were any
    while ((m_rgTokenInfo[CurToken()].dwFlags & TFF_MODIFIER) ||
        CheckForSpecName(SN_PARTIAL) && (m_rgTokenInfo[PeekToken()].dwFlags & (TFF_MODIFIER | TFF_NSELEMENT)))
    {
        switch (CurToken()) {
        // These guys are really unlikely to be put in a method body
        // and so they indicate that this is probably a real modifier on a real member
        case TID_PRIVATE: 
        case TID_PROTECTED: 
        case TID_INTERNAL: 
            {
                //
                if (dwSkipFlags & SBF_INPROPERTY) 
                {
                    break;
                }
            }
            //fall through
        case TID_PUBLIC:
        case TID_SEALED: 
        case TID_ABSTRACT: 
        case TID_VIRTUAL: 
        case TID_EXTERN: 
        case TID_OVERRIDE: 
        default:
            fHadModifiers = TRUE;
            break;
        // These are likely to appear inside a method as local modifiers (especially for C/C++
        // programmers) and so don't treat it as a modifier until we see something that couldn't be inside a method body
        case TID_STATIC: case TID_READONLY: case TID_VOLATILE: case TID_UNSAFE: case TID_NEW:
            break;
        }

        NextToken();
    }

    // See if this looks like a constructor (or method w/ missing type)
    if (CurToken() == TID_IDENTIFIER && PeekToken() == TID_OPENPAREN)
    {
        BOOL    fHadParms = FALSE;

        // Skip to the open paren and scan an argument list.  If we don't
        // get a valid one, this is not a member.
        NextToken();
        if (!ScanParameterList (false, &fHadParms))
            return SB_NOTAMEMBER;

        // NOTE:  ScanParameterList leaves the current token at what follows
        // the close paren if it was successful...

        // If the parameter list had confirmed parameters, then this MUST
        // have been a method/ctor declaration.  Also, if followed by an
        // open curly, it could only have been a method/ctor.
        
        // [grantri, 7/31/2002] - this isn't true anymore because of anonymous delegates!
        // an anonymous delegate looks almost identical to a constructor!!!
        // so we can't check for the open curly
        if (fHadParms || (fHadModifiers && CurToken() == TID_OPENCURLY))
            return SB_TYPEMEMBER;

        // Check for ":[this|base](" -- which clearly indicates a constructor
        if (CurToken() == TID_COLON && (PeekToken(1) == TID_THIS || PeekToken(1) == TID_BASE) && PeekToken(2) == TID_OPENPAREN)
            return SB_TYPEMEMBER;

        // If followed by a semicolon, and there were modifiers,
        // it must have been a member
        if (CurToken() == TID_SEMICOLON && fHadModifiers)
            return SB_TYPEMEMBER;

        // This wasn't a member...
        return SB_NOTAMEMBER;
    }

    if (CurToken() == TID_TILDE)
    {
        // Check for destructor
        if (PeekToken(1) == TID_IDENTIFIER && PeekToken(2) == TID_OPENPAREN && PeekToken(3) == TID_CLOSEPAREN && PeekToken(4) == TID_OPENCURLY)
            return SB_TYPEMEMBER;
        return SB_NOTAMEMBER;
    }

    if (CurToken() == TID_CONST)
    {
        // If preceded by modifiers, this must be a member
        if (fHadModifiers)
            return SB_TYPEMEMBER;

        // Can't tell a const member from a const local...
        // So keep looking
        return SB_NOTAMEMBER;
    }

    if (CurToken() == TID_EVENT)
    {
        // Events are always SB_TYPEMEMBER
        return SB_TYPEMEMBER;
    }

    if (m_rgTokenInfo[CurToken()].dwFlags & TFF_TYPEDECL)
    {
        // Nested types ALWAYS trigger SB_TYPEMEMBER (unless it's an anonymous method)
        if (CurToken() == TID_DELEGATE && (PeekToken() == TID_OPENPAREN || PeekToken() == TID_OPENCURLY))
            return SB_NOTAMEMBER;
        return SB_TYPEMEMBER;
    }

    if ((CurToken() == TID_IMPLICIT || CurToken() == TID_EXPLICIT) && PeekToken() == TID_OPERATOR)
    {
        // Conversion operators -- look no further
        return SB_TYPEMEMBER;
    }

    // Everything else must have a [return] type
    if (ScanType() == ScanTypeFlags::NotType)
        return SB_NOTAMEMBER;

    // Anything that had modifiers is considered a member
    if (fHadModifiers)
        return SB_TYPEMEMBER;

    // Operators...
    if (CurToken() == TID_OPERATOR)
        return SB_TYPEMEMBER;

    // Indexers...
    if (CurToken() == TID_THIS) 
    {
        long iMark = CurTokenIndex();
        NextToken();
        if (ScanParameterList(true, NULL))
        {
            Rewind(iMark);
            return SB_TYPEMEMBER;
        }
        return SB_NOTAMEMBER;
    }

    // NOTE (peterhal): can't have a dot here
    BOOL fNotAProperty = false;
    switch (ScanMemberName())
    {
    case MemberName::NotMemberName:
    case MemberName::NotMemberNameWithDot:
    default:
        return SB_NOTAMEMBER;

    case MemberName::GenericMethodName:
        fNotAProperty = true;
        break;

    case MemberName::PropertyOrEventOrMethodName:
    case MemberName::SimpleName:
        break;

    case MemberName::IndexerName:
        if (CurToken() == TID_OPENSQUARE)
            return SB_TYPEMEMBER;       // Explicit interface impl of indexer
        else
            return SB_NOTAMEMBER;
    }

    if (CurToken() == TID_OPENPAREN)
    {
        BOOL    fHadArgs = FALSE;

        // See if this really is a method
        if (!ScanParameterList (false, &fHadArgs))
            return SB_NOTAMEMBER;

        if (CurToken() == TID_OPENCURLY || CheckForSpecName(SN_WHERE))
            return SB_TYPEMEMBER;

        return SB_NOTAMEMBER;
    }

    if (!fNotAProperty && CurToken() == TID_OPENCURLY)
    {
        // Check to see if this really looks like a property
        NextToken();
        while (m_rgTokenInfo[CurToken()].dwFlags & TFF_MODIFIER)
            NextToken();
        if (CurToken() == TID_IDENTIFIER)
        {
            NAME    *pName = m_pTokens[CurTokenIndex()].Name();

            if (pName == SpecName(SN_GET) || pName == SpecName(SN_SET) || pName == SpecName(SN_ADD) || pName == SpecName(SN_REMOVE))
                return SB_TYPEMEMBER;
        }
        return SB_NOTAMEMBER;
    }

    return SB_NOTAMEMBER;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanParameterList

BOOL CParser::ScanParameterList (BOOL fIndexer, BOOL *pfHadParms)
{
    // NOTE:  This function completely barfs on parameters with attributes.  Need
    // the ability to scan an expression, which we don't (yet) have...
    // NOTE:  barfing on attributes is exactly what we want when we call this to
    // detect anonymous methods, so if you change this to recognize attributes
    // you might break anonymous methods

    int tidOpenParen = fIndexer ? TID_OPENSQUARE : TID_OPENPAREN;
    int tidCloseParen = fIndexer ? TID_CLOSESQUARE : TID_CLOSEPAREN;


    if (CurToken() != tidOpenParen)
        return FALSE;

    NextToken();
    if (CurToken() != tidCloseParen)
    {
        while (TRUE)
        {
            if (CurToken() == TID_ARGS)
            {
                NextToken();
                break;
            }

            if (CurToken() == TID_PARAMS)
                NextToken();

            if (CurToken() == TID_REF || CurToken() == TID_OUT)
                NextToken();

            ScanTypeFlagsEnum st = ScanType();

            if (st == ScanTypeFlags::NotType)
                return FALSE;

            if (CurToken() != TID_IDENTIFIER)
                return FALSE;

            if (st == ScanTypeFlags::MustBeType || st == ScanTypeFlags::AliasQualName)
                if (pfHadParms) 
                    *pfHadParms = TRUE;

            NextToken();
            if (CurToken() == TID_COMMA)
                NextToken();
            else
                break;
        }
    }

    if (CurToken() == tidCloseParen)
    {
        NextToken();
        return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanCast
//
// This function returns TRUE if the current token begins the following form:
//
//  ( <type> ) <term-or-unary-operator>

BOOL CParser::ScanCast ()
{
    if (CurToken() != TID_OPENPAREN)
        return FALSE;

    NextToken();
    ScanTypeFlagsEnum isType = ScanType();
    if (isType == ScanTypeFlags::NotType)
        return FALSE;

    if (CurToken() != TID_CLOSEPAREN)
        return FALSE;

    if (isType == ScanTypeFlags::PointerOrMult || isType == ScanTypeFlags::NullableType)
        return TRUE;

    NextToken();

    //
    // check for ambiguous type or expression followed by disambiguating token
    //
    //                  - or -
    //
    // non-ambiguous type
    //
    if ((IsAnyTypeOrExpr(isType) && !(m_rgTokenInfo[CurToken()].dwFlags & TFF_CASTEXPR)) || 
        (isType == ScanTypeFlags::MustBeType || isType == ScanTypeFlags::AliasQualName))
    {
        return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////

bool CParser::ScanOptionalInstantiation() 
{
    if (CurToken() == TID_OPENANGLE) {
        do
        {
            NextToken();
            
            // We currently do not have the ability to scan attributes, so if this is an open square, we early out and assume it is an attribute
            if (CurToken() == TID_OPENSQUARE)
                return true;

            if (ScanType() == ScanTypeFlags::NotType) 
                return false;
        } while (CurToken() == TID_COMMA);

        if (CurToken() != TID_CLOSEANGLE)
            return false;
        NextToken();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanType
//
// Returns TRUE if current token begins a valid type construct (doesn't build a
// parse tree for it)

ScanTypeFlagsEnum CParser::ScanType ()
{
    ScanTypeFlagsEnum result = ScanTypeFlags::Unknown;
    if (IsNameStart(CurToken()))
    {
        ScanNamedTypePartEnum partResult = ScanNamedTypePart();
        if (partResult == ScanNamedTypePart::NotName)
            return ScanTypeFlags::NotType;
        if (result != ScanTypeFlags::MustBeType)
            result = PartScanToTypeScan(partResult);

        bool fIsAlias = (CurToken() == TID_COLONCOLON);
        // Scan a name
        for (bool fFirstLoop = true; (CurToken() == TID_DOT || CurToken() == TID_COLONCOLON); fFirstLoop = false)
        {
            if (!fFirstLoop && fIsAlias)
                fIsAlias = false;
            NextToken();
            
            partResult = ScanNamedTypePart();

            if (partResult == ScanNamedTypePart::NotName)
                return ScanTypeFlags::NotType;
            
            result = PartScanToTypeScan(partResult);
        }

        if (fIsAlias)
            result = ScanTypeFlags::AliasQualName;
    }
    else if (m_rgTokenInfo[CurToken()].dwFlags & TFF_PREDEFINED)
    {
        // Simple type...
        NextToken();
        result = ScanTypeFlags::MustBeType;
    }
    else
    {
        // Can't be a type!
        return ScanTypeFlags::NotType; 
    }

    ASSERT(result != ScanTypeFlags::Unknown);

    if (CurToken() == TID_QUESTION) {
        NextToken();
        result = ScanTypeFlags::NullableType;
    }

    // Now check for pointer type(s)
    while (CurToken() == TID_STAR)
    {
        NextToken();
        if (IsAnyTypeOrExpr(result))
            result = ScanTypeFlags::PointerOrMult;
    }

    // Finally, check for array types and nullables.
    while (CurToken() == TID_OPENSQUARE) {
        NextToken();
        if (CurToken() != TID_CLOSESQUARE) {
            while (CurToken() == TID_COMMA)
                NextToken();
            if (CurToken() != TID_CLOSESQUARE)
                return ScanTypeFlags::NotType;
        }
        NextToken();
        result = ScanTypeFlags::MustBeType;
    }

    return result;
}

ScanNamedTypePartEnum CParser::ScanNamedTypePart()
{
    if (CurToken() != TID_IDENTIFIER)
    {
        return ScanNamedTypePart::NotName;
    }
    
    if (NextToken() == TID_OPENANGLE) 
    {
        if (!ScanOptionalInstantiation()) 
            return ScanNamedTypePart::NotName;
        return ScanNamedTypePart::GenericName;
    }
    else
    {
        return ScanNamedTypePart::SimpleName;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ScanStringLiteral
//
// Returns the STRCONST for either a TID_STRINGLIT or TID_VSLITERAL string, which
// was constructed at lex time.

STRCONST *CParser::ScanStringLiteral (long iTokenIndex)
{
    if (m_pTokens[iTokenIndex].Token() == TID_VSLITERAL)
        return &(m_pTokens[iTokenIndex].VSLiteral()->str);

    return &(m_pTokens[iTokenIndex].StringLiteral()->str);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanCharLiteral
//
// Scan a character literal and return its value.

WCHAR CParser::ScanCharLiteral (long iTokenIndex)
{
    return m_pTokens[iTokenIndex].CharValue();
}

// Include the code stolen from COM+ for numeric conversions...
namespace NumLit
{
#include "numlit.h"
};

using namespace NumLit;

////////////////////////////////////////////////////////////////////////////////
// CParser::ScanNumericLiteral
//
// Re-scan a number token and convert it to a numeric value, filling in the
// provided CONSTVALNODE structure.

void CParser::ScanNumericLiteral (long iTokenIndex, CONSTVALNODE *pConst)
{
    // Create a working copy of the number (removing any escapes)
    PCWSTR  pszToken = m_pTokens[iTokenIndex].Literal()->szText;
    long    iLength = m_pTokens[iTokenIndex].Literal()->iSourceLength;
    PSTR    pszBuf = STACK_ALLOC (char, iLength + 1);
    PSTR    psz = pszBuf;
    PCWSTR  p = pszToken, pEnd = pszToken + iLength;
    WCHAR   ch = 0, chNonLower;
    BOOL    fReal = FALSE;
    BOOL    fExponent = FALSE;
    BOOL    fNonZero = FALSE;       // for floating point only
    BOOL    fUnsigned = FALSE;
    BOOL    fLong = FALSE;

    while (p < pEnd)
    {
        *psz++ = (char)(ch = *p++);
        switch (ch)
        {
        case '.':
            fReal = TRUE;
            break;
        case 'e':
        case 'E':
            fExponent = TRUE;
            break;
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            fNonZero = fNonZero || !fExponent;
            break;
        }
    }

    *psz = 0;

    // At this point, ch will contain the suffix, if any.  Make it the lower-case version
    chNonLower = ch;  // save away possible uppercase version (just for warning purposes)
    ch |= 0x20;

    TOKENID iTypeTok = (TOKENID) -1;
    BOOL    fHex = (pszBuf[0] == '0' && (pszBuf[1] == 'x' || pszBuf[1] == 'X'));

    // For error reporting, determine the type name of this constant based on the
    // suffix
    switch (ch)
    {
        case 'd':
            if (!fHex)
            {
                iTypeTok = TID_DOUBLE;
                fReal = TRUE;
            }
            break;

        case 'f':
            if (!fHex)
            {
                iTypeTok = TID_FLOAT;
                fReal = TRUE;
            }
            break;

        case 'm':   iTypeTok = TID_DECIMAL; fReal = TRUE;       break;
        case 'l':
            fLong = TRUE;
            fUnsigned = (pszBuf + 2 <= psz && (psz[-2] | 0x20) == 'u');
            if (chNonLower == 'l' && !fUnsigned)
                Error(WRN_LowercaseEllSuffix); // warn about 'l' instead of 'L' (but not 'ul' -- that not a problem)
            break;
        case 'u':
            fUnsigned = TRUE;
            fLong = (pszBuf + 2 <= psz && (psz[-2] | 0x20) == 'l');
            if (fLong && psz[-2] == 'l')
                Error(WRN_LowercaseEllSuffix); // warn about 'l' instead of 'L'
            break;
        default:    iTypeTok = (!fHex && (fReal || fExponent)) ? TID_DOUBLE : TID_INT; break;
    }

    if (fReal && fHex)
    {
        Error (ERR_InvalidNumber);
        return;
    }

    if (fHex)
    {
        // Hex number.
        unsigned __int64     val = 0;

        for (PCSTR p = pszBuf+2; IsHexDigit (*p); p++)
        {
            if (val & UI64(0xf000000000000000))
            {
                Error (ERR_IntOverflow);
                break;
            }
            val = (val << 4) | HexValue (*p);
        }

        if (pszBuf[2] == '\0')
            Error (ERR_InvalidNumber);
        if (!fLong && ((val >> 32) == 0)) {
            if (fUnsigned || ((__int32)val < 0)) {
                pConst->other = PT_UINT;
                iTypeTok = TID_UINT;
            } else {
                pConst->other = PT_INT;
                iTypeTok = TID_INT;
            }
            pConst->val.uiVal = (unsigned int)val;
        } else {
            if (fUnsigned || ((__int64)val < 0)) {
                pConst->other = PT_ULONG;
                iTypeTok = TID_ULONG;
            } else {
                pConst->other = PT_LONG;
                iTypeTok = TID_LONG;
            }
            pConst->val.ulongVal = (unsigned __int64 *)MemAlloc (sizeof (unsigned __int64));
            *pConst->val.ulongVal = val;
        }

        return;
    }

    if (fReal || fExponent)
    {
        // Floating point number.  If it is a decimal, parse it as such to preserve significant digits.
        // Otherwise:  Check and remove the suffix (if there) and scan it as a double,
        // then downcast to float if the suffix is 'f'.
        if (iTypeTok == TID_DECIMAL)
        {
            // It's a decimal.  Need to allocate space for it
            pConst->other = PT_DECIMAL;
            pConst->val.decVal = (DECIMAL *)MemAlloc (sizeof (DECIMAL));

            NUMBER num;
            PCSTR pszTmp = pszBuf;
            if (!::ParseDecimalAsNumber(&pszTmp, &num) || ((*pszTmp | 0x20) != 'm') || !::NumberToDecimal(&num, pConst->val.decVal))
            {
                Error(ERR_FloatOverflow, m_rgTokenInfo[iTypeTok].pszText);
            }
            return;
        }

        // otherwise it is either a double or a float
        ASSERT((iTypeTok == TID_FLOAT) || (iTypeTok == TID_DOUBLE));

        double dbl;
        bool errConvert;

        // Convert to wide characters -- all chars are in ASCII range so no need to use MultiByteToWideChar.
        PWSTR    pwszBuf  = STACK_ALLOC (WCHAR, strlen(pszBuf) + 1);
        PWSTR    pwszTemp = pwszBuf;
        PSTR     pszTemp  = pszBuf;

        char chLast = *(psz - 1);
        if (chLast == 'f' || chLast == 'F' || chLast == 'd' || chLast == 'D')
        {
            // remove the type suffix
            *(--psz) = 0;
        }

        while ((*pwszTemp++ = *pszTemp++) != 0)
            ;

        if (fNonZero) {
            // Convert to a double, set errConvert on error. Note that we can't use atof here because it is sensitive to setlocale,
            // and we can't call setlocale ourselves because it is process wide instead of per-thread. (VS7:181428)
            if (FAILED(VarR8FromStr(pwszBuf, MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), LOCALE_NOUSEROVERRIDE, &dbl))) {
                dbl = 0.0;
                errConvert = true;
            }
            else
                errConvert = false;
        }
        else {
            // Zero. Don't send through VarR8FromStr, because of literals like 0E1234 which is actually valid (and is zero).
            dbl = 0.0;
            errConvert = false;
        }

        if (iTypeTok == TID_FLOAT)
        {
            pConst->other = PT_FLOAT;
            // Allocate space for it.
            pConst->val.doubleVal = (double *)MemAlloc (sizeof (double));

            // Force to R4 precision/range.
            float f;
            RoundToFloat(dbl, &f);

            if (errConvert || !_finite(f))
            {
                Error(ERR_FloatOverflow, m_rgTokenInfo[iTypeTok].pszText);
            }
            *(pConst->val.doubleVal) = f;
        }
        else
        {
            // It's a double.  Need to allocate space for it
            ASSERT(iTypeTok == TID_DOUBLE);
            pConst->other = PT_DOUBLE;
            pConst->val.doubleVal = (double *)MemAlloc (sizeof (double));
            *(pConst->val.doubleVal) = dbl;
            if (errConvert || !_finite(dbl))
            {
                Error(ERR_FloatOverflow, m_rgTokenInfo[iTypeTok].pszText);
            }
        }

        return;
    }

    // Must be simple integral form.  We always have to 64bit math because literals can
    // be ints or longs
    unsigned __int64 val = 0;

    for (PCSTR p = pszBuf; *p >= '0' && *p <= '9'; p++)
    {
        if (val > UI64(1844674407370955161) || (val == UI64(1844674407370955161) && *p > '5'))
        {
            Error (ERR_IntOverflow);
            break;
        }
        val = val * 10 + (*p - '0');
    }

    if (fUnsigned) {
        if (fLong || ((val >> 32) != 0)) {
            pConst->other = PT_ULONG;
            pConst->val.ulongVal = (unsigned __int64 *)MemAlloc (sizeof (unsigned __int64));
            *pConst->val.ulongVal = val;
            iTypeTok = TID_ULONG;
        } else {
            pConst->other = PT_UINT;
            pConst->val.uiVal = (unsigned int)val;
            iTypeTok = TID_UINT;
        }
    } else {
        if (!fLong && ((val >> 32) == 0)) {
            if ((__int32)val >= 0) {
                pConst->other = PT_INT;
                pConst->val.iVal = (int)val;
                iTypeTok = TID_INT;
            } else {
                pConst->other = PT_UINT;
                pConst->val.uiVal = (unsigned int)val;
                iTypeTok = TID_UINT;
                if (val == 0x80000000)
                    pConst->flags |= NF_CHECK_FOR_UNARY_MINUS;
            }
        } else if ((__int64)val >= 0) {
            pConst->other = PT_LONG;
            pConst->val.longVal = (__int64 *)MemAlloc (sizeof (__int64));
            *pConst->val.longVal = (__int64)val;
            iTypeTok = TID_LONG;
        } else {
            pConst->other = PT_ULONG;
            pConst->val.ulongVal = (unsigned __int64 *)MemAlloc (sizeof (unsigned __int64));
            *pConst->val.ulongVal = val;
            iTypeTok = TID_ULONG;
            if (val == UI64(0x8000000000000000))
                pConst->flags |= NF_CHECK_FOR_UNARY_MINUS;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// CParser::AllocNode
//
// This is the base node allocator.  Based on the node kind provided, the right
// amount of memory is allocated from the virtual allocation function MemAlloc.

BASENODE *CParser::AllocNode (NODEKIND kind, BASENODE * nodePar, long itok)
{

    byte * pb;
    BASENODE * pNew;
    int cbExtra = 0;

    switch (kind)
    {
#ifdef DEBUG
    #define NODEKIND(n,s,g,p) \
            case NK_##n: \
                pb = (byte *)MemAlloc (sizeof (s##NODE) + cbExtra); \
                pNew = (s##NODE*)(pb + cbExtra); \
                new (pNew) s##NODE; \
                pNew->pszNodeKind = #s "NODE (NK_" #n ")"; \
                pNew->fHasNid = !!cbExtra; \
                break;
#else
        #define NODEKIND(n,s,g,p) \
            case NK_##n: \
                pb = (byte *)MemAlloc (sizeof (s##NODE) + cbExtra); \
                pNew = (s##NODE*)(pb + cbExtra); \
                break;
#endif

        #include "nodekind.h"

        default:
            VSFAIL ("Unknown node kind passed to AllocNode!");
            pb = (byte *)MemAlloc (sizeof (BASENODE) + cbExtra);
            pNew = (BASENODE*)(pb + cbExtra);
            break;
    }

    pNew->kind = kind;
    pNew->flags = 0;
    pNew->other = 0;
    pNew->tokidx = itok;
    pNew->pParent = nodePar;

    return pNew;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::AllocDotNode

BINOPNODE *CParser::AllocDotNode (long iTokIdx, BASENODE *pParent, BASENODE *p1, BASENODE *p2)
{
    BINOPNODE   *pOp = AllocNode(NK_DOT, pParent, iTokIdx)->asDOT();
    pOp->p1 = p1;
    pOp->p2 = p2;
    p1->pParent = p2->pParent = pOp;
    return pOp;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::AllocBinaryOpNode

BINOPNODE *CParser::AllocBinaryOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent, BASENODE *p1, BASENODE *p2)
{
    ASSERT(p1 && p2);
    BINOPNODE   *pOp = AllocNode(NK_BINOP, pParent, iTokIdx)->asBINOP();
    pOp->p1 = p1;
    pOp->p2 = p2;
    pOp->other = op;
    ASSERT (p1 != NULL);
    p1->pParent = pOp;
    if (p2 != NULL)
        p2->pParent = pOp;
    return pOp;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::AllocUnaryOpNode

UNOPNODE *CParser::AllocUnaryOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent, BASENODE *p1)
{
    ASSERT(p1);
    UNOPNODE    *pOp = AllocNode(NK_UNOP, pParent, iTokIdx)->asUNOP();
    pOp->p1 = p1;
    pOp->other = op;
    p1->pParent = pOp;
    return pOp;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::AllocOpNode

BASENODE *CParser::AllocOpNode (OPERATOR op, long iTokIdx, BASENODE *pParent)
{
    BASENODE    *pOp = AllocNode(NK_OP, pParent, iTokIdx)->asOP();
    pOp->other = op;
    return pOp;
}

////////////////////////////////////////////////////////////////////////////////

NAMENODE *CParser::AllocNameNode (NAME *pName, long iTokIdx)
{
    NAMENODE * pNameNode = AllocNode(NK_NAME, NULL, iTokIdx)->asNAME();
    InitNameNode(pNameNode, pName);
    return pNameNode;
}

////////////////////////////////////////////////////////////////////////////////

NAMENODE *CParser::InitNameNode (NAMENODE *pNameNode, NAME *pName)
{
    ASSERT(0 <= pNameNode->tokidx && pNameNode->tokidx < m_iTokens);

    if (pName == NULL)
        pNameNode->pName = m_pTokens[pNameNode->tokidx].Name();
    else
        pNameNode->pName = pName;
    if (m_pTokens[pNameNode->tokidx].iUserBits & TF_VERBATIMSTRING)
        pNameNode->flags |= NF_NAME_LITERAL;

    pNameNode->pPossibleGenericName = NULL;

    return pNameNode;
}

////////////////////////////////////////////////////////////////////////////////

GENERICNAMENODE *CParser::AllocGenericNameNode (NAME *pName, long iTokIdx)
{
    GENERICNAMENODE    *pNameNode = AllocNode(NK_GENERICNAME, NULL, iTokIdx)->asGENERICNAME();
    InitNameNode(pNameNode, pName);
    pNameNode->iOpen = -1;
    pNameNode->iClose = -1;
    return pNameNode;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ParseSourceModule

BASENODE *CParser::ParseSourceModule ()
{
    VSASSERT(m_pTokens != NULL, "Parser state not set");

    // Not that it matters, but start the error count at 0
    m_iErrors = 0;
    m_fErrorOnCurTok = FALSE;

    // Source modules always start at the first non-noise token in any stream..
    m_iCurToken = 0; 
    while (m_rgTokenInfo[m_pTokens[m_iCurToken].Token()].dwFlags & TFF_NOISE)
        m_iCurToken++;

    // Every source module belongs in an unnamed namespace...
    NAMESPACENODE   *pNS = ParseRootNamespace ();

    // We're done!  Thus, we should be at end-of-file.
    if (CurToken() != TID_ENDFILE)
        Error (ERR_EOFExpected);

    return pNS;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseRootNamespace

NAMESPACENODE *CParser::ParseRootNamespace ()
{
    NAMESPACENODE   *pNS = AllocNode(NK_NAMESPACE, NULL, -1)->asNAMESPACE();
    pNS->pName = NULL;
    pNS->pGlobalAttr = NULL;
    pNS->pKey = GetNameMgr()->GetPredefName (PN_EMPTY);
    pNS->iOpen = pNS->iClose = -1;
    pNS->pNameSourceFile = m_pNameSourceFile;

    // ...and the contents of the source is just a namespace body
    ParseNamespaceBody (pNS);

    // We're done!  Thus, we should be at end-of-file.
    pNS->iClose = CurTokenIndex();
    return pNS;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseNamespaceBody

void CParser::ParseNamespaceBody (NAMESPACENODE *pNS)
{
    // A namespace body consists of (in order):
    // * zero or more externs
    // * zero or more using clauses
    // * zero or more namespace element declarations
    CListMaker   using_list(this, &pNS->pUsing);

    for (bool fSeenUsing = false; ; ) {
        switch (CurToken()) {
        case TID_EXTERN:
            if (fSeenUsing) {
                // Externs must come before usings.
                Error (ERR_ExternAfterElements);
            }
            using_list.Add (ParseExternAliasClause (pNS));
            continue;
        case TID_USING:
            fSeenUsing = true;
            using_list.Add (ParseUsingClause (pNS));
            continue;
        default:
            break;
        }
        break;
    }

    // The global (file level) namespace can also contain 'global' attributes
    if (NULL == pNS->pParent)
        pNS->pGlobalAttr = ParseGlobalAttributes(pNS);

    CListMaker elem_list(this, &pNS->pElements);
    for (;;) {
        TOKENID iTok = CurToken();

        if (m_rgTokenInfo[iTok].dwFlags & TFF_NSELEMENT) {
            // This token can start a type declaration
            elem_list.Add (ParseTypeDeclaration (pNS, NULL));
            continue;
        }

        switch (iTok) {
        case TID_NAMESPACE:
            // A nested namespace
            elem_list.Add (ParseNamespace (pNS));
            break;
        case TID_CLOSECURLY:
        case TID_ENDFILE:
            // This token marks the end of a namespace body
            return;
        case TID_USING:
            Error (ERR_UsingAfterElements);
            using_list.Add (ParseUsingClause (pNS));
            break;
        case TID_EXTERN:
            Error (ERR_ExternAfterElements);
            using_list.Add (ParseExternAliasClause (pNS));
            break;
        case TID_IDENTIFIER:
            if (CheckForSpecName(SN_PARTIAL)) {
                if (m_rgTokenInfo[PeekToken()].dwFlags & TFF_NSELEMENT) {
                    // partial can start a type declaration
                    elem_list.Add(ParseTypeDeclaration (pNS, NULL));
                    continue;
                }
                if (PeekToken() == TID_NAMESPACE) {
                    Error(ERR_BadModifiersOnNamespace);
                    NextToken();
                    continue;
                }
            }
            // Fall through.
        default:
            // Whoops, the code is unrecognizable.  Give an error and try to get
            // sync'd up with intended reality
            Error (ERR_NamespaceUnexpected);
            if (!SkipToNamespaceElement())
                return;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::ParseUsingClause (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_USING);

    // Create the using node, checking for namespace keyword
    USINGNODE   *pUsing = AllocNode(NK_USING, pParent)->asUSING();

    NextToken();

    if (CurToken() == TID_IDENTIFIER && PeekToken(1) == TID_EQUAL)
    {
        // Warn on "using global = X".
        if (CheckForSpecName(SN_GLOBAL)) {
            Error(WRN_GlobalAliasDefn);
        }

        // This is a using-alias directive.
        pUsing->pAlias = ParseIdentifier (pUsing);
        Eat (TID_EQUAL);
        pUsing->pName = ParseGenericQualifiedNameList(pUsing, false);
    }
    else
    {
        // This is a using-namespace directive.
        pUsing->pAlias = NULL;
        pUsing->pName = ParseDottedName (pUsing, true);
    }

    Eat (TID_SEMICOLON);
    return pUsing;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseExternAliasClause

BASENODE *CParser::ParseExternAliasClause (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_EXTERN);

    // Create the using node, checking for namespace keyword
    USINGNODE   *pUsing = AllocNode(NK_USING, pParent)->asUSING();

    NextToken();

    if (!CheckForSpecName(SN_ALIAS)) {
        Error(ERR_SyntaxError, SpecName(SN_ALIAS));
    }
    else {
        ReportFeatureUse(IDS_FeatureExternAlias);
        NextToken();
    }

    // Error on "extern alias global".
    if (CheckForSpecName(SN_GLOBAL))
        Error(ERR_GlobalExternAlias);

    pUsing->pAlias = ParseIdentifier (pUsing);
    pUsing->pName = NULL;

    Eat (TID_SEMICOLON);

    return pUsing;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseGlobalAttributes

BASENODE *CParser::ParseGlobalAttributes (BASENODE *pParent)
{
    if (CurToken() != TID_OPENSQUARE)
        return NULL;

    long    iIdent;

    if (TokenAt (iIdent = PeekTokenIndex(1)) > TID_IDENTIFIER || PeekToken (2) != TID_COLON)
        return NULL;

    BASENODE    *pAttr;
    CListMaker  list (this, &pAttr);

    while (TokenAt(iIdent) != TID_RETURN && (TokenAt(iIdent) != TID_IDENTIFIER || m_pTokens[iIdent].Name() != GetNameMgr()->GetPredefName(PN_TYPE)))
    {
        bool fAttributeHadErrorThrowaway;
        list.Add(ParseAttributeSection(pParent, AL_ASSEMBLY, (ATTRLOC) (AL_ASSEMBLY | AL_MODULE), &fAttributeHadErrorThrowaway));

        if (CurToken() != TID_OPENSQUARE || TokenAt(iIdent = PeekTokenIndex(1)) > TID_IDENTIFIER || PeekToken(2) != TID_COLON)
            break;
    }

    return pAttr;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseNamespace

BASENODE *CParser::ParseNamespace (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_NAMESPACE);

    // Create the namespace node
    NAMESPACENODE   *pNS = AllocNode(NK_NAMESPACE, pParent)->asNAMESPACE();
    pNS->pGlobalAttr = NULL;
    pNS->pKey = NULL;
    pNS->pNameSourceFile = NULL;

    NextToken();

    // Grab the name
    pNS->pName = ParseDottedName (pNS, false);

    // Now there should be an open curly
    pNS->iOpen = CurTokenIndex();
    Eat (TID_OPENCURLY);

    // Followed by a namespace body
    ParseNamespaceBody (pNS);

    // And lastly a close curly
    pNS->iClose = CurTokenIndex();
    Eat (TID_CLOSECURLY);

    // optional trailing semi-colon
    if (CurToken() == TID_SEMICOLON)
        NextToken();

    AddToNodeTable (pNS);
    return pNS;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseTypeDeclaration

BASENODE *CParser::ParseTypeDeclaration (BASENODE *pParent, BASENODE *pAttr)
{
    long        tokidx = CurTokenIndex();

    // All types can start with modifiers, so parse them now, IF we weren't given
    // any to start with (it's possible the caller already parsed them for us).
    // ParseAttributes will return NULL if there are none there...
    bool fAttributeHadError = false;
    if (pAttr == NULL)       
        pAttr = ParseAttributes (pParent, AL_TYPE, (ATTRLOC)0, &fAttributeHadError);
    else
    {
        BOOL fTmp;
        tokidx = GetFirstToken(pAttr, EF_FULL, &fTmp);
    }

    // This is the first non-attribute token index
    long iStart = CurTokenIndex();

    //We got an attribute, but the attribute had an error in it.
    //This means we may have consumed too many tokens (like the token "class" in "class Foo")
    //Back up a little to see if we can recover
    if (pAttr != NULL && fAttributeHadError)
    {
        //The attribute had an error.  This means that we may have consumed
        //too many tokens.  Look back a few to see if we can find one that
        //is valid (like TFF_NS_ELEMENT) and the start parsing from there
        //Note: 2 is arbitrary.  Feel free to replace with a better value
        for (int i = 0; i < 2 && CurTokenIndex() > tokidx; i++)
        {
            if (m_rgTokenInfo[CurToken()].dwFlags & TFF_NSELEMENT)
            {
                //ok!  found one!
                iStart = CurTokenIndex();
                break;
            }
            PrevToken();
        }

        //once we've found a NS_ELEMENT token keep on eating
        //ns element tokens before us
        while (CurTokenIndex() > tokidx && (m_rgTokenInfo[CurToken()].dwFlags & TFF_NSELEMENT))
        {
            iStart = CurTokenIndex();
            PrevToken();
            if ( ! (CurTokenIndex() > tokidx && m_rgTokenInfo[CurToken()].dwFlags & TFF_NSELEMENT))
            {
                //We consumed one too many tokens
                NextToken();
                break;
            }
        }
    }
    
    // Type declarations can begin with modifiers 'public', 'private', 'sealed',
    // and 'abstract'.  Not all combinations are legal, but we parse them all
    // into a bitfield anyway.
    unsigned    iMods = ParseModifiers (pParent->kind == NK_NESTEDTYPE ? FALSE : TRUE);


    BASENODE    *pType = NULL;

    if (iMods & NF_MOD_STATIC)
        ReportFeatureUse(IDS_FeatureStaticClasses);

    // The next token should be the one indicating what type to parse (class, enum,
    // struct, interface, or delegate)
    switch (CurToken())
    {
        case TID_CLASS:
        case TID_STRUCT:
        case TID_INTERFACE:
            pAttr = SetDefaultAttributeLocation(pAttr, AL_TYPE, AL_TYPE);
            pType = ParseAggregate (pParent, tokidx, iStart, pAttr, iMods);
            break;


        case TID_DELEGATE:
            pAttr = SetDefaultAttributeLocation(pAttr, AL_TYPE, (ATTRLOC) (AL_TYPE | AL_RETURN));
            pType = ParseDelegate (pParent, tokidx, iStart, pAttr, iMods);
            break;

        case TID_ENUM:
            pAttr = SetDefaultAttributeLocation(pAttr, AL_TYPE, AL_TYPE);
            pType = ParseEnum (pParent, tokidx, iStart, pAttr, iMods);
            break;

        case TID_NAMESPACE:
            if ((iMods || pAttr) && pParent->kind == NK_NAMESPACE) {
                ErrorAtToken (iStart, ERR_BadModifiersOnNamespace);
                pType = ParseNamespace (pParent);
                // We store the attributes, even though this is an error case, so that the IDE can correctly show completion lists.
                pType->asNAMESPACE()->pGlobalAttr = pAttr;                          
                break;
            }
            // Fall-through
        default:
            Error (ERR_BadTokenInType);
            break;
    }

    // NOTE:  If we didn't parse anything, we return NULL.  These are added to
    // lists using the CListMaker object, which deals with a new node of NULL and
    // doesn't mess with the current list.
    //
    //cyrusn 4/1/03
    //If we didn't parse a type we could have still parsed an attribute
    //so we create a dummy type whose only interesting component is the attribute
    //this way we can still do intellisense on attributes in a namespace not
    //attached to a type
    if (NULL == pType && NULL != pAttr) {
        return ParseMissingType(pParent, tokidx, iStart, iMods, pAttr);
    } else {
        return pType; 
    }
}

BASENODE *CParser::ParseMissingType(BASENODE* pParent, int tokidx, int iStart, unsigned iMods, BASENODE* pAttr)
{
    CLASSNODE   *pClass = AllocNode(NK_CLASS, pParent, tokidx)->asCLASS();

    pClass->iStart = iStart;
    pClass->flags = iMods;
    pClass->pAttr = pAttr;
    pClass->pKey = NULL;
    pClass->pName = ParseMissingName(pClass, iStart);
    pClass->pTypeParams = NULL;
    pClass->pBases = NULL;
    pClass->pConstraints = NULL;
    pClass->pMembers = NULL;
    
    //These is the best value I can come up with. It's necessary to set them so that 
    //GetExtent doesn't fail when you ask for the end position.
    pClass->iOpen = CurTokenIndex();
    pClass->iClose = CurTokenIndex();

    pAttr->pParent = pClass;

    AddToNodeTable (pClass);
    return pClass;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseAggregate

BASENODE *CParser::ParseAggregate (BASENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    ASSERT (CurToken() == TID_CLASS || CurToken() == TID_STRUCT || CurToken() == TID_INTERFACE);

    NODEKIND kind;
    switch (CurToken())
    {
    default:
        ASSERT(!"Bad token kind");
        // fall through
        
    case TID_CLASS:
        kind = NK_CLASS;
        break;
    case TID_INTERFACE:
        kind = NK_INTERFACE;
        break;
    case TID_STRUCT:
        kind = NK_STRUCT;
        break;
    }

    AGGREGATENODE   *pClass = AllocNode (kind, pParent, tokidx)->asAGGREGATE();

    pClass->iStart = iStart;
    pClass->flags = iMods;
    pClass->pAttr = pAttr;
    pClass->pKey = NULL;
    if (pAttr != NULL)
        pAttr->pParent = pClass;
    NextToken();

    pClass->pName = ParseIdentifier (pClass);
    pClass->pTypeParams = ParseInstantiation (pClass, true, NULL);
    pClass->pBases = ParseBaseTypesClause (pClass);
    pClass->pConstraints = ParseConstraintClause (pClass, pClass->pTypeParams != NULL);
    pClass->pMembers = NULL;

    // Parse class body
    pClass->iOpen = CurTokenIndex();

    bool fParseMembers = true;
    if (!Eat (TID_OPENCURLY)) {
        long curIdx = CurTokenIndex();
        //ok, interesting case.  The user might have something like:
        //class C :    //<-- they're typing here
        //class B {}
        //In this case it's most likely that B is a sibling of C and not a child.
        //So if we don't see an { and we're followed by the beginning of a 
        //non-member declaration then we bail out and don't treat it as a nested
        //type.
        //
        //Note: this won't handle the case where the type below has an attribute
        //on it, but maybe we can revisit that later
        ParseModifiers(false/*fReportErrors*/);
        switch (CurToken())
        {
        case TID_CLASS:
        case TID_INTERFACE:
        case TID_STRUCT:
        case TID_NAMESPACE:
            fParseMembers = false;
            break;
        default:
            break;
        }
        Rewind(curIdx);
    }

    //even if we saw a { or think we shoudl parse members bail out early since 
    //we know namespaces can't be nested inside types
    if (fParseMembers)
    {
        BOOL        fEndFound = FALSE;
        MEMBERNODE  **ppNext = &pClass->pMembers;

        // ignore members if missing type name and no open curly
        if (pClass->pName->pName == SpecName(SN_MISSING) &&  TokenAt(pClass->iOpen) != TID_OPENCURLY)
            fEndFound = TRUE;

        while (!fEndFound)
        {
            TOKENID iTok = CurToken();

            if (m_rgTokenInfo[iTok].dwFlags & TFF_MEMBER)
            {
                long    iTokIdx = CurTokenIndex();

                // This token can start a member -- go parse it
                *ppNext = ParseMember (pClass);
                ASSERT ((unsigned)(*ppNext)->iStart != 0xcccccccc);
                ASSERT ((unsigned)(*ppNext)->iClose != 0xcccccccc);
                ppNext = &(*ppNext)->pNext;

                // If we haven't advanced the token stream, skip
                // this token (an error will have been given)                              
                if (iTokIdx == CurTokenIndex())
                    NextToken();
            }
            else if (iTok == TID_CLOSECURLY || iTok == TID_ENDFILE)
            {
                // This marks the end of members of this class
                fEndFound = TRUE;
            }
            else
            {
                // Error -- try to sync up with intended reality
                ErrorInvalidMemberDecl(CurTokenIndex());
                fEndFound = !SkipToMember ();
            }
        }

        *ppNext = NULL;
    }

    pClass->iClose = CurTokenIndex();
    Eat (TID_CLOSECURLY);

    if (CurToken() == TID_SEMICOLON)
        NextToken();

    AddToNodeTable (pClass);
    return pClass;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseDelegate

BASENODE *CParser::ParseDelegate (BASENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    ASSERT (CurToken() == TID_DELEGATE);

    DELEGATENODE    *pDelegate = AllocNode(NK_DELEGATE, pParent, tokidx)->asDELEGATE();

    pDelegate->iStart = iStart;
    pDelegate->flags = iMods;
    pDelegate->pAttr = pAttr;
    pDelegate->pKey = NULL;
    if (pAttr != NULL)
        pAttr->pParent = pDelegate;

    NextToken();
    pDelegate->pType = ParseReturnType (pDelegate);
    pDelegate->pName = ParseIdentifier (pDelegate);

    // Check for type parameters
    pDelegate->pTypeParams = ParseInstantiation (pDelegate, true, NULL);

    ParseParameterList(pDelegate, &pDelegate->pParms, &pDelegate->iOpenParen, &pDelegate->iCloseParen, kppoNoVarargs);

    // parse constraints
    pDelegate->pConstraints = ParseConstraintClause (pDelegate, pDelegate->pTypeParams != NULL);

    pDelegate->iSemi = CurTokenIndex();
    Eat (TID_SEMICOLON);

    AddToNodeTable (pDelegate);
    return pDelegate;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseEnum

BASENODE *CParser::ParseEnum (BASENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    ASSERT (CurToken() == TID_ENUM);

    ENUMNODE   *pEnum = AllocNode(NK_ENUM, pParent, tokidx)->asENUM();

    pEnum->iStart = iStart;
    pEnum->flags = iMods;
    pEnum->pAttr = pAttr;
    pEnum->pKey = NULL;
    pEnum->pTypeParams = NULL;
    pEnum->pConstraints = NULL;
    if (pAttr != NULL)
        pAttr->pParent = pEnum;

    NextToken();
    pEnum->pName = ParseIdentifier (pEnum);

    if (CurToken() == TID_OPENANGLE) {
        Error (ERR_InvalidGenericEnum);
        ParseInstantiation (pEnum, true, NULL);
    }

    if (CurToken() == TID_COLON)
    {
        NextToken();

        TYPEBASENODE    *pBase = ParseType (pEnum);

        if ((pBase->kind != NK_PREDEFINEDTYPE) ||
            (pBase->asPREDEFINEDTYPE()->iType != PT_BYTE && pBase->asPREDEFINEDTYPE()->iType != PT_SHORT &&
             pBase->asPREDEFINEDTYPE()->iType != PT_INT && pBase->asPREDEFINEDTYPE()->iType != PT_LONG &&
             pBase->asPREDEFINEDTYPE()->iType != PT_SBYTE && pBase->asPREDEFINEDTYPE()->iType != PT_USHORT &&
             pBase->asPREDEFINEDTYPE()->iType != PT_UINT && pBase->asPREDEFINEDTYPE()->iType != PT_ULONG))
        {
            ErrorAtToken (pBase->tokidx, ERR_IntegralTypeExpected);
        }

        pEnum->pBases = pBase;
    }
    else
    {
        pEnum->pBases = NULL;
    }

    pEnum->iOpen = CurTokenIndex();

    Eat (TID_OPENCURLY);
    

    MEMBERNODE  **ppNext = &pEnum->pMembers;

    while (CurToken() != TID_CLOSECURLY)
    {
        ENUMMBRNODE *pMbr = AllocNode(NK_ENUMMBR, pEnum)->asENUMMBR();

        pMbr->pAttr = ParseAttributes (pMbr, AL_FIELD, AL_FIELD);
        pMbr->iStart = CurTokenIndex();
        pMbr->pName = ParseIdentifier (pMbr);
        if (CurToken() == TID_EQUAL)
        {
            NextToken();
            if (CurToken() == TID_COMMA || CurToken() == TID_CLOSECURLY)
            {
                Error (ERR_ConstantExpected);
                pMbr->pValue = NULL;
            }
            else
            {
                pMbr->pValue = ParseExpression (pMbr);
            }
        }
        else
        {
            pMbr->pValue = NULL;
        }

        pMbr->iClose = CurTokenIndex();
        AddToNodeTable (pMbr);
        *ppNext = pMbr;
        ppNext = &pMbr->pNext;

        if (CurToken() != TID_COMMA)
        {
            if (CurToken() == TID_IDENTIFIER) {
                // See if they just forgot a comma.
                switch (PeekToken()) {
                case TID_EQUAL:
                case TID_COMMA:
                case TID_CLOSECURLY:
                case TID_SEMICOLON:
                    CheckToken(TID_COMMA);
                    continue;

                default:
                    break;
                }
            }
            else if (CurToken() == TID_SEMICOLON) {
                CheckToken(TID_COMMA);
                NextToken();
                continue;
            }

            pMbr->iClose = PeekTokenIndexFrom( pMbr->iClose, -1);     // Don't include the } as the close for the last member...
            break;
        }

        NextToken ();
    }

    *ppNext = NULL;
    pEnum->iClose = CurTokenIndex();
    Eat (TID_CLOSECURLY);

    if (CurToken() == TID_SEMICOLON)
        NextToken();

    AddToNodeTable (pEnum);
    return pEnum;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseMember

MEMBERNODE *CParser::ParseMember (AGGREGATENODE *pParent)
{
    long        tokidx = CurTokenIndex();
    unsigned    iMods;
    BASENODE    *pAttr;
    bool        methodWithoutType = false;
    bool        isEvent = false;
    bool        isFixed = false;
    MemberNameEnum memberNameType;
    TOKENID     iTok;

    // Attributes are potentially at the beginning of everything here.  Parse 'em,
    // we'll get NULL back if there aren't any.
    pAttr = ParseAttributes (pParent, (ATTRLOC) 0, (ATTRLOC) 0);

    long        iMark = CurTokenIndex();
    long        iStart = iMark;

    // All things here can start with modifiers -- parse them into a bitfield.
    iMods = ParseModifiers (TRUE);

    // Check for [static] constructor form
    if (CurToken() == TID_IDENTIFIER && PeekToken(1) == TID_OPENPAREN)
    {
        if (m_pTokens[CurTokenIndex()].Name() == pParent->pName->pName)
            return ParseConstructor (pParent, tokidx, iStart, pAttr, iMods);

        methodWithoutType = true;
    }

    // Check for destructor form
    if (CurToken() == TID_TILDE)
        return ParseDestructor (pParent, tokidx, iStart, pAttr, iMods);

    // Check for constant
    if (CurToken() == TID_CONST)
        return ParseConstant (pParent, tokidx, iStart, pAttr, iMods);

    // Check for event. Remember for later processing.
    if (CurToken() == TID_EVENT)
    {
        Eat (TID_EVENT);
        isEvent = true;
    }

    // It's valid to have a type declaration here -- check for those
    if ((m_rgTokenInfo[CurToken()].dwFlags & TFF_TYPEDECL) && !isEvent)
    {
        BOOL    fMissing;

        CBasenodeLookupCache cache;
        Rewind (iMark);
        NESTEDTYPENODE  *pNested = AllocNode(NK_NESTEDTYPE, pParent, tokidx)->asNESTEDTYPE();
        pNested->pAttr = NULL;
        pNested->pType = ParseTypeDeclaration (pNested, pAttr);
        pNested->iStart = GetFirstToken (pNested->pType, EF_FULL, &fMissing);
        pNested->iClose = GetLastToken (cache, pNested->pType, EF_FULL, &fMissing);
        return pNested;
    }

    // Check for conversion operators (implicit/explicit)
    // Also allow operator here so we can give a good error message
    if ((CurToken() == TID_IMPLICIT || CurToken() == TID_EXPLICIT || CurToken() == TID_OPERATOR) && !isEvent)
        return ParseOperator (pParent, tokidx, iStart, pAttr, iMods, NULL);

    if (!isEvent && CurToken() == TID_FIXED)
    {
        ReportFeatureUse(IDS_FeatureFixedBuffer);
        Eat (TID_FIXED);
        isFixed = true;
    }

    // Everything that's left -- methods, fields, properties, and
    // non-conversion operators -- starts with a type (possibly void).  Parse one.
    // Unless we have a method without a return type (identifier followed by a '(')
    TYPEBASENODE *pType = NULL;
    if (methodWithoutType)
    {
        Error(ERR_MemberNeedsType);
        pType = AllocNode(NK_PREDEFINEDTYPE, NULL)->asPREDEFINEDTYPE();
        pType->asPREDEFINEDTYPE()->iType = PT_VOID;
    }
    else if (CurToken() == TID_ENDFILE)
    {
        Eat(TID_CLOSECURLY);
        pType = NULL;
    }
    else
    {
        pType = ParseReturnType (NULL);
    }


    if (isFixed)
        goto PARSE_FIELD;

    // Check here for operators
    // Allow old-style implicit/explicit casting operator syntax, just so we can give a better error
    if ((CurToken() == TID_OPERATOR || CurToken() == TID_IMPLICIT || CurToken() == TID_EXPLICIT) && !isEvent)
        return ParseOperator (pParent, tokidx, iStart, pAttr, iMods, pType);

    iMark = CurTokenIndex();
    memberNameType = ScanMemberName();
    iTok = CurToken();
    Rewind(iMark);

    switch (memberNameType)
    {
    case MemberName::IndexerName:
        if (!isEvent)
            return ParseProperty (NK_INDEXER, pParent, tokidx, iStart, pAttr, iMods, pType, false);
        break;

    case MemberName::GenericMethodName:
        if (!isEvent)
            return ParseMethod (pParent, tokidx, iStart, pAttr, iMods, pType);
        break;

    case MemberName::NotMemberNameWithDot:
        // Check for the situation where an incomplete explicit member is being entered.
        // In this case, the code may look something like "void ISomeInterface." followed
        // by either the end of the type of some other member (valid or not).  For statement
        // completion purposes, we need the name to be a potentially dotted name, so we
        // can't default into a field declaration (since it's name field is a NAMENODE).

        // Okay, whichever of these we call will fail -- however, they can handle
        // dotted names.
        if (isEvent)
            return ParseProperty (NK_PROPERTY, pParent, tokidx, iStart, pAttr, iMods, pType, isEvent);
        else
            return ParseMethod (pParent, tokidx, iStart, pAttr, iMods, pType);

    case MemberName::SimpleName:
    case MemberName::PropertyOrEventOrMethodName:
        if (iTok == TID_OPENPAREN && !isEvent)
            return ParseMethod (pParent, tokidx, iStart, pAttr, iMods, pType);

        if (iTok == TID_OPENCURLY)
            return ParseProperty (NK_PROPERTY, pParent, tokidx, iStart, pAttr, iMods, pType, isEvent);
        break;

    case MemberName::NotMemberName:
    default:
        // We don't know what it is yet, and it's incomplete.  Create a partial member
        // node to contain the type we parsed and return that.
        ErrorInvalidMemberDecl(CurTokenIndex());

        //If the user explicitly typed "event" then at least try to parse out an event.
        //It helps out the language service a lot
        if (isEvent && pType != NULL)
            break;

        PARTIALMEMBERNODE   *pMbr = AllocNode(NK_PARTIALMEMBER, pParent, tokidx)->asPARTIALMEMBER();
        pMbr->iStart = iStart;
        pMbr->pNode = pType;
        if (pType != NULL)
            pType->pParent = pMbr;
        pMbr->pAttr = pAttr;
        if (pAttr != NULL)
            pAttr->pParent = pMbr;
        pMbr->iClose = CurTokenIndex();
        return pMbr;

    }

PARSE_FIELD:

    return ParseField (pParent, tokidx, iStart, pAttr, iMods, pType, isEvent, isFixed);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseConstructor

MEMBERNODE *CParser::ParseConstructor (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    CTORMETHODNODE  *pCtor = AllocNode(NK_CTOR, pParent, tokidx)->asCTOR();

    pCtor->iStart = iStart;
    pCtor->flags = iMods;
    pCtor->pAttr = SetDefaultAttributeLocation(pAttr, AL_METHOD, AL_METHOD);
    if (pAttr != NULL)
        pAttr->pParent = pCtor;
    pCtor->pInteriorNode = NULL;

    NAME        *pName = m_pTokens[CurTokenIndex()].Name();

    // Current token is an identifier.  Check to see that the name matches that of the class
    ASSERT (pName != NULL);
    ASSERT (pName == pParent->pName->pName);
    pCtor->pType = NULL;    
    NextToken();

    // Get parameters
    ParseParameterList(pCtor, &pCtor->pParms, &pCtor->iOpenParen, &pCtor->iCloseParen, kppoAllowAll);

    // Check for :base(args) or :this(args)
    pCtor->pThisBaseCall = NULL;
    if (CurToken() == TID_COLON)
    {
        TOKENID iTok = NextToken ();

        if (iTok == TID_BASE || iTok == TID_THIS)
        {
            pCtor->other |= (iTok == TID_BASE ? NFEX_CTOR_BASE : NFEX_CTOR_THIS);

            pCtor->pThisBaseCall = AllocNode(NK_CALL, pCtor, PeekTokenIndex())->asCALL();
            pCtor->pThisBaseCall->other |= (iTok == TID_BASE ? NFEX_CTOR_BASE : NFEX_CTOR_THIS);

            {
                //parse the "this/base" name into p1
                NAME* callName = (iTok == TID_BASE ? GetNameMgr()->KeywordName(TID_BASE) : GetNameMgr()->KeywordName(TID_THIS));
                NAMENODE        *pName = AllocNameNode(callName, this->CurTokenIndex());
                pName->pParent = pCtor->pThisBaseCall;
                pCtor->pThisBaseCall->p1 = pName;
            }


            NextToken();
        
            //now try to parse the arguments
            long iErrorCount = m_iErrors;
            if (CheckToken(TID_OPENPAREN))
            {
                pCtor->pThisBaseCall->p2 = ParseArgumentList (pCtor->pThisBaseCall);
                int closeIndex = PeekTokenIndex(-1); // NOTE:  -1 because ParseArgumentList advanced past it...
                if (m_pTokens[closeIndex].Token() != TID_CLOSEPAREN) {
                    closeIndex = PeekTokenIndexFrom(closeIndex);
                }
                pCtor->pThisBaseCall->iClose = closeIndex;      
            }
            else
            {
                //we didn't read in a parenthesis.  So our arugments will be null               
                pCtor->pThisBaseCall->p2 = NULL;
            }

            if (iErrorCount != m_iErrors)
                pCtor->pThisBaseCall->flags |= NF_CALL_HADERROR;         // Parameter tips key on this for window placement...
        }
        else
        {
            if (iMods & NF_MOD_STATIC) // Static constructor can't have this or base
                Error (ERR_StaticConstructorWithExplicitConstructorCall, pName->text);
            else
                Error (ERR_ThisOrBaseExpected);
        }
    }

    // Next should be the block.  Remember where it started for the interior parse.
    pCtor->iOpen = CurTokenIndex();
    pCtor->pBody = NULL;
    if (CurToken() == TID_OPENCURLY || pCtor->other & (NFEX_CTOR_BASE | NFEX_CTOR_THIS))
    {
        SkipBlock (FALSE, &pCtor->iClose);
        if (CurToken() == TID_SEMICOLON)
        {
            pCtor->iClose = CurTokenIndex();
            NextToken();
        }
    }
    else
    {
        pCtor->iClose = CurTokenIndex();
        Eat (TID_SEMICOLON);
        pCtor->other |= NFEX_METHOD_NOBODY;
    }

    AddToNodeTable (pCtor);
    return pCtor;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ParseDestructor

MEMBERNODE *CParser::ParseDestructor (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    METHODBASENODE  *pDtor = AllocNode(NK_DTOR, pParent, tokidx)->asDTOR();

    pDtor->iStart = iStart;
    pDtor->flags = iMods;
    pDtor->pAttr = SetDefaultAttributeLocation(pAttr, AL_METHOD, AL_METHOD);
    pDtor->pType = NULL;
    pDtor->pParms = NULL;
    if (pAttr != NULL)
        pAttr->pParent = pDtor;
    pDtor->pInteriorNode = NULL;

    // Current token is an identifier.  Rescan it, and check to see that the name
    // matches that of the class
    ASSERT (CurToken() == TID_TILDE);
    if (NextToken() != TID_IDENTIFIER)
    {
        CheckToken(TID_IDENTIFIER); // issue error.
    }
    else
    {
        if (m_pTokens[CurTokenIndex()].Name() != pParent->pName->pName)
            Error (ERR_BadDestructorName);
        NextToken();
    }

    // Get parameters
    pDtor->iOpenParen = CurTokenIndex();
    Eat (TID_OPENPAREN);
    pDtor->iCloseParen = CurTokenIndex();
    Eat (TID_CLOSEPAREN);

    // Next should be the block.  Remember where it started for the interior parse.
    pDtor->iOpen = CurTokenIndex();
    pDtor->pBody = NULL;
    if (CurToken() == TID_OPENCURLY)
    {
        SkipBlock (FALSE, &pDtor->iClose);
    }
    else
    {
        pDtor->iClose = CurTokenIndex();
        Eat (TID_SEMICOLON);
        pDtor->other |= NFEX_METHOD_NOBODY;
    }


    AddToNodeTable (pDtor);
    return pDtor;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseConstant

MEMBERNODE *CParser::ParseConstant (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods)
{
    FIELDNODE   *pConst = AllocNode(NK_CONST, pParent, tokidx)->asCONST();
    BOOL fFirst = TRUE;

    pConst->iStart = iStart;
    pConst->flags = iMods;
    pConst->pAttr = SetDefaultAttributeLocation(pAttr, AL_FIELD, AL_FIELD);
    if (pAttr != NULL)
        pAttr->pParent = pConst;

    NextToken();
    pConst->pType = ParseType (pConst);

    CListMaker  list (this, &pConst->pDecls);
    long        iComma = -1;

    while (TRUE)
    {
        BASENODE    *pVar = ParseVariableDeclarator (pConst, pConst, PVDF_CONST, fFirst);

        AddToNodeTable (pVar);
        list.Add (pVar, iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
        fFirst = FALSE;
    }

    pConst->iClose = CurTokenIndex();
    Eat (TID_SEMICOLON);
    return pConst;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseParameterList

void CParser::ParseParameterList(BASENODE *pParent, BASENODE **ppParams, long *piOpen, long *piClose, int flags)
{
    *piOpen = CurTokenIndex();
    Eat ((flags & kppoSquare) ? TID_OPENSQUARE : TID_OPENPAREN);

    CListMaker list (this, ppParams);
    long iComma = -1;

    if (CurToken() != ((flags & kppoSquare) ? TID_CLOSESQUARE : TID_CLOSEPAREN)) {
        long itokParams = -1;
        bool fParams = false;

        while (true) {
            long itok = CurTokenIndex();

            // Parse a parameter
            list.Add(ParseParameter(pParent, flags), iComma);

            if (CurToken() != TID_COMMA)
                break;

            // Remember __arglist / params location for error reporting
            if ((pParent->other & (NFEX_METHOD_VARARGS | NFEX_METHOD_PARAMS)) && itokParams < 0) {
                itokParams = itok;
                fParams = !!(pParent->other & NFEX_METHOD_PARAMS);
            }

            iComma = CurTokenIndex();
            NextToken();
        }

        if (itokParams >= 0) {
            ErrorAtToken(itokParams, fParams ? ERR_ParamsLast : ERR_VarargsLast);
            // Clear the bits so downstream code isn't confused.
            pParent->other &= ~(NFEX_METHOD_VARARGS | NFEX_METHOD_PARAMS);
        }
    }

    *piClose = CurTokenIndex();
    Eat ((flags & kppoSquare) ? TID_CLOSESQUARE : TID_CLOSEPAREN);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseMethodName

BASENODE *CParser::ParseMethodName (BASENODE *pParent)
{
    // We let the binding code check for invalid type variable names and for
    // attributes in invalid locations.
    BASENODE * node = ParseGenericQualifiedNamePart(pParent, false, true);

    if (node->flags & NF_NAME_MISSING)
        return node;

    long itokDot = CurTokenIndex();

    CheckForAlias(node);

    BASENODE * lastNode = node;
    while (CurToken() == TID_DOT || CurToken() == TID_COLONCOLON) {
        itokDot = CurTokenIndex();
        if (node->kind != NK_ALIASNAME)
            CheckToken(TID_DOT);
        NextToken();
        lastNode = ParseGenericQualifiedNamePart(pParent, false, true);
        node = AllocDotNode(itokDot, pParent, node, lastNode);
    }

    if (node->IsDblColon())
        ErrorAtToken(itokDot, ERR_AliasQualAsExpression);

    return node;
}

////////////////////////////////////////////////////////////////////////////////

MEMBERNODE *CParser::ParseMethod (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType)
{
    METHODNODE  *pMethod = AllocNode(NK_METHOD, pParent, tokidx)->asMETHOD();

    // Store info given to us
    pMethod->iStart = iStart;
    pMethod->flags = iMods;
    pMethod->pType = pType;
    if (pType != NULL)
        pType->pParent = pMethod;
    pMethod->pAttr = SetDefaultAttributeLocation(pAttr, AL_METHOD, (ATTRLOC) (AL_METHOD | AL_RETURN));
    if (pAttr != NULL)
        pAttr->pParent = pMethod;
    pMethod->pInteriorNode = NULL;

    // Parse the name (it could be qualified)
    pMethod->pName = ParseMethodName (pMethod);

    // Get parameters
    ParseParameterList(pMethod, &pMethod->pParms, &pMethod->iOpenParen, &pMethod->iCloseParen, kppoAllowAll);

    // parse constraints
    pMethod->pConstraints = ParseConstraintClause (pMethod, pMethod->pName->LastNameOfDottedName()->kind == NK_GENERICNAME);

    //When a generic method overrides a generic method declared in a base 
    //class, or is an explicit interface member implementation of a method in 
    //a base interface, the method shall not specify any type-parameter-
    //constraints-clauses. In these cases, the type parameters of the method 
    //inherit constraints from the method being overridden or implemented
    if (pMethod->pConstraints != NULL &&
        (iMods & NF_MOD_OVERRIDE ||
            (pMethod->pName != NULL && pMethod->pName->kind == NK_DOT))) {
        BOOL fMissingName;
        long iTok = GetFirstToken(pMethod->pConstraints, EF_FULL, &fMissingName);
        ErrorAtToken(iTok, ERR_OverrideWithConstraints);
    }

    // Next should be the block.  Remember where it started for the interior parse.
    pMethod->iOpen = CurTokenIndex();
    pMethod->pBody = NULL;
    if (CurToken() == TID_OPENCURLY)
    {
        SkipBlock (FALSE, &pMethod->iClose);
    }
    else
    {
        pMethod->iClose = CurTokenIndex();
        Eat (TID_SEMICOLON);
        pMethod->other |= NFEX_METHOD_NOBODY;
    }

    AddToNodeTable (pMethod);
    return pMethod;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseProperty

MEMBERNODE *CParser::ParseProperty (NODEKIND nodekind, AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType, bool isEvent)
{
    PROPERTYNODE    *pProp = AllocNode (nodekind, pParent, tokidx)->asANYPROPERTY();
    long iOpenIndex = -1;

    pProp->iStart = iStart;
    pProp->flags = iMods;
    pProp->pType = pType;
    pProp->other = isEvent ? NFEX_EVENT : 0;
    pType->pParent = pProp;
    pProp->pAttr = isEvent ?
        SetDefaultAttributeLocation(pAttr, AL_EVENT,    AL_EVENT) :
        SetDefaultAttributeLocation(pAttr, AL_PROPERTY, AL_PROPERTY);
    if (pAttr != NULL)
        pAttr->pParent = pProp;
    pProp->iOpenSquare = -1;
    pProp->pParms = NULL;
    pProp->iCloseSquare = -1;

    if (nodekind == NK_PROPERTY)
    {
        pProp->pName = ParseGenericQualifiedNameList (pProp, false);
    }
    else
    {
        pProp->pName = ParseIndexerName(pProp);

        ParseParameterList(pProp, &pProp->pParms, &pProp->iOpenSquare, &pProp->iCloseSquare, kppoSquare | kppoNoRefOrOut | kppoNoVarargs);

        if (!pProp->pParms)
            ErrorAtToken (pProp->iCloseSquare, ERR_IndexerNeedsParam);
    }

    pProp->iOpen = CurTokenIndex();
    if (CurToken() == TID_OPENCURLY)
        iOpenIndex = CurTokenIndex();
    Eat (TID_OPENCURLY);

    BOOL fInterface = (pParent->kind == NK_INTERFACE);
    
    pProp->pGet = pProp->pSet = NULL;

    for (long i=0; i<4; i++)
    {
        if (CurToken() == TID_CLOSECURLY)
            break;

        BASENODE *      pAccessorAttr = ParseAttributes (pProp, AL_METHOD, (ATTRLOC) 0);
        long            iTokAccessor = CurTokenIndex();
        ACCESSORNODE    **ppAccessor = NULL;
        uint iAccessorMods = 0;

        if (isEvent)
        {
            while (m_rgTokenInfo[CurToken()].dwFlags & TFF_MODIFIER)
            {
                Error (ERR_NoModifiersOnAccessor);
                NextToken();
            }
        } else {
            iAccessorMods = ParseModifiers (TRUE);
            if (iAccessorMods) {
                ReportFeatureUse(IDS_FeaturePropertyAccessorMods);
            }
        }

        if (pAccessorAttr != NULL && CurToken() != TID_IDENTIFIER)
        {
            PrevToken();
            CSTOKEN& token = m_pTokens[CurTokenIndex()];
            if (token.Token() != TID_IDENTIFIER)
            {
                NextToken();
            }
            else
            {
                NAME* pName = token.Name();
                if (pName != SpecName(SN_REMOVE) && 
                    pName != SpecName(SN_ADD) &&
                    pName != SpecName(SN_GET) &&
                    pName != SpecName(SN_SET))
                {
                    NextToken();
                }
            }
        }

        if (CurToken() == TID_IDENTIFIER)
        {
            NAME    *pName = m_pTokens[CurTokenIndex()].Name();

            if (isEvent)
            {
                if (pName == SpecName(SN_REMOVE))
                    ppAccessor = &pProp->pGet;
                else if (pName == SpecName(SN_ADD))
                    ppAccessor = &pProp->pSet;
                else
                    Error (ERR_AddOrRemoveExpected);

                if (fInterface && ppAccessor) 
                {
                    Error (ERR_EventPropertyInInterface);
                }
            }
            else 
            {
                if (pName == SpecName(SN_GET))
                    ppAccessor = &pProp->pGet;
                else if (pName == SpecName(SN_SET))
                    ppAccessor = &pProp->pSet;
                else
                    Error (ERR_GetOrSetExpected);
            }

            NextToken();
        }
        else
        {
            Error (isEvent ? ERR_AddOrRemoveExpected : ERR_GetOrSetExpected);
        }

        if (ppAccessor == NULL && pAccessorAttr != NULL)
        {
            if (pProp->pGet == NULL)
            {
                ppAccessor = &pProp->pGet;
            }
            else if (pProp->pSet == NULL)
            {
                ppAccessor = &pProp->pSet;
            }
        }

        if (ppAccessor != NULL)
        {
            if (*ppAccessor != NULL)
                ErrorAtToken (iTokAccessor, ERR_DuplicateAccessor);

            ACCESSORNODE    *pAccessor = AllocNode(NK_ACCESSOR, pProp, iTokAccessor)->asACCESSOR();
            pAccessor->pAttr = SetDefaultAttributeLocation(pAccessorAttr, AL_METHOD, (ATTRLOC)((ppAccessor == &pProp->pGet && !isEvent) ? AL_METHOD | AL_RETURN : AL_METHOD | AL_RETURN | AL_PARAM));
            pAccessor->flags = iAccessorMods;
            pAccessor->pBody = NULL;
            pAccessor->iOpen = CurTokenIndex();
            pAccessor->pInteriorNode = NULL;
            *ppAccessor = pAccessor;
        }

        if (CurToken() == TID_OPENCURLY)
        {
            SkipBlock (SBF_INACCESSOR, ppAccessor == NULL ? NULL : &(*ppAccessor)->iClose);
        }
        else
        {
            if (isEvent && !fInterface)
                Error(ERR_AddRemoveMustHaveBody);
                
            if (ppAccessor != NULL)
            {
                (*ppAccessor)->other |= NFEX_METHOD_NOBODY;
                (*ppAccessor)->iClose = CurTokenIndex();
            }

            if (CurToken() == TID_SEMICOLON) 
                NextToken();
            else if (iOpenIndex != -1)
            {
                if (ppAccessor != NULL || iOpenIndex == -1)
                    ErrorAfterPrevToken(ERR_SemiOrLBraceExpected);
                else
                {
                    Rewind(iOpenIndex);
                    SkipBlock (SBF_INPROPERTY, &pProp->iClose); 

                    long getterClose = pProp->pGet ? pProp->pGet->iClose : -1;
                    long setterClose = pProp->pSet ? pProp->pSet->iClose : -1;

                    pProp->iClose = max(pProp->iClose, max(setterClose, getterClose));
                    goto FAIL;
                }
            }
        }
    }

    pProp->iClose = CurTokenIndex();
    if (CheckToken(TID_CLOSECURLY) && (NextToken() == TID_SEMICOLON)) {
        Error (ERR_UnexpectedSemicolon);
        NextToken();
    }
FAIL:
    AddToNodeTable (pProp);
    return pProp;
}


MEMBERNODE *CParser::ParseField (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType, bool isEvent, bool isFixed)
{
    FIELDNODE   *pField = AllocNode(NK_FIELD, pParent, tokidx)->asFIELD();
    BOOL        fFirst = TRUE;

    pField->iStart = iStart;
    pField->pType = pType;
    pType->pParent = pField;
    pField->flags = iMods;
    pField->other = isEvent ? NFEX_EVENT : 0;

    ATTRLOC alAllowed;

    if (!isEvent) {
        alAllowed = AL_FIELD;
    }
    else {
        alAllowed = (ATTRLOC) (AL_EVENT | AL_METHOD);
        switch (pParent->kind) {
        default:
            VSFAIL("Bad parent kind in ParseField");
        case NK_INTERFACE:
            break;
        case NK_CLASS:
            if (iMods & NF_MOD_ABSTRACT)
                break;
        case NK_STRUCT:
            alAllowed = (ATTRLOC) (AL_EVENT | AL_FIELD | AL_METHOD);
            break;
        }
    }

    pField->pAttr = SetDefaultAttributeLocation(pAttr, (isEvent ? AL_EVENT : AL_FIELD), alAllowed);
    if (pAttr != NULL)
        pAttr->pParent = pField;

    CListMaker  list (this, &pField->pDecls);
    long        iComma = -1;

    while (TRUE)
    {
        BASENODE    *pVar = ParseVariableDeclarator (pField, pField, isFixed ? PVDF_FIXED : 0, fFirst);

        AddToNodeTable (pVar);
        list.Add (pVar, iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex ();
        NextToken();
        fFirst = FALSE;
    }

    pField->iClose = CurTokenIndex();
    if (isEvent && CurToken() == TID_DOT)
        Error(ERR_ExplicitEventFieldImpl);

    Eat (TID_SEMICOLON);
    return pField;
}


MEMBERNODE *CParser::ParseOperator (AGGREGATENODE *pParent, long tokidx, long iStart, BASENODE *pAttr, unsigned iMods, TYPEBASENODE *pType)
{
    OPERATORMETHODNODE  *pOperator = AllocNode(NK_OPERATOR, pParent, tokidx)->asOPERATOR();

    pOperator->iStart = iStart;
    pOperator->pType = pType;
    pOperator->flags = iMods;
    pOperator->pAttr = SetDefaultAttributeLocation(pAttr, AL_METHOD, (ATTRLOC) (AL_METHOD | AL_RETURN));
    if (pAttr != NULL)
        pAttr->pParent = pOperator;
    pOperator->pInteriorNode = NULL;

    TOKENID iOpTok;
    long errorTokenIndex;

    if (CurToken() == TID_IMPLICIT || CurToken() == TID_EXPLICIT)
    {
        errorTokenIndex = CurTokenIndex ();
        iOpTok = CurToken();
        NextToken();
        if (CheckToken (TID_OPERATOR) && pType != NULL)
        {
            ErrorAtToken (pType->tokidx, ERR_BadOperatorSyntax, m_rgTokenInfo[iOpTok].pszText);
        }
    }
    else
    {
        ASSERT (CurToken() == TID_OPERATOR);
        iOpTok = NextToken ();
        errorTokenIndex = CurTokenIndex();
        if (iOpTok == TID_IMPLICIT || iOpTok == TID_EXPLICIT)
        {
            ErrorAtToken (pType ? pType->tokidx : errorTokenIndex, ERR_BadOperatorSyntax, m_rgTokenInfo[iOpTok].pszText);
        }

        if (CurToken() == TID_GREATER && PeekToken() == TID_GREATER)
        {
            POSDATA &posFirst(CurTokenPos());
            POSDATA &posNext(m_pTokens[PeekTokenIndex()]);
            if (posFirst.iLine == posNext.iLine && (posFirst.iChar + 1) == posNext.iChar)
            {
                iOpTok = TID_SHIFTRIGHT;
                NextToken();
            }
        }
    }

    NextToken ();

    if (pType == NULL)
    {
        if (iOpTok != TID_IMPLICIT && iOpTok != TID_EXPLICIT)
            ErrorAtToken( CurTokenIndex(), ERR_BadOperatorSyntax2, m_rgTokenInfo[iOpTok].pszText);
        pType = ParseType (pOperator);
        pOperator->pType = pType;
    }
    else
    {
        pType->pParent = pOperator;
    }

    ParseParameterList(pOperator, &pOperator->pParms, &pOperator->iOpenParen, &pOperator->iCloseParen, kppoNoRefOrOut | kppoNoParams | kppoNoVarargs);
    pOperator->tok = iOpTok;

    int carg = CountListNode(pOperator->pParms);

    switch (carg) {
    case 1:
        if (!(m_rgTokenInfo[iOpTok].dwFlags & TFF_OVLUNOP)) {
            ErrorAtToken (errorTokenIndex, ERR_OvlUnaryOperatorExpected);
            pOperator->iOp = OP_NONE;
        }
        else
            pOperator->iOp = (OPERATOR)m_rgTokenInfo[iOpTok].iUnaryOp;
        break;
    case 2:
        if (!(m_rgTokenInfo[iOpTok].dwFlags & TFF_OVLBINOP)) {
            ErrorAtToken (errorTokenIndex, ERR_OvlBinaryOperatorExpected);
            pOperator->iOp = OP_NONE;
        }
        else
            pOperator->iOp = (OPERATOR)m_rgTokenInfo[iOpTok].iBinaryOp;
        break;

    default:
        if (m_rgTokenInfo[iOpTok].dwFlags & TFF_OVLBINOP)
            ErrorAtToken(errorTokenIndex, ERR_BadBinOpArgs, m_rgTokenInfo[iOpTok].pszText);
        else if (m_rgTokenInfo[iOpTok].dwFlags & TFF_OVLUNOP)
            ErrorAtToken(errorTokenIndex, ERR_BadUnOpArgs, m_rgTokenInfo[iOpTok].pszText);
        else
            ErrorAtToken(errorTokenIndex, ERR_OvlOperatorExpected);
        pOperator->iOp = OP_NONE;
        break;
    }

    pOperator->iOpen = CurTokenIndex();
    pOperator->pBody = NULL;
    if (CurToken() == TID_SEMICOLON) {
        pOperator->iClose = CurTokenIndex();
        Eat (TID_SEMICOLON);
        pOperator->other |= NFEX_METHOD_NOBODY;
    } else {
        SkipBlock(FALSE, &pOperator->iClose);
    }

    AddToNodeTable (pOperator);
    return pOperator;
}

//

TYPEBASENODE *CParser::ParseUnderlyingType(BASENODE *pParent, bool *pfHadError)
{
    TYPEBASENODE *pType;

    if (m_rgTokenInfo[CurToken()].dwFlags & TFF_PREDEFINED)
    {
        pType = ParsePredefinedType(pParent);
        if (pType->asPREDEFINEDTYPE()->iType == PT_VOID && CurToken() != TID_STAR) {
            *pfHadError = true;
            ErrorAtToken(pType->tokidx,
                (pParent && pParent->kind == NK_PARAMETER) ? ERR_NoVoidParameter : ERR_NoVoidHere);
        }
    }
    else if (IsNameStart(CurToken()))
    {
        pType = ParseNamedType(pParent);
    }
    else
    {
        pType = AllocNode(NK_NAMEDTYPE, pParent)->asNAMEDTYPE();

        Error (ERR_TypeExpected);
        pType->asNAMEDTYPE()->pName = ParseMissingName (pType, CurTokenIndex());
        *pfHadError = true;
    }

    return pType;
}

//

TYPEBASENODE *CParser::ParsePointerTypeMods (BASENODE *pParent, TYPEBASENODE *pBaseType)
{
    while (CurToken() == TID_STAR)
    {
        POINTERTYPENODE *pPtr = AllocNode(NK_POINTERTYPE, pParent)->asPOINTERTYPE();
        NextToken();
        pPtr->pElementType = pBaseType;
        pBaseType->pParent = pPtr;
        pBaseType = pPtr;
    }

    return pBaseType;
}


TYPEBASENODE *CParser::ParseType (BASENODE *pParent, bool fIsOrAs)
{
    bool fHadError = false;
    TYPEBASENODE *pType = ParseUnderlyingType(pParent, &fHadError);
    if (fHadError)
        return pType;

    if (CurToken() == TID_QUESTION) {
        TOKENID tidNext = PeekToken();
        if (fIsOrAs && ((m_rgTokenInfo[tidNext].dwFlags & (TFF_TERM | TFF_PREDEFINED)) || IsUnaryOperator(tidNext)))
            return pType;

        ReportFeatureUse(IDS_FeatureNullable);
        NULLABLETYPENODE * nodeNull = AllocNode(NK_NULLABLETYPE, pParent)->asNULLABLETYPE();
        nodeNull->pElementType = pType;
        pType->pParent = nodeNull;
        pType = nodeNull;
        NextToken();
    }

    pType = ParsePointerTypeMods(pParent, pType);

    if (CurToken() == TID_OPENSQUARE) {
        ARRAYTYPENODE * nodeTop = AllocNode(NK_ARRAYTYPE, pParent)->asARRAYTYPE();
        ParseArrayRankSpecifier(nodeTop, true, &nodeTop->iDims);
        ASSERT(nodeTop->iDims > 0);
        ARRAYTYPENODE * nodeLast = nodeTop;

        while (CurToken() == TID_OPENSQUARE) {
            ARRAYTYPENODE * nodeT = AllocNode(NK_ARRAYTYPE, nodeLast)->asARRAYTYPE();
            nodeLast->pElementType = nodeT;
            ParseArrayRankSpecifier(nodeT, true, &nodeT->iDims);
            ASSERT(nodeT->iDims > 0);
            nodeLast = nodeT;
        }
        nodeLast->pElementType = pType;
        pType->pParent = nodeLast;
        pType = nodeTop;
    }

    return pType;
}


PREDEFINEDTYPENODE *CParser::ParsePredefinedType (BASENODE *pParent)
{
    ASSERT (m_rgTokenInfo[CurToken()].dwFlags & TFF_PREDEFINED);
    
    PREDEFINEDTYPENODE    *pType = AllocNode(NK_PREDEFINEDTYPE, pParent)->asPREDEFINEDTYPE();

    pType->tokidx = CurTokenIndex();
    pType->pParent = pParent;
    pType->iType = m_rgTokenInfo[CurToken()].iPredefType;

    NextToken();
    
    return pType;
}


NAMEDTYPENODE *CParser::ParseNamedType (BASENODE *pParent)
{
    NAMEDTYPENODE * pType = AllocNode(NK_NAMEDTYPE, pParent)->asNAMEDTYPE();
    
    pType->pName = ParseGenericQualifiedNameList (pType, false);
        
    return pType;
}


void CParser::CheckForAlias(BASENODE * node)
{
    if (CurToken() == TID_COLONCOLON && node->kind == NK_NAME) {
        ReportFeatureUse(IDS_FeatureGlobalNamespace);

        if (node->asNAME()->pName == SpecName(SN_GLOBAL))
            node->flags |= NF_GLOBAL_QUALIFIER;
        node->kind = NK_ALIASNAME;
    }
}


BASENODE *CParser::ParseGenericQualifiedNameList(BASENODE *pParent, bool fInExpr)
{
    BASENODE * node;

    node = ParseGenericQualifiedNamePart(pParent, fInExpr, false);
    if (node->flags & NF_NAME_MISSING)
        return node;

    long itokDot = CurTokenIndex();

    CheckForAlias(node);

    while (CurToken() == TID_DOT  || CurToken() == TID_COLONCOLON) {
        itokDot = CurTokenIndex();
        if (node->kind != NK_ALIASNAME)
            CheckToken(TID_DOT);
        NextToken();
        node = AllocDotNode(itokDot, pParent, node, ParseGenericQualifiedNamePart(pParent, fInExpr, false));
    }

    if (fInExpr && node->IsDblColon())
        ErrorAtToken(itokDot, ERR_AliasQualAsExpression);

    return node;
}


NAMENODE *CParser::ParseGenericQualifiedNamePart(BASENODE *pParent, bool fInExpr, bool fTypeVarList)
{
    ASSERT(!(fInExpr && fTypeVarList)); 

    if (!CheckToken (TID_IDENTIFIER))
        return ParseMissingName (pParent, CurTokenIndex());

    bool fDefinitelyGeneric;
    bool fPossibleGeneric;
    CheckIfGeneric(fInExpr, &fDefinitelyGeneric, &fPossibleGeneric);

    if (fDefinitelyGeneric) {
        GENERICNAMENODE *pGenericName = AllocGenericNameNode(NULL, CurTokenIndex());
        pGenericName->pParent = pParent;
        NextToken();

        ASSERT(CurToken() == TID_OPENANGLE);

        pGenericName->iOpen = CurTokenIndex();
        pGenericName->pParams = ParseInstantiation(pGenericName, !!fTypeVarList, &pGenericName->iClose);

        return pGenericName;
    }

    NAMENODE *pName = AllocNameNode(NULL, CurTokenIndex());
    pName->pParent = pParent;

    GENERICNAMENODE* pGenericName = NULL;
    if (fPossibleGeneric && this->SupportsErrorSuppression()) {
        long iStartIndex = CurTokenIndex();

        CErrorSuppression es = this->GetErrorSuppression();
        {
            pGenericName = AllocGenericNameNode(NULL, CurTokenIndex());
            pGenericName->pParent = pName;
            NextToken();

            ASSERT(CurToken() == TID_OPENANGLE);

            pGenericName->iOpen = CurTokenIndex();
            pGenericName->pParams = ParseInstantiation(pGenericName, fTypeVarList, &pGenericName->iClose);
        }

        Rewind(iStartIndex);
    }

    pName->pPossibleGenericName = pGenericName;
    NextToken();

    return pName;
}


bool CParser::PastCloseToken(long iClose)
{
    if (iClose == -1)
    {
        return false;
    }
    
    return CurTokenIndex() >= iClose;
}


void CParser::CheckIfGeneric(bool fInExpr, bool* pfDefinitelyGeneric, bool* pfPossiblyGeneric)
{
    bool fDefinitelyGeneric = false;
    bool fPossiblyGeneric = false;
    if (PeekToken() == TID_OPENANGLE)
    {
        if (fInExpr)
        {
            long mark = CurTokenIndex();
            NextToken();

            fDefinitelyGeneric = fPossiblyGeneric = ScanOptionalInstantiation();
            if (fDefinitelyGeneric)
            {
                TOKENID tid = CurToken();
                if (tid != TID_OPENPAREN && ((m_rgTokenInfo[tid].dwFlags & (TFF_TERM | TFF_PREDEFINED)) || IsUnaryOperator(tid)))
                    fDefinitelyGeneric = false;
            }
            Rewind(mark);
        }
        else
        {
            fDefinitelyGeneric = true;
        }
    }

    *pfDefinitelyGeneric = fDefinitelyGeneric;
    *pfPossiblyGeneric = fPossiblyGeneric;
}


TYPEBASENODE *CParser::ParseClassType (BASENODE *pParent)
{
    TYPEBASENODE    *pType;

    if (CurToken() == TID_OBJECT || CurToken() == TID_STRING)
    {
        pType = ParsePredefinedType(pParent);
    }
    else
    {
        if (!IsNameStart(CurToken()))
            Error (ERR_ClassTypeExpected);
         
        pType = ParseNamedType(pParent);
    }

    return pType;
}

//

TYPEBASENODE *CParser::ParseReturnType (BASENODE *pParent)
{
    if (CurToken() == TID_VOID && PeekToken(1) != TID_STAR)
    {
        return ParsePredefinedType(pParent);
    }

    return ParseType (pParent);
}

//
//
//
//
//
//
TYPEBASENODE *CParser::ParseOpenType (BASENODE *pParent)
{
    ReportFeatureUse(IDS_FeatureGenerics);

    NAMEDTYPENODE * nodeType = AllocNode(NK_OPENTYPE, pParent)->asOPENTYPE();
    nodeType->pName = NULL;

    long itokDot = -1;

    for (;;) {
        ASSERT(CurToken() == TID_IDENTIFIER);
        NAMENODE * nodeCur;

        if (PeekToken() == TID_OPENANGLE) {
            OPENNAMENODE * nodeOpen = AllocNode(NK_OPENNAME, nodeType)->asOPENNAME();
            InitNameNode(nodeOpen, NULL);
            NextToken();

            ASSERT(CurToken() == TID_OPENANGLE);
            nodeOpen->iOpen = CurTokenIndex();
            NextToken();
            nodeOpen->carg = 1;
            while (CurToken() == TID_COMMA) {
                NextToken();
                nodeOpen->carg++;
            }
            ASSERT(CurToken() == TID_CLOSEANGLE);
            nodeOpen->iClose = CurTokenIndex();
            NextToken();

            nodeCur = nodeOpen;
        }
        else {
            nodeCur = ParseIdentifier(nodeType);
        }

        if (nodeType->pName) {
            ASSERT(itokDot > 0);
            nodeType->pName = AllocDotNode(itokDot, nodeType, nodeType->pName, nodeCur);
        }
        else {
            ASSERT(itokDot == -1);
            CheckForAlias(nodeCur);
            nodeType->pName = nodeCur;
        }

        if (CurToken() != TID_DOT && CurToken() != TID_COLONCOLON)
            return nodeType;
        itokDot = CurTokenIndex();

        if (nodeType->pName->kind != NK_ALIASNAME)
            CheckToken(TID_DOT);
        NextToken();
    }
}

int CParser::ScanOpenType()
{
    int cargTot = 0;

    if (PeekToken() == TID_COLONCOLON) {
        if (CurToken() != TID_IDENTIFIER)
            return 0;
        NextToken();
        NextToken();
    }

    for (;;) {
        if (CurToken() != TID_IDENTIFIER)
            return 0;
        NextToken();

        if (CurToken() == TID_OPENANGLE) {
            NextToken();
            cargTot++;
            while (CurToken() == TID_COMMA) {
                NextToken();
                cargTot++;
            }
            if (CurToken() != TID_CLOSEANGLE)
                return 0;
            NextToken();
        }

        if (CurToken() != TID_DOT && CurToken() != TID_COLONCOLON)
            return cargTot;
        NextToken();
        if (CurToken() != TID_IDENTIFIER)
            return 0;
    }
}


//

TYPEBASENODE *CParser::ParseTypeName (BASENODE *pParent, BOOL fConstraint)
{
    TYPEBASENODE *pType = ParseType(pParent);
    if (pType->kind != NK_PREDEFINEDTYPE && pType->kind != NK_NAMEDTYPE)
    {
        if (fConstraint)
            ErrorAtToken (pType->tokidx, ERR_BadConstraintType);
        else
            ErrorAtToken (pType->tokidx, ERR_BadBaseType);
    }

    return pType;
}

//

BASENODE *CParser::ParseBaseTypesClause (BASENODE *pParent)
{
    if (CurToken() == TID_COLON)
    {
        NextToken();

        BASENODE    *pList;
        CListMaker   list (this, &pList);
        long        iComma = -1;

        while (TRUE)
        {
            TYPEBASENODE *pType = ParseTypeName(pParent, false);
            if (pType)
            {
                list.Add (pType, iComma);
            }
            if (CurToken() != TID_COMMA)
                break;
            iComma = CurTokenIndex();
            NextToken();
        }

        return pList;
    }
    else
    {
        return NULL;
    }
}


BASENODE *CParser::ParseInstantiation (BASENODE *pParent, bool fAllowAttrs, long * pitokClose)
{
    if (CurToken() != TID_OPENANGLE) {
        if (pitokClose)
            *pitokClose = -1;
        return NULL;
    }

    ReportFeatureUse(IDS_FeatureGenerics);
    NextToken();

    BASENODE *pList;
    CListMaker list(this, &pList);
    long iComma = -1;

    while (TRUE)
    {
        TYPEBASENODE * pType;

        if (CurToken() == TID_OPENSQUARE && fAllowAttrs) {
            TYPEWITHATTRNODE * node = AllocNode(NK_TYPEWITHATTR, pParent)->asTYPEWITHATTR();
            node->pAttr = ParseAttributes(node, AL_TYPEVAR, AL_TYPEVAR);
            node->pType = ParseType(node);
            pType = node;
        }
        else {
            pType = ParseType(pParent);
        }

        list.Add (pType, iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
    }

    if (pitokClose) {
        *pitokClose = CurTokenIndex();
    }
    Eat(TID_CLOSEANGLE);

    return pList;
}


CONSTRAINTNODE  *CParser::ParseConstraint(BASENODE *pParent)
{
    CONSTRAINTNODE *pConstraint = AllocNode(NK_CONSTRAINT, pParent)->asCONSTRAINT();

    pConstraint->pName = ParseIdentifier(pConstraint);
    pConstraint->flags = 0;
    if (CurToken() == TID_COLON) {
        pConstraint->iEnd = CurTokenIndex();
        NextToken();
    }
    else {
        pConstraint->iEnd = -1;
        Eat(TID_COLON);
    }

    CListMaker list(this, &pConstraint->pBounds);

    long iCommaTok = -1;
    for (;;) {
        switch (CurToken()) {
        case TID_NEW:
            if (pConstraint->flags & NF_CONSTRAINT_VALTYPE)
                Error(ERR_NewBoundWithVal);
            Eat(TID_NEW);
            Eat(TID_OPENPAREN);
            pConstraint->iEnd = CurTokenIndex();
            Eat(TID_CLOSEPAREN);
            pConstraint->flags |= NF_CONSTRAINT_NEWABLE;
            if (CurToken() == TID_COMMA)
                Error(ERR_NewBoundMustBeLast);
            break;

        case TID_CLASS:
        case TID_STRUCT:
            pConstraint->flags |= (CurToken() == TID_CLASS) ? NF_CONSTRAINT_REFTYPE : NF_CONSTRAINT_VALTYPE;
            if (iCommaTok >= 0)
                Error(ERR_RefValBoundMustBeFirst);
            pConstraint->iEnd = CurTokenIndex();
            NextToken();
            break;

        default:
            BASENODE *pType = ParseTypeName(pConstraint, true);
            list.Add(pType, iCommaTok);
            pConstraint->iEnd = -1;
            break;
        }
        if (CurToken() != TID_COMMA)
            break;
        iCommaTok = CurTokenIndex();
        pConstraint->iEnd = CurTokenIndex();
        NextToken();
    }

    return pConstraint;
}


BASENODE *CParser::ParseConstraintClause(BASENODE *pParent, BOOL fAllowed)
{
    if (!CheckForSpecName(SN_WHERE))
        return NULL;

    if (!fAllowed) {
        Error(ERR_ConstraintOnlyAllowedOnGenericDecl);
    }

    BASENODE *pList;
    CListMaker list(this, &pList);

    do {
        NextToken();

        CONSTRAINTNODE *pConstraint = ParseConstraint(pParent);
        list.Add(pConstraint);
    } while (CheckForSpecName(SN_WHERE));

    return fAllowed ? pList : NULL;
}


NAMENODE *CParser::ParseIdentifier (BASENODE *pParent)
{
    if (!CheckToken (TID_IDENTIFIER))
        return ParseMissingName (pParent, CurTokenIndex());

    NAMENODE * pName = AllocNameNode (NULL, CurTokenIndex());
    pName->pParent = pParent;
    NextToken();
    return pName;
}

//
//

NAMENODE *CParser::ParseIdentifierOrKeyword (BASENODE *pParent)
{
    ASSERT(CurToken() <= TID_IDENTIFIER);

    if (CurToken() == TID_IDENTIFIER)
    {
        return ParseIdentifier(pParent);
    }
    else
    {
        NAMENODE    *pName = AllocNode(NK_NAME, pParent)->asNAME();

        pName->pName = GetNameMgr()->KeywordName(CurToken());
        pName->pPossibleGenericName = NULL;

        NextToken();

        return pName;
    }
}

//

BASENODE *CParser::ParseDottedName (BASENODE *pParent, bool fAllowQualifier)
{
    BASENODE * pCur = ParseIdentifier(pParent);

    if (pCur->flags & NF_NAME_MISSING)
        return pCur;

    if (fAllowQualifier)
        CheckForAlias(pCur);

    while (CurToken() == TID_DOT || CurToken() == TID_COLONCOLON)
    {
        long iDotTokIdx = CurTokenIndex();
        if (!fAllowQualifier)
            CheckToken(TID_DOT);
        NextToken();

        NAMENODE * pName = ParseIdentifier(pParent);
        pCur = AllocDotNode (iDotTokIdx, pParent, pCur, pName);

        if (pName->flags & NF_NAME_MISSING)
            return pCur;
        fAllowQualifier = false;
    }

    ASSERT (pCur->pParent == pParent);
    return pCur;
}

//

NAMENODE *CParser::ParseMissingName (BASENODE *pParent, long iTokIndex)
{
    NAMENODE * pName = AllocNode(NK_NAME, pParent, PeekTokenIndexFrom(iTokIndex, -1))->asNAME();

    pName->pName = SpecName(SN_MISSING);
    pName->pPossibleGenericName = NULL;
    pName->flags |= NF_NAME_MISSING;
    return pName;
}

//

BASENODE *CParser::ParseIndexerName (BASENODE *pParent)
{
    if (CurToken() == TID_THIS) {
        NextToken();
        return NULL;
    }

    BASENODE * node = ParseGenericQualifiedNamePart(pParent, false, false);

    if (PeekToken(2) != TID_THIS)
        CheckForAlias(node);

    while (CurToken() == TID_DOT  || CurToken() == TID_COLONCOLON) {
        long itokDot = CurTokenIndex();
        if (node->kind != NK_ALIASNAME)
            CheckToken(TID_DOT);
        NextToken();
        if (CurToken() == TID_THIS) {
            NextToken();
            return node;
        }
        node = AllocDotNode(itokDot, pParent, node, ParseGenericQualifiedNamePart(pParent, false, false));
    }

    VSFAIL("Why didn't we find 'this'?");
    return node;
}


unsigned CParser::ParseModifiers (bool fReportErrors)
{
    unsigned    iMods = 0, iNewMod;
    bool        bNoDups = true, bNoDupAccess = true;

    while (TRUE)
    {
        TOKENID tid = CurToken();

        switch (tid)
        {
            case TID_PRIVATE:   iNewMod = NF_MOD_PRIVATE;   break;
            case TID_PROTECTED: iNewMod = NF_MOD_PROTECTED; break;
            case TID_INTERNAL:  iNewMod = NF_MOD_INTERNAL;  break;
            case TID_PUBLIC:    iNewMod = NF_MOD_PUBLIC;    break;
            case TID_SEALED:    iNewMod = NF_MOD_SEALED;    break;
            case TID_ABSTRACT:  iNewMod = NF_MOD_ABSTRACT;  break;
            case TID_STATIC:    iNewMod = NF_MOD_STATIC;    break;
            case TID_VIRTUAL:   iNewMod = NF_MOD_VIRTUAL;   break;
            case TID_EXTERN:    iNewMod = NF_MOD_EXTERN;    break;
            case TID_NEW:       iNewMod = NF_MOD_NEW;       break;
            case TID_OVERRIDE:  iNewMod = NF_MOD_OVERRIDE;  break;
            case TID_READONLY:  iNewMod = NF_MOD_READONLY;  break;
            case TID_VOLATILE:  iNewMod = NF_MOD_VOLATILE;  break;
            case TID_UNSAFE:    iNewMod = NF_MOD_UNSAFE;    break;

            case TID_IDENTIFIER:
                if (m_pTokens[CurTokenIndex()].Name() != SpecName(SN_PARTIAL))
                    return iMods;

                switch (PeekToken()) {
                default:
                    if (!(m_rgTokenInfo[PeekToken()].dwFlags & (TFF_NSELEMENT | TFF_MODIFIER)))
                        return iMods;
                    if (fReportErrors)
                        Error(ERR_PartialMisplaced);
                    break;

                case TID_CLASS:
                case TID_STRUCT:
                case TID_INTERFACE:
                    break;
                }

                iNewMod = NF_MOD_PARTIAL;
                ReportFeatureUse(IDS_FeaturePartialTypes);
                break;

            default:
                return iMods;
        }

        if (iMods & iNewMod && fReportErrors)
        {
            if (bNoDups && tid != TID_IDENTIFIER) {
                Error (ERR_DuplicateModifier, GetTokenInfo(CurToken())->pszText);
                bNoDups = false;
            }
        }
        else
        {
            if ((iMods & NF_MOD_ACCESSMODIFIERS) && (iNewMod & NF_MOD_ACCESSMODIFIERS))
            {
                if (! (((iNewMod == NF_MOD_PROTECTED) && (iMods & NF_MOD_INTERNAL)) || ((iNewMod == NF_MOD_INTERNAL) && (iMods & NF_MOD_PROTECTED))))
                {
                    if (fReportErrors)
                    {
                        if (bNoDupAccess)
                            Error (ERR_BadMemberProtection);
                        bNoDupAccess = false;
                    }
                    iNewMod = 0;
                }
            }
        }

        iMods |= iNewMod;
        NextToken();
    }
}


BASENODE *CParser::ParseParameter(BASENODE *pParent, int flags)
{
    PARAMETERNODE   *pParm = AllocNode(NK_PARAMETER, pParent)->asPARAMETER();

    if (flags & kppoNoAttrs)
        pParm->pAttr = NULL;
    else
        pParm->pAttr = ParseAttributes(pParm, AL_PARAM, AL_PARAM);

    if (CurToken() == TID_ARGS) {
        if (!pParent || (flags & kppoNoVarargs))
            Error(ERR_IllegalVarArgs);
        else
            pParent->other |= NFEX_METHOD_VARARGS;
        NextToken();
        return NULL;
    }

    if (CurToken() == TID_PARAMS) {
        if (!pParent || (flags & kppoNoParams))
            Error(ERR_IllegalParams);
        else
            pParent->other |= NFEX_METHOD_PARAMS;
        NextToken();
    }

    if (CurToken() == TID_REF) {
        if (flags & kppoNoRefOrOut)
            Error(ERR_IllegalRefParam);
        else
            pParm->flags = NF_PARMMOD_REF;
        NextToken();
    }
    else if (CurToken() == TID_OUT) {
        if (flags & kppoNoRefOrOut)
            Error(ERR_IllegalRefParam);
        else
            pParm->flags = NF_PARMMOD_OUT;
        NextToken();
    }

    pParm->pType = ParseType (pParm);

    if ((pParm->pType->kind == NK_PREDEFINEDTYPE) &&
        pParm->pType->asPREDEFINEDTYPE()->iType == PT_VOID && CurToken() == TID_CLOSEPAREN)
    {
        return NULL;
    }

    if (!(flags & kppoNoNames)) {
        pParm->pName = ParseIdentifier (pParm);

        if (CurToken() == TID_OPENSQUARE && PeekToken() == TID_CLOSESQUARE) {
            ErrorAtToken( CurTokenIndex(), ERR_BadArraySyntax);
            NextToken();
            NextToken();
        }
    }

    if (CurToken() == TID_EQUAL) {
        Error(ERR_NoDefaultArgs);
        NextToken();
        ParseExpression(pParent);
    }

    return pParm;
}

//
//

BASENODE *CParser::SetDefaultAttributeLocation(BASENODE *pAttrList, ATTRLOC defaultLocation, ATTRLOC validLocations)
{
    ASSERT(0 != defaultLocation);
    ASSERT(0 != validLocations);
    ASSERT(0 != (defaultLocation & validLocations));

    NODELOOP(pAttrList, ATTRDECL, pAttrSection)
        if (pAttrSection->location == 0)
        {
            pAttrSection->location = defaultLocation;
        }
        else if (!((validLocations | AL_UNKNOWN) & pAttrSection->location))
        {
            WCHAR szValidLocations[256];
            szValidLocations[0] = 0;
            ATTRLOC validTemp = validLocations;
            for (int i = 0; validTemp != 0; i += 1, (validTemp = (ATTRLOC) (validTemp >> 1)))
            {
                if (validTemp & 1)
                {
                    if (szValidLocations[0])
                    {
                        StringCchCatW(szValidLocations, lengthof(szValidLocations), L", ");
                    }
                    StringCchCatW(szValidLocations, lengthof(szValidLocations), GetNameMgr()->GetAttrLoc(i)->text);
                }
            }

            ErrorAtToken(pAttrSection->pNameNode->tokidx, WRN_AttributeLocationOnBadDeclaration, pAttrSection->pNameNode->pName->text, szValidLocations);
            pAttrSection->location = AL_UNKNOWN;
        }
    ENDLOOP

    return pAttrList;
}


BASENODE *CParser::ParseAttributes (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations)
{
    bool fAttributeHadErrorThrowaway;
    return ParseAttributes(pParent, defaultLocation, validLocations, &fAttributeHadErrorThrowaway);
}


//

BASENODE *CParser::ParseAttributes (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations, bool* pfAttributeHadError)
{
    if (CurToken() != TID_OPENSQUARE)
        return NULL;

    BASENODE    *pList;
    CListMaker   list (this, &pList);
    BASENODE    *pSection;

    while (NULL != (pSection = ParseAttributeSection(pParent, defaultLocation, validLocations, pfAttributeHadError)))
        list.Add(pSection);

    return pList;
}

//

BASENODE *CParser::ParseAttributeSection (BASENODE *pParent, ATTRLOC defaultLocation, ATTRLOC validLocations, bool* pfAttributeHadError)
{
    if (CurToken() != TID_OPENSQUARE)
        return NULL;

    ATTRDECLNODE    *pAttr = AllocNode(NK_ATTRDECL, pParent)->asATTRDECL();

    pAttr->location = (ATTRLOC) 0;
    pAttr->pNameNode = NULL;

    Eat (TID_OPENSQUARE);

    //
    //
    if (CurToken() <= TID_IDENTIFIER && PeekToken() == TID_COLON)
    {
        if (CurToken() == TID_IDENTIFIER && m_pTokens[CurTokenIndex()].Name() == GetNameMgr()->GetPredefName(PN_MODULE))
            ReportFeatureUse(IDS_FeatureModuleAttrLoc);

        pAttr->pNameNode = ParseIdentifierOrKeyword(pAttr);
        Eat(TID_COLON);
        if (!GetNameMgr()->IsAttrLoc(pAttr->pNameNode->pName, (int*) &pAttr->location))
        {
            ErrorAtToken(pAttr->pNameNode->tokidx, WRN_InvalidAttributeLocation, pAttr->pNameNode->pName->text);
            pAttr->location = AL_UNKNOWN;
        }
        else if (validLocations != 0)
        {
            SetDefaultAttributeLocation(pAttr, defaultLocation, validLocations);
        }
    }

    //
    //
    if (pAttr->location == 0)
    {
        pAttr->pNameNode = NULL;
        pAttr->location = defaultLocation;
    }

    //
    //
    CListMaker      list (this, &pAttr->pAttr);
    long            iComma = -1;
    while (TRUE)
    {
        list.Add (ParseAttribute (pAttr, pfAttributeHadError), iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
        if (CurToken() == TID_CLOSESQUARE)
            break;
    }

    pAttr->iClose = CurTokenIndex();
    Eat(TID_CLOSESQUARE);
    return pAttr;
}


BASENODE *CParser::ParseAttribute (BASENODE *pParent, bool* pfAttributeHadError)
{
    ATTRNODE    *pAttr = AllocNode(NK_ATTR, pParent)->asATTR();

    long mark = CurTokenIndex();
    pAttr->pName = ParseDottedName(pAttr, true);
    if (CurToken() == TID_OPENANGLE) {
        ErrorAfterPrevToken(ERR_AttributeCantBeGeneric);
        Rewind(mark);
        ParseGenericQualifiedNameList(pAttr, false);
    }

    CListMaker   list (this, &pAttr->pArgs);

    pAttr->iOpen = pAttr->iClose = -1;
    if (CurToken() == TID_OPENPAREN)
    {
        pAttr->iOpen = CurTokenIndex();
        NextToken();

        long        iTokOpen = CurTokenIndex(), iTokIdx = iTokOpen;
        BOOL        fNamed = FALSE;
        long        iErrors = m_iErrors;

        if (CurToken() != TID_CLOSEPAREN)
        {
            while (TRUE)
            {
                BASENODE    *pNew = list.Add (ParseAttributeArgument (pAttr, &fNamed));

                if (iTokIdx != iTokOpen)
                    pNew->tokidx = iTokIdx;

                if (CurToken() == TID_COMMA)
                    iTokIdx = CurTokenIndex();
                else
                    break;

                NextToken ();
            }
        }

        pAttr->iClose = CurTokenIndex();
        Eat (TID_CLOSEPAREN);
        if (iErrors != m_iErrors)
        {
            *pfAttributeHadError = true;
            pAttr->flags |= NF_CALL_HADERROR;
        }
    }

    return pAttr;
}


BASENODE *CParser::ParseAttributeArgument (BASENODE *pParent, BOOL *pfNamed)
{
    ATTRNODE    *pArg = AllocNode(NK_ATTRARG, pParent)->asATTRARG();

    pArg->iOpen = pArg->iClose = -1;

    if (CurToken() == TID_IDENTIFIER && PeekToken (1) == TID_EQUAL)
    {
        pArg->pName = ParseIdentifier (pArg);
        Eat (TID_EQUAL);
        *pfNamed = TRUE;
    }
    else
    {
        if (*pfNamed)
            Error (ERR_NamedArgumentExpected);
        pArg->pName = NULL;
    }

    pArg->pArgs = ParseExpression (pArg);
    return pArg;
}

bool CParser::IsLocalDeclaration(ScanTypeFlagsEnum st)
{

    if (st == ScanTypeFlags::MustBeType && CurToken() != TID_DOT)
        return true;
    if (st == ScanTypeFlags::NotType || CurToken() != TID_IDENTIFIER)
        return false;
    if (st == ScanTypeFlags::NullableType) {
    }
    return true;
}


STATEMENTNODE *CParser::ParseStatement (BASENODE *pParent, long iClose)
{
    TOKENID         iTok = CurToken();
    STATEMENTNODE   *pStmt;
    long            iErrorCount = m_iErrors;

    if (m_rgTokenInfo[iTok].iStmtParser != 0)
    {
        switch (m_rgTokenInfo[iTok].iStmtParser) {
#define PARSERDEF(name) \
        case EParse##name: \
            pStmt = Parse##name (pParent, iClose); \
            break;
#include "parsertype.h"
        default:
            VSFAIL("Unexpected parser type");
            pStmt = NULL;
            break;
        }
    }
    else if (iTok == TID_IDENTIFIER && PeekToken(1) == TID_COLON)
    {
        pStmt = ParseLabeledStatement (pParent, iClose);
    }
    else if (CheckForSpecName(SN_YIELD) && (PeekToken(1) == TID_RETURN || PeekToken(1) == TID_BREAK))
    {
        pStmt = ParseYieldStatement (pParent);
    }
    else if (iTok == TID_SEMICOLON)
    {
        pStmt = AllocNode(NK_EMPTYSTMT, pParent)->asEMPTYSTMT();
        NextToken();
    }
    else
    {
        long    iMark = CurTokenIndex();
        ScanTypeFlagsEnum st;

        if (CurToken() == TID_STATIC || CurToken() == TID_READONLY || CurToken() == TID_VOLATILE)
        {
            while (CurToken() == TID_STATIC || CurToken() == TID_READONLY || CurToken() == TID_VOLATILE)
                NextToken();

            long iNewMark = CurTokenIndex();
            st = ScanType();
            if (IsLocalDeclaration(st))
            {
                ErrorAtToken( iMark, ERR_BadMemberFlag, GetTokenText(iMark));
                Rewind(iMark = iNewMark);
                goto PARSE_DECLARATION;
            }
            Rewind(iMark);
        }

        st = ScanType();

        if (IsLocalDeclaration(st))
        {
            Rewind (iMark);
PARSE_DECLARATION:
            pStmt = ParseDeclarationStatement (pParent, iClose);
        }
        else
        {
            Rewind (iMark);
            pStmt = ParseExpressionStatement (pParent, iClose);
        }
    }

    pStmt->pNext = NULL;
    if (iErrorCount != m_iErrors)
        pStmt->flags |= NF_STMT_HADERROR;
    return pStmt;
}




STATEMENTNODE *CParser::ParseBlock (BASENODE *pParent, long iClose)
{
    BLOCKNODE   *pBlock = AllocNode(NK_BLOCK, pParent)->asBLOCK();

    pBlock->pNext = NULL;
    pBlock->pStatements = NULL;

    if (pParent)
    {
        switch (pParent->kind) 
        {
            case NK_ACCESSOR:
                iClose = pParent->asACCESSOR()->iClose;
                break;

            case NK_METHOD:
            case NK_CTOR:
            case NK_DTOR:
            case NK_OPERATOR:
                iClose = pParent->asANYMETHOD()->iClose;
                break;

            case NK_INDEXER:
            case NK_PROPERTY:
                iClose = pParent->asANYPROPERTY()->iClose;
                break;

            default:
                iClose = -1;
        }
    }

    Eat (TID_OPENCURLY);

    STATEMENTNODE   **ppNext = &pBlock->pStatements;

    while (!PastCloseToken(iClose) && CurToken() != TID_ENDFILE)
    {
        if ((iClose == -1 &&
            (CurToken() == TID_CLOSECURLY || CurToken() == TID_FINALLY || CurToken() == TID_CATCH)))
            break;
        if (CurToken() == TID_CLOSECURLY) {
            Eat (TID_OPENCURLY);
            NextToken();
            continue;
        }

        CEnsureParserProgress epp(this);

        *ppNext = ParseStatement (pBlock, iClose);
        ppNext = &(*ppNext)->pNext;
        ASSERT (*ppNext == NULL);
    }

    pBlock->iClose = CurTokenIndex();
    if (iClose != pBlock->iClose)
        Eat (TID_CLOSECURLY);
    else if (CurToken() == TID_CLOSECURLY)
        NextToken();
    else
        ASSERT(m_iErrors);

    return pBlock;
}

//

STATEMENTNODE *CParser::ParseBreakStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_BREAK || CurToken() == TID_CONTINUE);

    EXPRSTMTNODE    *pStmt = AllocNode (CurToken() == TID_BREAK ? NK_BREAK : NK_CONTINUE, pParent)->asANYEXPRSTMT();
    pStmt->pArg = NULL;
    NextToken();
    Eat (TID_SEMICOLON);
    return pStmt;
}


STATEMENTNODE *CParser::ParseConstStatement (BASENODE *pParent, long iClose)
{
    Eat(TID_CONST);

    DECLSTMTNODE    *pStmt = AllocNode(NK_DECLSTMT, pParent)->asDECLSTMT();
    pStmt->pType = ParseType (pStmt);
    pStmt->flags |= NF_CONST_DECL;

    BOOL        fFirst = TRUE;
    CListMaker  list(this, &pStmt->pVars);
    long        iComma = -1;

    while (TRUE)
    {
        list.Add (ParseVariableDeclarator (pStmt, pStmt, PVDF_CONST, fFirst), iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
        fFirst = FALSE;
    }
    Eat (TID_SEMICOLON);
    pStmt->pNext = NULL;
    return pStmt;
}


STATEMENTNODE *CParser::ParseLabeledStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_IDENTIFIER);

    LABELSTMTNODE   *pStmt = AllocNode(NK_LABEL, pParent)->asLABEL();

    pStmt->pLabel = ParseIdentifier (pStmt);
    Eat (TID_COLON);
    pStmt->pStmt = ParseStatement (pStmt, iClose);
    return pStmt;
}


STATEMENTNODE *CParser::ParseDoStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_DO);

    LOOPSTMTNODE    *pStmt = AllocNode(NK_DO, pParent)->asDO();
    NextToken();
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, false);
    Eat (TID_WHILE);
    Eat (TID_OPENPAREN);
    pStmt->pExpr = ParseExpression (pStmt);
    Eat (TID_CLOSEPAREN);
    Eat (TID_SEMICOLON);
    return pStmt;
}


STATEMENTNODE *CParser::ParseForStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_FOR);

    FORSTMTNODE     *pStmt = AllocNode(NK_FOR, pParent)->asFOR();
    NextToken();
    Eat (TID_OPENPAREN);

    long    iMark = CurTokenIndex();

    ScanTypeFlagsEnum st = ScanType();

    if (st != ScanTypeFlags::NotType && CurToken() == TID_IDENTIFIER)
    {
        Rewind (iMark);
        pStmt->pInit = ParseDeclarationStatement (pStmt, iClose);
    }
    else
    {
        Rewind (iMark);
        if (CurToken() == TID_SEMICOLON)
        {
            pStmt->pInit = NULL;
            NextToken();
        }
        else
        {
            pStmt->pInit = ParseExpressionList (pStmt);
            Eat (TID_SEMICOLON);
        }
    }

    pStmt->pExpr = (CurToken() != TID_SEMICOLON) ? ParseExpression (pStmt) : NULL;
    Eat (TID_SEMICOLON);
    pStmt->pInc = (CurToken() != TID_CLOSEPAREN) ? ParseExpressionList (pStmt) : NULL;
    pStmt->iCloseParen = CurTokenIndex();
    pStmt->iInKeyword = -1;
    Eat (TID_CLOSEPAREN);
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, true);
    return pStmt;
}


STATEMENTNODE *CParser::ParseForEachStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_FOREACH);

    FORSTMTNODE     *pStmt = AllocNode(NK_FOR, pParent)->asFOR();
    pStmt->flags = NF_FOR_FOREACH;
    pStmt->pInc = NULL;
    pStmt->iInKeyword = -1;
    NextToken();
    Eat (TID_OPENPAREN);

    //
    DECLSTMTNODE    *pDecl = AllocNode(NK_DECLSTMT, pStmt)->asDECLSTMT();
    pDecl->pType = ParseType (pDecl);
    pDecl->pNext = NULL;

    if (CurToken() == TID_IN) 
        Error (ERR_BadForeachDecl);    

    VARDECLNODE *pVar = AllocNode(NK_VARDECL, pDecl)->asVARDECL();
    pVar->pArg = NULL;
    pVar->pDecl = pDecl;
    pVar->pName = ParseIdentifier (pVar);
    pDecl->pVars = pVar;
    pStmt->pInit = pDecl;

    if (CurToken() != TID_IN) {
        Error (ERR_InExpected);
    } else {
        pStmt->iInKeyword = CurTokenIndex();
        NextToken();
    }

    pStmt->pExpr = ParseExpression (pStmt);
    pStmt->iCloseParen = CurTokenIndex();
    Eat (TID_CLOSEPAREN);
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, true);
    return pStmt;
}


STATEMENTNODE *CParser::ParseGotoStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_GOTO);

    EXPRSTMTNODE    *pStmt = AllocNode(NK_GOTO, pParent)->asGOTO();
    NextToken();
    if (CurToken() == TID_CASE || CurToken() == TID_DEFAULT)
    {
        TOKENID tok = CurToken();

        pStmt->flags = NF_GOTO_CASE;
        NextToken();
        pStmt->pArg = (tok == TID_CASE) ? ParseExpression (pStmt) : NULL;
    }
    else
    {
        pStmt->pArg = ParseIdentifier (pStmt);
    }

    Eat (TID_SEMICOLON);
    return pStmt;
}


STATEMENTNODE *CParser::ParseIfStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_IF);

    IFSTMTNODE      *pStmt = AllocNode(NK_IF, pParent)->asIF();
    NextToken();
    Eat (TID_OPENPAREN);
    pStmt->pExpr = ParseExpression (pStmt);
    Eat (TID_CLOSEPAREN);
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, false);
    if (CurToken() == TID_ELSE)
    {
        NextToken();
        pStmt->pElse = ParseEmbeddedStatement (pStmt, false);
    }
    else
    {
        pStmt->pElse = NULL;
    }
    return pStmt;
}


STATEMENTNODE *CParser::ParseReturnStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_RETURN);

    EXPRSTMTNODE    *pStmt = AllocNode(NK_RETURN, pParent)->asRETURN();
    NextToken();
    if (CurToken() == TID_SEMICOLON)
        pStmt->pArg = NULL;
    else
        pStmt->pArg = ParseExpression (pStmt);
    Eat (TID_SEMICOLON);
    return pStmt;
}


STATEMENTNODE *CParser::ParseYieldStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CheckForSpecName(SN_YIELD));

    EXPRSTMTNODE    *pStmt = AllocNode(NK_YIELD, pParent)->asYIELD();
    pStmt->pArg = NULL;

    ReportFeatureUse(IDS_FeatureIterators);

    NextToken();
    if (CurToken() == TID_BREAK) {
        Eat (TID_BREAK);
    } else {
        Eat (TID_RETURN);
        if(CurToken() == TID_SEMICOLON) {
            Error (ERR_EmptyYield);
        } else {
            pStmt->pArg = ParseExpression (pStmt);
        }
    }

    Eat (TID_SEMICOLON);
    return pStmt;
}


bool CParser::AtSwitchCase(long iClose)
{
    return !PastCloseToken(iClose) &&
        (CurToken() == TID_CASE) ||
        (CurToken() == TID_DEFAULT && PeekToken() != TID_OPENPAREN);
}


STATEMENTNODE *CParser::ParseSwitchStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_SWITCH);

    SWITCHSTMTNODE  *pStmt = AllocNode(NK_SWITCH, pParent)->asSWITCH();
    NextToken();
    Eat (TID_OPENPAREN);
    pStmt->pExpr = ParseExpression (pStmt);
    Eat (TID_CLOSEPAREN);
    pStmt->iOpen = CurTokenIndex();
    Eat (TID_OPENCURLY);

    CListMaker   list (this, &pStmt->pCases);

    if (CurToken() == TID_CLOSECURLY)
        Error (WRN_EmptySwitch);

    while (AtSwitchCase(iClose)) {
        CEnsureParserProgress epp(this);

        list.Add (ParseSwitchCase (pStmt, iClose));
    }

    pStmt->iClose = CurTokenIndex();
    Eat (TID_CLOSECURLY);
    return pStmt;
}


BASENODE *CParser::ParseSwitchCase (BASENODE *pParent, long iClose)
{
    CASENODE    *pCase = AllocNode(NK_CASE, pParent)->asCASE();

    ASSERT(AtSwitchCase(iClose));
    CListMaker   list (this, &pCase->pLabels);

    do
    {
        UNOPNODE    *pLabel = AllocNode(NK_CASELABEL, pCase)->asCASELABEL();
        if (CurToken() == TID_CASE)
        {
            NextToken();
            if (CurToken() == TID_COLON)
            {
                Error( ERR_ConstantExpected);
                pLabel->p1 = ParseMissingName(pLabel, CurTokenIndex());
            }
            else
            {
                pLabel->p1 = ParseExpression (pLabel);
            }
        }
        else
        {
            ASSERT(CurToken() == TID_DEFAULT);

            NextToken();
            pLabel->p1 = NULL;
        }

        Eat (TID_COLON);
        list.Add (pLabel);
    } while (AtSwitchCase(iClose));

    STATEMENTNODE** ppNext = &pCase->pStmts;

    while (!PastCloseToken(iClose) &&
           !AtSwitchCase(iClose) &&
           CurToken() != TID_ENDFILE && 
           CurToken() != TID_CLOSECURLY)
    {
        CEnsureParserProgress epp(this);

        *ppNext = ParseStatement (pCase);
        ppNext = &(*ppNext)->pNext;
    }

    *ppNext = NULL;
    return pCase;
}


STATEMENTNODE *CParser::ParseThrowStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_THROW);

    EXPRSTMTNODE    *pStmt = AllocNode(NK_THROW, pParent)->asTHROW();
    NextToken();
    pStmt->pArg = (CurToken() != TID_SEMICOLON) ? ParseExpression (pStmt) : NULL;
    Eat (TID_SEMICOLON);
    return pStmt;
}


STATEMENTNODE *CParser::ParseTryStatement (BASENODE *pParent, long iClose)
{
    TRYSTMTNODE     *pStmt = AllocNode(NK_TRY, pParent)->asTRY();

    switch (CurToken())
    {
    case TID_TRY:
        NextToken ();
        pStmt->pBlock = ParseBlock (pStmt)->asBLOCK();
        break;
    case TID_CATCH:
    case TID_FINALLY:
        Eat(TID_TRY);
        pStmt->pBlock = NULL;
        break;
    default:
        ASSERT(!"CurToken() not try, catch, or finally");
    }

    CListMaker   list (this, &pStmt->pCatch);
    if (CurToken() == TID_CATCH)
    {
        BOOL    fEmpty = FALSE;
        BASENODE * pLastAdded = NULL;

        while (CurToken() == TID_CATCH)
        {
            pLastAdded = ParseCatchClause (pStmt, &fEmpty);
            list.Add (pLastAdded);
        }

        pStmt->flags |= NF_TRY_CATCH;

        if (CurToken() == TID_FINALLY)
        {
            TRYSTMTNODE *pInnerTry = pStmt;

            pStmt = AllocNode(NK_TRY, pParent, pInnerTry->tokidx)->asTRY();

            BLOCKNODE   *pBlock = AllocNode(NK_BLOCK, pStmt, pStmt->tokidx)->asBLOCK();
            pBlock->pNext = NULL;
            pBlock->pStatements = pInnerTry;
            pBlock->iClose = pLastAdded->asCATCH()->pBlock->iClose;

            pInnerTry->pParent = pBlock;
            pInnerTry->pNext = NULL;
            pStmt->pBlock = pBlock;

            NextToken();
            pStmt->pCatch = ParseBlock (pStmt);
            pStmt->flags |= NF_TRY_FINALLY;
        }
    }
    else if (CurToken() == TID_FINALLY)
    {
        NextToken();
        pStmt->pCatch = ParseBlock (pStmt);
        pStmt->flags |= NF_TRY_FINALLY;
    }
    else
    {
        Error (ERR_ExpectedEndTry);
    }

    return pStmt;
}


STATEMENTNODE *CParser::ParseWhileStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_WHILE);

    LOOPSTMTNODE    *pStmt = AllocNode(NK_WHILE, pParent)->asWHILE();
    NextToken();
    Eat (TID_OPENPAREN);
    pStmt->pExpr = ParseExpression (pStmt);
    Eat (TID_CLOSEPAREN);
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, true);
    return pStmt;
}



STATEMENTNODE *CParser::ParseDeclaration (BASENODE *pParent, long iClose)
{
    DECLSTMTNODE    *pStmt = AllocNode(NK_DECLSTMT, pParent)->asDECLSTMT();
    pStmt->pType = ParseType (pStmt);

    BOOL        fFirst = TRUE;
    CListMaker  list(this, &pStmt->pVars);
    long        iComma = -1;

    while ( ! PastCloseToken(iClose))
    {        
        list.Add (ParseVariableDeclarator (pStmt, pStmt, PVDF_LOCAL, fFirst, iClose), iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
        fFirst = FALSE;
    }
    if (pStmt->pVars == NULL)
    {
        VARDECLNODE *pVar = AllocNode(NK_VARDECL, pStmt)->asVARDECL();
        pVar->pDecl = pStmt;
        pVar->pName = ParseMissingName (pVar, CurTokenIndex());
        pVar->pArg = NULL;

        pStmt->pVars = pVar;
    }
    pStmt->pNext = NULL;
    return pStmt;

}


STATEMENTNODE *CParser::ParseDeclarationStatement (BASENODE *pParent, long iClose)
{
    STATEMENTNODE * node = ParseDeclaration (pParent, iClose);
    Eat (TID_SEMICOLON);
    return node;
}


STATEMENTNODE *CParser::ParseExpressionStatement (BASENODE *pParent, long iClose)
{
    EXPRSTMTNODE    *pStmt = AllocNode(NK_EXPRSTMT, pParent)->asEXPRSTMT();
    pStmt->pArg = ParseExpression (pStmt, iClose);
    Eat (TID_SEMICOLON);
    pStmt->pNext = NULL;
    return pStmt;
}


STATEMENTNODE *CParser::ParseLockStatement (BASENODE *pParent, long iClose)
{
    ASSERT (CurToken() == TID_LOCK);

    LOOPSTMTNODE    *pStmt = AllocNode(NK_LOCK, pParent)->asLOCK();
    NextToken ();
    Eat (TID_OPENPAREN);
    pStmt->pExpr = ParseExpression (pStmt);
    Eat (TID_CLOSEPAREN);
    pStmt->pStmt = ParseEmbeddedStatement (pStmt, false);
    return pStmt;
}


STATEMENTNODE *CParser::ParseFixedStatement (BASENODE *pParent, long iClose)
{
    FORSTMTNODE     *pStmt = AllocNode(NK_FOR, pParent)->asFOR();

    pStmt->flags |= NF_FIXED_DECL;
    NextToken();
    Eat (TID_OPENPAREN);

    pStmt->pInit = ParseDeclaration (pStmt, iClose);
    pStmt->pInit->flags |= NF_FIXED_DECL;

    pStmt->pExpr = NULL;
    pStmt->pInc = NULL;
    pStmt->iCloseParen = CurTokenIndex();
    pStmt->iInKeyword = -1;
    Eat (TID_CLOSEPAREN);

    pStmt->pStmt = ParseEmbeddedStatement (pStmt, false);
    pStmt->pNext = NULL;

    return pStmt;
}


STATEMENTNODE *CParser::ParseUsingStatement (BASENODE *pParent, long iClose)
{
    FORSTMTNODE     *pStmt = AllocNode(NK_FOR, pParent)->asFOR();

    pStmt->flags |= NF_USING_DECL;
    NextToken();
    Eat (TID_OPENPAREN);


    long    iMark = CurTokenIndex();
    ScanTypeFlagsEnum st = ScanType();

    if (st == ScanTypeFlags::NullableType) {

        if (CurToken() != TID_IDENTIFIER)
            goto LExpr;

        switch (PeekToken()) {
        default:
            goto LExpr;

        case TID_COMMA:
        case TID_CLOSEPAREN:
            goto LDecl;

        case TID_EQUAL:
            break;
        }


        Rewind (iMark);
        DECLSTMTNODE * nodeDecl = ParseDeclaration (pStmt)->asDECLSTMT();
        ASSERT(nodeDecl->pType->kind == NK_NULLABLETYPE || nodeDecl->pType->kind == NK_POINTERTYPE);

        if (CurToken() == TID_COLON &&
            nodeDecl->pType->asNULLABLETYPE()->pElementType->kind == NK_NAMEDTYPE &&
            nodeDecl->pVars->kind  == NK_VARDECL)
        {
            NAMEDTYPENODE * nodeType = nodeDecl->pType->asNULLABLETYPE()->pElementType->asNAMEDTYPE();
            VARDECLNODE * nodeVar = nodeDecl->pVars->asVARDECL();
            ASSERT(nodeVar->pName && nodeVar->pArg);

            long itok = CurTokenIndex();
            NextToken();
            BINOPNODE * nodeColon = AllocBinaryOpNode(OP_COLON, itok, pStmt, nodeVar->pArg, ParseExpression(pStmt));
            pStmt->pExpr = AllocBinaryOpNode(OP_QUESTION, iMark, pStmt, nodeType->pName, nodeColon);
            pStmt->pInit = NULL;
        }
        else {
            pStmt->pInit = nodeDecl;
            pStmt->pInit->flags |= NF_USING_DECL;
            pStmt->pExpr = NULL;
        }
    }
    else if (st == ScanTypeFlags::MustBeType && CurToken() != TID_DOT ||
        st != ScanTypeFlags::NotType && CurToken() == TID_IDENTIFIER && (st == ScanTypeFlags::NonGenericTypeOrExpr || PeekToken() == TID_EQUAL))
    {
LDecl:
        Rewind (iMark);
        pStmt->pInit = ParseDeclaration (pStmt);
        pStmt->pInit->flags |= NF_USING_DECL;
        pStmt->pExpr = NULL;
    }
    else
    {
LExpr:
        Rewind (iMark);
        pStmt->pExpr = ParseExpression(pStmt);
        pStmt->pInit = NULL;
    }

    pStmt->pInc = NULL;
    pStmt->iCloseParen = CurTokenIndex();
    Eat (TID_CLOSEPAREN);

    pStmt->pStmt = ParseEmbeddedStatement (pStmt, false);
    pStmt->pNext = NULL;
    pStmt->iInKeyword = -1;

    return pStmt;
}



STATEMENTNODE *CParser::ParseUnsafeStatement(BASENODE *pParent, long iClose)
{
    ASSERT(CurToken() == TID_UNSAFE);

    LABELSTMTNODE   *pStmt = AllocNode(NK_UNSAFE, pParent)->asANYLABELSTMT();

    pStmt->pLabel = NULL;
    NextToken();
    pStmt->pStmt = ParseBlock(pStmt);

    return pStmt;
}


STATEMENTNODE *CParser::ParseCheckedStatement(BASENODE *pParent, long iClose)
{
    ASSERT(CurToken() == TID_CHECKED || CurToken() == TID_UNCHECKED);

    if (PeekToken() == TID_OPENPAREN)
        return ParseExpressionStatement(pParent, iClose);

    LABELSTMTNODE   *pStmt = AllocNode(NK_CHECKED, pParent)->asANYLABELSTMT();

    pStmt->pLabel = NULL;
    if (CurToken() == TID_UNCHECKED)
        pStmt->flags |= NF_UNCHECKED;
    NextToken();
    pStmt->pStmt = ParseBlock (pStmt);

    return pStmt;
}


STATEMENTNODE *CParser::ParseEmbeddedStatement (BASENODE *pParent, BOOL fComplexCheck)
{
    if (CurToken() == TID_SEMICOLON)
    {
        if (!fComplexCheck || PeekToken() == TID_OPENCURLY)
        {
            Error (WRN_PossibleMistakenNullStatement);
        }
    }

    STATEMENTNODE   *pStmt = ParseStatement (pParent);

    if (pStmt != NULL)
    {
        if (pStmt->kind == NK_LABEL || pStmt->kind == NK_DECLSTMT)
            ErrorAtToken (pStmt->tokidx, ERR_BadEmbeddedStmt);
    }

    return pStmt;
}

//

BASENODE *CParser::ParseVariableDeclarator (BASENODE *pParent, BASENODE *pParentDecl, DWORD dwFlags, BOOL fFirst, long iClose)
{
    VARDECLNODE *pVar = AllocNode(NK_VARDECL, pParent)->asVARDECL();
    pVar->pDecl = pParentDecl;
    pVar->pName = ParseIdentifier (pVar);
    pVar->pArg = NULL;

    switch (CurToken())
    {
    case TID_EQUAL:
    {
        if (dwFlags & PVDF_FIXED) goto DEFAULTCASE;
        pVar->flags |= NF_VARDECL_EXPR;
        long iIdxEqual = CurTokenIndex();
        Eat (TID_EQUAL);
        ASSERT(pParent->kind == NK_FIELD || pParent->kind == NK_CONST || pParent->kind == NK_DECLSTMT);
        pVar->pArg = ParseVariableInitializer (pVar, (dwFlags & (PVDF_LOCAL|PVDF_CONST)) == PVDF_LOCAL, iClose);
        pVar->pArg = AllocBinaryOpNode (OP_ASSIGN, iIdxEqual, pVar, AllocNameNode (pVar->pName->pName, pVar->pName->tokidx), pVar->pArg);
        break;
    }

    case TID_OPENPAREN:
        Error (ERR_BadVarDecl);

        if (PeekToken() == TID_CLOSEPAREN) {
            NextToken();
            NextToken();
        }
        else {
            long itok = CurTokenIndex();
            pVar->pArg = ParseArgumentList(pVar);
            if (pVar->pArg->kind == NK_LIST) {
                CALLNODE * nodeCall = AllocNode(NK_CALL, pVar, itok)->asCALL();
                nodeCall->other = OP_CALL;
                nodeCall->p2 = pVar->pArg;
                nodeCall->p2->pParent = nodeCall;
                nodeCall->p1 = ParseMissingName(nodeCall, itok);
                nodeCall->iClose = PeekTokenIndex(-1);
                pVar->pArg = nodeCall;
            }
            pVar->pArg = AllocBinaryOpNode(OP_ASSIGN, itok, pVar, AllocNameNode(pVar->pName->pName, pVar->pName->tokidx), pVar->pArg);
        }
        break;

    case TID_OPENSQUARE:
        if (dwFlags == PVDF_FIXED)
        {
            NextToken();
            pVar->flags |= NF_VARDECL_ARRAY;
            if (CurToken() == TID_CLOSESQUARE)
            {
                pVar->pArg = ParseMissingName(pVar, CurTokenIndex());
                Error (ERR_ValueExpected);
            }
            else
            {
                pVar->pArg = ParseExpression (pVar);
            }
            Eat (TID_CLOSESQUARE);
            break;
        }
        else
        {
            Error (ERR_CStyleArray);
            ParseArrayRankSpecifier(pVar,true);
            pVar->pArg = NULL;
        }
        break;
        
    case TID_IDENTIFIER:
        if (! fFirst) {
            Error(ERR_MultiTypeInDeclaration);
            NextToken();
            if (CurToken() == TID_EQUAL) {
                NextToken();
                ParseVariableInitializer(pVar, TRUE);
            }            
            break;
        }
        
        goto DEFAULTCASE;
        
    default:
DEFAULTCASE:    
        if (dwFlags & PVDF_CONST)
            Error (ERR_ConstValueRequired);
        else if (dwFlags & PVDF_FIXED) {
            if (pParentDecl && pParentDecl->asFIELD()->pType &&
                pParentDecl->asFIELD()->pType->kind == NK_ARRAYTYPE) {
                Error (ERR_FixedDimsRequired);
            }
            else
                Eat (TID_OPENSQUARE);
        }
        pVar->pArg = NULL;
    }

    return pVar;
}


BASENODE *CParser::ParseVariableInitializer (BASENODE *pParent, BOOL fAllowStackAlloc, long iClose)
{
    if (fAllowStackAlloc && CurToken() == TID_STACKALLOC)
        return ParseStackAllocExpression (pParent);

    if (CurToken() == TID_OPENCURLY)
        return ParseArrayInitializer (pParent);

    return ParseExpression (pParent, iClose);
}


BASENODE *CParser::ParseArrayInitializer (BASENODE *pParent)
{
    UNOPNODE    *pInit = AllocNode(NK_ARRAYINIT, pParent)->asARRAYINIT();

    Eat (TID_OPENCURLY);

    CListMaker  list (this, &pInit->p1);
    long        iComma = -1;

    while (CurToken() != TID_CLOSECURLY)
    {
        list.Add (ParseVariableInitializer (pInit, FALSE), iComma);
        if (CurToken() != TID_COMMA)
            break;
        iComma = CurTokenIndex();
        NextToken();
    }

    Eat (TID_CLOSECURLY);
    return pInit;
}


BASENODE *CParser::ParseStackAllocExpression (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_STACKALLOC);

    NEWNODE     *pNew = AllocNode(NK_NEW, pParent)->asNEW();

    Eat (TID_STACKALLOC);

    POINTERTYPENODE *pPointerType = AllocNode(NK_POINTERTYPE, pNew)->asPOINTERTYPE();

    pNew->flags |= NF_NEW_STACKALLOC;
    pNew->pType = pPointerType;
    pNew->pInit = NULL;
    pNew->pArgs = NULL;
    pNew->iOpen = pNew->iClose = -1;

    bool fHadError = false;
    pPointerType->pElementType = ParseUnderlyingType(pPointerType, &fHadError);

    if (fHadError)
        return pNew;

    pPointerType->pElementType = ParsePointerTypeMods(pPointerType, pPointerType->pElementType);

    if (CurToken() != TID_OPENSQUARE)
    {
        Error (ERR_BadStackAllocExpr);
    }
    else
    {
        pNew->iOpen = CurTokenIndex();
        Eat (TID_OPENSQUARE);
        pNew->pArgs = ParseExpression (pNew);
        pNew->iClose = CurTokenIndex();
        Eat (TID_CLOSESQUARE);
    }

    return pNew;
}


BASENODE *CParser::ParseCatchClause (BASENODE *pParent, BOOL *pfEmpty)
{
    ASSERT (CurToken() == TID_CATCH);

    CATCHNODE   *pCatch = AllocNode(NK_CATCH, pParent)->asCATCH();

    if (*pfEmpty)
        Error (ERR_TooManyCatches);

    NextToken();

    if (CurToken() == TID_OPENPAREN)
    {
        NextToken();
        pCatch->pType = ParseClassType (pCatch);
        if (CurToken() == TID_IDENTIFIER)
            pCatch->pName = ParseIdentifier (pCatch);
        else
            pCatch->pName = NULL;
        Eat (TID_CLOSEPAREN);
    }
    else
    {
        pCatch->pType = NULL;
        pCatch->pName = NULL;
        *pfEmpty = TRUE;
    }

    pCatch->pBlock = ParseBlock (pCatch)->asBLOCK();
    return pCatch;
}


BASENODE* CParser::ParseExpression (BASENODE *pParent, long iClose) 
{ 
    BASENODE *p = ParseSubExpression (pParent, 0, iClose); 
    p->pParent = pParent; 
    return p; 
}


BASENODE *CParser::ParseSubExpression (BASENODE *pParent, UINT iPrecedence, long iClose)
{
    BASENODE *pLeftOperand;
    UINT        iNewPrec;
    OPERATOR    op;

    //
    if (m_rgTokenInfo[CurToken()].dwFlags & TFF_INVALIDSUBEXPRESSION)
    {
        Error(ERR_InvalidExprTerm, m_rgTokenInfo[CurToken()].pszText);
        return ParseMissingName(pParent, CurTokenIndex());        
    }

    if (IsUnaryOperator (CurToken(), &op, &iNewPrec))
    {
        long        iTokIdx = CurTokenIndex();
        NextToken ();
        BASENODE    *pTerm = ParseSubExpression (pParent, iNewPrec);
        if (op == OP_NEG && pTerm->kind == NK_CONSTVAL && pTerm->flags & NF_CHECK_FOR_UNARY_MINUS)
        {
            pLeftOperand = pTerm;
            pLeftOperand->other = ((pTerm->other == PT_ULONG) ? PT_LONG : PT_INT);
            pLeftOperand->tokidx = iTokIdx;
        }
        else
        {
            pLeftOperand = AllocUnaryOpNode (op, iTokIdx, pParent, pTerm);
        }
    }
    else
    {
        pLeftOperand = ParseTerm (pParent, iClose);
    }

    while ( ! PastCloseToken(iClose))
    {
        if (!IsBinaryOperator (CurToken(), &op, &iNewPrec))
            break;

        ASSERT (iNewPrec > 0);

        BOOL fDoubleOp = false;
        if (CurToken() == TID_GREATER && (PeekToken() == TID_GREATER || PeekToken() == TID_GREATEREQUAL))
        {
            POSDATA &posFirst(CurTokenPos());
            POSDATA &posNext(m_pTokens[PeekTokenIndex()]);
            if (posFirst.iLine == posNext.iLine && (posFirst.iChar + 1) == posNext.iChar)
            {
                if (PeekToken() == TID_GREATER)
                    op = OP_RSHIFT;
                else
                    op = OP_RSHIFTEQ;
                    
                iNewPrec = GetOperatorPrecedence(op);
                fDoubleOp = true;
            }
        }

        if (iNewPrec < iPrecedence)
            break;

        if ((iNewPrec == iPrecedence) && !IsRightAssociative (op))
            break;

        long    iTokOp = CurTokenIndex();
        NextToken();
        if (fDoubleOp)
            NextToken();

        if (op == OP_QUESTION)
        {
            BASENODE*   pColonLeft = ParseSubExpression (pParent, iNewPrec - 1);
            long        iTokOpColon = CurTokenIndex();
            Eat (TID_COLON);

            BASENODE* pRightOperand;
            if (PeekToken(-1) == TID_COLON)
            {
                //there was a : the right side of the ? is a binop whose lhs is 
                //the expr before the : and whose RHS is the expr after it
                BASENODE* pColonRight = ParseSubExpression (pParent, iNewPrec - 1);
                pRightOperand = AllocBinaryOpNode (OP_COLON, iTokOpColon, pParent, pColonLeft, pColonRight);
            }
            else
            {
                // this should only happen in the error case, and we should always have an error because of the EAT(TID_COLON) above.
                ASSERT(m_iErrors > 0);  

                //there wasn't any colon, the right side of the ? is just the 
                //expression we parsed after the ?
                pRightOperand = pColonLeft;
            }
            pLeftOperand = AllocBinaryOpNode (OP_QUESTION, iTokOp, pParent, pLeftOperand, pRightOperand);
        }
        else if (op == OP_IS || op == OP_AS)
        {
            pLeftOperand = AllocBinaryOpNode (op, iTokOp, pParent, pLeftOperand, ParseType(pParent, true));
        }
        else
        {
            pLeftOperand = AllocBinaryOpNode (op, iTokOp, pParent, pLeftOperand, ParseSubExpression (pParent, iNewPrec, iClose));
        }
    }

    pLeftOperand->pParent = pParent;
    return pLeftOperand;
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::ParseTerm (BASENODE *pParent, long iClose)
{
    TOKENID     iTok = CurToken();
    long        iTokIdx = CurTokenIndex();
    BASENODE    *pExpr = NULL;

    switch (iTok)
    {
        case TID_TYPEOF:
            NextToken();
            Eat(TID_OPENPAREN);

            {
                long mark = CurTokenIndex();
                int cargTot = ScanOpenType();
                TOKENID tokNext = CurToken();
                Rewind(mark);
                if (cargTot > 0 && tokNext == TID_CLOSEPAREN)
                    pExpr = ParseOpenType(pParent);
                else
                    pExpr = ParseReturnType(pParent);
            }

            Eat (TID_CLOSEPAREN);

            pExpr = AllocUnaryOpNode (OP_TYPEOF, iTokIdx, pParent, pExpr);
            break;

        case TID_DEFAULT:
        case TID_SIZEOF:
            NextToken();
            Eat(TID_OPENPAREN);

            pExpr = ParseType (pParent);

            Eat (TID_CLOSEPAREN);

            if (iTok == TID_DEFAULT)
                ReportFeatureUse(IDS_FeatureDefault);

            pExpr = AllocUnaryOpNode ((OPERATOR)m_rgTokenInfo[iTok].iSelfOp, iTokIdx, pParent, pExpr);
            break;

        // check for special operators with function-like syntax
        case TID_MAKEREFANY:
        case TID_REFTYPE:
        case TID_CHECKED:
        case TID_UNCHECKED:
            NextToken();
            Eat(TID_OPENPAREN);

            pExpr = ParseSubExpression (pParent, 0);

            Eat (TID_CLOSEPAREN);

            pExpr = AllocUnaryOpNode ((OPERATOR)m_rgTokenInfo[iTok].iSelfOp, iTokIdx, pParent, pExpr);
            break;

        case TID_REFVALUE:
        {
            NextToken();
            Eat(TID_OPENPAREN);

            pExpr = ParseSubExpression (pParent, 0);

            Eat (TID_COMMA);

            BASENODE * pType = ParseType(pParent);

            Eat (TID_CLOSEPAREN);

            pExpr = AllocBinaryOpNode (OP_REFVALUE, iTokIdx, pParent, pExpr, pType);
            break;
        }

        case TID_COLONCOLON:
            // Report the error
            Error(ERR_IdentifierExpected);
            // Eat the '::'
            NextToken();
            return ParseTerm(pParent);

        case TID_IDENTIFIER:
            // Parse in 
            //      - references to static members of instantiaions ArrayList<String>.Sort
            //      - calls to generic methods....e.g. ArrayList.Syncronized<String>(foo,bar)
            //
            // "<" takes precedence over its use as a binary operator in 
            // this circumstance.   We look to see if we can
            // successfully scan in something that looks like a type list
            // immediately after this.  This rules
            // out accidently commiting to parsing "a < b" as a type application.
            pExpr = ParseGenericQualifiedNameList (pParent, true);
            break;

        case TID_ARGS:
        case TID_BASE:
        case TID_FALSE:
        case TID_THIS:
        case TID_TRUE:
        case TID_NULL:
            pExpr = AllocOpNode ((OPERATOR)m_rgTokenInfo[iTok].iSelfOp, CurTokenIndex(), pParent);
            NextToken();
            break;

        case TID_OPENPAREN:
            pExpr = ParseCastOrExpression (pParent);
            break;

        case TID_NUMBER:
            pExpr = ParseNumber (pParent);
            break;

        case TID_STRINGLIT:
        case TID_VSLITERAL:
            pExpr = ParseStringLiteral (pParent);
            break;

        case TID_CHARLIT:
            pExpr = ParseCharLiteral (pParent);
            break;

        case TID_NEW:
            pExpr = ParseNewExpression (pParent);
            break;

        case TID_DELEGATE:
            pExpr = ParseAnonymousMethodExpr (pParent);
            break;

        default:
            // check for intrinsic type followed by '.'
            if ((m_rgTokenInfo[CurToken()].dwFlags & TFF_PREDEFINED) && (m_rgTokenInfo[CurToken()].iPredefType != PT_VOID))
            {
                TYPEBASENODE    *pType = ParsePredefinedType(pParent);

                if (CurToken() != TID_DOT)
                {
                    pExpr = AllocOpNode (OP_NOP, CurTokenIndex(), pParent);
                    Error (ERR_InvalidExprTerm, m_rgTokenInfo[iTok].pszText);
                    NextToken();
                }
                else
                {
                    pExpr = pType;
                    // fall through to post-fix check below
                }
            }
            else
            {
                //pExpr = AllocOpNode (OP_NOP, CurTokenIndex(), pParent);
                pExpr = ParseMissingName(pParent, CurTokenIndex());
                Error (ERR_InvalidExprTerm, m_rgTokenInfo[iTok].pszText);
                NextToken();
            }
            break;
    }

    return ParsePostFixOperator(pParent, pExpr, iClose);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParsePostFixOperator

BASENODE *CParser::ParsePostFixOperator (BASENODE *pParent, BASENODE *pExpr, long iClose)
{
    ASSERT(pExpr);

    // Look for post-fix operators
    while (TRUE)
    {
        TOKENID iTok = CurToken();
        long    iTokOp = CurTokenIndex();

        if (PastCloseToken(iClose))
        {
            return pExpr;
        }

        switch (iTok)
        {
            case TID_OPENPAREN:
            {
                CALLNODE    *pOp = AllocNode(NK_CALL, pParent, iTokOp)->asCALL();
                pOp->p1 = pExpr;
                long    iErrorCount = m_iErrors;
                pOp->other = OP_CALL;
                pOp->p2 = ParseArgumentList (pOp);
                pExpr->pParent = pOp;
                pOp->iClose = PeekTokenIndex(-1);    // NOTE:  -1 because ParseArgumentList advanced past it...
                if (m_pTokens[pOp->iClose].Token() != TID_CLOSEPAREN)
                    pOp->iClose = PeekTokenIndexFrom(pOp->iClose);
                if (iErrorCount != m_iErrors)
                    pOp->flags |= NF_CALL_HADERROR;         // Parameter tips key on this for window placement...
                pExpr = pOp;
                break;
            }

            case TID_OPENSQUARE:
            {
                CALLNODE    *pOp = AllocNode(NK_DEREF, pParent, iTokOp)->asDEREF();
                pOp->p1 = pExpr;
                long    iErrorCount = m_iErrors;
                pOp->p2 = ParseDimExpressionList (pOp);
                pOp->other = OP_DEREF;
                pExpr->pParent = pOp;
                pOp->iClose = PeekTokenIndex(-1);    // NOTE:  -1 because ParseDimExpressionList advanced past it...
                if (m_pTokens[pOp->iClose].Token() != TID_CLOSESQUARE)
                    pOp->iClose = PeekTokenIndexFrom(pOp->iClose);
                if (iErrorCount != m_iErrors)
                    pOp->flags |= NF_CALL_HADERROR;         // Parameter tips key on this for window placement...
                pExpr = pOp;
                break;
            }

            case TID_PLUSPLUS:
            case TID_MINUSMINUS:
                pExpr = AllocUnaryOpNode (iTok == TID_PLUSPLUS ? OP_POSTINC : OP_POSTDEC, iTokOp, pParent, pExpr);
                NextToken();
                break;

            case TID_COLONCOLON:
                CheckToken(TID_DOT);
                // Fall through.
            case TID_ARROW:
            case TID_DOT:
            {
                NextToken();
                // Parse in calls to generic methods....e.g. ArrayList.Syncronized<String>(foo,bar)
                //
                // "<" takes precedence over its use as a binary operator in 
                // this circumstance.   We look to see if we can
                // successfully scan in something that looks like a type list, and we can see a "(", ")", or "."
                // immediately after this.  This rules
                // out accidently commiting to parsing "a < b" as a type application.
                pExpr = AllocDotNode (iTokOp, pParent, pExpr, ParseGenericQualifiedNamePart (pParent, true, false));
                if (iTok == TID_ARROW)
                    pExpr->kind = NK_ARROW;
                break;
            }

            default:
                return pExpr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseCastOrExpression

BASENODE *CParser::ParseCastOrExpression (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_OPENPAREN);

    long    iMark = CurTokenIndex();

    // We have a decision to make -- is this a cast, or is it a parenthesized
    // expression?  Because look-ahead is cheap with our token stream, we check
    // to see if this "looks like" a cast (without constructing any parse trees)
    // to help us make the decision.
    if (ScanCast ())
    {
        // Looks like a cast, so parse it as one.
        Rewind (iMark);
        long    tokidx = CurTokenIndex();
        NextToken();
        TYPEBASENODE    *pType = ParseType (NULL);
        Eat (TID_CLOSEPAREN);
        return AllocBinaryOpNode (OP_CAST, tokidx, pParent, pType, ParseSubExpression(pParent, GetCastPrecedence()));
    }

    // Doesn't look like a cast, so parse this as a parenthesized expression.
    Rewind (iMark);
    long    tokidx = CurTokenIndex();
    NextToken();
    BASENODE    *pExpr = ParseSubExpression (pParent, 0);
    Eat (TID_CLOSEPAREN);
    return AllocUnaryOpNode (OP_PAREN, tokidx, pParent, pExpr);
}

BASENODE *CParser::ParseAnonymousMethodExpr (BASENODE * pParent)
{
    ANONBLOCKNODE *pAnon = AllocNode(NK_ANONBLOCK, pParent)->asANONBLOCK();

    pAnon->pArgs = NULL;
    pAnon->iClose = -1;

    ReportFeatureUse(IDS_FeatureAnonDelegates);
    Eat(TID_DELEGATE);

    if (CurToken() == TID_OPENPAREN)
    {
        long iOpen;
        ParseParameterList(pAnon, &pAnon->pArgs, &iOpen, &pAnon->iClose, kppoNoParams | kppoNoVarargs | kppoNoAttrs);
        ASSERT(iOpen == PeekTokenIndexFrom(pAnon->tokidx));
    }

    pAnon->pBody = ParseBlock (pAnon)->asBLOCK();

    return pAnon;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseNumber

BASENODE *CParser::ParseNumber (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_NUMBER);

    CONSTVALNODE    *pConst = AllocNode(NK_CONSTVAL, pParent)->asCONSTVAL();

    // Scan number into actual numeric value
    CParser::ScanNumericLiteral (CurTokenIndex(), pConst);
    NextToken();
    return pConst;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseStringLiteral

BASENODE *CParser::ParseStringLiteral (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_STRINGLIT || CurToken() == TID_VSLITERAL);

    CONSTVALNODE    *pConst = AllocNode(NK_CONSTVAL, pParent)->asCONSTVAL();

    // Scan string into STRCONST object
    pConst->other = PT_STRING;
    pConst->val.strVal = ScanStringLiteral (CurTokenIndex());
    NextToken();
    return pConst;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseCharLiteral

BASENODE *CParser::ParseCharLiteral (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_CHARLIT);

    CONSTVALNODE    *pConst = AllocNode(NK_CONSTVAL, pParent)->asCONSTVAL();

    // Scan char into actual value
    pConst->other = PT_CHAR;
    pConst->val.iVal = ScanCharLiteral (CurTokenIndex());
    NextToken();
    return pConst;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseNewExpression

BASENODE *CParser::ParseNewExpression (BASENODE *pParent)
{
    ASSERT (CurToken() == TID_NEW);

    NEWNODE *pNew = AllocNode(NK_NEW, pParent)->asNEW();

    NextToken();

    bool fHadError = false;
    pNew->pType = ParseUnderlyingType(pNew, &fHadError);
    pNew->pInit = NULL;
    pNew->pArgs = NULL;
    pNew->iOpen = pNew->iClose = -1;

    if (fHadError)
        return pNew;

    // Check for nullable
    if (CurToken() == TID_QUESTION) {
        ReportFeatureUse(IDS_FeatureNullable);
        NULLABLETYPENODE * nodeNull = AllocNode(NK_NULLABLETYPE, pNew)->asNULLABLETYPE();
        nodeNull->pElementType = pNew->pType;
        pNew->pType->pParent = nodeNull;
        pNew->pType = nodeNull;
        NextToken();
    }

    // Check for pointer types
    pNew->pType = ParsePointerTypeMods(pParent, pNew->pType);

    switch (CurToken()) {
    case TID_OPENPAREN:
        // Object-creation, which is a class/struct type followed by '('
        {
            if (pNew->pType->kind == NK_POINTERTYPE) {
                CheckToken(TID_OPENSQUARE); // if it has pointer modifiers, it can only be a new array
                pNew->flags |= NF_CALL_HADERROR;
                break;
            }

            long    iErrors = m_iErrors;

            // This is a valid object-creation-expression (assuming the arg list is okay)
            pNew->iOpen = CurTokenIndex();
            pNew->pArgs = ParseArgumentList (pNew);
            pNew->iClose = PeekTokenIndex(-1);       // -1 because ParseArgumentList skipped it
            if (m_pTokens[pNew->iClose].Token() != TID_CLOSEPAREN)
                pNew->iClose = PeekTokenIndexFrom(pNew->iClose);

            if (iErrors != m_iErrors)
                pNew->flags |= NF_CALL_HADERROR;
        }
        break;

    case TID_OPENSQUARE:
        {
            // Similar to ParseType (in the array case) we must build a tree in a top-down fashion to
            // generate the appropriate left-to-right reading of array expressions.  However, we must
            // check the first (leftmost) rank specifier for an expression list, which is allowed on
            // a 'new expression'.  So, if we don't get a ']' OR ',' parse an expression list and
            // store it in the 'new' node, otherwise we need an initializer.
            pNew->iOpen = CurTokenIndex();

            ARRAYTYPENODE * nodeTop = AllocNode(NK_ARRAYTYPE, pNew)->asARRAYTYPE();
            ARRAYTYPENODE * nodeLast = nodeTop;

            if (PeekToken() != TID_CLOSESQUARE && PeekToken() != TID_COMMA)
                pNew->pArgs = ParseDimExpressionList(pNew, &nodeTop->iDims);
            else {
                ASSERT(!pNew->pArgs);
                ParseArrayRankSpecifier(pNew, false, &nodeTop->iDims);
            }

            while (CurToken() == TID_OPENSQUARE) {
                ARRAYTYPENODE * nodeT = AllocNode(NK_ARRAYTYPE, nodeLast)->asARRAYTYPE();
                nodeLast->pElementType = nodeT;
                ParseArrayRankSpecifier(nodeT, false, &nodeT->iDims);
                ASSERT(nodeT->iDims > 0);
                nodeLast = nodeT;
            }

            pNew->iClose = PeekTokenIndex(-1);       // -1 because we skipped the ']'

            pNew->pType->pParent = nodeLast;
            nodeLast->pElementType = pNew->pType;
            pNew->pType = nodeTop;

            // Check for an initializer.
            if (CurToken() == TID_OPENCURLY)
                pNew->pInit = ParseArrayInitializer (pNew);
            else if (!pNew->pArgs) 
                Error(ERR_MissingArraySize);
        }
        break;

    default:
        Error(ERR_BadNewExpr);
        pNew->flags |= NF_CALL_HADERROR;
        // the user may have just left the '[]' off of the array initializer.
        if (CurToken() == TID_OPENCURLY)
            pNew->pInit = ParseArrayInitializer (pNew);
        break;
    }

    return pNew;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseArgument

BASENODE *CParser::ParseArgument (BASENODE *pParent)
{
    DWORD       dwFlags = 0;

    if (CurToken() == TID_REF)
    {
        dwFlags |= NF_PARMMOD_REF;
        NextToken();
    }
    else if (CurToken() == TID_OUT)
    {
        dwFlags |= NF_PARMMOD_OUT;
        NextToken();
    }

    BASENODE    *pExpr = ParseExpression (pParent);
    pExpr->flags |= dwFlags;
    return pExpr;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseArgumentList

BASENODE *CParser::ParseArgumentList (BASENODE *pParent)
{
    Eat(TID_OPENPAREN);

    BASENODE    *pArgs;
    CListMaker  list (this, &pArgs);
    long        iComma = -1;

    if (CurToken() != TID_CLOSEPAREN)
    {
        while (TRUE)
        {
            BASENODE    *pArg = ParseArgument (pParent);
            list.Add (pArg, iComma);

            if (CurToken() != TID_COMMA)
                break;

            iComma = CurTokenIndex();
            NextToken();
        }
    }

    Eat (TID_CLOSEPAREN);
    return pArgs;
}

BASENODE *CParser::ParseArrayRankSpecifier (BASENODE *pParent, bool fIsDeclaration, int *nDim)
{
    ASSERT (CurToken() == TID_OPENSQUARE);
    if (nDim)
        *nDim = 0;

    bool fError = false;

    while (TRUE)
    {
        NextToken();
        if (nDim)
            (*nDim)++;

        if (CurToken() != TID_COMMA) {
            if (CurToken() == TID_CLOSESQUARE)
                break;

            if (!fError) {
                Error(fIsDeclaration ? ERR_ArraySizeInDeclaration : ERR_InvalidArray);
                fError = true;
            }

            ParseExpression(pParent);
            if (CurToken() != TID_COMMA)
                break;
        }
    }

    // Eat the close brace and we're done.
    Eat (TID_CLOSESQUARE);
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ParseDimExpressionList

BASENODE *CParser::ParseDimExpressionList (BASENODE *pParent, int *nDim)
{
    ASSERT (CurToken() == TID_OPENSQUARE);

    if (nDim)
        *nDim = 0;
    BASENODE    *pArgs;
    CListMaker  list (this, &pArgs);
    long        iComma = -1;

    while (TRUE)
    {
        NextToken();

        BASENODE *pArg;
        if (CurToken() == TID_COMMA || CurToken() == TID_CLOSESQUARE) {
            Error(ERR_ValueExpected);
            pArg = ParseMissingName(pParent, CurTokenIndex());
        }
        else {
            pArg = ParseExpression(pParent);
        }
        list.Add(pArg, iComma);

        if (nDim)
            (*nDim)++;
        if (CurToken() != TID_COMMA)
            break;

        iComma = CurTokenIndex();
    }

    // Eat the close brace and we're done
    Eat (TID_CLOSESQUARE);
    return pArgs;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::ParseExpressionList
//
// Parse a list of expressions separated by commas, terminated by semicolon or
// close-paren (currently only called from ParseForStatement).  The terminating
// token is NOT consumed.

BASENODE *CParser::ParseExpressionList (BASENODE *pParent)
{
    BASENODE    *pArgs;
    CListMaker  list (this, &pArgs);
    long        iComma = -1;

    if (CurToken() != TID_CLOSEPAREN && CurToken() != TID_SEMICOLON)
    {
        while (TRUE)
        {
            list.Add (ParseExpression (pParent), iComma);
            if (CurToken() != TID_COMMA)
                break;
            iComma = CurTokenIndex();
            NextToken();
        }
    }

    // we're done
    return pArgs;
}


////////////////////////////////////////////////////////////////////////////////
// CParser::ParseMemberRef
//
// This list is out of date. It also parses id<typeargs> etc and (for CSEE) id.id.~id(...)
// This function will parse a member reference that looks like:
//    id
//    id(arglist)
//    id.id.id
//    id.id(arglist)
//    this
//    this[arglist]
//    id.id.this
//    id.id.this[arglist]
//    operator <op>
//    operator <op> (arglist)
//    id.id.operator <op>
//    id.id.operator <op> (arglist)
//    <implicit|explicit> operator <type>
//    <implicit|explicit> operator <type> (arglist)
//    id.id.<implicit|explicit> operator <type>
//    id.id.<implicit|explicit> operator <type> (arglist)
//    alias::id
//    alias::id(arglist)
//    alias::id.id.id
//    alias::id.id(arglist)
//    alias::id.id.this
//    alias::id.id.this[arglist]
//    alias::id.id.operator <op>
//    alias::id.id.operator <op> (arglist)
//    alias::id.id.<implicit|explicit> operator <type>
//    alias::id.id.<implicit|explicit> operator <type> (arglist)
//
//    (First id can also be a predefined type (int, string, etc.)
//
// This is used for parsing cref references in XML comments. The returned
// NK_METHOD node has only the following fields filled in:
//    pName, pParms (if parameters), pType (if conversion operator).
//
// The pName field can actually have a NK_OP node in it for an operator <op>
// or "this" (OP_THIS). This is different than how operator/indexer are usually
// represented.
//
// CSEE: in the ee this is also used to parse named breakpoint locations
// CSEE: which consist of a function reference possibly followed by a
// CSEE: comma
METHODNODE * CParser::ParseMemberRefSpecial()
{
    METHODNODE * meth = AllocNode(NK_METHOD, NULL, -1)->asMETHOD();
    meth->pAttr         = NULL;
    meth->pNext         = NULL;
    meth->iStart        = -1;
    meth->iClose        = -1;

    meth->pType         = NULL;
    meth->iOpenParen    = -1;
    meth->pParms        = NULL;
    meth->iCloseParen   = -1;
    meth->iOpen         = -1;
    meth->pBody         = NULL;
    meth->pInteriorNode = NULL;

    meth->pName         = NULL;
    meth->pConstraints  = NULL;

    ParseDottedNameSpecial(meth);

    return meth;
}


void CParser::ParseDottedNameSpecial(METHODNODE * meth)
{
    ASSERT(!meth->pName);
    ASSERT(!meth->pParms);

    long itokDot = -1;

    for (bool fFirst = true; ; fFirst = false) {
        // Get a name component: id, this, operator.
        TOKENID tokCur = CurToken();
        BASENODE * node;

        switch (tokCur) {
        default:
            if (!meth->pName && (m_rgTokenInfo[tokCur].dwFlags & TFF_PREDEFINED)) {
                // This is a predefined type.
                node = ParsePredefinedType(meth);
                break;
            }
            // Fall through.
        case TID_VOID:
            CheckToken(TID_IDENTIFIER);
            return;

        case TID_IDENTIFIER:
            node = ParseSingleNameSpecial(meth);
            if (fFirst)
                CheckForAlias(node);
            break;

        case TID_THIS:
            AddToDottedListSpecial(&meth->pName, AllocOpNode(OP_THIS, CurTokenIndex(), meth), itokDot);
            NextToken();
            ParseParamListSpecial(meth, true);
            return;


        case TID_OPERATOR:
        case TID_IMPLICIT:
        case TID_EXPLICIT:
            AddToDottedListSpecial(&meth->pName, ParseOperatorSpecial(meth), itokDot);
            // ParserOperatorSpecial parses the params and checks eof as well.
            return;
        }

        AddToDottedListSpecial(&meth->pName, node, itokDot);

        if (CurToken() != TID_DOT && CurToken() != TID_COLONCOLON) {
            ParseParamListSpecial(meth, false);
            return;
        }

        itokDot = CurTokenIndex();
        if (node->kind != NK_ALIASNAME)
            CheckToken(TID_DOT);
        NextToken();
    }
}


void CParser::AddToDottedListSpecial(BASENODE ** pnodeAll, BASENODE * node, int itokDot)
{
    ASSERT(pnodeAll);
    ASSERT(node);

    if (!*pnodeAll)
        *pnodeAll = node;
    else {
        ASSERT(itokDot >= 0);
        *pnodeAll = AllocDotNode(itokDot, node->pParent, *pnodeAll, node);
    }
}


// Parses the parameter list for XML references and EE breakpoints.
// Checks for EOF.
int CParser::ParseParamListSpecial(METHODNODE * meth, bool fThis)
{
    ASSERT(!meth->pParms);

    TOKENID tokCur = CurToken();
    TOKENID tokEnd;

    switch (tokCur) {
    default:
        CheckEofSpecial(meth, fThis);
        return 0;

    case TID_OPENPAREN:
        if (fThis) {
            CheckToken(TID_OPENSQUARE);
            return 0;
        }
        tokEnd = TID_CLOSEPAREN;
        break;

    case TID_OPENSQUARE:
        if (!fThis) {
            CheckToken(TID_OPENPAREN);
            return 0;
        }
        tokEnd = TID_CLOSESQUARE;
        break;
    }
    NextToken();

    // Distinguish empty arg list from no args.
    if (CurToken() == tokEnd) {
        NextToken();
        meth->flags |= NF_MEMBERREF_EMPTYARGS;
        CheckEofSpecial(meth, fThis);
        return 0;
    }

    CListMaker list(this, &meth->pParms);
    long itokComma = -1;
    int carg = 0;

    for (;;) {
        list.Add(ParseParameter(meth, kppoNoNames | kppoNoParams | kppoNoVarargs | kppoNoAttrs), itokComma);
        carg++;
        if (CurToken() != TID_COMMA)
            break;
        itokComma = CurTokenIndex();
        NextToken();
    }

    Eat(tokEnd);
    CheckEofSpecial(meth, fThis);

    return carg;
}


void CParser::CheckEofSpecial(METHODNODE * meth, bool fThis)
{
    switch (CurToken()) {
    default:
        if (meth->pParms || (meth->flags & NF_MEMBERREF_EMPTYARGS)) {
            // A param list was specified.
            Error(ERR_SyntaxError, L"\"");
            break;
        }
        CheckToken(fThis ? TID_OPENSQUARE : TID_OPENPAREN);
        break;


    case TID_ENDFILE:
        break;
    }
}


NAMENODE * CParser::ParseSingleNameSpecial(BASENODE * nodePar)
{
    if (!CheckToken(TID_IDENTIFIER))
        return ParseMissingName(nodePar, CurTokenIndex());

    TOKENID tokNext = PeekToken();
    if (tokNext == TID_OPENANGLE) {
        GENERICNAMENODE * gen = AllocGenericNameNode(NULL, CurTokenIndex());
        gen->pParent = nodePar;
        NextToken();

        gen->iOpen = CurTokenIndex();
        gen->pParams = ParseInstantiation(gen, false, &gen->iClose);

        return gen;
    }

    NAMENODE * name = AllocNameNode(NULL, CurTokenIndex());
    name->pParent = nodePar;
    NextToken();

    return name;
}


BASENODE * CParser::ParseOperatorSpecial(METHODNODE * meth)
{
    BASENODE * node;
    TOKENID tokOp = CurToken();

    switch (tokOp) {
    default:
        ASSERT(0);
        return NULL;

    case TID_OPERATOR:
        NextToken();

        tokOp = CurToken();
        if (tokOp == TID_IMPLICIT || tokOp == TID_EXPLICIT) {
            Error(ERR_BadOperatorSyntax, m_rgTokenInfo[tokOp].pszText);
            return NULL;
        }

        // check for >>
        if (tokOp == TID_GREATER && PeekToken() == TID_GREATER) {
            POSDATA &posFirst(CurTokenPos());
            POSDATA &posNext(m_pTokens[PeekTokenIndex()]);
            if (posFirst.iLine == posNext.iLine && (posFirst.iChar + 1) == posNext.iChar)
            {
                tokOp = TID_SHIFTRIGHT;
                NextToken();
            }
        }

        node = AllocOpNode((OPERATOR)0, CurTokenIndex(), meth); // actual operator filled in later...
        NextToken();
        break;

    case TID_IMPLICIT:
    case TID_EXPLICIT:
        NextToken();

        if (CurToken() != TID_ENDFILE)
            Eat(TID_OPERATOR);

        node = AllocOpNode((OPERATOR)0, CurTokenIndex(), meth); // actual operator filled in later...

        meth->pType = ParseType(meth);
        break;
    }

    // We can't validate the operator until we know the number of parameters.
    int carg = ParseParamListSpecial(meth, false);

    if (!carg) {
        // No parms given -- pick the right one (binary over unary if both).
        if (m_rgTokenInfo[tokOp].dwFlags & TFF_OVLBINOP)
            node->other = (OPERATOR)m_rgTokenInfo[tokOp].iBinaryOp;
        else if (m_rgTokenInfo[tokOp].dwFlags & TFF_OVLUNOP)
            node->other = (OPERATOR)m_rgTokenInfo[tokOp].iUnaryOp;
        else
            ErrorAtToken(node->tokidx, ERR_OvlOperatorExpected);
    }
    else if (carg == 1) {
        node->other = (OPERATOR)m_rgTokenInfo[tokOp].iUnaryOp;
        if (!(m_rgTokenInfo[tokOp].dwFlags & TFF_OVLUNOP))
            ErrorAtToken(node->tokidx, ERR_OvlUnaryOperatorExpected);
    }
    else {
        node->other = (OPERATOR)m_rgTokenInfo[tokOp].iBinaryOp;
        if (!(m_rgTokenInfo[tokOp].dwFlags & TFF_OVLBINOP))
            ErrorAtToken(node->tokidx, ERR_OvlBinaryOperatorExpected);
    }

    return node;
}

////////////////////////////////////////////////////////////////////////////////

long CParser::GetFirstToken (BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName)
{
    *pfMissingName = FALSE;
REDO:
    if (pNode == NULL)
    {
        VSFAIL ("NULL node given to GetFirstToken!!!");
        return 0xf0000000; 
    }

    switch (pNode->kind)
    {
        case NK_ACCESSOR:
            {
                ACCESSORNODE* pAccessorNode = pNode->asACCESSOR();
                if (pAccessorNode->pAttr != NULL)
                {
                    return GetFirstToken(pAccessorNode->pAttr, flags, pfMissingName);
                }
                else 
                {
                    return pNode->tokidx;
                }
            }
            break;

        case NK_BINOP:
            // Special case the check for a cast -- the tokidx of the OP_CAST binop
            // node has the token index of the open paren
            if (pNode->Op() == OP_CAST)
                return pNode->tokidx;
            // fall through...
        case NK_CALL:
        case NK_DEREF:
        case NK_ARROW:
        case NK_DOT:
        case NK_LIST:
            pNode = pNode->asANYBINOP()->p1;
            goto REDO;

        case NK_UNOP:
            if (pNode->Op() == OP_POSTINC || pNode->Op() == OP_POSTDEC)
                return GetFirstToken (pNode->asUNOP()->p1, flags, pfMissingName);
            return pNode->tokidx;

        case NK_PREDEFINEDTYPE:
            return pNode->tokidx;
            
        case NK_NAMEDTYPE:
            return GetFirstToken (pNode->asNAMEDTYPE()->pName, flags, pfMissingName);

        case NK_OPENTYPE:
            return GetFirstToken (pNode->asOPENTYPE()->pName, flags, pfMissingName);

        case NK_ARRAYTYPE:
            return GetFirstToken (pNode->asARRAYTYPE()->pElementType, flags, pfMissingName);

        case NK_POINTERTYPE:
            return GetFirstToken (pNode->asPOINTERTYPE()->pElementType, flags, pfMissingName);

        case NK_NULLABLETYPE:
            return GetFirstToken (pNode->asNULLABLETYPE()->pElementType, flags, pfMissingName);

        case NK_NAMESPACE:
            if (pNode->tokidx == -1)
                return 0;
            return pNode->tokidx;

        case NK_NAME:
        case NK_ALIASNAME:
            *pfMissingName = (pNode->flags & NF_NAME_MISSING);
            return pNode->tokidx;

        case NK_NESTEDTYPE:
            return GetFirstToken (pNode->asNESTEDTYPE()->pType, flags, pfMissingName);

        case NK_DO:
            if (flags & EF_SINGLESTMT)
            {
                BOOL fMissing = FALSE;
                return PeekTokenIndexFromEx ( GetFirstToken ( pNode->asDO()->pExpr, flags, &fMissing), flags, -2); // Include 'while ('
            }
            else
                goto DEFAULT;

        case NK_DECLSTMT:
            if (pNode->flags & NF_CONST_DECL)
            {
                return PeekTokenIndexFromEx (pNode->tokidx, flags, -1);
            }
            else
                goto DEFAULT;

        case NK_OP:
            //The parser wasn't able to read in anything successfully.
            //So it will have read off past the current token to the next
            //
            //Like for (int ^
            //We want to be considered in the "init" part of the for
            if (pNode->other == OP_NOP && pNode->tokidx > 0)
            {
                return PeekTokenIndexFromEx (pNode->tokidx, flags, -1);
            }
            else
                goto DEFAULT;

        case NK_CONSTRAINT:
            {
                //need to consume the "where" token
                long prevToken = PeekTokenIndexFromEx (pNode->tokidx, flags, -1);

                if (prevToken < 0 || prevToken >= m_iTokens)
                {
                    // This should only happen if we ignore the token stream (i.e. if the caller
                    // knows the parse tree might be outdated).
                    VSASSERT((flags & EF_IGNORE_TOKEN_STREAM) == EF_IGNORE_TOKEN_STREAM, 
                        "How come we don't have a valid token if we're not ignoring the token stream?");
                    goto DEFAULT;
                }
                else
                {
                    // Ok, the prev index is valid. Check we have one the "where" token
                    if (CheckForName(prevToken, SpecName(SN_WHERE))) {
                        return prevToken;
                    } else {
                        goto DEFAULT;
                    }
                }
            }

        case NK_ATTRARG:
            {
                ATTRNODE* pAttrNode = pNode->asANYATTR();
                if (pAttrNode->pName != NULL) {
                    return GetFirstToken(pAttrNode->pName, flags, pfMissingName);
                } else if (pAttrNode->pArgs != NULL) {
                    return GetFirstToken(pAttrNode->pArgs, flags, pfMissingName);
                } else {
                    goto DEFAULT;
                }
            }

        case NK_ATTR:
            {
                ATTRNODE* pAttrNode = pNode->asATTR();
                if (pAttrNode->pName != NULL) {
                    return GetFirstToken(pAttrNode->pName, flags, pfMissingName);
                } else {
                    goto DEFAULT;
                }
            }

        case NK_PARAMETER:
            {
                PARAMETERNODE* pParameter = pNode->asPARAMETER();

                BASENODE *pParentMethod = pParameter->pParent;
                while (pParentMethod != NULL && pParentMethod->kind == NK_LIST)
                    pParentMethod = pParentMethod->pParent;

                const bool fIsLastParam = 
                    (pParameter->pParent->kind != NK_LIST) ||
                    ((pParameter->pParent->kind == NK_LIST) && (pParameter->pParent->asLIST()->p2 == pParameter));

                if (pParameter->flags & NF_PARMMOD_REF ||
                    pParameter->flags & NF_PARMMOD_OUT || 
                    (fIsLastParam && (pParentMethod != NULL) && (pParentMethod->other & NFEX_METHOD_PARAMS))) 
                {
                    //we had an out/ref/params flag.  Stick to the default
                    goto DEFAULT;
                } else if (pParameter->pAttr != NULL) {
                    return GetFirstToken(pParameter->pAttr, flags, pfMissingName);
                } else {
                    return GetFirstToken(pParameter->pType, flags, pfMissingName);
                }
            }

        default:
DEFAULT:
            // MOST nodes' token index values are the first for their construct
            ASSERT (pNode->tokidx != -1);       // If you hit this, you'll either need to fix the parser or special-case this node type!
            return pNode->tokidx;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CParser::IsUnaryOperator

inline bool CParser::IsUnaryOperator (TOKENID iTok, OPERATOR *piOp, UINT *piPrecedence)
{
    if (!IsUnaryOperator(iTok))
        return false;

    *piOp = (OPERATOR)m_rgTokenInfo[iTok].iUnaryOp;
    *piPrecedence = GetOperatorPrecedence(*piOp);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IsUnaryOperator

inline bool CParser::IsUnaryOperator (TOKENID iTok)
{
    return (m_rgTokenInfo[iTok].iUnaryOp != OP_NONE && (m_rgTokenInfo[iTok].dwFlags & TFF_OVLOPKWD) == 0);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IsBinaryOperator

inline BOOL CParser::IsBinaryOperator (TOKENID iTok, OPERATOR *piOp, UINT *piPrecedence)
{
    if (m_rgTokenInfo[iTok].iBinaryOp != OP_NONE && (m_rgTokenInfo[iTok].dwFlags & TFF_OVLOPKWD) == 0)
    {
        if (piOp != NULL)
        {
            *piOp = (OPERATOR)m_rgTokenInfo[iTok].iBinaryOp;
        }
        if (piPrecedence != NULL)
        {
            *piPrecedence = GetOperatorPrecedence(*piOp);
        }
        return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeNextToken

inline
long CParser::IncludeNextToken(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName, TOKENID iTok1)
{
    return IncludeOneOfNextToken (iTokIdx, flags, pfMissingName, iTok1, (TOKENID)-1);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeSemiColon

inline
long CParser::IncludeSemiColon(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_SEMICOLON);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeColon

inline
long CParser::IncludeColon(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_COLON);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeCloseParen

inline
long CParser::IncludeCloseParen(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_CLOSEPAREN);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeCloseSquare

inline
long CParser::IncludeCloseSquare(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_CLOSESQUARE);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeCloseCurly

inline
long CParser::IncludeCloseCurly(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_CLOSECURLY);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeComma

inline
long CParser::IncludeComma(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeNextToken (iTokIdx, flags, pfMissingName, TID_COMMA);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeCloseParenOrCloseSquare

inline
long CParser::IncludeCloseParenOrCloseSquare(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeOneOfNextToken (iTokIdx, flags, pfMissingName, TID_CLOSEPAREN, TID_CLOSESQUARE);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeSemiColonOrComma

inline
long CParser::IncludeSemiColonOrComma(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeOneOfNextToken (iTokIdx, flags, pfMissingName, TID_SEMICOLON, TID_COMMA);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeQuestion

inline
long CParser::IncludeQuestion(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName)
{
    return IncludeOneOfNextToken (iTokIdx, flags, pfMissingName, TID_QUESTION, (TOKENID)-1);
}

////////////////////////////////////////////////////////////////////////////////
// CParser::IncludeOneOfNextToken
//
//  Skip noisy tokens from "iTokIdx" up to a token "iTok1" or "iTok2".
//  Return the token index of "iTok1" or "iTok2", or return "iTokIdx" is the 
//  token found doesn't match.

long CParser::IncludeOneOfNextToken(long iTokIdx, ExtentFlags flags, BOOL* pfMissingName, TOKENID iTok1, TOKENID iTok2)
{
    // This assert is not quite right if we're called from the FindLeaf call
    // in ParseTopLevel method, because the parse tree may not be up to date
    // with the token stream. In that case, we will just return a bogus token
    // index (but within the range of the token stream array)
    //ASSERT(iTokIdx >= 0);
    //ASSERT(iTokIdx <= m_iTokens);

    if (flags & EF_IGNORE_TOKEN_STREAM)
    {
        //if we're going to read a token ahead no matter what, then we have to assume that we're
        //no longer at a missing name
        *pfMissingName = FALSE;
        return min(m_iTokens - 1, iTokIdx + 1);
    }
    else
    {
        long iNextTokenIdx = PeekTokenIndexFrom (iTokIdx, 1);
        ASSERT(iNextTokenIdx >= 0);
        ASSERT(iNextTokenIdx < m_iTokens);

        // Ok, the token index is valid. Check we have one of the "expected" tokens
        TOKENID iNextTokId = m_pTokens[iNextTokenIdx].Token();
        if (iNextTokId == iTok1 || iNextTokId == iTok2)
        {
            //we read ahead one token, we're no longer at a missing name
            *pfMissingName = FALSE;
            return iNextTokenIdx;
        }

        return iTokIdx;
    }
}

////////////////////////////////////////////////////////////////////////////////

long CParser::GetLastToken (CBasenodeLookupCache& lastTokenCache, BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName)
{
    {
        return GetLastTokenWorker (lastTokenCache, pNode, flags, pfMissingName);
    }
}

////////////////////////////////////////////////////////////////////////////////

long CParser::GetLastTokenWorker (CBasenodeLookupCache& lastTokenCache, BASENODE *pNode, ExtentFlags flags, BOOL *pfMissingName)
{
    *pfMissingName = FALSE;

REDO:
    if (pNode == NULL)
    {
        VSFAIL ("NULL node given to GetLastToken!!!");
        return 0xf0000000;
    }

    switch (pNode->kind)
    {
        case NK_ACCESSOR:
            if (flags & EF_SINGLESTMT)
                return pNode->asACCESSOR()->iOpen;
            return pNode->asACCESSOR()->iClose;

        case NK_ARRAYINIT:
            // Add one for the close curly
            if (pNode->asARRAYINIT()->p1 == NULL)
                return IncludeCloseCurly (pNode->tokidx, flags, pfMissingName);
            else 
                return IncludeCloseCurly (IncludeComma (GetLastToken(lastTokenCache, pNode->asARRAYINIT()->p1, flags, pfMissingName), flags, pfMissingName), flags, pfMissingName);

        case NK_CALL:
        case NK_DEREF:
            if (pNode->asANYCALL()->p2 == NULL)
                return IncludeCloseParenOrCloseSquare (pNode->tokidx, flags, pfMissingName);       // Get to the ) or ] in empty arg list
            return IncludeCloseParenOrCloseSquare (GetLastToken (lastTokenCache, pNode->asANYCALL()->p2, flags, pfMissingName), flags, pfMissingName); // Gets the ) or ]

        case NK_BINOP:
            if (flags & EF_POSSIBLE_GENERIC_NAME) {
                return max(GetLastToken(lastTokenCache, pNode->asANYBINOP()->p1, flags, pfMissingName), GetLastToken(lastTokenCache, pNode->asANYBINOP()->p2, flags, pfMissingName));
            }
            //fallthrough

        case NK_ARROW:
        case NK_DOT:
        case NK_LIST:
            pNode = pNode->asANYBINOP()->p2;
            goto REDO;

        case NK_ATTR:
            if (pNode->asATTR()->iClose != -1)
                return pNode->asATTR()->iClose;
            // fallthrough ...

        case NK_ATTRARG:
            if (pNode->asANYATTR()->pArgs != NULL)
                pNode = pNode->asANYATTR()->pArgs;
            else
                pNode = pNode->asANYATTR()->pName;
            goto REDO;

        case NK_ATTRDECL:
            return pNode->asATTRDECL()->iClose;

        case NK_BLOCK:
            return pNode->asBLOCK()->iClose;

        case NK_ANONBLOCK:
            if (flags & EF_SINGLESTMT)
            {
                // For debug info we only want everything up to the open-curly
                // which is either the first token, or iClose

                // -1 means no parens
                if (pNode->asANONBLOCK()->iClose == -1)
                    return pNode->tokidx;
                return pNode->asANONBLOCK()->iClose;
            }

            return GetLastToken(lastTokenCache, pNode->asANONBLOCK()->pBody, flags, pfMissingName);

        case NK_GOTO:
            if (pNode->asGOTO()->pArg == NULL && pNode->asGOTO()->flags & NF_GOTO_CASE)
                return IncludeSemiColon (IncludeNextToken(pNode->tokidx, flags, pfMissingName, TID_DEFAULT), flags, pfMissingName);       // Includes the semicolon...
            // fallthrough

        case NK_BREAK:
        case NK_CONTINUE:
        case NK_EXPRSTMT:
        case NK_THROW:
        case NK_RETURN:
            if (pNode->asANYEXPRSTMT()->pArg != NULL)
                return IncludeSemiColon (GetLastToken (lastTokenCache, pNode->asANYEXPRSTMT()->pArg, flags, pfMissingName), flags, pfMissingName);       // Includes semicolon...
            return IncludeSemiColon (pNode->tokidx, flags, pfMissingName);       // Includes the semicolon...

        case NK_YIELD:
            if (pNode->asYIELD()->pArg != NULL)
                return IncludeSemiColon (GetLastToken (lastTokenCache, pNode->asYIELD()->pArg, flags, pfMissingName), flags, pfMissingName);       // Includes semicolon...
            // might be yield; yield return; or yield break;
            return IncludeSemiColon (IncludeOneOfNextToken(pNode->tokidx, flags, pfMissingName, TID_BREAK, TID_RETURN), flags, pfMissingName);       // Includes the semicolon...

        case NK_CASE:
            if (pNode->asCASE()->pStmts != NULL)
                return GetLastToken (lastTokenCache, GetLastStatement (pNode->asCASE()->pStmts), flags, pfMissingName);
            
            return GetLastToken (lastTokenCache, pNode->asCASE()->pLabels, flags, pfMissingName);

        case NK_CASELABEL:
            if (pNode->asCASELABEL()->p1 != NULL)
                return IncludeColon (GetLastToken (lastTokenCache, pNode->asCASELABEL()->p1, flags, pfMissingName), flags, pfMissingName);  // Includes the colon...
            return IncludeColon (pNode->tokidx, flags, pfMissingName);                                           // Includes the colon...

        case NK_CATCH:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asCATCH()->pName != NULL)
                    return IncludeCloseParen( GetLastToken(lastTokenCache, pNode->asCATCH()->pName, flags, pfMissingName), flags, pfMissingName); // Includes the ')'
                if (pNode->asCATCH()->pType != NULL)
                    return IncludeCloseParen ( GetLastToken(lastTokenCache, pNode->asCATCH()->pType, flags, pfMissingName), flags, pfMissingName); // Includes the ')'
            }
            else
            {
                if (pNode->asCATCH()->pBlock != NULL)
                    return pNode->asCATCH()->pBlock->iClose;
            }
            return pNode->tokidx; // Nothing to include

        case NK_CHECKED:
        case NK_LABEL:
        case NK_UNSAFE:
            return GetLastToken (lastTokenCache, pNode->asANYLABELSTMT()->pStmt, flags, pfMissingName);

        case NK_CLASS:
        case NK_ENUM:
        case NK_INTERFACE:
        case NK_STRUCT:
            if (flags & EF_SINGLESTMT)
                return PeekTokenIndexFromEx ( pNode->asAGGREGATE()->iOpen, flags, -1); // Don't include the open curly

            //return pNode->asAGGREGATE()->iClose;
            //all aggregates can be followed by an optional semi-colon.
            return IncludeSemiColon (pNode->asAGGREGATE()->iClose, flags, pfMissingName);


        case NK_FIELD:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asFIELD()->pDecls->kind == NK_LIST)
                    return GetLastToken (lastTokenCache, (pNode->asFIELD()->pDecls->asANYBINOP())->p1, flags, pfMissingName);   // Semicolon included by the NK_VARDECL
                return GetLastToken (lastTokenCache, pNode->asFIELD()->pDecls, flags, pfMissingName);     // Semicolon included by the NK_VARDECL
            }
            // fallthrough

        case NK_CTOR:
        case NK_DTOR:
        case NK_METHOD:
        case NK_OPERATOR:
            if (flags & EF_SINGLESTMT)
                return PeekTokenIndexFromEx ( pNode->asANYMETHOD()->iOpen, flags, -1); // Don't Include the open curly
            goto ANYMEMBER;

        case NK_NESTEDTYPE:
            if (flags & EF_SINGLESTMT)
                return GetLastToken(lastTokenCache, pNode->asNESTEDTYPE()->pType, flags, pfMissingName);
            goto ANYMEMBER;

        case NK_INDEXER:
        case NK_PROPERTY:
        case NK_PARTIALMEMBER:
        case NK_ENUMMBR:
        case NK_CONST:
ANYMEMBER:
            return pNode->asANYMEMBER()->iClose;

        case NK_CONSTVAL:
            // Must special case check for the collapsed unary minus expression, which is two tokens instead of one.
            if ((pNode->flags & NF_CHECK_FOR_UNARY_MINUS) && (pNode->other == PT_LONG || pNode->other == PT_INT))
                return PeekTokenIndexFromEx ( pNode->tokidx, flags, 1);
            return pNode->tokidx;

        case NK_EMPTYSTMT:
        case NK_OP:
            return pNode->tokidx;

        case NK_DECLSTMT:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asDECLSTMT()->pVars->kind == NK_LIST)
                    return GetLastToken (lastTokenCache, (pNode->asDECLSTMT()->pVars->asANYBINOP())->p1, flags, pfMissingName);   // Semicolon included by the NK_VARDECL
                return GetLastToken (lastTokenCache, pNode->asDECLSTMT()->pVars, flags, pfMissingName);     // Semicolon included by the NK_VARDECL
            }
            else {
                long tokIdx = GetLastToken (lastTokenCache, pNode->asDECLSTMT()->pVars, flags, pfMissingName);
                if (pNode->pParent && pNode->pParent->kind == NK_FOR && (pNode->pParent->flags & (NF_FIXED_DECL | NF_USING_DECL | NF_FOR_FOREACH))) 
                    return tokIdx;
                return IncludeSemiColon (tokIdx, flags, pfMissingName);     // Include the semicolon
            }

        case NK_DELEGATE:
            return pNode->asDELEGATE()->iSemi;

        case NK_DO:
            return IncludeSemiColon (IncludeCloseParen (GetLastToken (lastTokenCache, pNode->asDO()->pExpr, flags, pfMissingName), flags, pfMissingName), flags, pfMissingName);     // Includes close paren and semicolon

        case NK_FOR:
            if (flags & EF_SINGLESTMT)
            {
                return pNode->tokidx; // This is just a foreach
            }
            return GetLastToken (lastTokenCache, pNode->asFOR()->pStmt, flags, pfMissingName);

        case NK_IF:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asIF()->pExpr != NULL)
                    return IncludeCloseParen (GetLastToken (lastTokenCache, pNode->asIF()->pExpr, flags, pfMissingName), flags, pfMissingName);   // Includes close paren
                return pNode->tokidx;
            }
            else
            {
                if (pNode->asIF()->pElse != NULL)
                    return GetLastToken (lastTokenCache, pNode->asIF()->pElse, flags, pfMissingName);
                return GetLastToken (lastTokenCache, pNode->asIF()->pStmt, flags, pfMissingName);
            }

        case NK_LOCK:
        case NK_WHILE:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asANYLOOPSTMT()->pExpr != NULL)
                    return IncludeCloseParen (GetLastToken (lastTokenCache, pNode->asANYLOOPSTMT()->pExpr, flags, pfMissingName), flags, pfMissingName); // Includes close paren
                return pNode->tokidx;
            }
            else
                return GetLastToken (lastTokenCache, pNode->asANYLOOPSTMT()->pStmt, flags, pfMissingName);

        case NK_NAME:
            if (flags & EF_POSSIBLE_GENERIC_NAME && pNode->asNAME()->pPossibleGenericName != NULL) {
                return GetLastToken(lastTokenCache, pNode->asNAME()->pPossibleGenericName, flags, pfMissingName);
            } 
            //fallthrough

        case NK_ALIASNAME:
            *pfMissingName = (pNode->flags & NF_NAME_MISSING);
            return pNode->tokidx;

        case NK_GENERICNAME:
            {
                GENERICNAMENODE* pGenericNameNode = pNode->asGENERICNAME();
                if ((pGenericNameNode->iClose > (pGenericNameNode->iOpen + 1)) ||
                    pGenericNameNode->pParams == NULL)
                {
                    return pGenericNameNode->iClose;
                }                
                else
                {
                    //we don't have a valid close position.
                    //return the end position of the parameters
                    return GetLastToken(lastTokenCache, pGenericNameNode->pParams, flags, pfMissingName);
                }
            }
            break;
           
        case NK_OPENNAME:
            {
                OPENNAMENODE* nodeOpen = pNode->asOPENNAME();
                ASSERT(nodeOpen->iClose > nodeOpen->iOpen);
                return nodeOpen->iClose;
            }
            break;

        case NK_TYPEWITHATTR:
            return GetLastToken(lastTokenCache, pNode->asTYPEWITHATTR()->pType, flags, pfMissingName);

        case NK_CONSTRAINT:
            if (pNode->asCONSTRAINT()->iEnd >= 0)
                return pNode->asCONSTRAINT()->iEnd;
            if (!pNode->asCONSTRAINT()->pBounds)
                return GetLastToken(lastTokenCache, pNode->asCONSTRAINT()->pName, flags, pfMissingName);
            return GetLastToken(lastTokenCache, pNode->asCONSTRAINT()->pBounds, flags, pfMissingName);

        case NK_NAMESPACE:
            if (flags & EF_SINGLESTMT)
                return pNode->asNAMESPACE()->iOpen;
            return pNode->asNAMESPACE()->iClose;

        case NK_NEW:
            {
                NEWNODE *pNew = pNode->asNEW();

                if (pNew->pInit != NULL)
                    return GetLastToken (lastTokenCache, pNew->pInit, flags, pfMissingName);
                if (pNew->iClose != -1)
                    return pNew->iClose;

                // Special case for stack alloc: The pType member is a NK_POINTERTYPE, although
                // the parser parsed a NK_ARRAYTYPE.
                // For "normal" new, only check the type is an array
                const bool fIsArrayType =
                    (pNew->pType != NULL) && 
                    ((pNew->pType->kind == NK_ARRAYTYPE) ||
                     (pNew->pType->kind == NK_POINTERTYPE && pNew->flags == NF_NEW_STACKALLOC));

                if (fIsArrayType && pNew->pArgs != NULL)
                    return IncludeCloseSquare (GetLastToken (lastTokenCache, pNew->pArgs, flags, pfMissingName), flags, pfMissingName);

                return GetLastToken (lastTokenCache, pNew->pType, flags, pfMissingName);
            }

        case NK_PARAMETER:
            return GetLastToken (lastTokenCache, pNode->asPARAMETER()->pName, flags, pfMissingName);

        case NK_TRY:
            if (!(flags & EF_SINGLESTMT))
            {
                if (pNode->asTRY()->pCatch != NULL)
                    return GetLastToken (lastTokenCache, pNode->asTRY()->pCatch, flags, pfMissingName);
                if (pNode->asTRY()->pBlock != NULL)
                    return GetLastToken (lastTokenCache, pNode->asTRY()->pBlock, flags, pfMissingName);
            }
            return pNode->tokidx;

        case NK_PREDEFINEDTYPE:
            return pNode->tokidx;

        case NK_POINTERTYPE:
            return pNode->tokidx;

        case NK_NULLABLETYPE:
            return pNode->tokidx;

        case NK_NAMEDTYPE:
            return GetLastToken (lastTokenCache, pNode->asNAMEDTYPE()->pName, flags, pfMissingName);

        case NK_OPENTYPE:
            return GetLastToken(lastTokenCache, pNode->asOPENTYPE()->pName, flags, pfMissingName);

        case NK_ARRAYTYPE:

            BASENODE *pArrayType, *pNextType;
            pArrayType = pNextType = pNode;
            while (pNextType->kind == NK_ARRAYTYPE)
            {
                pArrayType = pNextType;
                pNextType = pNextType->asARRAYTYPE()->pElementType;
            }
            // For array types, the token index is the open '['.  Add the number of
            // dimensions and you land on the ']' (1==[], 2==[,], 3==[,,], etc).
            if (pArrayType->asARRAYTYPE()->iDims == -1)
                return IncludeCloseSquare (IncludeQuestion (pNode->tokidx, flags, pfMissingName), flags, pfMissingName);               // unknown rank is [?]
            return PeekTokenIndexFromEx ( pArrayType->tokidx, flags, pArrayType->asARRAYTYPE()->iDims);

        case NK_SWITCH:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->asSWITCH()->pExpr != NULL)
                    return IncludeCloseParen (GetLastToken (lastTokenCache, pNode->asSWITCH()->pExpr, flags, pfMissingName), flags, pfMissingName); // Includes close paren
            }
            return pNode->asSWITCH()->iClose;

        case NK_UNOP:
            switch (pNode->Op()) {
            case OP_POSTINC:
            case OP_POSTDEC:
                return pNode->tokidx;
            case OP_DEFAULT:
            case OP_PAREN:
            case OP_UNCHECKED:
            case OP_CHECKED:
            case OP_TYPEOF:
            case OP_SIZEOF:
                return IncludeCloseParen (GetLastToken (lastTokenCache, pNode->asUNOP()->p1, flags, pfMissingName), flags, pfMissingName);      // Get the close paren
            default:
                return GetLastToken (lastTokenCache, pNode->asUNOP()->p1, flags, pfMissingName);
            }
            break;

        case NK_USING:
            if (pNode->asUSING()->pName != NULL)
            {
                return IncludeSemiColon (GetLastToken (lastTokenCache, pNode->asUSING()->pName, flags, pfMissingName), flags, pfMissingName); 
            }
            else
            {
                return IncludeSemiColon (GetLastToken (lastTokenCache, pNode->asUSING()->pAlias, flags, pfMissingName), flags, pfMissingName);      // Includes semicolon
            }
            

        case NK_VARDECL:
            if (flags & EF_SINGLESTMT)
            {
                if (pNode->flags & NF_VARDECL_ARRAY)
                    return IncludeSemiColonOrComma ( IncludeCloseSquare (GetLastToken (lastTokenCache, pNode->asVARDECL()->pArg, flags, pfMissingName), flags, pfMissingName), flags, pfMissingName);     // Includes ']' and semicolon or comma
                if (pNode->flags & NF_VARDECL_EXPR)
                    return IncludeSemiColonOrComma ( GetLastToken (lastTokenCache, pNode->asVARDECL()->pArg, flags, pfMissingName), flags, pfMissingName);     // Includes semicolon or comma
                return IncludeSemiColonOrComma ( GetLastToken (lastTokenCache, pNode->asVARDECL()->pName, flags, pfMissingName), flags, pfMissingName);     // Includes semicolon or comma
            }
            else
            {
                if (pNode->flags & NF_VARDECL_ARRAY)
                    return IncludeCloseSquare ( GetLastToken (lastTokenCache, pNode->asVARDECL()->pArg, flags, pfMissingName), flags, pfMissingName);     // Includes ']'
                if (pNode->flags & NF_VARDECL_EXPR)
                    return GetLastToken (lastTokenCache, pNode->asVARDECL()->pArg, flags, pfMissingName);
                return GetLastToken (lastTokenCache, pNode->asVARDECL()->pName, flags, pfMissingName);
            }

        default:
            // RARELY are nodes singularly represented (only names, constants, or other terms).
            // Represent them all explicitly!
            VSFAIL ("Unhandled node type in GetLastToken!");
            return pNode->tokidx;
    }
}

////////////////////////////////////////////////////////////////////////////////

STATEMENTNODE *CParser::GetLastStatement (STATEMENTNODE *pNode)
{
    while (pNode != NULL && pNode->pNext != NULL)
        pNode = pNode->pNext;

    return pNode;
}

////////////////////////////////////////////////////////////////////////////////

class CContainingNodeFinder : 
    public IComparer<POSDATA,BASENODE*>
{
private:
    //immutable state
    CParser* m_pParser;
    CBasenodeLookupCache& m_lastTokenCache;
    ExtentFlags m_flags;
    POSDATA m_pos;
    CAtlArray<BASENODE*>& m_array;

public:
    CContainingNodeFinder(CParser* pParser,
                          CBasenodeLookupCache& lastTokenCache, 
                          POSDATA pos,
                          CAtlArray<BASENODE*>& array,
                          ExtentFlags flags) :
        m_pParser(pParser),
        m_lastTokenCache(lastTokenCache),
        m_flags(flags),
        m_pos(pos),
        m_array(array)
    {
    }

    BASENODE* Do()
    {
        if (m_array.IsEmpty()) {
            return NULL;
        }

        long bestIndex = AtlArrayStableBinarySearch(m_array, m_pos, this);
        const long lastArrayElement = (long)m_array.GetCount() - 1;

        //If the element is not in the array, and would be inserted at the 
        //end of the array.  Then just return.  We didn't find it.
        if (bestIndex > lastArrayElement) {
            return NULL;
        }

        //CHeck that we actually found a valid element (as opposed ot 
        //just a position where the element would have gone).
        BASENODE* pBestGuessNode = m_array[bestIndex];
        long bestNodeCompare = this->Compare(m_pos, pBestGuessNode);
        if (bestNodeCompare != 0) {
            return NULL;
        }

        //If we're the last element of the array, then there's nothing more to look at
        //we're good to go.
        if (bestIndex == lastArrayElement) {
            return pBestGuessNode;
        }

        //if the caller has told us not to look at the right side of the cursor
        //then just return the best node we've found so far.
        if (m_flags & EF_PREFER_LEFT_NODE) {
            return pBestGuessNode;
        }

        //Ok, the cursor could be right at the end of the member or statement.  It 
        //could be code like:
        //
        //s DoFoo() {
        //}s DoBoo() {}
        //
        //In which case we need to check the next member
        POSDATA n1PosStart, n1PosEnd, n2PosStart, n2PosEnd;
        BASENODE* pNextBestGuessNode = m_array[bestIndex + 1];
        if (FAILED(m_pParser->GetExtent (m_lastTokenCache, pBestGuessNode    , m_flags, &n1PosStart, &n1PosEnd)) ||
            FAILED(m_pParser->GetExtent (m_lastTokenCache, pNextBestGuessNode, m_flags, &n2PosStart, &n2PosEnd)))
        {
            return pBestGuessNode;
        }

        if (m_pos == n1PosEnd && m_pos == n2PosStart) {
            return pNextBestGuessNode;
        }

        return pBestGuessNode;
    }

    long Compare(const POSDATA& pos, BASENODE* const& cpNode)
    {
        POSDATA posStart, posEnd;
        BASENODE* pNode = cpNode;
        m_pParser->GetExtent (m_lastTokenCache, pNode, m_flags, &posStart, &posEnd);

        if (pos < posStart) {
            return -1;
        } else if (pos > posEnd) {
            return 1;
        } else {
            return 0;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::FindContainingNodeInChain(CBasenodeLookupCache& lastTokenCache, POSDATA pos, BASENODE *pChain, ExtentFlags flags)
{
    //It's prettyu costly to keep adding individual elements to an atlarray
    //so we determine the total number of elements, and preset the size of the
    //array accordingly.
    size_t nodeCount = 0;
    CChainIterator iterator(pChain);
    while (iterator.MoveNext()) {
        nodeCount++;
    }

    CAtlArray<BASENODE*> nodes;
    nodes.SetCount(nodeCount);
    iterator.Reset();
    size_t i = 0;
    while (iterator.MoveNext()) {
        nodes[i++] = iterator.Current();
    }

    return CContainingNodeFinder(this, lastTokenCache, pos, nodes, flags).Do();
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::FindLeaf (const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ICSInteriorTree **ppTree)
{
    CBasenodeLookupCache cache;
    return FindLeafEx(cache, pos, pNode, pData, EF_FULL, ppTree);
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::FindLeafIgnoringTokenStream(const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ICSInteriorTree **ppTree)
{
    CBasenodeLookupCache lastTokenCache;
    return FindLeafEx(lastTokenCache, pos, pNode, pData, EF_IGNORE_TOKEN_STREAM, ppTree);
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CParser::FindLeafEx (CBasenodeLookupCache& lastTokenCache, const POSDATA &pos, BASENODE *pNode, CSourceData *pData, ExtentFlags flags, ICSInteriorTree **ppTree)
{
    // Check here rather than at every call site...
    if (pNode == NULL)
        return NULL;

    BASENODE    *pLeaf = NULL;    
    BASENODE    *pPossibleLeaf = NULL;

    POSDATA     posStart, posEnd;
    unsigned    iLoopCount = 0;

    // If this is an intrinsically chained node, run the chain until we
    // find one that "contains" the given position
    if (pNode->IsStatement () || pNode->InGroup (NG_MEMBER))
    {
        // Search this chain (might just be pNode)
        pNode = FindContainingNodeInChain(lastTokenCache, pos, pNode, flags);
    }

    if (pNode != NULL)
    {
        POSDATA posEOF = (m_iTokens >= 1) ? m_pTokens[m_iTokens - 1] : POSDATA(0,0);

        // If this node doesn't "contain" this position, we can bail here.
        GetExtent (lastTokenCache, pNode, flags, &posStart, &posEnd);

        // Fix up posStart and posEnd if GetExtent failed for some reason
        // See the comment in srcmod.cpp:
        if (posStart.IsUninitialized())
            posStart = POSDATA(0,0);

        if (posEnd.IsUninitialized())
            posEnd = posEOF;

        const bool fAtEOF = (posStart == posEOF) && (posEnd == posEOF);
        if (fAtEOF || pos < posStart || pos > posEnd)
            pNode = NULL;
    }

    if (pNode == NULL)
        return NULL;

    if (posStart > posEnd)
    {
        VSFAIL("Invalid extent found!");
        return NULL;
    }


REDO:
    switch (pNode->kind)
    {
    case NK_NAMESPACE:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNAMESPACE()->pName, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNAMESPACE()->pUsing, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNAMESPACE()->pGlobalAttr, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNAMESPACE()->pElements, pData, flags, ppTree);
        break;

    case NK_USING:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asUSING()->pName, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asUSING()->pAlias, pData, flags, ppTree);
        break;

    case NK_CLASS:
    case NK_ENUM:
    case NK_INTERFACE:
    case NK_STRUCT:
    {
        AGGREGATENODE   *pClass = pNode->asAGGREGATE();

        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pClass->pTypeParams, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pClass->pConstraints, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pClass->pBases, pData, flags, ppTree)) && 
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pClass->pMembers, pData, flags, ppTree)))
        {
            pPossibleLeaf = FindLeafEx (lastTokenCache, pos, pClass->pAttr, pData, flags, ppTree);
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pClass->pName, pData, flags, ppTree) )
                || (pLeaf->flags & NF_NAME_MISSING))
            {
                if (pPossibleLeaf) {
                    pLeaf = pPossibleLeaf;
                }                
            }
        }
            
        break;
    }

    case NK_DELEGATE:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pType, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pName, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pTypeParams, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pParms, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pConstraints, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDELEGATE()->pAttr, pData, flags, ppTree);
        break;

    case NK_MEMBER:
        ASSERT (FALSE);
        return NULL;

    case NK_ENUMMBR:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asENUMMBR()->pValue, pData, flags, ppTree))&&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asENUMMBR()->pName, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asENUMMBR()->pAttr, pData, flags, ppTree);
        break;

    case NK_CONST:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCONST()->pDecls, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCONST()->pType, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCONST()->pAttr, pData, flags, ppTree);
        break;

    case NK_FIELD:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFIELD()->pDecls, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFIELD()->pType, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFIELD()->pAttr, pData, flags, ppTree);
        break;

    case NK_METHOD:
    case NK_OPERATOR:
    case NK_CTOR:
    case NK_DTOR:
    {
        METHODBASENODE  *pMethod = pNode->asANYMETHOD();

        if ((pMethod->other & NFEX_METHOD_NOBODY) == 0 && pos >= m_pTokens[pMethod->iOpen])
        {
            if (pData == NULL)
                return pMethod;

            ASSERT (*ppTree == NULL);

            GetInteriorTree (pData, pMethod, ppTree);
            pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pBody, pData, flags, ppTree);
            return pLeaf ? pLeaf : pNode;
        }

        if (pMethod->kind == NK_CTOR)
        {
            // For NK_CTOR nodes, there is no name, but constructor arguments to
            // this/base, which FOLLOW the arguments to the constructor itself,
            // so the order must be this:
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->asCTOR()->pThisBaseCall, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pParms, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pType, pData, flags, ppTree)))
                pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pAttr, pData, flags, ppTree);
        }
        else if (pMethod->kind == NK_METHOD)
        {
            // Methods have names, which come before the parameters.
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->asMETHOD()->pName, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pParms, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->asMETHOD()->pConstraints, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pType, pData, flags, ppTree)))
                pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pAttr, pData, flags, ppTree);
        }
        else
        {
            // Operators and destructors have no name and no constructor args.
            ASSERT (pMethod->kind == NK_OPERATOR || pMethod->kind == NK_DTOR);
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pType, pData, flags, ppTree)) &&
                !(pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pParms, pData, flags, ppTree)))
                pLeaf = FindLeafEx (lastTokenCache, pos, pMethod->pAttr, pData, flags, ppTree);
        }

        if (pLeaf != NULL)
            return pLeaf;

        break;
    }

    case NK_PROPERTY:
    case NK_INDEXER:
    {
        PROPERTYNODE    *pProp = pNode->asANYPROPERTY();

        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pGet, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pSet, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pParms, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pName, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pType, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pProp->pAttr, pData, flags, ppTree);
        break;
    }

    case NK_ACCESSOR:
    {
        // Accessors are interior nodes.  If we weren't given a source data, we can't
        // build an interior tree so return this node.
        if (pData == NULL)
            return pNode;

        GetInteriorTree (pData, pNode, ppTree);
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asACCESSOR()->pBody, pData, flags, ppTree)))                
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asACCESSOR()->pAttr, pData, flags, ppTree);

        break;
    }

    case NK_PARAMETER:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPARAMETER()->pName, pData, flags, ppTree))
            || (pLeaf->flags & NF_NAME_MISSING))
        {  
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPARAMETER()->pType, pData, flags, ppTree))
                || (pLeaf->flags & NF_NAME_MISSING)) 
            { 
                //cyrusn  4/2/03
                //if the user has written:
                // public void foo([    //<-- cursor is after the [
                //we want to return the attribute declaration node, not the missing name node
                //
                //If there isn't a name node, or the name node is missing
                //use the attribute node if we can find one.
                pPossibleLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPARAMETER()->pAttr, pData, flags, ppTree);
                if (pPossibleLeaf) {
                    pLeaf = pPossibleLeaf;
                }                
            }                
        }
        break;

    case NK_NESTEDTYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNESTEDTYPE()->pType, pData, flags, ppTree);
        break;

    case NK_PARTIALMEMBER:               
        pPossibleLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPARTIALMEMBER()->pAttr, pData, flags, ppTree);
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPARTIALMEMBER()->pNode, pData, flags, ppTree);
        if (pLeaf == NULL ||
            (pLeaf->flags & NF_NAME_MISSING))
        {
            //If the name is missing, try to find the attribute.
            //If we're not able to, then default to the missing name
            if (pPossibleLeaf) {
                pLeaf = pPossibleLeaf;
            }
        }
        break;
        
    case NK_GENERICNAME:
        pLeaf = FindLeafEx(lastTokenCache, pos, pNode->asGENERICNAME()->pParams, pData, flags, ppTree);
        break;

    case NK_CONSTRAINT:
        if (!(pLeaf = FindLeafEx(lastTokenCache, pos, pNode->asCONSTRAINT()->pName, pData, flags, ppTree)))
            pLeaf = FindLeafEx(lastTokenCache, pos, pNode->asCONSTRAINT()->pBounds, pData, flags, ppTree);
        break;

    case NK_PREDEFINEDTYPE:
        pLeaf = pNode;
        break;
        
    case NK_NAMEDTYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNAMEDTYPE()->pName, pData, flags, ppTree);
        break;

    case NK_OPENTYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asOPENTYPE()->pName, pData, flags, ppTree);
        break;

    case NK_POINTERTYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asPOINTERTYPE()->pElementType, pData, flags, ppTree);
        break;

    case NK_NULLABLETYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNULLABLETYPE()->pElementType, pData, flags, ppTree);
        break;

    case NK_ARRAYTYPE:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asARRAYTYPE()->pElementType, pData, flags, ppTree);
        break;

    case NK_BLOCK:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asBLOCK()->pStatements, pData, flags, ppTree);
        break;

    case NK_ANONBLOCK:
        if (!(pLeaf = FindLeafEx(lastTokenCache, pos, pNode->asANONBLOCK()->pArgs, pData, flags, ppTree)))
            pLeaf = FindLeafEx(lastTokenCache, pos, pNode->asANONBLOCK()->pBody, pData, flags, ppTree);
        break;

    case NK_BREAK:
    case NK_CONTINUE:
    case NK_EXPRSTMT:
    case NK_GOTO:
    case NK_RETURN:
    case NK_YIELD:
    case NK_THROW:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYEXPRSTMT()->pArg, pData, flags, ppTree);
        break;

    case NK_CHECKED:
    case NK_LABEL:
    case NK_UNSAFE:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYLABELSTMT()->pStmt, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYLABELSTMT()->pLabel, pData, flags, ppTree);
        break;

    case NK_DO:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDO()->pExpr, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDO()->pStmt, pData, flags, ppTree);
        break;

    case NK_LOCK:
    case NK_WHILE:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYLOOPSTMT()->pStmt, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYLOOPSTMT()->pExpr, pData, flags, ppTree);
        break;

    case NK_FOR:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFOR()->pInit, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFOR()->pExpr, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFOR()->pInc, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asFOR()->pStmt, pData, flags, ppTree);
        break;

    case NK_IF:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asIF()->pExpr, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asIF()->pStmt, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asIF()->pElse, pData, flags, ppTree);
        break;

    case NK_DECLSTMT:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDECLSTMT()->pVars, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asDECLSTMT()->pType, pData, flags, ppTree);
        break;

    case NK_SWITCH:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asSWITCH()->pCases, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asSWITCH()->pExpr, pData, flags, ppTree);
        break;

    case NK_CASE:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCASE()->pStmts, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCASE()->pLabels, pData, flags, ppTree);
        break;

    case NK_TRY:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asTRY()->pCatch, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asTRY()->pBlock, pData, flags, ppTree);
        break;

    case NK_CATCH:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCATCH()->pBlock, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCATCH()->pName, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asCATCH()->pType, pData, flags, ppTree);
        break;

    case NK_ARRAYINIT:
    case NK_CASELABEL:
    case NK_UNOP:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYUNOP()->p1, pData, flags, ppTree);
        break;

    case NK_CALL:
    case NK_DEREF:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYBINOP()->p2, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYBINOP()->p1, pData, flags, ppTree);
        break;

    case NK_BINOP:
        //If they're looking for possible generics, then we have to try searching down 
        //both sides of a binop.  
        if (flags & EF_POSSIBLE_GENERIC_NAME) {
            if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYBINOP()->p1, pData, flags, ppTree))) {
                pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYBINOP()->p2, pData, flags, ppTree);
            }
            break;
        }
        //fallthrough;

    case NK_ARROW:
    case NK_LIST:
    case NK_DOT:
        {
            BINOPNODE * b = pNode->asANYBINOP();
            if (iLoopCount++ > 0x01000000) {
                VSFAIL("Expression too complex for FindLeafEx");
                break; // Expression too complex
            }
            if (b->p2) {
                ASSERT(!b->p2->IsStatement () && !b->p2->InGroup(NG_MEMBER));
                POSDATA savedStart = posStart;
                // Don't re-get (or test) the ending position because it's the same as mine
                if (FAILED(GetExtent (lastTokenCache, b->p2, flags, &posStart, NULL)))
                {
                    return NULL;
                }
                if (pos >= posStart) {
                    ASSERT(pos <= posEnd);
                    pNode = b->p2;
                    goto REDO;
                }
                posStart = savedStart;
            }
            if (b->p1) {
                ASSERT(!b->p1->IsStatement () && !b->p1->InGroup(NG_MEMBER));
                // Don't re-get (or test) the starting position because it's the same as mine
                if (FAILED(GetExtent (lastTokenCache, b->p1, flags, NULL, &posEnd)))
                {
                    return NULL;
                }
                if (pos <= posEnd) {
                    // We can't really assert this if the caller asks us to ignore the
                    // token stream, because the positions may be wrong.
                    // See the comment in srcmod.cpp:
                    VSIMPLIES((flags & EF_IGNORE_TOKEN_STREAM) == 0, pos >= posStart, "Wrong position.");
                    pNode = b->p1;
                    goto REDO;
                }
            }
        }
        break;

    case NK_OP:
    case NK_CONSTVAL:
        pLeaf = pNode;
        break;

    case NK_VARDECL:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asVARDECL()->pArg, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asVARDECL()->pName, pData, flags, ppTree);
        break;

    case NK_NEW:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNEW()->pInit, pData, flags, ppTree)) &&
            !(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNEW()->pArgs, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asNEW()->pType, pData, flags, ppTree);
        break;

    case NK_ATTR:
    case NK_ATTRARG:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYATTR()->pArgs, pData, flags, ppTree)))
            pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asANYATTR()->pName, pData, flags, ppTree);
        break;

    case NK_ATTRDECL:
        pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asATTRDECL()->pAttr, pData, flags, ppTree);
        break;

    case NK_TYPEWITHATTR:
        if (!(pLeaf = FindLeafEx (lastTokenCache, pos, pNode->asTYPEWITHATTR()->pType, pData, flags, ppTree))
            || (pLeaf->flags & NF_NAME_MISSING))
        {
            //if the user has written:
            // public class foo<[    //<-- cursor is after the [
            //we want to return the attribute declaration node, not the missing name node
            //
            //If there isn't a name node, or the name node is missing
            //use the attribute node if we can find one.
            pPossibleLeaf = FindLeafEx (lastTokenCache, pos, pNode->asTYPEWITHATTR()->pAttr, pData, flags, ppTree);
            if (pPossibleLeaf)
                pLeaf = pPossibleLeaf;
        }
        break;

    case NK_NAME:
        if (flags & EF_POSSIBLE_GENERIC_NAME && pNode->asNAME()->pPossibleGenericName != NULL) {
            return FindLeafEx(lastTokenCache, pos, pNode->asNAME()->pPossibleGenericName, pData, flags, ppTree);
        } else {
            break;
        }
    default:
        break;
    }

    if (pLeaf == NULL)
        pLeaf = pNode;

    return pLeaf;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT CParser::FindLeafNodeForTokenEx(long iToken, BASENODE* pNode, CSourceData* pData, ExtentFlags flags, BASENODE** ppNode, ICSInteriorTree** ppTree)
{
    HRESULT hr;
    LEXDATA lex;
    if (FAILED (hr = this->GetLexData (&lex)))
        return hr;

    if (iToken < 0 || iToken >= lex.TokenCount())
        return E_INVALIDARG;

    const CSTOKEN& token = lex.TokenAt(iToken);
    POSDATA pos = token;

    CBasenodeLookupCache cache;
    *ppNode = this->FindLeafEx (cache, pos, pNode, ppTree == NULL ? NULL : pData, flags, ppTree);
    if (*ppNode == NULL)
    {
        return E_FAIL;
    }


    // Now, check that FindLeafNode did a good job for corner cases
    // and compensate if it didn't
    long    iTokenFirst;
    long    iTokenLast;
    this->GetTokenExtent (*ppNode, &iTokenFirst, &iTokenLast, EF_FULL);

    // The 1st token is past the given token, we need to go backward a little bit
    if ((iTokenFirst > iToken) && (pos.iChar > 0))
    {
        pos.iChar--;
        if (ppTree && (*ppTree))
        {
            (*ppTree)->Release();
            (*ppTree) = NULL;
        }
        *ppNode = this->FindLeafEx (cache, pos, pNode, ppTree == NULL ? NULL : pData, flags, ppTree);
    }
    // The last token is before the given token, we need to go forward a little bit
    else if (iTokenLast < iToken)
    {
        if (ppTree && (*ppTree))
        {
            (*ppTree)->Release();
            (*ppTree) = NULL;
        }
        *ppNode = this->FindLeafEx(cache, token.StopPosition(), pNode, ppTree == NULL ? NULL : pData, flags, ppTree);
    }

    //NULL is how FindLeafEx indicates failure;
    return *ppNode == NULL ? E_FAIL : S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT CParser::GetExtent (CBasenodeLookupCache& cache, BASENODE *pNode, ExtentFlags flags, POSDATA *pposStart, POSDATA *pposEnd)
{
    BOOL    fMissingName;

    if (pposStart != NULL)
    {
        long    iTok = GetFirstToken (pNode, flags, &fMissingName);

        if (iTok < 0) {
            *pposStart = POSDATA(0,0);
            return E_FAIL;
        }

        if (iTok >= m_iTokens)
        {
            POSDATA posEOF = (m_iTokens >= 1) ? m_pTokens[m_iTokens - 1] : POSDATA(0,0);
            *pposStart = posEOF;
            return E_FAIL;
        }

        ASSERT (iTok >= 0 && iTok < m_iTokens);
        *pposStart = m_pTokens[iTok];
        if (fMissingName)
        {
            // The first token in this span was the "missing name" node.  So, the
            // start position of that token is the first character FOLLOWING the
            // token at the specified index.  Note that we only need to rescan the
            // token for that if it isn't a 'fixed' token.
            if (m_rgTokenInfo[m_pTokens[iTok].Token()].iLen == 0)
                *pposStart = m_pTokens[iTok].StopPosition();
            else
                pposStart->iChar += m_rgTokenInfo[m_pTokens[iTok].Token()].iLen;
        }
    }

    if (pposEnd != NULL)
    {
        long    iTok = GetLastToken (cache, pNode, flags, &fMissingName);

        if (iTok < 0) {
            *pposEnd = POSDATA(0,0);
            return E_FAIL;
        }

        //If the last token in the node exceeds teh number of tokens we have, 
        //then the end extent is the end of the file.
        if (iTok >= m_iTokens) 
        {
            POSDATA posEOF = (m_iTokens >= 1) ? m_pTokens[m_iTokens - 1] : POSDATA(0,0);
            *pposEnd = posEOF;
            return E_FAIL;
        }

        ASSERT (iTok >= 0 && iTok < m_iTokens);

        *pposEnd = m_pTokens[iTok].StopPosition();
        if (fMissingName)
        {
            long nextNonNoisy = this->PeekTokenIndexFrom(iTok, 1);
            if (nextNonNoisy > iTok && nextNonNoisy < m_iTokens)
            {
                *pposEnd = m_pTokens[nextNonNoisy]; 
            }
            /*
            // Adjust the end of this span to include all trailing whitespace.  The
            // token found is the "missing name" token, whose index is actually
            // the token PRECEDING the intended identifier.
            while ((m_pszSource + m_piLines[pposEnd->iLine])[pposEnd->iChar] == ' ' || (m_pszSource + m_piLines[pposEnd->iLine])[pposEnd->iChar] == '\t')
                pposEnd->iChar++;
            */
        }
    }
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CParser::GetTokenExtent

void CParser::GetTokenExtent (BASENODE *pNode, long *piFirst, long *piLast, ExtentFlags flags)
{
    BOOL    fMissingName;

    if (piFirst != NULL)
        *piFirst = GetFirstToken (pNode, flags, &fMissingName);

    CBasenodeLookupCache cache;
    if (piLast != NULL)
        *piLast = GetLastToken (cache, pNode, flags, &fMissingName);
}


////////////////////////////////////////////////////////////////////////////////
// CTextParser::CTextParser

CTextParser::CTextParser () :
    m_pParser(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::~CTextParser

CTextParser::~CTextParser ()
{
    if (m_pParser != NULL)
        delete m_pParser;
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::Initialize

HRESULT CTextParser::Initialize (CController *pController, CompatibilityMode cm)
{
    m_pParser = new CSimpleParser (pController);
    if (m_pParser == NULL)
        return E_OUTOFMEMORY;

    return CTextLexer::CreateInstance (pController->GetNameMgr(), cm, &m_spLexer);
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::SetInputText

STDMETHODIMP CTextParser::SetInputText (PCWSTR pszText, long iLen)
{
    return m_spLexer->SetInput (pszText, iLen);
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::CreateParseTree

STDMETHODIMP CTextParser::CreateParseTree (ParseTreeScope iScope, BASENODE *pParent, BASENODE **ppNode)
{
    LEXDATA     ld;
    HRESULT     hr;

    if (FAILED (hr = m_spLexer->GetLexResults (&ld)))
        return hr;

    m_pParser->SetInputData (L"", &ld);  
    m_pParser->Rewind (0);

    switch (iScope)
    {
        case PTS_TYPEBODY:
        case PTS_ENUMBODY:
            return E_NOTIMPL;

        case PTS_NAMESPACEBODY: 
            {
                if (pParent == NULL)
                {
                    *ppNode = m_pParser->ParseRootNamespace ();
                }
                else
                {
                    *ppNode = m_pParser->ParseNamespace(pParent);
                }
            }
            break;

        case PTS_MEMBER_REF_SPECIAL:*ppNode = m_pParser->ParseMemberRefSpecial();   break;
        case PTS_STATEMENT:         *ppNode = m_pParser->ParseStatement (pParent);  break;
        case PTS_EXPRESION:         *ppNode = m_pParser->ParseExpression (pParent); break;
        case PTS_TYPE:              *ppNode = m_pParser->ParseType (pParent);       break;
        case PTS_MEMBER:
            {
                MEMBERNODE* pNode = m_pParser->ParseMember (pParent->asAGGREGATE());
                if (pNode != NULL)
                {
                    pNode->pNext = NULL;
                }

                *ppNode = pNode;
            }
            break;
        case PTS_PARAMETER:     *ppNode = m_pParser->ParseParameter(pParent, CParser::kppoAllowAll); break;

        default:
            return E_INVALIDARG;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::AllocateNode

STDMETHODIMP CTextParser::AllocateNode (long iKind, BASENODE **ppNode)
{
    return E_NOTIMPL;
}


////////////////////////////////////////////////////////////////////////////////
// CTextParser::FindLeafNode

STDMETHODIMP CTextParser::FindLeafNode(const POSDATA pos, BASENODE* pNode, BASENODE **ppLeafNode)
{
    if (ppLeafNode && m_pParser)
    {
        *ppLeafNode = m_pParser->FindLeaf(pos, pNode, NULL, NULL);
        
        return (*ppLeafNode == NULL) ? E_FAIL : S_OK;
    }
    else
    {
        return E_FAIL;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CTextParser::GetLexResults

STDMETHODIMP CTextParser::GetLexResults(LEXDATA *pLexData)
{
    if (m_spLexer)
    {
        return m_spLexer->GetLexResults(pLexData);
    }
    else
    {
        return E_FAIL;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CTextParser::GetExtent

STDMETHODIMP CTextParser::GetExtent(BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags)
{
    if (m_pParser)
    {
        CBasenodeLookupCache cache;
        return m_pParser->GetExtent(cache, pNode, flags, pposStart, pposEnd);
    }
    else
    {
        return E_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CTextParser::GetExtent

STDMETHODIMP CTextParser::GetTokenExtent(BASENODE *pNode, long *piFirstToken, long *piLastToken, ExtentFlags flags)
{
    if (m_pParser)
    {
        m_pParser->GetTokenExtent(pNode, piFirstToken, piLastToken, flags);
        return NOERROR;
    }
    else
    {
        return E_FAIL;
    }
}
