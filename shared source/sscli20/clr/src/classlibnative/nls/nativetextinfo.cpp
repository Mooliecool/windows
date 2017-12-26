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
#include "comnls.h"
#include "nlstable.h"   // class NLSTable
#include "nativetextinfo.h" // class NativeTextInfo

NativeTextInfo::NativeTextInfo(
    LPBYTE pTextInfoTable, 
    LPBYTE pUpperCaseTable, 
    LPBYTE pLowerCaseTable, 
    LPBYTE pTitleCasingTable,
    PlaneOffset* pPlaneOffset) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTextInfoTable));
        PRECONDITION(CheckPointer(pUpperCaseTable));
        PRECONDITION(CheckPointer(pLowerCaseTable));
        PRECONDITION(CheckPointer(pTitleCasingTable));
        PRECONDITION(CheckPointer(pPlaneOffset));
    } CONTRACTL_END;

    m_pCaseTable = pTextInfoTable;
    m_pUppercaseIndexTable = (LPWORD)pUpperCaseTable;
    m_pLowercaseIndexTable = (LPWORD)pLowerCaseTable;
    m_pTitlecaseIndexTable = (LPWORD)pTitleCasingTable;

    m_pPlaneOffset = pPlaneOffset;
}


void NativeTextInfo::GetPointers(LPBYTE *ppCaseTable, LPBYTE *ppTitlecaseIndexTable)
{
    LEAF_CONTRACT;
    *ppCaseTable            = m_pCaseTable;
    *ppTitlecaseIndexTable  = (LPBYTE) m_pTitlecaseIndexTable;
}

int NativeTextInfo::ChangeCaseChar(BOOL bIsToUpper, int wch) {
    LEAF_CONTRACT;

    LPWORD pIndexTable = (bIsToUpper ? m_pUppercaseIndexTable : m_pLowercaseIndexTable);

    WORD value = pIndexTable[wch >> 8];
    value = pIndexTable[value + ((wch >> 4) & 0xf)];
    value = pIndexTable[value + (wch & 0xf)];

    if (value == HIGH_SURROGATE_START) 
    {
        // The high surrogate characters are marked as D800 (HIGH_SURROGATE_START).  In this case,
        // we should just return the high surrogate character.
        return (wch);
    }
    return (wch + (int)((short)value));
}

int NativeTextInfo::ChangeCaseSurrogate(BOOL bIsToUpper, 
    WCHAR highSurrogate, WCHAR lowSurrogate, __out_ecount(1) WCHAR* resultHighSurrogate, __out_ecount(1)  WCHAR* resultLowSurrogate) {
    LEAF_CONTRACT;
    
    STATIC_CONTRACT_SO_TOLERANT;
    
    _ASSERTE(IsHighSurrogateChar(highSurrogate));
    _ASSERTE(IsLowSurrogateChar(lowSurrogate));

    int plane = ((highSurrogate - HIGH_SURROGATE_START) >> 6);
    DWORD offset = (bIsToUpper ? m_pPlaneOffset[plane].OffsetToUppercasing: m_pPlaneOffset[plane].OffsetToLowercasing);
    if (offset != 0) {
        LPWORD pSurrogateIndexTable = (LPWORD)(m_pCaseTable + offset );
        WCHAR wch2 = lowSurrogate - LOW_SURROGATE_START;
        WCHAR result = InternalChangeCaseChar(pSurrogateIndexTable, (highSurrogate << 10) + wch2);                
        // 0xfc00 = 0x1111 1111 1100 0000
        // We masked off the lowest 6 bits so that the plane of the wch1 is preserved.
        // Here 
        // Then we add the highest 6 bits of the result into the high surrogate.
        // 
        *resultHighSurrogate = HIGH_SURROGATE_START  + (result >> 10); 
        // 0x03ff = 0x0000 0011 1111 1111
        // We get the lowest 10 bits of the result and make it a low surrogate.
        *resultLowSurrogate = LOW_SURROGATE_START + (result & 0x3ff);
    } else {
        *resultHighSurrogate = highSurrogate;
        *resultLowSurrogate = lowSurrogate;
    }
    return (TRUE);                    
}

WCHAR NativeTextInfo::InternalChangeCaseChar(LPWORD pIndexTable, WCHAR wch) {
    LEAF_CONTRACT;

    WORD value = pIndexTable[wch >> 8];
    value = pIndexTable[value + ((wch >> 4) & 0xf)];
    value = pIndexTable[value + (wch & 0xf)];
    return (wch + (int)((short)value));
}

