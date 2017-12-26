// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
#include "common.h"
#include <winwrap.h>
#include <excep.h>          // For COMPlusThrow
#include <appdomain.hpp>
#include <assembly.hpp>
#include "nlstable.h"       // Class declaration

/*=================================NLSTable==========================
**Action: Constructor for NLSTable.  It caches the assembly from which we will read data table files.
**Returns: Create a new NLSTable instance.
**Arguments: pAssembly  the Assembly that NLSTable will retrieve data table files from.
**Exceptions: None.
============================================================================*/

NLSTable::NLSTable(Assembly* pAssembly) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pAssembly));
    } CONTRACTL_END;
    m_pAssembly = pAssembly;
}

/*=================================CreateSharedMemoryMapping==================================
**Action: Create a file mapping object which can be shared among different users under Windows NT/2000.
**        Actually its just a memory mapped section of the swapfile.
**Returns: The file mapping handle.  NULL if any error happens.
**Arguments:
**      pMappingName    the name of the file mapping object.
**      iSize           Size to use
**Exceptions: 
**Note:
**      This function creates a DACL which grants GENERIC_ALL access to members of the "Everyone" group.
**      Then create a security descriptor using this DACL.  Finally, use this SA to create the file mapping object.
** WARNING:
**      This creates a shared file or shared paged memory section (if hFile == INVALID_HANDLE_VALUE) that is shared machine-wide
**      Therefore for side-by-side to work, the mapping names must be unique per version!
**      We utilize this feature for code pages in case it hasn't changed across versions we can still reuse the
**      tables, but it seems suspicious for other applications (as                         commented in MapDataFile below)
==============================================================================*/
// static method
HANDLE NLSTable::CreateSharedMemoryMapping(const LPCWSTR pMappingName, const int iSize ) {
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(iSize > 0);
        PRECONDITION(CheckPointer(pMappingName));
    } CONTRACTL_END;
    
    HANDLE hFileMap = NULL;

    {
        // In Windows 9x, security is not supported, so just pass NULL in security attribute.
        hFileMap = WszCreateFileMapping( 
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, iSize, pMappingName);    
    }

    // If still not allowed, try building one with no name
    if (hFileMap == NULL)
    {
        hFileMap = WszCreateFileMapping( 
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, iSize, NULL);    
    }

    return (hFileMap) ;
}

