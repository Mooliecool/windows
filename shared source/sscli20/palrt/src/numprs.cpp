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
// File: numprs.cpp
// 
// ===========================================================================
/***
*
*Purpose:
*  This module contains string parsing functions for all types.
*
*Implementation Notes:
*
*****************************************************************************/

#include "rotor_palrt.h"

#include "oautil.h"
#include "oleauto.h"

#include "convert.h"
#include <limits.h>
#include <ctype.h>
#include <math.h>

STDAPI VarCyMulI4(CY cyLeft, long lRight, LPCY pcyResult);

// Constants used by VarParseNumFromStr
//
#define MAXEXP (INT_MAX/2)

const SPLIT64    sdlTenToEighteen = { UI64(1000000000000000000) };

// Max value of Decimal (= 79228162514264337593543950335), less 1st digit.
const BYTE rgbMaxDec[DEC_MAXDIG - 1] = {9,2,2,8,1,6,2,5,1,4,2,6,4,3,3,7,5,9,3,5,4,3,9,5,0,3,3,5};

typedef struct {
    DBLSTRUCT  dsHi;
    DBLSTRUCT  dsLo;
} DBLPREC;

#define DEFDP(a,b,c,d,e,f) { DEFDS(a,b,c,0), DEFDS(d,e,f,0) }
#define BINEXPFACTOR 0x100

DBLPREC dpPwr10[16] = {
    DEFDP(0x37E08000, 0x1C379, 0x434, 0x00000000, 0x00000, 0x000), // 1E16
    DEFDP(0xB5056E16, 0x3B8B5, 0x469, 0x80000000, 0x677C0, 0x434), // 1E32
    DEFDP(0xA0A1C872, 0x5E531, 0x49E, 0xC204ADFD, 0x75A59, 0x469), // 1E48
    DEFDP(0xE93FF9F4, 0x84F03, 0x4D3, 0xFC2DC71D, 0xB54F2, 0x49E), // 1E64
    DEFDP(0x51F0FB5E, 0xAFCEF, 0x508, 0xCC117B25, 0xFEF70, 0x4D3), // 1E80
    DEFDP(0x62D8B362, 0xDF675, 0x53D, 0x3E253BCF, 0x28B17, 0x508), // 1E96
    DEFDP(0xB8132466, 0x0A1F5, 0x573, 0x147B5E30, 0x4F01F, 0x53D), // 1E112
    DEFDP(0xF9301D31, 0x27748, 0x5A8, 0xBC0DB0DA, 0x37F19, 0x573), // 1E128
    DEFDP(0x38B51A74, 0x48057, 0x5DD, 0x5801B271, 0xB9D7C, 0x5A8), // 1E144
    DEFDP(0x256FFCC2, 0x6C2D4, 0x612, 0xEE21ACC5, 0xEA95D, 0x5DD), // 1E160
    DEFDP(0x5230B377, 0x94514, 0x647, 0xEE3DD40F, 0xE4DF5, 0x612), // 1E176
    DEFDP(0xF1A724EA, 0xC0E1E, 0x67C, 0xEE094FD1, 0x5AA16, 0x647), // 1E192
    DEFDP(0x86A6F04C, 0xF25C1, 0x6B1, 0x08A56CCD, 0x45A77, 0x67A), // 1E208
    DEFDP(0xDFFC6799, 0x14A52, 0x6E7, 0xD0B70D99, 0x2F82B, 0x6B0), // 1E224
    DEFDP(0xDE7AD7E2, 0x33234, 0x71C, 0x28368782, 0xD96B3, 0x6E7), // 1E240
    // 1E256 has exponent reduced by BINEXPFACTOR (0x100)
    DEFDP(0x7F73BF3B, 0x54FDD, 0x651, 0xEE006E63, 0xA3776, 0x61C), // 1E256
    };

// This function is used to ensure it's operands are not extended precision.
//
inline double DIF(double a, double b) {return a - b;}

void DpMul(DBLPREC &dpDest, DBLPREC &dpSrc)
{
#define DBLSPLIT (~0x7ffffff)

    DBLSTRUCT   dsSplit;
    double      dblOp1Hi, dblOp1Mid, dblOp1Lo;
    double      dblOp2Hi, dblOp2Mid, dblOp2Lo;
    double      dblSumHi, dblSumMid, dblSumLo;
    double      dblTmp;

    // Split the high double into two: one with the upper 26 bits of the
    // mantissa, the other with the lower 27 bits.
    //
    dsSplit.dbl = dpDest.dsHi.dbl;
    dsSplit.u.mantLo &= DBLSPLIT;
    dblOp1Hi = dsSplit.dbl;
    dblOp1Mid = dpDest.dsHi.dbl - dblOp1Hi;
    dblOp1Lo = dpDest.dsLo.dbl;

    dsSplit.dbl = dpSrc.dsHi.dbl;
    dsSplit.u.mantLo &= DBLSPLIT;
    dblOp2Hi = dsSplit.dbl;
    dblOp2Mid = dpSrc.dsHi.dbl - dblOp2Hi;
    dblOp2Lo = dpSrc.dsLo.dbl;

    dblSumLo = dblOp1Mid * dblOp2Lo + dblOp1Lo * dblOp2Mid;
    dblSumLo += dblOp1Hi * dblOp2Lo + dblOp1Mid * dblOp2Mid + dblOp1Lo * dblOp2Hi;
    dblSumMid = dblOp1Hi * dblOp2Mid + dblOp1Mid * dblOp2Hi;
    dblSumHi = dblOp1Hi * dblOp2Hi;

    // We need to split the middle sum between hi and lo.  By just
    // adding hi to mid and then subtracting hi back out, we see how
    // much fits into hi and can add the rest into lo.
    //
    // Adding then subtracting only works if there is no extra precision
    // being kept.  An inline function call is used for the subtraction
    // in hopes of ensuring values are flushed from extended-precision
    // registers (if any).  [This is necessary at least for x86, which
    // is used for a test bed for this code.]
    //
    dblTmp = dblSumHi + dblSumMid;
    dblTmp = DIF(dblTmp, dblSumHi);
    dblSumHi += dblTmp;
    dblSumLo += dblSumMid - dblTmp;

    // Store result.
    //
    dpDest.dsHi.dbl = dblSumHi;
    dpDest.dsLo.dbl = dblSumLo;
}


HRESULT MulPower10(double *pdblVal, double dblValLo, int nPwr10)
{
    int         nCurPwr;
    int         nExp;
    DBLPREC     dpCurPwr;
    DBLPREC     dpRes;
    DBLPREC     dpQuo;

    nCurPwr = abs(nPwr10);

    if (dblValLo == 0 && nCurPwr <= MAXINTPWR10) {

      if (nPwr10 >= 0)
        *pdblVal *= dblPower10[nPwr10];
      else
        *pdblVal /= dblPower10[nCurPwr];

      return NOERROR;
    }

    dpCurPwr.dsHi.dbl = fnDblPower10(nCurPwr & 0xF);
    dpCurPwr.dsLo.dbl = 0;
    nCurPwr = nCurPwr >> 4;
    if ((nCurPwr & 0xF) != 0)
      DpMul(dpCurPwr, dpPwr10[(nCurPwr & 0xF) - 1]);

    if (nCurPwr > 15) {
      // We've got a really big power of 10.  Check for overflow
      // or underflow.
      //
      if (nPwr10 < -350) {
        *pdblVal = 0.0;
        return NOERROR;
      }

      if (nPwr10 >= 309)
        return RESULT(DISP_E_OVERFLOW);

      // In order to prevent possible overflow, the exponent for 1E256
      // has been reduced by BINEXPFACTOR.
      //
      DpMul(dpCurPwr, dpPwr10[15]);
    }

    dpRes.dsHi.dbl = *pdblVal;
    dpRes.dsLo.dbl = dblValLo;

    if (nPwr10 >= 0) {
      DpMul(dpRes, dpCurPwr);
      *pdblVal = dpRes.dsHi.dbl + dpRes.dsLo.dbl;
      if (nCurPwr > 15) {
        // We've reduced the exponent.  See if we can restore it without
        // overflow.
        //
        dpRes.dsHi.dbl = *pdblVal;
        nExp = dpRes.dsHi.u.exp + BINEXPFACTOR;
        if (nExp >= 0x7FF)
          return RESULT(DISP_E_OVERFLOW);
        dpRes.dsHi.u.exp = nExp;
        *pdblVal = dpRes.dsHi.dbl;
      }
    }
    else {
      if (nCurPwr > 15) {
        // We've reduced the exponent.  Apply it to the digit value now
        // so it will get factored into the final divide.
        //
        _ASSERTE(dpRes.dsHi.dbl != 0);
        dpRes.dsHi.u.exp -= BINEXPFACTOR;
        if (dpRes.dsLo.dbl != 0)
          dpRes.dsLo.u.exp -= BINEXPFACTOR;
      }

      // Perform double-precision divide.
      //
      dpQuo.dsHi.dbl = *pdblVal = dpRes.dsHi.dbl / dpCurPwr.dsHi.dbl;
      dpQuo.dsLo.dbl = 0;
      DpMul(dpQuo, dpCurPwr);
      dpRes.dsHi.dbl -= dpQuo.dsHi.dbl;
      dpRes.dsLo.dbl -= dpQuo.dsLo.dbl;
      *pdblVal += (dpRes.dsHi.dbl + dpRes.dsLo.dbl) / (dpCurPwr.dsHi.dbl + dpCurPwr.dsLo.dbl);
    }

    return NOERROR;
}


