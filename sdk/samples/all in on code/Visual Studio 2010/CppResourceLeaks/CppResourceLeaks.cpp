/****************************** Module Header ******************************\
Module Name:  CppResourceLeaks.cpp
Project:      CppResourceLeaks
Copyright (c) Microsoft Corporation.

CppResourceLeaks is designed to show typical resource leaks and their 
consequences. It focuses on two situations of resource leaks:

1. Handle Leak:

A handle leak is a type of software bug that occurs when a computer program 
asks for a handle to a resource but does not free the handle when it is no 
longer used. If this occurs frequently or repeatedly over an extended 
period of time, a large number of handles may be marked in-use and thus  
unavailable, causing performance problems or a crash.

2. Memory Leak:

A memory leak is a particular type of unintentional memory consumption by a 
computer program where the program fails to release memory when no longer 
needed. This condition is normally the result of a bug in a program that 
prevents it from freeing up memory that it no longer needs. Memory is 
allocated to a program, and that program subsequently loses the ability to 
access it due to program logic flaws.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


#pragma region Handle Leaks

//
//   FUNCTION: LeakFileHandle
//
//   PURPOSE: The function leaks two file handles.
//
void LeakFileHandle()
{
    DWORD dwHandleCount;
    wchar_t szTempFile[MAX_PATH];
    HANDLE hTempFile = NULL;
    HANDLE hTempFileRO = NULL;

    wprintf(L"Press ENTER to leak file handles ...");
    getwchar();

    // Get and print the current process handle count.
    if (!GetProcessHandleCount(GetCurrentProcess(), &dwHandleCount))
    {
        wprintf(L"GetProcessHandleCount failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    wprintf(L"Current process handle count: %u\n", dwHandleCount);

    // Get the temp path and the temp file name.
    if (GetTempPath(ARRAYSIZE(szTempFile), szTempFile) == 0)
    {
        wprintf(L"GetTempPath failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    if (GetTempFileName(szTempFile, L"HLeak", 0, szTempFile) == 0)
    {
        wprintf(L"GetTempFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    wprintf(L"Temp file name: %s\n", szTempFile);

    // Create and operate on the temp file.
    wprintf(L"Create the file\n");
    hTempFile = CreateFile(szTempFile, GENERIC_READ | GENERIC_WRITE, 
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hTempFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"CreateFile failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Read/Write the file using hTempFile.
    //WriteFile(hTempFile, ...
    //ReadFile(hTempFile, ...

    // Create another handle to the file object.
    // the handle has read-only access.
    wprintf(L"Duplicate the file handle\n");
    if (!DuplicateHandle(
        GetCurrentProcess(), hTempFile, 
        GetCurrentProcess(), &hTempFileRO, 
        GENERIC_READ, FALSE, 0))
    {
        wprintf(L"DuplicateHandle failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Read the file using hTempFileRO.
    //ReadFile(hTempFile, ...

Cleanup:

    // Leak the temp file handles.
    //CloseHandle(hTempFileRO);
    //CloseHandle(hTempFile);

    // Get and print the current process handle count.
    if (GetProcessHandleCount(GetCurrentProcess(), &dwHandleCount))
    {
        wprintf(L"Current process handle count: %u\n", dwHandleCount);
    }
}


//
//   FUNCTION: LeakInjectedHandle
//
//   PURPOSE: The function creates a child process (notepad.exe) and injects 
//   a mutex handle to the process. A handle will be leaked in the child 
//   process because the child process is not notified about the injection.
//
//   NOTE: It is possible for a process to open a handle and inject that 
//   handle into another process, assuming that the injecting process has 
//   the proper access rights. When that happens, and the injected handle is 
//   not closed by the target process, a handle leak occurs in the target 
//   process. No leak happens in the current injecting process.
//
void LeakInjectedHandle()
{
    HANDLE hMutex = NULL;
    PCWSTR pszMutexName = L"LeakyMutex";

    HANDLE hMutexInChildProcess = NULL;
    wchar_t szChildCmd[] = L"notepad.exe";
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { };

    // Create a mutex in the current process and get its handle.
    wprintf(L"Create the mutex \"%s\"\n", pszMutexName);
    hMutex = CreateMutex(NULL, FALSE, pszMutexName);
    if (hMutex == NULL)
    {
        wprintf(L"CreateMutex failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Start a child process.
    wprintf(L"Start the child process \"%s\"\n", szChildCmd);
    if (!CreateProcess(NULL, szChildCmd, NULL, NULL, FALSE, 0, NULL, NULL, 
        &si, &pi))
    {
        wprintf(L"CreateProcess failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    wprintf(L"Press ENTER to inject handle to the child process ...");
    // Allow attaching windbg to the child process and trace handles.
    getwchar();

    // Inject the mutex handle to the child process.
    wprintf(L"Inject (duplicate) the handle to the child process\n");

    // hMutexInChildProcess is a handle relative to the target process. It is 
    // meaningless to the current process.
    if (!DuplicateHandle(
        GetCurrentProcess(), hMutex, 
        pi.hProcess, &hMutexInChildProcess,
        0, FALSE, DUPLICATE_SAME_ACCESS))
    {
        wprintf(L"DuplicateHandle failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Use some IPC mechanism to inform the target process about the injected
    // handle value (hMutexInChildProcess). The target process is responsible  
    // for closing the handle.
    // ...

Cleanup:

    // Close the handles. (No handle leak in the current injecting process).

    if (hMutex)
    {
        CloseHandle(hMutex);
        hMutex = NULL;
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
        pi.hThread = NULL;
    }
    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
        pi.hProcess = NULL;
    }

    // NOTE: hMutexInChildProcess is a handle relative to the child process 
    // that identifies the same mutex object in kernel. The current 
    // (injecting) process should never attempt to close the duplicated 
    // handle by executing the code: CloseHandle(hMutexInChildProcess);  
    // because hMutexInChildProcess is meaningless to the injecting process.
}


//
//   FUNCTION: LeakGDIHandle
//
//   PURPOSE: The function leaks GDI handles.
//
void LeakGDIHandle()
{
    // TO BE FINISHED.
}

#pragma endregion


#pragma region Memory Leaks

//
//   FUNCTION: LeakHeapMemory
//
//   PURPOSE: Leak memory allocated on the process heap.
//
void LeakHeapMemory()
{
    wprintf(L"Press ENTER to leak heap memory ...");
    getwchar();

    while (TRUE)
    {
        const DWORD dwBytes = 500;

        // Allocate the memory on the heap.
        wprintf(L"Allocate %d bytes' heap memory\n", dwBytes);

        PVOID pMem = LocalAlloc(LPTR, dwBytes);
        if (pMem)
        {
            // Write to the memory pointed by pMem.
            strcpy_s((char *)pMem, dwBytes, "All-In-One Code Framework");

            // Memory was available and pointed to by p, but not saved. After 
            // this iteration, the pointer is destroyed, and the allocated 
            // memory becomes unreachable. To fix the leak problem:
            //LocalFree(pMem);
        }
    }
}


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//
//   FUNCTION: LeakCRTHeapMemory
//
//   PURPOSE: Leak memory allocated on CRT heap using CRT APIs.
//
void LeakCRTHeapMemory()
{
    wprintf(L"Press ENTER to leak CRT memory ...");
    getwchar();

    {
        void *p = malloc(500);
    }

    _CrtDumpMemoryLeaks();
}

#pragma endregion


void PrintInstructions()
{
    wprintf(L"CppResourceLeaks Instructions:\n" \
        L"-f   Leak file handles\n" \
        L"-i   Leak injected handles\n" \
        L"-g   Leak GDI handles\n" \
        L"-m   Leak process heap memory\n" \
        L"-c   Leak CRT heap memory\n"
        );
}


int wmain(int argc, wchar_t *argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"f", argv[1] + 1) == 0)
        {
            // Leak file handles.
            LeakFileHandle();
        }
        else if (_wcsicmp(L"i", argv[1] + 1) == 0)
        {
            // Leak injected handles.
            LeakInjectedHandle();
        }
        else if (_wcsicmp(L"g", argv[1] + 1) == 0)
        {
            // Leak GDI handles.
            LeakGDIHandle();
        }
        else if (_wcsicmp(L"m", argv[1] + 1) == 0)
        {
            // Leak process heap memory.
            LeakHeapMemory();
        }
        else if (_wcsicmp(L"c", argv[1] + 1) == 0)
        {
            // Leak CRT heap memory.
            LeakCRTHeapMemory();
        }
        else
        {
            PrintInstructions();
        }
    }
    else
    {
        PrintInstructions();
    }

    wprintf(L"Press ENTER to exit ...");
    getwchar();

    return 0;
}