/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    string.c

Abstract:

    Implementation of the string functions in the C runtime library that are Windows specific.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/cruntime.h"
#include "pal/thread.h"
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>

SET_DEFAULT_DEBUG_CHANNEL(CRT);

/*++
Function:
  _strnicmp

compare at most count characters from two strings, ignoring case

The strnicmp() function compares, with case insensitivity, at most count
characters from s1 to s2. All uppercase characters from s1 and s2 are 
mapped to lowercase for the purposes of doing the comparison.

Returns:

Value Meaning

< 0   s1 is less than s2 
0     s1 is equal to s2 
> 0   s1 is greater than s2

--*/
int 
__cdecl
_strnicmp( const char *s1, const char *s2, size_t count )
{
    int ret;

    PERF_ENTRY(_strnicmp);
    ENTRY("_strnicmp (s1=%p (%s), s2=%p (%s), count=%d)\n", s1?s1:"NULL", s1?s1:"NULL", s2?s2:"NULL", s2?s2:"NULL", count);

    ret = strncasecmp(s1, s2, count );

    LOGEXIT("_strnicmp returning int %d\n", ret);
    PERF_EXIT(_strnicmp);
    return ret;
}

/*++
Function:
  _stricmp

compare two strings, ignoring case

The stricmp() function compares, with case insensitivity, the string
pointed to by s1 to the string pointed to by s2. All uppercase
characters from s1 and s2 are mapped to lowercase for the purposes of
doing the comparison.

Returns:

Value Meaning

< 0   s1 is less than s2 
0     s1 is equal to s2 
> 0   s1 is greater than s2

--*/
int 
__cdecl
_stricmp(
         const char *s1, 
         const char *s2)
{
    int ret;

    PERF_ENTRY(_stricmp);
    ENTRY("_stricmp (s1=%p (%s), s2=%p (%s))\n", s1?s1:"NULL", s1?s1:"NULL", s2?s2:"NULL", s2?s2:"NULL");

    ret = strcasecmp(s1, s2);

    LOGEXIT("_stricmp returning int %d\n", ret);
    PERF_EXIT(_stricmp);
    return ret;
}


/*++
Function:
  _strlwr

Convert a string to lowercase.


This function returns a pointer to the converted string. Because the
modification is done in place, the pointer returned is the same as the
pointer passed as the input argument. No return value is reserved to
indicate an error.

Parameter

string  Null-terminated string to convert to lowercase

Remarks

The _strlwr function converts any uppercase letters in string to
lowercase as determined by the LC_CTYPE category setting of the
current locale. Other characters are not affected. For more
information on LC_CTYPE, see setlocale.

--*/
char *  
__cdecl
_strlwr(
        char *str)
{
    char *orig = str;

    PERF_ENTRY(_strlwr);
    ENTRY("_strlwr (str=%p (%s))\n", str?str:"NULL", str?str:"NULL");

    while (*str)
    {
        *str = tolower(*str);
        str++;
    } 
   
    LOGEXIT("_strlwr returning char* %p (%s)\n", orig?orig:"NULL", orig?orig:"NULL");
    PERF_EXIT(_strlwr);
    return orig;
}


/*++
Function:
  _swab

Swaps bytes.

Return Value

None

Parameters

src        Data to be copied and swapped
dest       Storage location for swapped data
n          Number of bytes to be copied and swapped

Remarks

The _swab function copies n bytes from src, swaps each pair of
adjacent bytes, and stores the result at dest. The integer n should be
an even number to allow for swapping. _swab is typically used to
prepare binary data for transfer to a machine that uses a different
byte order.

Example

char from[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char   to[] = "..........................";

printf("Before:\n%s\n%s\n\n", from, to);
_swab(from, to, strlen(from));
printf("After:\n%s\n%s\n\n", from, to);

Before:
ABCDEFGHIJKLMNOPQRSTUVWXYZ
..........................

After:
ABCDEFGHIJKLMNOPQRSTUVWXYZ
BADCFEHGJILKNMPORQTSVUXWZY

--*/
void
__cdecl
_swab(char *src, char *dest, int n)
{
    PERF_ENTRY(_swab);
    ENTRY("_swab (src=%p (%s), dest=%p (%s), n=%d)\n", src?src:"NULL", src?src:"NULL", dest?dest:"NULL", dest?dest:"NULL", n);
    swab(src, dest, n);
    LOGEXIT("_swab returning\n");
    PERF_EXIT(_swab);
}


