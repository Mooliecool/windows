/****************************** Module Header ******************************\
Module Name:  CppOutOfMemory.cpp
Project:      CppOutOfMemory
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


#pragma region Heap Fragmentation

//
//   FUNCTION: HaveHeapFragmentation
//
//   PURPOSE: 
//
void HaveHeapFragmentation()
{
    wprintf(L"Press ENTER to build memory environment ...");
    getwchar();

    const int numAllocs = 0x6000;

    PVOID separators[numAllocs] = {};
    const int separatorSize = 0x400; // 1KB

    PVOID fragments[numAllocs] = {};
    const int fragmentSize = 0x10000; // 64KB

    for (int i = 0; i < numAllocs; i++)
    {
        // Allocate a 1KB separator memory block.
        separators[i] = LocalAlloc(LPTR, separatorSize);
        if (separators[i] == NULL)
        {
            // Failed to build the memory environment. Exit directly.
            wprintf(L"The application failed to build memory environment " \
                L"w/err 0x%08lx\n", GetLastError());
            exit(0);
        }

        // Allocate a 64KB fragment memory block.
        fragments[i] = LocalAlloc(LPTR, fragmentSize);
        if (fragments[i] == NULL)
        {
            // Failed to build the memory environment. Exit directly.
            wprintf(L"The application failed to build memory environment " \
                L"w/err 0x%08lx\n", GetLastError());
            exit(0);
        }
    }

    // So far the function has allocated 24MB (0x6000 * 1KB) memory blocks 
    // and 1.5GB (0x6000 * 64KB) fragmented memory blocks. 

    // Free the fragmented memory blocks. This will theoretically free 1.5GB 
    // (0x6000 x 64KB) memory. However, the large freed blocks are interwined 
    // with the live separator memory blocks. It results in a highly 
    // fragmented heap.
    for (int i = 0; i < numAllocs; i++)
    {
        if (fragments[i] != NULL)
        {
            LocalFree(fragments[i]);
            fragments[i] = NULL;
        }
    }

    wprintf(L"Press ENTER to cause \"out of memory\" ...");
    getwchar();
    
    // Allocate 0x4CCC 80KB memory blocks, whose total size equals the 
    // previously freed memory blocks: 1.5GB. Since the total allocation 
    // sizes are equivalent, it appears that the allocation will succeed. 
    // However, because of the highly fragmented heap, the allocation will 
    // fail with the ERROR_NOT_ENOUGH_MEMORY (0x8) error very soon.
    const int newNumAllocs = 0x4CCC;
    PVOID newBuffers[newNumAllocs] = {};
    const int newSize = 0x14000; // 80KB

    for (int i = 0; i < newNumAllocs; i++)
    {
        // Allocate a 80KB memory block.
        newBuffers[i] = LocalAlloc(LPTR, newSize);
        if (newBuffers[i] == NULL)
        {
            // Failed to allocate the requested memory. Exit directly.
            wprintf(L"The application failed to allocate the requested " \
                L"memory w/err 0x%08lx\n", GetLastError());
            exit(0);
        }
    }

    // Free all allocated memory.
    for (int i = 0; i < numAllocs; i++)
    {
        if (separators[i] != NULL)
        {
            LocalFree(separators[i]);
            separators[i] = NULL;
        }
    }

    for (int i = 0; i < newNumAllocs; i++)
    {
        if (newBuffers[i] != NULL)
        {
            LocalFree(newBuffers[i]);
            newBuffers[i] = NULL;
        }
    }
}

#pragma endregion


void PrintInstructions()
{
    wprintf(L"CppOutOfMemory Instructions:\n" \
        L"-f   Out of memory caused by heap fragmentation\n" \
        L"-h   Out of memory caused by high memory usage\n"
        );
}


int wmain(int argc, wchar_t *argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"f", argv[1] + 1) == 0)
        {
            // Out of memory caused by heap fragmentation.
            HaveHeapFragmentation();
        }
        else if (_wcsicmp(L"h", argv[1] + 1) == 0)
        {
            // Out of memory caused by high memory usage.
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