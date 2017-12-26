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
// File: tokdata.h
//
// ===========================================================================

#ifndef _TOKDATA_H_
#define _TOKDATA_H_

#include "enum.h"
#include "posdata.h"
#include "constval.h"

#include "unichar.h"
#include "uniprop.h"
#undef scope

struct NAME;
class CWarningMap;

////////////////////////////////////////////////////////////////////////////////
// ITokenAllocator

interface ITokenAllocator
{
public:
    virtual void *AllocateMemory(size_t size) = 0;
    virtual void Free(void *p) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// TOKFLAGS

enum
{
    TFF_NSELEMENT               = 0x00000001,   // Token belongs to first set for namespace elements
    TFF_MEMBER                  = 0x00000002,   // Token belongs to first set for type members
    TFF_TYPEDECL                = 0x00000004,   // Token belongs to first set for type declarations (sans modifiers)
    TFF_PREDEFINED              = 0x00000008,   // Token is a predefined type
    TFF_TERM                    = 0x00000010,   // Token belongs to first set for term-or-unary-operator (follows casts), but is not a predefined type.
    TFF_OVLUNOP                 = 0x00000020,   // Token is an overloadable unary operator
    TFF_OVLBINOP                = 0x00000040,   // Token is an overloadable binary operator
    TFF_OVLOPKWD                = 0x00000080,   // Token is NOT an operator even though op != OP_NONE
    TFF_CASTEXPR                = 0x00000100,   // Token after an ambiguous type-or-expr cast forces a cast
    TFF_MODIFIER                = 0x00000200,   // Token is a modifier keyword
    TFF_NOISE                   = 0x00000400,   // Token is "noise"; should be ignored by parser
    TFF_MSKEYWORD               = 0x00000800,   // Token is a Microsoft Extension keyword (i.e not part of the ECMA or ISO standard)
    TFF_INVALIDSUBEXPRESSION    = 0x00001000,   // Token is not valid to start a sub expression with (like "if").

    // These flags are for instances of tokens -- they go in the user bits, of which there are 4.
    TF_OVERHEAD         = 0x1,          // Token has "overhead"
    TF_UNTERMINATED     = 0x2,          // Unterminated token such as TID_STRINGLIT or TID_CHARLIT
    TF_HEAPALLOCATED    = 0x4,          // Token's overhead was allocated on the heap (only set/checked by CSourceLexer)
    TF_VERBATIMSTRING   = 0x8,          // TID_STRINGLIT is a @"..." form
    TF_USEDCOMMENT      = 0x8,          // TID_DOCCOMMENT or TID_MLDOCCOMMENT has been used!
    TF_HEXLITERAL       = 0x8,          // a hexadecimal constant 0xDDDD. Used for enum conversions.

};

////////////////////////////////////////////////////////////////////////////////
// TOKENDATA

struct TOKENDATA
{
    TOKENID     iToken;                 // Token ID
    POSDATA     posTokenStart;          // First character of token
    POSDATA     posTokenStop;           // First character following token
    NAME        *pName;                 // Name (iff TID_IDENTIFIER)
    long        iLength;                // Length of token -- INCLUDING multiple lines in @"..." string cases
    DWORD       dwFlags;                // Flags about token (TF_* flags above, NOT TFF_* flags...)
};

////////////////////////////////////////////////////////////////////////////////
// ESCAPEDNAME
//
// Used to represent an identifier that contained unicode escape(s) in source.
// A special structure so we know how long the token was in the source code.

struct ESCAPEDNAME
{
    NAME    *pName;                 // Name (escapes converted)
    long    iLen;                   // In-source length
};

////////////////////////////////////////////////////////////////////////////////
// STRLITERAL
//
// Holds a string literal, including the converted text and length, as well as
// the in-source length (including delimiters, of course)

struct STRLITERAL
{
    long        iSourceLength;          // Length (in-source, including delimiters)

    STRCONST    str;                    // Actual string
};

////////////////////////////////////////////////////////////////////////////////
// VSLITERAL
//
// Holds a verbatim string literal, including the converted text and length, as 
// well as the end-position 

struct VSLITERAL
{
    POSDATA     posEnd;                 // End-position of string

