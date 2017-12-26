/*============================================================================
**
** Source:      test5.c (fprintf)
**
** Purpose:     Tests the count specifier (%n).
**              This test is modeled after the fprintf series.
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

#include <palsuite.h>
#include "../fprintf.h"

/* 
 * Depends on memcmp, strlen, fopen, fseek and fgets.
 */

static void DoTest(char *formatstr, int param, char *checkstr)
{
    FILE *fp;
    char buf[256] = { 0 };
    int n = -1;
    
    if ((fp = fopen("testfile.txt", "w+")) == NULL )
    {
        Fail("ERROR: fopen failed to create testfile\n");
    }

    if ((fprintf(fp, formatstr, &n)) < 0)
    {
        Fail("ERROR: fprintf failed\n");
    }

    if ((fseek(fp, 0, SEEK_SET)) != 0)
    {
        Fail("ERROR: fseek failed\n");
    }

    if ((fgets(buf, 100, fp)) == NULL)
    {
        Fail("ERROR: fseek failed\n");
    }
    
    if (n != param)
    {
        Fail("ERROR: Expected count parameter to resolve to %d, got %X\n",
 param, n);
    }

    if (memcmp(buf, checkstr, strlen(buf) + 1) != 0)
    {
        Fail("ERROR: Expected \"%s\" got \"%s\".\n", checkstr, buf);
    }
    

    if ((fclose( fp )) != 0)

    {

        Fail("ERROR: fclose failed to close \"testfile.txt\"\n");

    }
}

static void DoShortTest(char *formatstr, int param, char *checkstr)
{
    FILE *fp;
    char buf[256] = { 0 };
    short int n = -1;
    
    if ((fp = fopen("testfile.txt", "w+")) == NULL )
    {
        Fail("ERROR: fopen failed to create testfile\n");
    }

    if ((fprintf(fp, formatstr, &n)) < 0)
    {
        Fail("ERROR: fprintf failed\n");
    }

    if ((fseek(fp, 0, SEEK_SET)) != 0)
    {
        Fail("ERROR: fseek failed\n");
    }

    if ((fgets(buf, 100, fp)) == NULL)
    {
        Fail("ERROR: fseek failed\n");
    }
    
    if (n != param)
    {
        Fail("ERROR: Expected count parameter to resolve to %d, got %X\n",
 param, n);
    }

    if (memcmp(buf, checkstr, strlen(buf) + 1) != 0)
    {
        Fail("ERROR: Expected \"%s\" got \"%s\".\n", checkstr, buf);
    }

    if ((fclose( fp )) != 0)
    {
        Fail("ERROR: fclose failed to close \"testfile.txt\"\n");
    }
}

int __cdecl main(int argc, char *argv[])
{    
    if (PAL_Initialize(argc, argv) != 0)
        return(FAIL);

    DoTest("foo %n bar", 4, "foo  bar");
    DoTest("foo %#n bar", 4, "foo  bar");
    DoTest("foo % n bar", 4, "foo  bar");
    DoTest("foo %+n bar", 4, "foo  bar");
    DoTest("foo %-n bar", 4, "foo  bar");
    DoTest("foo %0n bar", 4, "foo  bar");
    DoShortTest("foo %hn bar", 4, "foo  bar");
    DoTest("foo %ln bar", 4, "foo  bar");
    DoTest("foo %Ln bar", 4, "foo  bar");
    DoTest("foo %I64n bar", 4, "foo  bar");
    DoTest("foo %20.3n bar", 4, "foo  bar");

    PAL_Terminate();
    return PASS;
}
