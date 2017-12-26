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
// File: variant.cpp
// 
// ===========================================================================
/*++
    

Abstract:

    PALRT variant conversion functions

Author:


Revision History:

--*/

#include "rotor_palrt.h"

#include "oleauto.h"

#include "oautil.h"
#include "convert.h"
#include <math.h>

int rgiPower10[4] = {10000, 1000, 100, 10};

#define C2TO32TH	4294967296.0

double g_dblTenThousand = 10000.0;	// make this a static in order to
					// prevent compilers from trying to
					// "optimize" a divide by 10000.0 to be
					// a multiply by 1/10000.0, which will
					// cause us to loose accuracy.

/***
*PUBLIC void VariantInit(VARIANT*)
*Purpose:
*  Initialize the given VARIANT to VT_EMPTY.
*
*Entry:
*  None
*
*Exit:
*  return value = void
*
*  pvarg = pointer to initialized VARIANT
*
***********************************************************************/
STDAPI_(void)
VariantInit(VARIANT FAR* pvarg)
{
    V_VT(pvarg) = VT_EMPTY;
}

STDAPI VarCyFromR4(FLOAT fltIn, CY * pcyOut)
{
  return VarCyFromR8(fltIn, pcyOut);
}

STDAPI VarCyFromR8(DOUBLE dblInput, CY * pcyOut)
{
   BOOL	fNegative = FALSE;
   double dblHi, dblLo;
   float flt;
   double dblDif;

   // test for overflow first
   // [Note: We are counting on the compiler rounding the following numbers
   // correctly (by IEEE rules to the nearest R8).  The magnitude of these
   // numbers are rounded up and, thus, are always outside the legal range
   // for currency.
   if (dblInput >= 922337203685477.58 ||
       dblInput <= -922337203685477.58)
     return RESULT(DISP_E_OVERFLOW);

   // if negative, set flag and make positive
   if (dblInput < 0.0) {
     fNegative = TRUE;
     dblInput = -dblInput;
   }

   // In order to maintain the necessary precision when multiplying
   // by 10000 (i.e., going from 53-bit to 64-bit), split the
   // input value into two different doubles and perform calcs using
   // them:
   //
   //   dblHi = has low bits 0
   //   dblLo = has high bits 0
   //

   // split input into two parts
   // Note: compiler doesn't do the right thing with this:
   //       "dblHi = (double) (float) dblInput"
   flt = (float) dblInput;
   dblHi = (double) flt;        // input rounded to 24-bit precision
   dblLo = dblInput - dblHi;    // diff between 24- and 53-bit input value

   // bias for currency

   dblHi = dblHi * 10000;
   dblLo = dblLo * 10000;


   // calculate cy.Hi
   pcyOut->u.Hi = (long) ((dblLo + dblHi) / C2TO32TH);

   // calculate cy.Lo
   dblHi -= (((double) pcyOut->u.Hi) * C2TO32TH);
   pcyOut->u.Lo = (unsigned long) (dblLo + dblHi);

   // round as necessary
   dblHi -= (double)(pcyOut->u.Lo) + 0.5;
   dblDif = dblLo + dblHi;
   if ( (dblDif > 0) || ((dblDif == 0) && (pcyOut->u.Lo & 1)) ) {
     pcyOut->u.Lo++;
     if (pcyOut->u.Lo == 0)
       pcyOut->u.Hi++;
   }

   // negate the result if input was negative
   if (fNegative) {
     pcyOut->u.Hi = ~pcyOut->u.Hi;
     if ((pcyOut->u.Lo = (unsigned long)(-(long)pcyOut->u.Lo)) == 0)
       pcyOut->u.Hi++;
   }

   return NOERROR;
}

STDAPI VarCyAdd(CY cyLeft, CY cyRight, LPCY pcyResult)
{
    CY	  cyRes;

    cyRes.int64 = cyLeft.int64 + cyRight.int64;
    if (cyRes.int64 < cyLeft.int64)
      return DISP_E_OVERFLOW;

    *pcyResult = cyRes;
    return NOERROR;
}

STDAPI VarCySub(CY cyLeft, CY cyRight, LPCY pcyResult)
{
    CY	  cyRes;

    cyRes.int64 = cyLeft.int64 - cyRight.int64;
    if (cyRes.int64 > cyLeft.int64)
      return DISP_E_OVERFLOW;

    *pcyResult = cyRes;
    return NOERROR;
}

