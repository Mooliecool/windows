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
// File: lexer.cpp
//
// ===========================================================================

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// CLexer::PositionOf

inline BOOL CLexer::PositionOf (PCWSTR psz, POSDATA *ppos)
{
    ASSERT(psz >= m_pszCurLine);
    if (psz < m_pszCurLine || psz - m_pszCurLine >= MAX_POS_LINE_LEN)
        return FALSE;
    ppos->iLine = m_iCurLine;
    ppos->iChar = psz - m_pszCurLine;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CLexer::PeekUnicodeEscape

WCHAR CLexer::PeekUnicodeEscape (PCWSTR &q, __out PWCH pchSurrogate)
{
    ASSERT(pchSurrogate);

    // if we're peeking, then we don't want to change the position
    PCWSTR p = q+1;
    return ScanUnicodeEscape(p, pchSurrogate, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
// CLexer::ScanUnicodeEscape

WCHAR CLexer::ScanUnicodeEscape (PCWSTR &p, __out PWCH pchSurrogate, BOOL fPeek)
{
    ASSERT(pchSurrogate);
	*pchSurrogate = 0;

    PCWSTR  pszStart = p - 1; // Back-up to the '\'
    ASSERT(*pszStart == '\\');

    WCHAR   ch = *p++;
    if (ch == 'U') {
        unsigned int     uChar = 0;

        if (!IsHexDigit (*p))
        {
            if (!fPeek)
                ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
        }
        else
        {
            for (int i=0; i<8; i++)
            {
                if (!IsHexDigit (*p))
                {
                    if (!fPeek)
                        ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
                    break;
                }
                uChar = (uChar << 4) + HexValue (*p++);
            }

            if (uChar < 0x00010000)
            {
                ch = (WCHAR)uChar;
                *pchSurrogate = L'\0';
            }
            else if (uChar > 0x0010FFFF)
            {
                if (!fPeek)
                    ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
            }
            else
            {
                ASSERT(uChar > 0x0000FFFF && uChar <= 0x0010FFFF);
                ch = (WCHAR)((uChar - 0x00010000) / 0x0400 + 0xD800);
                *pchSurrogate = (WCHAR)((uChar - 0x00010000) % 0x0400 + 0xDC00);
            }
        }
    } else {
        ASSERT(ch == L'u' || ch == L'x');
        
        int     iChar = 0;

        if (!IsHexDigit (*p))
        {
            if (!fPeek)
                ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
        }
        else
        {
            for (int i=0; i<4; i++)
            {
                if (!IsHexDigit (*p))
                {
                    if (ch == 'u' && !fPeek)
                        ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
                    break;
                }
                iChar = (iChar << 4) + HexValue (*p++);
            }

            ch = (WCHAR)iChar;
        }
    }

    return ch;
}

////////////////////////////////////////////////////////////////////////////////
// CLexer::ScanEscapeSequence

WCHAR CLexer::ScanEscapeSequence (PCWSTR &p, __out PWCH pchSurrogate)
{
    ASSERT(pchSurrogate);
	*pchSurrogate = 0;

    PCWSTR  pszStart = p;
    WCHAR   ch = *p++;

    switch (ch)
    {
        case '\'':
        case '\"':
        case '\\':
            break;

        case 'a':   ch = '\a';  break;
        case 'b':   ch = '\b';  break;
        case 'f':   ch = '\f';  break;
        case 'n':   ch = '\n';  break;
        case 'r':   ch = '\r';  break;
        case 't':   ch = '\t';  break;
        case 'v':   ch = '\v';  break;

        case 'x':
        case 'u':
        case 'U':
            p--; // Backup so ScanUnicodeEscape can re-read the character
            ch = ScanUnicodeEscape( p, pchSurrogate, FALSE);
            break;

        case '0':
            ch = '\0';
            break;
        default:
            ErrorAtPosition (m_iCurLine, (long)(pszStart - m_pszCurLine), (long)(p - pszStart), ERR_IllegalEscape);
            break;
    }

    return ch;
}




////////////////////////////////////////////////////////////////////////////////
// CLexer::ScanToken
//
// This function scans the next token present in the current input stream, and
// puts the result in the given CSTOKEN.

TOKENID CLexer::ScanToken (CSTOKEN *pToken)
{
    WCHAR       ch, chQuote, chSurrogate = L'\0';
    PCWSTR      p = m_pszCurrent, pszHold = NULL, pszToken;
    BOOL        fReal = FALSE, fEscaped = FALSE, fAtPrefix = FALSE;

    // Initialize for new token scan
    pToken->iChar = pToken->iLine = 0;
    pToken->iUserByte = TID_INVALID;
    pToken->iUserBits = 0;

    // Start scanning the token
    while (pToken->iUserByte == TID_INVALID)
    {
        if (!PositionOf (p, pToken) && !m_fThisLineTooLong)
        {
            ErrorAtPosition (m_iCurLine, MAX_POS_LINE_LEN - 1, 1, ERR_LineTooLong, MAX_POS_LINE_LEN);
            m_fLimitExceeded = TRUE;
            m_fThisLineTooLong = TRUE;
        }

        pszToken = p;

        switch (ch = *p++)
        {
            case 0:
            {
                // Back up to point to the 0 again...
                p--;      
                pToken->iUserByte = TID_ENDFILE;
                pToken->iLength = 0;
                break;
            }

            case '\t':
            case ' ':
            {
                // Tabs and spaces tend to roam in groups... scan them together
                while (*p == ' ' || *p == '\t')
                    p++;
                break;
            }

            case UCH_PS:
            case UCH_LS:
            case 0x0085:
            case '\n':
            {
                // This is a new line
                TrackLine (p);
                break;
            }

            case '\r':
            {
                // Bare CR's are lines, but CRLF pairs are considered a single line.
                if (*p == '\n')
                    p++;
                TrackLine (p);
                break;
            }

            // Other Whitespace characters
            case UCH_BOM:   // Unicode Byte-order marker
            case 0x001A:    // Ctrl+Z
            case '\v':      // Vertical Tab
            case '\f':      // Form-feed
            {
                break;
            }

            case '#':
            {
                p--;
                if (!ScanPreprocessorLine (p))
                {
                    ASSERT(!m_fPreproc);
                    p++;
                    ReportInvalidToken(pToken, pszToken, p);
                }
                break;
            }

            case '\"':
            case '\'':
            {
                CStringBuilder  sb;

                // "Normal" strings (double-quoted and single-quoted (char) literals).  We translate escape sequences
                // here, and construct the STRCONST (for strings) directly (char literals are encoded w/o overhead)
                chQuote = ch;
                while (*p != chQuote)
                {
                    WCHAR   c = *p++;

                    if (c == '\\')
                    {
                        WCHAR c2 = 0;
                        c = ScanEscapeSequence (p, &c2);

                        // We use a string building to construct the string constant's value.  Yes, CStringBuilder
                        // is equipped to deal with embedded nul characters.
                        sb.Append (c);
                        if (c2 != 0)
                            sb.Append (c2);
                    }
                    else if (IsEndOfLineChar (c) || c == 0)
                    {
                        ASSERT (p > pszToken);
                        p--;
                        ErrorAtPosition (m_iCurLine, (long)(pszToken - m_pszCurLine), (long)(p - pszToken), ERR_NewlineInConst);
                        pToken->iUserBits |= TF_UNTERMINATED;
                        break;
                    }
                    else
                    {
                        // We use a string building to construct the string constant's value.  Yes, CStringBuilder
                        // is equipped to deal with embedded nul characters.
                        sb.Append (c);
                    }
                }

                // Skip the terminating quote (if present)
                if ((pToken->iUserBits & TF_UNTERMINATED) == 0)
                    p++;

                if (chQuote == '\'')
                {
                    // This was a char literal -- no need to allocate overhead...
                    if (sb.GetLength() != 1)
                        ErrorAtPosition (m_iCurLine, (long)(pszToken - m_pszCurLine), (long)(p - pszToken), (sb.GetLength() != 0) ? ERR_TooManyCharsInConst : ERR_EmptyCharConst);

                    pToken->iUserByte = TID_CHARLIT;
                    pToken->chr.cCharValue = ((PCWSTR)sb)[0];
                    pToken->chr.iCharLen = (WCHAR)(p - pszToken);
                }
                else
                {
                    // This one requires special allocation.
                    pToken->iUserByte = TID_STRINGLIT;
                    pToken->iUserBits |= TF_OVERHEAD;
                    pToken->pStringLiteral = (STRLITERAL *)TokenMemAlloc (pToken, sizeof (STRLITERAL) + (sb.GetLength() * sizeof (WCHAR)));
                    pToken->pStringLiteral->iSourceLength = (long)(p - pszToken);
                    pToken->pStringLiteral->str.length = (long)sb.GetLength();
                    pToken->pStringLiteral->str.text = (WCHAR *)(pToken->pStringLiteral + 1);
                    memcpy (pToken->pStringLiteral->str.text, (PCWSTR)sb, pToken->pStringLiteral->str.length * sizeof (WCHAR));
                }

                break;
            }

            case '/':
            {
                // Lotsa things start with slash...
                switch (*p)
                {
                    case '/':
                    {
                        // Single-line comments...
                        bool    fDocComment = (p[1] == '/' && p[2] != '/');

                        // Find the end of the line, and make sure it's not too long (even for non-doc comments...)
                        while (*p != 0 && !IsEndOfLineChar (*p)) 
                        {
                            if (p - m_pszCurLine >= MAX_POS_LINE_LEN && !m_fThisLineTooLong)
                            {
                                ErrorAtPosition (m_iCurLine, MAX_POS_LINE_LEN - 1, 1, ERR_LineTooLong, MAX_POS_LINE_LEN);
                                m_fLimitExceeded = TRUE;
                                m_fThisLineTooLong = TRUE;
                            }

                            p++;
                        }

                        // Only put comments in the token stream if asked
                        if (RepresentNoiseTokens ())
                        {                            
                            if (fDocComment)
                            {
                                size_t cchToken = (p - pszToken);
                                size_t cchBuffer = cchToken + 1;
                                size_t cbBuffer = cchBuffer * sizeof(WCHAR);

                                // Doc comments require, ironically enough, overhead in the token stream.
                                pToken->iUserByte = TID_DOCCOMMENT;
                                pToken->iUserBits |= TF_OVERHEAD;
                                pToken->pDocLiteral = (DOCLITERAL *)TokenMemAlloc (pToken, sizeof (DOCLITERAL) + cbBuffer);
                                pToken->pDocLiteral->posEnd = POSDATA(m_iCurLine, (long)(p - m_pszCurLine));
                                wcsncpy_s (pToken->pDocLiteral->szText, cchBuffer, pszToken, cchToken);
                                pToken->pDocLiteral->szText[cchToken] = 0;
                            }
                            else
                            {
                                // No overhead incurred for single-line non-doc comments, but we do need the length.
                                pToken->iUserByte = TID_SLCOMMENT;
                                pToken->iLength = (long)(p - pszToken);
                            }
                        }
                        break;
                    }

                    case '*':
                    {
                        bool    fDocComment = (p[1] == '*' && p[2] != '*');
                        BOOL    fDone = FALSE;

                        // Multi-line comments...
                        p++;
                        while (!fDone)
                        {
                            if (*p == 0)
                            {
                                // The comment didn't end.  Report an error at the start point.
                                ErrorAtPosition (pToken->iLine, pToken->iChar, 2, ERR_OpenEndedComment);
                                if (RepresentNoiseTokens ())
                                    pToken->iUserBits |= TF_UNTERMINATED;
                                fDone = TRUE;
                                break;
                            }

                            if (*p == '*' && p[1] == '/')
                            {
                                p += 2;
                                break;
                            }

                            if (IsEndOfLineChar (*p))
                            {
                                if (*p == '\r' && p[1] == '\n')
                                    p++;
                                TrackLine (++p);
                            }
                            else
                            {
                                p++;
                            }
                        }

                        m_fFirstOnLine = FALSE;

                        if (RepresentNoiseTokens ())
                        {
                            pToken->iUserBits |= TF_OVERHEAD;
                            if (fDocComment)
                            {
                                // Doc comments require, ironically enough, overhead in the token stream.
                                size_t cchToken = (p - pszToken);
                                size_t cchBuffer = cchToken + 1; //+1 for null
                                size_t cbBuffer = cchBuffer * sizeof(WCHAR);

                                pToken->iUserByte = TID_MLDOCCOMMENT;
                                pToken->pDocLiteral = (DOCLITERAL *)TokenMemAlloc (pToken, sizeof (DOCLITERAL) + cbBuffer);
                                pToken->pDocLiteral->posEnd = POSDATA(m_iCurLine, (long)(p - m_pszCurLine));
                                wcsncpy_s (pToken->pDocLiteral->szText, cchBuffer, pszToken, cchToken);
                                pToken->pDocLiteral->szText[cchToken] = 0;
                                if (p - m_pszCurLine >= MAX_POS_LINE_LEN && !m_fThisLineTooLong)
                                {
                                    ErrorAtPosition (m_iCurLine, MAX_POS_LINE_LEN - 1, 1, ERR_LineTooLong, MAX_POS_LINE_LEN);
                                    m_fLimitExceeded = TRUE;
                                    m_fThisLineTooLong = TRUE;
                                }
                            }
                            else
                            {
                                // For multi-line comments, we don't put the text in but we do need the
                                // end position -- which means ML comments incur overhead...  :-(
                                pToken->iUserByte = TID_MLCOMMENT;
                                pToken->pposEnd = (POSDATA *)TokenMemAlloc (pToken, sizeof (POSDATA));
                                if (!PositionOf (p, pToken->pposEnd) && !m_fThisLineTooLong)
                                {
                                    ErrorAtPosition (m_iCurLine, MAX_POS_LINE_LEN - 1, 1, ERR_LineTooLong, MAX_POS_LINE_LEN);
                                    m_fLimitExceeded = TRUE;
                                    m_fThisLineTooLong = TRUE;
                                }
                            }

                        }
                        break;
                    }

                    case '=':
                    {
                        p++;
                        pToken->iUserByte = TID_SLASHEQUAL;
                        pToken->iLength = 2;
                        break;
                    }

                    default:
                    {
                        pToken->iUserByte = TID_SLASH;
                        pToken->iLength = 1;
                        break;
                    }
                }

                break;
            }

            case '.':
            {
                if (*p >= '0' && *p <= '9')
                {
                    p++;
                    ch = 0;
                    goto _parseNumber;
                }
                pToken->iUserByte = TID_DOT;
                pToken->iLength = 1;
                break;
            }

            case ',':
                pToken->iUserByte = TID_COMMA;
                pToken->iLength = 1;
                break;

            case ':':
                if (*p == ':')
                {
                    pToken->iUserByte = TID_COLONCOLON;
                    pToken->iLength = 2;
                    p++;
                }
                else
                {
                    pToken->iUserByte = TID_COLON;
                    pToken->iLength = 1;
                }
                break;

            case ';':
                pToken->iUserByte = TID_SEMICOLON;
                pToken->iLength = 1;
                break;

            case '~':
                pToken->iUserByte = TID_TILDE;
                pToken->iLength = 1;
                break;

            case '!':
            {
                if (*p == '=')
                {
                    pToken->iUserByte = TID_NOTEQUAL;
                    pToken->iLength = 2;
                    p++;
                }
                else
                {
                    pToken->iUserByte = TID_BANG;
                    pToken->iLength = 1;
                }
                break;
            }

            case '=':
            {
                if (*p == '=')
                {
                    pToken->iUserByte = TID_EQUALEQUAL;
                    pToken->iLength = 2;
                    p++;
                }
                else
                {
                    pToken->iUserByte = TID_EQUAL;
                    pToken->iLength = 1;
                }
                break;
            }

            case '*':
            {
                if (*p == '=')
                {
                    pToken->iUserByte = TID_SPLATEQUAL;
                    pToken->iLength = 2;
                    p++;
                }
                else
                {
                    pToken->iUserByte = TID_STAR;
                    pToken->iLength = 1;
                }
                break;
            }

            case '(':
            {
                pToken->iUserByte = TID_OPENPAREN;
                pToken->iLength = 1;
                break;
            }

            case ')':
            {
                pToken->iUserByte = TID_CLOSEPAREN;
                pToken->iLength = 1;
                break;
            }

            case '{':
            {
                pToken->iUserByte = TID_OPENCURLY;
                pToken->iLength = 1;
                break;
            }

            case '}':
            {
                pToken->iUserByte = TID_CLOSECURLY;
                pToken->iLength = 1;
                break;
            }

            case '[':
            {
                pToken->iUserByte = TID_OPENSQUARE;
                pToken->iLength = 1;
                break;
            }

            case ']':
            {
                pToken->iUserByte = TID_CLOSESQUARE;
                pToken->iLength = 1;
                break;
            }

            case '?':
            {
                if (*p == '?')
                {
                    p++;
                    pToken->iUserByte = TID_QUESTQUEST;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_QUESTION;
                    pToken->iLength = 1;
                }
                break;
            }

            case '+':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_PLUSEQUAL;
                    pToken->iLength = 2;
                }
                else if (*p == '+')
                {
                    p++;
                    pToken->iUserByte = TID_PLUSPLUS;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_PLUS;
                    pToken->iLength = 1;
                }
                break;
            }

            case '-':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_MINUSEQUAL;
                    pToken->iLength = 2;
                }
                else if (*p == '-')
                {
                    p++;
                    pToken->iUserByte = TID_MINUSMINUS;
                    pToken->iLength = 2;
                }
                else if (*p == '>')
                {
                    p++;
                    pToken->iUserByte = TID_ARROW;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_MINUS;
                    pToken->iLength = 1;
                }
                break;
            }

            case '%':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_MODEQUAL;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_PERCENT;
                    pToken->iLength = 1;
                }
                break;
            }

            case '&':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_ANDEQUAL;
                    pToken->iLength = 2;
                }
                else if (*p == '&')
                {
                    p++;
                    pToken->iUserByte = TID_LOG_AND;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_AMPERSAND;
                    pToken->iLength = 1;
                }
                break;
            }

            case '^':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_HATEQUAL;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_HAT;
                    pToken->iLength = 1;
                }
                break;
            }

            case '|':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_BAREQUAL;
                    pToken->iLength = 2;
                }
                else if (*p == '|')
                {
                    p++;
                    pToken->iUserByte = TID_LOG_OR;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_BAR;
                    pToken->iLength = 1;
                }
                break;
            }

            case '<':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_LESSEQUAL;
                    pToken->iLength = 2;
                }
                else if (*p == '<')
                {
                    p++;
                    if (*p == '=')
                    {
                        p++;
                        pToken->iUserByte = TID_SHIFTLEFTEQ;
                        pToken->iLength = 3;
                    }
                    else
                    {
                        pToken->iUserByte = TID_SHIFTLEFT;
                        pToken->iLength = 2;
                    }
                }
                else
                {
                    pToken->iUserByte = TID_LESS;
                    pToken->iLength = 1;
                }
                break;
            }

            case '>':
            {
                if (*p == '=')
                {
                    p++;
                    pToken->iUserByte = TID_GREATEREQUAL;
                    pToken->iLength = 2;
                }
                else
                {
                    pToken->iUserByte = TID_GREATER;
                    pToken->iLength = 1;
                }
                break;
            }

            case '@':
            {
                if (*p == '"')
                {
                    CStringBuilder  sb;
                    BOOL            fDone = FALSE;
                    WCHAR           c;

                    // Verbatim string literal.  While scanning/accumulating its value into
                    // the string builder, track lines and ignore escape characters (they don't
                    // apply in VSL's) -- watch for double-quotes as well.
                    p++;
                    while (!fDone)
                    {
                        switch (c = *p++)
                        {
                            case UCH_PS:
                            case UCH_LS:
                            case 0x0085:
                            case '\n':
                            {
                                TrackLine (p);
                                break;
                            }

                            case '\r':
                            {
                                if (*p == '\n')
                                {
                                    sb.Append (c);
                                    c = *p++;
                                }
                                TrackLine (p);
                                break;
                            }

                            case '\"':
                            {
                                if (*p == '\"')
                                    p++;            // Doubled quote -- skip & put the single quote in the string
                                else
                                    fDone = TRUE;
                                break;
                            }

                            case 0:
                            {
                                // Reached the end of the source without finding the end-quote.  Give
                                // an error back at the starting point.
                                ErrorAtPosition (pToken->iLine, pToken->iChar, 2, ERR_UnterminatedStringLit);
                                pToken->iUserBits |= TF_UNTERMINATED;
                                fDone = TRUE;
                                p--;
                                break;
                            }
                            default:
                                ASSERT(!IsEndOfLineChar(c));
                                break;
                        }

                        if (!fDone)
                            sb.Append (c);
                    }

                    pToken->iUserByte = TID_VSLITERAL;
                    pToken->iUserBits |= TF_OVERHEAD;
                    pToken->pVSLiteral = (VSLITERAL *)TokenMemAlloc (pToken, sizeof (VSLITERAL) + (sb.GetLength() * sizeof (WCHAR)));
                    PositionOf (p, &pToken->pVSLiteral->posEnd);
                    pToken->pVSLiteral->str.length = (long)sb.GetLength();
                    pToken->pVSLiteral->str.text = (WCHAR *)(pToken->pVSLiteral + 1);
                    memcpy (pToken->pVSLiteral->str.text, (PCWSTR)sb, sb.GetLength() * sizeof (WCHAR));
                    break;
                }

                // Check for identifiers.  NOTE: unicode escapes are allowed here!
                ch = PeekChar(p, &chSurrogate); 
                if (!IsIdentifierChar (ch)) // BUG 424819 : Handle identifier chars > 0xFFFF via surrogate pairs
                {
                    // After the '@' we have neither an identifier nor and string quote, so assume it is an identifier.
                    CreateInvalidToken(pToken, pszToken, p);
                    ErrorAtPosition (m_iCurLine, (long)(pszToken - m_pszCurLine), (long)(p - pszToken), ERR_ExpectedVerbatimLiteral);
                    break;
                }

                ch = NextChar(p, &chSurrogate);
                fAtPrefix = TRUE;
                goto _ParseIdentifier;  // (Goto avoids the IsSpaceSeparator() check and the redundant IsIdentifierChar() check below...)
            }

            case '\\':
                // Could be unicode escape. Try that.
                --p;
                ch = NextChar (p, &chSurrogate);

                // If we had a unicode escape, ch is it. If we didn't, ch is still a backslash. Unicode escape
                // must start an identifers, so check only for identifiers now.
                goto _CheckIdentifier;

            default:
                ASSERT(!IsEndOfLineChar(ch));
                if (IsSpaceSeparator (ch))    // Unicode class 'Zs'
                {
                    while (IsSpaceSeparator(*p))
                        p++;
                    break;
                }