LPWSTR NativeTextInfo::ChangeCaseString
    (BOOL bIsToUpper, int nStrLen, __in_ecount(nStrLen) LPWSTR source, __out_ecount(nStrLen) LPWSTR target) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(target));
    } CONTRACTL_END;
    //_ASSERTE(!source && !target);

    LPWORD pIndexTable = (bIsToUpper ? m_pUppercaseIndexTable : m_pLowercaseIndexTable);
    
    for (int i = 0; i < nStrLen; i++) {
        WCHAR wch = source[i];
        WORD value = pIndexTable[wch >> 8];
        value = pIndexTable[value + ((wch >> 4) & 0xf)];
        value = pIndexTable[value + (wch & 0xf)];
        if (value != HIGH_SURROGATE_START) {
            target[i] = wch + value;
        } else {
            // This is a high surrogate
            if (i+1 < nStrLen) {
                WCHAR wch2 = source[i+1] - LOW_SURROGATE_START;
                if (wch2 <= 0x03ff) {
                    // This is a surrogate pair.
                    int plane = ((wch - HIGH_SURROGATE_START) >> 6);
                    DWORD offset = (bIsToUpper ? m_pPlaneOffset[plane].OffsetToUppercasing: m_pPlaneOffset[plane].OffsetToLowercasing);
                    if (offset != 0) {
                        LPWORD pSurrogateIndexTable = (LPWORD)(m_pCaseTable + offset );
                        WCHAR result = InternalChangeCaseChar(pSurrogateIndexTable, (wch << 10) + wch2);                
                        // 0xfc00 = 0x1111 1111 1100 0000
                        // We masked off the lowest 6 bits so that the plane of the wch1 is preserved.
                        // Here 
                        // Then we add the highest 6 bits of the result into the high surrogate.
                        // 
                        target[i] = HIGH_SURROGATE_START  + (result >> 10); 
                        // 0x03ff = 0x0000 0011 1111 1111
                        // We get the lowest 10 bits of the result and make it a low surrogate.
                        target[++i] = LOW_SURROGATE_START + (result & 0x3ff);;      
                    } else {
                        target[i] = wch;
                        i++;    // Advance to next character.
                        target[i] = source[i];
                    }
                } else {
                    target[i] = wch;
                }
            } else {
                target[i] = wch;
            }            
        }
    }

    return (target);
}

BOOL NativeTextInfo::IsSurrogateChar(WCHAR ch) {
    // Surrogate char mapping is as following
    //     HIGH_SURROGATE_START  0xd800
    //     HIGH_SURROGATE_END    0xdbff
    //     LOW_SURROGATE_START   0xdc00
    //     LOW_SURROGATE_END     0xdfff
    // So the binary representation of a surrogate char will be like
    //    Surrogate: 11011xxxxxxxxxxx
    //    High Surrogate: 110110xxxxxxxxxx 
    //    Low Surrogate:  110111xxxxxxxxxx 
    LEAF_CONTRACT;
    return ((ch&0xF800) == 0xD800);
}

BOOL NativeTextInfo::IsHighSurrogateChar(WCHAR ch) {
    LEAF_CONTRACT;
    return ((ch&0xFC00) == 0xD800);
}


BOOL NativeTextInfo::IsLowSurrogateChar(WCHAR ch) {
    LEAF_CONTRACT;
    return ((ch&0xFC00) == 0xDC00);
}


INT32 NativeTextInfo::GetHashCodeOrdinalIgnoreCase(__in WCHAR* src) {
    LEAF_CONTRACT;   

    STATIC_CONTRACT_SO_TOLERANT;

    WCHAR     c, c2;
    WCHAR *s = src;
    INT32 hash = 5381;

    while ((c = *s) != 0) {
        // signed extensions or unsigned extension?
        if( IsHighSurrogateChar(c) )  {
            if( IsLowSurrogateChar(c2 = s[1])) {  
                // get a high/low surrogate pair
                int plane = ((c >> 6) &0x0f);
                DWORD offset = m_pPlaneOffset[plane].OffsetToUppercasing;
                if (offset != 0) {
                    LPWORD pIndexTable = (LPWORD)(m_pCaseTable + offset );
                    WCHAR result = InternalChangeCaseChar(pIndexTable, (c << 10) + c2 - LOW_SURROGATE_START);                
                    c = HIGH_SURROGATE_START + (result >> 10);                    
                    c2 = LOW_SURROGATE_START + (result & 0x3ff);      
                }               

                hash = ((hash << 5) + hash) ^ c;
                hash = ((hash << 5) + hash) ^ c2;
                s += 2;
            }
            else {
                hash = ((hash << 5) + hash) ^ c;
                ++s;
            }
            continue;
        }

        if(!IsSurrogateChar(c)) {
            c = InternalChangeCaseChar(m_pUppercaseIndexTable, c);
        }
        hash = ((hash << 5) + hash) ^ c;
        ++s;
    }
    return hash;    
}


