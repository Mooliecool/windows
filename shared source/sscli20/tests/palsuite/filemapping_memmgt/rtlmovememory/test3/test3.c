/*=============================================================
**
** Source: test3.c
**
** Purpose: Allocate 128 bytes of memory and store data in it.  Move 10
** other bytes of memory to that location. Check that the first 10 bytes
** carried over their data and that the other 118 were unchanged.
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
**============================================================*/

#include <palsuite.h>

enum Memory
{
    NEW_MEMORY_AMOUNT = 128,
    OLD_MEMORY_AMOUNT = 10
};

int __cdecl main(int argc, char *argv[])
{
    char NewAddress[NEW_MEMORY_AMOUNT];
    char OldAddress[OLD_MEMORY_AMOUNT];
    int i;

    
    if(PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Put some data into the block we'll be moving */
    memset(OldAddress, 'X', OLD_MEMORY_AMOUNT);

    /* Put some data into the block we're moving to */
    memset(NewAddress, 'Z', NEW_MEMORY_AMOUNT);

    /* Move the block to the NewAddress */
    RtlMoveMemory(NewAddress, OldAddress, OLD_MEMORY_AMOUNT);

    /* Check to ensure the moved data didn't get corrupted */
    for(i=0; i<OLD_MEMORY_AMOUNT; ++i)
    {
        if(NewAddress[i] != 'X')
        {
            Fail("ERROR: When the memory was moved to a new location, the "
                 "data which was stored in it was somehow corrupted.  "
                 "Character %d should have been 'X' but instead is %c.\n",
                 i, NewAddress[i]);
        }
    }
    
    /* Check to ensure the memory which didn't move didn't get corrupted */
    for(i=OLD_MEMORY_AMOUNT; i<NEW_MEMORY_AMOUNT; ++i)
    {
        if(NewAddress[i] != 'Z')
        {
            Fail("ERROR: When the memory was moved to a new location, the "
                 "data which was stored in it was somehow corrupted.  "
                 "Character %d should have been 'Z' but instead is %c.\n",
                 i, NewAddress[i]);
        }
    }

  
    PAL_Terminate();
    return PASS;
}