_CheckIdentifier:
                if (!IsIdentifierChar (ch)) // BUG 424819 : Handle identifier chars > 0xFFFF via surrogate pairs
                {
                    ReportInvalidToken(pToken, pszToken, p);
                    break;
                }
                // Fall through case.  All the 'common' identifier characters are represented directly in
                // these switch cases for optimal perf.  Calling IsIdentifierChar() functions is relatively
                // expensive.
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            case '_':
_ParseIdentifier:
            {
                CStringBuilder  sb;
                bool            doubleUnderscore = false;

                // Remember, because we're processing identifiers here, unicode escape sequences are
                // allowed and must be handled
                sb.Append (ch);
                if (chSurrogate)
                    sb.Append(chSurrogate);

                do
                {
                    ch = PeekChar (p, &chSurrogate);
                    switch (ch)
                    {
                        case '_':
                            // Common identifier character, but we need check for double consecutive underscores
                            if (!doubleUnderscore && ((PWSTR)sb)[sb.GetLength() - 1] == '_')
                                doubleUnderscore = true;
                            break;

                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        {
                            // Again, these are the 'common' identifier characters...
                            break;
                        }
                        case ' ': case '\t': case '.': case ';': case '(': case ')': case ',':
                        {
                            // ...and these are the 'common' stop characters.
                            goto LoopExit;
                        }
                        default:
                        {
                            // This is the 'expensive' call
                            if (IsIdentifierCharOrDigit (ch)) // BUG 424819 : Handle identifier chars > 0xFFFF via surrogate pairs
                            {
                                if (IsOtherFormat (ch))
                                {
                                    goto SkipChar; // Ignore formatting characters
                                }
                            }
                            else
                            {
                                // Not a valid identifier character, so bail.
                                goto LoopExit;
                            }
                        }
                    }
                    sb.Append (ch);
                    if (chSurrogate)
                        sb.Append(chSurrogate);

SkipChar:
                    ch = NextChar (p, &chSurrogate);
                }
                while (ch);

LoopExit:
                HRESULT hr;
                if (!SUCCEEDED(hr = sb.GetResultCode()))
                {
                    m_hr = hr;
                    return TID_INVALID;
                }

                PCWSTR  pszName = sb;
                long    iLength = (long)sb.GetLength();

                // "escaped" means there was an @ prefix, or there was a unicode escape -- both of which
                // indicate overhead, since the identifier length will not be equal to the token length
                fEscaped = (fAtPrefix || (p - pszToken > iLength));

                if (sb.GetLength() >= MAX_IDENT_SIZE)
                {
                    ErrorAtPosition (m_iCurLine, (long)(pszToken - m_pszCurLine), (long)(p - pszToken), ERR_IdentifierTooLong);
                    iLength = MAX_IDENT_SIZE - 1;
                }

                int     iKeyword;

                // Add the identifier to the name table
                pToken->pName = m_pNameMgr->AddString (pszName, iLength);

                // ...and check to see if it is a keyword, if appropriate
                if (fEscaped || !m_pNameMgr->IsNameKeyword (pToken->pName, m_eKeywordMode, &iKeyword))
                {
                    pToken->iUserByte = TID_IDENTIFIER;

                    if (doubleUnderscore && !fAtPrefix && m_eKeywordMode == CompatibilityECMA1) {
                        ErrorAtPosition (m_iCurLine, (long)(pszToken - m_pszCurLine), (long)(p - pszToken), ERR_ReservedIdentifier, pToken->pName->text);
                    }

                    if (fEscaped)
                    {
                        NAME    *pName = pToken->pName;     // Hold this so assignment to pEscName doesn't whack it

                        pToken->iUserBits |= TF_OVERHEAD;
                        pToken->pEscName = (ESCAPEDNAME *)TokenMemAlloc (pToken, sizeof (ESCAPEDNAME));
                        pToken->pEscName->iLen = (long)(p - pszToken);
                        pToken->pEscName->pName = pName;
                    }
                }
                else
                {
                    pToken->iUserByte = iKeyword;
                    pToken->iLength = iLength;
                }
                
                if (fAtPrefix)
                {
                    pToken->iUserBits |= TF_VERBATIMSTRING; // We need to know this later
                }

                break;
            }

            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            {
                BOOL fHexNumber;
                if ((fHexNumber = (ch == '0' && (*p == 'x' || *p == 'X'))))
                {
                    // it's a hex constant
                    p++;

                    // It's OK if it has no digits after the '0x' -- we'll catch it in ScanNumericLiteral
                    // and give a proper error then.
                    while (*p <= 'f' && isxdigit (*p))
                        p++;

                    if (*p == 'L' || *p == 'l')
                    {
                        p++;
                        if (*p == 'u' || *p == 'U')
                            p++;
                    }
                    else if (*p == 'u' || *p == 'U')
                    {
                        p++;
                        if (*p == 'L' || *p == 'l')
                            p++;
                    }
                }
                else
                {
                    // skip digits
                    while (*p >= '0' && *p <= '9')
                        p++;
                    if (*p == '.')
                    {
                        pszHold = p++;
                        if (*p >= '0' && *p <= '9')
                        {
                            // skip digits after decimal point
                            p++;
    _parseNumber:
                            fHexNumber = false;
                            fReal = TRUE;
                            while (*p >= '0' && *p <= '9')
                                p++;
                        }
                        else
                        {
                            // Number + dot + non-digit -- these are separate tokens, so don't absorb the
                            // dot token into the number.
                            p = pszHold;
                            size_t cchToken = (p - pszToken);
                            size_t cchBuffer = cchToken + 1;
                            size_t cbBuffer = cchBuffer * sizeof(WCHAR); 

                            pToken->iUserByte = TID_NUMBER;
                            pToken->iUserBits |= TF_OVERHEAD;
                            pToken->pLiteral = (LITERAL *)TokenMemAlloc (pToken, sizeof (LITERAL) + cbBuffer);
                            pToken->pLiteral->iSourceLength = (long)cchToken;
                            wcsncpy_s (pToken->pLiteral->szText, cchBuffer, pszToken, cchToken);
                            pToken->pLiteral->szText[cchBuffer] = 0;
                            break;
                        }
                    }

                    if (*p == 'E' || *p == 'e')
                    {
                        fReal = TRUE;

                        // skip exponent
                        p++;
                        if (*p == '+' || *p == '-')
                            p++;

                        while (*p >= '0' && *p <= '9')
                            p++;
                    }

                    if (fReal)
                    {
                        if (*p == 'f' || *p == 'F' || *p == 'D' || *p == 'd' || *p == 'm' || *p == 'M')
                            p++;
                    }
                    else if (*p == 'F' || *p == 'f' || *p == 'D' || *p == 'd' || *p == 'm' || *p == 'M')
                    {
                        p++;
                    }
                    else if (*p == 'L' || *p == 'l')
                    {
                        p++;
                        if (*p == 'u' || *p == 'U')
                            p++;
                    }
                    else if (*p == 'u' || *p == 'U')
                    {
                        p++;
                        if (*p == 'L' || *p == 'l')
                            p++;
                    }
                }
                size_t cchToken = (p - pszToken);
                size_t cchBuffer = cchToken + 1;
                size_t cbBuffer = cchBuffer * sizeof (WCHAR);

                pToken->iUserByte = TID_NUMBER;
                pToken->iUserBits |= TF_OVERHEAD;
                if (fHexNumber) 
                    pToken->iUserBits |= TF_HEXLITERAL;
                pToken->pLiteral = (LITERAL *)TokenMemAlloc (pToken, sizeof (LITERAL) + cbBuffer); 
                pToken->pLiteral->iSourceLength = (long)(cchToken);
                wcsncpy_s (pToken->pLiteral->szText, cchBuffer, pszToken, cchToken);
                pToken->pLiteral->szText[cchToken] = 0;
                break;
            }
        } // switch
    } // while

    m_pszCurrent = p;
    m_fFirstOnLine = FALSE;
    if (!m_fTokensSeen)
        m_fTokensSeen = ((CParser::m_rgTokenInfo[pToken->Token()].dwFlags & TFF_NOISE) == 0);
    return pToken->Token();
}

