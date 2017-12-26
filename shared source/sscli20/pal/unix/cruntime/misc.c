/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    cruntime/misc.c

Abstract:

    Implementation of C runtime functions that don't fit anywhere else

--*/
#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/misc.h"
#include "pal/thread.h"


#include <errno.h>
/* <stdarg.h> needs to be included after "palinternal.h" to avoid name
   collision for va_start and va_end */
#include <stdarg.h>
#include <time.h>
#if HAVE_CRT_EXTERNS_H
#include <crt_externs.h>
#endif  // HAVE_CRT_EXTERNS_H

SET_DEFAULT_DEBUG_CHANNEL(CRT);

char **palEnvironment = NULL;

/*++
Function:
  realloc

See MSDN doc.
--*/
void* 
__cdecl 
PAL_realloc(
        void* memblock, 
        size_t size)
{
    void *ret;

    PERF_ENTRY(realloc);
    ENTRY("realloc (memblock:%p size=%d)\n", memblock, size);    
    if (size == 0)
    {
        free(memblock);
        ret = NULL;
    }
    else
    {
        ret = realloc(memblock, size);
    }
    LOGEXIT("realloc returns void * %p\n", ret);
    PERF_EXIT(realloc);
    
    return ret;
}

/*++
Function:
  _rotl

See MSDN doc.
--*/
unsigned int
__cdecl 
_rotl( unsigned int value, int shift )
{
    unsigned int retval = 0;

    PERF_ENTRY(_rotl);
    ENTRY("_rotl( value:%u shift=%d )\n", value, shift );   
    shift &= 0x1f;
    retval = ( value << shift ) | ( value >> ( sizeof( int ) * CHAR_BIT - shift ));
    LOGEXIT("_rotl returns unsigned int %u\n", retval);
    PERF_EXIT(_rotl);
    return retval;
}

/*++
Function:
  _rotr

See MSDN doc.
--*/
unsigned int
__cdecl 
_rotr( unsigned int value, int shift )
{
    unsigned int retval;

    PERF_ENTRY(_rotr);
    ENTRY("_rotr( value:%u shift=%d )\n", value, shift );    
    shift &= 0x1f;
    retval = ( value >> shift ) | ( value << ( sizeof( int ) * CHAR_BIT - shift ) );
    LOGEXIT("_rotr returns unsigned int %u\n", retval);
    PERF_EXIT(_rotr);
    return retval;
}

/*++
Function:
  _gcvt

See MSDN doc.
--*/
char * 
__cdecl 
_gcvt( double value, int digits, char * buffer )
{
    PERF_ENTRY(_gcvt);
    ENTRY( "_gcvt( value:%f digits=%d, buffer=%p )\n", value, digits, buffer );    

    if ( !buffer )
    {
        ERROR( "buffer was an invalid pointer.\n" );
    }

    switch ( digits )
    {
    case 7 :
        /* Fall through */
    case 8 :
        /* Fall through */
    case 15 :
        /* Fall through */
    case 17 :
        
        sprintf( buffer, "%.*g", digits, value );
        break;
    
    default :
        ASSERT( "Only the digits 7, 8, 15, and 17 are valid.\n" );
        *buffer = '\0';
    }
    
    LOGEXIT( "_gcvt returns %p (%s)\n", buffer , buffer );
    PERF_EXIT(_gcvt);
    return buffer;
}
    

/*++
Function :

    __iscsym

See MSDN for more details.
--*/
int
__cdecl 
__iscsym( int c )
{
    PERF_ENTRY(__iscsym);
    ENTRY( "__iscsym( c=%d )\n", c );

    if ( isalnum( c ) || c == '_'  )
    {
        LOGEXIT( "__iscsym returning 1\n" );
        PERF_EXIT(__iscsym);
        return 1;
    }

    LOGEXIT( "__iscsym returning 0\n" );
    PERF_EXIT(__iscsym);
    return 0;
}




/*++
Function:

    isprint

See MSDN for more details.
--*/
int
__cdecl
PAL_isprint( int c )
{
    int retval;
    PERF_ENTRY(isprint);
    ENTRY( "isprint( c=%d )\n", c );
    retval = isprint( c );
    LOGEXIT( "isprint returns %d\n", retval);
    PERF_EXIT(isprint);
    return retval;
}


