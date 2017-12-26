/*=============================================================
**
** Source: UnmapViewOfFile.c (test 2)
**
** Purpose: Negative test the UnmapViewOfFile API.
**          Call UnmapViewOfFile to unmap a view of
**          NULL.
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
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    int err;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /* Negative test the UnmapViewOfFile by passing a NULL*/
    /* mapping address handle*/
    err = UnmapViewOfFile(NULL);
    if(0 != err)
    {
        Fail("ERROR: Able to call UnmapViewOfFile API "
             "by passing a NULL mapping address.\n" );

    }

    /* Terminate the PAL.
     */
    PAL_Terminate();
    return PASS;
}
