/*=============================================================
**
** Source:  virtualfree.c
**
** Purpose: Positive test the VirtualFree API.
**          Call VirtualFree with MEM_RELEASE
**          free operation type
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
    LPVOID lpVirtualAddress;

    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }
    
    //Allocate the physical storage in memory or in the paging file on disk 
    lpVirtualAddress = VirtualAlloc(NULL,//system determine where to allocate the region
            1024,            //specify the size 
            MEM_COMMIT,      //allocation type
            PAGE_READONLY);  //access protection
    if(NULL == lpVirtualAddress)
    {
        Fail("\nFailed to call VirtualAlloc API!\n");
        PAL_Terminate();
        return 1;
    }

    //decommit the specified region
    err = VirtualFree(lpVirtualAddress,//base address
                    0,           //must be zero with MEM_RELEASE
                    MEM_RELEASE);//free operation
    if(0 == err)
    {
        Fail("\nFailed to call VirtualFree API!\n");
        PAL_Terminate();
        return 1;
    }

    PAL_Terminate();
    return PASS;
}
