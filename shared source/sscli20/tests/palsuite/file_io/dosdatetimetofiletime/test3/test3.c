/*=====================================================================
**
** Source: test3.c
**
** Purpose: Check the upper and lower bounds of dates and times.
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
    WORD UpperDosDate = 0x739F;       /* Dec 31st, 2037 */
    WORD LowerDosDate = 0x21;         /* Jan, 1st, 1980 */
    WORD UpperDosTime = 0xBF7D;       /* 23:59:58 */
    WORD LowerDosTime = 0x0;          /* 0:00:00 */
    
    FILETIME ResultTime;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* 
       Convert a DosDateTime using the highest possible date and time.
    */
    if(DosDateTimeToFileTime(UpperDosDate, UpperDosTime, &ResultTime) == 0)
    {
        Fail("ERROR: DosDateTimeToFileTime failed when attempting to "
             "convert the highest possible date and time.  GetLastError() "
             "returned %d.\n",GetLastError());
    }

    /* 
       Convert a DosDateTime using the lowest possible date and time.
    */
    if(DosDateTimeToFileTime(LowerDosDate, LowerDosTime, &ResultTime) == 0)
    {
        Fail("ERROR: DosDateTimeToFileTime failed when attempting to "
             "convert the lowest possible date and time.  GetLastError() "
             "returned %d.\n",GetLastError());
    }

    
    PAL_Terminate();
    return PASS;
}

