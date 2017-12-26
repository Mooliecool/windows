/*============================================================================
**
** Source:  vswprintf.h
**
** Purpose: Containts common testing functions for vswprintf
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**==========================================================================*/

#ifndef __vswprintf_H__
#define __vswprintf_H__

/* These functions leaks memory like crazy. C'est la vie. */
int testvswp(wchar_t* buf, const wchar_t* format, ...)
{
	int retVal = 0;
	va_list arglist;

	va_start(arglist, format);
	retVal = vswprintf(buf, format, arglist);
	va_end(arglist);

	return( retVal);
}

void DoWStrTest(WCHAR *formatstr, WCHAR *param, WCHAR *checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, param);

    if (memcmp(buf, checkstr, wcslen(buf) * 2 + 2) != 0)
    {
        Fail("ERROR: failed to insert wide string \"%s\" into \"%s\".\n"
            "Expected \"%s\", got \"%s\".\n", 
            convertC(param), convertC(formatstr), 
            convertC(checkstr), convertC(buf));
    }
}

void DoStrTest(WCHAR *formatstr, char *param, WCHAR *checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, param);

    if (memcmp(buf, checkstr, wcslen(buf) * 2 + 2) != 0)
    {
        Fail("ERROR: failed to insert wide string \"%s\" into \"%s\".\n"
            "Expected \"%s\", got \"%s\".\n", 
            param, convertC(formatstr), convertC(checkstr), 
            convertC(buf));
    }
}

void DoCharTest(WCHAR *formatstr, char param, WCHAR *checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, param);
    if (memcmp(buf, checkstr, wcslen(buf)*2 + 2) != 0)
    {
        Fail("ERROR: failed to insert char \'%c\' (%d) into \"%s\"\n"
            "Expected \"%s\" got \"%s\".\n", 
            param, param, convertC(formatstr), convertC(checkstr), 
            convertC(buf));
    }    
}

void DoWCharTest(WCHAR *formatstr, WCHAR param, WCHAR *checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, param);
    if (memcmp(buf, checkstr, wcslen(buf)*2 + 2) != 0)
    {
        Fail("ERROR: failed to insert wide char \'%c\' (%d) into \"%s\"\n"
            "Expected \"%s\" got \"%s\".\n", 
            (char) param, param, convertC(formatstr), convertC(checkstr), 
            convertC(buf));
    }    
}

void DoNumTest(WCHAR *formatstr, int value, WCHAR*checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, value);
    if (memcmp(buf, checkstr, wcslen(buf)* 2 + 2) != 0)
    {
        Fail("ERROR: failed to insert %#x into \"%s\"\n"
            "Expected \"%s\" got \"%s\".\n", value, convertC(formatstr), 
            convertC(checkstr), convertC(buf));
    }    
}

void DoI64NumTest(WCHAR *formatstr, INT64 value, char *valuestr, WCHAR*checkstr)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, value);
    if (memcmp(buf, checkstr, wcslen(buf)* 2 + 2) != 0)
    {
        Fail("ERROR: failed to insert %s into \"%s\"\n"
            "Expected \"%s\" got \"%s\".\n", valuestr, convertC(formatstr), 
            convertC(checkstr), convertC(buf));
    }    
}
void DoDoubleTest(WCHAR *formatstr, double value, WCHAR *checkstr1, WCHAR
 *checkstr2)
{
    WCHAR buf[256] = { 0 };

    testvswp(buf, formatstr, value);
    if (memcmp(buf, checkstr1, wcslen(checkstr1) + 2) != 0 &&
        memcmp(buf, checkstr2, wcslen(checkstr2) + 2) != 0)
    {
        Fail("ERROR: failed to insert %f into \"%s\"\n"
            "Expected \"%s\" or \"%s\", got \"%s\".\n",
                value,
                convertC(formatstr),
                convertC(checkstr1),
                convertC(checkstr2),
                convertC(buf));
    }
}

#endif