// this function does the real ordinal case insensitive comparison

INT32 NativeTextInfo::CompareOrdinalIgnoreCaseHelper(DWORD* strAChars, DWORD* strBChars, int count) {
    LEAF_CONTRACT;   
    STATIC_CONTRACT_SO_TOLERANT;

    if( count == 0) 
        return 0;

    int temp = 0;

    _ASSERTE( count >0);    
    // Only go through fast code path if two strings have the same alignment
    if (((size_t)strAChars & 3) == ((size_t)strBChars & 3)) {
        int unalignedBytesA = (size_t)strAChars & 3;

        _ASSERTE(unalignedBytesA == 0 || unalignedBytesA == 2); 
        // First try to make the strings aligned at DWORD boundary. 
        if( unalignedBytesA != 0 ) {
            LPWSTR ptr1 = (LPWSTR)strAChars;
            LPWSTR ptr2 = (LPWSTR)strBChars;           
            
            if (*ptr1 != *ptr2) {
                temp = ((int)InternalChangeCaseChar(m_pUppercaseIndexTable, *ptr1)
                         - (int)InternalChangeCaseChar(m_pUppercaseIndexTable, *ptr2));
                if( temp != 0) {
                    return temp;
                }
            }

            --count;                
            strAChars = (DWORD *)(ptr1 + 1);      
            strBChars = (DWORD *)(ptr2 + 1);                      
        }

        // Loop comparing a DWORD at a time.
        while (count >= 2) {
            _ASSERTE(IS_ALIGNED((size_t)strAChars, 4) && IS_ALIGNED((size_t)strBChars, 4));
            if ((*strAChars - *strBChars) != 0) {
                LPWSTR ptr1 = (WCHAR*)strAChars;
                LPWSTR ptr2 = (WCHAR*)strBChars;            

                if (*ptr1 != *ptr2) {
                    temp = ((int)InternalChangeCaseChar(m_pUppercaseIndexTable, *ptr1)
                          - (int)InternalChangeCaseChar(m_pUppercaseIndexTable, *ptr2));
                }
                if (temp != 0) {
                    return (temp);
                }

                temp = (int)InternalChangeCaseChar(m_pUppercaseIndexTable, *(ptr1+1))
                     - (int)InternalChangeCaseChar(m_pUppercaseIndexTable, *(ptr2+1));
                if (temp != 0) {
                    return (temp);
                }            
            }
            ++strBChars;
            ++strAChars;
            count -= 2;
        }
    }
    
    // We can exit the loop when we see two different DWORDs and one of them contains surrogate 
    // or they are equal after case conversion. 
    // We can also exit the loop when there is no or only one character left. 
    if( count == 0) {
        return 0;
    }
            
    // we need to handle one special case here. Say we have two strings like:
    //  A HS1 LS1 HS2 LS2  or A HS1 LS1
    //  A HS1 LS2 HS2 LS2  or A HS1 NS
    // we need to go back a char to decide the order
    LPWSTR pwStrB = (LPWSTR)strBChars;
    LPWSTR pwStrA = (LPWSTR)strAChars;

    temp = 0;
    while ((count--) > 0)
    {
        WCHAR charA = *pwStrA++;
        WCHAR charB = *pwStrB++;

        if( charA != charB) {
            charA = InternalChangeCaseChar(m_pUppercaseIndexTable, charA);
            charB = InternalChangeCaseChar(m_pUppercaseIndexTable, charB);

            temp = (int)charA - (int)charB;

            if (temp != 0) {
                return (temp);
            }
        }
    }        

    return 0;
}

INT32 NativeTextInfo::CompareOrdinalIgnoreCase(__in_ecount(Length1) WCHAR* string1, int Length1, __in_ecount(Length2) WCHAR* string2, int Length2) {
    WRAPPER_CONTRACT;

    DWORD *strAChars, *strBChars;
    strAChars = (DWORD*)string1;
    strBChars = (DWORD*)string2;
 
    // If the strings are the same length, compare exactly the right # of chars.
    // If they are different, compare the shortest # + 1 (the '\0').    
    int count = Length1;
    if( count > Length2)
        count = Length2;

    INT32 ret = CompareOrdinalIgnoreCaseHelper(strAChars, strBChars, count);    
    if( ret == 0) {
        return Length1 - Length2;
    }
    else {
        return ret;     
    }
 }
  

