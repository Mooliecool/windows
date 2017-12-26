/*============================================================================
**
** Source:  test2.c
**
** Purpose: Calls qsort to sort a buffer, and verifies that it has done
**          the job correctly.
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

#include <palsuite.h>

int __cdecl twocharcmp(const void *pa, const void *pb)
{
    return memcmp(pa, pb, 2);
}

int __cdecl main(int argc, char **argv)
{
    char before[] = "ccggaaiieehhddbbjjff";
    const char after[] = "aabbccddeeffgghhiijj";

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    qsort(before, (sizeof(before) - 1) / 2, 2 * sizeof(char), twocharcmp);
  
    if (memcmp(before, after, sizeof(before)) != 0)
    {
        Fail("qsort did not correctly sort an array of 2-character "
             "buffers.\n");
    }

    PAL_Terminate();
    return PASS;

}





