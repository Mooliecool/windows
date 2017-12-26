/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the _makepath function.
**          Create a path, and ensure that it builds how it is
**          supposed to.
** 
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

#if WIN32
#define PATHNAME "C:\\test\\test.txt"
#else
#define PATHNAME "/test/test.txt"
#endif

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    char FullPath[128];

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc,argv)))
    {
        return FAIL;
    }

#if WIN32
    _makepath(FullPath,"C","\\test","test","txt");
#else
    _makepath(FullPath,NULL,"/test","test","txt");
#endif

    if(strcmp(FullPath,PATHNAME) != 0)
    {
        Fail("ERROR: The pathname which was created turned out to be %s "
               "when it was supposed to be %s.\n",FullPath,PATHNAME);
    }


    PAL_Terminate();
    return PASS;
}













