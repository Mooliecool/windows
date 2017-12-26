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

#include "common.h"

#include <object.h>
#include "ceeload.h"

#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "classnames.h"
#include "comarrayhelpers.h"
#include <memory.h>

INT32 ArrayHelper::IndexOfUINT8( UINT8* array, UINT32 index, UINT32 count, UINT8 value) {
	LEAF_CONTRACT;
	UINT8 * pvalue = (UINT8 *)memchr(array + index, value, count);
	if( NULL == pvalue ) {
		return -1;
       }
	else {
		return pvalue - array;
	}
}
		

// A fast IndexOf method for arrays of primitive types.  Returns TRUE or FALSE
// if it succeeds, and stores result in retVal.
FCIMPL5(FC_BOOL_RET, ArrayHelper::TrySZIndexOf, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal)
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
	
    VALIDATEOBJECTREF(array);
    _ASSERTE(array != NULL);

    if (array->GetRank() != 1 || array->GetLowerBoundsPtr()[0] != 0)
        FC_RETURN_BOOL(FALSE);

    _ASSERTE(retVal != NULL);
	_ASSERTE(index <= array->GetNumComponents());
	_ASSERTE(count <= array->GetNumComponents());
	_ASSERTE(array->GetNumComponents() >= index + count);
    *retVal = 0xdeadbeef;  // Initialize the return value.
    // value can be NULL, but of course, will not be in primitive arrays.
    
    TypeHandle arrayTH = array->GetArrayElementTypeHandle();
    const CorElementType arrayElType = arrayTH.GetVerifierCorElementType();
    if (!CorTypeInfo::IsPrimitiveType(arrayElType))
        FC_RETURN_BOOL(FALSE);
    // Handle special case of looking for a NULL object in a primitive array.
    if (value == NULL) {
        *retVal = -1;
        FC_RETURN_BOOL(TRUE);
    }
    TypeHandle valueTH = value->GetTypeHandle();
    if (arrayTH != valueTH)
        FC_RETURN_BOOL(FALSE);

    
    switch(arrayElType) {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        *retVal = IndexOfUINT8((U1*) array->GetDataPtr(), index, count, *(U1*)value->UnBox());
        break;

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        *retVal = ArrayHelpers<U2>::IndexOf((U2*) array->GetDataPtr(), index, count, *(U2*)value->UnBox());		
        break;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    IN_WIN32(case ELEMENT_TYPE_I:)
    IN_WIN32(case ELEMENT_TYPE_U:)
        *retVal = ArrayHelpers<U4>::IndexOf((U4*) array->GetDataPtr(), index, count, *(U4*)value->UnBox());
        break;

    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
    IN_WIN64(case ELEMENT_TYPE_I:)
    IN_WIN64(case ELEMENT_TYPE_U:)
        *retVal = ArrayHelpers<U8>::IndexOf((U8*) array->GetDataPtr(), index, count, *(U8*)value->UnBox());
        break;

    default:
        _ASSERTE(!"Unrecognized primitive type in ArrayHelper::TrySZIndexOf");
        FC_RETURN_BOOL(FALSE);
    }
    FC_RETURN_BOOL(TRUE);
FCIMPLEND

// A fast LastIndexOf method for arrays of primitive types.  Returns TRUE or FALSE
// if it succeeds, and stores result in retVal.
FCIMPL5(FC_BOOL_RET, ArrayHelper::TrySZLastIndexOf, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;	

    VALIDATEOBJECTREF(array);
    _ASSERTE(array != NULL);

    if (array->GetRank() != 1 || array->GetLowerBoundsPtr()[0] != 0)
        FC_RETURN_BOOL(FALSE);

    _ASSERTE(retVal != NULL);
    *retVal = 0xdeadbeef;  // Initialize the return value.
    // value can be NULL, but of course, will not be in primitive arrays.
    
    TypeHandle arrayTH = array->GetArrayElementTypeHandle();
    const CorElementType arrayElType = arrayTH.GetVerifierCorElementType();
    if (!CorTypeInfo::IsPrimitiveType(arrayElType))
        FC_RETURN_BOOL(FALSE);
    // Handle special case of looking for a NULL object in a primitive array.
    // Also handle case where the array is of 0 length.
    if (value == NULL || array->GetNumComponents() == 0) {
        *retVal = -1;
        FC_RETURN_BOOL(TRUE);
    }

	_ASSERTE(index < array->GetNumComponents());
	_ASSERTE(count <= array->GetNumComponents());
    _ASSERTE(index + 1 >= count);

    TypeHandle valueTH = value->GetTypeHandle();
    if (arrayTH != valueTH)
        FC_RETURN_BOOL(FALSE);

    
    switch(arrayElType) {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        *retVal = ArrayHelpers<U1>::LastIndexOf((U1*) array->GetDataPtr(), index, count, *(U1*)value->UnBox());
        break;

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        *retVal = ArrayHelpers<U2>::LastIndexOf((U2*) array->GetDataPtr(), index, count, *(U2*)value->UnBox());
        break;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    IN_WIN32(case ELEMENT_TYPE_I:)
    IN_WIN32(case ELEMENT_TYPE_U:)
        *retVal = ArrayHelpers<U4>::LastIndexOf((U4*) array->GetDataPtr(), index, count, *(U4*)value->UnBox());
        break;

    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
    IN_WIN64(case ELEMENT_TYPE_I:)
    IN_WIN64(case ELEMENT_TYPE_U:)
        *retVal = ArrayHelpers<U8>::LastIndexOf((U8*) array->GetDataPtr(), index, count, *(U8*)value->UnBox());
        break;

    default:
        _ASSERTE(!"Unrecognized primitive type in ArrayHelper::TrySZLastIndexOf");
        FC_RETURN_BOOL(FALSE);
    }
    FC_RETURN_BOOL(TRUE);
}
FCIMPLEND