INT32 NativeTextInfo::CompareOrdinalIgnoreCaseEx(__in_ecount(Length1) WCHAR* string1, int Length1, __in_ecount(Length2) WCHAR* string2, int Length2, int count) {
    WRAPPER_CONTRACT;
    DWORD *strAChars, *strBChars;
    strAChars = (DWORD*)string1;
    strBChars = (DWORD*)string2;
 
    bool bFinishedCount = true;
    // If the strings are the same length, compare exactly the right # of chars.
    // If they are different, compare the shortest # + 1 (the '\0').    
    int len = Length1;
    if( len > Length2)
        len = Length2;

    if( count > len ) {
        count  = len;
        bFinishedCount = false;            
    }
    
    INT32 ret;

    ret = CompareOrdinalIgnoreCaseHelper(strAChars, strBChars, count);    
    
    if( ret != 0) 
        return ret;
    else if( bFinishedCount )
        return 0;
    else
        return Length1 - Length2;      
 }



/*=================================GetTitleCaseChar==========================
**Action: Get the titlecasing for the specified character.
**Returns: The titlecasing character.
**Arguments:
**      wch
**Exceptions: None.
**  Normally, the titlecasing for a certain character is its uppercase form.  However
**  there are certain titlecasing characters (such as "U+01C4 DZ LATIN CAPITAL LETTER
**  DZ WITH CARON"), which needs special handling (the result will be "U+01C5 
**  LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON).
**  These special cases are stored in m_pTitleCase844, which is a 8:4:4 table.
**  
============================================================================*/

WCHAR NativeTextInfo::GetTitleCaseChar(WCHAR wch) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    
    //
    // Get the title case casing for wch.
    //
    WCHAR wchResult = InternalChangeCaseChar(m_pTitlecaseIndexTable, wch);
    
    if (wchResult == 0) {
        //
        // In the case like U+01c5, U+01c8, the titlecase chars are themselves.
        // We set up the table so that wchResult is zero.
        // So if we see the result is zero, just return wch itself.
        // This setup of table is necessary since the logic below.
        // When wchResult == wch, we will get the titlecase char from
        // the upper case table.  So we need a special way to deal the
        // U+01c5/U+01c8/etc. case.
        //
        return (wch);
    }
    //
    // If the wchResult is the same as wch, it means that this character
    // is not a titlecase character, so it doesn't not have a special 
    // titlecasing case (such as dz ==> Dz).
    // So we have to get the uppercase for this character from the uppercase table.
    //
    if (wchResult == wch) {
        wchResult = ChangeCaseChar(TRUE, wch);
    }
    return (wchResult);
}

INT32 NativeTextInfo::IndexOfStringOrdinalIgnoreCase(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END;
    WCHAR srcChar;
    WCHAR patChar;

    int endPattern = endIndex - patternLength + 1;

    if (endPattern<0) {
        return -1;
    }

    for (int ctrSrc = startIndex; ctrSrc<=endPattern; ctrSrc++) {
        int ctrPat;
        for (ctrPat = 0; (ctrPat < patternLength); ctrPat++) {
            srcChar = source[ctrSrc + ctrPat];
            srcChar = ChangeCaseChar(TRUE, srcChar);
            
            patChar = pattern[ctrPat];
            patChar = ChangeCaseChar(TRUE, patChar);

            if (srcChar!=patChar) {
                break;
            }
        }

        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

INT32 NativeTextInfo::LastIndexOfStringOrdinalIgnoreCase(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END;
    //startIndex is the greatest index into the string.
    int startPattern = startIndex - patternLength + 1;
    WCHAR srcChar;
    WCHAR patChar;

    if (startPattern < 0) {
        return (-1);
    }

    for (int ctrSrc = startPattern; ctrSrc >= endIndex; ctrSrc--) {
        int ctrPat;
        for (ctrPat = 0; (ctrPat<patternLength); ctrPat++) {
            srcChar = source[ctrSrc+ctrPat];
            srcChar = ChangeCaseChar(TRUE, srcChar);

            patChar = pattern[ctrPat];
            patChar = ChangeCaseChar(TRUE, patChar);

            if (srcChar!=patChar) {
                break;
            }
        }
        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

