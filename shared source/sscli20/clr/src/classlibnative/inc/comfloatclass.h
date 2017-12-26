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
#ifndef _COMFLOATCLASS_H
#define _COMFLOATCLASS_H

#include <object.h>
#include <fcall.h>

class COMDouble {
public:
    FCDECL1_V(static double, Floor, double d);
    FCDECL1_V(static double, Sqrt, double d);
    FCDECL1_V(static double, Log, double d);
    FCDECL1_V(static double, Log10, double d);
    FCDECL1_V(static double, Exp, double d);
    FCDECL2_VV(static double, Pow, double x, double y);
    FCDECL1_V(static double, Acos, double d);
    FCDECL1_V(static double, Asin, double d);
    FCDECL1_V(static double, Atan, double d);
    FCDECL2_VV(static double, Atan2, double x, double y);
    FCDECL1_V(static double, Cos, double d);
    FCDECL1_V(static double, Sin, double d);
    FCDECL1_V(static double, Tan, double d);
    FCDECL1_V(static double, Cosh, double d);
    FCDECL1_V(static double, Sinh, double d);
    FCDECL1_V(static double, Tanh, double d);
    FCDECL1_V(static double, Round, double d);
    FCDECL1_V(static double, Ceil, double d);
    FCDECL1_V(static float, AbsFlt, float f);
    FCDECL1_V(static double, AbsDbl, double d);
    FCDECL1(static double, ModFDouble, double* d);


};
    

#endif // _COMFLOATCLASS_H
