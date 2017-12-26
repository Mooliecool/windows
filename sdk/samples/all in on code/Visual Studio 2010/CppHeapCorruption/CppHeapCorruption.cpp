/****************************** Module Header ******************************\
Module Name:  CppHeapCorruption.cpp
Project:      CppHeapCorruption
Copyright (c) Microsoft Corporation.

CppHeapCorruption is designed to show heap corruption and its consequences.
It demonstrates four typical situations of heap corruption: 

A. Using Uninitialied State

B. Heap Overrun and Underrun

C. Heap Handle Mismatch

D. Heap Reuse After Deletion

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


#pragma region UseUninitializedMemory

//
//   FUNCTION: UseUninitializedMemory
//
//   PURPOSE: The function demonstrates heap corruption caused by the use of 
//   uninitialized memory. Uninitialized state is a common programming 
//   mistake that can lead to numerous hours of debugging to track down. 
//   Fundamentally, uninitialized state refers to a block of memory that has 
//   been successfully allocated but not yet initialized to a state in which 
//   it is considered valid for use. The memory block can range from simple 
//   native data types, such as integers, to complex data blobs. Using an 
//   uninitialized memory block results in unpredictable behavior.
//
void UseUninitializedMemory()
{
    // Allocate an array of integer pointers in the process heap.
    const int count = 10;
    int **aIntPtrs = (int **)HeapAlloc(GetProcessHeap(), 0, sizeof(int* [count]));
    if (aIntPtrs == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        return;
    }

    // After the successful allocation of memory for the pointer array, the  
    // memory contains random pointers that are not valid for use. Heap 
    // corruption may happen without the following initialization of the 
    // pointers. The application might experience an access violation if the 
    // address pointed by the pointer is invalid (in the sense that it is not 
    // accessible memory), or it might succeed and gives unpredictable result 
    // if the pointer is pointing to a valid address used elsewhere.
    /*
    // Initialize all elements in the array with valid integer pointers.
    for (int i = 0; i < 10; i++)
    {
    // Allocate memory for the integer.
    aIntPtrs[i] = (int *)HeapAlloc(GetProcessHeap(), 0, sizeof(int));
    }
    */

    wprintf(L"Press ENTER to corrupt the heap ...");
    getwchar();

    // Dereference the first pointer and set its value to 10.
    *(aIntPtrs[0]) = 10;

    /*
    // Uninitialize all elements in the array.
    for (int i = 0; i < 10; i++)
    {
    HeapFree(GetProcessHeap(), 0, aIntPtrs[i]);
    }
    */

    // Free the allocated memory of the array.
    HeapFree(GetProcessHeap(), 0, aIntPtrs);
}

#pragma endregion


#pragma region HeapOverrun

