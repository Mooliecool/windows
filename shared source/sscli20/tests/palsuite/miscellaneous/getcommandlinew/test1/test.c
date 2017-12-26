/*============================================================
**
** Source : test.c
**
** Purpose: Test for GetCommandLineW() function
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

#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{

    LPWSTR TheResult = NULL;	
    WCHAR *CommandLine = malloc(1024);  
    int i;
    WCHAR * p;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

  
    wcscpy(CommandLine,convert(argv[0]));

    for(i=1;i<argc;++i) 
    {
        wcscat(CommandLine,convert(" "));
        wcscat(CommandLine,convert(argv[i]));
    }
  
    TheResult = GetCommandLine();
  
    /* If it is NULL, it failed. */
    if(TheResult == NULL) 
    {
        Fail("ERROR: The command line returned was NULL -- but should be "
	     "a LPWSTR.");      
    }

    // It's ok that if there is trailing white spaces in "TheResult"
    // Let's trim them.
    p = TheResult + wcslen(TheResult) - 1;
    while (* p == L' ' || * p == L'\t') { printf(L"%c\n", *p); * p-- = 0 ; }
  
    if(memcmp(TheResult,CommandLine,wcslen(TheResult)*2+2) != 0) 
    {
        Fail("ERROR: The command line returned was %s instead of %s "
	     "which was the command.\n",
	     convertC(TheResult), convertC(CommandLine));
    }
  
    free(CommandLine);
    
    PAL_Terminate();
    return PASS;
  
}


