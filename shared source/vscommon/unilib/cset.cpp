// cset.cpp - Get the codepage by parsing the HTML charset tag.
//
// We need to do this with both (A)nsi and (U)nicode, so this file is designed to be
// #included by another module twice, with #define _WIDE_ 0 and 1.
// 
#include <stdio.h>
#if !defined(_M_CEE)
#pragma intrinsic (memset, strlen, strcmp)
#endif
#include "strsafe.h"

//$

// Once-only info in this block, not differing in A/W
#ifndef _HTMLCHARSET_
#define _HTMLCHARSET_

//-----------------------------------------------------------------
typedef enum 
{
    hi_Error = -1, hi_Unknown, hi_Eof, hi_Eol, hi_Text, hi_Entity, 
    hi_TagUnknown, 
    hi_TagSpecial, // ! tag or processing instruction (? tag)
    hi_TagFirst, _A = hi_TagFirst,
    _ADDRESS,   _APPLET,    _AREA,      _B,         _BASE,      _BASEFONT,  _BGSOUND,   
    _BIG,       _BLINK,     _BLOCKQUOTE,_BODY,      _BR,        _BUTTON,
    _CAPTION,   _CENTER,    _CITE,      _CODE,      _COL,       _COLGROUP,  _COMMENT,
    _DD,        _DFN,       _DIR,       _DIV,       _DL,        _DT,
    _EM,        _EMBED,     _FIELDSET,  _FONT,      _FORM,      _FRAME,     _FRAMESET,
    _H1,        _H2,        _H3,        _H4,        _H5,        _H6,        _HEAD,
    _HR,        _HTML,      _I,         _IFRAME,    _IMG,       _INPUT,     _ISINDEX,
    _KBD,       _LABEL,     _LEGEND,    _LI,        _LINK,      _LISTING,   
    _MAP,       _MARQUEE,   _MENU,      _META,      _METADATA,  _NOBR,      _NOFRAMES,  _NOSCRIPT,
    _OBJECT,    _OL,        _OPTION,    _P,         _PARAM,     _PLAINTEXT, _PRE,
    _S,         _SAMP,      _SCRIPT,    _SELECT,    _SMALL,     _SPAN,      _STRIKE,    
    _STRONG,    _STYLE,     _SUB,       _SUP,
    _TABLE,     _TBODY,     _TD,        _TEXTAREA,  _TFOOT,     _TH,        _THEAD, 
    _TITLE,     _TR,        _TT,        _U,         _UL,        _VAR,       _WBR,    _XMP,
    hi_TagMax  
} HTML_ITEM;
const int cchMaxTag    = 10;
const int cchMaxEntity = 6;

#endif // _HTMLCHARSET_ : once-only stuff

#undef SKIPWHITE_RET
#undef SKIPWHITE_FAIL
#undef strcchcopy
#undef icmp
#undef icmpn
#undef slen
#undef CH
#undef _CH
#undef PCCHSTR
#undef CBCH
#undef HTOK
#undef ScanCharSet  
#undef ScanItem     
#undef LookupElement
#undef CPFromCharSet
#undef HTOK
#undef HATT

//-----------------------------------------------------------------
#if (_WIDE_ == 1)

#define strcchcopy  StringCchCopyW
#define icmp     _wcsicmp
#define icmpn    _wcsnicmp
#define slen     StrLen
#define CH       WCHAR
#define _CH(x)   L ## x
#define CBCH     2
#define PCCHSTR  PCWSTR

typedef struct _HTOKW
{
    HTML_ITEM id;   // kind of item
    CH      * pch;  // text
    int       cch;  // extent
    bool      bEnd; // end tag
} HTOKW;
#define HTOK HTOKW

typedef struct _HATTW
{
    const CH * pchName;  // Name
    int        cchName;  // extent
    const CH * pchValue; // Value
    int        cchValue; // extent
} HATTW;
#define HATT HATTW

