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
** Class:  COMString
**
**                                        
**
** Purpose: The implementation of the String class.
**
** Date:  March 9, 1998
**
===========================================================*/
#include "common.h"

#include "object.h"
#include "utilcode.h"
#include "excep.h"
#include "frames.h"
#include "field.h"
#include "vars.hpp"
#include "comstringcommon.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "comutilnative.h"
#include "metasig.h"
#include "excep.h"
#include "comnlsinfo.h"


// This should beat other versions of memcpy in the case where we feed it aligned
// data (feeding it unaligned data is a bug in the caller).
/*static*/ void* COMString::memcpyNoGCRefsPtrAligned(void * dest, const void * src, size_t len) 
{
    WRAPPER_CONTRACT;

    _ASSERTE(IS_ALIGNED(dest, sizeof(size_t)));
    _ASSERTE(IS_ALIGNED(const_cast<void*>(src), sizeof(size_t)));

    BYTE* dPtr = reinterpret_cast<BYTE*>(dest);
    size_t diff = reinterpret_cast<BYTE*>(const_cast<void*>(src)) - dPtr;

    // len should always be multiple of 2 as it represents WCHAR in this usage
    _ASSERTE((len & 1) == 0);
    _ASSERTE(IS_ALIGNED(diff, sizeof(size_t)));

    while (len >= 16)
    {
        reinterpret_cast<DWORD*>(dPtr)[0]  = reinterpret_cast<DWORD*>(dPtr + diff)[0];
        reinterpret_cast<DWORD*>(dPtr)[1]  = reinterpret_cast<DWORD*>(dPtr + diff)[1];
        reinterpret_cast<DWORD*>(dPtr)[2]  = reinterpret_cast<DWORD*>(dPtr + diff)[2];
        reinterpret_cast<DWORD*>(dPtr)[3]  = reinterpret_cast<DWORD*>(dPtr + diff)[3];
        dPtr += 16;
        len -= 16;
    }
    if (len > 0)
    {
        if (len & 8)
        {
            reinterpret_cast<DWORD*>(dPtr)[0]  = reinterpret_cast<DWORD*>(dPtr + diff)[0];
            reinterpret_cast<DWORD*>(dPtr)[1]  = reinterpret_cast<DWORD*>(dPtr + diff)[1];
            dPtr += 8;
        }
        if (len & 4)
        {
            reinterpret_cast<DWORD*>(dPtr)[0] = reinterpret_cast<DWORD*>(dPtr + diff)[0];
            dPtr += 4;
        }
        if (len & 2)
        {
            reinterpret_cast<WCHAR*>(dPtr)[0] = reinterpret_cast<WCHAR*>(dPtr + diff)[0];
            dPtr += 2;
        }
    }

    return reinterpret_cast<void*>(dPtr);
}

//
//
// FORWARD DECLARATIONS
//
//
int ArrayContains(WCHAR searchChar, __in_ecount(length) WCHAR *begin, int length);

//
//
// STATIC MEMBER VARIABLES
//
//
STRINGREF* COMString::EmptyStringRefPtr=NULL;



//The special string #defines are used as flag bits for weird strings that have bytes
//after the terminating 0.  The only case where we use this right now is the VB BSTR as
//byte array which is described in MakeStringAsByteArrayFromBytes.
#define SPECIAL_STRING_VB_BYTE_ARRAY 0x100
#define MARKS_VB_BYTE_ARRAY(x) ((x) & SPECIAL_STRING_VB_BYTE_ARRAY)
#define MAKE_VB_TRAIL_BYTE(x)  ((WCHAR)((x) | SPECIAL_STRING_VB_BYTE_ARRAY))
#define GET_VB_TRAIL_BYTE(x)   ((x) & 0xFF)


/*==============================GetEmptyStringRefPtr============================
**Action:  Gets an empty string refptr, cache the result.
**Returns: The retrieved STRINGREF.
==============================================================================*/
STRINGREF* COMString::GetEmptyStringRefPtr() {
    CONTRACTL {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        SO_TOLERANT;
    } CONTRACTL_END;

    EEStringData data(0, L"", TRUE);
    EmptyStringRefPtr = SystemDomain::System()->DefaultDomain()->GetStringObjRefPtrFromUnicodeString(&data);
    return EmptyStringRefPtr;
}

//
//
//  CONSTRUCTORS
//
//

/*===========================StringInitSBytPtrPartialEx===========================
**Action:  Takes a byte *, startIndex, length, and encoding and turns this into a string.
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/

FCIMPL5(Object *, COMString::StringInitSBytPtrPartialEx, StringObject *thisString,
        I1 *ptr, INT32 startIndex, INT32 length, Object *encoding)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCallCheck calls ForbidenGC now
        MODE_COOPERATIVE;
        SO_TOLERANT;		
        PRECONDITION(thisString == 0);                        
        PRECONDITION(ptr != NULL);        		
    } CONTRACTL_END;

    STRINGREF pString = NULL;
    VALIDATEOBJECTREF(encoding);

    HELPER_METHOD_FRAME_BEGIN_RET_1(encoding);
    MethodDescCallSite createString(METHOD__STRING__CREATE_STRING);

    ARG_SLOT args[] = {
        PtrToArgSlot(ptr),
        startIndex,
        length,
        ObjToArgSlot(ObjectToOBJECTREF(encoding)),
    };

    pString = createString.Call_RetSTRINGREF(args);
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pString);
}
FCIMPLEND

// PAL doesn't yet have this API. Need to move into PAL sometime.
// See MSDN for documentation on this function.
static BOOL IsBadStringPtrA(LPCSTR lpsz, UINT_PTR ucchMax)
{
    BOOL bBad = FALSE;
    UINT_PTR ucch = 0;

    if (ucchMax == 0)
        return bBad;

    PAL_TRY {
        // Scan till '\0' or ucchMax whichever is smaller.
        while ((ucch < ucchMax) && (lpsz[ucch++] != '\0'));
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        bBad = TRUE;
    }
    PAL_ENDTRY

    return bBad;
}

/*=============================StringInitCharHelper=============================
**Action:
**Returns:
**Arguments:
**Exceptions:
**Note this
==============================================================================*/
STRINGREF __stdcall COMString::StringInitCharHelper(LPCSTR pszSource, int length) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    STRINGREF pString=NULL;
    int dwSizeRequired=0;
     _ASSERTE(length>=-1);                        
     
    if (!pszSource || length == 0) {
        return GetEmptyString();
    }
    else if ((size_t)pszSource < 64000) {
        COMPlusThrow(kArgumentException, L"Arg_MustBeStringPtrNotAtom");
    }	

    // Make sure we can read from the pointer.
    // This is better than try to read from the pointer and catch the access violation exceptions.
    if( length == -1) {
        if( IsBadStringPtrA(pszSource, (UINT_PTR)INT_MAX)) {
            COMPlusThrowArgumentOutOfRange(L"ptr", L"ArgumentOutOfRange_PartialWCHAR");        
        }
        length = (INT32)strlen(pszSource);
    }
    else {
        if( IsBadReadPtr(pszSource, (UINT_PTR)length + 1)) {
            COMPlusThrowArgumentOutOfRange(L"ptr", L"ArgumentOutOfRange_PartialWCHAR");        
        }        
    }       

    if(length > 0)  {  
        dwSizeRequired=WszMultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszSource, length, NULL, 0);
    }

    if (dwSizeRequired == 0) {
        if (length == 0) {
            return GetEmptyString();
        }
        COMPlusThrow(kArgumentException, L"Arg_InvalidANSIString");
    }

    pString = AllocateString(dwSizeRequired+1);        
    dwSizeRequired = WszMultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)pszSource, length, pString->GetBuffer(), dwSizeRequired);
    if (dwSizeRequired == 0) {
        COMPlusThrow(kArgumentException, L"Arg_InvalidANSIString");
    }

    pString->SetStringLength(dwSizeRequired);
    _ASSERTE((dwSizeRequired + 1 > dwSizeRequired) && pString->GetBuffer()[dwSizeRequired]==0);

    return pString;
}