STDAPI VarCyMul(CY cyLeft, CY cyRight, LPCY pcyResult)
{
    CY	  cyRes, cyTmp1, cyTmp2, cyTmp3;

    cyRes.int64 = UInt32x32To64(cyLeft.u.Lo, cyRight.u.Lo);
    cyTmp1.int64 = UInt32x32To64(cyLeft.u.Hi, cyRight.u.Lo);
    cyTmp2.int64 = UInt32x32To64(cyLeft.u.Lo, cyRight.u.Hi);
    cyTmp3.int64 = UInt32x32To64(cyLeft.u.Hi, cyRight.u.Hi);

    // Sum partial products
    //
    cyTmp1.int64 += cyRes.u.Hi;
    cyTmp3.int64 += cyTmp1.u.Hi;
    cyTmp1.u.Hi = 0;
    cyTmp2.int64 += cyTmp1.int64;
    cyTmp3.int64 += cyTmp2.u.Hi;

    // 128-bit result in cyTmp3:cyTmp2.Lo:cyRes.Lo.
    // Perform corrections for negative operands.
    //
    if (cyLeft.u.Hi < 0)
      cyTmp3.int64 -= cyRight.int64;

    if (cyRight.u.Hi < 0)
      cyTmp3.int64 -= cyLeft.int64;

    // Remove extra scale factor of 10000
    //
    cyTmp2.u.Hi = (long)(cyTmp3.int64 % 10000);
    cyTmp3.int64 /= 10000;

    cyRes.u.Hi = (long)(cyTmp2.int64 % 10000);
    cyTmp2.u.Lo = (long)(cyTmp2.int64 / 10000); // must fit in 32 bits

    cyTmp1.u.Lo = (long)(cyRes.int64 % 10000);
    cyRes.u.Lo = (long)(cyRes.int64 / 10000); // must fit in 32 bits

    cyRes.u.Hi = cyTmp2.u.Lo;

    // 128-bit result in cyTmp3:cyRes.  Round according
    // to remainder in cyTmp1.Lo.  Term "Lo & 1" causes
    // round up if odd and exactly halfway (IEEE rounding).
    //
    if (cyTmp1.u.Lo + (cyRes.u.Lo & 1) > 5000)
    {
      cyRes.int64++;
      if (cyRes.int64 == 0)
        cyTmp3.int64++;
    }

    // Check for overflow.  cyTmp3 should be sign extension
    // of cyRes.
    //
    if (cyRes.u.Hi < 0)
      cyTmp3.int64++;

    if (cyTmp3.int64 != 0)
      return DISP_E_OVERFLOW;

    *pcyResult = cyRes;
    return NOERROR;
}

STDAPI VarCyInt(CY cyIn, LPCY pcyResult)
{
    int   iFrac;

    iFrac = (int)(cyIn.int64 % 10000);
    if (iFrac != 0)
    {
      if (cyIn.u.Hi < 0)
        iFrac += 10000;
      cyIn.int64 -= iFrac;
    }

    *pcyResult = cyIn;
    return NOERROR;
}

STDAPI VarCyRound(CY cyIn, int cDecimals, LPCY pcyResult)
{
    if (cDecimals < 0)
      return RESULT(E_INVALIDARG);

    if (cDecimals >= 4)
    {
      *pcyResult = cyIn;
       return NOERROR;
    }

    long      lRem;
    long      lPwr;

    lPwr = rgiPower10[cDecimals];
    lRem = (long)(cyIn.int64 % lPwr) * 2;
    cyIn.int64 /= lPwr;

    if (labs(lRem) != lPwr || (cyIn.u.Lo & 1))
      cyIn.int64 += lRem / lPwr;

    // Multiply could overflow -- check for it by looking for sign change.
    //
    lRem = cyIn.u.Hi;
    cyIn.int64 *= lPwr;
    if ((cyIn.u.Hi ^ lRem) & 0x80000000)
      return RESULT(DISP_E_OVERFLOW);

    *pcyResult = cyIn;

    return NOERROR;
}

STDAPI VarCyFix(CY cyIn, LPCY pcyResult)
{
    pcyResult->int64 = cyIn.int64 - (cyIn.int64 % 10000);
    return NOERROR;
}


