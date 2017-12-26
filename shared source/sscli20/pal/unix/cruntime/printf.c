/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    printf.c

Abstract:

    Implementation of the printf family functions.

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/cruntime.h"


/* <stdarg.h> needs to be included after "palinternal.h" to avoid name
   collision for va_start and va_end */
#include <stdarg.h>
#include <errno.h>

SET_DEFAULT_DEBUG_CHANNEL(CRT);

#if SSCANF_SUPPORT_ll
const static char *scanf_longlongfmt = "ll";
#else
const static char *scanf_longlongfmt = "q";
#endif

#if SSCANF_CANNOT_HANDLE_MISSING_EXPONENT
static int SscanfFloatEBugWorkAround (LPCSTR buff, LPCSTR floatFmt, 
                                      void * voidPtr, int * pn);
#endif // SSCANF_CANNOT_HANDLE_MISSING_EXPONENT

/*******************************************************************************
Function:
  Internal_Convertfwrite
  This function is a wrapper around fwrite for cases where the buffer has
  to be converted from WideChar to MultiByte
*******************************************************************************/

static size_t  Internal_Convertfwrite(const void *buffer, size_t size, size_t count , FILE *stream,BOOL convert)
{
   int ret;

    if(convert)
    {
        int nsize;
        LPSTR newBuff = 0;
        nsize = WideCharToMultiByte(CP_ACP, 0,(LPCWSTR)buffer, count, 0, 0, 0, 0);
        if (!nsize)
        {
            ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                GetLastError());
                return -1;
        }
        newBuff = (LPSTR) malloc(nsize);
        if (!newBuff)
        {
            ERROR("malloc failed\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return -1;
        }
        nsize = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffer, count,
                                newBuff, nsize, 0, 0);
        if (!nsize)
        {
            ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                  GetLastError());
            free(newBuff);
            return -1;
        }
        ret = fwrite(newBuff,1,count,stream);
        free(newBuff);
   }
   else
   {
        ret = fwrite(buffer,size,count,stream);
   }
   return ret;

}
/*******************************************************************************
Function:
  Internal_ExtractFormatA

Paramaters:
  Fmt
    - format string to parse
    - first character must be a '%'
    - paramater gets updated to point to the character after
      the %<foo> format string
  Out
    - buffer will contain the %<foo> format string
  Flags
    - paramater will be set with the PRINTF_FORMAT_FLAGS defined above
  Width
    - will contain the width specified by the format string
    - -1 if none given
  Precision
    - will contain the precision specified in the format string
    - -1 if none given
  Prefix
    - an enumeration of the type prefix
  Type
    - an enumeration of the type value

Notes:
  - I'm also handling the undocumented %ws, %wc, %w...
  - %#10x, when we have a width greater than the length (i.e padding) the
    length of the padding is not consistent with MS's wsprintf
    (MS adds an extra 2 padding chars, length of "0x")
  - MS's wsprintf seems to ingore a 'h' prefix for number types
  - MS's "%p" is different than gcc's
    e.g. printf("%p", NULL);
        MS  -->  00000000
        gcc -->  0x0
  - the length of the exponent (precision) for floating types is different
    between MS and gcc
    e.g. printf("%E", 256.0);
        MS  -->  2.560000E+002
        gcc -->  2.560000E+02
*******************************************************************************/
static BOOL Internal_ExtractFormatA(LPCSTR *Fmt, LPSTR Out, LPINT Flags,
                                    LPINT Width, LPINT Precision,
                                    LPINT Prefix, LPINT Type)
{
    BOOL Result = FALSE;
    LPSTR TempStr;
    LPSTR TempStrPtr;

    *Width = -1;
    *Precision = -1;
    *Flags = 0;
    *Prefix = -1;
    *Type = -1;

    if (*Fmt && **Fmt == '%')
    {
        *Out++ = *(*Fmt)++;
    }
    else
    {
        return Result;
    }

    /* we'll never need a temp string longer than the original */
    TempStrPtr = TempStr = (LPSTR) malloc(strlen(*Fmt)+1);
    if (!TempStr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    /* parse flags */
    while (**Fmt && (**Fmt == '-' || **Fmt == '+' ||
           **Fmt == '0' || **Fmt == ' ' || **Fmt == '#'))
    {
        switch (**Fmt)
        {
        case '-':
            *Flags |= PFF_MINUS; break;
        case '+':
            *Flags |= PFF_PLUS; break;
        case '0':
            *Flags |= PFF_ZERO; break;
        case ' ':
            *Flags |= PFF_SPACE; break;
        case '#':
            *Flags |= PFF_POUND; break;
        }
            *Out++ = *(*Fmt)++;
    }
    /* '-' flag negates '0' flag */
    if ((*Flags & PFF_MINUS) && (*Flags & PFF_ZERO))
    {
        *Flags -= PFF_ZERO;
    }

    /* grab width specifier */
    if (isdigit((unsigned char) **Fmt))
    {
        TempStrPtr = TempStr;
        while (isdigit((unsigned char) **Fmt))
        {
            *TempStrPtr++ = **Fmt;
            *Out++ = *(*Fmt)++;
        }
        *TempStrPtr = 0; /* end string */
        *Width = atol(TempStr);
    }

    /* grab precision specifier */
    if (**Fmt == '.')
    {
        *Out++ = *(*Fmt)++;
        if (isdigit((unsigned char) **Fmt))
        {
            TempStrPtr = TempStr;
            while (isdigit((unsigned char) **Fmt))
            {
                *TempStrPtr++ = **Fmt;
                *Out++ = *(*Fmt)++;
            }
            *TempStrPtr = 0; /* end string */
            *Precision = atol(TempStr);
        }
        else if (**Fmt == '*')
        {
            *Precision = PRECISION_STAR;
            *Out++ = *(*Fmt)++;
            if (isdigit((unsigned char) **Fmt))
            {
                /* this is an invalid precision because we have a .* then a
                   number */
                /* printf handles this by just printing the whole string */
                *Precision = PRECISION_INVALID;
                while (isdigit((unsigned char) **Fmt))
                {
                    *Out++ = *(*Fmt)++;
                }
            }
            else
            {
                *Precision = PRECISION_STAR;
            }
        }
        else
        {
            *Precision = PRECISION_DOT;
        }
    }

    /* grab width specifier */
    if(**Fmt == '*')
    {
        *Precision = PRECISION_STAR;
        *Out++ = *(*Fmt)++;
        if (isdigit((unsigned char) **Fmt))
        {
            /* this is an invalid precision because we have a * then a
               number */
            /* printf handles this by just printing the whole string */
            *Precision = PRECISION_INVALID;
            while (isdigit((unsigned char) **Fmt))
            {
               *Out++ = *(*Fmt)++;
            }
        }
    }

    /* grab prefix of 'I64' for __int64 */
    if ((*Fmt)[0] == 'I' && (*Fmt)[1] == '6' && (*Fmt)[2] == '4')
    {
        /* convert to 'll' so BSD's snprintf can handle it */
        *Fmt += 3;
        *Prefix = PFF_PREFIX_LONGLONG;
    }
    /* grab a prefix of 'h' */
    else if (**Fmt == 'h')
    {
        *Prefix = PFF_PREFIX_SHORT;
        ++(*Fmt);
    }
    /* grab prefix of 'l' or the undocumented 'w' (at least in MSDN) */
    else if (**Fmt == 'l' || **Fmt == 'w')
    {
        *Prefix = PFF_PREFIX_LONG;
        ++(*Fmt);
    }
    else if (**Fmt == 'L')
    {
        /* a prefix of 'L' seems to be ignored */
        ++(*Fmt);
    }

    /* grab type 'c' */
    if (**Fmt == 'c' || **Fmt == 'C')
    {
        *Type = PFF_TYPE_CHAR;
        if (*Prefix != PFF_PREFIX_SHORT && **Fmt == 'C')
        {
            *Prefix = PFF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == PFF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 'c';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab type 's' */
    else if (**Fmt == 's' || **Fmt == 'S')
    {
        *Type = PFF_TYPE_STRING;
        if (*Prefix != PFF_PREFIX_SHORT && **Fmt == 'S')
        {
            *Prefix = PFF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == PFF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 's';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab int types types */
    else if (**Fmt == 'd' || **Fmt == 'i' || **Fmt == 'o' ||
             **Fmt == 'u' || **Fmt == 'x' || **Fmt == 'X')
    {
        *Type = PFF_TYPE_INT;
        if (*Prefix == PFF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        else if (*Prefix == PFF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        else if (*Prefix == PFF_PREFIX_LONGLONG)
        {
            *Out++ = 'l';
            *Out++ = 'l';
        }
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'e' || **Fmt == 'E' || **Fmt == 'f' ||
             **Fmt == 'g' || **Fmt == 'G')
    {
        /* we can safely ignore the prefixes and only add the type*/
        *Type = PFF_TYPE_FLOAT;
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'n')
    {
        if (*Prefix == PFF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        *Out++ = *(*Fmt)++;
        *Type = PFF_TYPE_N;
        Result = TRUE;
    }
    else if (**Fmt == 'p')
    {
        *Type = PFF_TYPE_P;
        (*Fmt)++;
        *Out++ = '.';
        
        if (*Prefix == PFF_PREFIX_LONGLONG)
        {
            /* native *printf does not support %I64p
               (actually %llp), so we need to cheat a little bit */
            *Out++ = '1';
            *Out++ = '6';
            *Out++ = 'l';
            *Out++ = 'l';
        }
        else
        {
            *Out++ = '8';
        }

        *Out++ = 'X';
        Result = TRUE;
    }

    *Out = 0;  /* end the string */
    free(TempStr);
    return Result;
}

/*******************************************************************************
Function:
  Internal_ExtractFormatW

  -- see Internal_ExtractFormatA above
*******************************************************************************/
static BOOL Internal_ExtractFormatW(LPCWSTR *Fmt, LPSTR Out, LPINT Flags,
                                    LPINT Width, LPINT Precision,
                                    LPINT Prefix, LPINT Type)
{
    BOOL Result = FALSE;
    LPSTR TempStr;
    LPSTR TempStrPtr;

    *Width = -1;
    *Precision = -1;
    *Flags = 0;
    *Prefix = -1;
    *Type = -1;

    if (*Fmt && **Fmt == '%')
    {
        *Out++ = (CHAR) *(*Fmt)++;
    }
    else
    {
        return Result;
    }

    /* we'll never need a temp string longer than the original */
    TempStrPtr = TempStr = (LPSTR) malloc(PAL_wcslen(*Fmt)+1);
    if (!TempStr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    /* parse flags */
    while (**Fmt && (**Fmt == '-' || **Fmt == '+' ||
           **Fmt == '0' || **Fmt == ' ' || **Fmt == '#'))
    {
        switch (**Fmt)
        {
        case '-':
            *Flags |= PFF_MINUS; break;
        case '+':
            *Flags |= PFF_PLUS; break;
        case '0':
            *Flags |= PFF_ZERO; break;
        case ' ':
            *Flags |= PFF_SPACE; break;
        case '#':
            *Flags |= PFF_POUND; break;
        }
            *Out++ = (CHAR) *(*Fmt)++;
    }
    /* '-' flag negates '0' flag */
    if ((*Flags & PFF_MINUS) && (*Flags & PFF_ZERO))
    {
        *Flags -= PFF_ZERO;
    }

    /* grab width specifier */
    if (isdigit(**Fmt))
    {
        TempStrPtr = TempStr;
        while (isdigit(**Fmt))
        {
            *TempStrPtr++ = (CHAR) **Fmt;
            *Out++ = (CHAR) *(*Fmt)++;
        }
        *TempStrPtr = 0; /* end string */
        *Width = atol(TempStr);
    }

    /* grab precision specifier */
    if (**Fmt == '.')
    {
        *Out++ = (CHAR) *(*Fmt)++;
        if (isdigit(**Fmt))
        {
            TempStrPtr = TempStr;
            while (isdigit(**Fmt))
            {
                *TempStrPtr++ = (CHAR) **Fmt;
                *Out++ = (CHAR) *(*Fmt)++;
            }
            *TempStrPtr = 0; /* end string */
            *Precision = atol(TempStr);
        }
        else if (**Fmt == '*')
        {
            *Precision = PRECISION_STAR;
            *Out++ = (CHAR) *(*Fmt)++;
            if (isdigit(**Fmt))
            {
                /* this is an invalid precision because we have a .* then a
                   number */
                /* printf handles this by just printing the whole string */
                *Precision = PRECISION_INVALID;
                while (isdigit(**Fmt))
                {
                    *Out++ = (CHAR) *(*Fmt)++;
                }
            }
            else
            {
                *Precision = PRECISION_STAR;
            }
        }
        else
        {
            *Precision = PRECISION_DOT;
        }
    }

    /* grab prefix of 'I64' for __int64 */
    if ((*Fmt)[0] == 'I' && (*Fmt)[1] == '6' && (*Fmt)[2] == '4')
    {
        /* convert to 'll' so BSD's snprintf can handle it */
        *Fmt += 3;
        *Prefix = PFF_PREFIX_LONGLONG;
    }
    /* grab a prefix of 'h' */
    else if (**Fmt == 'h')
    {
        *Prefix = PFF_PREFIX_SHORT;
        ++(*Fmt);
    }
    else if (**Fmt == 'l' || **Fmt == 'w')
    {
        *Prefix = PFF_PREFIX_LONG_W;
        ++(*Fmt);
    }
    else if (**Fmt == 'L')
    {
        /* a prefix of 'L' seems to be ignored */
        ++(*Fmt);
    }


    /* grab type 'c' */
    if (**Fmt == 'c' || **Fmt == 'C')
    {
        *Type = PFF_TYPE_CHAR;
        if (*Prefix != PFF_PREFIX_SHORT && **Fmt == 'c')
        {
            *Prefix = PFF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == PFF_PREFIX_LONG || PFF_PREFIX_LONG_W)
        {
            *Out++ = 'l';
            *Prefix = PFF_PREFIX_LONG;
        }
        *Out++ = 'c';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab type 's' */
    else if (**Fmt == 's' || **Fmt == 'S' )
    {
        if ( **Fmt == 'S' )
        {
           *Type = PFF_TYPE_WSTRING;
        }
        else
        {
            *Type = PFF_TYPE_STRING;
        }
        if (*Prefix != PFF_PREFIX_SHORT && **Fmt == 's')
        {
            *Prefix = PFF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == PFF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }

        *Out++ = 's';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab int types types */
    else if (**Fmt == 'd' || **Fmt == 'i' || **Fmt == 'o' ||
             **Fmt == 'u' || **Fmt == 'x' || **Fmt == 'X')
    {
        *Type = PFF_TYPE_INT;
        if (*Prefix == PFF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        else if (*Prefix == PFF_PREFIX_LONG || *Prefix == PFF_PREFIX_LONG_W)
        {
            *Out++ = 'l';
            *Prefix = PFF_PREFIX_LONG;
        }
        else if (*Prefix == PFF_PREFIX_LONGLONG)
        {
            *Out++ = 'l';
            *Out++ = 'l';
        }
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'e' || **Fmt == 'E' || **Fmt == 'f' ||
             **Fmt == 'g' || **Fmt == 'G')
    {
        /* we can safely ignore the prefixes and only add the type*/
        if (*Prefix == PFF_PREFIX_LONG_W)
        {
            *Prefix = PFF_PREFIX_LONG;
        }

        *Type = PFF_TYPE_FLOAT;
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'n')
    {
        if (*Prefix == PFF_PREFIX_LONG_W)
        {
            *Prefix = PFF_PREFIX_LONG;
        }

        if (*Prefix == PFF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        *Out++ = *(*Fmt)++;
        *Type = PFF_TYPE_N;
        Result = TRUE;
    }
    else if (**Fmt == 'p')
    {
        *Type = PFF_TYPE_P;
        (*Fmt)++;
        *Out++ = '.';

        if (*Prefix == PFF_PREFIX_LONGLONG)
        {
            /* native *printf does not support %I64p
               (actually %llp), so we need to cheat a little bit */
            *Out++ = '1';
            *Out++ = '6';
            *Out++ = 'l';
            *Out++ = 'l';
        }
        else
        {
            if (*Prefix == PFF_PREFIX_LONG_W)
            {
                *Prefix = PFF_PREFIX_LONG;
            }
            *Out++ = '8';
        }
        *Out++ = 'X';

        Result = TRUE;
    }

    *Out = 0;  /* end the string */
    free(TempStr);
    return Result;
}

/*******************************************************************************
Function:
  Internal_AddPaddingA

Parameters:
  Out
    - buffer to place padding and given string (In)
  Count
    - maximum chars to be copied so as not to overrun given buffer
  In
    - string to place into (Out) accompanied with padding
  Padding
    - number of padding chars to add
  Flags
    - padding style flags (PRINTF_FORMAT_FLAGS)
*******************************************************************************/
BOOL Internal_AddPaddingA(LPSTR *Out, INT Count, LPSTR In,
                                 INT Padding, INT Flags)
{
    LPSTR OutOriginal = *Out;
    INT PaddingOriginal = Padding;
    INT LengthInStr;
    LengthInStr = strlen(In);
 
 
    if (Padding < 0)
    {
        /* this is used at the bottom to determine if the buffer ran out */
        PaddingOriginal = 0;
    }
    if (Flags & PFF_MINUS) /* pad on right */
    {
        strncpy(*Out, In, min(LengthInStr + 1, Count));
        *Out += min(LengthInStr, Count);
    }
    if (Padding > 0)
    {
        if (Flags & PFF_ZERO) /* '0', pad with zeros */
        {
            while (Padding-- && Count > *Out - OutOriginal)
            {
                *(*Out)++ = '0';
            }
        }
        else /* pad left with spaces */
        {
            while (Padding-- && Count > *Out - OutOriginal)
            {
                *(*Out)++ = ' ';
            }
        }
    }
    if (!(Flags & PFF_MINUS)) /* put 'In' after padding */
    {
        strncpy(*Out, In,
                min(LengthInStr + 1, Count - (*Out - OutOriginal)));
        *Out += min(LengthInStr, Count - (*Out - OutOriginal));
    }

    if (LengthInStr + PaddingOriginal > Count)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
Function:
  Internal_AddPaddingW

  -- see Internal_AddPaddingA above
*******************************************************************************/
static BOOL Internal_AddPaddingW(LPWSTR *Out, INT Count, LPWSTR In,
                                 INT Padding, INT Flags)
{
    LPWSTR OutOriginal = *Out;
    INT PaddingOriginal = Padding;
    INT LengthInStr;
    LengthInStr = PAL_wcslen(In);
    

    if (Padding < 0)
    {
        /* this is used at the bottom to determine if the buffer ran out */
        PaddingOriginal = 0;
    }
    if (Flags & PFF_MINUS) /* pad on right */
    {
        lstrcpynW(*Out, In, min(LengthInStr + 1, Count));
        *Out += min(LengthInStr, Count);
    }
    if (Padding > 0)
    {

        if (Flags & PFF_ZERO) /* '0', pad with zeros */
        {
            while (Padding-- && Count > *Out - OutOriginal)
            {
                *(*Out)++ = '0';
            }
        }
        else /* pad left with spaces */
        {
            while (Padding-- && Count > *Out - OutOriginal)
            {
                *(*Out)++ = ' ';
            }
        }
    }
    if (!(Flags & PFF_MINUS)) /* put 'In' after padding */
    {
        lstrcpynW(*Out, In,
                  min(LengthInStr + 1, Count - (*Out - OutOriginal)));
        *Out += min(LengthInStr, Count - (*Out - OutOriginal));
    }

    if (LengthInStr + PaddingOriginal > Count)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/*******************************************************************************
Function:
  PAL_printf_arg_remover

Parameters:
  ap
    - pointer to the va_list from which to remove arguments
  Precision
    - the precision of the current format option
  Type
    - the type of the argument for the current format option
  Prefix
    - the prefix for the current format option
*******************************************************************************/
void PAL_printf_arg_remover(va_list *ap, INT Precision, INT Type, INT Prefix)
{
    /* remove arg and precision if needed */
    if (PRECISION_STAR == Precision ||
        PRECISION_INVALID == Precision)
    {
        (void)va_arg(*ap, LONG);
    }
    if (Type == PFF_TYPE_FLOAT)
    {
        (void)va_arg(*ap, double);
    }
    else if (Type == PFF_TYPE_INT && Prefix == PFF_PREFIX_LONGLONG)
    {  
        (void)va_arg(*ap, INT64);
    }
    else
    {
        (void)va_arg(*ap, LONG);
    }
}

/*******************************************************************************
Function:
  PAL_vsnprintf

Parameters:
  Buffer
    - out buffer
  Count
    - buffer size
  Format
    - format string
  ap
    - stdarg parameter list
*******************************************************************************/
static INT PAL_vsnprintf(LPSTR Buffer, INT Count, LPCSTR Format, va_list ap)
{
    BOOL BufferRanOut = FALSE;
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    LPSTR BufferPtr = Buffer;
    LPCSTR Fmt = Format;
    LPWSTR TempWStr;
    LPSTR TempStr;
    WCHAR TempWChar;
    INT Flags;
    INT Width;
    INT Precision;
    INT Prefix;
    INT Type;
    INT Length;
    INT TempInt;
    int wctombResult;

    while (*Fmt)
    {
        if ((BufferPtr - Buffer) >= Count)
        {
            BufferRanOut = TRUE;
            break;
        }
        else if(*Fmt == '%' &&
                TRUE == Internal_ExtractFormatA(&Fmt, TempBuff, &Flags,
                                                &Width, &Precision,
                                                &Prefix, &Type))
        {
            if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_STRING)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                else if (PRECISION_INVALID == Precision)
                {
                    /* both a '*' and a number, ignore, but remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }
                TempWStr = va_arg(ap, LPWSTR);
                Length = WideCharToMultiByte(CP_ACP, 0, TempWStr, -1, 0,
                                             0, 0, 0);
                if (!Length)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                          GetLastError());
                    return -1;
                }
                TempStr = (LPSTR) malloc(Length);
                if (!TempStr)
                {
                    ERROR("malloc failed\n");
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return -1;
                }
                if (PRECISION_DOT == Precision)
                {
                    /* copy nothing */
                    *TempStr = 0;
                    Length = 0;
                }
                else if (Precision > 0 && Precision < Length - 1)
                {
                    Length = WideCharToMultiByte(CP_ACP, 0, TempWStr,
                                                 Precision, TempStr, Length,
                                                 0, 0);
                    if (!Length)
                    {
                        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                              GetLastError());
                        free(TempStr);
                        return -1;
                    }
                    TempStr[Length] = 0;
                    Length = Precision;
                }
                /* copy everything */
                else
                {
                    wctombResult = WideCharToMultiByte(CP_ACP, 0, TempWStr, -1,
                                                       TempStr, Length, 0, 0);
                    if (!wctombResult)
                    {
                        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                              GetLastError());
                        free(TempStr);
                        return -1;
                    }
                    --Length; /* exclude null char */
                }

                /* do the padding (if needed)*/
                BufferRanOut = !Internal_AddPaddingA(&BufferPtr,
                                                   Count - (BufferPtr - Buffer),
                                                   TempStr,
                                                   Width - Length,
                                                   Flags);

                free(TempStr);
            }
            else if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_CHAR)
            {
                CHAR TempBuffer[5];
                if (PRECISION_STAR == Precision ||
                    PRECISION_INVALID == Precision)
                {
                    /* ignore (because it's a char), and remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }

                TempWChar = va_arg(ap, int);
                Length = WideCharToMultiByte(CP_ACP, 0, &TempWChar, 1,
                                             TempBuffer, sizeof(TempBuffer),
                                             0, 0);
                if (!Length)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                          GetLastError());
                    return -1;
                }
                TempBuffer[Length] = 0;

                /* do the padding (if needed)*/
                BufferRanOut = !Internal_AddPaddingA(&BufferPtr,
                                                   Count - (BufferPtr - Buffer),
                                                   TempBuffer,
                                                   Width - Length,
                                                   Flags);

            }
            /* this places the number of bytes written to the buffer in the
               next arg */
            else if (Type == PFF_TYPE_N)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                if (Prefix == PFF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = BufferPtr - Buffer;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = BufferPtr - Buffer;
                }
            }
            else if (Type == PFF_TYPE_CHAR && (Flags & PFF_ZERO) != 0)
            {
                // Some versions of sprintf don't support 0-padded chars,
                // so we handle them here.
                char ch[2];

                ch[0] = (char) va_arg(ap, int);
                ch[1] = '\0';
                Length = 1;
                BufferRanOut = !Internal_AddPaddingA(&BufferPtr,
                                           Count - (BufferPtr - Buffer),
                                           ch,
                                           Width - Length,
                                           Flags);
            }
            else if (Type == PFF_TYPE_STRING && (Flags & PFF_ZERO) != 0)
            {
                // Some versions of sprintf don't support 0-padded strings,
                // so we handle them here.
                char *tempStr;

                tempStr = va_arg(ap, char *);
                Length = strlen(tempStr);
                BufferRanOut = !Internal_AddPaddingA(&BufferPtr,
                                           Count - (BufferPtr - Buffer),
                                           tempStr,
                                           Width - Length,
                                           Flags);
            }
            else
            {
                // Types that sprintf can handle
                size_t TempCount = Count - (BufferPtr - Buffer);

#if !HAVE_LARGE_SNPRINTF_SUPPORT
                // Limit TempCount to 0x40000000, which is sufficient
                // for platforms on which snprintf fails for very large
                // sizes.
                if (TempCount > 0x40000000)
                {
                    TempCount = 0x40000000;
                }
#endif  // HAVE_LARGE_SNPRINTF_SUPPORT

                TempInt = 0;
                // %h (short) doesn't seem to be handled properly by local sprintf,
                // so we do the truncation ourselves for some cases.
                if (Type == PFF_TYPE_P && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert from pointer -> int -> short to avoid warnings.
                    long trunc1;
                    short trunc2;

                    trunc1 = va_arg(ap, LONG);
                    trunc2 = (short) trunc1;
                    trunc1 = trunc2;

                    TempInt = snprintf(BufferPtr, TempCount, TempBuff, trunc1);
                }
                else if (Type == PFF_TYPE_INT && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert explicitly from int to short to get
                    // correct sign extension for shorts on all systems.
                    int n;
                    short s;

                    n = va_arg(ap, int);
                    s = (short) n;

                    TempInt = snprintf(BufferPtr, TempCount, TempBuff, s);
                }
                else
                {
                     TempInt = vsnprintf(BufferPtr, TempCount, TempBuff, ap);
                     PAL_printf_arg_remover(&ap, Precision, Type, Prefix);
                }

                if (TempInt > TempCount) /* buffer not long enough */
                {
                    BufferPtr += TempCount;
                    BufferRanOut = TRUE;
                }
                else
                {
                    BufferPtr += TempInt;
                }
            }
        }
        else
        {
            *BufferPtr++ = *Fmt++; /* copy regular chars into buffer */
        }
    }

    if (Count > (BufferPtr - Buffer))
    {
        *BufferPtr = 0; /* end the string */
    }

    if (BufferRanOut)
    {
        return -1;
    }
    else
    {

        return BufferPtr - Buffer;
    }
}


/*******************************************************************************
Function:
  PAL_wvsnprintf

  -- see PAL_vsnprintf above
*******************************************************************************/
static INT PAL_wvsnprintf(LPWSTR Buffer, INT Count, LPCWSTR Format, va_list ap)
{
    BOOL BufferRanOut = FALSE;
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    LPWSTR BufferPtr = Buffer;
    LPCWSTR Fmt = Format;
    LPWSTR TempWStr = NULL;
    LPWSTR WorkingWStr = NULL;
    WCHAR TempWChar[2];
    INT Flags;
    INT Width;
    INT Precision;
    INT Prefix;
    INT Type;
    INT Length;
    INT TempInt;
    LPSTR TempNumberBuffer;
    int mbtowcResult;

    while (*Fmt)
    {
        if ((BufferPtr - Buffer) >= Count)
        {
            BufferRanOut = TRUE;
            break;
        }
        else if(*Fmt == '%' &&
                TRUE == Internal_ExtractFormatW(&Fmt, TempBuff, &Flags,
                                                &Width, &Precision,
                                                &Prefix, &Type))
        {
            if (((Prefix == PFF_PREFIX_LONG || Prefix == PFF_PREFIX_LONG_W) &&
                (Type == PFF_TYPE_STRING || Type == PFF_TYPE_WSTRING)) ||
                (Prefix == PFF_PREFIX_SHORT && Type == PFF_TYPE_STRING) ||
                (Type == PFF_TYPE_WSTRING && (Flags & PFF_ZERO) != 0))
            {
                BOOL needToFree = FALSE;
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                else if (PRECISION_INVALID == Precision)
                {
                    /* both a '*' and a number, ignore, but remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }

                if ((Type == PFF_TYPE_STRING && Prefix == PFF_PREFIX_LONG) ||
                    Prefix == PFF_PREFIX_LONG_W)
                {
                    TempWStr = va_arg(ap, LPWSTR);
                }
                else
                {
                    // %lS and %hs assume an LPSTR argument.
                    LPSTR s = va_arg(ap, LPSTR );
                    UINT Length = 0;
                    Length = MultiByteToWideChar( CP_ACP, 0, s, -1, NULL, 0 );
                    if ( Length != 0 )
                    {
                        TempWStr =
                            (LPWSTR)malloc( (Length + 1 ) * sizeof( WCHAR ) );
                        if ( TempWStr )
                        {
                            needToFree = TRUE;
                            MultiByteToWideChar( CP_ACP, 0, s, -1,
                                                 TempWStr, Length );
                        }
                        else
                        {
                            ERROR( "malloc failed.\n" );
                            return -1;
                        }
                    }
                    else
                    {
                        ASSERT( "Unable to convert from multibyte "
                               " to wide char.\n" );
                        return -1;
                    }

                }

                Length = PAL_wcslen(TempWStr);
                WorkingWStr = (LPWSTR) malloc(sizeof(WCHAR) * (Length + 1));
                if (!WorkingWStr)
                {
                    ERROR("malloc failed\n");
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    if (needToFree)
                    {
                        free(TempWStr);
                    }
                    return -1;
                }
                if (PRECISION_DOT == Precision)
                {
                    // Copy nothing
                    *WorkingWStr = 0;
                    Length = 0;
                }
                else if (Precision > 0 && Precision < Length)
                {
                    lstrcpynW(WorkingWStr, TempWStr, Precision+1);
                    Length = Precision;
                }
                else
                {
                    // Copy everything
                    PAL_wcscpy(WorkingWStr, TempWStr);
                }

                // Add padding if needed.
                BufferRanOut = !Internal_AddPaddingW(&BufferPtr,
                                                   Count - (BufferPtr - Buffer),
                                                   WorkingWStr,
                                                   Width - Length,
                                                   Flags);

                if (needToFree)
                {
                    free(TempWStr);
                }
                free(WorkingWStr);
            }
            else if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_CHAR)
            {
                if (PRECISION_STAR == Precision ||
                    PRECISION_INVALID == Precision)
                {
                    /* ignore (because it's a char), and remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }

                TempWChar[0] = va_arg(ap, int);
                TempWChar[1] = 0;

                /* do the padding (if needed)*/
                BufferRanOut = !Internal_AddPaddingW(&BufferPtr,
                                                   Count - (BufferPtr - Buffer),
                                                   TempWChar,
                                                   Width - 1,
                                                   Flags);

            }
            /* this places the number of bytes written to the buffer in the
               next arg */
            else if (Type == PFF_TYPE_N)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                if (Prefix == PFF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = BufferPtr - Buffer;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = BufferPtr - Buffer;
                }
            }
            else
            {
                // Types that sprintf can handle

                /* note: I'm using the wide buffer as a (char *) buffer when I
                   pass it to sprintf().  After I get the buffer back I make a
                   backup of the chars copied and then convert them to wide
                   and place them in the buffer (BufferPtr) */
                size_t TempCount = Count - (BufferPtr - Buffer);
                TempInt = 0;

#if !HAVE_LARGE_SNPRINTF_SUPPORT
                // Limit TempCount to 0x40000000, which is sufficient
                // for platforms on which snprintf fails for very large
                // sizes.
                if (TempCount > 0x40000000)
                {
                    TempCount = 0x40000000;
                }
#endif  // HAVE_LARGE_SNPRINTF_SUPPORT

                // %h (short) doesn't seem to be handled properly by local sprintf,
                // so we do the truncation ourselves for some cases.
                if (Type == PFF_TYPE_P && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert from pointer -> int -> short to avoid warnings.
                    long trunc1;
                    short trunc2;

                    trunc1 = va_arg(ap, LONG);
                    trunc2 = (short)trunc1;
                    trunc1 = trunc2;

                    TempInt = snprintf((LPSTR)BufferPtr, TempCount, TempBuff, trunc1);
                }
                else if (Type == PFF_TYPE_INT && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert explicitly from int to short to get
                    // correct sign extension for shorts on all systems.
                    int n;
                    short s;

                    n = va_arg(ap, int);
                    s = (short) n;

                    TempInt = snprintf((LPSTR)BufferPtr, TempCount, TempBuff, s);
                }
                else
                {
                    TempInt = vsnprintf((LPSTR) BufferPtr, TempCount, TempBuff, ap);
                    PAL_printf_arg_remover(&ap, Precision, Type, Prefix);
                }

                if (TempInt == 0)
                {
                    // The argument is "".
                    continue;
                }
                if (TempInt > TempCount) /* buffer not long enough */
                {
                    TempNumberBuffer = (LPSTR) malloc(TempCount);
                    if (!TempNumberBuffer)
                    {
                        ERROR("malloc failed\n");
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                        return -1;
                    }
                    strncpy(TempNumberBuffer, (LPSTR) BufferPtr, TempCount);
                    mbtowcResult = MultiByteToWideChar(CP_ACP, 0,
                                                       TempNumberBuffer,
                                                       TempCount,
                                                       BufferPtr, TempCount);
                    if (!mbtowcResult)
                    {
                        ASSERT("MultiByteToWideChar failed.  Error is %d\n",
                              GetLastError());
                        free(TempNumberBuffer);
                        return -1;
                    }
                    BufferPtr += TempCount;
                    BufferRanOut = TRUE;
                }
                else
                {
                    TempNumberBuffer = (LPSTR) malloc(TempInt);
                    if (!TempNumberBuffer)
                    {
                        ERROR("malloc failed\n");
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                        return -1;
                    }
                    strncpy(TempNumberBuffer, (LPSTR) BufferPtr, TempInt);
                    mbtowcResult = MultiByteToWideChar(CP_ACP, 0,
                                                       TempNumberBuffer,
                                                       TempInt,
                                                       BufferPtr, TempInt);
                    if (!mbtowcResult)
                    {
                        ASSERT("MultiByteToWideChar failed.  Error is %d\n",
                              GetLastError());
                        free(TempNumberBuffer);
                        return -1;
                    }
                    BufferPtr += TempInt;
                }
                free(TempNumberBuffer);
            }
        }
        else
        {
            *BufferPtr++ = *Fmt++; /* copy regular chars into buffer */
        }
    }

    if (Count > (BufferPtr - Buffer))
    {
        *BufferPtr = 0; /* end the string */
    }

    if (BufferRanOut)
    {
        return -1;
    }
    else
    {
        return BufferPtr - Buffer;
    }
}

/*******************************************************************************
Function:
  Internal_AddPaddingVfprintf

Parameters:
  stream
    - file stream to place padding and given string (In)
  In
    - string to place into (Out) accompanied with padding
  Padding
    - number of padding chars to add
  Flags
    - padding style flags (PRINTF_FORMAT_FLAGS)
*******************************************************************************/
static INT Internal_AddPaddingVfprintf(PAL_FILE *stream, LPSTR In,
                                       INT Padding, INT Flags)
{
    LPSTR Out;
    INT LengthInStr;
    INT Length;
    LPSTR OutOriginal;
    INT Written;

    LengthInStr = strlen(In);
    Length = LengthInStr;


    if (Padding > 0)
    {
        Length += Padding;
    }
    Out = (LPSTR) malloc(Length+1);
    if (!Out)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return -1;
    }
    OutOriginal = Out;

    if (Flags & PFF_MINUS) /* pad on right */
    {
        strcpy(Out, In);
        Out += LengthInStr;
    }
    if (Padding > 0)
    {
        if (Flags & PFF_ZERO) /* '0', pad with zeros */
        {
            while (Padding--)
            {
                *Out++ = '0';
            }
        }
        else /* pad with spaces */
        {
            while (Padding--)
            {
                *Out++ = ' ';
            }
        }
    }
    if (!(Flags & PFF_MINUS)) /* put 'In' after padding */
    {
        strcpy(Out, In);
        Out += LengthInStr;
    }
    
    Written = fwrite(OutOriginal, 1, Length, stream->bsdFilePtr );
    if (-1 == Written)
    {
        ERROR("fwrite() failed with errno == %d\n", errno);
    }
    free(OutOriginal);

    return Written;
}

/*******************************************************************************
Function:
  PAL_vfprintf

Parameters:
  stream
    - out stream
  Format
    - format string
  ap
    - stdarg parameter list
*******************************************************************************/
int __cdecl PAL_vfprintf(PAL_FILE *stream, const char *format, va_list ap)
{
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    LPCSTR Fmt = format;
    LPWSTR TempWStr;
    LPSTR TempStr;
    WCHAR TempWChar;
    INT Flags;
    INT Width;
    INT Precision;
    INT Prefix;
    INT Type;
    INT Length;
    INT TempInt;
    int wctombResult;
    int written = 0;
    int paddingReturnValue;
	
    PERF_ENTRY(vfprintf);
    while (*Fmt)
    {
        if (*Fmt == '%' &&
            TRUE == Internal_ExtractFormatA(&Fmt, TempBuff, &Flags,
                                            &Width, &Precision,
                                            &Prefix, &Type))
        {
            if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_STRING)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                else if (PRECISION_INVALID == Precision)
                {
                    /* both a '*' and a number, ignore, but remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }
                TempWStr = va_arg(ap, LPWSTR);
                Length = WideCharToMultiByte(CP_ACP, 0, TempWStr, -1, 0,
                                             0, 0, 0);
                if (!Length)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                          GetLastError());
		    PERF_EXIT(vfprintf);
                    return -1;
                }
                TempStr = (LPSTR) malloc(Length);
                if (!TempStr)
                {
                    ERROR("malloc failed\n");
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		    PERF_EXIT(vfprintf);		
                    return -1;
                }
                if (PRECISION_DOT == Precision)
                {
                    /* copy nothing */
                    *TempStr = 0;
                    Length = 0;
                }
                else if (Precision > 0 && Precision < Length - 1)
                {
                    Length = WideCharToMultiByte(CP_ACP, 0, TempWStr,
                                                 Precision, TempStr, Length,
                                                 0, 0);
                    if (!Length)
                    {
                        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                              GetLastError());
                        free(TempStr);
		        PERF_EXIT(vfprintf);							
                        return -1;
                    }
                    TempStr[Length] = 0;
                    Length = Precision;
                }
                /* copy everything */
                else
                {
                    wctombResult = WideCharToMultiByte(CP_ACP, 0, TempWStr, -1,
                                                       TempStr, Length, 0, 0);
                    if (!wctombResult)
                    {
                        ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                              GetLastError());
                        free(TempStr);
		        PERF_EXIT(vfprintf);							
                        return -1;
                    }
                    --Length; /* exclude null char */
                }

                /* do the padding (if needed)*/
                paddingReturnValue =
                  Internal_AddPaddingVfprintf(stream, TempStr,
                                              Width - Length, Flags);
                if (-1 == paddingReturnValue)
                {
                    ERROR("Internal_AddPaddingVfprintf failed\n");
                    free(TempStr);
		    PERF_EXIT(vfprintf);						
                    return -1;
                }
                written += paddingReturnValue;

                free(TempStr);
            }
            else if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_CHAR)
            {
                CHAR TempBuffer[5];
                if (PRECISION_STAR == Precision ||
                    PRECISION_INVALID == Precision)
                {
                    /* ignore (because it's a char), and remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }
                
                TempWChar = va_arg(ap, int);
                Length = WideCharToMultiByte(CP_ACP, 0, &TempWChar, 1,
                                             TempBuffer, sizeof(TempBuffer),
                                             0, 0);
                if (!Length)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                          GetLastError());
		    PERF_EXIT(vfprintf);						
                    return -1;
                }
                TempBuffer[Length] = 0;

                /* do the padding (if needed)*/
                paddingReturnValue =
                  Internal_AddPaddingVfprintf(stream, TempBuffer,
                                              Width - Length, Flags);
                if (-1 == paddingReturnValue)
                {
                    ERROR("Internal_AddPaddingVfprintf failed\n");
		    PERF_EXIT(vfprintf);						
                    return -1;
                }
                written += paddingReturnValue;

            }
            /* this places the number of bytes written to the buffer in the
               next arg */
            else if (Type == PFF_TYPE_N)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                if (Prefix == PFF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = written;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = written;
                }
            }
            else if (Type == PFF_TYPE_CHAR && (Flags & PFF_ZERO) != 0)
            {
                // Some versions of fprintf don't support 0-padded chars,
                // so we handle them here.
                char ch[2];

                ch[0] = (char) va_arg(ap, int);
                ch[1] = '\0';
                Length = 1;
                paddingReturnValue = Internal_AddPaddingVfprintf(stream,
                                                ch,
                                                Width - Length,
                                                Flags);
                if (-1 == paddingReturnValue)
                {
                    ERROR("Internal_AddPaddingVfprintf failed\n");
		    PERF_EXIT(vfprintf);						
                    return -1;
                }
                written += paddingReturnValue;
            }
            else if (Type == PFF_TYPE_STRING && (Flags & PFF_ZERO) != 0)
            {
                // Some versions of fprintf don't support 0-padded strings,
                // so we handle them here.
                char *tempStr;

                tempStr = va_arg(ap, char *);
                Length = strlen(tempStr);
                paddingReturnValue = Internal_AddPaddingVfprintf(stream,
                                                tempStr,
                                                Width - Length,
                                                Flags);
                if (-1 == paddingReturnValue)
                {
                    ERROR("Internal_AddPaddingVfprintf failed\n");
		    PERF_EXIT(vfprintf);						
                    return -1;
                }
                written += paddingReturnValue;
            }
            else
            {
                // Types that fprintf can handle.
                TempInt = 0;

                // %h (short) doesn't seem to be handled properly by local sprintf,
                // so we do the truncation ourselves for some cases.
                if (Type == PFF_TYPE_P && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert from pointer -> int -> short to avoid warnings.
                    long trunc1;
                    short trunc2;

                    trunc1 = va_arg(ap, LONG);
                    trunc2 = (short)trunc1;
                    trunc1 = trunc2;

                    TempInt = fprintf( stream->bsdFilePtr, TempBuff, trunc1);
                }
                else if (Type == PFF_TYPE_INT && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert explicitly from int to short to get
                    // correct sign extension for shorts on all systems.
                    int n;
                    short s;

                    n = va_arg(ap, int);
                    s = (short) n;

                    TempInt = fprintf( stream->bsdFilePtr, TempBuff, s);
                }
                else
                {
                    TempInt = vfprintf( stream->bsdFilePtr, TempBuff, ap);
                    PAL_printf_arg_remover(&ap, Precision, Type, Prefix);
                }

                if (-1 == TempInt)
                {
                    ERROR("vfprintf returned an error\n");
                }
                else
                {
                    written += TempInt;
                }
            }
        }
        else
        {
            int ret;

            ret = fwrite(Fmt++, 1, 1, stream->bsdFilePtr); /* copy regular chars into buffer */
            if (-1 == ret)
            {
                ERROR("fwrite() failed with errno == %d\n", errno);
		PERF_EXIT(vfprintf);					
                return -1;
            }
            ++written;
        }
    }
	
    PERF_EXIT(vfprintf);
    return written;
}

/*++
Function:
  PAL_printf

See MSDN doc.
--*/
int
__cdecl
PAL_printf(
      const char *format,
      ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(printf);
    ENTRY("PAL_printf (format=%p (%s))\n", format, format);

    va_start(ap, format);
    Length = PAL_vprintf(format, ap);
    va_end(ap);

    LOGEXIT("PAL_printf returns int %d\n", Length);
    PERF_EXIT(printf);
    return Length;
}


/*++
Function:
  PAL_fprintf

See MSDN doc.
--*/
int
__cdecl
PAL_fprintf(PAL_FILE *stream,const char *format,...)
{
    LONG Length = 0;
    va_list ap;

    PERF_ENTRY(fprintf);
    ENTRY("PAL_fprintf(stream=%p,format=%p (%s))\n",stream, format, format);

    va_start(ap, format);
    Length = PAL_vfprintf( stream, format, ap);
    va_end(ap);

    LOGEXIT("PAL_fprintf returns int %d\n", Length);
    PERF_EXIT(fprintf);
    return Length;
}

/*++
Function:
  PAL_wprintf

See MSDN doc.
--*/
int
__cdecl
PAL_wprintf(
      const wchar_16 *format,
      ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(wprintf);
    ENTRY("PAL_wprintf (format=%p (%S))\n", format, format);

    va_start(ap, format);
    Length = PAL_vfwprintf( PAL_get_stdout(PAL_get_caller), format, ap);
    va_end(ap);

    LOGEXIT("PAL_wprintf returns int %d\n", Length);
    PERF_EXIT(wprintf);
    return Length;
}



/*++
Function:
  PAL_vprintf

See MSDN doc.
--*/
int
__cdecl
PAL_vprintf(
      const char *format,
      va_list ap)
{
    LONG Length;

    PERF_ENTRY(vprintf);
    ENTRY("PAL_vprintf (format=%p (%s))\n", format, format);

    Length = PAL_vfprintf( PAL_get_stdout(PAL_get_caller), format, ap);

    LOGEXIT("PAL_vprintf returns int %d\n", Length);
    PERF_EXIT(vprintf);
    return Length;
}


/*++
Function:
  wsprintfA

See MSDN doc.
--*/
int
PALAPIV
wsprintfA(
      OUT LPSTR buffer,
      IN LPCSTR format,
      ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(wsprintfA);
    ENTRY("wsprintfA (buffer=%p, format=%p (%s))\n", buffer, format, format);

    va_start(ap, format);
    Length = PAL_vsnprintf(buffer, 1024, format, ap);
    va_end(ap);

    LOGEXIT("wsprintfA returns int %d\n", Length);
    PERF_EXIT(wsprintfA);
    return Length;
}

/*++
Function:
  wsprintfW

See MSDN doc.
--*/
int
PALAPIV
wsprintfW(
      OUT LPWSTR buffer,
      IN LPCWSTR format,
      ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(wsprintfW);
    ENTRY("wsprintfW (buffer=%p, format=%p (%S))\n", buffer, format, format);

    va_start(ap, format);
    Length = PAL_wvsnprintf(buffer, 1024, format, ap);
    va_end(ap);

    LOGEXIT("wsprintfW returns int %d\n", Length);
    PERF_EXIT(wsprintfW);
    return Length;
}


/*++
Function:
  _snprintf

See MSDN doc.
--*/
int
__cdecl
_snprintf(
     char *buffer,
     size_t count,
     const char *format,
     ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(_snprintf);
    ENTRY("_snprintf (buffer=%p, count=%lu, format=%p (%s))\n",
          buffer, (unsigned long) count, format, format);

    va_start(ap, format);
    Length = PAL_vsnprintf(buffer, count, format, ap);
    va_end(ap);

    LOGEXIT("_snprintf returns int %d\n", Length);
    PERF_EXIT(_snprintf);
    return Length;
}


/*++
Function:
  _snwprintf

See MSDN doc.
--*/
int
__cdecl
_snwprintf(
     wchar_16 *buffer,
     size_t count,
     const wchar_16 *format,
     ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(_snwprintf);
    ENTRY("_snwprintf (buffer=%p, count=%lu, format=%p (%S))\n",
          buffer, (unsigned long) count, format, format);

    va_start(ap, format);
    Length = PAL_wvsnprintf(buffer, count, format, ap);
    va_end(ap);

    LOGEXIT("_snwprintf returns int %d\n", Length);
    PERF_EXIT(_snwprintf);
    return Length;
}


/*******************************************************************************
Function:
  Internal_AddPaddingVfwprintf

Parameters:
  stream
    - file stream to place padding and given string (In)
  In
    - string to place into (Out) accompanied with padding
  Padding
    - number of padding chars to add
  Flags
    - padding style flags (PRINTF_FORMAT_FLAGS)
*******************************************************************************/
static INT Internal_AddPaddingVfwprintf(PAL_FILE *stream, LPWSTR In,
                                       INT Padding, INT Flags,BOOL convert)
{
    LPWSTR Out;
    LPWSTR OutOriginal;
    INT LengthInStr;
    INT Length;
    INT Written = 0;

    LengthInStr = PAL_wcslen(In);
    Length = LengthInStr;


    if (Padding > 0)
    {
        Length += Padding;
    }
    Out = (LPWSTR) malloc((Length+1) * sizeof(wchar_16));
    if (!Out)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return -1;
    }
    OutOriginal = Out;

    if (Flags & PFF_MINUS) /* pad on right */
    {
        lstrcpyW(Out, In);
        Out += LengthInStr;
    }
    if (Padding > 0)
    {
        if (Flags & PFF_ZERO) /* '0', pad with zeros */
        {
            while (Padding--)
            {
                *Out++ = '0';
            }
        }
        else /* pad with spaces */
        {
            while (Padding--)
            {
                *Out++ = ' ';
            }
        }
    }
    if (!(Flags & PFF_MINUS)) /* put 'In' after padding */
    {
        lstrcpyW(Out, In);
        Out += LengthInStr;
    }

    if (Length > 0) {
        Written = Internal_Convertfwrite(OutOriginal, sizeof(wchar_16), Length, 
            (FILE*)(stream->bsdFilePtr), 
            convert);

        if (-1 == Written)
        {
            ERROR("fwrite() failed with errno == %d\n", errno);
        }
        free(OutOriginal);
    }

    return Written;
}


/*++
Function:
  fwprintf

See MSDN doc.
--*/
int
__cdecl
PAL_fwprintf(
     PAL_FILE *stream,
     const wchar_16 *format,
     ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(fwprintf);
    ENTRY("PAL_fwprintf (stream=%p, format=%p (%S))\n", stream, format, format);

    va_start(ap, format);
    Length = PAL_vfwprintf( stream, format, ap);
    va_end(ap);

    LOGEXIT("PAL_fwprintf returns int %d\n", Length);
    PERF_EXIT(fwprintf);
    return Length;
}

/*++
Function:
  PAL_vfwprintf

Parameters:
  stream
    - out stream
  Format
    - format string
  ap
    - stdarg parameter list
--*/
int
__cdecl
PAL_vfwprintf(
     PAL_FILE *stream,
     const wchar_16 *format,
     va_list ap)
{
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    LPCWSTR Fmt = format;
    LPWSTR TempWStr = NULL;
    LPWSTR WorkingWStr = NULL;
    WCHAR TempWChar[2];
    INT Flags;
    INT Width;
    INT Precision;
    INT Prefix;
    INT Type;
    INT Length=0;
    INT TempInt;
    BOOL WStrWasMalloced = FALSE;
    int mbtowcResult;
    int written=0;
    int paddingReturnValue;
    int ret;

    /* fwprintf for now in the PAL is always used on file opened
       in text mode. In those case the output should be ANSI not Unicode */
    BOOL textMode = TRUE;

    PERF_ENTRY(vfwprintf);
    ENTRY("vfwprintf (stream=%p, format=%p (%S))\n",
          stream, format, format);

    while (*Fmt)
    {
        if(*Fmt == '%' &&
                TRUE == Internal_ExtractFormatW(&Fmt, TempBuff, &Flags,
                                                &Width, &Precision,
                                                &Prefix, &Type))
        {
            if (((Prefix == PFF_PREFIX_LONG || Prefix == PFF_PREFIX_LONG_W) &&
                 (Type == PFF_TYPE_STRING || Type == PFF_TYPE_WSTRING)) ||
                 (Type == PFF_TYPE_WSTRING && (Flags & PFF_ZERO) != 0))
            {
                WStrWasMalloced = FALSE;
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                else if (PRECISION_INVALID == Precision)
                {
                    /* both a '*' and a number, ignore, but remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }

                if ( Type == PFF_TYPE_STRING || Prefix == PFF_PREFIX_LONG_W)
                {
                    TempWStr = va_arg(ap, LPWSTR);
                }
                else
                {
                    /* %lS assumes a LPSTR argument. */
                    LPSTR s = va_arg(ap, LPSTR );
                    UINT Length = 0;
                    Length = MultiByteToWideChar( CP_ACP, 0, s, -1, NULL, 0 );
                    if ( Length != 0 )
                    {
                        TempWStr =
                            (LPWSTR)malloc( (Length) * sizeof( WCHAR ) );
                        if ( TempWStr )
                        {
                            WStrWasMalloced = TRUE;
                            MultiByteToWideChar( CP_ACP, 0, s, -1,
                                                 TempWStr, Length );
                        }
                        else
                        {
                            ERROR( "malloc failed.\n" );
                            LOGEXIT("vfwprintf returns int -1\n");
                            PERF_EXIT(vfwprintf);
                            return -1;
                        }
                    }
                    else
                    {
                        ASSERT( "Unable to convert from multibyte "
                               " to wide char.\n" );
                        LOGEXIT("vfwprintf returns int -1\n");
                        PERF_EXIT(vfwprintf);
                        return -1;
                    }

                }

                Length = PAL_wcslen(TempWStr);
                WorkingWStr = (LPWSTR) malloc(sizeof(WCHAR) * (Length + 1));
                if (!WorkingWStr)
                {
                    ERROR("malloc failed\n");
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    if (WStrWasMalloced)
                    {
                        free(TempWStr);
                    }
                    return -1;
                }
                if (PRECISION_DOT == Precision)
                {
                    /* copy nothing */
                    *WorkingWStr = 0;
                    Length = 0;
                }
                else if (Precision > 0 && Precision < Length)
                {
                    lstrcpynW(WorkingWStr, TempWStr, Precision+1);
                    Length = Precision;
                }
                /* copy everything */
                else
                {
                    PAL_wcscpy(WorkingWStr, TempWStr);
                }


                /* do the padding (if needed)*/
                paddingReturnValue =
                    Internal_AddPaddingVfwprintf( stream, WorkingWStr,
                                                 Width - Length,
                                                 Flags,textMode);

                if (paddingReturnValue == -1)
                {
                    ERROR("Internal_AddPaddingVfwprintf failed\n");
                    if (WStrWasMalloced)
                    {
                        free(TempWStr);
                    }
                    free(WorkingWStr);
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    return (-1);
                }
                written += paddingReturnValue;

                free(WorkingWStr);
                if (WStrWasMalloced)
                {
                    free(TempWStr);
                }
            }
            else if (Prefix == PFF_PREFIX_LONG && Type == PFF_TYPE_CHAR)
            {
                if (PRECISION_STAR == Precision ||
                    PRECISION_INVALID == Precision)
                {
                    /* ignore (because it's a char), and remove arg */
                    TempInt = va_arg(ap, INT); /* value not used */
                }

                TempWChar[0] = va_arg(ap, int);
                TempWChar[1] = 0;

               /* do the padding (if needed)*/
                paddingReturnValue =
                    Internal_AddPaddingVfwprintf(stream, TempWChar,
                                                 Width - 1,
                                                 Flags,textMode);
                if (paddingReturnValue == -1)
                {
                    ERROR("Internal_AddPaddingVfwprintf failed\n");
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    return(-1);
                }
                written += paddingReturnValue;
            }
            /* this places the number of bytes written to the buffer in the
               next arg */
            else if (Type == PFF_TYPE_N)
            {
                if (PRECISION_STAR == Precision)
                {
                    Precision = va_arg(ap, INT);
                }
                if (Prefix == PFF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = written;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = written;
                }
            }
            else
            {
                // Types that sprintf can handle.

                /* note: I'm using the wide buffer as a (char *) buffer when I
                   pass it to sprintf().  After I get the buffer back I make a
                   backup of the chars copied and then convert them to wide
                   and place them in the buffer (BufferPtr) */

                // This argument will be limited to 1024 characters.
                // It should be enough.
                size_t TEMP_COUNT = 1024;
                char TempSprintfStrBuffer[1024];
                char *TempSprintfStrPtr = NULL;
                char *TempSprintfStr = TempSprintfStrBuffer;
                LPWSTR TempWideBuffer;

                TempInt = 0;
                // %h (short) doesn't seem to be handled properly by local sprintf,
                // so we do the truncation ourselves for some cases.
                if (Type == PFF_TYPE_P && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert from pointer -> int -> short to avoid warnings.
                    long trunc1;
                    short trunc2;

                    trunc1 = va_arg(ap, LONG);
                    trunc2 = (short)trunc1;
                    trunc1 = trunc2;
                    
                    TempInt = snprintf(TempSprintfStr, TEMP_COUNT, TempBuff, trunc1);
                    
                    if (TempInt >= TEMP_COUNT)
                    {
                        if (NULL == (TempSprintfStrPtr = (char*)malloc(++TempInt)))
                        {
                            ERROR("malloc failed\n");
                            LOGEXIT("vfwprintf returns int -1\n");
                            PERF_EXIT(vfwprintf);
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            return -1;
                        }
                        
                        TempSprintfStr = TempSprintfStrPtr;
                        snprintf(TempSprintfStr, TempInt, TempBuff, trunc2);
                    }
                }
                else if (Type == PFF_TYPE_INT && Prefix == PFF_PREFIX_SHORT)
                {
                    // Convert explicitly from int to short to get
                    // correct sign extension for shorts on all systems.
                    int n;
                    short s;

                    n = va_arg(ap, int);
                    s = (short) n;

                    TempInt = snprintf(TempSprintfStr, TEMP_COUNT, TempBuff, s);

                    if (TempInt >= TEMP_COUNT)
                    {
                        if (NULL == (TempSprintfStrPtr = (char*)malloc(++TempInt)))
                        {
                            ERROR("malloc failed\n");
                            LOGEXIT("vfwprintf returns int -1\n");
                            PERF_EXIT(vfwprintf);
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            return -1;
                        }
                        
                        TempSprintfStr = TempSprintfStrPtr;
                        snprintf(TempSprintfStr, TempInt, TempBuff, s);
                    }
                }
                else
                {
                    TempInt = vsnprintf(TempSprintfStr, TEMP_COUNT, TempBuff, ap);

                    if (TempInt >= TEMP_COUNT)
                    {
                        if (NULL == (TempSprintfStrPtr = (char*)malloc(++TempInt)))
                        {
                            ERROR("malloc failed\n");
                            LOGEXIT("vfwprintf returns int -1\n");
                            PERF_EXIT(vfwprintf);
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            return -1;
                        }
                        
                        TempSprintfStr = TempSprintfStrPtr;
                        vsnprintf(TempSprintfStr, TempInt, TempBuff, ap);
                    }
                    
                    PAL_printf_arg_remover(&ap, Precision, Type, Prefix);
                }

                mbtowcResult = MultiByteToWideChar(CP_ACP, 0,
                                                   TempSprintfStr, -1,
                                                   NULL, 0);
 
                if (mbtowcResult == 0)
                {
                    ERROR("WideCharToMultiByte failed\n");
                    if(TempSprintfStrPtr)
                    {
                        free(TempSprintfStrPtr);
                    }
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    return -1;
                }

                TempWideBuffer = (LPWSTR) malloc(mbtowcResult*sizeof(WCHAR));
                if (!TempWideBuffer)
                {
                    ERROR("malloc failed\n");
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    if(TempSprintfStrPtr)
                    {
                        free(TempSprintfStrPtr);
                    }
                    return -1;
                }

                MultiByteToWideChar(CP_ACP, 0, TempSprintfStr, -1,
                                    TempWideBuffer, mbtowcResult);
 
                ret = Internal_Convertfwrite(TempWideBuffer, sizeof(wchar_16), 
                                             mbtowcResult-1, 
                                             (FILE*)stream->bsdFilePtr,
                                             textMode);

                if (-1 == ret)
                {
                    ERROR("fwrite() failed with errno == %d (%s)\n", errno, strerror(errno));
                    LOGEXIT("vfwprintf returns int -1\n");
                    PERF_EXIT(vfwprintf);
                    free(TempWideBuffer);
                    if(TempSprintfStrPtr)
                    {
                        free(TempSprintfStrPtr);
                    }
                    return -1;
                }
                if(TempSprintfStrPtr)
                {
                    free(TempSprintfStrPtr);
                }
                free(TempWideBuffer);
            }
        }
        else
        {
            int ret;
            ret = Internal_Convertfwrite(Fmt++, sizeof(wchar_16), 1, 
                                         (FILE*)stream->bsdFilePtr,
                                         textMode); /* copy regular chars into buffer */
            if (-1 == ret)
            {
                ERROR("fwrite() failed with errno == %d\n", errno);
                LOGEXIT("vfwprintf returns int -1\n");
                PERF_EXIT(vfwprintf);
                return -1;
            }
            ++written;
       }
    }

    LOGEXIT("vfwprintf returns int %d\n", written);
    PERF_EXIT(vfwprintf);
    return (written);
}


/*******************************************************************************
Function:
  Internal_ScanfExtractFormatA

Paramaters:
  Fmt
    - format string to parse
    - first character must be a '%'
    - paramater gets updated to point to the character after
      the %<foo> format string
  Out
    - buffer will contain the %<foo> format string
  Store
    - boolean value representing whether to store the type to be parsed
    - '*' flag
  Width
    - will contain the width specified by the format string
    - -1 if none given
  Prefix
    - an enumeration of the type prefix
  Type
    - an enumeration of the value type to be parsed

Notes:
  - I'm also handling the undocumented %ws, %wc, %w...
*******************************************************************************/
static BOOL Internal_ScanfExtractFormatA(LPCSTR *Fmt, LPSTR Out, LPBOOL Store,
                                         LPINT Width, LPINT Prefix, LPINT Type)
{
    BOOL Result = FALSE;
    LPSTR TempStr;
    LPSTR TempStrPtr;

    *Width = -1;
    *Store = TRUE;
    *Prefix = -1;
    *Type = -1;

    if (*Fmt && **Fmt == '%')
    {
        *Out++ = *(*Fmt)++;
    }
    else
    {
        return Result;
    }

    /* we'll never need a temp string longer than the original */
    TempStrPtr = TempStr = (LPSTR) malloc(strlen(*Fmt)+1);
    if (!TempStr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    /* parse '*' flag which means don't store */
    if (**Fmt == '*')
    {
        *Store = FALSE;
        *Out++ = *(*Fmt)++;
    }

    /* grab width specifier */
    if (isdigit((unsigned char) **Fmt))
    {
        TempStrPtr = TempStr;
        while (isdigit((unsigned char) **Fmt))
        {
            *TempStrPtr++ = **Fmt;
            *Out++ = *(*Fmt)++;
        }
        *TempStrPtr = 0; /* end string */
        *Width = atol(TempStr);
    }

    /* grab prefix of 'I64' for __int64 */
    if ((*Fmt)[0] == 'I' && (*Fmt)[1] == '6' && (*Fmt)[2] == '4')
    {
        /* convert to 'q'/'ll' so BSD's sscanf can handle it */
        *Fmt += 3;
        *Prefix = SCANF_PREFIX_LONGLONG;
    }
    /* grab a prefix of 'h' */
    else if (**Fmt == 'h')
    {
        *Prefix = SCANF_PREFIX_SHORT;
        ++(*Fmt);
    }
    /* grab prefix of 'l' or the undocumented 'w' (at least in MSDN) */
    else if (**Fmt == 'l' || **Fmt == 'w')
    {
        *Prefix = SCANF_PREFIX_LONG;
        ++(*Fmt);
    }
    else if (**Fmt == 'L')
    {
        /* a prefix of 'L' seems to be ignored */
        ++(*Fmt);
    }

    /* grab type 'c' */
    if (**Fmt == 'c' || **Fmt == 'C')
    {
        *Type = SCANF_TYPE_CHAR;
        if (*Prefix != SCANF_PREFIX_SHORT && **Fmt == 'C')
        {
            *Prefix = SCANF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 'c';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab type 's' */
    else if (**Fmt == 's' || **Fmt == 'S')
    {
        *Type = SCANF_TYPE_STRING;
        if (*Prefix != SCANF_PREFIX_SHORT && **Fmt == 'S')
        {
            *Prefix = SCANF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 's';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab int types types */
    else if (**Fmt == 'd' || **Fmt == 'i' || **Fmt == 'o' ||
             **Fmt == 'u' || **Fmt == 'x' || **Fmt == 'X' ||
             **Fmt == 'p')
    {
        *Type = SCANF_TYPE_INT;
        if (*Prefix == SCANF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        else if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        else if (*Prefix == SCANF_PREFIX_LONGLONG)
        {
            strcpy(Out, scanf_longlongfmt);
            Out += strlen(scanf_longlongfmt);
        }
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'e' || **Fmt == 'E' || **Fmt == 'f' ||
             **Fmt == 'g' || **Fmt == 'G')
    {
        /* we can safely ignore the prefixes and only add the type*/
        *Type = SCANF_TYPE_FLOAT;
        /* this gets rid of %E/%G since they're they're the
           same when scanning */
        *Out++ = tolower( *(*Fmt)++ );
        Result = TRUE;
    }
    else if (**Fmt == 'n')
    {
        if (*Prefix == SCANF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        *Out++ = *(*Fmt)++;
        *Type = SCANF_TYPE_N;
        Result = TRUE;
    }
    else if (**Fmt == '[')
    {
        /* There is a small compatibility problem in the handling of the []
           option in FreeBSD vs. Windows.  In Windows, you can have [z-a]
           as well as [a-z].  In FreeBSD, [z-a] fails.  So, we need to 
           reverse the instances of z-a to a-z (and [m-e] to [e-m], etc). */

        /* step 1 : copy the leading [ */
        *Out++ = '[';
        (*Fmt)++;

        /* step 2 : copy a leading ^, if present */
        if( '^' == **Fmt )
        {
            *Out++ = '^';
            (*Fmt)++;
        }

        /* step 3 : copy a leading ], if present; a ] immediately after the 
           leading [ (or [^) does *not* end the sequence, it is part of the 
           characters to match */
        if( ']' == **Fmt )
        {
            *Out++ = ']';
            (*Fmt)++;
        }

        /* step 4 : if the next character is already a '-', it's not part of an 
           interval specifier, so just copy it */
        if('-' == **Fmt )
        {
            *Out++ = '-';
            (*Fmt)++;
        }

        /* ok then, process the rest of it */
        while( '\0' != **Fmt ) 
        {
            if(']' == **Fmt)
            {
                /* ']' marks end of the format specifier; we're done */
                *Out++ = ']';
                (*Fmt)++;
                break;
            }
            if('-' == **Fmt)
            {
                if( ']' == (*Fmt)[1] )
                {
                    /* got a '-', next character is the terminating ']'; 
                       copy '-' literally */
                    *Out++ = '-';
                    (*Fmt)++;
                }
                else
                {
                    /* got a '-' indicating an interval specifier */
                    unsigned char prev, next;

                    /* get the interval boundaries */
                    prev = (*Fmt)[-1];
                    next = (*Fmt)[1];

                    /* if boundaries were inverted, replace the already-copied 
                       low boundary by the 'real' low boundary */
                    if( prev > next )
                    {
                        Out[-1] = next;

                        /* ...and save the 'real' upper boundary, which will be 
                           copied to 'Out' below */
                        next = prev;
                    }

                    *Out++ = '-';
                    *Out++ = next;

                    /* skip over the '-' and the next character, which we 
                       already copied */
                    (*Fmt)+=2;
                }
            }
            else
            {
                /* plain character; just copy it */
                *Out++ = **Fmt;
                (*Fmt)++;
            }            
        }

        *Type = SCANF_TYPE_BRACKETS;
        Result = TRUE;
    }
    else if (**Fmt == ' ')
    {
        *Type = SCANF_TYPE_SPACE;
    }
    
    /* add %n so we know how far to increment the pointer */
    *Out++ = '%';
    *Out++ = 'n';

    *Out = 0;  /* end the string */
    free(TempStr);
    return Result;
}

/*******************************************************************************
Function:
  Internal_ScanfExtractFormatW

  -- see Internal_ScanfExtractFormatA above
*******************************************************************************/
static BOOL Internal_ScanfExtractFormatW(LPCWSTR *Fmt, LPSTR Out, LPBOOL Store,
                                         LPINT Width, LPINT Prefix, LPINT Type)
{
    BOOL Result = FALSE;
    LPSTR TempStr;
    LPSTR TempStrPtr;

    *Width = -1;
    *Store = TRUE;
    *Prefix = -1;
    *Type = -1;

    if (*Fmt && **Fmt == '%')
    {
        *Out++ = *(*Fmt)++;
    }
    else
    {
        return Result;
    }

    /* we'll never need a temp string longer than the original */
    TempStrPtr = TempStr = (LPSTR) malloc(PAL_wcslen(*Fmt)+1);
    if (!TempStr)
    {
        ERROR("malloc failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    /* parse '*' flag which means don't store */
    if (**Fmt == '*')
    {
        *Store = FALSE;
        *Out++ = *(*Fmt)++;
    }

    /* grab width specifier */
    if (isdigit(**Fmt))
    {
        TempStrPtr = TempStr;
        while (isdigit(**Fmt))
        {
            *TempStrPtr++ = **Fmt;
            *Out++ = *(*Fmt)++;
        }
        *TempStrPtr = 0; /* end string */
        *Width = atol(TempStr);
    }

    /* grab prefix of 'I64' for __int64 */
    if ((*Fmt)[0] == 'I' && (*Fmt)[1] == '6' && (*Fmt)[2] == '4')
    {
        /* convert to 'q'/'ll' so BSD's sscanf can handle it */
        *Fmt += 3;
        *Prefix = SCANF_PREFIX_LONGLONG;
    }
    /* grab a prefix of 'h' */
    else if (**Fmt == 'h')
    {
        *Prefix = SCANF_PREFIX_SHORT;
        ++(*Fmt);
    }
    /* grab prefix of 'l' or the undocumented 'w' (at least in MSDN) */
    else if (**Fmt == 'l' || **Fmt == 'w')
    {
        *Prefix = SCANF_PREFIX_LONG;
        ++(*Fmt);
    }
    else if (**Fmt == 'L')
    {
        /* a prefix of 'L' seems to be ignored */
        ++(*Fmt);
    }

    /* grab type 'c' */
    if (**Fmt == 'c' || **Fmt == 'C')
    {
        *Type = SCANF_TYPE_CHAR;
        if (*Prefix != SCANF_PREFIX_SHORT && **Fmt == 'c')
        {
            *Prefix = SCANF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 'c';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab type 's' */
    else if (**Fmt == 's' || **Fmt == 'S')
    {
        *Type = SCANF_TYPE_STRING;
        if (*Prefix != SCANF_PREFIX_SHORT && **Fmt == 's')
        {
            *Prefix = SCANF_PREFIX_LONG; /* give it a wide prefix */
        }
        if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        *Out++ = 's';
        ++(*Fmt);
        Result = TRUE;
    }
    /* grab int types types */
    else if (**Fmt == 'd' || **Fmt == 'i' || **Fmt == 'o' ||
             **Fmt == 'u' || **Fmt == 'x' || **Fmt == 'X' ||
             **Fmt == 'p')
    {
        *Type = SCANF_TYPE_INT;
        if (*Prefix == SCANF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        else if (*Prefix == SCANF_PREFIX_LONG)
        {
            *Out++ = 'l';
        }
        else if (*Prefix == SCANF_PREFIX_LONGLONG)
        {
            strcpy(Out, scanf_longlongfmt);
            Out += strlen(scanf_longlongfmt);
        }
        *Out++ = *(*Fmt)++;
        Result = TRUE;
    }
    else if (**Fmt == 'e' || **Fmt == 'E' || **Fmt == 'f' ||
             **Fmt == 'g' || **Fmt == 'G')
    {
        /* we can safely ignore the prefixes and only add the type*/
        *Type = SCANF_TYPE_FLOAT;
        /* this gets rid of %E/%G since they're they're the
           same when scanning */
        *Out++ = tolower( *(*Fmt)++ );
        Result = TRUE;
    }
    else if (**Fmt == 'n')
    {
        if (*Prefix == SCANF_PREFIX_SHORT)
        {
            *Out++ = 'h';
        }
        *Out++ = *(*Fmt)++;
        *Type = SCANF_TYPE_N;
        Result = TRUE;
    }
    else if (**Fmt == '[')
    {
        /* There is a small compatibility problem in the handling of the []
           option in FreeBSD vs. Windows.  In Windows, you can have [z-a]
           as well as [a-z].  In FreeBSD, [z-a] fails.  So, we need to 
           reverse the instances of z-a to a-z (and [m-e] to [e-m], etc). */

        /* step 1 : copy the leading [ */
        *Out++ = '[';
        (*Fmt)++;

        /* step 2 : copy a leading ^, if present */
        if( '^' == **Fmt )
        {
            *Out++ = '^';
            (*Fmt)++;
        }

        /* step 3 : copy a leading ], if present; a ] immediately after the 
           leading [ (or [^) does *not* end the sequence, it is part of the 
           characters to match */
        if( ']' == **Fmt )
        {
            *Out++ = ']';
            (*Fmt)++;
        }

        /* step 4 : if the next character is already a '-', it's not part of an 
           interval specifier, so just copy it */
        if('-' == **Fmt )
        {
            *Out++ = '-';
            (*Fmt)++;
        }

        /* ok then, process the rest of it */
        while( '\0' != **Fmt ) 
        {
            if(']' == **Fmt)
            {
                /* ']' marks end of the format specifier; we're done */
                *Out++ = ']';
                (*Fmt)++;
                break;
            }
            if('-' == **Fmt)
            {
                if( ']' == (*Fmt)[1] )
                {
                    /* got a '-', next character is the terminating ']'; 
                       copy '-' literally */
                    *Out++ = '-';
                    (*Fmt)++;
                }
                else
                {
                    /* got a '-' indicating an interval specifier */
                    unsigned char prev, next;

                    /* get the interval boundaries */
                    prev = (*Fmt)[-1];
                    next = (*Fmt)[1];

                    /* if boundaries were inverted, replace the already-copied 
                       low boundary by the 'real' low boundary */
                    if( prev > next )
                    {
                        Out[-1] = next;

                        /* ...and save the 'real' upper boundary, which will be 
                           copied to 'Out' below */
                        next = prev;
                    }

                    *Out++ = '-';
                    *Out++ = next;

                    /* skip over the '-' and the next character, which we 
                       already copied */
                    (*Fmt)+=2;
                }
            }
            else
            {
                /* plain character; just copy it */
                *Out++ = **Fmt;
                (*Fmt)++;
            }            
        }

        *Type = SCANF_TYPE_BRACKETS;
        Result = TRUE;
    }
    else if (**Fmt == ' ')
    {
        *Type = SCANF_TYPE_SPACE;
    }

    /* add %n so we know how far to increment the pointer */
    *Out++ = '%';
    *Out++ = 'n';

    *Out = 0;  /* end the string */
    free(TempStr);
    return Result;
}

/*******************************************************************************
Function:
  PAL_vsscanf

Parameters:
  Buffer
    - buffer to parse values from
  Format
    - format string
  ap
    - stdarg parameter list
*******************************************************************************/
int PAL_vsscanf(LPCSTR Buffer, LPCSTR Format, va_list ap)
{
    INT Length = 0;
    LPCSTR Buff = Buffer;
    LPCSTR Fmt = Format;
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    BOOL Store;
    INT Width;
    INT Prefix;
    INT Type = -1;

    while (*Fmt)
    {
        if (!*Buff && Length == 0)
        {
            Length = EOF;
            break;
        }
        /* remove any number of blanks */
        else if (isspace((unsigned char) *Fmt))
        {
            while (isspace((unsigned char) *Buff))
            {
                ++Buff;
            }
            ++Fmt;
        }
        else if (*Fmt == '%' &&
                 Internal_ScanfExtractFormatA(&Fmt, TempBuff, &Store,
                                              &Width, &Prefix, &Type))
        {
            if (Prefix == SCANF_PREFIX_LONG &&
                (Type == SCANF_TYPE_STRING || Type == SCANF_TYPE_CHAR))
            {
                int len = 0;
                int res;
                WCHAR *charPtr = 0;

                /* a single character */
                if (Type == SCANF_TYPE_CHAR && Width == -1)
                {
                    len = Width = 1;
                }

                /* calculate length of string to copy */
                while (Buff[len] && !isspace((unsigned char) Buff[len]))
                {
                    if (Width != -1 && len >= Width)
                    {
                        break;
                    }
                    ++len;
                }

                if (Store)
                {
                    charPtr = va_arg(ap, WCHAR *);

                    res = MultiByteToWideChar(CP_ACP, 0, Buff, len,
                                            charPtr, len);
                    if (!res)
                    {
                        ASSERT("MultiByteToWideChar failed.  Error is %d\n",
                            GetLastError());
                        return -1;
                    }
                    if (Type == SCANF_TYPE_STRING)
                    {
                        /* end string */
                        charPtr[res] = 0;
                    }
                    ++Length;
                }
                Buff += len;
            }
            /* this places the number of bytes stored into the next arg */
            else if (Type == SCANF_TYPE_N)
            {
                if (Prefix == SCANF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = Buff - Buffer;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = Buff - Buffer;
                }
            }
            /* types that sscanf can handle */
            else
            {
                int ret;
                int n;
                LPVOID voidPtr = NULL;

                if (Store)
                {
                    voidPtr = va_arg(ap, LPVOID);
                    ret = sscanf(Buff, TempBuff, voidPtr, &n);
                }
                else
                {
                    ret = sscanf(Buff, TempBuff, &n);
                }

#if SSCANF_CANNOT_HANDLE_MISSING_EXPONENT
                if ((ret == 0) && (Type == SCANF_TYPE_FLOAT))
                {
                    ret = SscanfFloatEBugWorkAround (Buff, TempBuff, voidPtr, &n);
                }
#endif // SSCANF_CANNOT_HANDLE_MISSING_EXPONENT

                if (ret > 0)
                {
                    Length += ret;
                }
                else
                {
                    /* no match, break scan */
                    break;
                }
                Buff += n;
            }
        }
        else
        {
            /* grab, but not store */
            if (*Fmt == *Buff && Type != SCANF_TYPE_SPACE)
            {
                ++Fmt;
                ++Buff;
            }
            /* doesn't match, break scan */
            else
            {
                break;
            }
        }
    }

    return Length;
}

/*******************************************************************************
Function:
  PAL_wvsscanf

  -- see PAL_vsscanf above
*******************************************************************************/
int PAL_wvsscanf(LPCWSTR Buffer, LPCWSTR Format, va_list ap)
{
    INT Length = 0;
    LPCWSTR Buff = Buffer;
    LPCWSTR Fmt = Format;
    CHAR TempBuff[1024]; /* used to hold a single %<foo> format string */
    BOOL Store;
    INT Width;
    INT Prefix;
    INT Type = -1;

    while (*Fmt)
    {
        if (!*Buff && Length == 0)
        {
            Length = EOF;
            break;
        }
        /* remove any number of blanks */
        else if (isspace(*Fmt))
        {
            while (isspace(*Buff))
            {
                ++Buff;
            }
            ++Fmt;
        }
        else if (*Fmt == '%' &&
                 Internal_ScanfExtractFormatW(&Fmt, TempBuff, &Store,
                                              &Width, &Prefix, &Type))
        {
            if (Prefix == SCANF_PREFIX_LONG &&
                (Type == SCANF_TYPE_STRING || Type == SCANF_TYPE_CHAR))
            {
                int len = 0;
                WCHAR *charPtr = 0;

                /* a single character */
                if (Type == SCANF_TYPE_CHAR && Width == -1)
                {
                    len = Width = 1;
                }

                /* calculate length of string to copy */
                while (Buff[len] && !isspace(Buff[len]))
                {
                    if (Width != -1 && len >= Width)
                    {
                        break;
                    }
                    ++len;
                }

                if (Store)
                {
                    int i;
                    charPtr = va_arg(ap, WCHAR *);

                    for (i = 0; i < len; i++)
                    {
                        charPtr[i] = Buff[i];
                    }
                    if (Type == SCANF_TYPE_STRING)
                    {
                        /* end string */
                        charPtr[len] = 0;
                    }
                    ++Length;
                }
                Buff += len;
            }
            /* this places the number of bytes stored into the next arg */
            else if (Type == SCANF_TYPE_N)
            {
                if (Prefix == SCANF_PREFIX_SHORT)
                {
                    *(va_arg(ap, short *)) = Buff - Buffer;
                }
                else
                {
                    *(va_arg(ap, LPLONG)) = Buff - Buffer;
                }
            }
            /* types that sscanf can handle */
            else
            {
                int ret;
                int n;
                int size;
                LPSTR newBuff = 0;
                LPVOID voidPtr = NULL;

                size = WideCharToMultiByte(CP_ACP, 0, Buff, -1, 0, 0, 0, 0);
                if (!size)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                        GetLastError());
                    return -1;
                }
                newBuff = (LPSTR) malloc(size);
                if (!newBuff)
                {
                    ERROR("malloc failed\n");
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return -1;
                }
                size = WideCharToMultiByte(CP_ACP, 0, Buff, size,
                                           newBuff, size, 0, 0);
                if (!size)
                {
                    ASSERT("WideCharToMultiByte failed.  Error is %d\n",
                        GetLastError());
                    free(newBuff);    
                    return -1;
                }

                if (Store)
                {
                    if (Type == SCANF_TYPE_BRACKETS)
                    {
                        WCHAR *strPtr;
                        int i;

                        /* add a '*' to %[] --> %*[]  */
                        i = strlen(TempBuff) + 1;
                        while (i)
                        {
                            /* shift everything right one */
                            TempBuff[i] = TempBuff[i - 1];
                            --i;
                        }
                        TempBuff[0] = '%';
                        TempBuff[1] = '*';

                        /* %n doesn't count as a conversion. Since we're 
                           suppressing conversion of the %[], sscanf will 
                           always return 0, so we can't use the return value 
                           to determine success. Set n to 0 before the call; if 
                           it's still 0 afterwards, we know the call failed */
                        n = 0;
                        sscanf(newBuff, TempBuff, &n);
                        if(0 == n)
                        {
                            /* sscanf failed, nothing matched. set ret to 0, 
                               so we know we have to break */
                            ret = 0;
                        }
                        else
                        {
                            strPtr = va_arg(ap, WCHAR *);
                            for (i = 0; i < n; i++)
                            {
                                strPtr[i] = Buff[i];
                            }
                            strPtr[n] = 0; /* end string */
                            ret = 1;
                        }
                    }
                    else
                    {
                        voidPtr = va_arg(ap, LPVOID);
                        ret = sscanf(newBuff, TempBuff, voidPtr, &n);
                    }
                }
                else
                {
                    ret = sscanf(newBuff, TempBuff, &n);
                }

#if SSCANF_CANNOT_HANDLE_MISSING_EXPONENT
                if ((ret == 0) && (Type == SCANF_TYPE_FLOAT))
                {
                    ret = SscanfFloatEBugWorkAround (newBuff, TempBuff, voidPtr, &n);
                }
#endif // SSCANF_CANNOT_HANDLE_MISSING_EXPONENT
                
                free(newBuff);
                if (ret > 0)
                {
                    Length += ret;
                }
                else
                {
                    /* no match; break scan */
                    break;
                }
                Buff += n;
            }
       }
        else
        {
            /* grab, but not store */
            if (*Fmt == *Buff && Type != SCANF_TYPE_SPACE)
            {
                ++Fmt;
                ++Buff;
            }
            /* doesn't match, break scan */
            else
            {
                break;
            }
        }
    }

    return Length;
}

/*++
Function:
  PAL_sscanf

See MSDN doc.
--*/
int
__cdecl
PAL_sscanf(
           const char *buffer,
           const char *format,
           ...)
{
    int Length;
    va_list ap;

    PERF_ENTRY(sscanf);
    ENTRY("PAL_sscanf (buffer=%p (%s), format=%p (%s))\n", buffer, buffer, format, format);

    va_start(ap, format);
    Length = PAL_vsscanf(buffer, format, ap);
    va_end(ap);

    LOGEXIT("PAL_sscanf returns int %d\n", Length);
    PERF_EXIT(sscanf);
    return Length;
}

/*++
Function:
  PAL_sprintf

See MSDN doc.
--*/
int
__cdecl
PAL_sprintf(
          char *buffer,
          const char *format,
          ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(sprintf);
    ENTRY("PAL_sprintf (buffer=%p, format=%p (%s))\n", buffer, format, format);

    va_start(ap, format);
    Length = PAL_vsnprintf(buffer, 0x7fffffff, format, ap);
    va_end(ap);

    LOGEXIT("PAL_sprintf returns int %d\n", Length);
    PERF_EXIT(sprintf);
    return Length;
}


/*++
Function:
  PAL_swprintf

See MSDN doc.
--*/
int
__cdecl
PAL_swprintf(
          wchar_16 *buffer,
          const wchar_16 *format,
          ...)
{
    LONG Length;
    va_list ap;

    PERF_ENTRY(swprintf);
    ENTRY("PAL_swprintf (buffer=%p, format=%p (%S))\n", buffer, format, format);

    va_start(ap, format);
    Length = PAL_wvsnprintf(buffer, 0x7fffffff, format, ap);
    va_end(ap);

    LOGEXIT("PAL_swprintf returns int %d\n", Length);
    PERF_EXIT(swprintf);
    return Length;
}

/*++
Function:
  PAL_swscanf

See MSDN doc.
--*/
int
__cdecl
PAL_swscanf(
          const wchar_16 *buffer,
          const wchar_16 *format,
          ...)
{
    int Length;
    va_list ap;

    PERF_ENTRY(swscanf);
    ENTRY("PAL_swscanf (buffer=%p (%S), format=%p (%S))\n", buffer, buffer, format, format);

    va_start(ap, format);
    Length = PAL_wvsscanf(buffer, format, ap);
    va_end(ap);

    LOGEXIT("PAL_swscanf returns int %d\n", Length);
    PERF_EXIT(swscanf);
    return Length;
}


/*++
Function:
  PAL_vsprintf

See MSDN doc.
--*/
int 
__cdecl 
PAL_vsprintf(char *buffer, 
         const char *format, 
         va_list argptr)
{
    LONG Length;

    PERF_ENTRY(vsprintf);
    ENTRY("PAL_vsprintf (buffer=%p, format=%p (%s), argptr=%p)\n", 
          buffer, format, format, argptr);

    Length = PAL_vsnprintf(buffer, 0x7fffffff, format, argptr);

    LOGEXIT("PAL_vsprintf returns int %d\n", Length);
    PERF_EXIT(vsprintf);

    return Length;
}


/*++
Function:
  _vsnprintf

See MSDN doc.
--*/
int 
__cdecl 
_vsnprintf(char *buffer, 
           size_t count, 
           const char *format, 
           va_list argptr)
{
    LONG Length;

    PERF_ENTRY(_vsnprintf);
    ENTRY("_vsnprintf (buffer=%p, count=%d, format=%p (%s), argptr=%p)\n", 
          buffer, count, format, format, argptr);

    Length = PAL_vsnprintf(buffer, count, format, argptr);

    LOGEXIT("_vsnprintf returns int %d\n", Length);
    PERF_EXIT(_vsnprintf);

    return Length;
}



/*++
Function:
  PAL_vswprintf

See MSDN doc.
--*/
int 
__cdecl 
PAL_vswprintf(wchar_16 *buffer, 
              const wchar_16 *format, 
              va_list argptr)
{
    LONG Length;

    PERF_ENTRY(vswprintf);
    ENTRY("PAL_vswprintf (buffer=%p, format=%p (%S), argptr=%p)\n", 
          buffer, format, format, argptr);

    Length = PAL_wvsnprintf(buffer, 0x7fffffff, format, argptr);

    LOGEXIT("PAL_vswprintf returns int %d\n", Length);
    PERF_EXIT(vswprintf);

    return Length;
}


/*++
Function:
  _vsnwprintf

See MSDN doc.
--*/
int 
__cdecl 
_vsnwprintf(wchar_16 *buffer, 
            size_t count, 
            const wchar_16 *format, 
            va_list argptr)
{
    LONG Length;

    PERF_ENTRY(_vsnwprintf);
    ENTRY("_vsnwprintf (buffer=%p, count=%lu, format=%p (%S), argptr=%p)\n", 
          buffer, (unsigned long) count, format, format, argptr);

    Length = PAL_wvsnprintf(buffer, count, format, argptr);

    LOGEXIT("_vsnwprintf returns int %d\n", Length);
    PERF_EXIT(_vsnwprintf);

    return Length;
}

#if SSCANF_CANNOT_HANDLE_MISSING_EXPONENT
/*++
Function:
  SscanfFloatEBugWorkAround

  Parameters:
  buff:     pointer to the buffer to be parsed; the target float must be at 
            the beginning of the buffer, except for any number of leading 
            spaces
  floatFmt: must be "%e%n" (or "%f%n" or "%g%n")
  voidptr:  optional pointer to output variable (which should be a float)
  pn:       pointer to an int to receive the number of bytes parsed.
  
  Notes:
  On some platforms (specifically AIX) sscanf fails to parse a float from 
  a string such as 12.34e (while it succeeds for e.g. 12.34a). Sscanf 
  initially interprets the 'e' as the keyword for the beginning of a 
  10-exponent of a floating point in scientific notation (as in 12.34e5), 
  but then it fails to parse the actual exponent. At this point sscanf should 
  be able to fall back on the narrower pattern, and parse the floating point 
  in common decimal notation (i.e. 12.34). However AIX's sscanf fails to do 
  so and it does not parse any number.
  This function works around the bug removing the 'e' before parsing the 
  float.

--*/

static int SscanfFloatEBugWorkAround (LPCSTR buff, LPCSTR floatFmt,
                                      void * voidPtr, int * pn)
{
    int ret = 0;
    int digits = 0;
    int points = 0;
    LPCSTR pos = buff;

    /* skip initial spaces */
    while (*pos && isspace(*pos))
        pos++;

    /* go to the end of a float, if there is one */
    while (*pos)
    {
        if (isdigit(*pos))
            digits++;
        else if (*pos == '.')
        {
            if (++points > 1)
                break;
        }
        else
            break;

        pos++;
    }

    /* check if it is something like 12.34e and the trailing 'e' is not 
       the suffix of a valid exponent of 10, such as 12.34e+5 */
    if ( digits > 0 && *pos && tolower(*pos) == 'e' &&
         !( *(pos+1) && 
            ( isdigit(*(pos+1)) || 
              ( (*(pos+1) == '+' || *(pos+1) == '-') && isdigit(*(pos+2)) )
                )
             )
        )
    {
        CHAR * pLocBuf = (CHAR *)malloc((pos-buff+1)*sizeof(CHAR));
        if (pLocBuf)
        {
            memcpy(pLocBuf, buff, (pos-buff)*sizeof(CHAR));
            pLocBuf[pos-buff] = 0;                              
            if (voidPtr)
                ret = sscanf(pLocBuf, floatFmt, voidPtr, pn);
            else
                ret = sscanf(pLocBuf, floatFmt, pn);
            free (pLocBuf);
        }
    }
    return ret;
}
#endif // SSCANF_CANNOT_HANDLE_MISSING_EXPONENT