/*==============================StringInitCharPtr===============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL2(Object *, COMString::StringInitCharPtr, StringObject *stringThis, INT8 *ptr)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
	
    _ASSERTE(stringThis == 0);      // This is the constructor
    Object *result = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    result = OBJECTREFToObject(StringInitCharHelper((LPCSTR)ptr, -1));
    HELPER_METHOD_FRAME_END();
    return result;
}
FCIMPLEND

/*===========================StringInitCharPtrPartial===========================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL4(Object *, COMString::StringInitCharPtrPartial, StringObject *stringThis, INT8 *value,
        INT32 startIndex, INT32 length)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCallCheck calls ForbidenGC now
        MODE_COOPERATIVE;
        PRECONDITION(stringThis ==0);      
        SO_TOLERANT;		
    } CONTRACTL_END;

    STRINGREF pString = NULL;

    //Verify the args.
    if (startIndex<0) {
        FCThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_StartIndex");
    }

    if (length<0) {
        FCThrowArgumentOutOfRange(L"length", L"ArgumentOutOfRange_NegativeLength");
    }

    // This is called directly now. There is no check in managed code.
    if( value == NULL) {
        FCThrowArgumentNull(L"value");
    }

    LPCSTR pBase = (LPCSTR)value;
    LPCSTR pFrom = pBase + startIndex;
    if (pFrom < pBase) {
        // Check for overflow of pointer addition
        FCThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_PartialWCHAR");
    }

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    
    pString = StringInitCharHelper(pFrom, length);
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(pString);
}
FCIMPLEND

/*==================================NewString===================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
STRINGREF COMString::NewString(INT32 length) {
    CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(length>=0);
    } CONTRACTL_END;

    STRINGREF pString;

    if (length<0) {
        return NULL;
    } else {
        pString = AllocateString(length+1);
        pString->SetStringLength(length);
        _ASSERTE((length+ 1 > length) && pString->GetBuffer()[length] == 0);

        return pString;
    }
}


/*==================================NewString===================================
**Action: Many years ago, VB didn't have the concept of a byte array, so enterprising
**        users created one by allocating a BSTR with an odd length and using it to
**        store bytes.  A generation later, we're still stuck supporting this behavior.
**        The way that we do this is to take advantage of the difference between the
**        array length and the string length.  The string length will always be the
**        number of characters between the start of the string and the terminating 0.
**        If we need an odd number of bytes, we'll take one wchar after the terminating 0.
**        (e.g. at position StringLength+1).  The high-order byte of this wchar is
**        reserved for flags and the low-order byte is our odd byte. This function is
**        used to allocate a string of that shape, but we don't actually mark the
**        trailing byte as being in use yet.
**Returns: A newly allocated string.  Null if length is less than 0.
**Arguments: length -- the length of the string to allocate
**           bHasTrailByte -- whether the string also has a trailing byte.
**Exceptions: OutOfMemoryException if AllocateString fails.
==============================================================================*/
STRINGREF COMString::NewString(INT32 length, BOOL bHasTrailByte) {
    CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(length>=0);
    } CONTRACTL_END;

    INT32 allocLen=0;
    STRINGREF pString;
    if (length<0) {
        return NULL;
    } else {
        allocLen = length + (bHasTrailByte?1:0);
        pString = AllocateString(allocLen+1);
        pString->SetStringLength(length);
        _ASSERTE( (length + 1 > length) && pString->GetBuffer()[length]==0);
        if (bHasTrailByte) {
            _ASSERTE( (allocLen+1 > allocLen) && pString->GetBuffer()[length+1]==0);
        }
    }

    return pString;
}

//========================================================================
// Creates a System.String object and initializes from
// the supplied null-terminated C string.
//
// Maps NULL to null. This function does *not* return null to indicate
// error situations: it throws an exception instead.
//========================================================================
STRINGREF COMString::NewString(const WCHAR *pwsz)
{
    CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (!pwsz)
    {
        return NULL;
    }
    else
    {

        DWORD nch = (DWORD)wcslen(pwsz);
        if (nch==0) {
            return GetEmptyString();
        }


        STRINGREF pString = AllocateString( nch + 1);

        memcpyNoGCRefs(pString->GetBuffer(), pwsz, nch*sizeof(WCHAR));
        pString->SetStringLength(nch);
        _ASSERTE(pString->GetBuffer()[nch] == 0);
        return pString;
    }
}

STRINGREF COMString::NewString(const WCHAR *pwsz, int length) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(length>=0);
    } CONTRACTL_END;

    if (!pwsz)
    {
        return NULL;
    }
    else if (length==0) {
        return GetEmptyString();
    } else {

        STRINGREF pString = AllocateString( length + 1);

        memcpyNoGCRefs(pString->GetBuffer(), pwsz, length*sizeof(WCHAR));
        pString->SetStringLength(length);
        _ASSERTE( (length+1 > length) && pString->GetBuffer()[length] == 0);
        return pString;
    }
}

STRINGREF COMString::NewString(LPCUTF8 psz)
{
    CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
        PRECONDITION(CheckPointer(psz));
    } CONTRACTL_END;

    int length = (int)strlen(psz);
    if (length == 0) {
        return GetEmptyString();
    }
    CQuickBytes qb;
    WCHAR* pwsz = (WCHAR*) qb.AllocThrows((length) * sizeof(WCHAR));
    length = WszMultiByteToWideChar(CP_UTF8, 0, psz, length, pwsz, length);
    if (length == 0) {
        COMPlusThrow(kArgumentException, L"Arg_InvalidUTF8String");
    }
    return NewString(pwsz, length);
}