/*++
Function:
   PAL_strtok

Finds the next token in a string.

Return value:

A pointer to the next token found in strToken.  Returns NULL when no more 
tokens are found.  Each call modifies strToken by substituting a NULL 
character for each delimiter that is encountered.

Parameters:
strToken        String cotaining token(s)
strDelimit      Set of delimiter characters

Remarks:
In FreeBSD, strtok is not re-entrant, strtok_r is.  It manages re-entrancy 
by using a passed-in context pointer (which will be stored in thread local
storage)  According to the strtok MSDN documentation, "Calling these functions
simultaneously from multiple threads does not have undesirable effects", so
we need to use strtok_r.
--*/
char *
__cdecl
PAL_strtok(char *strToken, const char *strDelimit)
{
    THREAD* lpThread = NULL;
    char *retval=NULL;

    PERF_ENTRY(strtok);
    ENTRY("strtok (strToken=%p (%s), strDelimit=%p (%s))\n", 
          strToken?strToken:"NULL", 
          strToken?strToken:"NULL", strDelimit?strDelimit:"NULL", strDelimit?strDelimit:"NULL");

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    retval = strtok_r(strToken, strDelimit, &lpThread->strtokContext);
 
done:
    LOGEXIT("strtok returns %p (%s)\n", retval?retval:"NULL", retval?retval:"NULL");
    PERF_EXIT(strtok);
    return(retval);
}


/*++
Function:
  PAL_strtoul

Convert string to an unsigned long-integer value.

Return Value

strtoul returns the converted value, if any, or ULONG_MAX on
overflow. It returns 0 if no conversion can be performed. errno is
set to ERANGE if overflow or underflow occurs.

Parameters

c_szNumber  Null-terminated string to convert to a ULONG
pszEnd          Pointer to character that stops scan
nBase           Number base to use

Remarks

strtoul stops reading the string c_szNumber at the first character it cannot
recognize as part of a number. This may be the terminating null
character, or it may be the first numeric character greater than or
equal to base. The LC_NUMERIC category setting of the current locale
determines recognition of the radix character in c_szNumber; for more
information, see setlocale. If pszEnd is not NULL, a pointer to the
character that stopped the scan is stored at the location pointed to
by pszEnd. If no conversion can be performed (no valid digits were
found or an invalid base was specified), the value of c_szNumber is stored
at the location pointed to by pszEnd.

Notes :
    MSDN states that only space and tab are accepted as leading whitespace, but
    tests indicate that other whitespace characters (newline, carriage return,
    etc) are also accepted. This matches the behavior on Unix systems.

    For strtoul, we need to check if the value to be returned 
    is outside the 32 bit range. If so, the returned value needs to be set
    as appropriate, according to the MSDN pages and in all instances errno 
    must be set to ERANGE (The one exception is converting a string 
    representing a negative value to unsigned long).
    Note that on 64 bit Windows, long's are still 32 bit. Thus, to match
    Windows behavior, we must return long's in the 32 bit range.
--*/

/* The use of ULONG is by design, to ensure that a 32 bit value is always 
returned from this function. If "unsigned long" is used instead of ULONG,
then a 64 bit value could be returned on 64 bit platforms like HP-UX, thus
breaking Windows behavior .*/
ULONG 
__cdecl 
PAL_strtoul(const char *c_szNumber, char **pszEnd, int nBase)
{
    unsigned long ulResult;
    
    PERF_ENTRY(strtoul);
    ENTRY("strtoul (c_szNumber=%p (%s), pszEnd=%p, nBase=%d)\n", 
        c_szNumber?c_szNumber:"NULL", 
        c_szNumber?c_szNumber:"NULL",
        pszEnd, 
        nBase);

    ulResult = strtoul(c_szNumber, pszEnd, nBase);


    LOGEXIT("strtoul returning unsigned long %lu\n", ulResult);
    PERF_EXIT(wcstoul);
    
    /* When returning unsigned long res from this function, it will be 
    implicitly cast to ULONG. This handles situations where a string that
    represents a negative number is passed in to strtoul. The Windows
    behavior is analogous to taking the binary equivalent of the negative
    value and treating it as a positive number. Returning a ULONG from
    this function, as opposed to native unsigned long, allows us to match
    this behavior. The explicit cast to ULONG below is used to silence any
    potential warnings due to the implicit casting.  */    
    return (ULONG)ulResult;
    
}


/*++
Function:
    PAL_atol

Convert string to a long value.

Return Value

atol returns the converted value, if any. In the case of overflow, 
the return value is undefined.

Parameters

c_szNumber  Null-terminated string to convert to a LONG

Notes:
    On Windows, atol returns LONG_MAX or LONG_MIN if the correct value is 
    outside the range of representable values. On some UNIX platforms, however,
    atol ignores overflow conditions and wrap around occurs. Thus, to match 
    Windows atol behavior on UNIX, strtol is used since it does not
    ignore overflow conditions.
--*/

/* The use of LONG is by design, to ensure that a 32 bit value is always 
returned from this function. If "long" is used instead of LONG, then a 64 bit 
value could be returned on 64 bit platforms like HP-UX, thus breaking 
Windows behavior. */
LONG 
__cdecl
PAL_atol(const char *c_szNumber)
{
    long lResult;

    PERF_ENTRY(atol);
    ENTRY("atol (c_szNumber=%p (%s))\n", 
        c_szNumber?c_szNumber:"NULL"
        );
    
    lResult = strtol(c_szNumber, (char **)NULL, 10);


    LOGEXIT("atol returning long %ld\n", (LONG)lResult);
    PERF_EXIT(atol);
    /* This explicit cast to LONG is used to silence any potential warnings
    due to implicitly casting the native long lResult to LONG when returning. */
    return (LONG)lResult;

}

