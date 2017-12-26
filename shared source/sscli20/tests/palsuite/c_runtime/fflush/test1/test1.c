/*============================================================================
**
** Source:  test1.c
**
** Purpose: Tests to see that fflush is working properly.  Flushes a couple
** buffers and checks the return value.  Can't figure out a way to test
** and ensure it is really dropping the buffers, since the system
** does this automatically most of the time ...
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

/* This function is really tough to test.  Right now it just tests 
   a bunch of return values.  No solid way to ensure that it is really
   flushing a buffer or not -- might have to be a manual test someday.
*/

#include <palsuite.h>


int __cdecl main(int argc, char **argv)
{
    
    int TheReturn;
    FILE* TheFile; 
    FILE* AnotherFile = NULL;
  
    PAL_Initialize(argc,argv);
     
    TheFile = fopen("theFile","w+");

    if(TheFile == NULL) 
    {
        Fail("ERROR: fopen failed.  Test depends on this function.");
    }
    
    TheReturn = fwrite("foo",3,3,TheFile);
    
    if(TheReturn != 3) 
    {
        Fail("ERROR: fwrite failed.  Test depends on this function.");
    }
  
    /* Test to see that FlushFileBuffers returns a success value */
    TheReturn = fflush(TheFile);

    if(TheReturn != 0) 
    {
        Fail("ERROR: The fflush function returned non-zero, which "
               "indicates failure, when trying to flush a buffer.");
    }

    /* Test to see that FlushFileBuffers returns a success value */
    TheReturn = fflush(NULL);

    if(TheReturn != 0) 
    {
        Fail("ERROR: The fflush function returned non-zero, which "
               "indicates failure, when trying to flush all buffers.");
    }

    /* Test to see that FlushFileBuffers returns a success value */
    TheReturn = fflush(AnotherFile);

    if(TheReturn != 0) 
    {
        Fail("ERROR: The fflush function returned non-zero, which "
               "indicates failure, when trying to flush a stream not "
               "associated with a file.");
    }
  
    PAL_Terminate();
    return PASS;
}


