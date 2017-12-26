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
#ifndef _H_INTEROPCONVERTER_
#define _H_INTEROPCONVERTER_

#include "debugmacros.h"

//
// THE FOLLOWING ARE THE MAIN APIS USED BY EVERYONE TO CONVERT BETWEEN
// OBJECTREFs AND COM IPs


//--------------------------------------------------------
// Only IUnknown* is supported without FEATURE_COMINTEROP
//--------------------------------------------------------
IUnknown* __stdcall GetComIPFromObjectRef(OBJECTREF* poref);
OBJECTREF __stdcall GetObjectRefFromComIP(IUnknown* pUnk, MethodTable* pMTClass = NULL);
void UnMarshalObjectForCurrentDomain(AppDomain* pObjDomain, ComCallWrapper* pWrap, OBJECTREF* pResult);


#endif // #ifndef _H_INTEROPCONVERTER_

