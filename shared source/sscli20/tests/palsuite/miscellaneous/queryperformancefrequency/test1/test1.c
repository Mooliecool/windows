/*============================================================
**
** Source: test1.c
**
** Purpose: Test for QueryPerformanceFrequency function
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

int __cdecl main(int argc, char *argv[])
{

    LARGE_INTEGER Freq;

    /* Initialize the PAL.
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Check the return value of the performance 
     * frequency, a value of zero indicates that 
     * either the call has failed or the 
     * high-resolution performance counter is not
     * installed.
     */
    if (!QueryPerformanceFrequency(&Freq))
    {
        
        Fail("ERROR:%u:Unable to retrieve the frequency of the "
             "high-resolution performance counter.\n", 
             GetLastError());
    }
    
    
    /* Check the return value the frequency the
     * value should be non-zero.
     */
    if (Freq.QuadPart == 0)
    {

        Fail("ERROR: The frequency has been determined to be 0 "
             "the frequency should be non-zero.\n");

    }
    
    /* Terminate the PAL.
     */  
    PAL_Terminate();
    return PASS;
}