FCIMPL5(FC_BOOL_RET, ArrayHelper::TrySZBinarySearch, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal)
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    VALIDATEOBJECTREF(array);
    _ASSERTE(array != NULL);

    if (array->GetRank() != 1 || array->GetLowerBoundsPtr()[0] != 0)
        FC_RETURN_BOOL(FALSE);

    _ASSERTE(retVal != NULL);
	_ASSERTE(index <= array->GetNumComponents());
	_ASSERTE(count <= array->GetNumComponents());
	_ASSERTE(array->GetNumComponents() >= index + count);
    *retVal = 0xdeadbeef;  // Initialize the return value.
    // value can be NULL, but of course, will not be in primitive arrays.
    TypeHandle arrayTH = array->GetArrayElementTypeHandle();
    const CorElementType arrayElType = arrayTH.GetVerifierCorElementType();
    if (!CorTypeInfo::IsPrimitiveType(arrayElType))
        FC_RETURN_BOOL(FALSE);
    // Handle special case of looking for a NULL object in a primitive array.
    if (value == NULL) {
        *retVal = -1;
        FC_RETURN_BOOL(TRUE);
    }

    TypeHandle valueTH = value->GetTypeHandle();
    if (arrayTH != valueTH)
        FC_RETURN_BOOL(FALSE);

    switch(arrayElType) {
    case ELEMENT_TYPE_I1:
		*retVal = ArrayHelpers<I1>::BinarySearchBitwiseEquals((I1*) array->GetDataPtr(), index, count, *(I1*)value->UnBox());
		break;

    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        *retVal = ArrayHelpers<U1>::BinarySearchBitwiseEquals((U1*) array->GetDataPtr(), index, count, *(U1*)value->UnBox());
        break;

    case ELEMENT_TYPE_I2:
        *retVal = ArrayHelpers<I2>::BinarySearchBitwiseEquals((I2*) array->GetDataPtr(), index, count, *(I2*)value->UnBox());
        break;

    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        *retVal = ArrayHelpers<U2>::BinarySearchBitwiseEquals((U2*) array->GetDataPtr(), index, count, *(U2*)value->UnBox());
        break;

    case ELEMENT_TYPE_I4:
        *retVal = ArrayHelpers<I4>::BinarySearchBitwiseEquals((I4*) array->GetDataPtr(), index, count, *(I4*)value->UnBox());
        break;

    case ELEMENT_TYPE_U4:
        *retVal = ArrayHelpers<U4>::BinarySearchBitwiseEquals((U4*) array->GetDataPtr(), index, count, *(U4*)value->UnBox());
        break;

    case ELEMENT_TYPE_R4:
        *retVal = ArrayHelpers<R4>::BinarySearchBitwiseEquals((R4*) array->GetDataPtr(), index, count, *(R4*)value->UnBox());
        break;

    case ELEMENT_TYPE_I8:
        *retVal = ArrayHelpers<I8>::BinarySearchBitwiseEquals((I8*) array->GetDataPtr(), index, count, *(I8*)value->UnBox());
        break;

    case ELEMENT_TYPE_U8:
        *retVal = ArrayHelpers<U8>::BinarySearchBitwiseEquals((U8*) array->GetDataPtr(), index, count, *(U8*)value->UnBox());
        break;

	case ELEMENT_TYPE_R8:
        *retVal = ArrayHelpers<R8>::BinarySearchBitwiseEquals((R8*) array->GetDataPtr(), index, count, *(R8*)value->UnBox());
        break;

    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
        // In V1.0, IntPtr & UIntPtr are not fully supported types.  They do 
        // not implement IComparable, so searching & sorting for them should
        // fail.  In V1.1 or V2.0, this should change.  --                                   
        FC_RETURN_BOOL(FALSE);

    default:
        _ASSERTE(!"Unrecognized primitive type in ArrayHelper::TrySZBinarySearch");
        FC_RETURN_BOOL(FALSE);
    }
    FC_RETURN_BOOL(TRUE);