    STRCONST    str;                    // Actual string
};

////////////////////////////////////////////////////////////////////////////////
// LITERAL
//
// Used to represent any token whose text is interesting/essential post-lex,
// such as strings, numbers, even comments.  The text pointer

struct LITERAL
{
    long    iSourceLength;          // In-source length
    WCHAR   szText[1];              // Text of token (extent of which depends on token; i.e. comments to not include delimiters, etc.)
};

////////////////////////////////////////////////////////////////////////////////
// DOCLITERAL
//
// Used to represent a doc comment token whose text is interesting/essential post-lex,

struct DOCLITERAL
{
    POSDATA posEnd;                 // In-source end-pos (either EOL char or '/' char in terminating "*/")
    WCHAR   szText[1];              // Text of token (includes "///", EOL, "/**", and "*/")
};

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN
//
// Here's the new token structure.  Since TOKENDATA will become obsolete, we'll
// rename this to TOKENDATA once the compiler is producing the new stream form.

struct CSTOKEN : public POSDATA
{
    friend class CLexer;

private:
    // Based on what the token is, the extra data is kept here
    union
    {
        NAME        *pName;             // TID_IDENTIFIER (HasOverhead = FALSE), or TID_UNKNOWN (which is treated as an identifier):
                                        //      Name stored in name table
        ESCAPEDNAME *pEscName;          // TID_IDENTIFIER (HasOverhead = TRUE):
                                        //      ESCAPEDNAME structure containing name and length
        STRLITERAL  *pStringLiteral;    // TID_STRINGLIT:
                                        //      STRLITERAL structure containing converted text, converted length, and in-source length
        VSLITERAL   *pVSLiteral;        // TID_VSLITERAL:
                                        //      VSLITERAL structure
        LITERAL     *pLiteral;          // TID_NUMBER:
                                        //      LITERAL structure containing text of token and length
        POSDATA     *pposEnd;           // TID_MLCOMMENT:
                                        //      POSDATA containing the position of the end of the comment
        DOCLITERAL  *pDocLiteral;       // TID_DOCCOMMENT, TID_MLDOCCOMMENT:
                                        //      DOCCOMMENT containing text of comment and ending position
        long        iLength;            // All "fixed" tokens, plus TID_SLCOMMENT:
                                        //      Length of token (including delimiter(s))
        struct {
                                            // TID_CHARLIT:
            unsigned    cCharValue:16;      //      Character value
            unsigned    iCharLen:16;        //      Length (in-source, including delimiters)
        } chr;
    };

public:
    // Here are the public accessors

    //Turns out inline is important for this function.  Dont' remove it without reason
    __forceinline TOKENID Token() const
    {
        return (TOKENID)iUserByte; 
    }


    bool        IsKeyword();
    bool        IsModifier();
    bool        IsAccessibilityModifier();
    bool        HasOverhead() const { return iUserBits & TF_OVERHEAD; }
    BOOL        IsUnterminated () { return iUserBits & TF_UNTERMINATED; }
    
    bool IsComment() const
    {
        return iUserByte == TID_SLCOMMENT || iUserByte == TID_MLCOMMENT || iUserByte == TID_DOCCOMMENT || iUserByte == TID_MLDOCCOMMENT;
    }

    bool IsDocComment() const 
    {
        return iUserByte == TID_DOCCOMMENT || iUserByte == TID_MLDOCCOMMENT; 
    }

    BOOL        IsNoisy() const { return IsComment() || Token() == TID_UNKNOWN; }
    NAME*       Name () const { ASSERT (Token() == TID_IDENTIFIER || Token() == TID_UNKNOWN); return (Token() == TID_IDENTIFIER && HasOverhead()) ? pEscName->pName : pName; }
    STRLITERAL* StringLiteral() const { ASSERT (Token() == TID_STRINGLIT); return pStringLiteral; }
    VSLITERAL*  VSLiteral () { ASSERT (Token() == TID_VSLITERAL); return pVSLiteral; }
    LITERAL*    Literal () const { ASSERT (Token() == TID_NUMBER); return pLiteral; }
    DOCLITERAL* DocLiteral () const { ASSERT (Token() == TID_DOCCOMMENT || Token() == TID_MLDOCCOMMENT); return pDocLiteral; } const
    WCHAR       CharValue () { ASSERT (Token() == TID_CHARLIT); return chr.cCharValue; }
    long        Length() const;  // ALWAYS returns IN-SOURCE length; -1 if token is a multi-line token (TID_MLCOMMENT or TID_VSLITERAL spanning > 1 line)
    void*       Overhead () { return HasOverhead() ? pLiteral : NULL; }
    POSDATA     StartPosition() const { return *this; }
    POSDATA     StopPosition() const;
    HRESULT     CloneOverhead(ITokenAllocator *pAllocator, CSTOKEN *pToken);
    void        SetRawOverhead(void *ptr) { pLiteral = (LITERAL *)ptr; }
    bool        IsEqual(CSTOKEN &other, bool fComparePositions);

