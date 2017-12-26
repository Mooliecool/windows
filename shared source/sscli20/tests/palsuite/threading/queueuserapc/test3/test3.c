/*=====================================================================
**
** Source:  test3.c
**
** Purpose: Tests how QueueUserAPC handles an invalid thread.
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

int __cdecl main (int argc, char **argv) 
{    
    int ret;

    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ret = QueueUserAPC(NULL, NULL, 0);
    if (ret != 0)
    {
        Fail("QueueUserAPC passed with an invalid thread!\n");
    }

    PAL_Terminate();
    return PASS;
}