/***
* VarR8FromCy - convert R8 from currency
* Purpose:
*	The specified currency value is scaled to a
*   double-precision real value.
*
* Entry:
*	cyInput - currency input value
*
* Exit:
*	Returns a eight-byte real number.
*
* Exceptions:
*
***********************************************************************/

STDAPI VarR8FromCy(CY cyInput, double FAR* pdblOut)
{
  *pdblOut = (double)cyInput.int64 / g_dblTenThousand;
  return NOERROR;
}


/***
* VarR4FromCy - convert R4 from Currenty
* Purpose:
*	The specified currency value is scaled to a
*   single-precision real value.
*
* Entry:
*	cyInput - currency input value
*
* Exit:
*	Returns a four-byte real number.
*
* Exceptions:
*
***********************************************************************/

STDAPI VarR4FromCy(CY cyInput, float FAR* pfltOut)
{
  *pfltOut = (float) ((double)cyInput.int64 / g_dblTenThousand);
  return NOERROR;
}

#define VT_VMAX2 (VT_UINT+1)
#define VTSW(vtSrc, vtDst) ((vtSrc) * VT_VMAX2 + (vtDst))

/***
*LOCAL HRESULT IsLegalVartype(VARTYPE)
*Purpose:
*  Determines if the given vt is a legal VARTYPE.
*
*Entry:
*  vt = VARTYPE to check
*
*Exit:
*  return value = HRESULT
*    NOERROR
*    DISP_E_BADVARTYPE
*
***********************************************************************/
HRESULT IsLegalVartype(VARTYPE vt)
{

    _ASSERTE (vt >= VT_VARIANT);	// caller should have checked for all
    // vt's < VT_VARIANT for speed.

    // NOTE: optimized for speed, rather than for maintainablity
    if  (vt & (VT_BYREF | VT_ARRAY))
    {
        vt &= ~(VT_BYREF | VT_ARRAY);
    }

    if  ((vt >= VT_I2 && vt <= VT_DECIMAL)  || 
         (vt >= VT_I1 && vt <= VT_UI8)      ||
         vt == VT_INT                       ||
         vt == VT_UINT                      || 
         vt == VT_RECORD)
    {
        return NOERROR;
    }

    return RESULT(DISP_E_BADVARTYPE);
}

/***
*PUBLIC HRESULT VariantClear(VARIANTARG FAR*)
*Purpose:
*  Set the variant to nothing, releaseing any string or object
*  reference owned by that variant.
*
*Entry:
*  pvarg = the VARIANTARG to set to VT_EMPTY
*
*Exit:
*  return value = HRESULT
*    NOERROR
*    E_INVALIDARG
*    DISP_E_BADVARTYPE
*    DISP_E_ARRAYISLOCKED
*
*Note:
*  We dont release or clear anything thats ByRef. These aren't
*  owned by the variant, but by what the variant points at.
*
***********************************************************************/
STDAPI
VariantClear(VARIANTARG FAR* pvarg)
{

    if (V_VT(pvarg) == VT_BSTR) {
        SysFreeString(V_BSTR(pvarg));
    }
    else
    if (V_VT(pvarg) == VT_UNKNOWN && V_UNKNOWN(pvarg) != NULL) {
        V_UNKNOWN(pvarg)->Release();
    }        

    V_VT(pvarg) = VT_EMPTY;
    return NOERROR;
}

/***
*PUBLIC HRESULT VariantChangeType
*Purpose:
*  This function changes the data type of a VARIANTARG to the given vt.
*  If the variant in initailly BYREF, it is converted to a VARIANT that
*  is not BYREF.
*
*Entry:
*   pargSrc = points to VARIANTARG to be converted.
*   vt = desired type of variant.
*
*Exit:
*  return value = HRESULT
*    NOERROR
*    E_INVALIDARG
*    E_OUTOFMEMORY
*    RESULT(DISP_E_OVERFLOW)
*    DISP_E_BADVARTYPE
*    DISP_E_TYPEMISMATCH
*
*  *pargDest = contains the converted value.
*
***********************************************************************/