STRINGREF COMString::NewString(LPCUTF8 psz, int cBytes)
{
    CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
        PRECONDITION(CheckPointer(psz, NULL_OK));
    } CONTRACTL_END;

    if (!psz)
        return NULL;

    _ASSERTE(psz);
    _ASSERTE(cBytes >= 0);
    if (cBytes == 0) {
        return GetEmptyString();
    }
    CQuickBytes qb;
    WCHAR* pwsz = (WCHAR*) qb.AllocThrows((cBytes) * sizeof(WCHAR));
    int length = WszMultiByteToWideChar(CP_UTF8, 0, psz, cBytes, pwsz, cBytes);
    if (length == 0) {
        COMPlusThrow(kArgumentException, L"Arg_InvalidUTF8String");
    }
    return NewString(pwsz, length);
}

STRINGREF COMString::NewString(STRINGREF *srChars, int start, int length) {
    WRAPPER_CONTRACT;
    return NewString(srChars, start, length, length);
}

STRINGREF COMString::NewString(STRINGREF *srChars, int start, int length, int capacity) {
    CONTRACTL {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;


    if (length==0 && capacity==0) {
        return GetEmptyString();
    }

    STRINGREF pString = AllocateString( capacity + 1);

        memcpyNoGCRefs(pString->GetBuffer(),&(((*srChars)->GetBuffer())[start]), length*sizeof(WCHAR));
    pString->SetStringLength(length);
    _ASSERTE(pString->GetBuffer()[length] == 0);

    return pString;
}

STRINGREF COMString::NewString(I2ARRAYREF *srChars, int start, int length) {
    WRAPPER_CONTRACT;
    return NewString(srChars, start, length, length);
}

STRINGREF COMString::NewString(I2ARRAYREF *srChars, int start, int length, int capacity) {
    CONTRACTL {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;


    if (length==0 && capacity==0) {
        return GetEmptyString();
    }

    STRINGREF pString = AllocateString( capacity + 1);

    memcpyNoGCRefs(pString->GetBuffer(),&(((*srChars)->GetDirectPointerToNonObjectElements())[start]), length*sizeof(WCHAR));
    pString->SetStringLength(length);
    _ASSERTE(pString->GetBuffer()[length] == 0);

    return pString;
}

/*===============================IsFastSort===============================
**Action: Call the helper to walk the string and see if we have any high chars.
**Returns: void.  The appropriate bits are set on the String.
**Arguments: vThisRef - The string to be checked.
**Exceptions: None.
==============================================================================*/
FCIMPL1(FC_BOOL_RET, COMString::IsFastSort, StringObject* thisRef) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(thisRef);
    _ASSERTE(thisRef!=NULL);
    DWORD state = thisRef->GetHighCharState();
    if (IS_STRING_STATE_UNDETERMINED(state)) {
        state = InternalCheckHighChars(STRINGREF(thisRef));
        FC_GC_POLL_RET();
    }
    else {
        FC_GC_POLL_NOT_NEEDED();
    }
    FC_RETURN_BOOL(IS_FAST_SORT(state)); //This can indicate either high chars or special sorting chars.
}
FCIMPLEND

/*===============================IsAscii===============================
**Action: Call the helper to walk the string and see if we have any high chars.
**Returns: void.  The appropriate bits are set on the String.
**Arguments: vThisRef - The string to be checked.
**Exceptions: None.
==============================================================================*/
FCIMPL1(FC_BOOL_RET, COMString::IsAscii, StringObject* thisRef) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;


    VALIDATEOBJECTREF(thisRef);
    _ASSERTE(thisRef!=NULL);
    DWORD state = thisRef->GetHighCharState();
    if (IS_STRING_STATE_UNDETERMINED(state)) {
        state = InternalCheckHighChars(STRINGREF(thisRef));
        FC_GC_POLL_RET();
    }
    else {
        FC_GC_POLL_NOT_NEEDED();
    }
    FC_RETURN_BOOL(IS_ASCII(state)); //This can indicate either high chars or special sorting chars.
}
FCIMPLEND


/*===============================ValidModifiableString===============================*/

#ifdef _DEBUG
FCIMPL1(FC_BOOL_RET, COMString::ValidModifiableString, StringObject* thisRef) {
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
	
    FC_GC_POLL_NOT_NEEDED();
    _ASSERTE(thisRef!=NULL);
    VALIDATEOBJECTREF(thisRef);
        // we disallow these bits to be set because stringbuilder is going to modify the
        // string, which will invalidate them.
    FC_RETURN_BOOL(IS_STRING_STATE_UNDETERMINED(thisRef->GetHighCharState()));
}
FCIMPLEND
#endif


// strAChars must be null-terminated, with an appropriate aLength
// strBChars must be null-terminated, with an appropriate bLength OR bLength == -1
// If bLength == -1, we stop on the first null character in strBChars
BOOL COMString::CaseInsensitiveCompHelper(__in_ecount(aLength) WCHAR *strAChars, __in_z WCHAR *strBChars, INT32 aLength, INT32 bLength, INT32 *result) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(strAChars));
        PRECONDITION(CheckPointer(strBChars));
        PRECONDITION(CheckPointer(result));
        SO_TOLERANT;
    } CONTRACTL_END;

    WCHAR *strAStart = strAChars;
    WCHAR *strBStart = strBChars;
    unsigned charA;
    unsigned charB;

    for(;;) {        
        charA = *strAChars;
        charB = *strBChars;

        //Case-insensitive comparison on chars greater than 0x7F
        //requires a locale-aware casing operation and we're not going there.
        if ((charA|charB)>0x7F) {
            *result = 0;
            return FALSE;
        }

        // uppercase both chars. 
        if (charA>='a' && charA<='z') {
            charA ^= 0x20;
        } 
        if (charB>='a' && charB<='z') {
            charB ^= 0x20;
        }

        //Return the (case-insensitive) difference between them.
        if (charA!=charB) {
            *result = (int)(charA-charB);
            return TRUE;
        }


        if (charA==0)   // both strings have null character
        {
            if (bLength == -1)
            {
                *result = aLength - (strAChars - strAStart); 
                return TRUE;
            }
            if (strAChars==strAStart + aLength || strBChars==strBStart + bLength)
            {
                *result = aLength - bLength; 
                return TRUE;
            }
            // else both embedded zeros
        } 

        // Next char
        strAChars++; strBChars++;
    }
	
}

