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

    Header file for file mapping functions.

--*/

#ifndef _PAL_MAP_H_
#define _PAL_MAP_H_

#include "pal/handle.h"
#include "pal/shmemory.h"

#ifndef NO_INO
#define NO_INO ((ino_t)-1)
#endif

/*++
Function :
    MapInitialize

    Initialize the critical sections.

Return value:
    TRUE  if initialization succeeded
    FALSE otherwise        
--*/
BOOL MAPInitialize( void );

/*++
Function :
    MapCleanup

    Deletes the critical sections.

--*/
void MAPCleanup( void );

/*++
Function :
    MAPGetRegionSize

    Parameters: 
    lpAddress: pointer to the starting memory location, not necessary
               to be rounded to the page location

    Note: This function is to be used in virtual.c
          
    Returns the region size in byte
--*/

SIZE_T MAPGetRegionSize( LPVOID lpAddress);

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
typedef struct _NativeMapHolder
{
    LONG volatile ref_count;
    LPVOID address;
    SIZE_T size;
    SIZE_T offset; /* for future use */
} NativeMapHolder;
#endif

/* Process specific information. This 
structure is not stored in shared memory.*/
typedef struct _MVL
{
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
    NativeMapHolder * pNMHolder; /* Ref-counted holder for memory mapping */
#endif
    LPVOID lpAddress;           /* The pointer to the mapped memory. */
    SIZE_T NumberOfBytesToMap;  /* Number of bytes to map. */
    DWORD dwDesiredAccess;      /* Desired access. */
    HANDLE hFileMapping;        /* duplicate handle of the file mapping */
    
    struct _MVL * pNext;               
    struct _MVL * pPrev;

}MAPPED_VIEW_LIST, * PMAPPED_VIEW_LIST;

/* Global information.
This structure contains the information needed to for any process in the PAL
system to access the mapped memory objects.
*/
typedef struct _GMMO
{
    /* The member fields prefixed with an underscore are only used
    when the GLOBAL_FILE_MAPPING_OBJECT is shared across processes.
    Else these fields are ignored. They wont be initialized, so don't 
    use them. */
    SHM_NAMED_OBJECTS _ShmHeader;   /* Contains information to describe the 
                                       type of named object */
    UINT              _RefCount;    /* Global reference counter. */
    
    union
    {
        SHMPTR  shm;                /* Shared memory storage for filename. */
        LPSTR   lp;                 /* Heap memory storage for filename. */
    } fileName;
    
    UINT    MaxSize;                /* The max size of the file mapping 
                                       object. */
    DWORD   flProtect;              /* Protection desired for the file view.*/
    BOOL    bPALCreatedTempFile;    /* TRUE if its a PAL created file. */

} GLOBAL_FILE_MAPPING_OBJECT, * PGLOBAL_FILE_MAPPING_OBJECT;

/* Per process mapped memory information.*/
typedef struct _MMI 
{
    HOBJSTRUCT HandleData;              /* The handle information. */
    PMAPPED_VIEW_LIST pViewList;        /* Linked list of mapped views. */ 
    DWORD   dwDesiredAccessWhenOpened;  /* FILE_MAP_WRITE etc */
    UINT    HandleRefCount;             /* Number of open handles. */
    
    BOOL    bIsNamed;                   /* Whether or not the mapping is named*/
    union 
    {
        SHMPTR  shm;                    /* Shared memory pointer to the kernel object.*/
        PGLOBAL_FILE_MAPPING_OBJECT lp; /* HeapPtr to the kernel object */
    } kernelObject;

    INT     UnixFd;                     /* File descriptor. */
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
    dev_t   MappedFileDevNum;           /* ID of device containing the file to be mapped */
    ino_t   MappedFileInodeNum;         /* Inode number of file to be mapped.
					   These two fields are used used to uniquely 
					   identify files on systems that do not allow 
					   more than one shared mmapping per region of 
					   physical file, per process */
#endif
    
    struct _MMI * pNext;                /* Pointer to the next mapped kernel object. */
    struct _MMI * pPrev;                /* Pointer to the previous mapped kernel object. */

} MAPPED_MEMORY_INFO, * PMAPPED_MEMORY_INFO;

#endif /* _PAL_MAP_H_ */
