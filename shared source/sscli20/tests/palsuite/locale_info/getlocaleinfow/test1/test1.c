/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that GetLocaleInfoW gives the correction information for 
**          LOCALE_NEUTRAL.
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


int Types[] = { LOCALE_SDECIMAL, LOCALE_STHOUSAND, LOCALE_ILZERO, 
    LOCALE_SCURRENCY, LOCALE_SMONDECIMALSEP, LOCALE_SMONTHOUSANDSEP };

char *TypeStrings[] = { "LOCALE_SDECIMAL", "LOCALE_STHOUSAND", "LOCALE_ILZERO",
    "LOCALE_SCURRENCY", "LOCALE_SMONDECIMALSEP", "LOCALE_SMONTHOUSANDSEP" };

#define NUM_TYPES (sizeof(Types) / sizeof(Types[0]))

typedef WCHAR InfoStrings[NUM_TYPES][4];

typedef struct
{
    LCID lcid;
    InfoStrings Strings;
} LocalInfoType;

LocalInfoType Locales[] =
{
    {LOCALE_NEUTRAL, 
        {{'.',0}, {',',0}, {'1',0}, {'$',0}, {'.',0}, {',',0}}},
};

int NumLocales = sizeof(Locales) / sizeof(Locales[0]);


int __cdecl main(int argc, char *argv[])
{    
    WCHAR buffer[256] = { 0 };
    int ret;
    int i,j;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    for (i=0; i<NumLocales; i++)
    {
        for (j=0; j<NUM_TYPES; j++)
        {
            ret = GetLocaleInfoW(Locales[i].lcid, Types[j], buffer, 256);
            
            if (ret == 0)
            {
                Fail("GetLocaleInfoW returned an unexpected error!\n");
            }


            if (wcscmp(buffer, Locales[i].Strings[j]) != 0)
            {

                Fail("GetLocaleInfoW gave incorrect result for %s, "
                    "locale %#x:\nExpected \"%S\", got \"%S\"!\n", TypeStrings[j], 
                    Locales[i].lcid, Locales[i].Strings[j], buffer);
                    
            }

            if (ret != wcslen(Locales[i].Strings[j]) + 1)
            {
                Fail("GetLocaleInfoW returned incorrect value for %s, "
                    "locale %#x:\nExpected %d, got %d!\n", TypeStrings[j], 
                    Locales[i].lcid, wcslen(Locales[i].Strings[j])+1, ret);
            }
        }        
    }
    

    PAL_Terminate();

    return PASS;
}