FORCEINLINE INT32 FastCompareStringHelper(DWORD* strAChars, INT32 countA, DWORD* strBChars, INT32 countB)
{
    STATIC_CONTRACT_SO_TOLERANT;
    INT32 count    = (countA < countB) ? countA : countB;
    ptrdiff_t diff = (char *)strAChars - (char *)strBChars;

#if defined(_WIN64) || defined(ALIGN_ACCESS)
    int alignmentA = ((SIZE_T)strAChars) & (sizeof(SIZE_T) - 1);
    int alignmentB = ((SIZE_T)strBChars) & (sizeof(SIZE_T) - 1);
#endif // _WIN64 || ALIGN_ACCESS

#if defined(ALIGN_ACCESS)
    if ( ( !IS_ALIGNED((size_t)strAChars, sizeof(DWORD)) || 
           !IS_ALIGNED((size_t)strBChars, sizeof(DWORD)) )  && 
         (abs(alignmentA - alignmentB) != 4) )
    {
        _ASSERTE(IS_ALIGNED((size_t)strAChars, sizeof(WCHAR)));
        _ASSERTE(IS_ALIGNED((size_t)strBChars, sizeof(WCHAR)));
        LPWSTR ptr2 = (WCHAR *)strBChars;

        while ((count -= 1) >= 0)
        {
            if (( *((WCHAR*)((char *)ptr2 + diff)) - *ptr2) != 0)
            {
                return ((int)*((WCHAR*)((char *)ptr2 + diff)) - (int)*ptr2);
            }
            ++ptr2;
        }
    }
    else
#endif // ALIGN_ACCESS
    {
#if defined(_WIN64) || defined(ALIGN_ACCESS)
        if (abs(alignmentA - alignmentB) == 4)
        {
            if ((alignmentA == 2) || (alignmentB == 2))
            {
                LPWSTR ptr2 = (WCHAR *)strBChars;

                if (( *((WCHAR*)((char *)ptr2 + diff)) - *ptr2) != 0)
                {
                    return ((int)*((WCHAR*)((char *)ptr2 + diff)) - (int)*ptr2);
                }
                strBChars = (DWORD*)(++ptr2);
                count -= 1;
            }
        }
#endif // WIN64 || ALIGN_ACCESS

        // Loop comparing a DWORD at a time.
        while ((count -= 2) >= 0)
        {
            if ((*((DWORD* )((char *)strBChars + diff)) - *strBChars) != 0)
            {
                LPWSTR ptr1 = (WCHAR*)((char *)strBChars + diff);
                LPWSTR ptr2 = (WCHAR*)strBChars;
                if (*ptr1 != *ptr2) {
                    return ((int)*ptr1 - (int)*ptr2);
                }
                return ((int)*(ptr1+1) - (int)*(ptr2+1));
            }
            ++strBChars;
        }

        int c;
        if (count == -1)
            if ((c = *((WCHAR *) ((char *)strBChars + diff)) - *((WCHAR *) strBChars)) != 0)
                return c;
    }

    return countA - countB;
}

// For aligned string comparisons this will significantly beat the normal
// FastCompareStringHelper. Feeding it non-aligned data is a bug in the caller.
FORCEINLINE INT32 FastCompareStringHelperAligned(DWORD* strAChars, INT32 countA, DWORD* strBChars, INT32 countB)
{
    STATIC_CONTRACT_SO_TOLERANT;
    INT32 count    = (countA < countB) ? countA : countB;
    ptrdiff_t diff = (char *)strAChars - (char *)strBChars;

    // Loop comparing a DWORD at a time.
    while ((count -= 2) >= 0)
    {
        if ((*((DWORD* )((char *)strBChars + diff)) - *strBChars) != 0)
        {
            LPWSTR ptr1 = (WCHAR*)((char *)strBChars + diff);
            LPWSTR ptr2 = (WCHAR*)strBChars;
            if (*ptr1 != *ptr2) {
                return ((int)*ptr1 - (int)*ptr2);
            }
            return ((int)*(ptr1+1) - (int)*(ptr2+1));
        }
        ++strBChars;
    }

    int c;
    if (count == -1)
        if ((c = *((WCHAR *) ((char *)strBChars + diff)) - *((WCHAR *) strBChars)) != 0)
            return c;


    return countA - countB;
}

/*================================CompareOrdinal===============================*/
FCIMPL3(INT32, COMString::FCCompareOrdinal, StringObject* strA, StringObject* strB, CLR_BOOL bIgnoreCase) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    VALIDATEOBJECTREF(strA);
    VALIDATEOBJECTREF(strB);
    DWORD *strAChars, *strBChars;
    INT32 strALength, strBLength;

    //Checks for null are handled in the managed code.
    RefInterpretGetStringValuesDangerousForGC(strA, (WCHAR **) &strAChars, &strALength);
    RefInterpretGetStringValuesDangerousForGC(strB, (WCHAR **) &strBChars, &strBLength);

    //Handle the comparison where we wish to ignore case.
    if (bIgnoreCase) {
        INT32 result;
        if (CaseInsensitiveCompHelper((WCHAR *)strAChars, (WCHAR *)strBChars, strALength, strBLength, &result)) {
            return result;
        } else {
            //This will happen if we have characters greater than 0x7F.
            FCThrow(kArgumentException);
        }
    }

    // FastCompareStringHelperAligned use by default here assumes that the WCHAR[] in 
    // StringObject is pointer aligned
    _ASSERTE(IS_ALIGNED(StringObject::GetBufferOffset(), sizeof(SIZE_T)));
    
    INT32 result = FastCompareStringHelperAligned(strAChars, strALength, strBChars, strBLength);

    FC_GC_POLL_RET();
    return result; 
}
FCIMPLEND

//This function relies on the fact that we put a terminating null on the end of
//all managed strings.
FCIMPL4(INT32, COMString::FCCompareOrdinalWC, StringObject* strA, __in WCHAR *strBChars, CLR_BOOL bIgnoreCase, CLR_BOOL *bSuccess) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(strA);
    WCHAR *strAChars;
    WCHAR *strAStart;
    INT32 aLength;
    INT32 ret;

    *bSuccess = 1;

    _ASSERT(strA != NULL && strBChars != NULL);

    //Get our data.
    RefInterpretGetStringValuesDangerousForGC(strA, (WCHAR **) &strAChars, &aLength);

    //Record the start pointer for some comparisons at the end.
    strAStart = strAChars;

    if (!bIgnoreCase) { //Handle the case-sensitive comparison first
        while ( *strAChars==*strBChars && *strAChars!='\0') {
            strAChars++; strBChars++;
        }
        if (*strAChars!=*strBChars) {
            ret = INT32(*strAChars - *strBChars);
        }

        //We've reached a terminating null in string A, so we need to ensure that
        //String B isn't a substring of A.  (A may have an embedded null.  B is
        //known to be a null-terminated string.)  We do this by comparing the number
        //of characters which we walked in A with the expected length.
        else if ( (strAChars - strAStart) != aLength) {
            ret = 1;
        }
        else {
            //The two strings were equal.
            ret = 0;
        }
    } else { //Handle the case-insensitive comparison separately.
        if (!CaseInsensitiveCompHelper(strAChars, strBChars, aLength, -1, &ret)) {
            //This will happen if we have characters greater than 0x7F. This indicates that the function failed.
            // We don't throw an exception here. You can look at the success value returned to do something meaningful.
            *bSuccess = 0;
            ret = 1;
        }
    }
    FC_GC_POLL_RET();
    return ret;
}
FCIMPLEND

INT32 DoLookup(wchar_t charA, wchar_t charB) {
    LEAF_CONTRACT;

    if ((charA ^ charB) & 0x20) {
        //We may be talking about a special case
        if (charA>='A' && charA<='Z') {
            return charB - charA;
        }

        if (charA>='a' && charA<='z') {
            return charB - charA;
        }
    }

    return charA-charB;
}