FCIMPLEND


FCIMPL4(FC_BOOL_RET, ArrayHelper::TrySZSort, ArrayBase * keys, ArrayBase * items, UINT32 left, UINT32 right)
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    VALIDATEOBJECTREF(keys);
	VALIDATEOBJECTREF(items);
    _ASSERTE(keys != NULL);

    if (keys->GetRank() != 1 || keys->GetLowerBoundsPtr()[0] != 0)
        FC_RETURN_BOOL(FALSE);

	_ASSERTE(left <= right);
	_ASSERTE(right < keys->GetNumComponents() || keys->GetNumComponents() == 0);

    TypeHandle keysTH = keys->GetArrayElementTypeHandle();
    const CorElementType keysElType = keysTH.GetVerifierCorElementType();
    if (!CorTypeInfo::IsPrimitiveType(keysElType))
        FC_RETURN_BOOL(FALSE);
	if (items != NULL) {
		TypeHandle itemsTH = items->GetArrayElementTypeHandle();
		if (keysTH != itemsTH)
			FC_RETURN_BOOL(FALSE);   // Can't currently handle sorting different types of arrays.
	}

	if (left == right || right == 0xffffffff)
		FC_RETURN_BOOL(TRUE);

    switch(keysElType) {
    case ELEMENT_TYPE_I1:
		ArrayHelpers<I1>::QuickSort((I1*) keys->GetDataPtr(), (I1*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
		break;

    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        ArrayHelpers<U1>::QuickSort((U1*) keys->GetDataPtr(), (U1*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_I2:
        ArrayHelpers<I2>::QuickSort((I2*) keys->GetDataPtr(), (I2*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
		ArrayHelpers<U2>::QuickSort((U2*) keys->GetDataPtr(), (U2*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_I4:
		ArrayHelpers<I4>::QuickSort((I4*) keys->GetDataPtr(), (I4*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_U4:
        ArrayHelpers<U4>::QuickSort((U4*) keys->GetDataPtr(), (U4*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_R4:
        ArrayHelpers<R4>::QuickSort((R4*) keys->GetDataPtr(), (R4*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_I8:
        ArrayHelpers<I8>::QuickSort((I8*) keys->GetDataPtr(), (I8*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_U8:
        ArrayHelpers<U8>::QuickSort((U8*) keys->GetDataPtr(), (U8*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

	case ELEMENT_TYPE_R8:
        ArrayHelpers<R8>::QuickSort((R8*) keys->GetDataPtr(), (R8*) (items == NULL ? NULL : items->GetDataPtr()), left, right);
        break;

    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
        // In V1.0, IntPtr & UIntPtr are not fully supported types.  They do 
        // not implement IComparable, so searching & sorting for them should
        // fail.  In V1.1 or V2.0, this should change.                                   
        FC_RETURN_BOOL(FALSE);

    default:
        _ASSERTE(!"Unrecognized primitive type in ArrayHelper::TrySZSort");
        FC_RETURN_BOOL(FALSE);
    }
    FC_RETURN_BOOL(TRUE);
FCIMPLEND

FCIMPL3(FC_BOOL_RET, ArrayHelper::TrySZReverse, ArrayBase * array, UINT32 index, UINT32 count)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    VALIDATEOBJECTREF(array);
    _ASSERTE(array != NULL);

    if (array->GetRank() != 1 || array->GetLowerBoundsPtr()[0] != 0)
        FC_RETURN_BOOL(FALSE);

	_ASSERTE(index <= array->GetNumComponents());
	_ASSERTE(count <= array->GetNumComponents());
	_ASSERTE(array->GetNumComponents() >= index + count);

    TypeHandle arrayTH = array->GetArrayElementTypeHandle();
    const CorElementType arrayElType = arrayTH.GetVerifierCorElementType();
    if (!CorTypeInfo::IsPrimitiveType(arrayElType))
        FC_RETURN_BOOL(FALSE);

    switch(arrayElType) {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        ArrayHelpers<U1>::Reverse((U1*) array->GetDataPtr(), index, count);
        break;

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        ArrayHelpers<U2>::Reverse((U2*) array->GetDataPtr(), index, count);
        break;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    IN_WIN32(case ELEMENT_TYPE_I:)
    IN_WIN32(case ELEMENT_TYPE_U:)
        ArrayHelpers<U4>::Reverse((U4*) array->GetDataPtr(), index, count);
        break;

    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
    IN_WIN64(case ELEMENT_TYPE_I:)
    IN_WIN64(case ELEMENT_TYPE_U:)
        ArrayHelpers<U8>::Reverse((U8*) array->GetDataPtr(), index, count);
        break;

    default:
        _ASSERTE(!"Unrecognized primitive type in ArrayHelper::TrySZReverse");
        FC_RETURN_BOOL(FALSE);
    }
    FC_RETURN_BOOL(TRUE);
}
FCIMPLEND
