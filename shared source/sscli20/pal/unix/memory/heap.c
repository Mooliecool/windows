/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    heap.c

Abstract:

    Implementation of heap memory management functions.

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include <errno.h>


SET_DEFAULT_DEBUG_CHANNEL(MEM);


#define HEAP_MAGIC 0xEAFDC9BB
#define DUMMY_HEAP 0x01020304


/*++
Function:
  RtlMoveMemory

See MSDN doc.
--*/
VOID
PALAPI
RtlMoveMemory(
          IN PVOID Destination,
          IN CONST VOID *Source,
          IN SIZE_T Length)
{
    PERF_ENTRY(RtlMoveMemory);
    ENTRY("RtlMoveMemory(Destination:%p, Source:%p, Length:%d)\n", 
          Destination, Source, Length);
    
    memmove(Destination, Source, Length);
    
    LOGEXIT("RtlMoveMemory returning\n");
    PERF_EXIT(RtlMoveMemory);
}

/*++
Function:
  GetProcessHeap

See MSDN doc.
--*/
HANDLE
PALAPI
GetProcessHeap(
	       VOID)
{
    HANDLE ret;

    PERF_ENTRY(GetProcessHeap);
    ENTRY("GetProcessHeap()\n");

    ret = (HANDLE) DUMMY_HEAP;
  
    LOGEXIT("GetProcessHeap returning HANDLE %p\n", ret);
    PERF_EXIT(GetProcessHeap);
    return ret;
}

