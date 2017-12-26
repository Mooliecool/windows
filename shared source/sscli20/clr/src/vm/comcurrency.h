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
#ifndef _COMCURRENCY_H_
#define _COMCURRENCY_H_

#include <oleauto.h>
#include <pshpack1.h>

class COMCurrency 
{
public:
    static FCDECL2   (void, DoToDecimal,  DECIMAL * result, CY c);
};

#include <poppack.h>

#endif // _COMCURRENCY_H_
