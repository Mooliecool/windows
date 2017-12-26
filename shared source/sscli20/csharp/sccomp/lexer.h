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
// File: lexer.h
//
// ===========================================================================

#ifndef __lexer_h__
#define __lexer_h__

#include "csiface.h"
#include "tokinfo.h"
#include "namemgr.h"
#include "array.h"

class COptionData;

////////////////////////////////////////////////////////////////////////////////
// CLexer
//
// This is the base lexer class.  It's abstract -- you must create a derivation
// and implement the pure virtuals.

class CLexer
{
public:
    NAMEMGR           *m_pNameMgr;                // Name manager
    PCWSTR            m_pszCurrent;               // Current character
    PCWSTR            m_pszCurLine;               // Beginning of current line
    long              m_iCurLine;                 // Index of current line
    CompatibilityMode m_eKeywordMode;           // Which keywords this lexer recognizes
    unsigned          m_fEscaped:1;               // TRUE if token had escapes
    unsigned          m_fFirstOnLine:1;           // TRUE if no tokens have been scanned from this line yet
    unsigned          m_fPreproc:1;               // TRUE if preprocessor tokens are valid
    unsigned          m_fTokensSeen:1;            // TRUE if tokens have been seen (and thus #define/#undef are invalid)
    unsigned          m_fLimitExceeded:1;         // TRUE if too many lines or line too long
    unsigned          m_fThisLineTooLong:1;       // TRUE if this line is too long and an error has already been given

    CLexer (NAMEMGR *pNameMgr, CompatibilityMode eKeywordMode) :
        m_pNameMgr(pNameMgr),
        m_pszCurrent(NULL),
        m_pszCurLine(NULL),
        m_iCurLine(0),
        m_eKeywordMode(eKeywordMode),
        m_fEscaped(FALSE),
        m_fFirstOnLine(FALSE),
        m_fPreproc(FALSE),
        m_fTokensSeen(FALSE),
        m_fLimitExceeded(FALSE),
        m_fThisLineTooLong(FALSE),
        m_hr(S_OK) {}
    virtual ~CLexer () {}

    BOOL        PositionOf (PCWSTR psz, POSDATA *ppos);
    TOKENID     ScanToken (CSTOKEN *pToken);
    WCHAR       ScanEscapeSequence (PCWSTR &p, __out PWCH pchSurrogate);

    // Overridables
    // Derived classes must call CLexer::TrackLine so that we can reset m_fThisLineTooLong
    // Otherwise we can't properly report exactly 1 error message when a line is too long
    virtual void            TrackLine (PCWSTR pszNewLine) {m_fThisLineTooLong = FALSE; }
    virtual BOOL            ScanPreprocessorLine (PCWSTR &p) = 0;
    virtual BOOL            RepresentNoiseTokens () = 0;
    virtual void            *TokenMemAlloc (CSTOKEN *pToken, size_t iSize) = 0;

    void ErrorAtPosition(long iLine, long iCol, long iExtent, int iErrorId)
        { ErrorPosArgs(iLine, iCol, iExtent, iErrorId, 0, NULL); }
    void ErrorAtPosition(long iLine, long iCol, long iExtent, int iErrorId, ErrArg arg)
        { ErrorPosArgs(iLine, iCol, iExtent, iErrorId, 1, &arg); }

protected:
    friend class CSourceModule;
    WCHAR       ScanUnicodeEscape (PCWSTR &p, __out PWCH pchSurrogate, BOOL fPeek);
    WCHAR       PeekUnicodeEscape (PCWSTR &p, __out PWCH pchSurrogate);
    WCHAR       PeekChar (PCWSTR  p, __out PWCH pchSurrogate);
    WCHAR       NextChar (PCWSTR &p, __out PWCH pchSurrogate);
    
    void        CreateInvalidToken(CSTOKEN *pToken, PCWSTR pszTokenStart, PCWSTR pszTokenEnd);
    void        ReportInvalidToken(CSTOKEN *pToken, PCWSTR pszTokenStart, PCWSTR pszTokenEnd);

    virtual void ErrorPosArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg) = 0;

    HRESULT           m_hr;

};

////////////////////////////////////////////////////////////////////////////////
// CTextLexer
//
// This is the implementation of a CLexer that also exposes ICSLexer for
// external clients.  It duplicates the given input text (and thus isn't intended
// for large-scale lexing tasks), creates a line table and token stream, and has
// rescan capability.

