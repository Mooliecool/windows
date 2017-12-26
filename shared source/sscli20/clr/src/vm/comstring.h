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
** Header:  COMString.h
**       
**
** Purpose: Contains types and method signatures for the String class
**
** Date:  March 12, 1998
**
===========================================================*/
#include "comstringcommon.h"
#include "fcall.h"
#include "excep.h"
#include "comvarargs.h"

#ifndef _COMSTRING_H
#define _COMSTRING_H
//
// Each function that we call through native only gets one argument,
// which is actually a pointer to it's stack of arguments.  Our structs
// for accessing these are defined below.
//

//
//These are the type signatures for String
//
//
// The method signatures for each of the methods we define.
// N.B.: There's a one-to-one mapping between the method signatures and the
// type definitions given above.
//



/*=================RefInterpretGetStringValuesDangerousForGC======================
**N.B.: This perfoms no range checking and relies on the caller to have done this.
**Args: (IN)ref -- the String to be interpretted.
**      (OUT)chars -- a pointer to the characters in the buffer.
**      (OUT)length -- a pointer to the length of the buffer.
**Returns: void.
**Exceptions: None.
==============================================================================*/
// !!!! If you use this function, you have to be careful because chars is a pointer
// !!!! to the data buffer of ref.  If GC happens after this call, you need to make
// !!!! sure that you have a pin handle on ref, or use GCPROTECT_BEGINPINNING on ref.
#ifdef USE_CHECKED_OBJECTREFS
inline void RefInterpretGetStringValuesDangerousForGC(STRINGREF ref, __deref_out_ecount(*length + 1) WCHAR **chars, int *length) {
    WRAPPER_CONTRACT;

    _ASSERTE(ref != NULL);
    *length = (ref)->GetStringLength();
    *chars  = (ref)->GetBuffer();
    ENABLESTRESSHEAP();
}
#endif

inline void RefInterpretGetStringValuesDangerousForGC(StringObject* ref, __deref_out_ecount(*length + 1) WCHAR **chars, int *length) {
    WRAPPER_CONTRACT;

    _ASSERTE(ref && ref->GetGCSafeMethodTable() == g_pStringClass);
    *length = (ref)->GetStringLength();
    *chars  = (ref)->GetBuffer();
#ifdef _DEBUG
    ENABLESTRESSHEAP();
#endif
}

//The first two macros are essentially the same.  I just define both because
//having both can make the code more readable.
#define IS_FAST_SORT(state) (((state) == STRING_STATE_FAST_OPS))
#define IS_SLOW_SORT(state) (((state) != STRING_STATE_FAST_OPS))

//This macro should be used to determine things like indexing, casing, and encoding.
#define IS_FAST_OPS_EXCEPT_SORT(state) (((state)==STRING_STATE_SPECIAL_SORT) || ((state)==STRING_STATE_FAST_OPS))
#define IS_ASCII(state) (((state)==STRING_STATE_SPECIAL_SORT) || ((state)==STRING_STATE_FAST_OPS))
#define IS_FAST_CASING(state) IS_ASCII(state)
#define IS_FAST_INDEX(state)  IS_ASCII(state)
#define IS_STRING_STATE_UNDETERMINED(state) ((state)==STRING_STATE_UNDETERMINED)
#define HAS_HIGH_CHARS(state) ((state)==STRING_STATE_HIGH_CHARS)


class COMString {
//
// These are the method signatures for String
//
    static STRINGREF* EmptyStringRefPtr;

private:
    static void* memcpyNoGCRefsPtrAligned(void * dest, const void * src, size_t len);


public:

    static STRINGREF GetEmptyString();
    static STRINGREF* GetEmptyStringRefPtr();

    //
    // Constructors
    //
    static FCDECL5(Object *, StringInitSBytPtrPartialEx, StringObject *thisString,
                   I1 *ptr, INT32 startIndex, INT32 length, Object* encoding);
    static FCDECL2(Object *, StringInitCharPtr, StringObject *stringThis, INT8 *ptr);
    static FCDECL4(Object *, StringInitCharPtrPartial, StringObject *stringThis, INT8 *value,
                   INT32 startIndex, INT32 length);

    //
    // Search/Query Methods
    //
    static FCDECL3(INT32, FCCompareOrdinal, StringObject* strA, StringObject* strB, CLR_BOOL bIgnoreCase);
    static FCDECL1(FC_BOOL_RET, IsFastSort, StringObject* pThisRef);
    static FCDECL1(FC_BOOL_RET, IsAscii, StringObject* pThisRef);
    static FCDECL1(FC_BOOL_RET, ValidModifiableString, StringObject* pThisRef);