#undef ScanHTMLCharSetName
#undef ScanHTMLCharSet
#undef ScanItem     
#undef LookupElement
#undef CPFromCharSet

#define ScanHTMLCharSetName ScanHTMLCharSetNameW
#define ScanHTMLCharSet     ScanHTMLCharSetW
#define ScanItem            ScanItemW
#define LookupElement       LookupElementW
#define CPFromCharSet       CPFromCharSetW

#define SKIPWHITE_RET \
    while ((pchScan < pchEnd) && (*pchScan <= 32)) \
        pchScan++;                                 \
    if (pchScan >= pchEnd)                         \
        return;

#define SKIPWHITE_FAIL \
    while ((pchScan < pchEnd) && (*pchScan <= 32)) \
        pchScan++;                                 \
    if (pchScan >= pchEnd)                         \
        return E_FAIL;

//-----------------------------------------------------------------
#elif (_WIDE_ == 0)
#define strcchcopy(a,n,b) StringCchCopyA((char *)a, n, (const char *)b)
#define icmp(a,b)      _stricmp((const char *)a,(const char *)b)
#define icmpn(a,b,n)   _strnicmp((const char *)a,(const char *)b, n)
#define slen(x)        StrLenA((const char *)x)
#define CH       unsigned char
#define PCCHSTR  PCSTR
#define _CH(x)   x
#define CBCH     1
typedef struct _HTOKA
{
    HTML_ITEM id;   // kind of item
    CH      * pch;  // text
    int       cch;  // extent
    bool      bEnd; // end tag
} HTOKA;
#define HTOK HTOKA

typedef struct _HATTA
{
    const CH * pchName;  // Name
    int        cchName;  // extent
    const CH * pchValue; // Value
    int        cchValue; // extent
} HATTA;
#define HATT HATTA

#undef ScanHTMLCharSetName
#undef ScanHTMLCharSet
#undef ScanItem     
#undef LookupElement
#undef CPFromCharSet

#define ScanHTMLCharSetName ScanHTMLCharSetNameA
#define ScanHTMLCharSet     ScanHTMLCharSetA
#define ScanItem            ScanItemA
#define LookupElement       LookupElementA
#define CPFromCharSet       CPFromCharSetA

#define SKIPWHITE_RET \
    while ((pchScan < pchEnd) && (*pchScan <= 32)) \
        pchScan++;                                 \
    if (pchScan >= pchEnd)                         \
        return;

#define SKIPWHITE_FAIL \
    while ((pchScan < pchEnd) && (*pchScan <= 32)) pchScan++; \
    if (pchScan >= pchEnd) return E_FAIL;

//-----------------------------------------------------------------
#else
#error "You must define _WIDE_ as 1 or 0 before building this file"
#endif


//-----------------------------------------------------------------
inline bool IsADigit(CH c) { return (_CH('0') <= c && c <= _CH('9')); }
inline bool IsAAlpha(CH c) { return (_CH('a') <= c && c <= _CH('z')) || (_CH('A') <= c && c <= _CH('Z')); }
inline bool IsAAlNum(CH c) { return (_CH('a') <= c && c <= _CH('z')) || (_CH('A') <= c && c <= _CH('Z')) || (_CH('0') <= c && c <= _CH('9')); }
inline bool IsACSChar(CH c)
{ 
    return IsAAlNum(c) 
        || (_CH('_') == c)
        || (_CH('-') == c)
        || (_CH(':') == c)
        || (_CH('.') == c);
}

//-----------------------------------------------------------------
static HTML_ITEM LookupElement (const CH* szName)
{
    if (0 == icmp(szName, _CH("HEAD"))) return _HEAD;
    if (0 == icmp(szName, _CH("META"))) return _META;
    if (0 == icmp(szName, _CH("BODY"))) return _BODY;
    return hi_TagUnknown;
}

