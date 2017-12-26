/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the _wmakepath function.
**          Create a path, and ensure that it builds how it is
**          supposed to.
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

#define UNICODE

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    WCHAR FullPath[128];
    WCHAR File[] = {'t','e','s','t','\0'};
    WCHAR Ext[] = {'t','x','t','\0'};
    char * PrintResult=NULL;  /* Used for printing out errors */
    char * PrintCorrect=NULL;

#if WIN32
    WCHAR Drive[] = {'C','\0'};
    WCHAR Dir[] = {'\\','t','e','s','t','\0'};
    WCHAR PathName[] =
        {'C',':','\\','t','e','s','t','\\','t','e',
         's','t','.','t','x','t','\0'};
#else
    WCHAR *Drive = NULL;
    WCHAR Dir[] = {'/','t','e','s','t','\0'};
    WCHAR PathName[] =
 {'/','t','e','s','t','/','t','e','s','t','.','t','x','t','\0'};
#endif

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc,argv)))
    {
        return FAIL;
    }

    _wmakepath(FullPath, Drive, Dir, File, Ext);

    if(wcscmp(FullPath,PathName) != 0)
    {
        PrintResult = convertC(FullPath);
        PrintCorrect = convertC(PathName);

        Fail("ERROR: The pathname which was created turned out to be %s "
               "when it was supposed to be %s.\n",PrintResult,PrintCorrect);
    }


    PAL_Terminate();
    return PASS;
}













