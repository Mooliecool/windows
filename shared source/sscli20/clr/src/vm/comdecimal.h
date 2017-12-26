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
#ifndef _COMDECIMAL_H_
#define _COMDECIMAL_H_

#include <oleauto.h>

#include <pshpack1.h>

#include "comnumber.h"

#define DECIMAL_PRECISION 29

class COMDecimal {
public:
    static FCDECL2_IV(void, InitSingle, DECIMAL *_this, float value);
    static FCDECL2_IV(void, InitDouble, DECIMAL *_this, double value);
    static FCDECL2_VV(INT32, Compare, DECIMAL d1, DECIMAL d2);
    static FCDECL1(INT32, GetHashCode, DECIMAL *d);

    static FCDECL3_IVV(void, DoAdd, DECIMAL * result, DECIMAL d1, DECIMAL d2);
    static FCDECL3_IVV(void, DoSubtract, DECIMAL * result, DECIMAL d1, DECIMAL d2);
    static FCDECL3_IVV(void, DoDivide, DECIMAL * result, DECIMAL d1, DECIMAL d2);
    static FCDECL3_IVV(void, DoMultiply, DECIMAL * result, DECIMAL d1, DECIMAL d2);
    static FCDECL3_IVI(void, DoRound, DECIMAL * result, DECIMAL d, INT32 decimals);
    static FCDECL2_IV(void, DoToCurrency, CY * result, DECIMAL d);
    static FCDECL2_IV(void, DoTruncate, DECIMAL * result, DECIMAL d);
    static FCDECL2_IV(void, DoFloor, DECIMAL * result, DECIMAL d);

    static FCDECL1(double, ToDouble, DECIMAL d);
    static FCDECL1(float, ToSingle, DECIMAL d);
    static FCDECL1(INT32, ToInt32, DECIMAL d);	
    static FCDECL1(Object*, ToString, DECIMAL d);
    
    static void DecimalToNumber(DECIMAL* value, NUMBER* number);
    static int NumberToDecimal(NUMBER* number, DECIMAL* value);
    

    static FORCEINLINE MethodTable * GetMethodTable()
    {
    	WRAPPER_CONTRACT;
    	if (g_pDecimalMethodTable == NULL)
    		g_pDecimalMethodTable = (&g_Mscorlib)->GetClass(CLASS__DECIMAL);
    	return g_pDecimalMethodTable;
    }

protected:
    static MethodTable *g_pDecimalMethodTable;
   
};

#include <poppack.h>

#endif // _COMDECIMAL_H_