STDAPI
VariantChangeType(
    VARIANTARG FAR* pvargDest,
    VARIANTARG FAR* pvargSrc,
    unsigned short wFlags,
    VARTYPE vt)
{
    VARIANT varTmp;
    HRESULT hresult;
    VARTYPE vtSrc;

    if (pvargDest == NULL || pvargSrc == NULL)
      return(E_INVALIDARG);

    vtSrc = V_VT(pvargSrc); // get src vartype

    hresult = NOERROR;          // assume success
    // NOTE: all code after this point must go to LError0 upon Error.

    // should have validated the destination type at this point (to ensure
    // that the below optimized switch statement will work properly).
    switch ( VTSW(vtSrc, vt) )
    {
      case VTSW(VT_BOOL, VT_BOOL):
      case VTSW(VT_I2, VT_I2):
      case VTSW(VT_I4, VT_I4):
      case VTSW(VT_INT, VT_INT):
      case VTSW(VT_I8, VT_I8):
      case VTSW(VT_R4, VT_R4):
      case VTSW(VT_R8, VT_R8):
      case VTSW(VT_CY, VT_CY):
      case VTSW(VT_DECIMAL, VT_DECIMAL):
      case VTSW(VT_I1, VT_I1):
      case VTSW(VT_UI1, VT_UI1):
      case VTSW(VT_UI2, VT_UI2):
      case VTSW(VT_UI4, VT_UI4):
      case VTSW(VT_UINT, VT_UINT):
      case VTSW(VT_UI8, VT_UI8):
        varTmp = *pvargSrc;
        break;

      case VTSW(VT_EMPTY, VT_BSTR):
        hresult = ErrSysAllocString(OASTR(""), &V_BSTR(&varTmp));
        break;

      case VTSW(VT_BOOL, VT_BSTR):
        //fall through to return "0" or "-1"
      case VTSW(VT_I2, VT_BSTR):
        hresult = VarBstrFromI2(V_I2(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_I4, VT_BSTR):
      case VTSW(VT_INT, VT_BSTR):
        hresult = VarBstrFromI4(V_I4(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_I8, VT_BSTR):
        hresult = VarBstrFromI8(V_I8(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_R4, VT_BSTR):
        hresult = VarBstrFromR4(V_R4(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;


      case VTSW(VT_R8, VT_BSTR):
        hresult = VarBstrFromR8(V_R8(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_CY, VT_BSTR):
        hresult = VarBstrFromCy(V_CY(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_DECIMAL, VT_BSTR):
        hresult = VarBstrFromDec(&pvargSrc->n1.decVal, LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_I1, VT_BSTR):
        hresult = VarBstrFromI1(V_I1(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_UI1, VT_BSTR):
        hresult = VarBstrFromUI1(V_UI1(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_UI2, VT_BSTR):
        hresult = VarBstrFromUI2(V_UI2(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_UI4, VT_BSTR):
      case VTSW(VT_UINT, VT_BSTR):
        hresult = VarBstrFromUI4(V_UI4(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_UI8, VT_BSTR):
        hresult = VarBstrFromUI8(V_UI8(pvargSrc), LOCALE_USER_DEFAULT, 0, &V_BSTR(&varTmp));
        break;

      case VTSW(VT_BSTR, VT_BSTR):
        hresult = ErrSysAllocString(V_BSTR(pvargSrc), &V_BSTR(&varTmp));
        break;

      case VTSW(VT_DATE, VT_BSTR):
          _ASSERTE(false);
          hresult = RESULT(DISP_E_TYPEMISMATCH);
          break;

      default:
        if (vtSrc >= VT_VARIANT) {
          IfFailGo(IsLegalVartype(vtSrc), LError0);
        }

        // everybody else gets a type mis-match error
        _ASSERTE(false);
        hresult = RESULT(DISP_E_TYPEMISMATCH);
        break;
    }

    if (FAILED(hresult))
    {
        goto LError0;
    }

    // now set the tag in temp variant.  Errors after this point have to
    // goto LError1 to potentially clear this variant.
    V_VT(&varTmp) = vt;

    // now that we have succeeded, we can go ahead and destroy our
    // destination variant.
    if (V_VT(pvargDest) >= VT_BSTR) {
        IfFailGo(VariantClear(pvargDest), LError1);
    }

    // copy in the variant we just created
    memcpy(pvargDest, &varTmp, sizeof(VARIANT));

    return NOERROR;

LError1:;
    VariantClear(&varTmp);
    // fall through

LError0:;
    return hresult;
}
