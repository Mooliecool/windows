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
#ifndef __NATIVE_TEXTINFO_H
#define __NATIVE_TEXTINFO_H

typedef  P844_TABLE    PCASE;        // ptr to Lower or Upper Case table
//
// Struct for offsets of the casing tables (uppercasing & lowercasing)
// of the suplementary characters.  
//
typedef struct 
{
    DWORD OffsetToUppercasing;
    DWORD OffsetToLowercasing;
} PlaneOffset;

class NativeTextInfo {
    public:
        NativeTextInfo(LPBYTE pTextInfoTable, LPBYTE pUpperCaseTable, LPBYTE pLowerCaseTable, LPBYTE pTitleCasingTable, PlaneOffset* pPlaneOffset);

        int  ChangeCaseChar(BOOL bIsToUpper, int wch);
        int ChangeCaseSurrogate(BOOL bIsToUpper, WCHAR highSurrogate, WCHAR lowSurrogate, __out_ecount(1) WCHAR* resultHighSurrogate, __out_ecount(1) WCHAR* resultLowSurrogate);

        LPWSTR ChangeCaseString(BOOL bIsToUpper, int nStrLen, __in_ecount(nStrLen) LPWSTR source, __out_ecount(nStrLen) LPWSTR target);
        WCHAR GetTitleCaseChar(WCHAR wch);
        INT32 CompareOrdinalIgnoreCase(__in_ecount(Length1) WCHAR* string1, int Length1, __in_ecount(Length2) WCHAR* string2, int Length2);
        INT32 CompareOrdinalIgnoreCaseEx(__in_ecount(Length1) WCHAR* string1, int Length1, __in_ecount(Length2) WCHAR* string2, int Length2, int count);
        INT32 GetHashCodeOrdinalIgnoreCase(__in WCHAR *str);
        INT32 IndexOfStringOrdinalIgnoreCase(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);
        INT32 LastIndexOfStringOrdinalIgnoreCase(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);
        void GetPointers(LPBYTE *ppCaseTable, LPBYTE *ppTitlecaseIndexTable);
        
    private:        
        __forceinline WCHAR InternalChangeCaseChar(LPWORD pIndexTable, WCHAR wch);
        __forceinline BOOL IsSurrogateChar(WCHAR ch);
        __forceinline BOOL IsHighSurrogateChar(WCHAR ch);
        __forceinline BOOL IsLowSurrogateChar(WCHAR ch);
        INT32 CompareOrdinalIgnoreCaseHelper(DWORD* stringAChars, DWORD* stringBChars, int count);
#if defined(ALIGN_ACCESS)
        INT32 CompareOrdinalIgnoreCaseHelperUnAligned(__in_ecount(count) WCHAR* stringAChars, __in_ecount(count) WCHAR* stringBChars, int count);
#endif // ALIGN_ACCESS
        
    private:
        LPBYTE m_pCaseTable;
        LPWORD m_pUppercaseIndexTable;
        LPWORD m_pLowercaseIndexTable;
        LPWORD m_pTitlecaseIndexTable;
        PlaneOffset* m_pPlaneOffset;
};
#endif
