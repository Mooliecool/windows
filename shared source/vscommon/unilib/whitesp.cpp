// whitesp.cpp
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//-----------------------------------------------------------------

#include "pch.h"
#include "unichar.h"
#include "unistr.h"
#include "uniprop.h"

void WINAPI TrimWhitespace(_Inout_opt_z_ WCHAR * sz)
{
    if (sz)
    {
        WCHAR*  pszFirstNonWS = sz;
        while (IsWhitespace(*pszFirstNonWS))
            pszFirstNonWS++;

        WCHAR*  pszAfterLastNonWS = pszFirstNonWS + wcslen(pszFirstNonWS); //Now points to the 0-terminator
        while ((--pszAfterLastNonWS >= pszFirstNonWS) && IsWhitespace(*pszAfterLastNonWS))
            ;   //Intentionally empty
        pszAfterLastNonWS++;   //Now points to the first character after the last non-whitespace character

        UASSERT(sz <= pszFirstNonWS);
        UASSERT(pszFirstNonWS <= pszAfterLastNonWS);
        UASSERT(pszAfterLastNonWS <= sz + wcslen(sz));

        if (pszFirstNonWS > sz)
        {
            //Eliminate leading whitespace by shifting the string back
            memmove(sz, pszFirstNonWS, sizeof(WCHAR) * (pszAfterLastNonWS - pszFirstNonWS));

            //Shift trailing pointer to the new end of string
            pszAfterLastNonWS -= (pszFirstNonWS - sz);
        }

        *pszAfterLastNonWS = L'\0';     //zero-terminate
    }
}

//-----------------------------------------------------------------
StripBlanksFlags WINAPI StripBlanks( _Inout_z_cap_(*plen+1) _Prepost_count_(*plen) WCHAR * pchBuf, unsigned int * plen, int flags /*= STB_NONE*/)
{
    StripBlanksFlags type = STB_NONE;
    WCHAR * pchScan;
    WCHAR * pchPoke;
    WCHAR * pchEnd;
    typedef BOOL (WINAPI * CompareFnPtr)(WCHAR ch);
    CompareFnPtr CompareFn;
 
    if (flags & STB_STRIP_LINEBREAKS) // if line breaks to be stripped as well
        CompareFn = IsWhitespace;
    else // just tabs/spaces
        CompareFn = IsBlank;
 
    pchEnd = pchScan = pchPoke = pchBuf;
    pchEnd += *plen;

    //Should be the location of the 0-terminator (which we assume exists).
    UASSERT(*pchEnd == L'\0');

    while (pchScan < pchEnd)
    {
        if (CompareFn(*pchScan))
        {
            if (flags & (STB_SINGLE))
                *pchPoke++ = UCH_SPACE;
            type = max(type, STB_SINGLE);
            pchScan++;
            if (pchScan >= pchEnd)
                break;
            if (CompareFn(*pchScan))
            {
                type = STB_MULTIPLE;
                do {
                    pchScan++;
                } while ((pchScan < pchEnd) && CompareFn(*pchScan));
                if (pchScan < pchEnd)
                    *pchPoke++ = *pchScan++;
            }
        }
        else
            *pchPoke++ = *pchScan++;
    }

    if (pchPoke < pchEnd)
    {
        *pchPoke = UCH_NULL;
    }
    *plen = (unsigned int)(pchPoke - pchBuf);
    return type;
}


////////////////////////////////////////////////////////////////////////////////////
// TrimLineBlanks - Remove blanks adjacent to line ends within a buffer.
//
// Set TLB_START in dwFlags to also remove blanks at the start of the buffer.
// Set TLB_END in dwFlags to also remove blanks at the end of the buffer.
//
// This function does NOT null terminate the passed in string
// Callers need to null terminate as junk may left behind.
// Null termination should be done at the location returned by this function
unsigned int WINAPI TrimLineBlanks( _Inout_ _Pre_count_(cch) _Post_count_(return) PWSTR pchBuf, unsigned int cch, DWORD dwFlags)
{
    PWSTR pchScan;
    PWSTR pchPoke;

    if (cch == 0)
        return 0;

    //
    // Replace blanks to remove with UCH_NONCHAR
    //

    // Leading blanks
    //
    if (dwFlags & TLB_START)
    {
        for (pchScan = pchBuf; IsBlank(*pchScan); pchScan++)
            *pchScan = UCH_NONCHAR;
    }

    // Trailing blanks
    //
    if (dwFlags & TLB_END)
    {
        for (pchScan = pchBuf + cch-1; pchScan >= pchBuf && IsBlank(*pchScan); pchScan--)
        {
            *pchScan = UCH_NONCHAR;
        }
    }

    // Blanks adjacent to line ends in the interior
    //
    pchScan = pchBuf;
    while ((pchScan < pchBuf + cch) && 
        (NULL != (pchScan = FindCharInSet(pchScan, (LONG32) (cch - (pchScan - pchBuf)), USZ_EOLCHARSET))))
    {
        pchPoke = pchScan -1;
        while (pchPoke >= pchBuf && IsBlank(*pchPoke))
            *pchPoke-- = UCH_NONCHAR;
        pchScan = LineBreakAdvance(pchScan);
        while (IsBlank(*pchScan))
            *pchScan++ = UCH_NONCHAR;
    }

    //
    // Copy all but UCH_NONCHAR
    //
    pchScan = pchPoke = pchBuf;

    unsigned int cchCopied = cch;
    while (cchCopied)
    {
        if (UCH_NONCHAR != *pchScan)
            *pchPoke++ = *pchScan;
        pchScan++;
        cchCopied--;
    }

    // ASSUME: never longer than 4GB
    return (unsigned int) (pchPoke - pchBuf);
}