/*================================CompareOrdinalEx===============================
**Args: typedef struct {STRINGREF thisRef; INT32 options; INT32 length; INT32 valueOffset;\
        STRINGREF value; INT32 thisOffset;} _compareOrdinalArgsEx;
==============================================================================*/

FCIMPL5(INT32, COMString::CompareOrdinalEx, StringObject* strA, INT32 indexA, StringObject* strB, INT32 indexB, INT32 count)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(strA);
    VALIDATEOBJECTREF(strB);
    DWORD *strAChars, *strBChars;
    int strALength, strBLength;

    // This runtime test is handled in the managed wrapper.
    _ASSERTE(strA != NULL && strB != NULL);

    //If any of our indices are negative throw an exception.
    if (count<0)
    {
        FCThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_MustBePositive");
    }
    if (indexA < 0)
    {
        FCThrowArgumentOutOfRange(L"indexA", L"ArgumentOutOfRange_MustBePositive");
    }
    if (indexB < 0)
    {
        FCThrowArgumentOutOfRange(L"indexB", L"ArgumentOutOfRange_MustBePositive");
    }

    RefInterpretGetStringValuesDangerousForGC(strA, (WCHAR **) &strAChars, &strALength);
    RefInterpretGetStringValuesDangerousForGC(strB, (WCHAR **) &strBChars, &strBLength);

    int countA = count;
    int countB = count;

    //Do a lot of range checking to make sure that everything is kosher and legit.
    if (count  > (strALength - indexA)) {
        countA = strALength - indexA;
        if (countA < 0)
            FCThrowArgumentOutOfRange(L"indexA", L"ArgumentOutOfRange_Index");
    }

    if (count > (strBLength - indexB)) {
        countB = strBLength - indexB;
        if (countB < 0)
            FCThrowArgumentOutOfRange(L"indexB", L"ArgumentOutOfRange_Index");
    }

    // Set up the loop variables.
    strAChars = (DWORD *) ((WCHAR *) strAChars + indexA);
    strBChars = (DWORD *) ((WCHAR *) strBChars + indexB);

    INT32 result;
        result = FastCompareStringHelper(strAChars, countA, strBChars, countB);

    FC_GC_POLL_RET();
    return result;

}
FCIMPLEND

/*=================================IndexOfChar==================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/

FCIMPL4 (INT32, COMString::IndexOfChar, StringObject* thisRef, CLR_CHAR value, INT32 startIndex, INT32 count )
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(thisRef);
    if (thisRef==NULL)
        FCThrow(kNullReferenceException);

    WCHAR *thisChars;
    int thisLength;

    RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

    if (startIndex < 0 || startIndex > thisLength) {
        FCThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");
    }

    if (count   < 0 || count > thisLength - startIndex) {
        FCThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_Count");
    }

    int endIndex = startIndex + count;
    for (int i=startIndex; i<endIndex; i++)
    {
        if (thisChars[i]==((WCHAR)value))
        {
            FC_GC_POLL_RET();
            return i;
        }
    }

    FC_GC_POLL_RET();
    return -1;
}
FCIMPLEND

/*===============================IndexOfCharArray===============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL4(INT32, COMString::IndexOfCharArray, StringObject* thisRef, CHARArray* valueRef, INT32 startIndex, INT32 count )
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(thisRef);
    VALIDATEOBJECTREF(valueRef);

    if (thisRef==NULL)
        FCThrow(kNullReferenceException);
    if (valueRef==NULL)
        FCThrow(kArgumentNullException);

    WCHAR *thisChars;
    WCHAR *valueChars;
    int valueLength;
    int thisLength;

    RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

    if (startIndex<0 || startIndex>thisLength) {
        FCThrow(kArgumentOutOfRangeException);
    }

    if (count < 0 || count > thisLength - startIndex) {
        FCThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_Count");
    }


    int endIndex = startIndex + count;

    valueLength = valueRef->GetNumComponents();
    valueChars = (WCHAR *)valueRef->GetDataPtr();

    for (int i=startIndex; i<endIndex; i++) {
        if (ArrayContains(thisChars[i], valueChars, valueLength) >= 0) {
            FC_GC_POLL_RET();
            return i;
        }
    }

    FC_GC_POLL_RET();
    return -1;
}
FCIMPLEND


/*===============================LastIndexOfChar================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/

FCIMPL4(INT32, COMString::LastIndexOfChar, StringObject* thisRef, CLR_CHAR value, INT32 startIndex, INT32 count )
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(thisRef);
    WCHAR *thisChars;
    int thisLength;

    if (thisRef==NULL) {
        FCThrow(kNullReferenceException);
    }

    RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

    if (thisLength == 0) {
        FC_GC_POLL_RET();
        return -1;
    }


    if (startIndex<0 || startIndex>=thisLength) {
        FCThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");
    }

    if (count<0 || count - 1 > startIndex) {
        FCThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_Count");
    }

    int endIndex = startIndex - count + 1;

    //We search [startIndex..EndIndex]
    for (int i=startIndex; i>=endIndex; i--) {
        if (thisChars[i]==((WCHAR)value)) {
            FC_GC_POLL_RET();
            return i;
        }
    }

    FC_GC_POLL_RET();
    return -1;
}
FCIMPLEND
/*=============================LastIndexOfCharArray=============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/

FCIMPL4(INT32, COMString::LastIndexOfCharArray, StringObject* thisRef, CHARArray* valueRef, INT32 startIndex, INT32 count )
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    VALIDATEOBJECTREF(thisRef);
    VALIDATEOBJECTREF(valueRef);
    WCHAR *thisChars, *valueChars;
    int thisLength, valueLength;

    if (thisRef==NULL) {
        FCThrow(kNullReferenceException);
    }

    if (valueRef==NULL)
        FCThrow(kArgumentNullException);

    RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

    if (thisLength == 0) {
        return -1;
    }

    if (startIndex<0 || startIndex>=thisLength) {
        FCThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");
    }

    if (count<0 || count - 1 > startIndex) {
        FCThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_Count");
    }


    valueLength = valueRef->GetNumComponents();
    valueChars = (WCHAR *)valueRef->GetDataPtr();

    int endIndex = startIndex - count + 1;

    //We search [startIndex..EndIndex]
    for (int i=startIndex; i>=endIndex; i--) {
        if (ArrayContains(thisChars[i],valueChars, valueLength) >= 0) {
            FC_GC_POLL_RET();
            return i;
        }
    }

    FC_GC_POLL_RET();
    return -1;
}
FCIMPLEND

/*==================================GETCHARAT===================================
**Returns the character at position index.  Thows IndexOutOfRangeException as
**appropriate.
**This method is not actually used. JIT will generate code for indexer method on string class.
**
==============================================================================*/
FCIMPL2(FC_CHAR_RET, COMString::GetCharAt, StringObject* str, INT32 index) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    FC_GC_POLL_NOT_NEEDED();
    VALIDATEOBJECTREF(str);
    if (str == NULL) {
        FCThrow(kNullReferenceException);
    }
    _ASSERTE(str->GetMethodTable() == g_pStringClass);

    if (index >=0 && index < (INT32)str->GetStringLength()) {
        //Return the appropriate character.
          return str->GetBuffer()[index];
    }

    FCThrow(kIndexOutOfRangeException);
}
FCIMPLEND


