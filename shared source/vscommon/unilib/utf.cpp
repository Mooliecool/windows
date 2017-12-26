// utf.cpp
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde] Updated for new lib, add surrogate/UCS4 handling
//
//-----------------------------------------------------------------

#include "pch.h"
#include "unichar.h"
#include "utf.h"

#define BIT7(a)        ((a) & 0x80)
#define BIT6(a)        ((a) & 0x40)

inline int _UTF8LengthOfW4Char (DWORD ch)
{
    char r = 2;
    if      (ch <  0x00000080) r = 1;
    else if (ch <  0x00000800) {}
    else if (ch <  0x00010000) r = 3;
    else if (ch <= 0x0010FFFF) r = 4;
    return (int)r;
}


// $

inline BOOL IsNonCharW4 (W4CHAR ch)
{
    return (ch >= 0xFDD0 && (ch <= 0xFDEF || (ch & 0xFFFE) == 0xFFFE));
}


/* illegal overlong sequences
>  1100 0000

>  1110 0000
>  100x xxxx

>  1111 0000 
>  1000 xxxx

>  1111 1000
>  1000 0xxx

>  1111 1100
>  1000 00xx
*/
//-----------------------------------------------------------------

int WINAPI UTF8ToUTF16Info ( /* __in_xcount(*pcbUTF8) */ PCSTR pUTF8, int * pcbUTF8, __out_ecount(cchUTF16) PWSTR pUTF16, int cchUTF16, DWORD* pdwInfo)
{
    UASSERT(cchUTF16 > 0);
    
    DWORD        ch;                            // character we're building
    PWSTR        pchOut  = pUTF16;
    PWSTR        pchEnd  = pUTF16 + cchUTF16;
    int          cbTrail = 0;                   // # bytes to follow
    int          cbTrailCheck = 0;              // check for overlong sequence
    SSIZE_T      cbUTF   = *pcbUTF8;
    const BYTE * pUTF    = (BYTE*)pUTF8;
    DWORD        dwInfo  = 0;                   // A-O-K

#ifdef _DEBUG
    memset(pUTF16, -1, cchUTF16*sizeof(WCHAR));
#endif
    if (NULL_TERMINATED_MODE == cbUTF)
        cbUTF = StrLenA(pUTF8);
    
    while (cbUTF-- && (pchOut < pchEnd))
    {
        BYTE bUtf = (BYTE)*pUTF++;
        if (bUtf <= 0x7f)
        {
            if (cbTrail)
                dwInfo |= U8TU_COUNT_NO_TRAIL;
            *pchOut++ = (WCHAR)bUtf;
        }
        else if (0 == BIT6(bUtf))  // trail byte
        {
            dwInfo |= U8TU_NONASCII;
            if (cbTrail)
            {
                ch <<= 6;                 // make room for trail byte
                ch |= (bUtf & 0x3f);      // add trail byte value
                cbTrail--;
                if (0 == cbTrail)         // finished building char
                {
                    if (cbTrailCheck > _UTF8LengthOfW4Char(ch))
                    {
                        dwInfo |= U8TU_OVERLONG;
                        *pchOut++ = UCH_REPLACE;
                    }
                    else if (ch <= 0x0000FFFF)
                    {
                        // ok for Unicode - write it out
                        *pchOut++ = (WCHAR)ch;
                    }
                    else
                    {
                        // UCS-4 - compose surrogate
                        dwInfo |= U8TU_UCS4;
                        if (pchOut + 1 >= pchEnd) // we need 2 wchars
                            goto _End;
                        if (ch <= 0x0010FFFF)
                        {
                            ch -= 0x0010000;
                            *pchOut++ = (WCHAR)((ch >> 10) + UCH_HI_SURROGATE_FIRST);
                            *pchOut++ = (WCHAR)((ch & 0x000003FF) + UCH_LO_SURROGATE_FIRST);
                        }
                        else
                        {
                            // can't represent it
                            dwInfo |= U8TU_UCS4OUTOFRANGE;
                            *pchOut++ = UCH_REPLACE;
                        }
                    }
                }
            }
            else
            {
                dwInfo |= U8TU_TRAIL_NO_COUNT;
                *pchOut++ = UCH_REPLACE;   // write placeholder instead of bad char
            }
        }
        else
        {   // Lead byte
            dwInfo |= U8TU_NONASCII;
            if (cbTrail)                   // error, previous sequence not finished
            {
                dwInfo |= U8TU_COUNT_NO_TRAIL;
                *pchOut++ = UCH_REPLACE;   // write placeholder instead of incomplete char
                cbTrail = 0;
            }
            // calculate # bytes to follow
            while (0 != BIT7(bUtf))        // until first 0 bit encountered from left to right
            {
                bUtf <<= 1;
                cbTrail++;
            }
            ch = bUtf >> cbTrail;          // store value from the first byte
            cbTrailCheck = cbTrail;
            cbTrail--;                     // # bytes to follow
        }
        UASSERT(cbTrail >= 0);
        UASSERT(pchOut <= pchEnd);
    }

_End:
    // null-terminated mode
    if (NULL_TERMINATED_MODE == *pcbUTF8)
    {
        // guarantee result is 0-terminated
        if (pchOut < pchEnd)
            *pchOut++ = 0;
        else
            *(pchEnd-1) = 0;
    }

#ifdef _DEBUG
    {
        DWORD dw;
        int cch;
        // ASSUME: this will never be more than 4GB
        int cbUTF8 = (LONG32) *pcbUTF8;
        cch = GetUTF8Info(pUTF8, &cbUTF8, &dw, TRUE);
        UASSERT(dw == dwInfo);           // got same info
        UASSERT(cch == (pchOut - pUTF16) || pchOut >= pchEnd); // got same count of wchars if we didn't run out of the output buffer
        UASSERT(cbUTF8 == (pUTF - ((BYTE*)pUTF8)) || pchOut >= pchEnd); // scanned the same # of chars if we didn't run out of the output buffer
    }
#endif
    UASSERT((long)*pcbUTF8 == NULL_TERMINATED_MODE || ((pUTF - ((BYTE*)pUTF8)) == (long)*pcbUTF8) || (pchOut == pchEnd));
    *pcbUTF8 = (LONG32) (pUTF - ((BYTE*)pUTF8));

    SETRETVAL(pdwInfo, dwInfo);
    return (LONG32) (pchOut - pUTF16);
}

