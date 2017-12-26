/*============================================================
**
** Source: test.c
**
** Purpose: Test for CloseHandle function
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
**=========================================================*/

/* Depends on: CreateFile and WriteFile */

#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{

    HANDLE FileHandle = NULL;
    LPDWORD WriteBuffer; /* Used with WriteFile */

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    WriteBuffer = malloc(sizeof(WORD));
    
    if ( WriteBuffer == NULL )
    {
        Fail("ERROR: Failed to allocate memory for WriteBuffer pointer. "
             "Can't properly exec test case without this.\n");
    }
                                     
 
    /* Create a file, since this returns to us a HANDLE we can use */
    FileHandle = CreateFile("testfile",   
                            GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,  
                            FILE_ATTRIBUTE_NORMAL,          
                            NULL);
  
    /* Should be able to close this handle */
    if(CloseHandle(FileHandle) == 0)
    {
	free(WriteBuffer);
        Fail("ERROR: (Test 1) Attempted to close a HANDLE on a file, but the "
             "return value was <=0, indicating failure.\n");    
    }
  
    free(WriteBuffer);
    
    PAL_Terminate();
    return PASS;
}