/*++

Function :

    PAL_errno
    
    Returns the address of the errno.

--*/
int * __cdecl PAL_errno( int caller )
{
    int *retval;
    PERF_ENTRY(errno);
    ENTRY( "PAL_errno( void )\n" );
    retval = (INT*)(&errno);
    LOGEXIT("PAL_errno returns %p\n",retval);
    PERF_EXIT(errno);
    return retval;
}

/*++

Function : _putenv.
    
See MSDN for more details.

Note:   The BSD implementation can cause
        memory leaks. See man pages for more details.
--*/
int
__cdecl 
_putenv( const char * envstring )
{
    int ret = -1;

    PERF_ENTRY(_putenv);
    ENTRY( "_putenv( %p (%s) )\n", envstring ? envstring : "NULL", envstring ? envstring : "NULL") ;
    
    if (!envstring)
    {
        ERROR( "_putenv() called with NULL envstring!\n");
        goto EXIT;
    }

    ret = MiscPutenv(envstring, TRUE) ? 0 : -1;

EXIT:    
    LOGEXIT( "_putenv returning %d\n", ret);
    PERF_EXIT(_putenv);
    return ret;
}


/*++

Function : PAL_getenv
    
See MSDN for more details.
--*/
char * __cdecl PAL_getenv(const char *varname)
{
    char *retval;

    PERF_ENTRY(getenv);
    ENTRY("getenv (%p (%s))\n", varname ? varname : "NULL", varname ? varname : "NULL");
    
    if (strcmp(varname, "") == 0)
    {
        ERROR("getenv called with a empty variable name\n");
        LOGEXIT("getenv returning NULL\n");
        PERF_EXIT(getenv);
        return(NULL);
    }
    retval = MiscGetenv(varname);

    LOGEXIT("getenv returning %p\n", retval);
    PERF_EXIT(getenv);
    return(retval);
}

/*++
Function:

    localtime

See MSDN for more details.
--*/

struct PAL_tm *
__cdecl
PAL_localtime(const PAL_time_t *clock)
{
    THREAD* lpThread = NULL;
    struct tm tmpResult;
    struct PAL_tm *result = NULL;

    PERF_ENTRY(localtime);
    ENTRY( "localtime( clock=%p )\n",clock );

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }
    result = &lpThread->localtimeBuffer;

    localtime_r(clock, &tmpResult);

    // Copy the result into the Windows struct.
    result->tm_sec = tmpResult.tm_sec;
    result->tm_min = tmpResult.tm_min;
    result->tm_hour = tmpResult.tm_hour;
    result->tm_mday = tmpResult.tm_mday;
    result->tm_mon  = tmpResult.tm_mon;
    result->tm_year = tmpResult.tm_year;
    result->tm_wday = tmpResult.tm_wday;
    result->tm_yday = tmpResult.tm_yday;
    result->tm_isdst = tmpResult.tm_isdst;

done:
    LOGEXIT( "localtime returned %p\n", result );
    PERF_EXIT(localtime);
    return result;
}

/*++
Function:

    mktime

See MSDN for more details.
--*/

PAL_time_t 
__cdecl 
PAL_mktime(struct PAL_tm *tm)
{
    time_t result;
    struct tm tmpTm;

    PERF_ENTRY(mktime);
    ENTRY( "mktime( tm=%p )\n",tm );

    /*copy the value of Windows struct into BSD struct*/
    tmpTm.tm_sec = tm->tm_sec;
    tmpTm.tm_min = tm->tm_min;
    tmpTm.tm_hour = tm->tm_hour;
    tmpTm.tm_mday = tm->tm_mday;
    tmpTm.tm_mon  = tm->tm_mon;
    tmpTm.tm_year = tm->tm_year;
    tmpTm.tm_wday = tm->tm_wday;
    tmpTm.tm_yday = tm->tm_yday;
    tmpTm.tm_isdst = tm->tm_isdst;

    result = mktime(&tmpTm);

    LOGEXIT( "mktime returned %#lx\n",result );
    PERF_EXIT(mktime);
    return result;
}