int WINAPI GetUTF8Info ( /* __in_xcount(*pcbUTF8) */ PCSTR pUTF8, int * pcbUTF8, DWORD * pdwInfo, BOOL fScanAll /*= FALSE*/)
{
    DWORD        ch;                       // character we're building
    int          cbTrail = 0;              // # bytes to follow
    int          cbTrailCheck = 0;
    SSIZE_T      cbUTF    = *pcbUTF8;
    const BYTE * pUTF     = (BYTE*)pUTF8;
    DWORD        dwInfo   = 0;             // start off with everything ok
    int          cchUTF16 = 0;

    if (NULL_TERMINATED_MODE == cbUTF)
    {
        cbUTF = StrLenA(pUTF8);
        cchUTF16++; // zero terminator
    }
    while (cbUTF--)
    {
        BYTE bUtf = (BYTE)*pUTF++;
        if (bUtf <= 0x7f) // ASCII
        {
            cchUTF16++;
            if (cbTrail)
            {
                dwInfo |= U8TU_COUNT_NO_TRAIL;
                if (!fScanAll) break;
            }
        }
        else if (0 == BIT6(bUtf)) // trail byte
        {
            dwInfo |= U8TU_NONASCII;
            if (cbTrail)
            {
                ch <<= 6;              // make room for trail byte
                ch |= (bUtf & 0x3f);   // add trail byte value
                cbTrail--;
                if (0 == cbTrail)      // finished building char
                {
                    cchUTF16++;
                    if (cbTrailCheck > _UTF8LengthOfW4Char(ch))
                        dwInfo |= U8TU_OVERLONG; // not error, so keep going
                    else if (ch <= 0x0000FFFF)
                        ; // in UCS-2/UTF-16 range: do nothing
                    else
                    {
                        // UCS-4 character - compose surrogate
                        dwInfo |= U8TU_UCS4;
                        if (ch > 0x0010FFFF)
                        {
                            // can't represent it
                            dwInfo |= U8TU_UCS4OUTOFRANGE;
                            if (!fScanAll) break;
                        }
                        else
                            cchUTF16++; 
                    }
                }
            }
            else
            {
                cchUTF16++;
                dwInfo |= U8TU_TRAIL_NO_COUNT;
                if (!fScanAll) break;
            }
        }
        else // lead byte
        {
            dwInfo |= U8TU_NONASCII;
            if (cbTrail)               // error, previous sequence not finished
            {
                dwInfo |= U8TU_COUNT_NO_TRAIL;
                cchUTF16++;
                if (!fScanAll) break;
                cbTrail = 0;
            }
            while (0 != BIT7(bUtf))       // until first 0 encountered from left to right
            {
                bUtf <<= 1;
                cbTrail++;
            }
            ch = bUtf >> cbTrail;     // store value from the first byte
            cbTrailCheck = cbTrail;
            cbTrail--;             // # bytes to follow
        }
        UASSERT(cbTrail >= 0);
    }

    *pcbUTF8 = (LONG32) (pUTF - ((BYTE*)pUTF8));
    SETRETVAL(pdwInfo, dwInfo);
    return cchUTF16;
}


