/*=====================================================================
**
** Source:  SetFileTime.c
**
** Purpose: Tests the PAL implementation of the SetFileTime
** This test first tries to SetFileTime on a HANDLE which doesn't have
** GENERIC_WRITE set, which should fail.
**
**
** Depends:
**         CreateFile
**         CloseHandle
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




int __cdecl main(int argc, char **argv)
{

    FILETIME SetCreation,SetLastAccess,SetLastWrite;
    HANDLE TheFileHandle;
    BOOL result;
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* Populate some FILETIME structures with values 
       These values are valid Creation, Access and Write times
       which I generated, and should work properly.
       
       The access times are not seperated into WIN32 and FreeBSD here,
       but it should be fine, as no comparisons are being done in this
       test.
    */

    SetCreation.dwLowDateTime = 458108416;
    SetCreation.dwHighDateTime = 29436904;

    SetLastAccess.dwLowDateTime = 341368832;
    SetLastAccess.dwHighDateTime = 29436808;

    SetLastWrite.dwLowDateTime = -1995099136;
    SetLastWrite.dwHighDateTime = 29436915;
    
    
/* Open the file to get a HANDLE, without GENERIC WRITE */
    
    TheFileHandle = 
        CreateFile(
            "the_file",                 
            GENERIC_READ,  
            0,                           
            NULL,                        
            OPEN_ALWAYS,                 
            FILE_ATTRIBUTE_NORMAL,       
            NULL);                       

	
    if(TheFileHandle == INVALID_HANDLE_VALUE) 
    {
        Fail("ERROR: Failed to open the file.  The error number "
               "returned was %d.",GetLastError());
    }

    /* This SetFileTime should fail, because the HANDLE isn't set with
       GENERIC_WRITE 
    */
    result = SetFileTime(TheFileHandle,
                         &SetCreation,&SetLastAccess,&SetLastWrite);
    
    if(result != 0)
    {
	    Fail("ERROR: SetFileTime should have failed, but returned a "
	       "non-zero result.  The File HANDLE passed was no set Writable "
	       "which should cause failure.");
    }
   
    result = CloseHandle(TheFileHandle);

    if(result == 0)
    {
        Fail("ERROR: CloseHandle failed.  This test depends upon "
               "it working.");
    }
    
    

    PAL_Terminate();
    return PASS;
}
