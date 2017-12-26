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
#include <common.h>

#include "comfloat.h"
#include "comfloatclass.h"

#define IS_DBL_INFINITY(x) ((*((UINT64 *)((void *)&x)) & UI64(0x7FFFFFFFFFFFFFFF)) == UI64(0x7FF0000000000000))
#define IS_DBL_ONE(x)      ((*((UINT64 *)((void *)&x))) == UI64(0x3FF0000000000000))
#define IS_DBL_NEGATIVEONE(x)      ((*((UINT64 *)((void *)&x))) == UI64(0xBFF0000000000000))




#ifdef _MSC_VER
#pragma float_control(precise, off)
#endif

/*====================================Floor=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Floor, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) floor(d);
FCIMPLEND


/*====================================Ceil=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Ceil, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) ceil(d);
FCIMPLEND

/*=====================================Sqrt=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Sqrt, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) sqrt(d);
FCIMPLEND

/*=====================================Acos=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Acos, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) acos(d);
FCIMPLEND


/*=====================================Asin=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Asin, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) asin(d);
FCIMPLEND


/*=====================================AbsFlt=====================================
**
==============================================================================*/
FCIMPL1_V(float, COMDouble::AbsFlt, float f) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    FCUnique(0x14);
    return fabsf(f);
FCIMPLEND

/*=====================================AbsDbl=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::AbsDbl, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return fabs(d);
FCIMPLEND

/*=====================================Atan=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Atan, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) atan(d);
FCIMPLEND

/*=====================================Atan2=====================================
**
==============================================================================*/
FCIMPL2_VV(double, COMDouble::Atan2, double x, double y) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

        // the intrinsic for Atan2 does not produce Nan for Atan2(+-inf,+-inf)
    if (IS_DBL_INFINITY(x) && IS_DBL_INFINITY(y)) {
        return(x / y);      // create a NaN
    }
    return (double) atan2(x, y);
FCIMPLEND

/*=====================================Sin=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Sin, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) sin(d);
FCIMPLEND

/*=====================================Cos=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Cos, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) cos(d);
FCIMPLEND

/*=====================================Tan=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Tan, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) tan(d);
FCIMPLEND

/*=====================================Sinh====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Sinh, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) sinh(d);
FCIMPLEND

/*=====================================Cosh====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Cosh, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) cosh(d);
FCIMPLEND

/*=====================================Tanh====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Tanh, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) tanh(d);
FCIMPLEND

FCIMPL1(double, COMDouble::ModFDouble, double* pdblValue)
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    double      dblFrac;
    dblFrac = modf(*pdblValue, pdblValue);
    return dblFrac;
FCIMPLEND

#ifdef _MSC_VER
#pragma float_control(precise, on )
#endif

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///
///                         End of /fp:fast scope    
///
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// Log, Log10 and Exp are slower with /fp:fast on SSE2 enabled HW (see #500373)
// So we'll leave them as fp precise for the moment

/*=====================================Log======================================
**This is the natural log
==============================================================================*/
FCIMPL1_V(double, COMDouble::Log, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) log(d);
FCIMPLEND


/*====================================Log10=====================================
**This is log-10
==============================================================================*/
FCIMPL1_V(double, COMDouble::Log10, double d) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (double) log10(d);
FCIMPLEND


/*=====================================Exp======================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Exp, double x) 
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

        // The C intrinsic below does not handle +- infinity properly
        // so we handle these specially here
    if (IS_DBL_INFINITY(x)) {
        if (x < 0)      
            return(+0.0);
        return(x);      // Must be + infinity
    }
    return((double) exp(x));
FCIMPLEND

FCIMPL2_VV(double, COMDouble::Pow, double x, double y)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    double r1;
    if(IS_DBL_INFINITY(y)) {
        if(IS_DBL_ONE(x)) {
            return x;        
        }

        if(IS_DBL_NEGATIVEONE(x)) {
            *((INT64 *)(&r1)) = CLR_NAN_64;
            return r1;
        }    
    }
    else if(_isnan(y) || _isnan(x)) {
        *((INT64 *)(&r1)) = CLR_NAN_64;
        return r1;
    }
    
    return (double) pow(x, y);
}
FCIMPLEND



/*====================================Round=====================================
**
==============================================================================*/
FCIMPL1_V(double, COMDouble::Round, double d) 
    LEAF_CONTRACT;
    double tempVal;
    double flrTempVal;
    // If the number has no fractional part do nothing
    // This shortcut is necessary to workaround precision loss in borderline cases on some platforms
    if ( d == (double)(__int64)d )
        return d;
    tempVal = (d+0.5);
    //We had a number that was equally close to 2 integers. 
    //We need to return the even one.
    flrTempVal = floor(tempVal);
    if (flrTempVal==tempVal) {
        if (0 != fmod(tempVal, 2.0)) {
            flrTempVal -= 1.0;
        }
    }
    flrTempVal = _copysign(flrTempVal, d);
    return flrTempVal;
FCIMPLEND