DWORD WINAPI ValidateUTF16 ( /* __in_xcount(cchUTF16) */ PCWSTR pUTF16, int cchUTF16, BOOL fScanAll /* = FALSE */)
{
    DWORD  dwInfo = 0;              // start off with everything ok

    PCWSTR pch    = pUTF16;
    PCWSTR pchEnd = pUTF16 + (cchUTF16 == NULL_TERMINATED_MODE ? StrLen(pUTF16) : cchUTF16);

    while (pch < pchEnd)
    {
        DWORD ch = *pch++;
        if (ch > 0x7F)
        {
            dwInfo |= VU16_NONASCII;
        }

        if (IsHighSurrogate(ch))
        {
            if (pch < pchEnd)
            {
                DWORD ch2 = *pch;
                if (IsLowSurrogate(ch2))
                {
                    ch = ((ch  - UCH_HI_SURROGATE_FIRST) << 10)
                        + (ch2 - UCH_LO_SURROGATE_FIRST) + 0x0010000;
                    ++pch;
                    dwInfo |= VU16_UCS4;
                    if (IsNonCharW4(ch))
                    {
                        // info - that's a non-character
                        dwInfo |= VU16_NONCHAR;
                    }                        
                }
                else
                {
                    // error - no low surrogate of pair
                    dwInfo |= VU16_UNPAIRSURROGATE;
                    if (!fScanAll) break;
                }
            }
            else
            {
                // error - expected low surrogate but out of data
                dwInfo |= VU16_UNPAIRSURROGATE;
                if (!fScanAll) break;
            }
        }
        else if (IsLowSurrogate(ch))
        {
            // error - unpaired low surrogate
            dwInfo |= VU16_UNPAIRSURROGATE;
            if (!fScanAll) break;
        }
        else if (IsNonCharW4(ch))
        {
            // info - that's a non-character
            dwInfo |= VU16_NONCHAR;
        }                        
    }

    return dwInfo;
}

/////////////////////////////////////////////////////////////////
// Faster version with fewest checks
int WINAPI UTF8ToUTF16 ( /* __in_xcount(cbUTF) */ PCSTR pUTF8, int cbUTF, __out_ecount(cchUTF16) PWSTR pUTF16, int cchUTF16)
{
    DWORD        ch;                           // character we're building
    PWSTR        pchOut  = pUTF16;
    const BYTE * pUTF    = (BYTE*)pUTF8;
    PWSTR        pchEnd  = pUTF16 + cchUTF16;
    int          cbTrail = 0;              // # bytes to follow
    int          cbTrailCheck = 0;
    int          cb      = cbUTF;

    if (NULL_TERMINATED_MODE == cb)
        cb = StrLenA(pUTF8);
    while ((cb--) && (pchOut < pchEnd))
    {
        BYTE bUtf = (BYTE)*pUTF++;
        if (bUtf <= 0x7f)
        {
            *pchOut++ = (WCHAR)bUtf;
        }
        else if (0 == BIT6(bUtf))
        {
            if (cbTrail)
            {
                ch <<= 6;               // make room for trail byte
                ch |= (bUtf & 0x3f);    // add trail byte value
                cbTrail--;
                if (0 == cbTrail)       // finished building char
                {
                    if (cbTrailCheck > _UTF8LengthOfW4Char(ch))
                    {
                        //dwInfo |= U8TU_OVERLONG;
                        *pchOut++ = UCH_REPLACE;
                    }
                    else if (ch <= 0x0000FFFF)
                    {
                        *pchOut++ = (WCHAR)ch; // write output char
                    }
                    else
                    {
                        // UCS-4 character - compose surrogate
                        if (pchOut + 1 >= pchEnd) // we need 2 wchars
                            goto _End;
                        if (ch <= 0x0010FFFF)
                        {
                            ch -= 0x0010000;
                            *pchOut++ = (WCHAR)((ch >> 10) + UCH_HI_SURROGATE_FIRST);
                            *pchOut++ = (WCHAR)((ch & 0x000003FF) + UCH_LO_SURROGATE_FIRST);
                        }
                        else
                        {
                            // can't represent it
                            *pchOut++ = UCH_REPLACE;
                        }
                    }
                }
            }
            else
            {
                *pchOut++ = UCH_REPLACE;   // write placeholder instead of bad char
            }
        }
        else
        {
            if (cbTrail)               // error, previous sequence not finished
            {
                *pchOut++ = UCH_REPLACE;   // write placeholder instead of incomplete char
                cbTrail = 0;
            }
            while (0 != BIT7(bUtf))       // until first 0 encountered from left to right
            {
                bUtf <<= 1;
                cbTrail++;
            }
            ch = bUtf >> cbTrail;     // store value from the first byte
            cbTrailCheck = cbTrail;
            cbTrail--;             // # bytes to follow
        }
    }

_End:
    if (NULL_TERMINATED_MODE == cbUTF)
    {
        // guarantee result is 0-terminated
        if (pchOut < pchEnd)
            *pchOut++ = 0;
        else
            *(pchEnd-1) = 0;
    }
    return (LONG32) (pchOut - pUTF16);
}

