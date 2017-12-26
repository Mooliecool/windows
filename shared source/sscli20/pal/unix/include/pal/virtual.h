/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/map.h

Abstract:
    Header file for common mapping functions.

--*/

#ifndef _PAL_VIRTUAL_H_
#define _PAL_VIRTUAL_H_

typedef struct _CMI {
    
    struct _CMI * pNext;        /* Link to the next entry. */
    struct _CMI * pLast;        /* Link to the previous entry. */

    UINT_PTR   startBoundary;   /* Starting location of the region. */
    SIZE_T   memSize;         /* Size of the entire region.. */

    DWORD  accessProtection;    /* Initial allocation access protection. */
    DWORD  allocationType;      /* Initial allocation type. */

    BYTE * pAllocState;         /* Individual allocation type tracking for each */
                                /* page in the region. */

    BYTE * pProtectionState;    /* Individual allocation type tracking for each */
                                /* page in the region. */
#if MMAP_DOESNOT_ALLOW_REMAP
    BYTE * pDirtyPages;         /* Pages that need to be cleared if re-committed */
#endif // MMAP_DOESNOT_ALLOW_REMAP

}CMI, * PCMI;


enum VIRTUAL_CONSTANTS
{
    /* Allocation type. */
    VIRTUAL_COMMIT_ALL_BITS     = 0xFF,
    VIRTUAL_RESERVE_ALL_BITS    = 0x0,
    
    /* Protection Type. */
    VIRTUAL_READONLY,
    VIRTUAL_READWRITE,
    VIRTUAL_EXECUTE_READWRITE,
    VIRTUAL_NOACCESS,
    VIRTUAL_EXECUTE,
    VIRTUAL_EXECUTE_READ,
    
    /* Page manipulation constants. */
    VIRTUAL_PAGE_SIZE       = 0x1000,
    VIRTUAL_PAGE_MASK       = VIRTUAL_PAGE_SIZE - 1,
    BOUNDARY_64K    = 0xffff
};

/*++
Function :
    VIRTUALInitialize

    Initialize the critical sections.

Return value:
    TRUE  if initialization succeeded
    FALSE otherwise.        
--*/
BOOL VIRTUALInitialize( void );

/*++
Function :
    VIRTUALCleanup

    Deletes the critical sections.

--*/
void VIRTUALCleanup( void );


#endif /* _PAL_VIRTUAL_H_ */







