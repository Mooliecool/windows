/*============================================================================
**
** Source:  test3.c
**
** Purpose: Tries to read from an empty file using fgets(), to verify
**          handling of EOF condition.
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

int __cdecl main(int argc, char **argv)
{
    char inBuf[10];
    const char filename[] = "testfile.tmp";

    FILE * fp;
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /*write the empty file that we will use to test */
    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        Fail("Unable to open file for write.\n");
    }

    /*Don't write anything*/
  
    if (fclose(fp) != 0) 
    {
        Fail("Error closing stream opened for write.\n");
    }


    /*Open the file and try to read.*/
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        Fail("Unable to open file for read.\n");
    }

  
    if (fgets(inBuf, sizeof(inBuf) , fp) != NULL)
    {
        /*NULL could also mean an error condition, but since the PAL
          doesn't supply feof or ferror, we can't distinguish between
          the two.*/
        Fail("fgets doesn't handle EOF properly.  When asked to read from "
             "an empty file, it didn't return NULL as it should have.\n");
    }

    if (fclose(fp) != 0)
    {
        Fail("Error closing an empty file after trying to use fgets().\n");
    }
    PAL_Terminate();
    return PASS;

}

  



