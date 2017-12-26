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
// Helper methods for the Array class
// Specifically, this contains indexing, sorting & searching templates.

#ifndef _COMARRAYHELPERS_H_
#define _COMARRAYHELPERS_H_

#include "fcall.h"


template <class KIND>
class ArrayHelpers
{
public:
    static int IndexOf(KIND array[], UINT32 index, UINT32 count, KIND value) {
        LEAF_CONTRACT;

        _ASSERTE(array != NULL && index >= 0 && count >= 0);
        for(UINT32 i=index; i<index+count; i++)
            if (array[i] == value)
                return i;
        return -1;
    }

    static int LastIndexOf(KIND array[], UINT32 index, UINT32 count, KIND value) {
        LEAF_CONTRACT;

        INT32 startIndex = (INT32)index;
        INT32 n = (INT32)count;
        _ASSERTE(array != NULL && startIndex >= 0 && n>= 0);        
        // Note (startIndex- n) may be -1 when startIndex is 0 and n is 1.        
        _ASSERTE(startIndex - n >= -1);        
        INT32 endIndex = max(startIndex - n, -1);
        
        for(INT32 i=startIndex; i> endIndex; i--)
            if (array[i] == value)
                return i;
        return -1;
    }
    
    static int BinarySearchBitwiseEquals(KIND array[], int index, int length, KIND value) {
        WRAPPER_CONTRACT;

        _ASSERTE(array != NULL && length >= 0 && index >= 0);
        int lo = index;
        int hi = index + length - 1;
        // Note: if length == 0, hi will be Int32.MinValue, and our comparison
        // here between 0 & -1 will prevent us from breaking anything.
        while (lo <= hi) {
            int i = lo + ((hi - lo) >> 1);
            if (array[i] < value) {
                lo = i + 1;
            } 
            else if (array[i] > value){
                hi = i - 1;
            }
            else {
                return i;
            }
        }
        return ~lo;
    }

    static void QuickSort(KIND keys[], KIND items[], int left, int right) {
        WRAPPER_CONTRACT;

        // Make sure left != right in your own code.
        _ASSERTE(keys != NULL && left < right);
        do {
            int i = left;
            int j = right;
            KIND x = keys[i + ((j - i) >> 1)];
            do {
                while (keys[i] < x) i++;
                while (x < keys[j]) j--;
                _ASSERTE(i>=left && j<=right);
                if (i > j) break;
                if (i < j) {
                    KIND key = keys[i];
                    keys[i] = keys[j];
                    keys[j] = key;
                    if (items != NULL) {
                        KIND item = items[i];
                        items[i] = items[j];
                        items[j] = item;
                    }
                }
                i++;
                j--;
            } while (i <= j);
            if (j - left <= right - i) {
                if (left < j) QuickSort(keys, items, left, j);
                left = i;
            }
            else {
                if (i < right) QuickSort(keys, items, i, right);
                right = j;
            }
        } while (left < right);
    }

    static void Reverse(KIND array[], UINT32 index, UINT32 count) {
        LEAF_CONTRACT;

        _ASSERTE(array != NULL);
        if (count == 0) {
            return;
        }
        UINT32 i = index;
        UINT32 j = index + count - 1;
        while(i < j) {
            KIND temp = array[i];
            array[i] = array[j];
            array[j] = temp;
            i++;
            j--;
        }
    }
};


class ArrayHelper
{
    public:
    // These methods return TRUE or FALSE for success or failure, and the real
    // result is an out param.  They're helpers to make operations on SZ arrays of 
    // primitives significantly faster.
    static FCDECL5(FC_BOOL_RET, TrySZIndexOf, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal);
    static FCDECL5(FC_BOOL_RET, TrySZLastIndexOf, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal);
    static FCDECL5(FC_BOOL_RET, TrySZBinarySearch, ArrayBase * array, UINT32 index, UINT32 count, Object * value, INT32 * retVal);

    static FCDECL4(FC_BOOL_RET, TrySZSort, ArrayBase * keys, ArrayBase * items, UINT32 left, UINT32 right);
    static FCDECL3(FC_BOOL_RET, TrySZReverse, ArrayBase * array, UINT32 index, UINT32 count);

    // Helper methods	
    static INT32 IndexOfUINT8( UINT8* array, UINT32 index, UINT32 count, UINT8 value);
};

#endif // _COMARRAYHELPERS_H_