DWORDLONG UInt64x64To128(SPLIT64 sdlOp1, SPLIT64 sdlOp2, DWORDLONG *pdlHi)
{
    SPLIT64  sdlTmp1;
    SPLIT64  sdlTmp2;
    SPLIT64  sdlTmp3;

    sdlTmp1.int64 = UInt32x32To64(sdlOp1.u.Lo, sdlOp2.u.Lo); // lo partial prod
    sdlTmp2.int64 = UInt32x32To64(sdlOp1.u.Lo, sdlOp2.u.Hi); // mid 1 partial prod
    sdlTmp1.u.Hi += sdlTmp2.u.Lo;
    if (sdlTmp1.u.Hi < sdlTmp2.u.Lo)  // test for carry
      sdlTmp2.u.Hi++;
    sdlTmp3.int64 = UInt32x32To64(sdlOp1.u.Hi, sdlOp2.u.Hi) + (DWORDLONG)sdlTmp2.u.Hi;
    sdlTmp2.int64 = UInt32x32To64(sdlOp1.u.Hi, sdlOp2.u.Lo);
    sdlTmp1.u.Hi += sdlTmp2.u.Lo;
    if (sdlTmp1.u.Hi < sdlTmp2.u.Lo)  // test for carry
      sdlTmp2.u.Hi++;
    sdlTmp3.int64 += (DWORDLONG)sdlTmp2.u.Hi;

    *pdlHi = sdlTmp3.int64;
    return sdlTmp1.int64;
}

/* decide if an OLECHAR is a digit taking consideration of locale */
INTERNAL_(BOOL)
ISOADIGIT (LCID lcid, OLECHAR ch)
{
  if (ch >= '0' && ch <= '9')
    return TRUE;
  else if (IsThai(lcid) && ch >= xchThaiZero && ch <= xchThaiNine)
    return TRUE;
  else
    return FALSE;
}

/* return the corresponding digit taking consideration of locale
   return -1 if not a digit */
INTERNAL_(int)
GETOADIGIT (LCID lcid, OLECHAR ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (IsThai(lcid) && ch >= xchThaiZero && ch <= xchThaiNine)
    return ch - xchThaiZero;
  else
    return -1;
}

/***
* VarParseNumFromStr
*
* Entry:
*   pstr    - pointer to zero-terminated string.
*   lcid    - lcid to use.
*   dwFlags - optional LOCALE_NOUSEROVERRIDE flag.
*   pnumprs - pointer to NUMPARSE structure to fill in with return info.
*   rgbDig  - pointer for array of values for each digit.
*
* Purpose:
*   Parse a string to a number.  pnumprs and rgbDig are pointers
*   where information will be filled in.  
*
*   rgbDig is an array of bytes that will be filled in with the value 
*   of each digit character in the string.  This array will contain 
*   nothing but values in the range 0 - 9 for decimal numbers, 
*   0 - 15 for hex, 0 - 7 for octal.  Leading zeros are always stripped
*   off.  
*
*   pnumprs->cDig contains the max size of this array on entry.  The
*   caller should use a local (frame) array that is 1 element longer
*   than the maximum number of digits that make sense for the type
*   of the number.  This extra element will contain the rounding
*   digit.  For decimal numbers, if there are more digits 
*   in the string than elements in the array, the NUMPRS_INEXACT
*   flag will be set.  For non-decimal numbers, an Overflow error
*   is returned if the number of digits is too large.
*
*   pnumprs->dwInFlags contains an array of bit flags that identify
*   the syntactic elements that should be accepted.
*
*   The NUMPARSE structure pointed to by pnumprs is filled in with
*   the rest of the info needed to convert the string to a number:
*
*   cDig - Modified to have the number of elements in rgbDig that
*   are actually filled in.  Will never exceed it's value on entry.
*
*   nPwr10 - The power of 10 of the last digit in rgbDig.  Thus the
*   number represented by rgbDig can be converted to a binary integer,
*   and must then be scaled by 10^nPwr10.  Will always be zero (and
*   can be ignored) for non-decimal numbers.
*
*   nBaseShift - Indicates the number base: 0 for decimal, 3 for octal,
*   4 for hex.  Represents the bit-shift count for binary-derived bases.
*
*   dwOutFlags - Contains a bit strings identifying the syntactic
*   elements found in the string.  Also includes NUMPRS_NEG if the
*   number is negative and NUMPRS_INEXACT as mentioned above.
*
* Exit:
*   *pnumprs and rgbDig filled in.
*   HRESULT of operation returned.
*
* Exceptions:
*   A memory allocation is made if DBCS lcid so the string can have
*   characters mapped.  Otherwise, the only possible errors are
*   Overflow and Type Mismatch.
*
***********************************************************************/

