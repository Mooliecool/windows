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
// ===========================================================================
// File: crtstuff.cpp
// 
// ===========================================================================

/*** 
*
*Purpose:
*  Misc C Runtime style helper functions.
*
*****************************************************************************/


#include "rotor_palrt.h"
#include "oautil.h"
#include "crtstuff.h"

void * _stdcall _lfind (
    const void *key,
    const void *base,
    unsigned int *num,
    unsigned int width,
    int (__cdecl *compare)(const void *, const void *))
{
    unsigned int place = 0;
    while (place < *num )
        if (!(*compare)(key,base))
            return( (void *)base );
        else
        {
            base = (char *)base + width;
            place++;
        }
    return( NULL );
}

#ifdef __cplusplus
extern "C" {
#endif

/* helper routine that does the main job. */

INTERNAL_(void)
xtoa(unsigned long val, OLECHAR FAR* buf, int is_neg)
{
    OLECHAR FAR* p;	/* pointer to traverse string */
    OLECHAR FAR* firstdig; /* pointer to first digit */
    OLECHAR temp;		/* temp char */
    unsigned digval;	/* value of digit */

    p = buf;

    if (is_neg) {
	/* negative, so output '-' and negate */
	*p++ = OASTR('-');
	val = (unsigned long)-(long)val;
    }

    firstdig = p;		/* save pointer to first digit */

    do {
	digval = (unsigned) (val % 10);
	val /= 10;	/* get next digit */

	/* convert to ascii and store */
	*p++ = (OLECHAR) (digval + OASTR('0'));      /* a digit */
    } while (val > 0);

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

    *p-- = OASTR('\0');		/* terminate string; p points to last digit */

    do {
	temp = *p;
	*p = *firstdig;
	*firstdig = temp;	/* swap *p and *firstdig */
	--p;
	++firstdig;		/* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */
}


/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

INTERNAL_(OLECHAR FAR*)
disp_itoa(int val, OLECHAR FAR* buf)
{
    if (val < 0)
      xtoa((unsigned long)val, buf, 1);
    else
      xtoa((unsigned long)(unsigned int)val, buf, 0);
    return buf;
}

INTERNAL_(OLECHAR FAR*)
disp_ltoa(long val, OLECHAR FAR* buf)
{
    xtoa((unsigned long)val, buf, (val < 0));
    return buf;
}

INTERNAL_(OLECHAR FAR*)
disp_ultoa(unsigned long val, OLECHAR FAR* buf)
{
    xtoa(val, buf, FALSE);
    return buf;
}

INTERNAL_(void)
disp_gcvt(double dblIn, int ndigits, OLECHAR FAR* pchOut, int bufSize)
{
      char buf[40];

      _gcvt(dblIn, ndigits, buf);
      MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, -1, pchOut, bufSize);
}

INTERNAL_(void)
x64toa(ULONG64 val, OLECHAR FAR* buf, int is_neg)
{
    OLECHAR FAR* p;	/* pointer to traverse string */
    OLECHAR FAR* firstdig; /* pointer to first digit */
    OLECHAR temp;		/* temp char */
    unsigned digval;	/* value of digit */

    p = buf;

    if (is_neg) {
	/* negative, so output '-' and negate */
	*p++ = OASTR('-');
	val = (unsigned long)-(long)val;
    }

    firstdig = p;		/* save pointer to first digit */

    do {
	digval = (unsigned) (val % 10);
	val /= 10;	/* get next digit */

	/* convert to ascii and store */
	*p++ = (OLECHAR) (digval + OASTR('0'));      /* a digit */
    } while (val > 0);

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

    *p-- = OASTR('\0');		/* terminate string; p points to last digit */

    do {
	temp = *p;
	*p = *firstdig;
	*firstdig = temp;	/* swap *p and *firstdig */
	--p;
	++firstdig;		/* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */
}

INTERNAL_(OLECHAR FAR*)
disp_i64toa(LONG64 val, OLECHAR FAR* buf)
{
    x64toa(val, buf, (val < 0));
    return buf;
}

INTERNAL_(OLECHAR FAR*)
disp_ui64toa(ULONG64 val, OLECHAR FAR* buf)
{
    x64toa(val, buf, FALSE);
    return buf;
}

#ifdef __cplusplus
}
#endif