/////////////////////////////////////////////////////////////////
// Faster version with fewest checks
int WINAPI UTF16LengthOfUTF8 ( /* __in_xcount(cbUTF8) */ PCSTR pUTF8, int cbUTF8)
{
    int          cbTrail = 0;              // # bytes to follow
    DWORD        ch;                       // character we're building
    const BYTE * pUTF    = (BYTE*)pUTF8;
    int          cchUTF16  = 0;
    if (NULL_TERMINATED_MODE == cbUTF8)
    {
        cbUTF8 = StrLenA(pUTF8);
        cchUTF16++;
    }
    while (cbUTF8--)
    {
        BYTE bUtf = (BYTE)*pUTF++;
        if (bUtf <= 0x7f)
        {
            cchUTF16++;
        }
        else if (0 == BIT6(bUtf))
        {
            if (cbTrail)
            {
                ch <<= 6;              // make room for trail byte
                ch |= (bUtf & 0x3f);   // add trail byte value
                cbTrail--;
                if (0 == cbTrail)      // finished building char
                {
                    cchUTF16++;
                    if ((ch > 0x0000FFFF) && (ch <= 0x0010FFFF))
                        cchUTF16++;
                }
            }
            else
                cchUTF16++;
        }
        else
        {
            if (cbTrail)               // error, previous sequence not finished
                cchUTF16++;
            while (0 != BIT7(bUtf))       // until first 0 encountered from left to right
            {
                bUtf <<= 1;
                cbTrail++;
            }
            ch = bUtf >> cbTrail;     // store value from the first byte
            cbTrail--;             // # bytes to follow
        }
    }
    return cchUTF16;
}

