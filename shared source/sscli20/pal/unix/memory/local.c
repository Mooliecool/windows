/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    local.c

Abstract:

    Implementation of local memory management functions.

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"


SET_DEFAULT_DEBUG_CHANNEL(MEM);

static
int
AllocFlagsToHeapAllocFlags (IN  UINT  AllocFlags,
                       OUT PUINT pHeapallocFlags)
{
    int success = 1;
    UINT new = 0, flags = AllocFlags;
    if (flags & LMEM_ZEROINIT) {
        new |= HEAP_ZERO_MEMORY;
        flags &= ~LMEM_ZEROINIT;
    }
    if (flags != 0) {
        ASSERT("Invalid parameter AllocFlags=0x%x\n", AllocFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        success = 0;
    }
    if (success) {
        *pHeapallocFlags = new;
    }
    return success;
}
        
    

/*++
Function:
  LocalAlloc

See MSDN doc.
--*/
HLOCAL
PALAPI
LocalAlloc(
	   IN UINT uFlags,
	   IN SIZE_T uBytes)
{
    LPVOID lpRetVal = NULL;
    PERF_ENTRY(LocalAlloc);
    ENTRY("LocalAlloc (uFlags=%#x, uBytes=%u)\n", uFlags, uBytes);

    if (!AllocFlagsToHeapAllocFlags (uFlags, &uFlags)) {
        goto done;
    }

    lpRetVal = HeapAlloc( GetProcessHeap(), uFlags, uBytes );

done:
    LOGEXIT( "LocalAlloc returning %p.\n", lpRetVal );
    PERF_EXIT(LocalAlloc);
    return (HLOCAL) lpRetVal;
}


/*++
Function:
  LocalFree

See MSDN doc.
--*/
HLOCAL
PALAPI
LocalFree(
	  IN HLOCAL hMem)
{
    BOOL bRetVal = FALSE;
    PERF_ENTRY(LocalFree);
    ENTRY("LocalFree (hmem=%p)\n", hMem);

    if ( hMem )
    {
        bRetVal = HeapFree( GetProcessHeap(), 0, hMem );
    }
    else
    {
        bRetVal = TRUE;
    }
    
    LOGEXIT( "LocalFree returning %p.\n", bRetVal == TRUE ? (HLOCAL)NULL : hMem );
    PERF_EXIT(LocalFree);
    return bRetVal == TRUE ? (HLOCAL)NULL : hMem;
}
