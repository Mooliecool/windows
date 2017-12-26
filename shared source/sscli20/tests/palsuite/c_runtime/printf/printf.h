/*============================================================================
**
** Source:  printf.h
**
** Purpose: Containts common testing functions for printf
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

#ifndef __printf_H__
#define __printf_H__

void DoStrTest(char *formatstr, char* param, char *checkstr)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }
}

void DoWStrTest(char *formatstr, WCHAR* param, char *checkstr)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }
}

void DoPointerTest(char *formatstr, void* param, char* paramstr, 
                   char *checkstr1)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr1))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr1), ret);
    }
}

void DoCountTest(char *formatstr, int param, char *checkstr)
{
    int ret;
    int n = -1;
    
    ret = printf(formatstr, &n);

    if (n != param)
    {
        Fail("Expected count parameter to resolve to %d, got %d\n", param, n);
    }

    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }    
}

void DoShortCountTest(char *formatstr, int param, char *checkstr)
{
    int ret;
    short int n = -1;
    
    ret = printf(formatstr, &n);

    if (n != param)
    {
        Fail("Expected count parameter to resolve to %d, got %d\n", param, n);
    }

    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }    
}


void DoCharTest(char *formatstr, char param, char *checkstr)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }
}

void DoWCharTest(char *formatstr, WCHAR param, char *checkstr)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }    
}

void DoNumTest(char *formatstr, int param, char *checkstr)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr), ret);
    }    
}

void DoI64Test(char *formatstr, INT64 param, char *valuestr, 
               char *checkstr1)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr1))
    {
        Fail("Expected printf to return %d, got %d.\n", 
            strlen(checkstr1), ret);
    }
}

void DoDoubleTest(char *formatstr, double param, 
                  char *checkstr1, char *checkstr2)
{
    int ret;

    ret = printf(formatstr, param);
    if (ret != strlen(checkstr1) && ret != strlen(checkstr2))
    {
        Fail("Expected printf to return %d or %d, got %d.\n", 
            strlen(checkstr1), strlen(checkstr2), ret);
    }
}

void DoArgumentPrecTest(char *formatstr, int precision, void *param, 
                        char *paramstr, char *checkstr1, char *checkstr2)
{
    int ret;

    ret = printf(formatstr, precision, param);
    if (ret != strlen(checkstr1) && ret != strlen(checkstr2))
    {
        Fail("Expected printf to return %d or %d, got %d.\n", 
            strlen(checkstr1), strlen(checkstr2), ret);
    }
}

void DoArgumentPrecDoubleTest(char *formatstr, int precision, double param, 
    char *checkstr1, char *checkstr2)
{
    int ret;

    ret = printf(formatstr, precision, param);
    if (ret != strlen(checkstr1) && ret != strlen(checkstr2))
    {
        Fail("Expected printf to return %d or %d, got %d.\n", 
            strlen(checkstr1), strlen(checkstr2), ret);
    }
}

#endif

