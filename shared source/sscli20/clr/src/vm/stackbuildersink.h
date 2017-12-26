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
/*============================================================
**
** File:    StackBuilderSink.h
**       
**
** Purpose: Native implementaion for Microsoft.Runtime.StackBuilderSink
**
** Date:    Mar 24, 1999
**
===========================================================*/
#ifndef __STACKBUILDERSINK_H__
#define __STACKBUILDERSINK_H__


void CallDescrWithObjectArray(OBJECTREF& pServer, MethodDesc *pMD, //ReflectMethod *pMD,
                  const BYTE *pTarget, MetaSig* sig, VASigCookie *pCookie,
                  BOOL fIsStatic, PTRARRAYREF& pArguments,
                  OBJECTREF* pVarRet, PTRARRAYREF* ppVarOutParams);

//+----------------------------------------------------------
//
//  Class:      CStackBuilderSink
// 
//  Synopsis:   EE counterpart to 
//              Microsoft.Runtime.StackBuilderSink
//              Code helper to build a stack of parameter 
//              arguments and make a function call on an 
//              object.
//
//------------------------------------------------------------
class CStackBuilderSink
{
public:    
   
    static FCDECL7(Object*, PrivateProcessMessage, 
                                Object* pSBSinkUNSAFE, 
                                MethodDesc* pMD, 
                                PTRArray* pArgsUNSAFE, 
                                Object* pServerUNSAFE, 
                                CLR_I4 __unused__iMethodPtr, 
                                CLR_BOOL fContext, 
                                PTRARRAYREF* ppVarOutParams);
};

#endif  // __STACKBUILDERSINK_H__
