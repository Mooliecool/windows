/****************************** Module Header ******************************\
Module Name:  CppStackOverflow.cpp
Project:      CppStackOverflow
Copyright (c) Microsoft Corporation.

CppStackOverflow is designed to show how stack overflow happens in C++ 
applications. When a thread is created, 1MB of virtual memory is reserved for 
use by the thread as a stack. Unlike the heap, it does not expand as needed. 
Its initial size can be changed via /STACK linker switch (See Project 
Property Pages / Linker / System / Stack Reserve Size). When too much memory 
is used on the call stack the stack is said to overflow, typically resulting 
in a program crash. This class of software bug is usually caused by one of 
two types of programming errors: allocation of large stack variable and 
deeply recursive function calls.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


void AllocateLargeStackVariable()
{
    // Stack is 1MB by default and unlike the heap, it does not expand as 
    // needed. Its initial size can be changed via /STACK linker switch. 
    // Here we allocate 2MB stack variable and it will overflow the stack 
    // directly.

    char buffer[2000000];

    // Ensure that the buffer is not omitted in the release build.
    strcpy_s(buffer, ARRAYSIZE(buffer), "Allocation succeeded\n");
    printf(buffer);
}


void CallRecursiveFunction(int level)
{
    int buffer[260];

    // Initialize the buffer with unique characters so you can see each 
    // allocation on the stack.
    for (int i = 0; i < ARRAYSIZE(buffer); i++)
    {
        buffer[i] = level;
    }

    // Recursively call the function.
    CallRecursiveFunction(level + 1);
}


void PrintInstructions()
{
    wprintf(L"CppStackOverflow Instructions:\n" \
        L"-a   Allocate large stack variable to overflow the stack\n" \
        L"-r   Call deeply recursive function to overflow the stack\n"
        );
}


int wmain(int argc, wchar_t *argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"a", argv[1] + 1) == 0)
        {
            wprintf(L"Press ENTER to allocate large stack variable ...");
            getwchar();

            // Allocate large stack variable to overflow the stack.
            AllocateLargeStackVariable();
        }
        else if (_wcsicmp(L"r", argv[1] + 1) == 0)
        {
            wprintf(L"Press ENTER to call deeply recursive function ...");
            getwchar();

            // Call deeply recursive function to overflow the stack.
            CallRecursiveFunction(0);
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