/*++
Function:

    ctime

    There appears to be a difference between the FreeBSD and windows
    implementations.  FreeBSD gives Wed Dec 31 18:59:59 1969 for a
    -1 param, and Windows returns NULL

See MSDN for more details.
--*/
char *
__cdecl
PAL_ctime( const PAL_time_t *clock )
{
    THREAD* lpThread = NULL;
    char * retval = NULL;

    PERF_ENTRY(ctime);
    ENTRY( "ctime( clock=%p )\n",clock );
    if(*clock < 0)
    {
        /*If the input param is less than zero the value
         *returned is less than the Unix epoch
         *1st of January 1970*/
        WARN("The input param is less than zero");
        goto done;
    }

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }
    retval = lpThread->ctimeBuffer;

    ctime_r(clock,retval);

done:
    LOGEXIT( "ctime() returning %p (%s)\n",retval,retval);
    PERF_EXIT(ctime);
    return retval;
}

/*++
Function:

   rand

   The difference between the FreeBSD and Windows implementations is the max
   of the return value. in FreeBSD, RAND_MAX is 0x7fffffff and in Windows
   it's 0x7fff.

See MSDN for more details.
--*/
int
__cdecl PAL_rand(void)
{
    int ret;
    PERF_ENTRY(rand);
    ENTRY("rand(void)\n");

    ret = rand() % (PAL_RAND_MAX + 1);

    LOGEXIT("rand() returning %d\n", ret);
    PERF_EXIT(rand);
    return ret;
}