/*=================================OpenOrCreateMemoryMapping==================================
**Action: Opens an existing memory mapped object, or creates a new one (by calling above fn).
**        Worst case just allocate some memory.
**Returns: The pointer to our memory.  NULL if any error happens.
**Arguments:
**      pMappingName    the name of the file mapping object.
**      iSize           Size to use
**Exceptions:
**
**IMPORTANT:
**      Memory mapped sections are cleared when set.  We expect the caller to set the last int
**      to a non-zero value, so we test this flag.  If it is still zero when we open it, we
**      assume that we've gotten a result in an unfinished state and allocate a new one instead
**      of trying to use the one with the zeros.
**
**Note:
**      This function creates a DACL which grants GENERIC_ALL access to members of the "Everyone" group.
**      Then create a security descriptor using this DACL.  Finally, use this SA to create the file mapping object.
** WARNING:
**      This creates a shared file or shared paged memory section (if hFile == INVALID_HANDLE_VALUE) that is shared machine-wide
**      Therefore for side-by-side to work, the mapping names must be unique per version!
**      We utilize this feature for code pages in case it hasn't changed across versions we can still reuse the
**      tables, but it seems suspicious for other applications (as                         commented in MapDataFile below)
==============================================================================*/
PBYTE NLSTable::OpenOrCreateMemoryMapping(const LPCWSTR pMappingName, const int iSize, HANDLE* mappedFile)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(iSize % 4 == 0);
        PRECONDITION(iSize > 0);
        PRECONDITION(CheckPointer(pMappingName));
        PRECONDITION(CheckPointer(mappedFile));
    } CONTRACTL_END;

    _ASSERTE(pMappingName != NULL); // Must have a string name.
    _ASSERTE(iSize > 0);            // Pointless to have <= 0 allocation
    _ASSERTE(iSize % 4 == 0);       // Need 4 byte alignment for flag check

    LPVOID  pResult = NULL;

    // Try creating/opening it.
    HANDLE  hMap = NULL;

    *mappedFile = hMap;
    // Calls into OS a lot, should switch to preemp mode
    GCX_PREEMP();

    if (hMap == NULL) {
        // If access is denied for global\namespace or the name is not opened in global namespace, try the local namespace.
        // Also if we're rotor or win 9x.
        hMap = WszOpenFileMapping(FILE_MAP_READ, TRUE, pMappingName);       
    }  
    
    if (hMap != NULL) {
        // We got a section, map a view, READ ONLY!
        pResult = MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0);

        // Anything found?
        if (pResult != NULL)
        {
            // Make sure our result is allocated.  We expect a non-0 flag to be set for last int of our section
            int* pFlag = (int*)(((BYTE*)pResult) + iSize - 4);
            if (*pFlag != 0)
            {
                *mappedFile = hMap;
                // Found a valid already opened section!
                return (PBYTE)pResult;
            }

            // Couldn't find it, unmap it.
            UnmapViewOfFile(pResult);
            pResult = NULL;
        }

        // We can't use this one, so close it
        CloseHandle(hMap);
        hMap = NULL;
    }
    
    // Didn't get a section, try to create one, NT/XP/.Net gets security permissions, 9X doesn't,
    // but our helper fn takes care of that for us.
    hMap = NLSTable::CreateSharedMemoryMapping(pMappingName, iSize);

    // Were we successfull?
    if (hMap != NULL)
    {
        // We have hMap, try to get our section
        pResult = MapViewOfFile( hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

        // Don't close hMap unless we aren't using it.
        // That confuses the mapping stuff and we lose the name, it'll close when runtime shuts down.
        if (pResult == NULL)
        {
            CloseHandle(hMap);
            hMap = NULL;
        }
        // There is no need to zero out the mapCodePageCached field, since the initial contents of the pages in the file mapping object are zero.
        
        *mappedFile = hMap;
    }

    return (PBYTE)pResult;
}

/*=================================GetResource==================================
**Action: Open the specified NLS+ data file from system assembly.
**Returns: Pointer to the interesting memory
**Arguments: The required NLS+ data file name (in ANSI)
**Exceptions: OutOfMemoryException if error happens in get the data file
**            from system assembly.
==============================================================================*/
PBYTE NLSTable::GetResource(LPCSTR pFileName) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;    // Assembly::GetResource() may trigger a GC.
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pFileName));
    } CONTRACTL_END;

    _ASSERTE(m_pAssembly != NULL);

    DWORD cbResource;
    PBYTE pbInMemoryResource = NULL;
    //
    // Get the base system assembly (mscorlib.dll to most of us);
    //
    
    // Get the resource, and associated file handle, from the assembly.
    if (m_pAssembly->GetResource(pFileName,
                                 &cbResource, &pbInMemoryResource,
                                 NULL, NULL, NULL) == FALSE) {
        COMPlusThrowOM();
    }

    _ASSERTE(pbInMemoryResource != NULL);

    return (pbInMemoryResource);
}

/*=================================GetResource==================================
**Action: Open a NLS+ resource from system assembly.
**Returns: Pointer to the interesting memory
**Arguments: The required NLS+ data file name (in Unicode)
**Exceptions: OutOfMemoryException if buffer can not be allocated.
**            ExecutionEngineException if error happens in calling OpenDataFile(LPCSTR)
==============================================================================*/

PBYTE NLSTable::GetResource(LPCWSTR pFileName)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pFileName));
    } CONTRACTL_END;

    PBYTE pbInMemoryResource = NULL;
    // The following macro will delete pAnsiFileName when
    // getting out of the scope of this function.
    MAKE_ANSIPTR_FROMWIDE(pAnsiFileName, pFileName);
    if (!pAnsiFileName)
    {
        COMPlusThrowOM();
    }

    pbInMemoryResource = GetResource((LPCSTR)pAnsiFileName);
    _ASSERTE(pbInMemoryResource != NULL);
    return (pbInMemoryResource);
}

