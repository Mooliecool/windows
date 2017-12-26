/*=====================================================================
**
** Source:  test2.c
**
** Purpose: Open a read-only file and attempt to write some data to it.
** Check to ensure that an ferror occurs.
**
** Depends:
**     fopen
**     fwrite
**     fclose
**     
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
    const char filename[] = "testfile";
    FILE * fp = NULL;
    int result;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Open a file in READONLY mode */
  
    if((fp = fopen(filename, "r")) == NULL)
    {
        Fail("Unable to open a file for reading.");
    }

    /* Attempt to write 14 characters to the file. */
  
    if((result = fwrite("This is a test",1,14,fp)) != 0)
    {
        Fail("ERROR: %d characters written.  0 characters should "
             "have been written, since this file is read-only.", result);
    }
  
    if(ferror(fp) == 0)
    {
        Fail("ERROR:  ferror should have generated an error when "
             "write was called on a read-only file.  But, it "
             "retured 0, indicating no error.\n");
    }
  
    /* Close the file. */

    if(fclose(fp) != 0)
    {
        Fail("ERROR: fclose failed when trying to close a file pointer. "
	     "This test depends on fclose working properly.");
    }

   
    PAL_Terminate();
    return PASS;
}
   