static HRESULT ScanName( const CH * pchStart, const CH * pchEnd, const CH ** ppch, int * pcch)
{
    if(pchStart == NULL)
        return E_INVALIDARG;

    const CH * pchScan = pchStart;
    const CH * pchName = 0;

    if (ppch) *ppch = 0;
    if (pcch) *pcch = 0;
    
    SKIPWHITE_FAIL
    
    if (!IsAAlpha(*pchScan)) return E_FAIL;
    pchName = pchScan++;
    while ((pchScan < pchEnd) && (IsAAlNum(*pchScan) || (_CH('-') == *pchScan) || (_CH(':') == *pchScan)))
        pchScan++;
    if (pchScan >= pchEnd) return E_FAIL;

    if (ppch) *ppch = pchName;
    if (pcch) *pcch = (int)(pchScan - pchName);
    return S_OK;
}

static HRESULT ScanValue(const CH * pchStart, const CH * pchEnd, const CH ** ppch, int * pcch)
{
    if(pchStart == NULL)
        return E_INVALIDARG;

    const CH * pchScan = pchStart;
    const CH * pch = 0;
    
    if (ppch) *ppch = 0;
    if (pcch) *pcch = 0;
    
    SKIPWHITE_FAIL

    pch = pchScan;
    switch(*pchScan)
    {
    case _CH('"'):
        pchScan++;
        while ((pchScan < pchEnd) && *pchScan != '"')
            pchScan++;
        pchScan++;
        break;
    case _CH('\''):
        pchScan++;
        while ((pchScan < pchEnd) && *pchScan != '\'')
            pchScan++;
        pchScan++;
        break;
    default:
        while ((pchScan < pchEnd) && (*pchScan > 32) && *pchScan != '>')
            pchScan++;
    }
    if (pchScan >= pchEnd) return E_FAIL;
    
    if (ppch) *ppch = pch;
    if (pcch) *pcch = (int)(pchScan - pch);
    return S_OK;
}

static HRESULT ScanComment(const CH * pchStart, const CH * pchEnd, const CH ** ppch, int * pcch)
{
    if(pchStart == NULL)
        return E_INVALIDARG;

    const CH * pchScan = pchStart;
    const CH * pch = 0;
    
    if (ppch) *ppch = 0;
    if (pcch) *pcch = 0;
    
    SKIPWHITE_FAIL
    
    // leading --
    if (_CH('-') != *pchScan) return E_FAIL;
    pch = pchScan++;
    if (pchScan >= pchEnd) return E_FAIL;
    if (_CH('-') != *pchScan++) return E_FAIL;

    // look for trailing --
    for (;;)
    {
        while (pchScan < pchEnd && _CH('-') != *pchScan)
            pchScan++;
        if (++pchScan >= pchEnd) return E_FAIL;
        if (_CH('-') == *pchScan++)
            break;
    }
    
    if (ppch) *ppch = pch;
    if (pcch) *pcch = (int)(pchScan - pch);
    return S_OK;
}

//-----------------------------------------------------------------
static HRESULT ScanAttribute( const CH * pchStart, const CH * pchEnd, HATT * pAtt)
{
    if(pchStart == NULL)
        return E_INVALIDARG;

    const CH * pch;
    int        cch;
    const CH * pchScan  = pchStart;
    HRESULT hr;

    if (pAtt) memset(pAtt, 0, sizeof(HATT));

    SKIPWHITE_FAIL

    switch (*pchScan)
    {
    case _CH('>'): return S_FALSE; // end of tag -- no attribute

    case _CH('/'):
        // End of empty element (e.g. "<foo ... />").  No more attributes.
        return S_FALSE;

    case _CH('-'): // comment
        if (pAtt)
            return ScanComment(pchScan, pchEnd, &pAtt->pchValue, &pAtt->cchValue);
        else
            return ScanComment(pchScan, pchEnd, 0, 0);
        break;
    }

    // scan Name
    hr = ScanName(pchScan, pchEnd, &pch, &cch);
    if (FAILED(hr)) return hr;
    if (pAtt)
    {
        pAtt->pchName = pch;
        pAtt->cchName = cch;
    }
    pchScan = pch + cch;    
    SKIPWHITE_FAIL

    if (*pchScan != _CH('=')) 
        return hr; 
    pchScan++;
    
    SKIPWHITE_FAIL

    hr = ScanValue(pchScan, pchEnd, &pch, &cch);
    if (SUCCEEDED(hr) && pAtt)
    {
        pAtt->pchValue = pch;
        pAtt->cchValue = cch;
    }
    return hr;
}

