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
// COMArrayInfo
//	This file defines the native methods for the ArrayInfo class
//	found in reflection.  ArrayInfo allows for late bound access
//	to COM+ Arrays.
////////////////////////////////////////////////////////////////////////////////

#ifndef __COMARRAYINFO_H__
#define __COMARRAYINFO_H__

#include "fcall.h"
#include "objectclone.h"

class COMArrayInfo
{
public:
	// This method will create a new array of type type, with zero lower
	//	bounds and rank.
    static FCDECL4(Object*, CreateInstance,   void* elementTypeHandle, INT32 rank, INT32* pLengths, INT32* pBounds);

	// This method will return a TypedReference to the array element
    static FCDECL4(void, GetReference, ArrayBase* refThisUNSAFE, TypedByRef* elemRef, INT32 rank, INT32* pIndices);

	// This set of methods will set a value in an array
	static FCDECL2(void, SetValue, TypedByRef* target, Object* objUNSAFE);

	// This method will initialize an array from a TypeHandle
	//	to a field.
	static FCDECL2(void, InitializeArray, ArrayBase* vArrayRef, HANDLE handle);

};


#endif
