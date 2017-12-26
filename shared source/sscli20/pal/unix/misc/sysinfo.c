/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    sysinfo.c

Abstract:

    Implements GetSystemInfo.

Revision History:

--*/

#include "pal/palinternal.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#if HAVE_SYSCTL
#include <sys/sysctl.h>
#elif !HAVE_SYSCONF
#error Either sysctl or sysconf is required for GetSystemInfo.
#endif

#include <sys/param.h>
#if HAVE_SYS_VMPARAM_H
#include <sys/vmparam.h>
#endif  // HAVE_SYS_VMPARAM_H

// On some platforms sys/user.h ends up defining _DEBUG; if so
// remove the definition before including the header and put
// back our definition afterwards
#if USER_H_DEFINES_DEBUG
#define OLD_DEBUG _DEBUG
#undef _DEBUG
#endif
#include <sys/user.h>
#if USER_H_DEFINES_DEBUG
#undef _DEBUG
#define _DEBUG OLD_DEBUG
#undef OLD_DEBUG
#endif

#include "pal/dbgmsg.h"


SET_DEFAULT_DEBUG_CHANNEL(MISC);



/*++
Function:
  GetSystemInfo

GetSystemInfo

The GetSystemInfo function returns information about the current system. 

Parameters

lpSystemInfo 
       [out] Pointer to a SYSTEM_INFO structure that receives the information. 

Return Values

This function does not return a value. 

Note:
  fields returned by this function are:
    dwNumberOfProcessors
    dwPageSize
Others are set to zero.

--*/
VOID
PALAPI
GetSystemInfo(
          OUT LPSYSTEM_INFO lpSystemInfo)
{
    int nrcpus = 0;
#if HAVE_SYSCTL
    int rc;
    size_t sz;
    int mib[2];
#endif  // HAVE_SYSCTL
    long pagesize;
    
    PERF_ENTRY(GetSystemInfo);
    ENTRY("GetSystemInfo (lpSystemInfo=%p)\n", lpSystemInfo);
    
    pagesize = getpagesize();
    
    lpSystemInfo->wProcessorArchitecture_PAL_Undefined = 0;
    lpSystemInfo->wReserved_PAL_Undefined = 0;
    lpSystemInfo->dwPageSize = pagesize;
    lpSystemInfo->dwActiveProcessorMask_PAL_Undefined = 0;

#if HAVE_SYSCTL
    sz = sizeof(nrcpus);
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    rc = sysctl(mib, 2, &nrcpus, &sz, NULL, 0);
    if (rc != 0)
    {
        ASSERT("sysctl failed for HW_NCPU (%d)\n", errno);
    }
#else // HAVE_SYSCONF

    nrcpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (nrcpus < 1)
    {
        ASSERT("sysconf failed for _SC_NPROCESSORS_ONLN (%d)\n", errno);
    }
#endif
    TRACE("dwNumberOfProcessors=%d\n", nrcpus);
    lpSystemInfo->dwNumberOfProcessors = nrcpus;

#ifdef VM_MAXUSER_ADDRESS
    lpSystemInfo->lpMaximumApplicationAddress = (PVOID) VM_MAXUSER_ADDRESS;
#elif defined(USERLIMIT)
    lpSystemInfo->lpMaximumApplicationAddress = (PVOID) USERLIMIT;
#elif defined(USRSTACK)
    lpSystemInfo->lpMaximumApplicationAddress = (PVOID) USRSTACK;
#else
#error The maximum application address is not known on this platform.
#endif

    lpSystemInfo->lpMinimumApplicationAddress = (PVOID) pagesize;

    lpSystemInfo->dwProcessorType_PAL_Undefined = 0;

    lpSystemInfo->dwAllocationGranularity = pagesize;

    lpSystemInfo->wProcessorLevel_PAL_Undefined = 0;
    lpSystemInfo->wProcessorRevision_PAL_Undefined = 0;

    LOGEXIT("GetSystemInfo returns VOID\n");
    PERF_EXIT(GetSystemInfo);
}