STDAPI VarParseNumFromStr(OLECHAR *        pstr,
                          LCID             lcid,
                          ULONG            dwFlags,
                          NUMPARSE *       pnumprs,
                          BYTE *           rgbDig)
{
    HRESULT     hresult = NOERROR;
    LPOLESTR    pstrDBCS = NULL;
    LPOLESTR    pstrStart;
    OLECHAR     ch1;
    BYTE        *pbDig = rgbDig;
    BYTE        *pbDigEnd;
    NUMPARSE    np;

    _ASSERTE(lcid ==  0x0409);
    _ASSERTE(dwFlags == LOCALE_NOUSEROVERRIDE);

    if (rgbDig == NULL || pnumprs == NULL)
      return RESULT(E_INVALIDARG);

    // Local structure keeps working values and results.
    //
    np.cDig = 0;
    np.dwInFlags = pnumprs->dwInFlags;
    np.dwOutFlags = 0;
    np.cchUsed = 0;
    np.nBaseShift = 0;
    np.nPwr10 = 0;
    pbDigEnd = rgbDig + pnumprs->cDig;

    if (pstr == NULL) {
      hresult = RESULT(DISP_E_TYPEMISMATCH);
      goto Error;
    }

	// We know this is LCID = 0x0409 so there's no need to worry about
	// DBCS

    ch1 = *pstr++;
    pstrStart = pstr;

    if ( iswspace(ch1) && (np.dwInFlags & NUMPRS_LEADING_WHITE) ) {
      np.dwOutFlags |= NUMPRS_LEADING_WHITE;
      while (iswspace(ch1 = *pstr++));
    }

    if (ch1 == OASTR('&') && (np.dwInFlags & NUMPRS_HEX_OCT)) {

      int   nMaxVal;
      int   nVal;
      BOOL  fHavDig = FALSE;

      // Get hex or octal integer.
      //
      ch1 = *pstr++;

      if (ch1 == 'h' || ch1== 'H') {
        np.nBaseShift = 4; // base 16 digits
        nMaxVal = 15;
        ch1 = *pstr++;
      }
      else {
        np.nBaseShift = 3; // base 8 digits
        nMaxVal = 7;

        if (ch1 == 'o' || ch1 == 'O')
          ch1 = *pstr++;
      }

      if (ch1 == '0')
        fHavDig = TRUE;

      while (ch1 == '0')  // scan off leading zeros
        ch1 = *pstr++;

      for(;;) {

        if ((nVal = GETOADIGIT (lcid, ch1)) >= 0)
              ; 

        else if (ch1 >= 'a' && ch1 <= 'f')
              nVal = ch1 - 'a' + 10;

        else if (ch1 >= 'A' && ch1 <= 'F')
              nVal = ch1 - 'A' + 10;

        else break;

        if (nVal > nMaxVal)
          break;

          if (pbDig >= pbDigEnd) {
          // If we have too many digits, return the same info as if
          // this wasn't an error.
          //
          hresult = RESULT(DISP_E_OVERFLOW);
          break;
        }

        *pbDig++ = nVal;
        ch1 = *pstr++;
      }

      if (pbDig == rgbDig) {
        if (!fHavDig) {  // find any valid digits?
          // If we have no digits, leave the NUMPARSE blank.
          //
          hresult = RESULT(DISP_E_TYPEMISMATCH);
          goto Error;
        }
        *pbDig++ = 0;
      }

      np.dwOutFlags |= NUMPRS_HEX_OCT;

      if ( iswspace(ch1) && (np.dwInFlags & NUMPRS_TRAILING_WHITE) ) {
        np.dwOutFlags |= NUMPRS_TRAILING_WHITE;
        while (iswspace(ch1 = *pstr++));
      }
    } 
    else {

      // Get decimal number.  Possible formatting options:
      //
      // 1. A + or - sign at the beginning or end, or parens () to indicate
      // negative.  May have spaces around it.
      //
      // 2. A currency symbol at the beginning or end. May be on either
      // side of the sign indicator.  May have spaces around it.
      //
      // 3. Thousands separators are ignored.
      //
      // 4. Base-10 exponent may be specfied with E or D.
      //

#define HAVSIGN (NUMPRS_LEADING_PLUS | NUMPRS_LEADING_MINUS | NUMPRS_TRAILING_PLUS | NUMPRS_TRAILING_MINUS | NUMPRS_PARENS)

      BOOL      fOpenParen = FALSE;
      BOOL      fNegPwr = FALSE;
      int       nPwrCnt = 0;
      int       nExp;
      LPOLESTR  pstrTmp;
      NUMINFO   *pnuminfo;

      IfFailGo(GetNumInfo(lcid, dwFlags, &pnuminfo), Error);

      for ( ; !ISOADIGIT (lcid, ch1); ch1 = *pstr++ ) {
        // Parse off junk in front of first digit
        //
        if ( iswspace(ch1) ) {
          if ( !(np.dwInFlags & NUMPRS_LEADING_WHITE) )
            goto TmError;
          np.dwOutFlags |= NUMPRS_LEADING_WHITE;
          continue;
        }

        if (ch1 == '+') {
          if ( !(np.dwInFlags & NUMPRS_LEADING_PLUS) || 
               (np.dwOutFlags & HAVSIGN) )
            goto TmError;
          np.dwOutFlags |= NUMPRS_LEADING_PLUS;
          continue;
        }

        if (ch1 == '-') {
          if ( !(np.dwInFlags & NUMPRS_LEADING_MINUS) ||
               (np.dwOutFlags & HAVSIGN) )
            goto TmError;
          np.dwOutFlags |= NUMPRS_LEADING_MINUS | NUMPRS_NEG;
          continue;
        }

        if (ch1 == '(') {
          if ( !(np.dwInFlags & NUMPRS_PARENS) ||
               (np.dwOutFlags & HAVSIGN) )
            goto TmError;
          fOpenParen = TRUE;
          np.dwOutFlags |= NUMPRS_PARENS | NUMPRS_NEG;
          continue;
        }

        if (ch1 == pnuminfo->chDecimal) {
          ch1 = *pstr++;
          if ( !(np.dwInFlags & NUMPRS_DECIMAL) ||
               ! ISOADIGIT (lcid, ch1)) // verify DP is followed by digit
            goto TmError;
          np.dwOutFlags |= NUMPRS_DECIMAL;
          nPwrCnt = -1;
          break;
        }

    // If the character is the Currency Decimal, and we might be parsing for
    //  currency, and the character can't be confused with the Number 
    //  Thousands Separater, take it as a decimal.
    if ((ch1 == pnuminfo->chCurrencyDecimal) && (np.dwInFlags & NUMPRS_CURRENCY) &&
        (ch1 != pnuminfo->chThousand1 && ch1 != pnuminfo->chThousand2))
    {
        ch1 = *pstr++;
        if (!(np.dwInFlags & NUMPRS_DECIMAL) ||
             ch1 < '0' || ch1 > '9' ) // verify DP is followed by digit
          goto TmError;
        np.dwOutFlags |= (NUMPRS_DECIMAL | NUMPRS_CURRENCY);
        nPwrCnt = -1;
        break;
    }

        // Haven't recognized it yet.  Maybe it's the Currency symbol.
        //
        if ( (np.dwInFlags & NUMPRS_CURRENCY) && !(np.dwOutFlags & NUMPRS_CURRENCY) ) {
          if (pnuminfo->fFastCur) {
            if (ch1 == pnuminfo->rgbCurrency[0]) {
              np.dwOutFlags |= NUMPRS_CURRENCY;
              continue;
            }
          }
          else {
            // Must do case-insensitive check for currency symbol.
            // First check if there's enough characters left in the string.
            //
            pstr--;
            pstrTmp = pstr + pnuminfo->cchCurrency;
            while (pstr < pstrTmp)
              if (*pstr++ == 0)
                goto TmError;

            if ( CompareStringW(lcid, 
                                NORM_IGNORECASE|NORM_IGNOREWIDTH, 
                                pstr - pnuminfo->cchCurrency, 
                                pnuminfo->cchCurrency, 
                                pnuminfo->rgbCurrency, 
                                pnuminfo->cchCurrency)
                 == 2) {
              np.dwOutFlags |= NUMPRS_CURRENCY;
              continue; // it matched; we've already skipped over it.
            }
          }
        }

        // Don't know what it is
        //
TmError:
        hresult = RESULT(DISP_E_TYPEMISMATCH);
        goto Error;
      }

      // Found the first decimal digit, in ch1.
      //
StripZeros:
    for( ; ; ch1 = *pstr++)
    {
        if (GETOADIGIT (lcid, ch1) == 0)
          np.nPwr10 += nPwrCnt;
            else if (ch1 == pnuminfo->chThousand1 || ch1 == pnuminfo->chThousand2)
        {
            if ( ch1 == 0 || !(np.dwInFlags & NUMPRS_THOUSANDS) )
              goto ParseDone;
            np.dwOutFlags |= NUMPRS_THOUSANDS;
        }
        else if (ch1 == pnuminfo->chCurrencyThousand && ch1 != pnuminfo->chDecimal)
        {
              if (!(np.dwInFlags & NUMPRS_CURRENCY))
                break;
              if (ch1 == 0 || !(np.dwInFlags & NUMPRS_THOUSANDS))
                goto ParseDone;
              np.dwOutFlags |= (NUMPRS_THOUSANDS | NUMPRS_CURRENCY);
        }
        else
            break;
    }

LoadDigits:
      for( ; ; ch1 = *pstr++) {
        if ( ISOADIGIT (lcid, ch1) ) {
          if (pbDig < pbDigEnd) {
            *pbDig++ = GETOADIGIT (lcid, ch1);
            np.nPwr10 += nPwrCnt;
                }
                else
                {
            // We filled up all the digits.
            //
            if (ch1 != '0')
              np.dwOutFlags |= NUMPRS_INEXACT;
            np.nPwr10 += nPwrCnt + 1;
          }
        }
        else if (ch1 == pnuminfo->chThousand1 || ch1 == pnuminfo->chThousand2) {
          if ( ch1 == 0 || !(np.dwInFlags & NUMPRS_THOUSANDS) )
            goto ParseDone;
          np.dwOutFlags |= NUMPRS_THOUSANDS;
        }
        else if (ch1 == pnuminfo->chCurrencyThousand && ch1 != pnuminfo->chDecimal) {
          if (!(np.dwInFlags & NUMPRS_CURRENCY))
            break;
          if (ch1 == 0 || !(np.dwInFlags & NUMPRS_THOUSANDS))
            goto ParseDone;
          np.dwOutFlags |= (NUMPRS_THOUSANDS | NUMPRS_CURRENCY);
        }
        else
          break;
      }

      // End of string of decimal digits.
      //
        if (ch1 == pnuminfo->chDecimal)
        {
            if (!(np.dwInFlags & NUMPRS_DECIMAL) || (np.dwOutFlags & NUMPRS_DECIMAL))
          goto ParseDone;

        nPwrCnt = -1;
        np.dwOutFlags |= NUMPRS_DECIMAL;
        ch1 = *pstr++;
        if (pbDig == rgbDig)  // No non-zero digits yet,
          goto StripZeros;    // keep scanning off zeros.
        goto LoadDigits;
      }

        if (ch1 == pnuminfo->chCurrencyDecimal && ch1 != pnuminfo->chThousand1 && ch1 != pnuminfo->chThousand2)
        {
        if (!(np.dwInFlags & (NUMPRS_DECIMAL | NUMPRS_CURRENCY)) ||
            (np.dwOutFlags & NUMPRS_DECIMAL))
          goto ParseDone;

        nPwrCnt = -1;
        np.dwOutFlags |= (NUMPRS_DECIMAL | NUMPRS_CURRENCY);
        ch1 = *pstr++;
        if (pbDig == rgbDig)  // No non-zero digits yet,
          goto StripZeros;    // keep scanning off zeros.
        goto LoadDigits;
      }

      // Look for exponent.  Be sure not to confuse it with currency.
      //
      if ( (ch1 == 'e' || ch1 == 'E' || ch1 == 'd' || ch1 == 'D') &&
           (np.dwInFlags & NUMPRS_EXPONENT) ) {

        pstrTmp = pstr;  // save in case of unrecognized "e/d" (in which case
                         // it may be an alpha currency symbol. 
        ch1 = *pstr++;

        if (ch1 == '-') {
          fNegPwr = TRUE;
          ch1 = *pstr++;
        }
        else if (ch1 == '+')
          ch1 = *pstr++;

        if ( ISOADIGIT (lcid, ch1)) {
          // Get exponent digits, convert to number
          //
          nExp = GETOADIGIT (lcid, ch1);
          while (ISOADIGIT (lcid, ch1 = *pstr++)) {
            if (nExp >= MAXEXP/10) {
              pstr = pstrTmp;
              hresult = RESULT(DISP_E_OVERFLOW);
              goto ParseDone;
            }
            nExp = nExp * 10 + GETOADIGIT (lcid, ch1);
          }
          if (fNegPwr)
            nExp = -nExp;
          np.nPwr10 += nExp;
          np.dwOutFlags |= NUMPRS_EXPONENT;
        } 
        else {
          // not a valid "eN" or "dN" exponent -- revert back to where we
          // were in case this is something like the "DM" currency symbol.
          pstr = pstrTmp;
          ch1 = *(pstr-1);
        }
      }

      // Parse off junk after number.
      //
      for ( ; ch1 != '\0'; ch1 = *pstr++) {

        if ( iswspace(ch1) ) {
          if ( !(np.dwInFlags & NUMPRS_TRAILING_WHITE) )
            break;
          np.dwOutFlags |= NUMPRS_TRAILING_WHITE;
          continue;
        }

        if (ch1 == ')' && fOpenParen) {
          fOpenParen = FALSE;
          continue;
        }

        // check for trailing + or - signs (wierd, but sometimes happens)
        //
        if (ch1 == '+') {
          if ( !(np.dwInFlags & NUMPRS_TRAILING_PLUS) || 
               (np.dwOutFlags & HAVSIGN) )
            break;
          np.dwOutFlags |= NUMPRS_TRAILING_PLUS;
          continue;
        }

        if (ch1 == '-') {
          if ( !(np.dwInFlags & NUMPRS_TRAILING_MINUS) ||
               (np.dwOutFlags & HAVSIGN) )
            break;
          np.dwOutFlags |= NUMPRS_TRAILING_MINUS | NUMPRS_NEG;
          continue;
        }

        // Haven't recognized it yet.  Maybe it's the Currency symbol.
        //
        if (np.dwInFlags & NUMPRS_CURRENCY) {
          if (pnuminfo->fFastCur) {
            if (ch1 == pnuminfo->rgbCurrency[0]) {
              np.dwOutFlags |= NUMPRS_CURRENCY;
              continue;
            }
          }
          else {
            // Must do case-insensitive check for currency symbol.
            // First check if there's enough characters left in the string.
            //
            pstr--;
            pstrTmp = pstr + pnuminfo->cchCurrency;
            while (pstr < pstrTmp)
              if (*pstr++ == 0)
                goto NotTrailCur;

            if ( CompareStringW(lcid, 
                                NORM_IGNORECASE|NORM_IGNOREWIDTH, 
                                pstr - pnuminfo->cchCurrency, 
                                pnuminfo->cchCurrency, 
                                pnuminfo->rgbCurrency, 
                                pnuminfo->cchCurrency)
                 == 2) {
              np.dwOutFlags |= NUMPRS_CURRENCY;
              continue; // it matched; we've already skipped over it.
            }

            // Currency string didn't match.  Back up position so we
            // can report cchUsed correctly.
            //
NotTrailCur:
            pstr = pstrTmp - pnuminfo->cchCurrency + 1;
          }
        }

        // Unknown character
        //
        break;

      } // for

ParseDone:
      while (pbDig > rgbDig + 1 && pbDig[-1] == 0) {
        pbDig--;
        np.nPwr10++;
      }

      if (pbDig == rgbDig)
        *pbDig++ = 0;           // make sure we have a 0 digit.

      if (fOpenParen)           // if leftover left paren, then error.
        hresult = RESULT(DISP_E_TYPEMISMATCH);
    }

    // Fill in return info
    //
    {
    ULONG_PTR ulDigDiff = (ULONG_PTR)(pbDig - rgbDig); // WIN64
    _ASSERTE( ulDigDiff < 0x8FFFFFFF );
    np.cDig = (int)ulDigDiff;
    ULONG_PTR ulUsedDiff = (ULONG_PTR)(pstr - pstrStart);
    _ASSERTE( ulUsedDiff < 0x8FFFFFFF );
    np.cchUsed = (int)ulUsedDiff;
    if ( ch1 != 0 && (np.dwInFlags & NUMPRS_USE_ALL) )
      hresult = RESULT(DISP_E_TYPEMISMATCH);
    }

Error:

    if (pstrDBCS)
      DispFree(pstrDBCS);

    *pnumprs = np;
    return hresult;
}


