/*=====================================================================
**
** Source:  test2.c
**
** Purpose: Tests the PAL implementation of the fopen function. 
**          Test to ensure that you can write to a 'w' mode file.
**          And that you can't read from a 'w' mode file.
**
** Depends:
**      fprintf
**      fseek
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
                                                                         
#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
  
    FILE *fp;
    char buffer[128];
  
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

  
    if( (fp = fopen( "testfile", "w" )) == NULL )
    {
        Fail( "ERROR: The file failed to open with 'w' mode.\n" );
    }  

    /* Test that you can write */
    if(fprintf(fp,"%s","some text") <= 0) 
    {
        Fail("ERROR: Attempted to WRITE to a file opened with 'w' mode "
               "but fprintf failed.  Either fopen or fprintf have problems.");
    }

    if(fseek(fp, 0, SEEK_SET)) 
    {
        Fail("ERROR: fseek failed, and this test depends on it.");
    }

    /* Test that you can't read */
    if(fgets(buffer,10,fp) != NULL)
    {
        Fail("ERROR: Tried to READ from a file with only 'w' mode set. "
               "This should fail, but fgets didn't return NULL.  Either "
               "fgets or fopen is broken.");
    }

    PAL_Terminate();
    return PASS;
}
   

