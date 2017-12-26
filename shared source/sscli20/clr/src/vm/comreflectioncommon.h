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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _COMREFLECTIONCOMMON_H_
#define _COMREFLECTIONCOMMON_H_

// VM and other stuff
#include "comutilnative.h"

class ReflectCtors;
class ReflectMethods;
class ReflectOtherMethods;
class ReflectFields;

// GetClassStringVars
// This method will extract the string from a STRINGREF and convert it to a UTF8 string
//  A QuickBytes buffer must be provided by the caller to hold the result.
LPUTF8 GetClassStringVars(STRINGREF stringRef, CQuickBytes *pBytes, DWORD* pCnt, bool bEatWhitespace = false);

// NormalizeArrayTypeName
//  Parse and normalize an array type name removing *, return the same string passed in.
//  The string can be shrinked and it is never reallocated.
//  For single dimensional array T[] != T[*], but for multi dimensional array
//  T[,] == T[*,*]
//  T[?] is not valid any more
LPUTF8 NormalizeArrayTypeName(LPUTF8 strArrayTypeName, DWORD dwLength);

#endif // _COMREFLECTIONCOMMON_H_
