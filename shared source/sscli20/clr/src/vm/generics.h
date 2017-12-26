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

#ifndef _GENERICS_H
#define _GENERICS_H

class CrawlFrame;

// Generics helper functions
class Generics
{
public:
  static TypeHandle GetMethodDeclaringType(TypeHandle owner, MethodDesc *pMD, BOOL allowCreate = TRUE);
  static TypeHandle GetFieldDeclaringType(TypeHandle owner, FieldDesc *pFD);
  static WORD FindClassDictionaryToken(EEClass *pClass, DictionaryEntryLayout *pEntryLayout, WORD *offsets);

  // Check for legal instantiations.  Returns true if the instantiation is legal.
  static BOOL CheckInstantiation(DWORD nGenericArgs, TypeHandle *pGenericArgs);
  
  // Check for overly deep generic instantiations, attempting to catch
  // recursive generic type definitions.  Returns true if the instantiation 
  // is not recursive.
  static BOOL CheckInstantiationForRecursion(const unsigned int nGenericArgs, const TypeHandle pGenericArgs[]);

  static BOOL GetExactInstantiationsFromCallInformation(/* in */  MethodDesc *pRepMethod,
                                                    /* in */  OBJECTREF pThis,
                                                    /* in */  void *paramTypeArg,
                                                    /* out*/  TypeHandle *pSpecificClass,
                                                    /* out*/  MethodDesc** pSpecificMethod);

  static BOOL GetExactInstantiationsFromCallInformation(/* in */  MethodDesc *pRepMethod,
                                                    /* in */  void *pExactGenericArgsToken,
                                                    /* out*/  TypeHandle *pSpecificClass,
                                                    /* out*/  MethodDesc** pSpecificMethod);
};

#endif
