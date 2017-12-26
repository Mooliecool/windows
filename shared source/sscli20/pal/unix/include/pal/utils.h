/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/utils.h

Abstract:
    Miscellaneous helper functions for the PAL, which don't fit anywhere else

--*/

#ifndef _PAL_UTILS_H_
#define _PAL_UTILS_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*++
Function:
  UTIL_inverse_wcspbrk

  Opposite of wcspbrk : searches a string for the first character NOT in the 
  given set

Parameters :
    LPWSTR lpwstr :   string to search
    LPCWSTR charset : list of characters to search for
                                      
Return value :
    pointer to first character of lpwstr that isn't in the set
    NULL if all characters are in the set                                                                 
--*/
LPWSTR UTIL_inverse_wcspbrk(LPWSTR lpwstr, LPCWSTR charset);

/*++
Function : 
    UTIL_IsReadOnlyBitsSet
    
    Takes a struct stat *
    Returns true if the file is read only,
--*/
BOOL UTIL_IsReadOnlyBitsSet( struct stat * stat_data );

/*++
Function : 
    UTIL_IsExecuteBitsSet
    
    Takes a struct stat *
    Returns true if the file is executable.
--*/
BOOL UTIL_IsExecuteBitsSet( struct stat * stat_data );


/*++
Function : 
    UTIL_WCToMB_Alloc
    
    Converts a wide string to a multibyte string, allocating the required buffer
    
Parameters :
    LPCWSTR lpWideCharStr : string to convert
    int cchWideChar : number of wide characters to convert
                      (-1 to convert a complete null-termnated string)
    
Return Value :
    newly allocated buffer containing the converted string. Conversion is 
    performed using CP_ACP. Buffer is allocated with malloc(), release it 
    with free().
    In case if failure, LastError will be set.
--*/
LPSTR UTIL_WCToMB_Alloc(LPCWSTR lpWideCharStr, int cchWideChar);

/*++
Function : 
    UTIL_MBToWC_Alloc
    
    Converts a multibyte string to a wide string, allocating the required buffer
    
Parameters :
    LPCSTR lpMultiByteStr : string to convert
    int cbMultiByte : number of bytes to convert 
                      (-1 to convert a complete null-termnated string)
    
Return Value :
    newly allocated buffer containing the converted string. Conversion is 
    performed using CP_ACP. Buffer is allocated with malloc(), release it 
    with free().
    In case if failure, LastError will be set.
--*/
LPWSTR UTIL_MBToWC_Alloc(LPCSTR lpMultiByteStr, int cbMultiByte);

#if HAVE_VM_ALLOCATE
/*++
Function:
  UTIL_SetLastErrorFromMach

    Maps a Mach kern_return_t to a Win32 LastError code.
--*/
void
UTIL_SetLastErrorFromMach(
                          kern_return_t MachReturn);
#endif //HAVE_VM_ALLOCATE


#endif /* _PAL_UTILS_H_ */