//-----------------------------------------------------------------
// FindAttribute - Find a specific attribute
//
// Returns:
//   S_OK     Found Attribute
//   S_FALSE  Attribute not found
//   E_FAIL   Syntax error
//
static HRESULT FindAttribute (const CH * pchTag, const CH *pchEnd, const CH * pszAName, const CH **ppchVal, int * pcchVal)
{
    if(pchTag == NULL || pszAName == NULL || *pszAName == NULL)
    {
        VSASSERT(false,"");
        return E_INVALIDARG;
    }
    
    HRESULT    hr;
    const CH * pch;
    int        cch;
    int        cchAttIn = slen(pszAName);
    const CH * pchScan = pchTag;
    const CH * pchVal  = 0;
    
    *ppchVal = 0;
    *pcchVal = 0;

    if (*pchScan == _CH('<'))
        pchScan++;

    // don't look in ! tags or end tags
    if (*pchScan == _CH('!') || *pchScan == _CH('/') || *pchScan == _CH('?'))
        return S_FALSE;

    SKIPWHITE_FAIL

    hr = ScanName(pchScan, pchEnd, &pch, &cch);
    if (FAILED(hr))
        return E_FAIL;
    pchScan = pch + cch;
    if (pchScan >= pchEnd)
        return S_FALSE;  // no attributes

    for(;;)
    {
        HATT    hatt;
        HRESULT hr = ScanAttribute(pchScan, pchEnd, &hatt);
        if (FAILED(hr))
            return E_FAIL;
        if (S_FALSE == hr)
            break;
        pchScan = hatt.pchValue ? hatt.pchValue + hatt.cchValue : hatt.pchName + hatt.cchName;
        if (!hatt.pchName || !hatt.cchName)
            continue; // comment

        if ((cchAttIn == hatt.cchName) && (0 == icmpn(hatt.pchName, pszAName, cchAttIn)))        
        {
            /// strip quotes, if any
            CH ch = *hatt.pchValue;
            if (ch == _CH('"') || ch == _CH('\''))
            {
                VSASSERT(ch == hatt.pchValue[hatt.cchValue-1],"");
                hatt.pchValue++;
                hatt.cchValue -= 2;
            }
            // done!
            *ppchVal = hatt.pchValue;
            *pcchVal = hatt.cchValue;
            return S_OK;
        }
    }
    return S_FALSE;
}

