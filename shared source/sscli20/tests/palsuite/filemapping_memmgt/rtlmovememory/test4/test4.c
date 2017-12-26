/*=============================================================
**
** Source: test4.c
**
** Purpose: Test simple overlapping.  Move the first 50 bytes of a 
** piece of memory to the latter 50 bytes.  ie i -> i+50 Check to make sure
** no data is lost.
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
    MEMORY_AMOUNT = 128
};

int __cdecl main(int argc, char *argv[])
{
    char* NewAddress;
    char OldAddress[MEMORY_AMOUNT];
    int i;
    
    if(PAL_Initialize(argc, argv))
    {
        return FAIL;
    }
    
    NewAddress = OldAddress+50;

    /* Put some data into the block we'll be moving 
       The first 50 byes will be 'X' and the rest set to 'Z'
    */
    memset(OldAddress, 'X', 50);
    memset(NewAddress, 'Z', MEMORY_AMOUNT-50);  

    /* Move the first 50 bytes of OldAddress to OldAddress+50.  This
       is to test that the source and destination addresses can overlap.
    */
    RtlMoveMemory(NewAddress, OldAddress, 50);

    /* Check to ensure the moved data didn't get corrupted 
       The first 50 bytes should be 'X'
    */
    for(i=0; i<50; ++i)
    {
        if(NewAddress[i] != 'X')
        {
            Fail("ERROR: When the memory was moved to a new location, the "
                 "data which was stored in it was somehow corrupted.  "
                 "Character %d should have been 'X' but instead is %c.\n",
                 i, NewAddress[i]);
        }
    }

    /* The rest of the memory should be 'Z' */
    for(i=50; i<MEMORY_AMOUNT-50; ++i)
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