/**
Function:

    _ecvt

See MSDN for more information.

NOTES:
    There is a difference between PAL _ecvt and Win32 _ecvt.

    If Window's _ecvt receives a double 0.000000000000000000005, and count 50
    the result is "49999999999999998000000000000000000000000000000000"

    Under BSD the same call will result in :
    49999999999999998021734900744965462766153934333829

    The difference is due to the difference between BSD and Win32 sprintf.

--*/
char * __cdecl
_ecvt( double value, int count, int * dec, int * sign )
{
    CONST CHAR * FORMAT_STRING = "%.348e";
    CHAR TempBuffer[ ECVT_MAX_BUFFER_SIZE ];
    THREAD* lpThread = NULL;
    LPSTR lpReturnBuffer = NULL;
    LPSTR lpStartOfReturnBuffer = NULL;
    LPSTR lpTempBuffer = NULL;
    LPSTR lpEndOfTempBuffer = NULL;
    UINT nTempBufferLength = 0;
    CHAR ExponentBuffer[ 6 ];
    INT nExponentValue = 0;
    INT LoopIndex = 0;

    PERF_ENTRY(_ecvt);
    ENTRY( "_ecvt( value=%.30g, count=%d, dec=%p, sign=%p )\n",
           value, count, dec, sign );

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    lpStartOfReturnBuffer = lpReturnBuffer = lpThread->ECVTBuffer;

    /* Sanity checks */
    if ( !dec || !sign )
    {
        ERROR( "dec and sign have to be valid pointers.\n" );
        *lpReturnBuffer = '\0';
        goto done;
    }
    else
    {
        *dec = *sign = 0;
    }

    if ( value < 0.0 )
    {
        *sign = 1;
    }

    if ( count > ECVT_MAX_COUNT_SIZE )
    {
        count = ECVT_MAX_COUNT_SIZE;
    }

    /* Get the string to work with. */
    sprintf( TempBuffer, FORMAT_STRING, value );

    /* Check to see if value was a valid number. */
    if ( strcmp( "NaN", TempBuffer ) == 0 || strcmp( "-NaN", TempBuffer ) == 0 )
    {
        TRACE( "value was not a number!\n" );
        strcpy( lpStartOfReturnBuffer, "1#QNAN0" );
        *dec = 1;
        goto done;
    }

    /* Check to see if it is infinite. */
    if ( strcmp( "Inf", TempBuffer ) == 0 || strcmp( "-Inf", TempBuffer ) == 0 )
    {
        TRACE( "value is infinite!\n" );
        strcpy( lpStartOfReturnBuffer, "1#INF00" );
        *dec = 1;
        if ( *TempBuffer == '-' )
        {
            *sign = 1;
        }
        goto done;
    }

    nTempBufferLength = strlen( TempBuffer );
    lpEndOfTempBuffer = &(TempBuffer[ nTempBufferLength ]);

    /* Extract the exponent, and convert it to integer. */
    while ( *lpEndOfTempBuffer != 'e' && nTempBufferLength > 0 )
    {
        nTempBufferLength--;
        lpEndOfTempBuffer--;
    }
    
    ExponentBuffer[ 0 ] = '\0';
    strncat( ExponentBuffer, lpEndOfTempBuffer + 1, 5 ); 
    nExponentValue = atoi( ExponentBuffer );

    /* End the string at the 'e' */
    *lpEndOfTempBuffer = '\0';
    nTempBufferLength--;

    /* Determine decimal location. */
    if ( nExponentValue == 0 )
    {
        *dec = 1;
    }
    else
    {
        *dec = nExponentValue + 1;
    }

    if ( value == 0.0 )
    {
        *dec = 0;
    }
    /* Copy the string from the temp buffer upto count characters, 
    removing the sign, and decimal as required. */
    lpTempBuffer = TempBuffer;
    *lpReturnBuffer = '0';
    lpReturnBuffer++;

    while ( LoopIndex < ECVT_MAX_COUNT_SIZE )
    {
        if ( *lpTempBuffer != '-' && 
             *lpTempBuffer != '+' && 
             *lpTempBuffer != '.')
        {
            *lpReturnBuffer = *lpTempBuffer;
            LoopIndex++;
            lpReturnBuffer++;
        }
        lpTempBuffer++;

        if ( LoopIndex == count + 1 )
        {
            break;
        }
    }

    *lpReturnBuffer = '\0';

    /* Round if needed. If count is less then 0 
    then windows does not round for some reason.*/
    nTempBufferLength = strlen( lpStartOfReturnBuffer ) - 1;
    
    /* Add one for the preceeding zero. */
    lpReturnBuffer = ( lpStartOfReturnBuffer + 1 );

    if ( nTempBufferLength >= count && count >= 0 )
    {
        /* Determine whether I need to round up. */
        if ( *(lpReturnBuffer + count) >= '5' )
        {
            CHAR cNumberToBeRounded;
            if ( count != 0 )
            {
                cNumberToBeRounded = *(lpReturnBuffer + count - 1);
            }
            else
            {
                cNumberToBeRounded = *lpReturnBuffer;
            }
            
            if ( cNumberToBeRounded < '9' )
            {
                if ( count > 0 )
                {
                    /* Add one to the character. */
                    (*(lpReturnBuffer + count - 1))++;
                }
                else
                {
                    if ( cNumberToBeRounded >= '5' )
                    {
                        (*dec)++;
                    }
                }
            }
            else
            {
                LPSTR lpRounding = NULL;

                if ( count > 0 )
                {
                    lpRounding = lpReturnBuffer + count - 1;
                }
                else
                {
                    lpRounding = lpReturnBuffer + count;
                }

                while ( cNumberToBeRounded == '9' )
                {
                    cNumberToBeRounded = *lpRounding;
                    
                    if ( cNumberToBeRounded == '9' )
                    {
                        *lpRounding = '0';
                        lpRounding--;
                    }
                }
                
                if ( lpRounding == lpStartOfReturnBuffer )
                {
                    /* Overflow. number is a whole number now. */
                    *lpRounding = '1';
                    memset( ++lpRounding, '0', count);

                    /* The decimal has moved. */
                    (*dec)++;
                }
                else
                {
                    *lpRounding = ++cNumberToBeRounded;
                }
            }
        }
        else
        {
            /* Get rid of the preceding 0 */
            lpStartOfReturnBuffer++;
        }
    }

    if ( *lpStartOfReturnBuffer == '0' )
    {
        lpStartOfReturnBuffer++;
    }

    if ( count >= 0 )
    {
        *(lpStartOfReturnBuffer + count) = '\0';
    }
    else
    {
        *lpStartOfReturnBuffer = '\0';
    }

done:    
    LOGEXIT( "_ecvt returning %p (%s)\n", lpStartOfReturnBuffer , lpStartOfReturnBuffer );
    PERF_EXIT(_ecvt);
    return lpStartOfReturnBuffer;
}


