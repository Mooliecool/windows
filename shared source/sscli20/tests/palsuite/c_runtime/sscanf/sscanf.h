/*============================================================================
**
** Source:  sscanf.h
**
** Purpose: Contains common testing functions for sscanf.h
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

#ifndef __SSCANF_H__
#define __SSCANF_H__

void DoVoidTest(char *inputstr, char *formatstr)
{
    char buf[256] = { 0 };
    int i;
    int ret;

    ret = sscanf(inputstr, formatstr, buf);
    if (ret != 0)
    {
        Fail("ERROR: Expected sscanf to return 0, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    for (i=0; i<256; i++)
    {
        if (buf[i] != 0)
        {
            Fail("ERROR: Parameter unexpectedly modified scanning \"%s\" "
                "using \"%s\".\n", inputstr, formatstr);
        }
    }

}

void DoStrTest(char *inputstr, char *formatstr, char *checkstr)
{
    char buf[256] = { 0 };
    int ret;

    ret = sscanf(inputstr, formatstr, buf);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (memcmp(checkstr, buf, strlen(checkstr) + 1) != 0)
    {
        Fail("ERROR: scanned string incorrectly from \"%s\" using \"%s\".\n"
            "Expected \"%s\", got \"%s\".\n", inputstr, formatstr, checkstr, 
            buf);
    }

}

void DoWStrTest(char *inputstr, char *formatstr, WCHAR *checkstr)
{
    WCHAR buf[256] = { 0 };
    int ret;

    ret = sscanf(inputstr, formatstr, buf);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (memcmp(checkstr, buf, wcslen(checkstr)*2 + 2) != 0)
    {
        Fail("ERROR: scanned wide string incorrectly from \"%s\" using \"%s\".\n"
            "Expected \"%s\", got \"%s\".\n", inputstr, formatstr, 
            convertC(checkstr), convertC(buf));
    }

}

void DoNumTest(char *inputstr, char *formatstr, int checknum)
{
    int num;
    int ret;

    ret = sscanf(inputstr, formatstr, &num);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (checknum != num)
    {
        Fail("ERROR: scanned number incorrectly from \"%s\" using \"%s\".\n"
            "Expected %d, got %d.\n", inputstr, formatstr, checknum, num);
    }
}

void DoShortNumTest(char *inputstr, char *formatstr, short checknum)
{
    short num;
    int ret;

    ret = sscanf(inputstr, formatstr, &num);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (checknum != num)
    {
        Fail("ERROR: scanned number incorrectly from \"%s\" using \"%s\".\n"
            "Expected %hd, got %hd.\n", inputstr, formatstr, checknum, num);
    }
}

void DoI64NumTest(char *inputstr, char *formatstr, INT64 checknum)
{
    char buf[256];
    char check[256];
    INT64 num;
    int ret;

    ret = sscanf(inputstr, formatstr, &num);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (checknum != num)
    {
        sprintf(buf, "%I64d", num);
        sprintf(check, "%I64d", checknum);
        Fail("ERROR: scanned I64 number incorrectly from \"%s\" using \"%s\".\n"
            "Expected %s, got %s.\n", inputstr, formatstr, check, buf);
    }
}

void DoCharTest(char *inputstr, char *formatstr, char* checkchars, int numchars)
{
    char buf[256];
    int ret;
    int i;

    for (i=0; i<256; i++)
        buf[i] = (char)-1;

    ret = sscanf(inputstr, formatstr, buf);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (memcmp(buf, checkchars, numchars) != 0)
    {
        buf[numchars] = 0;

        Fail("ERROR: scanned character(s) incorrectly from \"%s\" using \"%s\".\n"
            "Expected %s, got %s.\n", inputstr, formatstr, checkchars, 
            buf);
    }

    if (buf[numchars] != (char)-1)
    {
        Fail("ERROR: overflow occured in scanning character(s) from \"%s\" "
            "using \"%s\".\nExpected %d character(s)\n", inputstr, formatstr, 
            numchars);
    }
}

void DoWCharTest(char *inputstr, char *formatstr, WCHAR* checkchars, int numchars)
{
    WCHAR buf[256];
    int ret;
    int i;

    for (i=0; i<256; i++)
        buf[i] = (WCHAR)-1;

    ret = sscanf(inputstr, formatstr, buf);
    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (memcmp(buf, checkchars, numchars) != 0)
    {
        buf[numchars] = 0;

        Fail("ERROR: scanned wide character(s) incorrectly from \"%s\" using \"%s\".\n"
            "Expected %s, got %s.\n", inputstr, formatstr, convertC(checkchars), 
            convertC(buf));
    }

    if (buf[numchars] != (WCHAR)-1)
    {
        Fail("ERROR: overflow occured in scanning wide character(s) from \"%s\" "
            "using \"%s\".\nExpected %d character(s)\n", inputstr, formatstr, 
            numchars);
    }
}


void DoFloatTest(char *inputstr, char *formatstr, float checkval)
{
    char buf[256] = { 0 };
    float val;
    int ret;
    int i;

    for (i=0; i<256; i++)
        buf[i] = (char)-1;

    ret = sscanf(inputstr, formatstr, buf);
    val = *(float*)buf;

    if (ret != 1)
    {
        Fail("ERROR: Expected sscanf to return 1, got %d.\n"
            "Using \"%s\" in \"%s\".\n", ret, inputstr, formatstr);
    }

    if (val != checkval)
    {
        Fail("ERROR: scanned float incorrectly from \"%s\" using \"%s\".\n"
            "Expected \"%f\", got \"%f\".\n", inputstr, formatstr, checkval, 
            val);
    }

    if (buf[4] != (char)-1)
    {
        Fail("ERROR: overflow occured in scanning float from \"%s\" "
            "using \"%s\".\n", inputstr, formatstr);

    }
}


#endif
