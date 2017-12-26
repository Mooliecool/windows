/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Check three cases of searching for a string within a string.  First when
** the string is contained, check that the pointer returned points to it.
** Then when it isn't contained, ensure it returns null.  And when the string
** we're searching for is empty, it should return a pointer to the string 
** we're searching through.Test #1 for the strstr function
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


struct testCase
{
    char *result;
    char *string1;
    char *string2;

};

int __cdecl main(int argc, char *argv[])
{
    int i=0;
    char *ptr=NULL;

    struct testCase testCases[]=
    {
        {"is is a test", "This is a test","is"},
        {"fghijkl","abcdefghijkl","fgh"},
        {NULL,"aabbccddeeffgg","h"},
        {NULL,"aabb", "eeeeeee"},
        {"AAA", "BBddfdaaaaAAA","A"},
        {"fdaaaaAAA", "BBddfdaaaaAAA","f"},
        {"aadfsadfas","aadfsadfas",""},
        {NULL,"","ccc"}
    };

    /*
     *  Initialize the PAL
     */
    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    for (i=0; i<sizeof(testCases)/sizeof(struct testCase); i++)
    {
        ptr = strstr(testCases[i].string1,testCases[i].string2);
        if (ptr==NULL)
	{
           if (testCases[i].result != NULL)
           {
	       Fail("ERROR: strstr returned incorrect value\n"
		    "Expected a pointer to \"%s\" , got a pointer to NULL\n",
		    testCases[i].result);
	   }
        }
        else
        {
           if (strcmp(testCases[i].result,ptr) != 0)
	   {
	       Fail("ERROR: strstr returned incorrect value\n"
		    "Expected a pointer to \"%s\" , got a pointer to \"%s\"\n",
		    testCases[i].result, ptr);
           }
        }
    }

    PAL_Terminate();
    return PASS;
}

