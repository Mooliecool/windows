/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/misc.h

Abstract:
    Header file for the initialization and clean up functions
    for the misc Win32 functions

--*/

#ifndef __MISC_H_
#define __MISC_H_

/*++
Variable :

    palEnvironment

    A global variable equivalent to environ on systems on which that exists,
    and a pointer to an array of environment strings on systems without
    environ.
--*/
extern char **palEnvironment;

/*++
Function :

    FMTMSG_FormatMessageCleanUp
    
    Frees the satellite file from memory.
    
--*/
BOOL FMTMSG_FormatMessageCleanUp( void );

/*++
Function :
    MsgBoxInitialize

    Initialize the critical sections.

Return value:
    TRUE if initialize succeeded
    FALSE otherwise

--*/
BOOL MsgBoxInitialize( void );

/*++
Function :
    MsgBoxCleanup

    Deletes the critical sections.

--*/
void MsgBoxCleanup( void );

/*++

Function:
  MiscInitialize

--*/
BOOL MiscInitialize();

/*++
Function:
  MiscCleanup

--*/
VOID MiscCleanup();

/*++
Function:
  MiscGetenv

Gets an environment variable's value from environ. The returned buffer
must not be modified or freed.
--*/
char *MiscGetenv(const char *name);

/*++
Function:
  MiscPutenv

Sets an environment variable's value by directly modifying environ.
Returns TRUE if the variable was set, or FALSE if malloc or realloc
failed or if the given string is malformed.
--*/
BOOL MiscPutenv(const char *string, BOOL deleteIfEmpty);

/*++
Function:
  MiscUnsetenv

Removes a variable from the environment. Does nothing if the variable
does not exist in the environment.
--*/
void MiscUnsetenv(const char *name);

/*++
Function:
  TIMECleanUpTransitionDates

Free the memory allocated in TIMEGetStdDstDates.
--*/
void TIMECleanUpTransitionDates();

#endif /* __MISC_H_ */