/*==================================LENGTH=================================== */

FCIMPL1(INT32, COMString::Length, StringObject* str) {
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    FC_GC_POLL_NOT_NEEDED();
    if (str == NULL)
        FCThrow(kNullReferenceException);

    return str->GetStringLength();
}
FCIMPLEND


/*==================================PadHelper===================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL4(Object*, COMString::PadHelper, StringObject* thisRefUNSAFE, INT32 totalWidth, CLR_CHAR paddingChar, CLR_BOOL isRightPadded)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    STRINGREF thisRef = (STRINGREF) thisRefUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, thisRef);
    //-[autocvtpro]-------------------------------------------------------

    WCHAR *thisChars, *padChars;
    INT32 thisLength;


    if (thisRef==NULL) {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

    //Don't let them pass in a negative totalWidth
    if (totalWidth<0) {
        COMPlusThrowArgumentOutOfRange(L"totalWidth", L"ArgumentOutOfRange_NeedNonNegNum");
    }

    //If the string is longer than the length which they requested, give them
    //back the old string.
    if (totalWidth<thisLength) {
        refRetVal = thisRef;
        goto lExit;
    }

    if (isRightPadded) {
        refRetVal = NewString(&(thisRef), 0, thisLength, totalWidth);
        padChars = refRetVal->GetBuffer();
        for (int i=thisLength; i<totalWidth; i++) {
            padChars[i] = paddingChar;
        }
        refRetVal->SetStringLength(totalWidth);
        _ASSERTE(padChars[totalWidth] == 0);
    } else {
        refRetVal = NewString(totalWidth);
        INT32 startingPos = totalWidth-thisLength;
        padChars = refRetVal->GetBuffer();
        // Reget thisChars, since if NewString triggers GC, thisChars may become trash.
        RefInterpretGetStringValuesDangerousForGC(thisRef, &thisChars, &thisLength);

            memcpyNoGCRefs(padChars+startingPos, thisChars, thisLength * sizeof(WCHAR));

        for (int i=0; i<startingPos; i++) {
            padChars[i] = paddingChar;
        }
    }

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

/*===================================Replace====================================
**Action: Replaces all instances of oldChar with newChar.
**Returns: A new String with all instances of oldChar replaced with newChar
**Arguments: oldChar -- the character to replace
**           newChar -- the character with which to replace oldChar.
**Exceptions: None
==============================================================================*/
FCIMPL3(LPVOID, COMString::Replace, StringObject* thisRefUNSAFE, CLR_CHAR oldChar, CLR_CHAR newChar)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    int length = 0;
    int firstFoundIndex = -1;
    WCHAR *oldBuffer = NULL;
    WCHAR *newBuffer;

    STRINGREF   newString   = NULL;
    STRINGREF   thisRef     = (STRINGREF)thisRefUNSAFE;

    if (thisRef==NULL) {
        FCThrowRes(kNullReferenceException, L"NullReference_This");
    }

    //Perf: If no replacements required, return initial reference
    oldBuffer = thisRef->GetBuffer();
    length = thisRef->GetStringLength();

    for(int i=0; i<length; i++) 
    {
        if ((WCHAR)oldChar==oldBuffer[i])
        {
            firstFoundIndex = i;
            break;
        }
    }

    if (-1==firstFoundIndex)
    {	
        return thisRefUNSAFE;
    }


    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, newString, thisRef);

    //Get the length and allocate a new String
    //We will definitely do an allocation here, but there's nothing which
    //requires GC_PROTECT.
    newString = NewString(length);

    //After allocation, thisRef may have moved
    oldBuffer = thisRef->GetBuffer();

    //Get the buffers in both of the Strings.
    newBuffer = newString->GetBuffer();

    //Copy the characters, doing the replacement as we go.
    for (int i=0; i<firstFoundIndex; i++) {
        newBuffer[i]=oldBuffer[i];
    }
    for (int i=firstFoundIndex; i<length; i++) {
        newBuffer[i]=(oldBuffer[i]==((WCHAR)oldChar))?((WCHAR)newChar):oldBuffer[i];
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(newString);
}
FCIMPLEND