class CTextLexer :
    public CComObjectRootMT,
    public ICSLexer,
    public CLexer
{
private:
    CStructArray<CSTOKEN>   m_arrayTokens;
    PWSTR                   m_pszInput;
    CStructArray<long>      m_arrayLines;
    BOOL                    m_fTokenized;
    BOOL                    m_fRepresentNoise;

public:
    BEGIN_COM_MAP(CTextLexer)
        COM_INTERFACE_ENTRY(ICSLexer)
    END_COM_MAP()

    CTextLexer () : CLexer(NULL, CompatibilityNone), m_pszInput(NULL), m_fTokenized(FALSE), m_fRepresentNoise(TRUE) {}
    ~CTextLexer ();

    static  HRESULT CreateInstance (ICSNameTable *pNameTable, CompatibilityMode eKeywordMode, ICSLexer **ppLexer);

    HRESULT         Initialize (ICSNameTable *pNameTable, CompatibilityMode eKeywordMode);

    // ICSLexer
    STDMETHOD(SetInput)(PCWSTR pszInput, long iLen);
    STDMETHOD(GetLexResults)(LEXDATA *pLexData);
    STDMETHOD(RescanToken)(long iToken, TOKENDATA *pTokenData);

    // CLexer
    void            TrackLine (PCWSTR pszNewLine);
    BOOL            ScanPreprocessorLine (PCWSTR &p) { return FALSE; }
    BOOL            RepresentNoiseTokens () { return m_fRepresentNoise; }
    void            *TokenMemAlloc (CSTOKEN *pToken, size_t iSize);

protected:
    void ErrorPosArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg) {}
};


////////////////////////////////////////////////////////////////////////////////
// CScanLexer
//
// This is the simplest form of a lexer, which allows you to initialize it with
// a piece of null-terminated text, and then call ScanToken() repeatedly until
// it returns TID_ENDFILE.  There is no rescan capability, short of calling
// SetInput() again.  

class CScanLexer : public CLexer
{
private:
    BOOL            m_fRepresentNoise;
    void            *m_pTokenMem;
    size_t          m_iMemSize;
    size_t          m_iMemUsed;

public:
    CScanLexer (NAMEMGR *pNameMgr, BOOL fRepresentNoise, CompatibilityMode eKeywordMode) : CLexer (pNameMgr, eKeywordMode), m_fRepresentNoise(fRepresentNoise), m_pTokenMem(NULL), m_iMemSize(0), m_iMemUsed(0) {}
    ~CScanLexer ();

    void        SetInput (PCWSTR pszInput) { m_pszCurLine = m_pszCurrent = pszInput; }

    // CLexer overrides.  Note, nothing happens for ALL of them (except TrackLine)...
    void            TrackLine (PCWSTR pszNewLine) { m_pszCurLine = pszNewLine; m_iCurLine++; CLexer::TrackLine(pszNewLine); }
    BOOL            ScanPreprocessorLine (PCWSTR &p) { return FALSE; }
    BOOL            RepresentNoiseTokens () { return TRUE; }
    void            *TokenMemAlloc (CSTOKEN *pToken, size_t iSize);

protected:
    void ErrorPosArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg) {}
};

////////////////////////////////////////////////////////////////////////////////
// CLexer::PeekChar

__forceinline WCHAR CLexer::PeekChar (PCWSTR p, __out PWCH pchSurrogate)
{
    ASSERT(pchSurrogate);

    WCHAR ch = *p;
    *pchSurrogate = L'\0';
    if ('\\' == ch && (p[1] == 'U' || p[1] == 'u'))
        return PeekUnicodeEscape (p, pchSurrogate);
    else
        return ch;
}

////////////////////////////////////////////////////////////////////////////////
// CLexer::NextChar

__forceinline WCHAR CLexer::NextChar (PCWSTR &p, __out PWCH pchSurrogate)
{
    ASSERT(pchSurrogate);

    WCHAR ch = *p++;
    *pchSurrogate = L'\0';
    if ('\\' == ch && (*p == 'U' || *p == 'u'))
        return ScanUnicodeEscape (p, pchSurrogate, FALSE);
    else
        return ch;
}

#endif //__lexer_h__