    void UpdateStartAndEndPositions(int iLineDelta, int iCharDelta);

    CSTOKEN &   operator = (const POSDATA &pos) { iChar = pos.iChar; iUserByte = pos.iUserByte; iLine = pos.iLine; iUserBits = pos.iUserBits; this->pName = NULL; return *this; }
};

////////////////////////////////////////////////////////////////////////////////

class CIdentTable;


struct TOKENSTREAM
{
    TOKENSTREAM() : pTokens(NULL), iTokens(0) {}
    CSTOKEN *pTokens;
    long iTokens;
};

////////////////////////////////////////////////////////////////////////////////
// LEXDATA
//
// This structure contains all the data that is accumlated/calculated/produced
// by bringing a source module's token stream up to date.  This includes the
// token stream (tokens, positions, count), the line table (line offsets and
// count), the raw source text, the comment table (positions and count), the
// conditional compilation state transitions (line numbers and count), etc., etc.
// Note that some of the fields in this struct will not be filled with data
// unless the appropriate flags were passed to the construction of the
// compiler (controller) object -- for example, the identifier table will not
// be present unless CCF_KEEPIDENTTABLES was given, etc.
//
// All the data pointed to by this structure is owned by the originating
// source module, and is only valid while the calling thread holds a reference
// to the source data object through which it obtained this data!

struct LEXDATA
{
    // The incremental tokenizer is really partying on with the token stream array
    friend class CSourceModuleBase;
    friend class CSourceModule;
    friend class CSourceModuleClone;

protected:
    // Token stream data
    CSTOKEN         *pTokens;               // Array of CSTOKEN's 
    long            iTokens;                // Token count

    // Line table data
    long            *piLines;               // Array of offsets from pszSource of beginning of each line (piLines[0] == 0 always)
    long            iLines;                 // Line count

    // Conditional compilation data
    long            *piTransitionLines;     // Array of line numbers indicating conditional inclusion/exclusion transitions
    long            iTransitionLines;       // Transition count

    // Region table data
    long            *piRegionStart;         // Array of line numbers indicating #region directives
    long            *piRegionEnd;           // Array of line numbers indicating #endregion directives (CORRESPONDING -- these could be nested but are represented here in order of appearance of #region)
    long            iRegions;               // Region count

    // Raw source text pointer
    PCWSTR           pszSource;             // Pointer to the entire provided source text, null-terminated

public:

    // Identifier table                     (CCF_KEEPIDENTTABLES flag required)
    CIdentTable     *pIdentTable;           // Identifier table

    // #pragma warning map
    CWarningMap     *pWarningMap;

public:
    LEXDATA();

    void            InitTokens (CSTOKEN *pTok, long iTok);
    void            UnsafeExposeTokens(CSTOKEN **ppTokens, long *piTokens) const;
    long            TokenCount() const;
    CSTOKEN &       TokenAt (int iTok) const;

    void            InitLineOffsets(long *piLines, long iLines);
    void            UnsafeExposeLineOffsets(long **ppLines, long *piLines) const;
    long            LineCount() const;
    long            LineOffsetAt(long iLineOffset) const;

    long            TransitionLineCount() const;
    long            TransitionLineAt(long iTransitionLine) const;

    long            RegionCount() const;
    long            RegionStartAt(long iRegion) const;
    long            RegionEndAt(long iRegion) const;

    void            InitSource(PCWSTR pszSource);
    bool            HasSource() const;
    void            UnsafeExposeSource(PCWSTR *ppszSource) const;