/***
* VarNumFromParseNum
*
* Entry:
*   pnumprs  - pointer to NUMPARSE structure to fill in with return info.
*   rgbDig   - pointer to array of digit values.
*   dwVtBits - array of bits indicating acceptable result type.
*   pvar     - pointer to variant to be filled in with result.
*
* Purpose:
*   After a string has been parsed with VarParseNumFromStr, convert
*   the parsed output to a number.  See that function for a detailed
*   description of rgbDig and pnumprs.  Only the cDig, nBaseShift,
*   dwOutFlags, and nPwr10 fields of the NUMPARSE structure are used.
*
*   dwVtBits may have any number of bits set corresponding to numeric
*   types that are acceptable for the result.  This function will
*   choose the smallest allowed type that can hold the result value 
*   with as little precision loss as possible.  For non-integer types,
*   the following selection algorithm is used, ordered from
*   smallest to largest type:
*
*   No precision loss:
*   1. R4 - Integer of 7 or fewer digits.
*   2. R8 - Integer of 15 or fewer digits.
*   3. CY - 15 or fewer digits to left of d.p., 4 or fewer to the right. 
*   4. Decimal  - 29 or fewer digts, 28 or less to right of d.p.
*
* Exit:
*   *pvar filled in (not released first).
*   HRESULT of operation returned.
*
* Exceptions:
*   Overflow if the none of the allowed types can hold the result.
*
***********************************************************************/