/*++
Function:
  HeapAlloc

Abstract
  Implemented as wrapper over malloc

See MSDN doc.
--*/
LPVOID
PALAPI
HeapAlloc(
	  IN HANDLE hHeap,
	  IN DWORD dwFlags,
	  IN SIZE_T dwBytes)
{
    BYTE *pMem;
    int nSize = 0;

    PERF_ENTRY(HeapAlloc);
    ENTRY("HeapAlloc (hHeap=%p, dwFlags=%#x, dwBytes=%u)\n",
          hHeap, dwFlags, dwBytes);

    #if defined(_DEBUG)
        nSize =  max(sizeof(void*),sizeof(double));
    #endif

    if (hHeap != (HANDLE) DUMMY_HEAP)
    {
        ERROR("Invalid heap handle\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT("HeapAlloc returning NULL\n");
        PERF_EXIT(HeapAlloc);
        return NULL;
    }

    if ((dwFlags != 0) && (dwFlags != HEAP_ZERO_MEMORY))
    {
        ASSERT("Invalid parameter dwFlags=%#x\n", dwFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT("HeapAlloc returning NULL\n");
        PERF_EXIT(HeapAlloc);
        return NULL;
    }

    pMem = (BYTE *) malloc(dwBytes+nSize);

#if MALLOC_ZERO_RETURNS_NULL
    /* Some platforms return NULL for malloc(0), check for that */
    if ((pMem == NULL) && (dwBytes+nSize == 0))
    {
        pMem = (BYTE *) malloc(1);
    }
#endif // MALLOC_ZERO_RETURNS_NULL

    if (pMem == NULL)
    {
        ERROR("Not enough memory\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        LOGEXIT("HeapAlloc returning NULL\n");
        PERF_EXIT(HeapAlloc);
        return NULL;
    }

    /* use a magic number, to know it has been allocated with HeapAlloc
       when doing HeapFree */
    /*we are doing this only for the debug build*/
    #if defined(_DEBUG)
        *((DWORD *) pMem) = HEAP_MAGIC;
    #endif

    /*If the Heap Zero memory flag is set initialize to zero*/
    if (dwFlags == HEAP_ZERO_MEMORY)
    {
        memset(pMem+nSize, 0, dwBytes);
    }
    LOGEXIT("HeapAlloc returning LPVOID %p\n", pMem+nSize);
    PERF_EXIT(HeapAlloc);
    return (pMem + nSize);
}


/*++
Function:
  HeapFree

Abstract
  Implemented as wrapper over free

See MSDN doc.
--*/
BOOL
PALAPI
HeapFree(
	 IN HANDLE hHeap,
	 IN DWORD dwFlags,
	 IN LPVOID lpMem)
{
    int nSize =0;
    BOOL bRetVal = FALSE;

    PERF_ENTRY(HeapFree);
    ENTRY("HeapFree (hHeap=%p, dwFalgs = %#x lpMem=%p)\n", 
          hHeap, dwFlags, lpMem);

    #if defined(_DEBUG)
        nSize =  max(sizeof(void*),sizeof(double));
    #endif

    if (hHeap != (HANDLE) DUMMY_HEAP)
    {
        ERROR("Invalid heap handle\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if (dwFlags != 0)
    {
        ASSERT("Invalid parameter dwFlags=%#x\n", dwFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if ( !lpMem )
    {
        bRetVal = TRUE;
        goto done;
    }
    /*nSize + nMemAlloc is the size of Magic Number plus
     *size of the int to store value of Memory allocated */
    lpMem -= (nSize);
    
    /* Check if it is valid pointer. The checking works only when nSize is non-zero. 
     * When it is zero and we're trying to free a memory block of allocated size 0, 
     * we might fail since the memory needn't be writable (even though it's non-NULL).
     * Note that the compiler should optimize away the code when nSize is 0
     */
    if (nSize && IsBadReadPtr(lpMem, 1))
    {
        ERROR("Invalid Pointer (%p)\n", lpMem+nSize);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    /* check if the memory has been allocated by HeapAlloc */
    /*we are doing this only for the debug build*/
    #if defined(_DEBUG)
        if (*((DWORD *) lpMem) != HEAP_MAGIC)
        {
            ERROR("Pointer hasn't been allocated with HeapAlloc (%p)\n", lpMem+nSize);
            SetLastError(ERROR_INVALID_PARAMETER);
            goto done;
        }
        *((DWORD *) lpMem) = 0;
    #endif

    bRetVal = TRUE;
    free (lpMem);

done:
    LOGEXIT( "HeapFree returning BOOL %d\n", bRetVal );
    PERF_EXIT(HeapFree);
    return bRetVal;
}




/*++
Function:
  HeapReAlloc

Abstract
  Implemented as wrapper over realloc

See MSDN doc.
--*/
LPVOID
PALAPI
HeapReAlloc(
	  IN HANDLE hHeap,
	  IN DWORD dwFlags,
	  IN LPVOID lpmem,
	  IN SIZE_T dwBytes)
{
    BYTE *pMem = NULL;
    int nSize = 0;

    PERF_ENTRY(HeapReAlloc);
    ENTRY("HeapReAlloc (hHeap=%p, dwFlags=%#x, lpmem=%p, dwBytes=%u)\n",
          hHeap, dwFlags, lpmem, dwBytes);

    #if defined(_DEBUG)
        nSize =  max(sizeof(void*),sizeof(double));
    #endif

    if (hHeap != GetProcessHeap())
    {
        ASSERT("Invalid heap handle\n");
        SetLastError(ERROR_INVALID_HANDLE);
        goto done;
    }

    if ((dwFlags != 0))
    {
        ASSERT("Invalid parameter dwFlags=%#x\n", dwFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if (lpmem == NULL)
    {
        WARN("NULL memory pointer to realloc.Do not do anything\n");
        /* set LastError back to zero. this appears to be an undocumented
        behavior in Windows, in doesn't cost much to match it */
        SetLastError(0);
        goto done;
    }

   /*nSize + nMemAlloc is the size of Magic Number plus
     *size of the int to store value of Memory allocated */
   lpmem -= (nSize);

    /* Check if it is valid pointer. The checking works only when nSize is non-zero. 
     * When it is zero and we're trying to realloc a memory block of previously 
     * allocated size 0, we might fail since the location of previous allocation needn't
     * be writable (even though its non-NULL).
     * Note that the compiler should optimize away the code when nSize is 0
     */
    if (nSize && IsBadReadPtr(lpmem, 1))
    {
        ERROR("Invalid Pointer (%p)\n", lpmem+nSize);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    /* check if the memory has been allocated by HeapAlloc */
    /*we are doing this only for the debug build*/
    #if defined(_DEBUG)
        if (*((DWORD *) lpmem) != HEAP_MAGIC)
        {
            ERROR("Pointer hasn't been allocated with HeapAlloc (%p)\n", lpmem+nSize);
            SetLastError(ERROR_INVALID_PARAMETER);
            goto done;
        }
    #endif

   pMem = (BYTE *) realloc(lpmem,dwBytes+nSize);

   /*make sure the pointer is null and it is not because of
    * any reason other than ENOMEM*/
   if ((pMem == NULL) && (errno != 0))
   {
       ERROR("Not enough memory\n");
       SetLastError(ERROR_NOT_ENOUGH_MEMORY);
       goto done;
   }

     /* use a magic number, to know it has been allocated with HeapAlloc
       when doing HeapFree */
    /*we are doing this only for the debug build*/
    #if defined(_DEBUG)
        *((DWORD *) pMem) = HEAP_MAGIC;
    #endif

done:
    LOGEXIT("HeapReAlloc returns LPVOID %p\n", pMem ? (pMem+nSize) : pMem);
    PERF_EXIT(HeapReAlloc);
    return pMem ? (pMem+nSize) : pMem;
}