PALIMPORT 
void __cdecl 
PAL_qsort(void *base, size_t nmemb, size_t size, 
          int (__cdecl *compar )(const void *, const void *))
{
    PERF_ENTRY(qsort);
    ENTRY("qsort(base=%p, nmemb=%lu, size=%lu, compar=%p\n",
          base,(unsigned long) nmemb,(unsigned long) size, compar);

/* reset ENTRY nesting level back to zero, qsort will invoke app-defined 
   callbacks and we want their entry traces... */
#if !_NO_DEBUG_MESSAGES_
{
    int old_level;
    old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

    qsort(base,nmemb,size,compar);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
    DBG_change_entrylevel(old_level);
}
#endif /* !_NO_DEBUG_MESSAGES_ */

    LOGEXIT("qsort returns\n");
    PERF_EXIT(qsort);
}

PALIMPORT 
void * __cdecl 
PAL_bsearch(const void *key, const void *base, size_t nmemb, size_t size,
            int (__cdecl *compar)(const void *, const void *))
{
    void *retval;

    PERF_ENTRY(bsearch);
    ENTRY("bsearch(key=%p, base=%p, nmemb=%lu, size=%lu, compar=%p\n", 
          key, base, (unsigned long) nmemb, (unsigned long) size, compar);

/* reset ENTRY nesting level back to zero, bsearch will invoke app-defined 
   callbacks and we want their entry traces... */
#if !_NO_DEBUG_MESSAGES_
{
    int old_level;
    old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

    retval = bsearch(key,base,nmemb,size,compar);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
    DBG_change_entrylevel(old_level);
}
#endif /* !_NO_DEBUG_MESSAGES_ */

    LOGEXIT("bsearch returns %p\n",retval);
    PERF_EXIT(bsearch);
    return retval;
}


/*++
Function:
  MiscInitialize

Initialization function called from PAL_Initialize.
Allocates the TLS Index. On systems that use extern variables for
time zone information, this also initializes those variables.

Note: This is called before debug channels are initialized, so it
      cannot use debug tracing calls.
--*/
BOOL
MiscInitialize(void)
{
#if HAVE__NSGETENVIRON
    palEnvironment = *(_NSGetEnviron());
#else   // HAVE__NSGETENVIRON
    extern char **environ;
    
    palEnvironment = environ;
#endif  // HAVE__NSGETENVIRON
    
#if HAVE_TIMEZONE_VAR
    tzset();
#endif  // HAVE_TIMEZONE_VAR

    return TRUE;
}

/*++
Function:
  MiscCleanup

Termination function called from PAL_Terminate to delete the
TLS Keys created in MiscInitialize
--*/
void MiscCleanup(void)
{
     TRACE("Cleaning Misc...\n");
}

/*++
Function:
  MiscGetenv

Gets an environment variable's value from environ. The returned buffer
must not be modified or freed.
--*/
char *MiscGetenv(const char *name)
{
    int i, length;
    char *equals;
    
    length = strlen(name);
    for(i = 0; palEnvironment[i] != NULL; i++)
    {
        if (memcmp(palEnvironment[i], name, length) == 0)
        {
            equals = palEnvironment[i] + length;
            if (*equals == '\0')
            {
                return (char *) "";
            } 
            else if (*equals == '=') 
            {
                return equals + 1;
            }
        }
    }
    return NULL;
}

