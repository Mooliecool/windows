/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    wchar.c

Abstract:

    Implementation of wide char string functions.

--*/


#include "pal/palinternal.h"
#include "pal/cruntime.h"
#include "pal/dbgmsg.h"
#include "pal/unicode_data.h"
#include "pal/thread.h"


#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_CFSTRING
#include <CoreFoundation/CoreFoundation.h>
#endif

#if HAVE_WCHAR_H
#include <wchar.h>
#endif
#if HAVE_WCTYPE_H
#include <wctype.h>
#endif
#include <ctype.h>
#include <pthread.h>
#include <limits.h>


SET_DEFAULT_DEBUG_CHANNEL(CRT);


/*--
Function:
  wtolower (internal)

16-bit wide character version of the ANSI tolower() function.

  --*/
static
wchar_16
wtolower(wchar_16 c)
{
    /* Note: Surrogate pairs unicode character are not supported */

#if HAVE_TOWLOWER

    wchar_t w;
    w = (wchar_t) c;
    w = towlower(w);
    return (wchar_16) w;

#else

    return PAL_towlower(c);

#endif

}

/*******************************************************************************
Function:
  Internal_i64tow

Parameters:
  value
    - INT64 value to be converted to a string
  string
    - out buffer to place interger string
  radix
    - numeric base to convert to
  isI64
    - TRUE if value is INT64, FALSE if value is a long

Note:
  - only a radix of ten (and value < 0) will result in a negative
    sign in the output buffer
*******************************************************************************/
LPWSTR Internal_i64tow(INT64 value, LPWSTR string, int radix, BOOL isI64)
{
    int length = 0;
    int n;
    int r;
    UINT64 uval = value;
    LPWSTR stringPtr = string;
    int start = 0;
    int end;
    WCHAR tempCh;

    if (radix < 2 || radix > 36)
    {
        ASSERT( "Invalid radix, radix must be between 2 and 36\n" );
        SetLastError(ERROR_INVALID_PARAMETER);
        return string;
    }
    if (FALSE == isI64)
    {
        uval = (ULONG) uval;
    }
    if (10 == radix && value < 0)
    {
        uval = value * -1;
    }
    if(0 == uval)
    {
        ++length;
        *stringPtr++ = '0';
    }
    else while (uval > 0)
    {
        ++length;
        n = uval / radix;
        r = uval - (n * radix);
        uval /= radix;
        if (r > 9)
        {
            *stringPtr++ = r + 87;
        }
        else
        {
            *stringPtr++ = r + 48;
        }
    }
    if (10 == radix && value < 0)
    {
        *stringPtr++ = '-';
        ++length;
    }
    *stringPtr = 0; /* end the string */

    /* reverse the string */
    end = length - 1;
    while (start < end)
    {
        tempCh = string[start];
        string[start] = string[end];
        string[end] = tempCh;
        ++start;
        --end;
    }

    return string;
}

/*--
Function:
  _itow

16-bit wide character version of the ANSI tolower() function.

  --*/
wchar_16 *
__cdecl
_itow(
    int value,
    wchar_16 *string,
    int radix)
{
    wchar_16 *ret;

    PERF_ENTRY(_itow);
    ENTRY("_itow (value=%d, string=%p, radix=%d)\n",
          value, string, radix);

    ret = Internal_i64tow(value, string, radix, FALSE);

    LOGEXIT("_itow returns wchar_t* %p\n", ret);
    PERF_EXIT(_itow);

    return ret;
}

/*--
Function:
  _i64tow

See MSDN doc
--*/
wchar_16 *
 __cdecl 
_i64tow(
    __int64 value, 
    wchar_16 *string, 
    int radix)
{
    wchar_16 *ret;

    PERF_ENTRY(_i64tow);
    ENTRY("_i64tow (value=%ld, string=%p, radix=%d)\n",
          value, string, radix);

    ret = Internal_i64tow(value, string, radix, TRUE);

    LOGEXIT("_i64tow returns wchar_t* %p\n", ret);
    PERF_EXIT(_i64tow);

    return ret;
}


/*--
Function:
  _wtoi

See MSDN doc
--*/
int
__cdecl
_wtoi(
    const wchar_16 *string)
{
    int len;
    int ret;
    char *tempStr;

    PERF_ENTRY(_wtoi);
    ENTRY("_wtoi (string=%p)\n", string);

    len = WideCharToMultiByte(CP_ACP, 0, string, -1, 0, 0, 0, 0);
    if (!len)
    {
        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
              GetLastError());
        return -1;
    }
    tempStr = (char *) malloc(len);
    if (!tempStr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return -1;
    }
    len = WideCharToMultiByte(CP_ACP, 0, string, -1, tempStr, len, 0, 0);
    if (!len)
    {
        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
              GetLastError());
        free(tempStr);
        return -1;
    }
    ret = atoi(tempStr);

    free(tempStr);
    LOGEXIT("_wtoi returns int %d\n", ret);
    PERF_EXIT(_wtoi);
    return ret;
}



/*--
Function:
  PAL_iswspace

See MSDN doc
--*/
int 
__cdecl 
PAL_iswspace(wchar_16 c)
{
    int ret;
#if HAVE_CFSTRING
    static CFCharacterSetRef sSpaceSet;
    
    if (sSpaceSet == NULL)
    {
        sSpaceSet = CFCharacterSetGetPredefined(kCFCharacterSetWhitespace);
    }
    PERF_ENTRY(iswspace);
    ENTRY("PAL_iswspace (c=%C)\n", c);
    if (c >= 0x2000 && c <= 0x200b)
    {
        // U+2000 through U+200b are space characters according to
        // Core Foundation, but not on Windows.
        ret = FALSE;
    }
    else if ((c >= 0x000a && c <= 0x000d) || c == 0x0085 || c == 0x1680)
    {
        // U+000A through U+000D, U+0085, and U+1680 are space
        // characters according to Windows, but not with Core Foundation.
        ret = TRUE;
    }
    else
    {
        ret = CFCharacterSetIsCharacterMember(sSpaceSet, c);
    }
#else   // HAVE_CFSTRING
    WORD Info;

    PERF_ENTRY(iswspace);
    ENTRY("PAL_iswspace (c=%C)\n", c);
    
    ret = GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, (WCHAR*)&c, 1, &Info);

    if (ret == FALSE)
    {
        ASSERT("GetStringTypeExW failed to get information for %#X!\n", c);
        return -1;
    }

    ret = (Info & C1_SPACE);