//
//   FUNCTION: OverrunHeap
//
//   PURPOSE: The function demonstrates heap corruption caused by heap 
//   overruns. Heap overruns, like static buffer overruns, can lead to memory 
//   and stack corruption. Because heap overruns occur in heap memory rather 
//   than on the stack, some people consider them to be less able to cause 
//   serious problems; nevertheless, heap overruns require real programming 
//   care and are just as able to allow system risks as static buffer 
//   overruns.
//
void OverrunHeap()
{
    HANDLE hHeap = NULL;
    PBYTE p1 = NULL;
    PBYTE p2 = NULL;
    char *p = NULL;

    // Build the heap environment for the example.

    // Create a private heap.
    hHeap = HeapCreate(0, 0, 0);
    if (hHeap == NULL)
    {
        wprintf(L"HeapCreate failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Create private heap at %08lx\n", hHeap);

    // Allocate 16 bytes + 8 bytes(block header) in the heap.
    p1 = (PBYTE)HeapAlloc(hHeap, 0, 16);
    if (p1 == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block at %p\n", p1);

    // Allocate 24 bytes + 8 bytes(block header) next to the above allocation.
    p2 = (PBYTE)HeapAlloc(hHeap, 0, 24);
    if (p2 == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block at %p\n", p2);

    // Free the first 16 + 8 bytes heap block.
    wprintf(L"Free heap block at %p\n", p1);
    HeapFree(hHeap, 0, p1);
    p1 = NULL;

    // Allocate 16 bytes + 8 bytes(block header) in the heap. The previously 
    // freed block of 16 bytes will be reused.
    p = (char *)HeapAlloc(hHeap, 0, 16);
    if (p == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block at %p\n", p);

    // Overrun the heap by overwriting data to the adjacent blocks.
    wprintf(L"Press ENTER to overrun the heap ...");
    getwchar();

    // a) Heap overrun caused by the incorrect size
    //ZeroMemory(p, 24);  // Overwrite to the next block pointed by p2

    // [-or-]

    // b) Heap overrun when a buffer is written to with more data than it was 
    // allocated to hold, using operations such as CopyMemory, strcat, strcpy, 
    // or wcscpy.
    // The source string can be user's input or the argument of the process.
    char *pszSource = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    strcpy(p, pszSource);  // Overwrite to the next block pointed by p2.

    // So far, no crash happens ...

    // Free the allocated memory. It may trigger Access Violation.
    wprintf(L"Free heap block at %p\n", p);
    HeapFree(hHeap, 0, p);
    p = NULL;

Cleanup:

    if (p1)
    {
        HeapFree(hHeap, 0, p1);
        p1 = NULL;
    }
    if (p2)
    {
        HeapFree(hHeap, 0, p2);
        p2 = NULL;
    }
    if (p)
    {
        HeapFree(hHeap, 0, p);
        p = NULL;
    }
    if (hHeap)
    {
        HeapDestroy(hHeap);
        hHeap = NULL;
    }
}

#pragma endregion


#pragma region MismatchHeapHandle

//
//   FUNCTION: MismatchHeapHandle
//
//   PURPOSE: The function demonstrates heap corruption caused by heap handle 
//   mismatch. The heap manager keeps a list of active heaps in a process. 
//   The heaps are considered separate entities in the sense that the 
//   internal per-heap state is only valid within the context of that 
//   particular heap. Developers working with the heap manager must take 
//   great care to respect this separation by ensuring that the correct heaps 
//   are used when allocating and freeing heap memory. The separation is 
//   exposed to the developer by using heap handles in the heap API calls. 
//   Each heap handle uniquely represents a particular heap in the list of 
//   heaps for the process. If the uniqueness is broken, heap corruption will 
//   ensue.
//
void MismatchHeapHandle()
{
    // Allocate 16 bytes + 8 bytes(block header) in the process heap.
    char *p = (char *)HeapAlloc(GetProcessHeap(), 0, 16);
    if (p)
    {
        wprintf(L"Allocate heap block at %p\n", p);

        wprintf(L"Press ENTER to corrupt the heap ...");
        getwchar();

        // Free the memory in the CRT heap that mismatches the heap of the 
        // allocation.
        wprintf(L"Free heap block at %p\n", p);
        free(p);
    }
}

#pragma endregion


#pragma region ReuseHeapAfterDeletion

//
//   FUNCTION: ReuseHeapAfterDeletion
//
//   PURPOSE: The function demonstrates heap corruption caused by heap reuse 
//   after deletion. Next to heap overruns, heap reuse after deletion is the 
//   second most common source of heap corruptions. After a heap block has 
//   been freed, it is put on the free lists (or look aside list) by the heap 
//   manager. From there on, it is considered invalid for use by the 
//   application. If an application uses the free block in any way, e.g. free 
//   the block again, the state of the block on the free list will most 
//   likely be corrupted.
//
void ReuseHeapAfterDeletion()
{
    HANDLE hHeap = NULL;
    PBYTE p1 = NULL;
    PBYTE p2 = NULL;
    PBYTE p3 = NULL;

    // Build the heap environment for the example.

    // Create a private heap.
    hHeap = HeapCreate(0, 0, 0);
    if (hHeap == NULL)
    {
        wprintf(L"HeapCreate failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Create private heap at %08lx\n", hHeap);

    // Allocate three blocks of 24 bytes + 8 bytes(block header) in the heap.

    p1 = (PBYTE)HeapAlloc(hHeap, 0, 24);
    if (p1 == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block 1 at %p\n", p1);

    p2 = (PBYTE)HeapAlloc(hHeap, 0, 24);
    if (p2 == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block 2 at %p\n", p2);

    p3 = (PBYTE)HeapAlloc(hHeap, 0, 24);
    if (p3 == NULL)
    {
        wprintf(L"HeapAlloc failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    wprintf(L"Allocate heap block 3 at %p\n", p3);

    // Reuse the heap block after freeing.
    wprintf(L"Press ENTER to corrupt the heap");
    getwchar();

    // Free the heap block 1.
    wprintf(L"Free heap block 1 at %p\n", p1);
    HeapFree(hHeap, 0, p1);

    // After the free, the first two DWORDs of the block body becomes FLINK 
    // and BLINK pointers. The heap manager uses the links to walk the free 
    // entries. 

    // a) Corrupt the FLINK and BLINK pointers
    *((DWORD* )p1) = 0xA;
    *((DWORD* )p1 + 1) = 0xB;

    // [-or-]

    // b) Double free the heap block.
    //HeapFree(hHeap, 0, p1);

Cleanup:

    if (p1)
    {
        HeapFree(hHeap, 0, p1);
        p1 = NULL;
    }
    if (p2)
    {
        HeapFree(hHeap, 0, p2);
        p2 = NULL;
    }
    if (p3)
    {
        HeapFree(hHeap, 0, p3);
        p3 = NULL;
    }
    if (hHeap)
    {
        HeapDestroy(hHeap);
        hHeap = NULL;
    }
}

#pragma endregion


void PrintInstructions()
{
    wprintf(L"CppHeapCorruption Instructions:\n" \
        L"-u   Use uninitialized memory to corrupt the heap\n" \
        L"-o   Overrun the heap\n" \
        L"-h   Mismatch the heap handle\n" \
        L"-r   Reuse heap after deletion\n"
        );
}


int wmain(int argc, wchar_t *argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"u", argv[1] + 1) == 0)
        {
            // Use uninitialized memory to corrupt the heap.
            UseUninitializedMemory();
        }
        else if (_wcsicmp(L"o", argv[1] + 1) == 0)
        {
            // Overrun the heap.
            OverrunHeap();
        }
        else if (_wcsicmp(L"h", argv[1] + 1) == 0)
        {
            // Mismatch the heap handle.
            MismatchHeapHandle();
        }
        else if (_wcsicmp(L"r", argv[1] + 1) == 0)
        {
            // Reuse heap after deletion.
            ReuseHeapAfterDeletion();
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