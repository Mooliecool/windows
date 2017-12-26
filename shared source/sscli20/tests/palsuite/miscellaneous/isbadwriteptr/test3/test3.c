/*============================================================
**
** Source: test3.c
**
** Purpose: 
** Check that IsBadWritePtr returns non-zero on Read-only memory.
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

#define PAGE_SIZE 4096

int __cdecl main(int argc, char *argv[]) {
    
    LPVOID PageOne;

    if(0 != (PAL_Initialize(argc, argv)))
    {
	return FAIL;
    }
    
    /* Reserve enough space for four pages.  We'll commit this memory
       and set the correct access for each page below.
    */
    
    PageOne = VirtualAlloc(NULL, 
			   PAGE_SIZE, 
			   MEM_COMMIT, 
			   PAGE_READONLY);

    if(PageOne == NULL)
    {
	Fail("ERROR: VirtualAlloc failed to commit the required memory.\n");
    }

    if(IsBadWritePtr(PageOne,PAGE_SIZE) == 0)
    {
	VirtualFree(PageOne,0,MEM_RELEASE);

	Fail("ERROR: IsBadWritePtr returned 0 when checking a section of "
	     "read-only memory.  It should be non-zero.\n");
    }

    VirtualFree(PageOne,0,MEM_RELEASE);
    PAL_Terminate();
    return PASS;
}