    static FCDECL4(INT32, FCCompareOrdinalWC, StringObject* strA, __in WCHAR *strB, CLR_BOOL bIgnoreCase, CLR_BOOL *bSuccess);

    static FCDECL5(INT32, CompareOrdinalEx, StringObject* strA, INT32 indexA, StringObject* strB, INT32 indexB, INT32 count);

    static FCDECL4(INT32, IndexOfChar, StringObject* vThisRef, CLR_CHAR value, INT32 startIndex, INT32 count );

    static FCDECL4(INT32, LastIndexOfChar, StringObject* thisRef, CLR_CHAR value, INT32 startIndex, INT32 count );

    static FCDECL4(INT32, LastIndexOfCharArray, StringObject* thisRef, CHARArray* valueRef, INT32 startIndex, INT32 count );

    static FCDECL4(INT32, IndexOfCharArray, StringObject* vThisRef, CHARArray* value, INT32 startIndex, INT32 count );

    static FCDECL1(INT32, GetHashCode, StringObject* pThisRef);
    static FCDECL2(FC_CHAR_RET, GetCharAt, StringObject* pThisRef, INT32 index);
    static FCDECL1(INT32, Length, StringObject* pThisRef);

    //
    // Modifiers
    //


    static FCDECL4(Object*, PadHelper, StringObject* thisRefUNSAFE, INT32 totalWidth, CLR_CHAR paddingChar, CLR_BOOL isRightPadded);

    static FCDECL3(LPVOID, Replace, StringObject* thisRef, CLR_CHAR oldChar, CLR_CHAR newChar);
    static FCDECL3(Object*, ReplaceString, StringObject* thisRef, StringObject* oldValue, StringObject* newValue);

    static FCDECL3(Object*, Insert, StringObject* thisRefUNSAFE, INT32 startIndex, StringObject* valueUNSAFE);

    static FCDECL3(Object*, Remove, StringObject* thisRefUNSAFE, INT32 startIndex, INT32 count);


    //========================================================================
    // Creates a System.String object. All the functions that take a length
    // or a count of bytes will add the null terminator after length
    // characters. So this means that if you have a string that has 5
    // characters and the null terminator you should pass in 5 and NOT 6.
    //========================================================================
    static STRINGREF NewString(int length);
    static STRINGREF NewString(int length, BOOL bHasTrailByte);
    static STRINGREF NewString(const WCHAR *pwsz);
    static STRINGREF NewString(const WCHAR *pwsz, int length);
    static STRINGREF NewString(LPCUTF8 psz);
    static STRINGREF NewString(LPCUTF8 psz, int cBytes);
    static STRINGREF NewString(STRINGREF *srChars, int start, int length);
    static STRINGREF NewString(STRINGREF *srChars, int start, int length, int capacity);
    static STRINGREF NewString(I2ARRAYREF *srChars, int start, int length);
    static STRINGREF NewString(I2ARRAYREF *srChars, int start, int length, int capacity);
    static STRINGREF __stdcall StringInitCharHelper(LPCSTR pszSource, int length);
    static DWORD InternalCheckHighChars(STRINGREF inString);
    static bool TryConvertStringDataToUTF8(STRINGREF inString, LPUTF8 outString, DWORD outStrLen);

    static BOOL HasTrailByte(STRINGREF str);
    static BOOL GetTrailByte(STRINGREF str, BYTE *bTrailByte);
    static BOOL SetTrailByte(STRINGREF str, BYTE bTrailByte);
    static BOOL CaseInsensitiveCompHelper(__in_ecount(aLength) WCHAR * strA, __in WCHAR * strB, int aLength, int bLength, int *result);
    
#ifdef VERIFY_HEAP
    //has to use raw object to avoid recursive validation
    static BOOL ValidateHighChars (StringObject * inString);
#endif //VERIFY_HEAP
};

/*================================GetEmptyString================================
**Get a reference to the empty string.  If we haven't already gotten one, we
**query the String class for a pointer to the empty string that we know was
**created at startup.
**
**Args: None
**Returns: A STRINGREF to the EmptyString
**Exceptions: None
==============================================================================*/
inline STRINGREF COMString::GetEmptyString() {

    CONTRACTL {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
    } CONTRACTL_END;
    STRINGREF* refptr = EmptyStringRefPtr;

    //If we've never gotten a reference to the EmptyString, we need to go get one.
    if (refptr==NULL) {
        refptr = GetEmptyStringRefPtr();
    }
    //We've already have a reference to the EmptyString, so we can just return it.
    return *refptr;
}

#endif // _COMSTRING_H