#endif  // HAVE_CFSTRING
    LOGEXIT("PAL_iswspace returns int %d\n", ret);
    PERF_EXIT(iswspace);
    return ret;
}

/*++
Function:
  _wcsnicmp

Compare characters of two strings without regard to case

Return Value

The return value indicates the relationship between the substrings as follows.

Return Value

Description

< 0        string1 substring less than string2 substring
  0        string1 substring identical to string2 substring
> 0        string1 substring greater than string2 substring

Parameters

string1, string2        Null-terminated strings to compare
count                   Number of characters to compare

Remarks

The _strnicmp function lexicographically compares, at most, the first
count characters of string1 and string2. The comparison is performed
without regard to case; _strnicmp is a case-insensitive version of
strncmp. The comparison ends if a terminating null character is
reached in either string before count characters are compared. If the
strings are equal when a terminating null character is reached in
either string before count characters are compared, the shorter string
is lesser.

--*/
int
__cdecl
_wcsnicmp(
          const wchar_16 *string1,
          const wchar_16 *string2, 
          size_t count)
{
    int i;
    int diff = 0;

    PERF_ENTRY(_wcsnicmp);
    ENTRY("_wcsnicmp (string1=%p (%S), string2=%p (%S), count=%lu)\n", 
          string1?string1:W16_NULLSTRING, 
          string1?string1:W16_NULLSTRING, string2?string2:W16_NULLSTRING, string2?string2:W16_NULLSTRING,
         (unsigned long) count);

    for (i = 0; i < count; i++)
    {
        diff = wtolower(string1[i]) - wtolower(string2[i]);
        if (diff != 0 || 0 == string1[i] || 0 == string2[i])
        {
            break;
        }
    }
    LOGEXIT("_wcsnicmp returning int %d\n", diff);
    PERF_EXIT(_wcsnicmp);
    return diff;
}

/*++
Function:
  _wcsicmp

Compare characters of two strings without regard to case

Return Value

The return value indicates the relationship between the substrings as follows.

Return Value

Description

< 0        string1 substring less than string2 substring
  0        string1 substring identical to string2 substring
> 0        string1 substring greater than string2 substring

Parameters

string1, string2        Null-terminated strings to compare

--*/
int
__cdecl
_wcsicmp(
          const wchar_16 *string1,
          const wchar_16 *string2)
{
    int ret;

    PERF_ENTRY(_wcsicmp);
    ENTRY("_wcsicmp (string1=%p (%S), string2=%p (%S))\n", 
          string1?string1:W16_NULLSTRING, 
          string1?string1:W16_NULLSTRING, string2?string2:W16_NULLSTRING, string2?string2:W16_NULLSTRING);

    ret = _wcsnicmp(string1, string2, 0x7fffffff);

    LOGEXIT("_wcsnicmp returns int %d\n", ret);
    PERF_EXIT(_wcsicmp);
    return ret;
}