//-----------------------------------------------------------------
int WINAPI UTF16ToUTF8 ( /* __in_xcount(*pcchUTF16) */ PCWSTR pUTF16, int * pcchUTF16, __out_ecount(cbUTF8) PSTR pUTF8, int cbUTF8)
{
    int    cb;
    DWORD  ch;
    PCWSTR pchEnd;
    PCWSTR pch     = pUTF16;
    BYTE * pUTF    = (BYTE*)pUTF8;
    BYTE * pUTFEnd = pUTF + cbUTF8;

    {
        SSIZE_T cchUTF16 = *pcchUTF16;
        if (NULL_TERMINATED_MODE == cchUTF16)
        {
            cchUTF16 = StrLen(pUTF16);
            --pUTFEnd; // leave room for 0 terminator
        }
        pchEnd = pUTF16 + cchUTF16;
    }
    while (pch < pchEnd)
    {
        ch = *pch++;
        if (ch < 0x00000080)  // one byte
        {
            // pUTF points one beyond the last byte we wrote.  If it points
            // at pUTFEnd (or beyond) we are done.  pUTFEnd is one past the
            // last writable byte in the UTF8 buffer.
            if (pUTF >= pUTFEnd)
            {
                --pch; // didn't consume last character
                break;
            }
            else
            {
                *pUTF++ = (BYTE)ch;
                continue;
            }
        }
        if (IsHighSurrogate(ch))
        {
            if (pch < pchEnd && IsLowSurrogate(*pch))
            {   
                // Surrogate pair
                ch = ((ch  - UCH_HI_SURROGATE_FIRST) << 10)
                    + (*pch - UCH_LO_SURROGATE_FIRST) + 0x0010000;
                UASSERT(ch >= 0x10000); // surrogate exceeds two bytes
                ++pch;
            }
            else
            {
                // Conversion from one UTF to another should preserve everything,
                // even unpaired surrogates.  Just render the high surrogate in ch
                // as a regular 3-byte sequence.
                UASSERT(ch < 0x10000); // only surrogate exceeds two bytes
            }
        }
        else
            UASSERT(ch < 0x10000); // only surrogate exceeds two bytes

        UASSERT(ch >= 0x00000080);
        if      (ch <  0x00000800) { cb = 2; } 
        else if (ch <  0x00010000) { cb = 3; }
        else if (ch <= 0x0010FFFF) { cb = 4; } // Max UCS4 value representable in UTF-16
        // ... this code would be for UCS-4 conversion.
        //else if (ch <  0x00200000) { cb = 4; }
        //else if (ch <  0x04000000) { cb = 5; }
        //else if (ch <= 0x7FFFFFFF) { cb = 6; } // Max legal UCS4
        else     /* bad data */    { cb = 3; ch = UCH_REPLACE; UASSERT(0); }
        
        UASSERT(IN_RANGE(cb, 2, 4));
        
        pUTF += cb;
        // Unlike above, we will not write at character position pUTF here.
        // We will subtract at least 1 byte before writing.  So it's fine
        // if pUTF is equal to pUTFEnd.  We stop only if pUTF is strictly
        // greater than pUTFEnd.
        if (pUTF > pUTFEnd)
        {
            pUTF -= cb;
            --pch; // we didn't consume the last character so back up the pointer.
            if (ch >= 0x0010000) // last char was surrogate pair - back up two.
                --pch;
            break;
        }
        // write trail bytes right to left
        switch (cb) // all cases fall through
        {
        //case 6: *--pUTF = (BYTE)((ch | 0x80) & 0xBF); ch >>= 6;
        //case 5: *--pUTF = (BYTE)((ch | 0x80) & 0xBF); ch >>= 6;
        case 4: *--pUTF = (BYTE)((ch | 0x80) & 0xBF); ch >>= 6;
        case 3: *--pUTF = (BYTE)((ch | 0x80) & 0xBF); ch >>= 6;
        case 2: *--pUTF = (BYTE)((ch | 0x80) & 0xBF); ch >>= 6;
        }
        // compute lead byte
        switch (cb)
        {
        case 2: ch |= 0xC0; break;
        case 3: ch |= 0xE0; break;
        case 4: ch |= 0xF0; break;
        //case 5: ch |= 0xF8; break;
        //case 6: ch |= 0xFC; break;
        }
        // write lead byte
        *--pUTF = (BYTE)ch;
        pUTF += cb;
    }
    if (NULL_TERMINATED_MODE == *pcchUTF16)
        *pUTF++ = 0;
    *pcchUTF16 = (LONG32) (pch - pUTF16);
    return (LONG32) (pUTF - (BYTE*)pUTF8);
}

int WINAPI UTF8LengthOfUTF16 ( /* __in_xcount(cchUTF16) */ PCWSTR pUTF16, int cchUTF16)
{
    DWORD  ch;
    PCWSTR pchEnd;
    int    cbUTF8  = 0;
    PCWSTR pch     = pUTF16;
    if (NULL_TERMINATED_MODE == cchUTF16)
    {
        cchUTF16 = StrLen(pUTF16);
        cbUTF8++; // for 0 terminator
    }
    pchEnd = pUTF16 + cchUTF16;
    while (pch < pchEnd)
    {
        ch = *pch++;
        if (IsHighSurrogate(ch) && pch < pchEnd && IsLowSurrogate(*pch))
        {
            // Any character encoded by a surrogate pair in UTF16 takes exactly 4 bytes in UTF8
            cbUTF8 += 4;
            ++pch;
        }
        else
        {
            cbUTF8 += _UTF8LengthOfW4Char(ch);
        }
    }
    return cbUTF8;
}
