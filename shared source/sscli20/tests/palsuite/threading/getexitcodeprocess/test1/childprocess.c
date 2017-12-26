/*============================================================
**
** Source: childprocess.c
**
** Purpose: Test to ensure GetExitCodeProcess returns the right 
** value. All this program does is return a predefined value.
**
** Dependencies: none
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

#include <rotor_pal.h>
#include "myexitcode.h"

int __cdecl main( int argc, char **argv ) 
{
    int i;
    
    // simulate some activity 
    for( i=0; i<10000; i++ )
        ;
        
    // return the predefined exit code
    return TEST_EXIT_CODE;
}
