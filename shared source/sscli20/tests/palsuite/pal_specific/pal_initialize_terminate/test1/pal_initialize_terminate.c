/*=============================================================
**
** Source: pal_initialize_terminate.c
**
** Purpose: Positive test the PAL_Initialize and PAL_Terminate API.
**          Call PAL_Initialize to initialize the PAL 
**          environment and call PAL_Terminate to clean up the PAL
**          environment.
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
  
    /* Initialize the PAL environment */
    err = PAL_Initialize(argc, argv);
    
    if(0 != err)
    {
        ExitProcess(1);
    }
    
    PAL_Terminate();
    return 0;
}