void CLexer::CreateInvalidToken(CSTOKEN *pToken, PCWSTR pszTokenStart, PCWSTR pszTokenEnd)
{
    pToken->iUserByte = TID_UNKNOWN;
    pToken->pName = m_pNameMgr->AddString (pszTokenStart, (long)(pszTokenEnd-pszTokenStart));
}

void CLexer::ReportInvalidToken(CSTOKEN *pToken, PCWSTR pszTokenStart, PCWSTR pszTokenEnd)
{
    CreateInvalidToken(pToken, pszTokenStart, pszTokenEnd);
    ErrorAtPosition (m_iCurLine, (long)(pszTokenStart - m_pszCurLine), (long)(pszTokenEnd - pszTokenStart), ERR_UnexpectedCharacter, pToken->pName);
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::~CTextLexer

CTextLexer::~CTextLexer ()
{
    if (m_pNameMgr != NULL)
        m_pNameMgr->Release();

    if (m_pszInput != NULL)
        VSFree (m_pszInput);

    for (long i=0; i<m_arrayTokens.Count(); i++)
    {
        if (m_arrayTokens[i].HasOverhead()) {
            VSFree (m_arrayTokens[i].Overhead());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::CreateInstance

HRESULT CTextLexer::CreateInstance (ICSNameTable *pNameTable, CompatibilityMode eKeywordMode, ICSLexer **ppLexer)
{
    HRESULT                 hr = E_OUTOFMEMORY;
    CComObject<CTextLexer>  *pObj;

    if (SUCCEEDED (hr = CComObject<CTextLexer>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Initialize (pNameTable, eKeywordMode)) ||
            FAILED (hr = pObj->QueryInterface (IID_ICSLexer, (void **)ppLexer)))
        {
            delete pObj;
        }
    }
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::Initialize

HRESULT CTextLexer::Initialize (ICSNameTable *pNameTable, CompatibilityMode eKeywordMode)
{
    m_eKeywordMode = eKeywordMode;
    return pNameTable->QueryInterface (IID_ICSPrivateNameTable, (void **)&m_pNameMgr);
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::SetInput

STDMETHODIMP CTextLexer::SetInput (PCWSTR pszInput, long iLen)
{
    HRESULT     hr;

    // Release anything we already have
    if (m_pszInput != NULL)
        VSFree (m_pszInput);
    for (long i=0; i<m_arrayTokens.Count(); i++)
    {
        if (m_arrayTokens[i].HasOverhead()) {
            VSFree (m_arrayTokens[i].Overhead());
        }
    }
    m_arrayTokens.ClearAll();
    m_arrayLines.ClearAll();
    m_fTokenized = FALSE;
    m_hr = S_OK;

    // If iLen is -1, use the null-terminated length
    if (iLen == -1)
        iLen = (long)wcslen (pszInput);

    // Copy the text, ensuring that it's null-terminated
    m_pszInput = (PWSTR)VSAlloc ((iLen + 1) * sizeof (WCHAR));
    if (m_pszInput == NULL)
        return E_OUTOFMEMORY;

    memcpy (m_pszInput, pszInput, iLen * sizeof (WCHAR));
    m_pszInput[iLen] = 0;

    long    *piLine;

    // Establish the first entry in the line table
    if (FAILED (hr = m_arrayLines.Add (NULL, &piLine)))
        return hr;

    *piLine = 0;

    // Start the input stream for tokenization
    m_pszCurrent = m_pszCurLine = m_pszInput;
    m_iCurLine = 0;

    TOKENID     tok = TID_INVALID;
    CSTOKEN     *pToken;

    // Scan the text
    while (tok != TID_ENDFILE && !FAILED (m_hr))
    {
        if (FAILED (hr = m_arrayTokens.Add (NULL, &pToken)))
            return hr;

        tok = ScanToken (pToken);
    }

    // That's it!  If we didn't fail, signal ourselves as being tokenized
    if (!FAILED (m_hr))
        m_fTokenized = TRUE;

    return m_hr;
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::GetLexResults

HRESULT CTextLexer::GetLexResults (LEXDATA *pLexData)
{
    // Must be tokenized
    if (!m_fTokenized)
        return E_FAIL;

    // Just refer to the arrays accumulated in SetInput
    pLexData->InitTokens (m_arrayTokens.Base(), m_arrayTokens.Count());
    pLexData->InitSource(m_pszInput);
    pLexData->InitLineOffsets(m_arrayLines.Base(), m_arrayLines.Count());

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::RescanToken

STDMETHODIMP CTextLexer::RescanToken (long iToken, TOKENDATA *pTokenData)
{
    /*
    // Must be tokenized
    if (!m_fTokenized)
        return E_FAIL;

    // Make sure the token index given is valid...
    if (iToken < 0 || iToken >= m_arrayTokens.Count())
        return E_INVALIDARG;

    FULLTOKEN   tok;
    POSDATA     *pposToken = m_arrayTokens.GetAt (iToken);

    // Set the current pointer to the beginning of the token
    m_pszCurLine = m_pszInput + m_arrayLines[pposToken->iLine];
    m_pszCurrent = m_pszCurLine + pposToken->iChar;

    // Scan it and return the token data
    ScanToken (&tok);
    *pTokenData = tok;
    return S_OK;
    */
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::TrackLine

void CTextLexer::TrackLine (PCWSTR pszLine)
{
    // We only update the line table here if !m_fTokenized (which means we're
    // currently doing the initial tokenization of new input).
    if (!m_fTokenized)
    {
        long    *piLine;

        if (SUCCEEDED (m_hr = m_arrayLines.Add (NULL, &piLine)))
            *piLine = (long)(pszLine - m_pszInput);
    }

    m_pszCurLine = pszLine;
    m_iCurLine++;

    CLexer::TrackLine(pszLine);
}

////////////////////////////////////////////////////////////////////////////////
// CTextLexer::TokenMemAlloc



void *CTextLexer::TokenMemAlloc (CSTOKEN *pToken, size_t iSize)
{
    // Each token for which extra space is allocated must be (is) placed in our
    // array of tokens, which we are responsible for cleaning up during our
    // destruction.
    return VSAlloc (iSize);
}



////////////////////////////////////////////////////////////////////////////////
// CScanLexer::~CScanLexer

CScanLexer::~CScanLexer ()
{
    if (m_pTokenMem != NULL)
        VSFree (m_pTokenMem);
}

////////////////////////////////////////////////////////////////////////////////
// CScanLexer::TokenMemAlloc

void *CScanLexer::TokenMemAlloc (CSTOKEN *pToken, size_t iSize)
{
    if (SizeAdd(iSize, m_iMemUsed) > (size_t)m_iMemSize)
    {
        size_t  iRoundSize = SizeAdd(m_iMemUsed + iSize, 0x100) & ~(size_t)0xff; // Hard-coded growth by 256 byte blocks
        void    *pMem = (m_pTokenMem == NULL) ? VSAlloc (iRoundSize) : VSRealloc (m_pTokenMem, iRoundSize);

        if (pMem == NULL)
            return NULL;

        m_pTokenMem = pMem;
        m_iMemSize = iRoundSize;
    }

    ASSERT (SizeAdd(m_iMemUsed, iSize) <= (size_t)m_iMemSize);
    void    *pRet = ((BYTE *)m_pTokenMem + m_iMemUsed);
    m_iMemUsed += iSize;
    return pRet;
}