/*++
Function:
  MiscPutenv

Sets an environment variable's value by directly modifying palEnvironment.
Returns TRUE if the variable was set, or FALSE if malloc or realloc
failed or if the given string is malformed.
--*/
BOOL MiscPutenv(const char *string, BOOL deleteIfEmpty)
{
    const char *equals, *existingEquals;
    char *copy;
    int length;
    int i, j;
    BOOL result = FALSE;
    
    equals = strchr(string, '=');
    if (equals == string || equals == NULL)
    {
        // "=foo" and "foo" have no meaning
        goto done;
    }
    if (equals[1] == '\0' && deleteIfEmpty)
    {
        // "foo=" removes foo from the environment in _putenv() on Windows.
        // The same string can result from a call to SetEnvironmentVariable()
        // with the empty string as the value, but in that case we want to
        // set the variable's value to "". deleteIfEmpty will be FALSE in
        // that case.
        length = strlen(string);
        copy = (char *) malloc(length);
        if (copy == NULL)
        {
            goto done;
        }
        memcpy(copy, string, length - 1);
        copy[length - 1] = '\0';    // Change '=' to '\0'
        MiscUnsetenv(copy);
        free(copy);
        result = TRUE;
    }
    else
    {
        // See if we are replacing an item or adding one.
        
        // Make our copy up front, since we'll use it either way.
        copy = strdup(string);
        if (copy == NULL)
        {
            goto done;
        }
        
        length = equals - string;
        for(i = 0; palEnvironment[i] != NULL; i++)
        {
            existingEquals = strchr(palEnvironment[i], '=');
            if (existingEquals == NULL)
            {
                // The PAL screens out malformed strings, but
                // environ comes from the system, so it might
                // have strings without '='. We treat the entire
                // string as a name in that case.
                existingEquals = palEnvironment[i] + strlen(palEnvironment[i]);
            }
            if (existingEquals - palEnvironment[i] == length)
            {
                if (memcmp(string, palEnvironment[i], length) == 0)
                {
                    // Replace this one. Don't free the original,
                    // though, because there may be outstanding
                    // references to it that were acquired via
                    // getenv. This is an unavoidable memory leak.
                    palEnvironment[i] = copy;
                    result = TRUE;
                    break;
                }
            }
        }
        if (palEnvironment[i] == NULL)
        {
            static BOOL sAllocatedEnviron;
            // Add a new environment variable.
            // We'd like to realloc palEnvironment, but we can't do that the
            // first time through.
            char **newEnviron = NULL;
            if (sAllocatedEnviron) {
                if ((newEnviron = realloc(palEnvironment, (i + 2) * sizeof(char *))) == NULL)
                {
                    goto done;
                }
            }
            else
            {
                // Allocate palEnvironment ourselves so we can realloc it later.
                newEnviron = (char **) malloc((i + 2) * sizeof(char *));
                if (newEnviron == NULL)
                {
                    goto done;
                }
                for(j = 0; palEnvironment[j] != NULL; j++)
                {
                    newEnviron[j] = palEnvironment[j];
                }
                sAllocatedEnviron = TRUE;
            }
            palEnvironment = newEnviron;
#if HAVE__NSGETENVIRON
            *(_NSGetEnviron()) = palEnvironment;
#endif  // HAVE__NSGETENVIRON
            palEnvironment[i] = copy;
            palEnvironment[i + 1] = NULL;
            result = TRUE;
        }
    }
done:
    return result;
}

/*++
Function:
  MiscUnsetenv

Removes a variable from the environment. Does nothing if the variable
does not exist in the environment.
--*/
void MiscUnsetenv(const char *name)
{
    const char *equals;
    int length;
    int i, j;
    
    length = strlen(name);
    for(i = 0; palEnvironment[i] != NULL; i++)
    {
        equals = strchr(palEnvironment[i], '=');
        if (equals == NULL)
        {
            equals = palEnvironment[i] + strlen(palEnvironment[i]);
        }
        if (equals - palEnvironment[i] == length)
        {
            if (memcmp(name, palEnvironment[i], length) == 0)
            {
                // Remove this one. Don't free it, though, since
                // there might be oustanding references to it that
                // were acquired via getenv. This is an
                // unavoidable memory leak.
                for(j = i + 1; palEnvironment[j] != NULL; j++) { }
                // i is now the one we want to remove. j is the
                // last index in palEnvironment, which is NULL.

                // Shift palEnvironment down by the difference between i and j.
                memmove(palEnvironment + i, palEnvironment + i + 1, (j - i) * sizeof(char *));
            }
        }
    }
}
