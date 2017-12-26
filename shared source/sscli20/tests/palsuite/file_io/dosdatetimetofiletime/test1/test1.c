/*=====================================================================
**
** Source: test1.c
**
** Purpose: Convert two valid DOS times to FileTimes.  Then check that
** the FileTimes are the same distance apart as the DOS times were.
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
    
    WORD DosDate = 0x14CF;            /* Dec 15th, 2000 */
    WORD DosTime = 0x55AF;            /* 10:45:30       */
    WORD SecondDosTime = 0x55B0;      /* 10:45:32 */
    
    FILETIME FirstResultTime;
    FILETIME SecondResultTime;

    ULONG64 FullFirstTime;
    ULONG64 FullSecondTime;
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* Convert two DosDateTimes to FileTimes, ensure that each call returns
       non-zero
    */
    if(DosDateTimeToFileTime(DosDate, DosTime, &FirstResultTime) == 0)
    {
        Fail("ERROR: DosTimeToFileTime should have returned non-0 to indicate "
             "success.  GetLastError() returned %d.",GetLastError());
    }
    
    if(DosDateTimeToFileTime(DosDate, SecondDosTime, &SecondResultTime) == 0)
    {
        Fail("ERROR: DosTimeToFileTime should have returned non-0 to indicate "
             "success.  GetLastError() returned %d.",GetLastError());
    }

    /* Move the FILETIME structures into a ULONG value which we can 
       work with
    */
    FullFirstTime = ((((ULONG64)FirstResultTime.dwHighDateTime)<<32) | 
                    ((ULONG64)FirstResultTime.dwLowDateTime));
    
    FullSecondTime = ((((ULONG64)SecondResultTime.dwHighDateTime)<<32) | 
                      ((ULONG64)SecondResultTime.dwLowDateTime));
    
    /* Check to see that these two times are two seconds apart, just like
       those that were converted.
    */
    if((FullSecondTime-FullFirstTime) != 20000000)
    {
        Fail("ERROR: Two times were converted which were two seconds "
             "apart.  When convert to a FileTime they should have been "
             "20000000 nano-seconds apart, but instead were %d seconds "
             "apart.",FullSecondTime-FullFirstTime);
    }

    PAL_Terminate();
    return PASS;
}

