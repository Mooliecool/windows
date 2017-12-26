/****************************** Module Header ******************************\
* Module Name:  CppFileMappingServer.cpp
* Project:      CppFileMappingServer
* Copyright (c) Microsoft Corporation.
* 
* File mapping is a mechanism for one-way or duplex inter-process 
* communication among two or more processes in the local machine. To share a 
* file or memory, all of the processes must use the name or the handle of the 
* same file mapping object.
* 
* To share a file, the first process creates or opens a file by using the 
* CreateFile function. Next, it creates a file mapping object by using the 
* CreateFileMapping function, specifying the file handle and a name for the 
* file mapping object. The names of event, semaphore, mutex, waitable timer, 
* job, and file mapping objects share the same name space. Therefore, the 
* CreateFileMapping and OpenFileMapping functions fail if they specify a name
* that is in use by an object of another type.
* 
* To share memory that is not associated with a file, a process must use the 
* CreateFileMapping function and specify INVALID_HANDLE_VALUE as the hFile 
* parameter instead of an existing file handle. The corresponding file mapping 
* object accesses memory backed by the system paging file. You must specify 
* a size greater than zero when you use an hFile of INVALID_HANDLE_VALUE in a 
* call to CreateFileMapping.
* 
* Processes that share files or memory must create file views by using the 
* MapViewOfFile or MapViewOfFileEx function. They must coordinate their access 
* using semaphores, mutexes, events, or some other mutual exclusion technique.
* 
* This VC++ code sample demonstrates creating a file mapping object named 
* "Local\SampleMap" and writing a string to the file mapping.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#pragma endregion


// In terminal services: The name can have a "Global\" or "Local\"  prefix 
// to explicitly create the object in the global or session namespace. The 
// remainder of the name can contain any character except the backslash 
// character (\). For more information, see:
// http://msdn.microsoft.com/en-us/library/aa366537.aspx
#define MAP_PREFIX          L"Local\\"
#define MAP_NAME            L"SampleMap"
#define FULL_MAP_NAME       MAP_PREFIX MAP_NAME

// Max size of the file mapping object.
#define MAP_SIZE            65536

// File offset where the view is to begin.
#define VIEW_OFFSET         0

// The number of bytes of a file mapping to map to the view. All bytes of the 
// view must be within the maximum size of the file mapping object (MAP_SIZE). 
// If VIEW_SIZE is 0, the mapping extends from the offset (VIEW_OFFSET) to  
// the end of the file mapping.
#define VIEW_SIZE           1024

// Unicode string message to be written to the mapped view. Its size in byte 
// must be less than the view size (VIEW_SIZE).
#define MESSAGE             L"Message from the first process."


int wmain(int argc, wchar_t* argv[])
{
    HANDLE hMapFile = NULL;
    PVOID pView = NULL;

    // Create the file mapping object.
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // Use paging file - shared memory
        NULL,                   // Default security attributes
        PAGE_READWRITE,         // Allow read and write access
        0,                      // High-order DWORD of file mapping max size
        MAP_SIZE,               // Low-order DWORD of file mapping max size
        FULL_MAP_NAME           // Name of the file mapping object
        );
    if (hMapFile == NULL) 
    {
        wprintf(L"CreateFileMapping failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"The file mapping (%s) is created\n", FULL_MAP_NAME);

    // Map a view of the file mapping into the address space of the current 
    // process.
    pView = MapViewOfFile(
        hMapFile,               // Handle of the map object
        FILE_MAP_ALL_ACCESS,    // Read and write access
        0,                      // High-order DWORD of the file offset 
        VIEW_OFFSET,            // Low-order DWORD of the file offset 
        VIEW_SIZE               // The number of bytes to map to view
        );
    if (pView == NULL)
    { 
        wprintf(L"MapViewOfFile failed w/err 0x%08lx\n", GetLastError()); 
        goto Cleanup;
    }
    wprintf(L"The file view is mapped\n");

    // Prepare a message to be written to the view.
    PWSTR pszMessage = MESSAGE;
    DWORD cbMessage = (wcslen(pszMessage) + 1) * sizeof(*pszMessage);

    // Write the message to the view.
    memcpy_s(pView, VIEW_SIZE, pszMessage, cbMessage);

    wprintf(L"This message is written to the view:\n\"%s\"\n",
        pszMessage);

    // Wait to clean up resources and stop the process.
    wprintf(L"Press ENTER to clean up resources and quit");
    getchar();

Cleanup:

    if (hMapFile)
    {
        if (pView)
        {
            // Unmap the file view.
            UnmapViewOfFile(pView);
            pView = NULL;
        }
        // Close the file mapping object.
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }

    return 0;
}