/*====================================Insert====================================
**Action:Inserts a new string into the given string at position startIndex
**       Inserting at String.length is equivalent to appending the string.
**Returns: A new string with value inserted.
**Arguments: value -- the string to insert
**           startIndex -- the position at which to insert it.
**Exceptions: ArgumentException if startIndex is not a valid index or value is null.
==============================================================================*/
FCIMPL3(Object*, COMString::Insert, StringObject* thisRefUNSAFE, INT32 startIndex, StringObject* valueUNSAFE)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    STRINGREF thisRef = (STRINGREF) thisRefUNSAFE;
    STRINGREF value = (STRINGREF) valueUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, thisRef, value);
    //-[autocvtpro]-------------------------------------------------------

    int thisLength, newLength, valueLength;
    WCHAR *newChars;
    WCHAR *thisChars;
    WCHAR *valueChars;

    if (thisRef==NULL) {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    //Check the Arguments
    thisLength = thisRef->GetStringLength();
    if (startIndex<0 || startIndex>thisLength) {
        COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");
    }
    if (!value) {
        COMPlusThrowArgumentNull(L"value",L"ArgumentNull_String");
    }

    //Allocate a new String.
    valueLength = value->GetStringLength();
    newLength = thisLength + valueLength;
    refRetVal = NewString(newLength);

    //Get the buffers to access the characters directly.
    newChars = refRetVal->GetBuffer();
    thisChars = thisRef->GetBuffer();
    valueChars = value->GetBuffer();

    //Copy all of the characters to the appropriate locations.
    COMString::memcpyNoGCRefsPtrAligned(newChars, thisChars, (startIndex*sizeof(WCHAR)));
    
    newChars+=startIndex;
        memcpyNoGCRefs(newChars, valueChars, valueLength*sizeof(WCHAR));
    
    newChars+=valueLength;
        memcpyNoGCRefs(newChars, thisChars+startIndex, (thisLength - startIndex)*sizeof(WCHAR));

    //Set the String length and return;
    //We'll count on the fact that Strings are 0 initialized to set the terminating null.
    refRetVal->SetStringLength(newLength);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


/*====================================Remove====================================
**Action: Removes a range from args->startIndex to args->startIndex+args->count
**        from this string.
**Returns: A new string with the specified range removed.
**Arguments: startIndex -- the position from which to start.
**           count -- the number of characters to remove
**Exceptions: ArgumentException if startIndex and count do not specify a valid
**            range.
==============================================================================*/
FCIMPL3(Object*, COMString::Remove, StringObject* thisRefUNSAFE, INT32 startIndex, INT32 count)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    STRINGREF thisRef = (STRINGREF) thisRefUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, thisRef);
    //-[autocvtpro]-------------------------------------------------------

    int thisLength, newLength;
    WCHAR *newChars;
    WCHAR *thisChars;

    if (thisRef==NULL) {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    //Range check everything;
    thisLength = thisRef->GetStringLength();
    if (count<0) {
        COMPlusThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_NegativeCount");
    }
    if (startIndex<0) {
        COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_StartIndex");
    }

    if ((count) > (thisLength-startIndex)) {
        COMPlusThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_IndexCount");
    }

    //Calculate the new length and allocate a new string.
    newLength = thisLength - count;
    refRetVal = NewString(newLength);

    //Get pointers to the character arrays.
    thisChars = thisRef->GetBuffer();
    newChars = refRetVal->GetBuffer();

    //Copy the appropriate characters to the correct locations.
    COMString::memcpyNoGCRefsPtrAligned(newChars, thisChars, startIndex * sizeof (WCHAR));
        memcpyNoGCRefs(&(newChars[startIndex]), &(thisChars[startIndex + count]), (thisLength-(startIndex + count))*sizeof(WCHAR));

    //Set the string length, null terminator and exit.
    refRetVal->SetStringLength(newLength);
    _ASSERTE(newChars[newLength]==0);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

//
//
// HELPER METHODS
//
//

/*================================ArrayContains=================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
int ArrayContains(WCHAR searchChar, __in_ecount(length) WCHAR *begin, int length) {
    LEAF_CONTRACT;
    _ASSERTE(begin != NULL);
    _ASSERTE(length >= 0);

    for(int i = 0; i < length; i++) {
        if(begin[i] == searchChar) {
            return i;
        }
    }
    return -1;
}


/*================================ReplaceString=================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL3(Object*, COMString::ReplaceString, StringObject* thisRefUNSAFE, StringObject* oldValueUNSAFE, StringObject* newValueUNSAFE)
{
    CONTRACTL {
        THROWS;
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;		
    } CONTRACTL_END;

    struct _gc
    {
        STRINGREF     thisRef;
        STRINGREF     oldValue;
        STRINGREF     newValue;
        STRINGREF     retValString;
    } gc;

    gc.thisRef        = ObjectToSTRINGREF(thisRefUNSAFE);
    gc.oldValue       = ObjectToSTRINGREF(oldValueUNSAFE);
    gc.newValue       = ObjectToSTRINGREF(newValueUNSAFE);
    gc.retValString   = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    int *replaceIndex;
    int index=0;
    int replaceCount=0;
    int readPos, writePos;
    WCHAR *thisBuffer, *oldBuffer, *newBuffer, *retValBuffer;
    int thisLength, oldLength, newLength;
    int endIndex;
    CQuickBytes replaceIndices;


    if (gc.thisRef==NULL) {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    //Verify all of the arguments.
    if (!gc.oldValue) {
        COMPlusThrowArgumentNull(L"oldValue", L"ArgumentNull_Generic");
    }

    //If they asked to replace oldValue with a null, replace all occurances
    //with the empty string.
    if (!gc.newValue) {
        gc.newValue = COMString::GetEmptyString();
    }

    RefInterpretGetStringValuesDangerousForGC(gc.thisRef,  &thisBuffer, &thisLength);
    RefInterpretGetStringValuesDangerousForGC(gc.oldValue, &oldBuffer,  &oldLength);
    RefInterpretGetStringValuesDangerousForGC(gc.newValue, &newBuffer,  &newLength);

    //Record the endIndex so that we don't need to do this calculation all over the place.
    endIndex = thisLength;

    //If our old Length is 0, we won't know what to replace
    if (oldLength==0) {
        COMPlusThrowArgumentException(L"oldValue", L"Argument_StringZeroLength");
    }

    //replaceIndex is made large enough to hold the maximum number of replacements possible:
    //The case where every character in the current buffer gets replaced.
    replaceIndex = (int *)replaceIndices.AllocThrows((thisLength/oldLength+1)*sizeof(int));
    index=0;
    while (((index=COMStringBuffer::LocalIndexOfString(thisBuffer,oldBuffer,thisLength,oldLength,index))>-1) && (index<=endIndex-oldLength))
    {
        replaceIndex[replaceCount++] = index;
        index+=oldLength;
    }

    if (replaceCount != 0)
    {
        //Calculate the new length of the string and ensure that we have sufficent room.
        INT64 retValBuffLength = thisLength - ((oldLength - newLength) * (INT64)replaceCount);
        _ASSERTE(retValBuffLength >= 0);
        if (retValBuffLength > 0x7FFFFFFF)
            COMPlusThrowOM();

        gc.retValString = COMString::NewString((INT32)retValBuffLength);
        retValBuffer = gc.retValString->GetBuffer();

        //Get the update buffers for all the Strings since the allocation could have triggered a GC.
        thisBuffer  = gc.thisRef->GetBuffer();
        newBuffer   = gc.newValue->GetBuffer();
        oldBuffer   = gc.oldValue->GetBuffer();


        //Set replaceHolder to be the upper limit of our array.
        int replaceHolder = replaceCount;
        replaceCount=0;

        //Walk the array forwards copying each character as we go.  If we reach an instance
        //of the string being replaced, replace the old string with the new string.
        readPos = 0;
        writePos = 0;
        while (readPos<thisLength)
        {
            if (replaceCount<replaceHolder&&readPos==replaceIndex[replaceCount])
            {
              replaceCount++;
              readPos+=(oldLength);
                  memcpyNoGCRefs(&retValBuffer[writePos], newBuffer, newLength*sizeof(WCHAR));
              writePos+=(newLength);
            }
            else
            {
              retValBuffer[writePos++] = thisBuffer[readPos++];
            }
         }
         retValBuffer[retValBuffLength]='\0';

         gc.retValString->SetStringLength(retValBuffLength);
    }
    else
    {
        gc.retValString = gc.thisRef;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.retValString);
}
FCIMPLEND


/*=============================InternalHasHighChars=============================
**Action:  Checks if the string can be sorted quickly.  The requirements are that
**         the string contain no character greater than 0x80 and that the string not
**         contain an apostrophe or a hypen.  Apostrophe and hyphen are excluded so that
**         words like co-op and coop sort together.
**Returns: Void.  The side effect is to set a bit on the string indicating whether or not
**         the string contains high chars.
**Arguments: The String to be checked.
**Exceptions: None
==============================================================================*/
DWORD COMString::InternalCheckHighChars(STRINGREF inString) {
    WRAPPER_CONTRACT;

    WCHAR *chars;
    WCHAR c;
    INT32 length;

    RefInterpretGetStringValuesDangerousForGC(inString, (WCHAR **) &chars, &length);

    DWORD stringState = STRING_STATE_FAST_OPS;

    for (int i=0; i<length; i++) {
        c = chars[i];
        if (c>=0x80) {
            inString->SetHighCharState(STRING_STATE_HIGH_CHARS);
            return STRING_STATE_HIGH_CHARS;
        } else if (HighCharHelper::IsHighChar((int)c)) {
            //This means that we have a character which forces special sorting,
            //but doesn't necessarily force slower casing and indexing.  We'll
            //set a value to remember this, but we need to check the rest of
            //the string because we may still find a charcter greater than 0x7f.
            stringState = STRING_STATE_SPECIAL_SORT;
        }
    }

    inString->SetHighCharState(stringState);
    return stringState;
}

#ifdef VERIFY_HEAP
/*=============================ValidateHighChars=============================
**Action:  Validate if the HighChars bits is set correctly, no side effect
**Returns: BOOL for result of validation
**Arguments: The String to be checked.
**Exceptions: None
==============================================================================*/
BOOL COMString::ValidateHighChars(StringObject * inString)
{
    DWORD curStringState = inString->GetHighCharState ();
    // state could always be undetermined
    if (curStringState == STRING_STATE_UNDETERMINED)
    {
        return TRUE;
    }

    WCHAR *chars;
    INT32 length;
    RefInterpretGetStringValuesDangerousForGC(inString, (WCHAR **) &chars, &length);

    DWORD stringState = STRING_STATE_FAST_OPS;
    for (int i=0; i<length; i++) {
        WCHAR c = chars[i];
        if (c>=0x80) 
        {
            // if there is a high char in the string, the state has to be STRING_STATE_HIGH_CHARS
            return curStringState == STRING_STATE_HIGH_CHARS;
        } 
        else if (HighCharHelper::IsHighChar((int)c)) {
            //This means that we have a character which forces special sorting,
            //but doesn't necessarily force slower casing and indexing.  We'll
            //set a value to remember this, but we need to check the rest of
            //the string because we may still find a charcter greater than 0x7f.
            stringState = STRING_STATE_SPECIAL_SORT;
        }
    }
    
    return stringState == curStringState;
}

#endif //VERIFY_HEAP

/*=============================TryConvertStringDataToUTF8=============================
**Action:   If the string has no high chars, converts the string into UTF8. If a
**          high char is found, just returns false. In either case, the high char state
**          on the stringref is set appropriately
**Returns:  bool. True - Success
            False - Caller has to use OS API
**Arguments:inString - String to be checked
**          outString - Caller allocated space where the result will be placed
**          outStrLen - Number of bytes allocated
==================================================================================*/
bool COMString::TryConvertStringDataToUTF8(STRINGREF inString, LPUTF8 outString, DWORD outStrLen){
    WRAPPER_CONTRACT;

    WCHAR   *buf = inString->GetBuffer();
    DWORD   strLen = inString->GetStringLength();
    bool    bSuccess = true;
    if (HAS_HIGH_CHARS(inString->GetHighCharState())) {
        return false;
    }

    bool    bNeedCheck = IS_STRING_STATE_UNDETERMINED(inString->GetHighCharState());
    // Should be at least strLen + 1
    _ASSERTE(outStrLen > strLen);

    if (outStrLen <= strLen)
        return false;

    // First try to do it yourself..if high char found, return false
    for (DWORD index = 0; index < strLen; index++){

        if (bNeedCheck && (buf[index] >= 0x80 || HighCharHelper::IsHighChar((int)buf[index]))){

            bSuccess = false;
            break;
        }
        outString[index] = (char)buf[index];
    }

    //The actual algorithm for setting the string state has gotten more compilcated and isn't
    //germane to this function, so if we don't get success, we'll simply bail and not set
    //the string state.
    if (bSuccess)
    {
        outString[strLen] = '\0';
        if(bNeedCheck)
        {
            // It only makes sense to set this if the string is undetermined
            inString->SetHighCharState(STRING_STATE_FAST_OPS);
        }
    }

    return bSuccess;
}


/*============================InternalTrailByteCheck============================
**Action: Many years ago, VB didn't have the concept of a byte array, so enterprising
**        users created one by allocating a BSTR with an odd length and using it to
**        store bytes.  A generation later, we're still stuck supporting this behavior.
**        The way that we do this is to take advantage of the difference between the
**        array length and the string length.  The string length will always be the
**        number of characters between the start of the string and the terminating 0.
**        If we need an odd number of bytes, we'll take one wchar after the terminating 0.
**        (e.g. at position StringLength+1).  The high-order byte of this wchar is
**        reserved for flags and the low-order byte is our odd byte.
**
**Returns: True if <CODE>str</CODE> contains a VB trail byte, false otherwise.
**Arguments: str -- The string to be examined.
**Exceptions: None
==============================================================================*/
BOOL COMString::HasTrailByte(STRINGREF str) {
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    _ASSERTE(str != NULL);
    
    //The difference between the arrayLength and the stringLength is normally 1 (the
    //terminating null).  If it's two or greater, we may have a trail byte, or we may
    //just have a string created from a StringBuilder.  If we find this difference,
    //we need to check the high byte of the first character after the terminating null.
    INT32 strLength = str->GetStringLength();
    if ((str->GetArrayLength() - strLength)>=2) {
        WCHAR *buffer = str->GetBuffer();
        if (MARKS_VB_BYTE_ARRAY(buffer[strLength+1])) {
            return TRUE;
        }
    }
    return FALSE;
}

/*=================================GetTrailByte=================================
**Action:  If <CODE>str</CODE> contains a vb trail byte, returns a copy of it.
**Returns: True if <CODE>str</CODE> contains a trail byte.  *bTrailByte is set to
**         the byte in question if <CODE>str</CODE> does have a trail byte, otherwise
**         it's set to 0.
**Arguments: str -- The string being examined.
**           bTrailByte -- An out param to hold the value of the trail byte.
**Exceptions: None.
==============================================================================*/
BOOL COMString::GetTrailByte(STRINGREF str, BYTE *bTrailByte) {
    WRAPPER_CONTRACT;
    _ASSERTE(bTrailByte);
    *bTrailByte=0;

    INT32 strLength = str->GetStringLength();    
    if ((str->GetArrayLength() - strLength)>=2) {
        WCHAR *buffer = str->GetBuffer();
        WCHAR trailWChar = buffer[strLength + 1];
        if (MARKS_VB_BYTE_ARRAY(trailWChar)) {
            *bTrailByte=GET_VB_TRAIL_BYTE(trailWChar);                
            return TRUE;
        }                
    }        

    return FALSE;
}

/*=================================SetTrailByte=================================
**Action: Sets the trail byte if <CODE>str</CODE> has enough room to contain one.
**Returns: True if the trail byte could be set, false otherwise.
**Arguments: str -- The string into which to set the trail byte.
**           bTrailByte -- The trail byte to be added to the string.
**Exceptions: None.
==============================================================================*/
BOOL COMString::SetTrailByte(STRINGREF str, BYTE bTrailByte) {
    WRAPPER_CONTRACT;
    _ASSERTE(str != NULL);

    INT32 strLength = str->GetStringLength();    
    if ((str->GetArrayLength() - strLength)>=2) {
        WCHAR *buffer = str->GetBuffer();
        buffer[strLength + 1] = MAKE_VB_TRAIL_BYTE(bTrailByte);

        return TRUE;
    }
    return FALSE;
}

