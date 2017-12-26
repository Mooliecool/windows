/*=====================================================================
**
** Source:  test4.c
**
** Purpose: Tests the PAL implementation of the _wfopen function. 
**          Test to ensure that you can't write to a 'r' mode file.
**          And that you can read from a 'r' mode file.
**
** Depends:
**      fprintf
**      fclose
**      fgets
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

#define UNICODE                                                          

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
  
    FILE *fp;
    char buffer[128];
    WCHAR filename[] = {'t','e','s','t','f','i','l','e','\0'};
    WCHAR write[] = {'w','\0'};
    WCHAR read[] = {'r','\0'};

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

  
    /* Open a file with 'w' mode */
    if( (fp = _wfopen( filename, write )) == NULL )
    {
        Fail( "ERROR: The file failed to open with 'w' mode.\n" );
    }  
  
    /* Write some text to the file */
    if(fprintf(fp,"%s","some text") <= 0) 
    {
        Fail("ERROR: Attempted to WRITE to a file opened with 'w' mode "
               "but fprintf failed.  Either fopen or fprintf have problems.");
    }

    if(fclose(fp))
    {
        Fail("ERROR: Attempted to close a file, but fclose failed. "
               "This test depends upon it.");
    }

    /* Open a file with 'r' mode */
    if( (fp = _wfopen( filename, read )) == NULL )
    {
        Fail( "ERROR: The file failed to open with 'r' mode.\n" );
    } 
  
    /* Attempt to read from the 'r' only file, should pass */
    if(fgets(buffer,10,fp) == NULL)
    {
        Fail("ERROR: Tried to READ from a file with 'r' mode set. "
               "This should succeed, but fgets returned NULL.  Either fgets "
               "or fopen is broken.");
    }

    /* Write some text to the file */
    if(fprintf(fp,"%s","some text") > 0) 
    {
        Fail("ERROR: Attempted to WRITE to a file opened with 'r' mode "
               "but fprintf succeeded  It should have failed.  "
               "Either fopen or fprintf have problems.");
    }
 

    PAL_Terminate();
    return PASS;
}
   

