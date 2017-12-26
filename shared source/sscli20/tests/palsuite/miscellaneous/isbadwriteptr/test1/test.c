/*============================================================
**
** Source: test.c
**
** Purpose: IsBadWritePtr() function
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

#include <palsuite.h>

#define MEMORY_AMOUNT 16

int __cdecl main(int argc, char *argv[]) {
    
    void * TestingPointer = NULL;
    BOOL ResultValue = 0;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
  
    TestingPointer = malloc(MEMORY_AMOUNT);
    if ( TestingPointer == NULL )
    {
	Fail("ERROR: Failed to allocate memory for TestingPointer pointer. "
             "Can't properly exec test case without this.\n");
    }


    /* This should be writeable, return 0 */
    ResultValue = IsBadWritePtr(TestingPointer,MEMORY_AMOUNT);

    if(ResultValue != 0) 
    {
	free(TestingPointer);

        Fail("ERROR: Returned %d when 0 should have been returned, checking "
             "to see if writable memory is unwriteable.\n",ResultValue);
    }

    free(TestingPointer);
  
    /* This should be !writeable, return nonezero */
    TestingPointer = (void*)0x08; /* non writeable address */
    ResultValue = IsBadWritePtr(TestingPointer,sizeof(int));
    
    if(ResultValue == 0) 
    {
        Fail("ERROR: Returned %d when nonezero should have been returned, "
             "checking to see if unwriteable memory  is writeable.\n",
             ResultValue);
    }
  
    /* This should be !writeable, return Nonezero */
    ResultValue = IsBadWritePtr(NULL,MEMORY_AMOUNT);

    if(ResultValue == 0) 
    {
        Fail("ERROR: Returned %d when nonezero should have been "
	     "returned,checking "
             "to see if a NULL pointer is writeable.\n",
             ResultValue);
    }
    
    PAL_Terminate();
    return PASS;
}




