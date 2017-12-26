/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that GetTimeZoneInformation gives reasonable values.
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

int __cdecl main(int argc, char *argv[])
{
    TIME_ZONE_INFORMATION tzi;
    DWORD ret;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    ret = GetTimeZoneInformation(&tzi);
    if (ret == TIME_ZONE_ID_UNKNOWN) 
    {        
        /* Occurs in time zones that do not use daylight savings time. */
        if (tzi.StandardBias != 0)
        {
            Fail("GetTimeZoneInformation() gave invalid data!\n"
                "Returned TIME_ZONE_ID_UNKNOWN but StandardBias != 0!\n");
        }
        if (tzi.DaylightBias != 0)
        {
            Fail("GetTimeZoneInformation() gave invalid data!\n"
                "Returned TIME_ZONE_ID_UNKNOWN but DaylightBias != 0!\n");
        }
    }    
    else if (ret == TIME_ZONE_ID_STANDARD)
    {
        if (tzi.StandardBias != 0)
        {
            Fail("GetTimeZoneInformation() gave invalid data!\n"
                "StandardBias is %d, should be 0!\n", tzi.StandardBias);
        }
    }
    else if (ret == TIME_ZONE_ID_DAYLIGHT)
    {
        if (tzi.DaylightBias != -60 && tzi.DaylightBias != 0)
        {
            Fail("GetTimeZoneInformation() gave invalid data!\n"
                "DaylightBias is %d, should be 0 or -60!\n", tzi.DaylightBias);
        }
    }
    else 
    {
        Fail("GetTimeZoneInformation() returned an invalid value!\n");
    }

    if (tzi.Bias % 30 != 0)
    {
        Fail("GetTimeZoneInformation() gave an invalid bias of %d!\n", 
            tzi.Bias);
    }

    PAL_Terminate();

    return PASS;
}