    PCWSTR          TextAt (long iLine, long iChar)  const;
    PCWSTR          TextAt (const POSDATA& pos)  const;
    HRESULT         ExtractText(long iLine, BSTR *pbstrText) const;
    HRESULT         ExtractText(const POSDATA& posStart, BSTR *pbstrText) const;
    HRESULT         ExtractText(const POSDATA &posStart, const POSDATA &posEnd, BSTR *pbstrText) const;
    WCHAR           CharAt (long iLine, long iChar) const;
    WCHAR           CharAt (const POSDATA &pos) const;
    HRESULT         FindFirstTokenOnLine (long iLine, long *piToken) const;
    HRESULT         FindLastTokenOnLine(long iLine, long *piToken) const;
    BOOL            IsFirstTokenOnLine (long iToken) const;
    BOOL            IsLastTokenOnLine (long iToken) const;
    HRESULT         FindFirstNonWhiteChar (long iLine, long *piChar) const;
    HRESULT         FindEndOfComment (long iComment, POSDATA *pposEnd) const;
    HRESULT         FindFirstPrecedingNonWhiteChar (const POSDATA &pos, POSDATA *pposNonWhite) const;
    HRESULT         GetPreprocessorDirective (long iLine, ICSNameTable *pNameTable, PPTOKENID *piToken, long *piStart, long *piEnd) const;
    bool            IsPreprocessorLine(long iLine, ICSNameTable* pNameTable) const;
    HRESULT         SpanContainsPreprocessorDirective(long iStartLine, long iEndLine, ICSNameTable *pNameTable, bool *pfContains) const;
    long            GetLineLength (long iLine) const;
    BOOL            IsWhitespace (long iStartLine, long iStartChar, long iEndLine, long iEndChar) const;
    BOOL            IsLineWhitespaceBefore (long iLine, long iChar) const;
    BOOL            IsLineWhitespaceBefore (const POSDATA &pos) const;
    BOOL            IsLineWhitespaceAfter (long iLine, long iChar) const;
    BOOL            IsLineWhitespaceAfter (const POSDATA &pos) const;
    bool            IsInsideString(const POSDATA& pos) const;
    bool            IsInsideSkippedPreProcessorRegion(const POSDATA& pos) const;
    long            GetAbsolutePosition (long iToken) const;
    long            FindNearestPosition (const POSDATA& pos) const;
    HRESULT         CloneTokenStream(ITokenAllocator *pAllocator, long iFirstToken, long iTokenCount, long iAllocTokens, TOKENSTREAM *pTokenstream) const;
    HRESULT         FreeClonedTokenStream(ITokenAllocator *pAllocator, TOKENSTREAM *pTokenstream) const;
    HRESULT         FindPositionOfText(POSDATA startPos, PCWSTR pszText, POSDATA *ppos) const;
    HRESULT         FindPositionOfText(PCWSTR pszText, POSDATA *ppos) const;

    bool            TokenTouchesLine(long iTok, unsigned long iLine) const;

    BOOL            IsNoisyToken(long iToken) const;
    BOOL            IsFirstTokenOnLine (long iToken, bool bSkipNoisyTokens) const;
    BOOL            IsLastTokenOnLine (long iToken, bool bSkipNoisyTokens) const;
    long            PeekTokenIndexFrom (long iCur, long iPeek = 1) const;
    long            NextNonNoisyToken (long iCur) const;
    long            SkipNoisyTokens(long iCur, long iInc) const;

    static long     PeekTokenIndexFromInternal (CSTOKEN * pTokens, long iTokens, const long iCur, long iPeek);
};

// Useful functions...
inline BOOL IsIdentifierProp (BYTE prop) 
{ 
    return IsPropAlpha(prop) || 
           IsPropLetterDigit(prop); 
}

inline BOOL IsIdentifierChar (WCHAR c) 
{ 
    return IsIdentifierProp(UProp(c)) || 
           c == L'_'; 
}
inline BOOL IsIdentifierPropOrDigit(BYTE prop) 
{ 
    return IsIdentifierProp(prop) || 
           IsPropDecimalDigit(prop) || 
           IsPropNonSpacingMark(prop) || 
           IsPropCombiningMark(prop)  || 
           IsPropConnectorPunctuation(prop) || 
           IsPropOtherFormat(prop); 
}
inline BOOL IsIdentifierCharOrDigit (WCHAR c) 
{ 
    return IsIdentifierPropOrDigit(UProp(c)); 
}
inline BOOL IsHexDigit (WCHAR c) 
{
    return (c >= '0' && c <= '9') || 
           (c >= 'A' && c <= 'F') || 
           (c >= 'a' && c <= 'f'); 
}
inline int  HexValue (WCHAR c) 
{ 
    ASSERT (IsHexDigit (c)); 
    return (c >= '0' && c <= '9') ? c - '0' : (c & 0xdf) - 'A' + 10; 
}

inline BOOL IsWhitespaceChar (WCHAR ch) 
{ 
    return  
        ch == ' ' || 
        ch == UCH_TAB || 
        ch == '\v' || 
        ch == '\f' || 
        (ch > 127 && IsSpaceSeparator(ch)); 
}

inline BOOL IsEndOfLineChar (WCHAR ch) 
{ 
    return 
        ch == '\r' || 
        ch == '\n' || 
        ch == UCH_PS || 
        ch == UCH_LS || 
        ch == 0x0085;
}

#endif  // _TOKDATA_H_
