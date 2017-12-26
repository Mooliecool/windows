/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the feof function.
**          Open a file, and read some characters.  Check that
**          feof states that it hasn't gone by the EOF.  Then
**          read enough characters to go beyond the EOF, and check
**          that feof states this is so.
**
** Depends:
**     fopen
**     fread
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

/* The file 'testfile' should exist with 15 characters in it.  If not,
   something has been lost ...
*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    const char filename[] = "testfile";
    char buffer[128];
    FILE * fp = NULL;
    int result;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

  
    /* Open a file in READ mode */

    if((fp = fopen(filename, "r")) == NULL)
    {
        Fail("Unable to open a file for reading.  Is the file "
               "in the directory?  It should be.");
    }

    /* Read 10 characters from the file.  The file has 15 
       characters in it.
    */
  
    if((result = fread(buffer,1,10,fp)) == 0)
    {
        Fail("ERROR: Zero characters read from the file.  It should have "
               "read 10 character in it.");
    }

    if(feof(fp))
    {
        Fail("ERROR:  feof returned a value greater than 0. No read "
               "operation has gone beyond the EOF yet, and feof should "
               "return 0 still.");
    }
    
    /* Read 10 characters from the file.  The file has 15 
       characters in it.  The file pointer should have no passed
       the end of file.
    */
  
    if((result = fread(buffer,1,10,fp)) == 0)
    {
        Fail("ERROR: Zero characters read from the file.  It should have "
               "read 5 character in it.");
    }

    if(feof(fp) == 0)
    {
        Fail("ERROR:  feof returned 0. The file pointer has gone beyond "
               "the EOF and this function should return positive now.");
    }

    PAL_Terminate();
    return PASS;
}
   