//-----------------------------------------------------------------
static void ScanItem (const CH * pchStart, const CH * pchEnd, HTOK * pk)
{
    if(pchStart == NULL || pk == NULL)
    {
        VSASSERT(false,"");
        return;
    }
    
    HRESULT hr;

    pk->id     = hi_Unknown;
    pk->pch    = (CH*)pchStart;
    pk->cch    = 0;
    pk->bEnd   = false;

    if (pchStart >= pchEnd)
    {
        pk->id = hi_Eof;
        return;
    }
    const CH * pchScan = pchStart;
    
    switch (*pchScan)
    {
//  case _CH(0):
//      pk->id = hi_Eof;
//      break;
//
//  case _CH('\n'):
//  case _CH('\r'):
//      pchScan = AdvanceLineBreak((CH*)pchScan);
//      pk->id = hi_Eol;
//      break;
    
    case _CH('<'):
        {
            const CH * pchName;
            int cch;
            
            pchScan++;          
            pk->id = hi_Error;
            switch (*pchScan)
            {
            case _CH('!'):
                pk->id = hi_Error;
                pchScan++;
                while (_CH('>') != *pchScan)
                {
                    const CH * pch;
                    int        cch;

                    SKIPWHITE_RET

                    if (IsAAlpha(*pchScan))
                        hr = ScanName(pchScan, pchEnd, &pch, &cch);
                    else
                    {
                        switch(*pchScan)
                        {
                        case CH('\''):
                        case CH('"'):
                            hr = ScanValue(pchScan, pchEnd, &pch, &cch);
                            break;

                        case CH('-'):
                            hr = ScanComment(pchScan, pchEnd, &pch, &cch);
                            break;
                        
                        default:
                            hr = S_OK;
                            pch = pchScan;
                            cch = 1;
                            break;
                        }
                    }

                    pchScan = pch + cch;
                    if (FAILED(hr) || (pchScan >= pchEnd))
                        return;
                }
                pk->id = hi_TagSpecial;
                break;

            case _CH('%'):  // ASP 'tag'
                pk->id = hi_TagSpecial;
                do {
                    do {
                        pchScan++;
                    } while ((pchScan < pchEnd) && (_CH('>') != *pchScan));
                } while ((pchScan < pchEnd) &&  (*(pchScan-1) != _CH('%')));
                break;

            case _CH('/'):
                pk->bEnd = true;
                pchScan++;
                break;

            case _CH('?'):  //
                pk->id = hi_TagSpecial;
                pchScan++;
                while ((pchScan < pchEnd) && (_CH('>') != *pchScan))
                    pchScan++;
                break;

            }
            if (pk->id != hi_TagSpecial)
            {
                pk->id = hi_Error;
                SKIPWHITE_RET
                if (!IsAAlpha(*pchScan))
                    return;
                pchName = pchScan;
                while ((pchScan < pchEnd) && IsAAlNum(*pchScan))
                    pchScan++;
                if (pchScan >= pchEnd)
                    return;
                cch = (int)(pchScan - pchName);
                if (cch <= cchMaxTag && cch > 0)
                {
                    CH * pszName = (CH*)_alloca((cch+1)*CBCH );
                    strcchcopy(pszName, cch+1, pchName);
                    pk->id = LookupElement(pszName);
                }
                else
                    return;
                if (!pk->bEnd)
                {
                    // scan attributes and comments
                    for(;;)
                    {
                        HATT    hatt;
                        HRESULT hr = ScanAttribute(pchScan, pchEnd, &hatt);
                        if (FAILED(hr))
                        {
                            pk->id = hi_Error;
                            return;
                        }
                        if (S_FALSE == hr)
                            break;
                        pchScan = hatt.pchValue ? hatt.pchValue + hatt.cchValue : hatt.pchName + hatt.cchName;
                    }
                }
            }
            // Skip over whitespace and, if this is an empty element, the closing '/' (e.g. "<foo ... />")
            while ((pchScan < pchEnd) && ((*pchScan <= 32) || (*pchScan == _CH('/'))))
                pchScan++;
            if ((pchScan >= pchEnd) || (_CH('>') != *pchScan))
                pk->id = hi_Error;
            pchScan++;
        }
        break;

//  case _CH('&'):
//      pk->id = hi_Entity;
//      if (!ScanEntity(pchScan, &pk->ch, &pchScan))
//          goto L_Text;
//      break;
    
    default:
//L_Text:
        pk->id = hi_Text;
        for (bool f = true; f && (pchScan < pchEnd); )
        {
            switch (*pchScan)
            {
//          case _CH(0):
//          case _CH('\n'):
//          case _CH('\r'):
//          case _CH('&'):
            case _CH('<'):
                f = false;
                break;
            default:
                pchScan++;
                break;
            }
        }
        break;
    }
    pk->cch  = (int)(pchScan - pchStart);
    return;
}


