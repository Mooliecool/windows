/****************************** Module Header ******************************\
Module Name:  CppMailslotClient.cpp
Project:      CppMailslotClient
Copyright (c) Microsoft Corporation.

Mailslot is a mechanism for one-way inter-process communication in the local 
machine or across the computers in the intranet. Any clients can store 
messages in a mailslot. The creator of the slot, i.e. the server, retrieves 
the messages that are stored there:

Client (GENERIC_WRITE) ---> Server (GENERIC_READ)

This sample demonstrates a mailslot client that connects and writes to the 
mailslot "\\.\mailslot\SampleMailslot". 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#pragma endregion


void WriteMailslot(HANDLE hMailslot, PCWSTR pszMessage);


int wmain(int argc, wchar_t *argv[])
{
    // The name of the mailslot. It is in the form of \\.\mailslot\[path]name 
    // The name field must be unique. The name may include multiple levels of 
    // pseudo directories separated by backslashes. For example, both 
    // \\.\mailslot\mailslot_name and \\.\mailslot\abc\def\ghi are valid.
    const PCWSTR MAILSLOT_NAME = L"\\\\.\\mailslot\\SampleMailslot";

    HANDLE hMailslot = INVALID_HANDLE_VALUE;
    DWORD dwError = ERROR_SUCCESS;

    // Open the mailslot with write access (Mailslot is a mechanism for one-
    // way IPC. The client is just responsible for writing to the mailslot.)
    hMailslot = CreateFile(
        MAILSLOT_NAME,              // The name of the mailslot
        GENERIC_WRITE,              // Write access
        FILE_SHARE_READ,            // Share mode
        NULL,                       // Default security attributes
        OPEN_EXISTING,              // Opens existing mailslot
        FILE_ATTRIBUTE_NORMAL,      // The file has no other attributes set
        NULL                        // No template file
        );
    if (hMailslot == INVALID_HANDLE_VALUE) 
    {
        dwError = GetLastError();
        wprintf(L"Unable to open mailslot w/err 0x%08lx\n", dwError);
        goto Cleanup;
    }

    wprintf(L"The mailslot (%s) is opened.\n", MAILSLOT_NAME);

    // Write messages to the mailslot.

    WriteMailslot(hMailslot, L"Message 1 for mailslot");
    WriteMailslot(hMailslot, L"Message 2 for mailslot");
    Sleep(3000); // Sleep for 3 seconds for the demo purpose
    WriteMailslot(hMailslot, L"Message 3 for mailslot");

Cleanup:

    // Centralized cleanup for all allocated resources.
    if (hMailslot != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hMailslot);
        hMailslot = INVALID_HANDLE_VALUE;
    }

    return dwError;
}


//
//   FUNCTION: WriteMailslot(HANDLE, PCWSTR);
//
//   PURPOSE: Write a message to the specified mailslot.
//
//   PARAMETERS:
//      * hMailslot - The handle of the mailslot.
//      * pszMessage - The message to be written to the slot.
//
void WriteMailslot(HANDLE hMailslot, PCWSTR pszMessage)
{
    assert(hMailslot != INVALID_HANDLE_VALUE);
    assert(pszMessage != NULL);

    DWORD cbMessageBytes = 0;       // Message size in bytes
    DWORD cbBytesWritten = 0;       // Number of bytes written to the slot

    // Calculate the message size (including the NULL terminator) in bytes.
    cbMessageBytes = (wcslen(pszMessage) + 1) * sizeof(*pszMessage);

    BOOL fSucceeded = WriteFile(    // Write to the mailslot.
        hMailslot,                  // Handle of the slot
        pszMessage,                 // Message to be written
        cbMessageBytes,             // Number of bytes to write
        &cbBytesWritten,            // Number of bytes written
        NULL                        // Not overlapped
        );

    if (!fSucceeded || cbMessageBytes != cbBytesWritten) 
    {
        wprintf(L"WriteFile failed w/err 0x%08lx\n", GetLastError());
    }
    else
    {
        wprintf(L"The message \"%s\" is written to the slot\n", pszMessage);
    }
}