/*++
Function:
  _wcslwr

Convert a string to lowercase.

Return Value

Returns a pointer to the converted string. Because the modification is
done in place, the pointer returned is the same as the pointer passed
as the input argument. No return value is reserved to indicate an
error.

Parameter

string   Null-terminated string to convert to lowercase

Remarks

--*/
wchar_16 * 
__cdecl
_wcslwr(
        wchar_16 *string)
{
    int i;

    PERF_ENTRY(_wcslwr);
    ENTRY("_wcslwr (string=%p (%S))\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING);

    for (i=0 ; string[i] != 0; i++)
    {
        string[i] = wtolower(string[i]);
    }
  
    LOGEXIT("_wcslwr returning wchar_t %p (%S)\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING);
    PERF_EXIT(_wcslwr);
    return string;
}


/*++
Function:
  PAL_wcstol

Convert string to a long-integer value.

Return Value

wcstol returns the value represented in the string nptr, except when
the representation would cause an overflow, in which case it returns
LONG_MAX or LONG_MIN. strtol returns 0 if no conversion can be
performed. errno is set to ERANGE if overflow or underflow occurs.

Parameters

nptr    Null-terminated string to convert 
endptr  Pointer to character that stops scan
base    Number base to use

Remarks

The wcstol function converts nptr to a long. It stops reading the
string nptr at the first character it cannot recognize as part of a
number. This may be the terminating null character, or it may be the
first numeric character greater than or equal to base.

Notes :
    MSDN states that only space and tab are accepted as leading whitespace, but
    tests indicate that other whitespace characters (newline, carriage return,
    etc) are also accepted. This matches the behavior on Unix systems.
    
    For wcstol and wcstoul, we need to check if the value to be returned 
    is outside the 32 bit range. If so, the returned value needs to be set  
    as appropriate, according to the MSDN pages for wcstol and wcstoul,
    and in all instances errno must be set to ERANGE (The one exception
    is converting a string representing a negative value to unsigned long).
    Note that on 64 bit Windows, long's are still 32 bit. Thus, to match
    Windows behavior, we must return long's in the 32 bit range.  
--*/

/*                                                                               
                           */
LONG
__cdecl
PAL_wcstol(
        const wchar_16 *nptr,
        wchar_16 **endptr,
        int base)
{
    char *s_nptr = 0;
    char *s_endptr = 0;
    long res;
    int size;
    DWORD dwLastError = 0;

    PERF_ENTRY(wcstol);
    ENTRY("wcstol (nptr=%p (%S), endptr=%p, base=%d)\n", nptr?nptr:W16_NULLSTRING, nptr?nptr:W16_NULLSTRING,
          endptr, base);

    size = WideCharToMultiByte(CP_ACP, 0, nptr, -1, NULL, 0, NULL, NULL);
    if (!size)
    {
        dwLastError = GetLastError();
        ASSERT("WideCharToMultiByte failed.  Error is %d\n", dwLastError);
        SetLastError(ERROR_INVALID_PARAMETER);
        res = 0;
        goto PAL_wcstolExit;
    }
    s_nptr = malloc(size);
    if (!s_nptr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        res = 0;
        goto PAL_wcstolExit;
    }
    size = WideCharToMultiByte(CP_ACP, 0, nptr, -1, s_nptr, size, NULL, NULL);
    if( size==0 )
    {
        dwLastError = GetLastError();
        ASSERT("WideCharToMultiByte failed.  Error is %d\n", dwLastError);
        SetLastError(ERROR_INVALID_PARAMETER);
        res = 0;
        goto PAL_wcstolExit;
    }

    res = strtol(s_nptr, &s_endptr, base);

    /* only ASCII characters will be accepted by strtol, and those always get
       mapped to single-byte characters, so the first rejected character will
       have the same index in the multibyte and widechar strings */
    if( endptr )
    {
        size = s_endptr - s_nptr;
        *endptr = (wchar_16 *)&nptr[size];
    }

PAL_wcstolExit:
    free(s_nptr);
    LOGEXIT("wcstol returning long %ld\n", res);
    PERF_EXIT(wcstol);
    /* This explicit cast to LONG is used to silence any potential warnings
    due to implicitly casting the native long res to LONG when returning. */
    return (LONG)res;
}


/*++
Function:
  PAL_wcstoul

Convert string to an unsigned long-integer value.

Return Value

wcstoul returns the converted value, if any, or ULONG_MAX on
overflow. It returns 0 if no conversion can be performed. errno is
set to ERANGE if overflow or underflow occurs.

Parameters

nptr    Null-terminated string to convert 
endptr  Pointer to character that stops scan
base    Number base to use

Remarks

wcstoul stops reading the string nptr at the first character it cannot
recognize as part of a number. This may be the terminating null
character, or it may be the first numeric character greater than or
equal to base. The LC_NUMERIC category setting of the current locale
determines recognition of the radix character in nptr; for more
information, see setlocale. If endptr is not NULL, a pointer to the
character that stopped the scan is stored at the location pointed to
by endptr. If no conversion can be performed (no valid digits were
found or an invalid base was specified), the value of nptr is stored
at the location pointed to by endptr.

Notes :
    MSDN states that only space and tab are accepted as leading whitespace, but
    tests indicate that other whitespace characters (newline, carriage return,
    etc) are also accepted. This matches the behavior on Unix systems.
    
    For wcstol and wcstoul, we need to check if the value to be returned 
    is outside the 32 bit range. If so, the returned value needs to be set
    as appropriate, according to the MSDN pages for wcstol and wcstoul,
    and in all instances errno must be set to ERANGE (The one exception
    is converting a string representing a negative value to unsigned long).
    Note that on 64 bit Windows, long's are still 32 bit. Thus, to match
    Windows behavior, we must return long's in the 32 bit range.
--*/

/*                                                                                
                                    */
ULONG
__cdecl
PAL_wcstoul(
        const wchar_16 *nptr,
        wchar_16 **endptr,
        int base)
{
    char *s_nptr = 0;
    char *s_endptr = 0;
    unsigned long res;
    int size;
    DWORD dwLastError = 0;

    PERF_ENTRY(wcstoul);
    ENTRY("wcstoul (nptr=%p (%S), endptr=%p, base=%d)\n", nptr?nptr:W16_NULLSTRING, nptr?nptr:W16_NULLSTRING,
          endptr, base);

    size = WideCharToMultiByte(CP_ACP, 0, nptr, -1, NULL, 0, NULL, NULL);
    if (!size)
    {
        dwLastError = GetLastError();
        ASSERT("WideCharToMultiByte failed.  Error is %d\n", dwLastError);
        SetLastError(ERROR_INVALID_PARAMETER);
        res = 0;
        goto PAL_wcstoulExit;
    }
    s_nptr = malloc(size);
    if (!s_nptr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        res = 0;
        goto PAL_wcstoulExit;
    }
    size = WideCharToMultiByte(CP_ACP, 0, nptr, -1, s_nptr, size, NULL, NULL);
    if (!size)
    {
        dwLastError = GetLastError();
        ASSERT("WideCharToMultiByte failed.  Error is %d\n", dwLastError);
        SetLastError(ERROR_INVALID_PARAMETER);
        res = 0;
        goto PAL_wcstoulExit;
    }

    res = strtoul(s_nptr, &s_endptr, base);

    /* only ASCII characters will be accepted by strtol, and those always get
       mapped to single-byte characters, so the first rejected character will
       have the same index in the multibyte and widechar strings */
    if( endptr )
    {
        size = s_endptr - s_nptr;
        *endptr = (wchar_16 *)&nptr[size];
    }

PAL_wcstoulExit:
    free(s_nptr);
    LOGEXIT("wcstoul returning unsigned long %lu\n", res);
    PERF_EXIT(wcstoul);
    
    /* When returning unsigned long res from this function, it will be 
    implicitly cast to ULONG. This handles situations where a string that
    represents a negative number is passed in to wcstoul. The Windows
    behavior is analogous to taking the binary equivalent of the negative
    value and treating it as a positive number. Returning a ULONG from
    this function, as opposed to native unsigned long, allows us to match
    this behavior. The explicit case to ULONG below is used to silence any
    potential warnings due to the implicit casting.  */    
    return (ULONG)res;
}


/*++
Function:
  PAL_towlower

See MSDN

--*/
wchar_16
__cdecl
PAL_towlower( wchar_16 c )
{
#if HAVE_CFSTRING
    PERF_ENTRY(towlower);
    ENTRY("towlower (c=%d)\n", c);
    if (!PAL_iswlower(c))
    {
        CFMutableStringRef cfString = CFStringCreateMutable(
                                            kCFAllocatorDefault, 1);
        if (cfString != NULL)
        {
            CFStringAppendCharacters(cfString, &c, 1);
            CFStringLowercase(cfString, NULL);
            c = CFStringGetCharacterAtIndex(cfString, 0);
            CFRelease(cfString);
        }
    }
    PERF_ENTRY(towlower);
    ENTRY("towlower returns int %d\n", c );
    return c;
#else   /* HAVE_CFSTRING */
    UnicodeDataRec dataRec;
    
    PERF_ENTRY(towlower);
    ENTRY("towlower (c=%d)\n", c);
    
    if (!GetUnicodeData(c, &dataRec))
    {
        TRACE( "Unable to retrive unicode data for the character %c.\n", c );
        LOGEXIT("towlower returns int %d\n", c );
        PERF_EXIT(towlower);
        return c;
    }

    if ( (dataRec.C1_TYPE_FLAGS & C1_LOWER) || (dataRec.nOpposingCase ==  0 ))
    {
        LOGEXIT("towlower returns int %d\n", c );
        PERF_EXIT(towlower);
        return c;
    }
    else
    {
        LOGEXIT("towlower returns int %d\n", dataRec.nOpposingCase );
        PERF_EXIT(towlower);
        return dataRec.nOpposingCase;
    }
#endif  /* HAVE_CFSTRING */
}


/*++
Function:
  PAL_towupper

See MSDN

--*/
wchar_16
__cdecl
PAL_towupper( wchar_16 c )
{
#if HAVE_CFSTRING
    PERF_ENTRY(towupper);
    ENTRY("towupper (c=%d)\n", c);
    if (!PAL_iswupper(c))
    {
        CFMutableStringRef cfString = CFStringCreateMutable(
                                            kCFAllocatorDefault, 1);
        if (cfString != NULL)
        {
            CFStringAppendCharacters(cfString, &c, 1);
            CFStringUppercase(cfString, NULL);
            c = CFStringGetCharacterAtIndex(cfString, 0);
            CFRelease(cfString);
        }
    }
    PERF_ENTRY(towupper);
    ENTRY("towupper returns int %d\n", c );
    return c;
#else   /* HAVE_CFSTRING */
    UnicodeDataRec dataRec;

    PERF_ENTRY(towupper);
    ENTRY("towupper (c=%d)\n", c);

    if (!GetUnicodeData(c, &dataRec))
    {
        TRACE( "Unable to retrive unicode data for the character %c.\n", c );
        LOGEXIT("towupper returns int %d\n", c );
        PERF_EXIT(towupper);
        return c;
    }
    
    if ( (dataRec.C1_TYPE_FLAGS & C1_UPPER) || (dataRec.nOpposingCase ==  0 ))
    {
        LOGEXIT("towupper returns int %d\n", c );
        PERF_EXIT(towupper);
        return c;
    }
    else
    {
        LOGEXIT("towupper returns int %d\n", dataRec.nOpposingCase );
        PERF_EXIT(towupper);
        return dataRec.nOpposingCase;
    }
#endif  /* HAVE_CFSTRING */
}

/*++
Function:
  PAL_iswupper

See MSDN

--*/
int
__cdecl
PAL_iswupper( wchar_16 c )
{
    BOOL bRetVal = FALSE;
#if HAVE_CFSTRING
    static CFCharacterSetRef sUppercaseSet;
    
    if (sUppercaseSet == NULL)
    {
        sUppercaseSet = CFCharacterSetGetPredefined(
                                        kCFCharacterSetUppercaseLetter);
    }
    PERF_ENTRY(iswupper);
    ENTRY( "iswupper (c=%d)\n", c );
    bRetVal = CFCharacterSetIsCharacterMember(sUppercaseSet, c);
#else   /* HAVE_CFSTRING */
    UnicodeDataRec dataRec;

    PERF_ENTRY(iswupper);
    ENTRY( "iswupper (c=%d)\n", c );

    if (!GetUnicodeData(c, &dataRec))
    {
        TRACE( "Unable to retrive unicode data for the character %c.\n", c );
        goto exit;
    }
    
    if (dataRec.C1_TYPE_FLAGS & C1_UPPER)
    {
        bRetVal = TRUE;
    }
exit:
#endif  /* HAVE_CFSTRING */
    LOGEXIT( "iswupper returns %s.\n", bRetVal == TRUE ? "TRUE" : "FALSE" );
    PERF_EXIT(iswupper);
    return bRetVal;
}

/*++
Function:
  PAL_iswlower

See MSDN

--*/
int
__cdecl
PAL_iswlower( wchar_16 c )
{
    BOOL bRetVal = FALSE;
#if HAVE_CFSTRING
    static CFCharacterSetRef sLowercaseSet;
    
    if (sLowercaseSet == NULL)
    {
        sLowercaseSet = CFCharacterSetGetPredefined(
                                        kCFCharacterSetLowercaseLetter);
    }
    PERF_ENTRY(iswlower);
    ENTRY( "iswlower (c=%d)\n", c );
    bRetVal = CFCharacterSetIsCharacterMember(sLowercaseSet, c);
#else   /* HAVE_CFSTRING */
    UnicodeDataRec dataRec;

    PERF_ENTRY(iswlower);
    ENTRY("iswlower (c=%d)\n", c);
    
    if (!GetUnicodeData(c, &dataRec))
    {
        TRACE( "Unable to retrive unicode data for the character %c.\n", c );
        goto exit;
    }
    
    if (dataRec.C1_TYPE_FLAGS & C1_LOWER)
    {
        bRetVal = TRUE;
    }
exit:
#endif  /* HAVE_CFSTRING */
    LOGEXIT( "iswlower returns %s.\n", bRetVal == TRUE ? "TRUE" : "FALSE" );
    PERF_EXIT(iswlower);
    return bRetVal;
}

/*++
Function:
  PAL_iswalpha

See MSDN

--*/
int
__cdecl
PAL_iswalpha( wchar_16 c )
{
    PERF_ENTRY(iswalpha);
    ENTRY( "PAL_iswalpha (c=%d)\n", c);
    
    if ( PAL_iswupper( c ) || PAL_iswlower( c ) )
    {
        LOGEXIT( "PAL_iswalpha returns 1.\n" );
        PERF_EXIT(iswalpha);
        return 1;
    }

    LOGEXIT( "PAL_iswalpha returns 0.\n" );
    PERF_EXIT(iswalpha);
    return 0;
}


/*++
Function:
  PAL_wcscat

See MSDN or the man page for mcscat.

--*/
wchar_16 * 
__cdecl
PAL_wcscat(
        wchar_16 *strDestination, 
        const wchar_16 *strSource)
{
    wchar_16 *ret;
    PERF_ENTRY(wcscat);
    ENTRY("wcscat (strDestination=%p (%S), strSource=%p (%S))\n", 
          strDestination?strDestination:W16_NULLSTRING, 
          strDestination?strDestination:W16_NULLSTRING, strSource?strSource:W16_NULLSTRING, strSource?strSource:W16_NULLSTRING);

    ret = PAL_wcsncat( strDestination, strSource, PAL_wcslen( strSource ) );
    
    LOGEXIT("wcscat returnng wchar_t %p (%S)\n", ret, ret);
    PERF_EXIT(wcscat);
    return ret;
}


/*++
Function:
  PAL_wcscpy

See MSDN or the man page for mcscpy.

--*/
wchar_16 * 
__cdecl
PAL_wcscpy(
        wchar_16 *strDestination, 
        const wchar_16 *strSource)
{
    wchar_16 *start = strDestination;

    PERF_ENTRY(wcscpy);
    ENTRY("wcscpy (strDestination=%p, strSource=%p (%S))\n", 
          strDestination, strSource ? strSource:W16_NULLSTRING, strSource ? strSource:W16_NULLSTRING);

    if (strDestination == NULL)
    {
        ERROR("invalid strDestination argument\n");
        LOGEXIT("wcscpy returning wchar_t NULL\n");
        PERF_EXIT(wcscpy);
        return NULL;
    }

    if (strSource == NULL)
    {
        ERROR("invalid strSource argument\n");
        LOGEXIT("wcscpy returning wchar_t NULL\n");
        PERF_EXIT(wcscpy);
        return NULL;
    }

    /* copy source string to destination string */
    while(*strSource)
    {
        *strDestination++ = *strSource++;
    }

    /* add terminating null */
    *strDestination = '\0';

    LOGEXIT("wcscpy returning wchar_t %p (%S)\n", start, start);
    PERF_EXIT(wcscpy);
    return start;
}


/*++
Function:
  PAL_wcslen

See MSDN or the man page for wcslen.

--*/
size_t 
__cdecl
PAL_wcslen(
        const wchar_16 *string)
{
    size_t nChar = 0;

    PERF_ENTRY(wcslen);
    ENTRY("wcslen (string=%p (%S))\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING);
    
    if ( !string )
    {
        LOGEXIT("wcslen returning size_t %u\n", 0);
        PERF_EXIT(wcslen);
        return 0;
    }
    while (*string++)
    {
        nChar++;
    }

    LOGEXIT("wcslen returning size_t %u\n", nChar);
    PERF_EXIT(wcslen);
    return nChar;
}


/*++
Function:
  PAL_wcsncmp

See MSDN or the man page for wcsncmp.
--*/
int 
__cdecl
PAL_wcsncmp(
        const wchar_16 *string1,
        const wchar_16 *string2,
        size_t count)
{
    int i;
    int diff = 0;

    PERF_ENTRY(wcsncmp);
    ENTRY("wcsncmp (string1=%p (%S), string2=%p (%S) count=%lu)\n", 
          string1?string1:W16_NULLSTRING, 
          string1?string1:W16_NULLSTRING, string2?string2:W16_NULLSTRING, string2?string2:W16_NULLSTRING, 
          (unsigned long) count);

    for (i = 0; i < count; i++)
    {
        diff = string1[i] - string2[i];
        if (diff != 0)
        {
            break;
        }

        /* stop if we reach the end of the string */
        if(string1[i]==0)
        {
            break;
        }
    }
    LOGEXIT("wcsncmp returning int %d\n", diff);
    PERF_EXIT(wcsncmp);
    return diff;
}

/*++
Function:
  PAL_wcscmp

See MSDN or the man page for wcscmp.
--*/
int
__cdecl
PAL_wcscmp(
        const wchar_16 *string1,
        const wchar_16 *string2)
{
    int ret;

    PERF_ENTRY(wcscmp);
    ENTRY("wcscmp (string1=%p (%S), string2=%p (%S))\n",
          string1?string1:W16_NULLSTRING,
          string1?string1:W16_NULLSTRING, string2?string2:W16_NULLSTRING, string2?string2:W16_NULLSTRING);

    ret = PAL_wcsncmp(string1, string2, 0x7fffffff);

    LOGEXIT("wcscmp returns int %d\n", ret);
    PERF_EXIT(wcscmp);
    return ret;
}

/*++
Function:
  PAL_wcschr

See MSDN or man page for wcschr.

--*/
wchar_16 _WConst_return * 
__cdecl
PAL_wcschr(
        const wchar_16 * string, 
        wchar_16 c)
{
    PERF_ENTRY(wcschr);
    ENTRY("wcschr (string=%p (%S), c=%C)\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING, c);
    
    while (*string)
    {
        if (*string == c)
        {
            LOGEXIT("wcschr returning wchar_t %p (%S)\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING);
            PERF_EXIT(wcschr);
            return (wchar_16 *) string;
        }
        string++;
    }

    // Check if the comparand was \000
    if (*string == c)
        return (wchar_16 *) string;
    
    LOGEXIT("wcschr returning wchar_t NULL\n");
    PERF_EXIT(wcschr);
    return NULL;
}


/*++
Function:
  PAL_wcsrchr

See MSDN or man page for wcsrchr.

--*/
wchar_16 _WConst_return * 
__cdecl
PAL_wcsrchr(
        const wchar_16 * string, 
        wchar_16 c)
{
    wchar_16 *last = NULL;

    PERF_ENTRY(wcsrchr);
    ENTRY("wcsrchr (string=%p (%S), c=%C)\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING, c);

    while (*string)
    {
        if (*string == c)
        {
            last = (wchar_16 *) string;
        }
        string++;
    }
    
    LOGEXIT("wcsrchr returning wchar_t %p (%S)\n", last?last:W16_NULLSTRING, last?last:W16_NULLSTRING);
    PERF_EXIT(wcsrchr);
    return (wchar_16 *)last;
}


/*++
Function:
  PAL_wcsspn

See MSDN or man page for wcspbrk.
--*/
size_t
__cdecl
PAL_wcsspn (const wchar_16 *string, const wchar_16 *stringCharSet)
{
    ASSERT(0);
    return 0;
}


/*++
Function:
  PAL_wcspbrk

See MSDN or man page for wcspbrk.
--*/
wchar_16 * 
__cdecl
PAL_wcspbrk(
        const wchar_16 *string, 
        const wchar_16 *strCharSet)
{
    PERF_ENTRY(wcspbrk);
    ENTRY("wcspbrk (string=%p (%S), strCharSet=%p (%S))\n",
          string?string:W16_NULLSTRING,
          string?string:W16_NULLSTRING, strCharSet?strCharSet:W16_NULLSTRING, strCharSet?strCharSet:W16_NULLSTRING);

    while (*string)
    {
        if (PAL_wcschr(strCharSet, *string) != NULL)
        {
            LOGEXIT("wcspbrk returning wchar_t %p (%S)\n", string?string:W16_NULLSTRING, string?string:W16_NULLSTRING);
            PERF_EXIT(wcspbrk);
            return (wchar_16 *) string;
        }

        string++;
    }

    LOGEXIT("wcspbrk returning wchar_t NULL\n");
    PERF_EXIT(wcspbrk);
    return NULL;
}


/*++
Function:
  PAL_wcsstr

See MSDN or man page for wcsstr.
--*/
wchar_16 *
__cdecl
PAL_wcsstr(
        const wchar_16 *string, 
        const wchar_16 *strCharSet)
{
    wchar_16 *ret = NULL;
    int i;

    PERF_ENTRY(wcsstr);
    ENTRY("wcsstr (string=%p (%S), strCharSet=%p (%S))\n", 
      string?string:W16_NULLSTRING, 
      string?string:W16_NULLSTRING, strCharSet?strCharSet:W16_NULLSTRING, strCharSet?strCharSet:W16_NULLSTRING);
  
    if (string == NULL)
    {
        ret = NULL;
        goto leave;
    }
  
    if (strCharSet == NULL)
    {
        ret = NULL;
        goto leave;
    }
  
    if (*strCharSet == 0)
    {
        ret = (wchar_16 *)string;
        goto leave;
    } 
  
    while (*string != 0)
    {
        i = 0;
        while (1)
        {
            if (*(string + i) == 0 || *(strCharSet + i) == 0)
            {
                ret = (wchar_16 *) string;
                goto leave;
            }
            if (*(string + i) != *(strCharSet + i))
            {
                break;
            }
        i++;
        }
        string++;
    }
  
 leave:    
    LOGEXIT("wcsstr returning wchar_t %p (%S)\n", ret?ret:W16_NULLSTRING, ret?ret:W16_NULLSTRING);
    PERF_EXIT(wcsstr);
    return ret;
}

/*++
Function : 
    
    PAL_wcsncpy            
    
see msdn doc.
--*/
wchar_16 * 
__cdecl
PAL_wcsncpy( wchar_16 * strDest, const wchar_16 *strSource, size_t count )
{
    UINT length = sizeof( wchar_16 ) * count;    
    PERF_ENTRY(wcsncpy);
    ENTRY("wcsncpy( strDest:%p, strSource:%p (%S), count:%lu)\n", 
          strDest, strSource, strSource, (unsigned long) count);
    
    memset( strDest, 0, length );
    length = min( count, PAL_wcslen( strSource ) ) * sizeof( wchar_16 );
    memcpy( strDest, strSource, length );
    
    LOGEXIT("wcsncpy returning (wchar_16*): %p\n", strDest);
    PERF_EXIT(wcsncpy);
    return strDest;
}

/*++
Function : 
    
    wcsncat            
    
see msdn doc.
--*/
wchar_16 * 
__cdecl
PAL_wcsncat( wchar_16 * strDest, const wchar_16 *strSource, size_t count )
{
    wchar_16 *start = strDest;
    UINT LoopCount = 0;
    UINT StrSourceLength = 0;

    PERF_ENTRY(wcsncat);
    ENTRY( "wcsncat (strDestination=%p (%S), strSource=%p (%S), count=%lu )\n", 
            strDest ? strDest : W16_NULLSTRING, 
            strDest ? strDest : W16_NULLSTRING,
            strSource ? strSource : W16_NULLSTRING,
            strSource ? strSource : W16_NULLSTRING, (unsigned long) count);

    if ( strDest == NULL )
    {
        ERROR("invalid strDest argument\n");
        LOGEXIT("wcsncat returning wchar_t NULL\n");
        PERF_EXIT(wcsncat);
        return NULL;
    }

    if ( strSource == NULL )
    {
        ERROR("invalid strSource argument\n");
        LOGEXIT("wcsncat returning wchar_t NULL\n");
        PERF_EXIT(wcsncat);
        return NULL;
    }

    /* find end of source string */
    while ( *strDest )
    {
        strDest++;
    }

    StrSourceLength = PAL_wcslen( strSource ); 
    if ( StrSourceLength < count )
    {
        count = StrSourceLength;
    }
    
    /* concatenate new string */
    while( *strSource && LoopCount < count )
    {
      *strDest++ = *strSource++;
      LoopCount++;
    }

    /* add terminating null */
    *strDest = '\0';

    LOGEXIT("wcsncat returning wchar_t %p (%S)\n", start, start);
    PERF_EXIT(wcsncat);
    return start;
}

static BOOL MISC_CRT_WCSTOD_IsValidCharacter( WCHAR c )
{
    if ( c == '+' || c == '-' || c == '.' || ( c >= '0' && c <= '9' ) || 
         c == 'e' || c == 'E' || c == 'd' || c == 'd' )
    {
        return TRUE;
    }
    return FALSE;
}                                                               

/*++
Function : 
    
    wcstod

    There is a slight difference between the Windows version of wcstod
    and the BSD versio of wcstod.

    Under Windows the string "  -1b  " returns -1.000000 stop char = 'b'
    Under BSD the same string returns 0.000000 stop ' '

see msdn doc.
--*/
double
__cdecl
PAL_wcstod( const wchar_16 * nptr, wchar_16 **endptr )
{
    double RetVal = 0.0;
    LPSTR  lpStringRep = NULL;
    LPWSTR lpStartOfExpression = (LPWSTR)nptr;
    LPWSTR lpEndOfExpression = NULL;
    UINT Length = 0;

    PERF_ENTRY(wcstod);
    ENTRY( "wcstod( %p (%S), %p (%S) )\n", nptr, nptr, endptr , endptr );

    if ( !nptr )
    {
        ERROR( "nptr is invalid.\n" );
        LOGEXIT( "wcstod returning 0.0\n" );
        PERF_EXIT(wcstod);
        return 0.0;
    }

    /* Eat white space. */
    while ( PAL_iswspace( *lpStartOfExpression ) )
    {
        lpStartOfExpression++;
    }

    /* Get the end of the expression. */
    lpEndOfExpression = lpStartOfExpression;
    while ( *lpEndOfExpression )
    {
        if ( !MISC_CRT_WCSTOD_IsValidCharacter( *lpEndOfExpression ) )
        {
            break;
        }
        lpEndOfExpression++;
    }

    if ( lpEndOfExpression != lpStartOfExpression )
    {
        Length = lpEndOfExpression - lpStartOfExpression;
        lpStringRep = (LPSTR)malloc( Length + 1);

        if ( lpStringRep )
        {
            if ( WideCharToMultiByte( CP_ACP, 0, lpStartOfExpression, Length,
                                      lpStringRep, Length + 1 ,
                                      NULL, 0 ) != 0 )
            {
                LPSTR ScanStop = NULL;
                lpStringRep[Length]= 0;
                RetVal = strtod( lpStringRep, &ScanStop );

                /* See if strtod failed. */
                if ( RetVal == 0.0 && ScanStop == lpStringRep )
                {
                    ASSERT( "An error occured in the conversion.\n" );
                    lpEndOfExpression = (LPWSTR)nptr;
                }
            }
            else
            {
                ASSERT( "Wide char to multibyte conversion failed.\n" );
                lpEndOfExpression = (LPWSTR)nptr;
            }
        }
        else
        {
            ERROR( "Not enough memory.\n" );
            lpEndOfExpression = (LPWSTR)nptr;
        }
    }
    else
    {
        ERROR( "Malformed expression.\n" );
        lpEndOfExpression = (LPWSTR)nptr;
    }

    /* Set the stop scan character. */
    if ( endptr != NULL )
    {
        *endptr = lpEndOfExpression;
    }

    free( lpStringRep );
    LOGEXIT( "wcstod returning %f.\n", RetVal );
    PERF_EXIT(wcstod);
    return RetVal;
}

/*++
Function :

    _ui64tow

See MSDN for more details.
--*/
wchar_16 *
__cdecl
_ui64tow( unsigned __int64 value , wchar_16 * string , int radix )
{
    UINT ReversedIndex = 0;
    WCHAR ReversedString[ 65 ];
    LPWSTR lpString = string;
    UINT Index = 0;

    PERF_ENTRY(_ui64tow);
    ENTRY( "_ui64tow( value=%I64d, string=%p (%S), radix=%d )\n", 
           value, string, string, radix );

    if ( !string )
    {
        ERROR( "string has to be a valid pointer.\n" );
        LOGEXIT( "_ui64tow returning NULL.\n" );
        PERF_EXIT(_ui64tow);
        return NULL;
    }
    if ( radix < 2 || radix > 36 )
    {
        ERROR( "radix has to be between 2 and 36.\n" );
        LOGEXIT( "_ui64tow returning NULL.\n" );
        PERF_EXIT(_ui64tow);
        return NULL;
    }

    if(0 == value)
    {
        ReversedString[0] = '0';
        Index++;
    }
    else while ( value )
    {
        int temp = value % radix;
        value /= radix;
        
        if ( temp < 10 )
        {
            ReversedString[ Index ] = temp + '0';
            Index++;
        }
        else
        {
            ReversedString[ Index ] = temp - 10 + 'a';
            Index++;
        }
    }
    
    /* Reverse the string. */
    ReversedIndex = Index;
    for ( Index = 0; ReversedIndex > 0; ReversedIndex--, Index++ )
    {
        string[ Index ] = ReversedString[ ReversedIndex - 1 ];
    }

    string[ Index ] = '\0';
    LOGEXIT( "_ui64tow returning %p (%S).\n", lpString , lpString );
    PERF_EXIT(_ui64tow);
    return lpString;
}


/*++
Function:

    iswdigit
    
See MSDN for more details.
--*/
int
__cdecl
PAL_iswdigit( wchar_16 c )
{
    UINT nRetVal = 0;
#if HAVE_CFSTRING
    static CFCharacterSetRef sDigitSet;
    
    if (sDigitSet == NULL)
    {
        sDigitSet = CFCharacterSetGetPredefined(
                                        kCFCharacterSetDecimalDigit);
    }
    PERF_ENTRY(iswdigit);
    ENTRY( "iswdigit (c=%d)\n", c );
    nRetVal = CFCharacterSetIsCharacterMember(sDigitSet, c);
#else   /* HAVE_CFSTRING */
    UnicodeDataRec dataRec;

    PERF_ENTRY(iswdigit);
    ENTRY( "iswdigit (c=%d)\n", c );
    
    if (GetUnicodeData(c, &dataRec))
    {
        if (dataRec.C1_TYPE_FLAGS & C1_DIGIT)
        {
            nRetVal = 1; 
        }
        else
        {
            nRetVal = 0;
        }
    }
    else
    {
        TRACE( "No corresonding unicode record for character %d.\n", c );
    }
#endif  /* HAVE_CFSTRING */
    LOGEXIT( "iswdigit returning %d\n", nRetVal );
    PERF_EXIT(iswdigit);
    return nRetVal;
}

/*++
Function:

    iswxdigit
    
See MSDN for more details.

Notes :
the information in UnicodeData doesn't help us, it doesn't have enough 
granularity. Results in windows show that only ASCII and "Fullwidth" (>0xFF10)
numbers and letters are considered as "hex"; other "numbers" 
(nGeneralCategory==8) aren't.
--*/
int
__cdecl
PAL_iswxdigit( wchar_16 c )
{
    UINT nRetVal = 0;

    PERF_ENTRY(iswxdigit);
    ENTRY( "iswxdigit( c=%d )\n", c );
    
    /* ASCII characters */
    if((c>= 'A' && c<='F') ||        /* uppercase hex letters */
       (c>= 'a' && c<='f') ||        /* lowercase hex letters */
       (c>= '0' && c<='9'))          /* digits */
    {
        nRetVal = 1;
    }
    else
    /* "fullwidth" characters, whatever that is */
    if((c>= 0xFF10 && c<=0xFF19) ||  /* digits */
       (c>= 0xFF21 && c<=0xFF26) ||  /* uppercase hex letters */
       (c>= 0xFF41 && c<=0xFF46))    /* lowercase hex letters */
    {
        nRetVal = 1;
    }
    else
    {
        nRetVal = 0;
    }               
    LOGEXIT( "iswxdigit returning %d\n", nRetVal );
    PERF_EXIT(iswxdigit);
    return nRetVal;
}

/*++
Function:

    iswprint
     
See MSDN for more details.
--*/
int
__cdecl
PAL_iswprint( wchar_16 c ) 
{
    WORD CharType;
    int ret;
    

    PERF_ENTRY(iswprint);
    ENTRY ("iswprint (%#X)\n", c);

    ret = GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, (WCHAR*)&c, 1, &CharType);
    if (!ret)
    {
        ASSERT("GetStringTypeExW failed to get information for %#X!\n", c);
        ret = 0;
    }
    else
    {
        if (CharType & (C1_BLANK|C1_PUNCT|C1_ALPHA|C1_DIGIT))
        {
            /* Character is printable */
            ret = 1;
        }
        else
        {
            /* Character is not printable */
            ret = 0;
        }
    }

    LOGEXIT ("iswprint returns %d\n", ret);
    PERF_EXIT(iswprint);
    return (ret);
}


/*++
Function:
   PAL_wcstok

Finds the next token in a wide character string.

Return value:

A pointer to the next token found in strToken.  Returns NULL when no more 
tokens are found.  Each call modifies strToken by substituting a NULL 
character for each delimiter that is encountered.

Parameters:
strToken        String containing token(s)
strDelimit      Set of delimiter characters

--*/
wchar_16 *
__cdecl
PAL_wcstok(wchar_16 *strToken, const wchar_16 *strDelimit)
{
    THREAD* lpThread = NULL;
    wchar_16 *retval = NULL;
    wchar_16 *delim_ptr;
    wchar_16 *next_context;     /* string to save in TLS for future calls */

    PERF_ENTRY(wcstok);
    ENTRY("wcstok (strToken=%p (%S), strDelimit=%p (%S))\n",
          strToken?strToken:W16_NULLSTRING,
          strToken?strToken:W16_NULLSTRING, 
          strDelimit?strDelimit:W16_NULLSTRING, 
          strDelimit?strDelimit:W16_NULLSTRING);

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }
    
    if(NULL == strDelimit)
    {
        ERROR("delimiter string is NULL\n");
        goto done;
    }

    /* get token string from TLS if none is provided */
    if(NULL == strToken)
    {
        TRACE("wcstok() called with NULL string, using previous string\n");
        strToken = lpThread->wcstokContext;
        if(NULL == strToken)
        {            
            ERROR("wcstok called with NULL string without a previous call\n");
            goto done;
        }
    }
    
    /* first, skip all leading delimiters */
    while ((*strToken != '\0') && (PAL_wcschr(strDelimit,*strToken)))
    {
        strToken++;
    }

    /* if there were only delimiters, there's no string */
    if('\0' == strToken[0])
    {
        TRACE("end of string already reached, returning NULL\n");
        goto done;
    }

    /* we're now at the beginning of the token; look for the first delimiter */
    delim_ptr = PAL_wcspbrk(strToken,strDelimit);
    if(NULL == delim_ptr)
    {
        TRACE("no delimiters found, this is the last token\n");
        /* place the next context at the end of the string, so that subsequent 
           calls will return NULL */
        next_context = strToken+PAL_wcslen(strToken);
        retval = strToken;
    }
    else
    {
        /* null-terminate current token */
        *delim_ptr=0;

        /* place the next context right after the delimiter */
        next_context = delim_ptr+1;
        retval = strToken;
        
        TRACE("found delimiter; next token will be %p\n",next_context);
    }

    lpThread->wcstokContext = next_context;

done:
    LOGEXIT("wcstok() returns %p (%S)\n", retval?retval:W16_NULLSTRING, retval?retval:W16_NULLSTRING);
    PERF_EXIT(wcstok);
    return(retval);
}

/*++
Function:
   PAL_wcscspn

Finds the number of consecutive characters from the start of the string
that are not in the set.

Return value:

The number of characters from the start of the string that are not in
the set.

Parameters:
string          String
strCharSet      Set of delimiter characters

--*/
size_t
__cdecl
PAL_wcscspn(const wchar_16 *string, const wchar_16 *strCharSet)
{
    const wchar_16 *temp;
    size_t count = 0;

    PERF_ENTRY(wcscspn);
    
    while(*string != 0)
    {
        for(temp = strCharSet; *temp != 0; temp++)
        {
            if (*string == *temp)
            {
                PERF_EXIT(wcscspn);
                return count;
            }
        }
        count++;
        string++;
    }
    PERF_EXIT(wcscspn);
    return count;
}

#if HAVE_CFSTRING
/*--
Function:
  PAL_iswblank

Returns TRUE if c is a Win32 "blank" character.
--*/
int 
__cdecl 
PAL_iswblank(wchar_16 c)
{
    int ret;
    static CFCharacterSetRef sSpaceAndNewlineSet;
    
    if (sSpaceAndNewlineSet == NULL)
    {
        sSpaceAndNewlineSet = CFCharacterSetGetPredefined(
                                            kCFCharacterSetWhitespaceAndNewline);
    }
    switch (c)
    {
        case 0x0085:
        case 0x1680:
        case 0x202f:
        case 0xfeff:
            // These are blank characters on Windows, but are not part
            // of the SpaceAndNewline character set in Core Foundation.
            ret = TRUE;
            break;
        case 0x2028:
        case 0x2029:
            // These are not blank characters on Windows, but are part
            // of the SpaceAndNewline character set in Core Foundation.
            ret = FALSE;
            break;
        default:
            ret = CFCharacterSetIsCharacterMember(sSpaceAndNewlineSet, c);
            break;
    }
    return ret;
}

/*--
Function:
  PAL_iswcntrl

Returns TRUE if c is a control character.
--*/
int 
__cdecl 
PAL_iswcntrl(wchar_16 c)
{
    int ret;
    static CFCharacterSetRef sControlSet;
    
    if (sControlSet == NULL)
    {
        sControlSet = CFCharacterSetGetPredefined(kCFCharacterSetControl);
    }
    ret = CFCharacterSetIsCharacterMember(sControlSet, c);
    return ret;
}
#endif  // HAVE_CFSTRING