#define VTBIT_FRAC (VTBIT_R4  | VTBIT_R8  | VTBIT_CY | VTBIT_DECIMAL)
#define VTBIT_UINT (VTBIT_UI1 | VTBIT_UI2 | VTBIT_UI4 | VTBIT_UI8)
#define VTBIT_SINT (VTBIT_I1  | VTBIT_I2  | VTBIT_I4 | VTBIT_I8)
#define VTBIT_INT  (VTBIT_UINT| VTBIT_SINT)

STDAPI VarNumFromParseNum(NUMPARSE *       pnumprs,
                          BYTE *           rgbDig,
                          ULONG            dwVtBits,
                          VARIANT *        pvar)
{
    int         nBaseShift = pnumprs->nBaseShift;
    int         cDig = pnumprs->cDig;
    int         nPwr10 = pnumprs->nPwr10;
    DWORD       dwOutFlags = pnumprs->dwOutFlags;
    int         nDec = 0;
    int         nIntDig = cDig + nPwr10;
    int         nMaxDig;
    BYTE        *pbDig = rgbDig;
    ULONG64     ullVal = 0;
    ULONG64     ullTmp;
    LONG64      llTmp;
    double      dblVal;
    CY          cyVal;
    // fRound is to deal with a bug in OA32.
    bool fRound = true;
    SPLIT64     sdlLo;
    SPLIT64     sdlHi;


    if (nBaseShift != 0) {
      // Hex or octal number.  Check for overflow case first.
      //
      int nBits = nBaseShift * cDig;  // no. of bits in number
      if (nBits > 66 || nBits == 66 && rgbDig[0] > 1)
        return RESULT(DISP_E_OVERFLOW);

      for ( ; cDig > 0; cDig--)
        ullVal = (ullVal << nBaseShift) + *pbDig++;

      llTmp = ullVal;  // llTmp & ullVal both used for storing result

      if (dwVtBits & VTBIT_INT) {
        if (dwVtBits & (VTBIT_I1 | VTBIT_UI1)) {
          if ((dwVtBits & VTBIT_I1) && (ullVal <= 0xFF || ullVal >= (ULONG)SCHAR_MIN))
            goto PickI1;
          if ((dwVtBits & VTBIT_UI1) && ullVal <= 0xFF)
            goto PickUI1;
        }
        if (dwVtBits & (VTBIT_I2 | VTBIT_UI2)) {
          if ((dwVtBits & VTBIT_I2) && (ullVal <= 0xFFFF || ullVal >= (ULONG)SHRT_MIN))
            goto PickI2;
          if ((dwVtBits & VTBIT_UI2) && ullVal <= 0xFFFF)
            goto PickUI2;
        }
        if (dwVtBits & (VTBIT_I4 | VTBIT_UI4)) {
          if ((dwVtBits & VTBIT_I4) && (ullVal <= 0xFFFFFFFF || ullVal >= (ULONG)INT_MIN))
            goto PickI4;
          if ((dwVtBits & VTBIT_UI4) && ullVal <= 0xFFFFFFFF)
            goto PickUI4;
        }
        if (dwVtBits & VTBIT_I8)
          goto PickI8;

        if (dwVtBits & VTBIT_UI8)
          goto PickUI8;

      }

      // Not using an integer type.
      //
      if (ullVal & UI64(0x8000000000000000)) {
        dwOutFlags |= NUMPRS_NEG;
        pnumprs->dwOutFlags = dwOutFlags;
        ullVal = -llTmp;
      }
      // To be compatible with OA32, if we are passed in a hex string
      // with 31st bit set, force it to be a negative number.
      else if (ullVal >= UI64(0x80000000) && ullVal < UI64(0x100000000)) {
          dwOutFlags |= NUMPRS_NEG;
          pnumprs->dwOutFlags = dwOutFlags;
          if (ullVal != UI64(0x80000000))
          {
              LONG lTmp = (LONG)ullVal;
              ullVal = -lTmp;
          }
      }

      if (ullVal >= UI64(10000000) && (dwVtBits & (VTBIT_R8 | VTBIT_CY | VTBIT_DECIMAL)))
        dwVtBits &= ~VTBIT_R4;  // don't use R4 if too big

      goto IntOvflow;
    }

    if (nPwr10 < 0)
      nDec = -nPwr10;  // set no. of decimal places (initialized to 0)

    // See if we can convert the digits to a long int.
    //
    nMaxDig = cDig;
    if ( !(dwVtBits & VTBIT_FRAC) ) {  // Accept fractions?
      nMaxDig -= nDec;  // no, don't convert decimal places
      nPwr10 += nDec;
    }

    if ((nMaxDig < 20 && !(dwVtBits & VTBIT_FRAC))
        || (nMaxDig <= 15 && (dwVtBits & VTBIT_FRAC))) {
      // Fits within a 64-bit integer
      //
      if (nMaxDig == 10 && *pbDig == 4)
        fRound = false;
      
//IntConv:
      cDig -= nMaxDig;
      for ( ; nMaxDig > 0; nMaxDig--)
        ullVal = ullVal * 10 + *pbDig++;

      // The number fit into a ULONG64 so far.  See if it needs scaling
      // or rounding.  (nMaxDig < 0 if number < .1)
      //
      if (cDig > 0 && nMaxDig == 0 && fRound) {
        // Need to round the integer
        //
        if (*pbDig > 5)
          ullVal++;

        else if (*pbDig++ == 5) {
          if (dwOutFlags & NUMPRS_INEXACT)
            goto RoundUp;

          for ( ; cDig > 1; cDig--)
            if (*pbDig++ != 0)
              goto RoundUp;

          // Round even.
          //
          if (ullVal & 1)
RoundUp:
            ullVal++;
        }
      }

      // Figure out what type to use.
      //
IntConvDone:
      if ( (dwVtBits & VTBIT_INT) && nIntDig <= 20 && (nDec == 0 || !(dwVtBits & VTBIT_FRAC)) ) {
        // Come here if 
        //   Integer type is selected AND
        //   the magnitude fits in 20 digits AND
        //     there are no decimal places OR
        //     no fractional type is acceptable.
        //
        // Might fit in an integer.
        //
        if (nPwr10 > 0) {
          ullTmp = ullVal * ulPower10[nPwr10];  // this could overflow

          if (nIntDig == 20 && rgbDig[0] >= 1) {
            // Make sure scaling didn't cause overflow.
            //
            if (rgbDig[0] > 1 || ullTmp < UI64(10000000000000000000))
              goto IntOvflow;
          }
          ullVal = ullTmp;
          nPwr10 = 0; // we've already scaled it.
        }

        llTmp = ullVal;  // make a signed copy
        if ((dwOutFlags & NUMPRS_NEG) && ullVal != 0) {
          dwVtBits &= ~VTBIT_UINT; // no UINTs if negative
          llTmp = -llTmp;    // set correct sign
        }

        if ( (dwVtBits & (VTBIT_I2 | VTBIT_UI1 | VTBIT_I1)) && ullVal <= SHRT_MAX+1 ) {
          // Try I2, UI1, I1
          //
          if ( (dwVtBits & (VTBIT_UI1 | VTBIT_I1)) && ullVal <= 255 ) {
            if ( (dwVtBits & VTBIT_I1) && ullVal <= 128 && llTmp < 128 ) {
PickI1:
              V_VT(pvar) = VT_I1;
              V_I1(pvar) = (signed char)llTmp;
              return NOERROR;
            }
            if (dwVtBits & VTBIT_UI1) {
PickUI1:
              V_VT(pvar) = VT_UI1;
              V_UI1(pvar) = (BYTE)ullVal;
              return NOERROR;
            }
          }

          if ( (dwVtBits & VTBIT_I2) && llTmp <= SHRT_MAX ) {
PickI2:
            V_VT(pvar) = VT_I2;
            V_I2(pvar) = (SHORT)llTmp;
            return NOERROR;
          }
        }

        if ( (dwVtBits & (VTBIT_I4 | VTBIT_UI4 | VTBIT_UI2)) && ullVal <= ULONG_MAX) {
          // Try UI4, I4, UI2
          //
          if ( (dwVtBits & VTBIT_UI2) && ullVal <= USHRT_MAX ) {
PickUI2:
            V_VT(pvar) = VT_UI2;
            V_UI2(pvar) = (USHORT)ullVal;
            return NOERROR;
          }

          if ( (dwVtBits & VTBIT_I4) && ullVal <= (ULONG)LONG_MAX+1 && llTmp <= LONG_MAX) {
PickI4:
            V_VT(pvar) = VT_I4;
            V_I4(pvar) = (LONG)llTmp;
            return NOERROR;
          }

          if ((dwVtBits & VTBIT_UI4) && ullVal <= ULONG_MAX) {
PickUI4:
            V_VT(pvar) = VT_UI4;
            V_UI4(pvar) = (ULONG)ullVal;
            return NOERROR;
          }

        
        }
        if ( (dwVtBits & VTBIT_I8) && llTmp <= _I64_MAX && llTmp >= _I64_MIN
             && !(llTmp < 0 && !(dwOutFlags & NUMPRS_NEG))) {
PickI8:
          V_VT(pvar) = VT_I8;
          V_I8(pvar) = llTmp;
          return NOERROR;
        }

        if ((dwVtBits & VTBIT_UI8) && ullVal <= _UI64_MAX) {
PickUI8:
          V_VT(pvar) = VT_UI8;
          V_UI8(pvar) = ullVal;
          return NOERROR;
        }
        
      }

IntOvflow:
      // It's either big or has a fraction.  Don't use integer type.
      // We know we have 20 or fewer digits.
      //
      // Pick the type that makes most sense.  Test types in
      // order of size, using the condition of no precision loss
      // OR no other type remains as a choice.
      //
      if ( (dwVtBits & (VTBIT_R4 | VTBIT_R8)) &&
           (!(dwVtBits & (VTBIT_CY | VTBIT_DECIMAL)) || nDec == 0 && 
           (nIntDig <= 7 || nIntDig <= 15 && (dwVtBits & VTBIT_R8))) ) {

PickDouble:

    dblVal = (double)ullVal;
        IfFailRet( MulPower10(&dblVal, 0, nPwr10) );

        if (dblVal > dsR4Max.dbl)
          dwVtBits &= ~VTBIT_R4;

        if (dwOutFlags & NUMPRS_NEG)
          dblVal = -dblVal;

        if ( (dwVtBits & VTBIT_R4) &&
             (!(dwVtBits & (VTBIT_R8 | VTBIT_CY | VTBIT_DECIMAL)) ||
             nIntDig <= 7 && nDec == 0) ) {
          V_VT(pvar) = VT_R4;
          V_R4(pvar) = (float)dblVal;
          return NOERROR;
        }

        if (dwVtBits & VTBIT_R8) {
          V_VT(pvar) = VT_R8;
          V_R8(pvar) = dblVal;
          return NOERROR;
        }
      }

      // If the optimal case of nDec <= 4 fails, and R8 & Decimal are
      // not available, then prefer CY over R4 if the total significant
      // digits of CY (nIntDig + 4) is > the significant digits of R4 (7).
      //
      if ( (dwVtBits & VTBIT_CY) && nIntDig <= 15 && 
           (!(dwVtBits & (VTBIT_R4 | VTBIT_R8 | VTBIT_DECIMAL)) || 
           nDec <= 4 || !(dwVtBits & (VTBIT_R8 | VTBIT_DECIMAL)) && 
           nIntDig > 3) ) {

        // Check for negative scaling.
        //
        nPwr10 += 4; // scale by 10000
        cyVal.u.Hi = 0;
        if (nPwr10 < 0) {
          ULONG ulScale;

          if (nPwr10 <= -10) {
            // Too small -- 0.43 or less (2^32/10^10).  Return 0.
            //
            cyVal.u.Lo = 0;
            goto ReturnCy;
          }
          ulScale = (ULONG)ulPower10[-nPwr10];
          cyVal.int64 = ullVal / ulScale;

          // Check for round up.
          //
          ullVal = (ullVal % ulScale) << 1;  // get remainder * 2
          if (ullVal > ulScale || ullVal == ulScale && 
              ((cyVal.int64 & 1) || (dwOutFlags & NUMPRS_INEXACT)))
            cyVal.int64++;
          goto SetSignCy;
        }

        // Positive scaling can be handled by large CY conversion routine.
        // If it ends up in overflow, it will be in the "big number" loop
        // and give Decimal a try if applicable.
        //
        dwVtBits &= VTBIT_FRAC; // set up for big number loop
        cyVal.int64 = ullVal;
        goto ScaleCy;
      }

      // If the optimal case of nDec <= DECMAX fails, then prefer 
      // Decimal over R4 if the total significant digits of Decimal
      // (cDig - (nDec - DECMAX)) is > the significant digits of R4 (7).
      //
      if ( (dwVtBits & VTBIT_DECIMAL) && nIntDig <= DEC_MAXDIG &&
           (!(dwVtBits & (VTBIT_R4 | VTBIT_R8)) || nDec <= DECMAX ||
           !(dwVtBits & VTBIT_R8) && pnumprs->cDig - nDec + DECMAX > 7) ) {

        // The large Decimal conversion routine builds scaling into
        // the digit conversion, so we need our own scaling here.
        //
        if (nPwr10 < 10) {
          if (nPwr10 > 0) {
            sdlLo.int64 = UInt32x32To64((ULONG)(ullVal&0xffffffff), (ULONG)ulPower10[nPwr10]);
            sdlHi.int64 = UInt32x32To64((ULONG)(ullVal>>32), (ULONG)ulPower10[nPwr10]);
            sdlHi.int64 += sdlLo.u.Hi;
            sdlLo.u.Hi = sdlHi.u.Lo;
            sdlHi.u.Lo = sdlHi.u.Hi;
            sdlHi.u.Hi = 0;
            nPwr10 = 0;
          }
          else  {
            sdlHi.u.Lo = 0;

            if (nPwr10 >= -DECMAX) {
              sdlLo.int64 = ullVal;
            }
            else if (nIntDig >= -DECMAX) {
              // Power is so negative that we can't use all the digits.
              //
              ULONG64 ulScale = ulPower10[-DECMAX - nPwr10];
              sdlLo.int64 = ullVal / ulScale;
              nPwr10 = -DECMAX;

              // Check for round up.
              //
              ullVal = (ullVal % ulScale) << 1;  // get remainder * 2
              if (ullVal > ulScale || ullVal == ulScale && 
                  ((sdlLo.u.Lo & 1) || (dwOutFlags & NUMPRS_INEXACT)))
                sdlLo.int64++;
            }
            else {
              sdlLo.u.Hi = 0;
              sdlLo.u.Lo = 0;
              nPwr10 = 0;
            }
          }
        }
        else {
          // Have a big power of 10.
          //
          if (nPwr10 > 18) {
            if (nPwr10 > 27) {
              ullVal = ullVal * ulPower10[nPwr10 - 27];
              nPwr10 = 27;
            }
            sdlLo.int64 = UInt32x32To64((ULONG)(ullVal&0xffffffff), (ULONG)ulPower10[nPwr10 - 18]);
            sdlHi.int64 = UInt32x32To64((ULONG)(ullVal>>32), (ULONG)ulPower10[nPwr10 - 18]);
            _ASSERTE (sdlHi.u.Hi == 0);
            sdlLo.u.Hi += sdlHi.u.Lo;
            sdlLo.int64 = UInt64x64To128(sdlLo, sdlTenToEighteen, &sdlHi.int64);

            if (sdlHi.u.Hi != 0) {
              nPwr10 = pnumprs->nPwr10;
              goto TryAgain;
            }
          }
          else {
            sdlLo.int64 = UInt32x32To64((ULONG)(ullVal&0xffffffff), (ULONG)ulPower10[nPwr10 - 9]);
            sdlHi.int64 = UInt32x32To64((ULONG)(ullVal>>32), (ULONG)ulPower10[nPwr10 - 9]);
            SPLIT64 sdlSave;
            sdlSave.u.Lo = sdlHi.u.Hi;
            sdlLo.u.Hi += sdlHi.u.Lo;
            sdlHi.int64 = UInt32x32To64(ulTenToNine, sdlLo.u.Hi);
            sdlLo.int64 = UInt32x32To64(ulTenToNine, sdlLo.u.Lo);
            sdlSave.int64 = UInt32x32To64(ulTenToNine, sdlSave.u.Lo);
            if (sdlSave.u.Hi != 0)
              goto TryAgain;
            
            sdlHi.int64 += sdlLo.u.Hi;
            sdlLo.u.Hi = sdlHi.u.Lo;
            sdlHi.u.Lo = sdlHi.u.Hi + sdlSave.u.Lo;
            sdlHi.u.Hi = 0;
          }
          nPwr10 = 0;
        }
        goto ReturnDec;
      }
TryAgain:

      if (dwVtBits & (VTBIT_R8 | VTBIT_R4))
        goto PickDouble;

      return RESULT(DISP_E_OVERFLOW);
    }
    else {
      // It might still fit into an I8 or UI8.  See if it can.
      //
      if (nMaxDig == 20  && !(dwVtBits & VTBIT_FRAC)) {
        if (*pbDig == 1) {
          // Special case for UI8 when very near to overflow.
          //
          for ( ullVal = 0; nMaxDig > 0; nMaxDig--)
            ullVal = ullVal * 10 + *pbDig++;

          if (ullVal >= UI64(10000000000000000000)) {
            cDig -= (int)(pbDig - rgbDig);
            goto IntConvDone;
          }
        }
      }

      // nMaxDig >= 20.  
      // Pick the type that makes most sense.  Test types in
      // order of size, using the condition of no precision loss
      // OR no other type remains as a choice.
      //
      dwVtBits &= VTBIT_FRAC;

      while (dwVtBits) {

        pbDig = rgbDig;  // start over
        cDig = pnumprs->cDig;
        nPwr10 = pnumprs->nPwr10;

        // Since we have at least 20 digits, R4 is never an optimal
        // choice -- use it only as last resort.
        //
        if ( !(dwVtBits & (VTBIT_CY | VTBIT_DECIMAL)) || 
             (dwVtBits & VTBIT_R8) && nDec == 0 && cDig <= 15 ) {

          // *********
          // Use R4/R8
          // *********
          //

          // Decimal number
          //

          double    dblValLo = 0;
          int       nMaxDig;

          dblVal = 0;
          nMaxDig = min(cDig, 15);
          cDig -= nMaxDig;

          for ( ; nMaxDig > 0; nMaxDig--)
            dblVal = dblVal * 10.0 + *pbDig++;

          if (cDig > 0) {
            nMaxDig = cDig;
            for ( ; cDig > 0; cDig--)
              dblValLo = dblValLo * 10.0 + *pbDig++;

            dblValLo /= fnDblPower10(nMaxDig);
            nPwr10 += nMaxDig;
          }

          IfFailRet( MulPower10(&dblVal, dblValLo, nPwr10) );

          // Set sign
          //
          if (dblVal > dsR4Max.dbl)
            dwVtBits &= ~VTBIT_R4;

          if (dwOutFlags & NUMPRS_NEG)
            dblVal = -dblVal;

          if (dwVtBits & VTBIT_R8) {
            V_VT(pvar) = VT_R8;
            V_R8(pvar) = dblVal;
            return NOERROR;
          }
          else if (dwVtBits & VTBIT_R4) {
            V_VT(pvar) = VT_R4;
            V_R4(pvar) = (float)dblVal;
            return NOERROR;
          }
          return RESULT(DISP_E_OVERFLOW);

        } // Try R8

        // If the optimal case of nDec <= 4 fails, and Decimal is not
        // available, then prefer CY over R4/R8 if the total significant
        // digits of CY (nIntDig + 4) is > the significant digits of 
        // R4/R8 (7/15).
        //
        if ( (dwVtBits & VTBIT_CY) && nIntDig <= 15 && 
             (!(dwVtBits & (VTBIT_R4 | VTBIT_R8 | VTBIT_DECIMAL)) || 
             nDec <= 4 || !(dwVtBits & VTBIT_DECIMAL) && nIntDig > 11 ||
             !(dwVtBits & VTBIT_R8) && nIntDig > 3) ) {

          // ******
          // Try CY
          // ******
          //
          CY  cyTmp;

          cyVal.u.Lo = 0;
          cyVal.u.Hi = 0;

          // Apply the scale factor first
          //
          nPwr10 += 4;  // scale by 10^4

          // Compute the number of digits to convert.  This is limited to
          // integer digits and the number of digits we actually have.  If
          // this is more than 9, we'll need to break up the conversion
          // into blocks of up to 9 digits each.
          //
          nMaxDig = min( max(nIntDig+4, 0), cDig );
          cDig -= nMaxDig;  // non-integer digits left for rounding
          nPwr10 += cDig;   // increase power for digits not converted

          if (nMaxDig > 9) {
            if (nMaxDig > 18) {
              cyTmp.u.Lo = *pbDig++;
              cyTmp.u.Hi = 0;
              VarCyMulI4(cyTmp, (ULONG)ulPower10[9], &cyTmp); // can't overflow

              nMaxDig--;
              for ( ; nMaxDig > 9; nMaxDig--)
                cyVal.u.Lo = cyVal.u.Lo * 10 + *pbDig++;

              cyVal.u.Lo += cyTmp.u.Lo;
              cyVal.u.Hi += cyTmp.u.Hi;
              if (cyVal.u.Lo < cyTmp.u.Lo) // got a carry?
                cyVal.u.Hi++;
            }
            else
              for ( ; nMaxDig > 9; nMaxDig--)
                cyVal.u.Lo = cyVal.u.Lo * 10 + *pbDig++;

            if ( FAILED(VarCyMulI4(cyVal, (ULONG)ulPower10[9], &cyVal)) )
              goto CyOvflow;
          }

          ullVal = 0;
          for ( ; nMaxDig > 0; nMaxDig--) {
            ullVal = ullVal * 10 + *pbDig++;
          }

          cyVal.u.Lo += ULONG(ullVal);
          if (cyVal.u.Lo < ullVal) // got a carry?
            cyVal.u.Hi++;

          // Scale by power of 10 if necessary.  Enter here from integer
          // code.
          //
ScaleCy:
          if (nPwr10 > 0) {
            while (nPwr10 > 9) {
              if ( FAILED(VarCyMulI4(cyVal, (ULONG)ulPower10[9], &cyVal)) )
                goto CyOvflow;
              nPwr10 -= 9;
            }
            if ( FAILED(VarCyMulI4(cyVal, (ULONG)ulPower10[nPwr10], &cyVal)) )
              goto CyOvflow;
          }
          else if (cDig > 0 && nPwr10 == 0) {
            // Need to round the integer
            //
            if (*pbDig > 5)
              goto RoundUpCy;

            else if (*pbDig++ == 5) {
              if (dwOutFlags & NUMPRS_INEXACT)
                goto RoundUpCy;

              for ( ; cDig > 1; cDig--)
                if (*pbDig++ != 0)
                  goto RoundUpCy;
              // Round even.
              //
              if (cyVal.u.Lo & 1) {
RoundUpCy:
                cyVal.u.Lo++;
                if (cyVal.u.Lo == 0)
                  cyVal.u.Hi++;
              }
            }
          }
SetSignCy:
          // Set sign and check for overflow.
          //
          if (dwOutFlags & NUMPRS_NEG) {
            if (cyVal.u.Lo == 0)
              cyVal.u.Hi = -cyVal.u.Hi;
            else {
              cyVal.u.Lo = -(LONG)cyVal.u.Lo;
              cyVal.u.Hi = ~cyVal.u.Hi;
            }

            if (cyVal.u.Hi > 0 )
              goto CyOvflow;
          }
          else if (cyVal.u.Hi < 0 ) {
CyOvflow:
            dwVtBits &= ~VTBIT_CY;
            continue;  // try another data type
          }
ReturnCy:
          V_VT(pvar) = VT_CY;
          V_CY(pvar) = cyVal;
          return NOERROR;

        } // Try CY

        // If the optimal case of nDec <= DECMAX fails, then prefer 
        // Decimal over R4/R8 if the total significant digits of Decimal
        // (cDig - (nDec - DECMAX)) is > the significant digits of 
        // R4/R8 (7/15).
        //
        if ( (dwVtBits & VTBIT_DECIMAL) && nIntDig <= DEC_MAXDIG &&
             (!(dwVtBits & (VTBIT_R4 | VTBIT_R8)) || nDec <= DECMAX ||
             (nMaxDig = cDig - nDec + DECMAX) > 15 ||
             !(dwVtBits & VTBIT_R8) && nMaxDig > 7) ) {

          // ***********
          // Try Decimal
          // ***********
          //
          DWORDLONG  dlTmp;
          int        nHiDig;

          // Compute the number of digits to convert.  This is limited to
          // digits before the DECMAX decimal place, the number of digits  
          // we actually have, and DEC_MAXDIG (the max digits that will fit 
          // into 96 bits).  If this is more than 9, we'll need to break 
          // up the conversion into blocks of up to 9 digits each.
          //
          nMaxDig = min(min(max(nIntDig+DECMAX, 0), cDig), DEC_MAXDIG);
          cDig -= nMaxDig;           // digits left for rounding
          nHiDig = max(nIntDig, nMaxDig);   // power of hi digit
          nPwr10 += cDig;               // power of lowest digit we use

          sdlLo.int64 = 0;
          sdlHi.int64 = 0;
          ullVal = 0;

          if (nIntDig < -DECMAX)
            nPwr10 = 0;

          else {

            if (nHiDig > 9) {
              // nHiDig is the same as nMaxDig unless there is a positive
              // exponent on the number.  It's as if the trailing zeros
              // equivalent to the exponent were added on.  We don't want 
              // to convert the digits, then multiply by the exponent in 
              // a separate step.
              //
              // We can't necessarily convert a 29-digit number if the 29
              // digits exceed 79228162514264337593543950335. If some of the
              // digits are fractional because of a decimal point, we should
              // simply convert 1 less digit.  We need to a compare for this
              // value in advance so we know how many digits we're converting.
              // This test must include a test for rounding up, taking advantage
              // of the fact that the max value is odd, so "round even" is 
              // always "round up" if the next digit is 5.
              //
              LONG lTmp;
              if (nMaxDig == DEC_MAXDIG && rgbDig[0] >= 7 && 
                  (rgbDig[0] > 7 || (lTmp = memcmp(rgbDig+1, rgbMaxDec, DEC_MAXDIG - 1)) > 0 || 
                  lTmp == 0 && cDig > 0 && rgbDig[DEC_MAXDIG] >= 5)) {

                // Have the overflow case.
                //
                if (nIntDig >= nMaxDig)
                  goto DecOvFlow;

                // We're not actually out of range. Just chop off the last digit.
                //
                nMaxDig--;// number of digits to convert
                nHiDig--;       // power of highest digit
                cDig++; // digits left for rounding
                nPwr10++;       // power of lowest digit we use
              }

              // Look for digits 28 to nMaxDig.
              //
              for ( ; nHiDig > 27 && nMaxDig > 0; nHiDig--, nMaxDig--)
                ullVal = ullVal * 10 + *pbDig++;

              if (ullVal > 0) {
                sdlLo.int64 += UInt32x32To64(ulTenToNine, (ULONG)ullVal);
                ullVal = 0;
              }

              // Now look for digits 19 - 27.
              //
              for ( ; nHiDig > 18 && nMaxDig > 0; nHiDig--, nMaxDig--)
                ullVal = ullVal * 10 + *pbDig++;

                if (nHiDig > 18)
                        ullVal *= ulPower10[nHiDig - 18];


              sdlLo.int64 += ullVal;
              if (sdlLo.int64 != 0) {
                sdlLo.int64 = UInt64x64To128(sdlLo, sdlTenToEighteen, &sdlHi.int64);
                ullVal = 0;
              }

              // 9 <= nHiDig <= 18.
              //
              for ( ; nHiDig > 9 && nMaxDig > 0; nHiDig--, nMaxDig--)
                ullVal = ullVal * 10 + *pbDig++;

              if (ullVal > 0) {
                dlTmp = ulTenToNine * ullVal;

                if (nHiDig > 9)
                        dlTmp *= ((DWORDLONG)ulPower10[nHiDig - 9]);

                sdlLo.int64 += dlTmp;
                if (sdlLo.int64 < dlTmp)
                  sdlHi.int64++;
                ullVal = 0;
              }
            }

            // nHiDig <= 9, meaning what's left is < 1E9.
            //
            for ( ; nMaxDig > 0; nMaxDig--)
              ullVal = ullVal * 10 + *pbDig++;

            if (nPwr10 > 0) {
              dlTmp = ullVal * ulPower10[nPwr10];
              nPwr10 = 0;
            }
            else
              dlTmp = ullVal;

            sdlLo.int64 += dlTmp;
            if (sdlLo.int64 < dlTmp)
              sdlHi.int64++;

            if (sdlHi.u.Hi != 0)
              goto DecOvFlow;

            // Round result
            //
            if (cDig > 0) {
              // Need to round
              //
              if (*pbDig > 5)
                goto RoundUpDec;

              else if (*pbDig++ == 5) {
                if (dwOutFlags & NUMPRS_INEXACT)
                  goto RoundUpDec;

                for ( ; cDig > 1; cDig--)
                  if (*pbDig++ != 0)
                    goto RoundUpDec;
                // Round even.
                //
                if ( (sdlLo.int64 & 1) == 1 ) {
RoundUpDec:
                  if (++sdlLo.int64 == 0 && ++sdlHi.u.Lo == 0) {
DecOvFlow:
                    dwVtBits &= ~VTBIT_DECIMAL;
                    continue;
                  }
                }
              }
            }
          }


ReturnDec:
          V_VT(pvar) = VT_DECIMAL;
          DECIMAL_LO32(pvar->n1.decVal) = sdlLo.u.Lo;
          DECIMAL_MID32(pvar->n1.decVal) = sdlLo.u.Hi;
          DECIMAL_HI32(pvar->n1.decVal) = sdlHi.u.Lo;
          DECIMAL_SCALE(pvar->n1.decVal) = -nPwr10;

          // Set sign.
          //
          if (dwOutFlags & NUMPRS_NEG)
            DECIMAL_SIGN(pvar->n1.decVal) = DECIMAL_NEG;
          else
            DECIMAL_SIGN(pvar->n1.decVal) = 0;

          return NOERROR;


        } // Try Decimal

        dwVtBits &= ~(VTBIT_DECIMAL | VTBIT_CY);

      } // while dwVtBits

      return RESULT(DISP_E_OVERFLOW);
    }
}


STDAPI VarCyMulI4(CY cyLeft, long lRight, LPCY pcyResult)
{
    CY	  cyRes, cyTmp;

    cyRes.int64 = UInt32x32To64(cyLeft.u.Lo, lRight);
    cyTmp.int64 = UInt32x32To64(cyLeft.u.Hi, lRight);
    cyTmp.int64 += cyRes.u.Hi;    // Sum partial products

    // Perform corrections for negative operands.
    //
    if (lRight < 0)
      cyTmp.int64 -= cyLeft.int64;

    if (cyLeft.u.Hi < 0)
      cyTmp.u.Hi -= lRight;

    cyRes.u.Hi = cyTmp.u.Lo;

    // Check for overflow.  cyTmp.Hi should be sign extension
    // of cyTmp.Lo.
    //
    if (cyTmp.int64 != (__int64)(long)cyTmp.u.Lo)
      return DISP_E_OVERFLOW;

    *pcyResult = cyRes;
    return NOERROR;
}