//-----------------------------------------------------------------
HRESULT ScanHTMLCharSetName (PCCHSTR pData, int cch, PCCHSTR * ppszCharset, int * pcchName)
{
    if (!ppszCharset || !pcchName) return E_POINTER;
    *ppszCharset = 0;
    *pcchName = 0;

    HRESULT    hr      = S_FALSE;
    UINT       cp      = CP_ACP;
    const CH * pEnd    = (const CH *)pData + cch;
    const CH * pchScan = (const CH *)pData;
    HTOK       tok;
    bool       bHead   = false;

    tok.id = hi_Unknown;
    while (hi_Eof != tok.id)
    {
        ScanItem(pchScan, pEnd, &tok);
        switch (tok.id)
        {
        case _META:
            if (bHead)
            {
                INT        cchVal;
                const CH * pVal = NULL;
                hr = FindAttribute(pchScan, pchScan + tok.cch, (const CH *)_CH("http-equiv"), &pVal, &cchVal);
                if (FAILED(hr))
                {
                    hr = E_FAIL;
                    goto _Exit;
                }
                if (S_FALSE == hr)
                    break;
                for (; cchVal > 0 && *pVal <= 32; pVal++, cchVal--)
                    ;
                if ((cchVal < 12) || (0 != icmpn(pVal, _CH("Content-Type"), 12)))
                    break;

                hr = FindAttribute(pchScan, pchScan + tok.cch, (const CH *)_CH("content"), &pVal, &cchVal);
                if (FAILED(hr) || (S_FALSE == hr))
                    break;
                if (cchVal < 21)
                    break;
                // parse out the charset name
                {
                  const CH * pch    = pVal;
                  const CH * pchEnd = pVal + cchVal;
                  // skip whitespace
                  while ((pch < pchEnd) && (*pch <= 32)) pch++; if (pch >= pchEnd) break;
                  // expect "text/html"
                  if (0 != icmpn(_CH("text/html"), pch, 9))
                      if(0 != icmpn(_CH("text-html"), pch, 9)) 
                          break; 
                  pch += 9;
                  // skip whitespace
                  while ((pch < pchEnd) && (*pch <= 32)) pch++; if (pch >= pchEnd) break;
                  // expect ';'
                  if (*pch++ != _CH(';')) break;
                  // skip whitespace
                  while ((pch < pchEnd) && (*pch <= 32)) pch++; if (pch >= pchEnd) break;
                  // expect "charset"
                  if (0 != icmpn(_CH("charset"), pch, 7)) break; pch += 7;
                  // skip whitespace
                  while ((pch < pchEnd) && (*pch <= 32)) pch++; if (pch >= pchEnd) break;
                  // expect '='
                  if (*pch++ != _CH('=')) break;
                  // skip whitespace
                  while ((pch < pchEnd) && (*pch <= 32)) pch++; if (pch >= pchEnd) break;
                  // found it!
                  *ppszCharset = (PCCHSTR)pch;
                  // calculate length
                  const CH * pchStart = pch; while (IsACSChar(*pch)) pch++;
                  *pcchName = (int)(pch - pchStart);
                  hr = S_OK;
                }
                goto _Exit;
            }
            break;
        case _HEAD:
            if (tok.bEnd) { hr = S_FALSE; goto _Exit; }
            bHead = true;
            break;
        case _BODY: hr = S_FALSE; goto _Exit;
        case hi_Error: hr = E_FAIL; goto _Exit;
        }
        pchScan += tok.cch;
    }
_Exit:
    return hr;
}


//-----------------------------------------------------------------
HRESULT ScanHTMLCharSet (PCCHSTR pData, int cch, UINT * pCp)
{
    if (!pCp) return E_POINTER;
    *pCp = 0;
    PCCHSTR pchName;
    HRESULT hr = ScanHTMLCharSetName (pData, cch, &pchName, & cch);
    if (S_OK == hr)
    {
#if (_WIDE_ == 1)
        hr = CodepageFromCharsetNameW (pchName, cch, pCp);
#else
        hr = CodepageFromCharsetNameA (pchName, cch, pCp);
#endif
    }
    return hr;
}
