/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/critsect.h

Abstract:
    
    Header file for the critical sections functions.
    
--*/

#ifndef _PAL_CRITSECT_H_
#define _PAL_CRITSECT_H_

#include "pal/thread.h"

/*++
Function : 
    SYNCInitializeCriticalSection


Parameters :
    IN lpCriticalSection  pointer to critical section object

    
Return value :
    0                if pthread mutex was succesfully created
    STATUS_NO_MEMORY (exception code) not enough memory to create pthread mutex
--*/
DWORD 
SYNCInitializeCriticalSection(
    OUT LPCRITICAL_SECTION lpCriticalSection);                

void SYNCEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection, BOOL pal_internal);
void SYNCLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection, BOOL pal_internal);
void SYNCEnterCriticalSectionCore(LPCRITICAL_SECTION lpCriticalSection, BOOL pal_internal, BOOL noDebug);
void SYNCLeaveCriticalSectionCore(LPCRITICAL_SECTION lpCriticalSection, BOOL pal_internal, BOOL noDebug);
#endif /* _PAL_CRITSECT_H_ */

