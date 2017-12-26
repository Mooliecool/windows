/*=====================================================================
**
** Source:  test2.c
**
** Purpose:  Check to see that fputs fails and returns EOF when called on
**           a closed file stream and a read-only file stream.
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
**===================================================================*/
                                 
#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    
    FILE* TheFile;
    char* StringOne = "FooBar";
    int ret;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Create a file with read/write access */

    TheFile = fopen("TestFile", "w+");
  
    if(TheFile == NULL)
    {
        Fail("ERROR: fopen failed to open the file 'TestFile' in read/write "
             "mode.\n");
    }

    /* Then close that file we just opened */

    if(fclose(TheFile) != 0)
    {
        Fail("ERROR: fclose failed to close the file.\n");
    }

    /* Check that calling fputs on this closed file stream fails. */

    if((ret = fputs(StringOne, TheFile)) >= 0)
    {
        Fail("ERROR: fputs should have failed to write to a closed "
             "file stream, but it didn't return a negative value.\n");
    }

    if(ret != EOF)
    {
        Fail("ERROR: fputs should have returned EOF on an error, but instead "
              "returned %d.\n",ret);
    }

    /* Open a file as Readonly */

    TheFile = fopen("TestFile", "r");
  
    if(TheFile == NULL)
    {
        Fail("ERROR: fopen failed to open the file 'TestFile' in read/write "
             "mode.\n");
    }

    /* Check that fputs fails when trying to write to a read-only stream */

    if((ret = fputs(StringOne, TheFile)) >= 0)
    {
        Fail("ERROR: fputs should have failed to write to a read-only "
             "file stream, but it didn't return a negative value.\n");
    }
    
    if(ret != EOF)
    {
        Fail("ERROR: fputs should have returned EOF when writing to a "
             "read-only filestream, but instead  "
             "returned %d.\n",ret);
    }
    
    PAL_Terminate();
    return PASS;
}
