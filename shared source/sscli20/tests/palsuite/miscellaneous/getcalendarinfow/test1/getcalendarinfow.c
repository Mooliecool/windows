/*=============================================================
**
** Source: GetCalendarInfoW.c
**
** Purpose: Positive test the GetCalendarInfoW API.
**          Call GetCalendarInfoW to retrive the information of a 
**          calendar
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
#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    int err;
    LCID Locale = LOCALE_USER_DEFAULT;
    CALTYPE CalType = CAL_ITWODIGITYEARMAX|CAL_RETURN_NUMBER;
    DWORD dwValue;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }
    
    err = GetCalendarInfoW(Locale,/*locale idendifier*/
                            CAL_GREGORIAN, /*calendar identifier*/
                            CalType,  /*calendar tyope*/
                            NULL,     /*buffer to store the retrive info*/
                            0,        /*alwayse zero*/
                            &dwValue);/*to store the requrest data*/               
    if (0 == err)
    {
        Fail("GetCalendarInfoW failed for CAL_GREGORIAN!\n");
    }
    
    err = GetCalendarInfoW(Locale,/*locale idendifier*/
                            CAL_GREGORIAN_US, /*calendar identifier*/
                            CalType,  /*calendar tyope*/
                            NULL,     /*buffer to store the retrive info*/
                            0,        /*alwayse zero*/
                            &dwValue);/*to store the requrest data*/               
    if (0 == err)
    {
        Fail("GetCalendarInfoW failed for CAL_GREGORIAN_US!\n");
    }


    PAL_Terminate();
    return PASS;
}
