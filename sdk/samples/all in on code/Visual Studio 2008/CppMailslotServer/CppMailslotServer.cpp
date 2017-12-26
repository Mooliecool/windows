/****************************** Module Header ******************************\
Module Name:  CppMailslotServer.cpp
Project:      CppMailslotServer
Copyright (c) Microsoft Corporation.

Mailslot is a mechanism for one-way inter-process communication in the local 
machine or across the computers in the intranet. Any clients can store 
messages in a mailslot. The creator of the slot, i.e. the server, retrieves 
the messages that are stored there:

Client (GENERIC_WRITE) ---> Server (GENERIC_READ)

This code sample demonstrates calling CreateMailslot to create a mailslot 
named "\\.\mailslot\SampleMailslot". The security attributes of the slot are  
customized to allow Authenticated Users read and write access to the slot, 
and to allow the Administrators group full access to it. The sample first 
creates such a mailslot, then it reads and displays new messages in the slot 
when user presses ENTER in the console.

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
#include <sddl.h>
#pragma endregion


BOOL ReadMailslot(HANDLE hMailslot);
BOOL CreateMailslotSecurity(PSECURITY_ATTRIBUTES *ppSa);
void FreeMailslotSecurity(PSECURITY_ATTRIBUTES pSa);


int wmain(int argc, wchar_t *argv[])
{
    // The name of the mailslot. It is in the form of \\.\mailslot\[path]name 
    // The name field must be unique. The name may include multiple levels of 
    // pseudo directories separated by backslashes. For example, both 
    // \\.\mailslot\mailslot_name and \\.\mailslot\abc\def\ghi are valid.
    const PCWSTR MAILSLOT_NAME = L"\\\\.\\mailslot\\SampleMailslot";

    DWORD dwError = ERROR_SUCCESS;
    PSECURITY_ATTRIBUTES pSa = NULL;
    HANDLE hMailslot = INVALID_HANDLE_VALUE;

    // Prepare the security attributes (the lpSecurityAttributes parameter in 
    // CreateMailslot) for the mailslot. This is optional. If the 
    // lpSecurityAttributes parameter of CreateMailslot is NULL, the mailslot 
    // gets a default security descriptor and the handle cannot be inherited.
    // The ACLs in the default security descriptor of a mailslot grant full 
    // control to the LocalSystem account, (elevated) administrators, and the 
    // creator owner. They also give only read access to members of the 
    // Everyone group and the anonymous account. However, if you want to 
    // customize the security permission of the mailslot, (e.g. to allow 
    // Authenticated Users to read from and write to the pipe), you need to 
    // create a SECURITY_ATTRIBUTES structure.
    if (!CreateMailslotSecurity(&pSa))
    {
        dwError = GetLastError();
        wprintf(L"CreateMailslotSecurity failed w/err 0x%08lx\n", dwError);
        goto Cleanup;
    }

    // Create the mailslot.
    hMailslot = CreateMailslot(
        MAILSLOT_NAME,              // The name of the mailslot
        0,                          // No maximum message size 
        MAILSLOT_WAIT_FOREVER,      // No time-out for operations 
        pSa                         // Security attributes
        );

    if (hMailslot == INVALID_HANDLE_VALUE) 
    {
        dwError = GetLastError();
        wprintf(L"Unable to create mailslot w/err 0x%08lx\n", dwError);
        goto Cleanup;
    }

    wprintf(L"The mailslot (%s) is created.\n", MAILSLOT_NAME);

    // Check messages in the mailslot.
    wprintf(L"Press ENTER to check new messages or press Q to quit ...");
    char cmd = getchar();
    while (cmd != 'Q' && cmd != 'q')
    {
        if (cmd == '\n')
        {
            wprintf(L"Checking new messages...\n");
            ReadMailslot(hMailslot);

            wprintf(L"Press ENTER to check new messages or press Q to quit ...");
        }
        cmd = getchar();
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pSa != NULL)
    {
        FreeMailslotSecurity(pSa);
        pSa = NULL;
    }
    if (hMailslot != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hMailslot);
        hMailslot = INVALID_HANDLE_VALUE;
    }

    return 0;
}


//
//   FUNCTION: ReadMailslot(HANDLE);
//
//   PURPOSE: Read the messages from a mailslot by using the mailslot handle 
//   in a call to the ReadFile function. 
//
//   PARAMETERS:
//      * hMailslot - The handle of the mailslot.
//
BOOL ReadMailslot(HANDLE hMailslot) 
{
    assert(hMailslot != INVALID_HANDLE_VALUE);

    DWORD cbMessageBytes = 0;       // Size of the message in bytes
    DWORD cbBytesRead = 0;          // Number of bytes read from the slot
    DWORD cMessages = 0;            // Number of messages in the mailslot
    DWORD nMessageId = 0;           // Message ID

    BOOL fSucceeded = FALSE;

    PWSTR pszBuffer;                // A buffer used to store one message

    // Check for the number of messages in the mailslot.
    fSucceeded = GetMailslotInfo(
        hMailslot,                  // Handle of the mailslot
        NULL,                       // No maximum message size 
        &cbMessageBytes,            // Size of next message 
        &cMessages,                 // Number of messages 
        NULL                        // No read time-out 
        );
    if (!fSucceeded) 
    {
        wprintf(L"GetMailslotInfo failed w/err 0x%08lx\n", GetLastError()); 
        return fSucceeded;
    }

    if (cbMessageBytes == MAILSLOT_NO_MESSAGE) 
    { 
        // There are no new messages in the mailslot at present.
        wprintf(L"No new messages.\n");
        return fSucceeded;
    }

    // Retrieve the messages one by one from the mailslot.
    while (cMessages != 0)
    {
        nMessageId++;

        // Allocate the memory for the message based on its size info, 
        // cbMessageBytes, which was retrieved from GetMailslotInfo.
        pszBuffer = (PWSTR)LocalAlloc(LPTR, cbMessageBytes);
        if (NULL == pszBuffer)
        {
            fSucceeded = FALSE;
            break;
        }

        // Read from the mailslot.
        fSucceeded = ReadFile(
            hMailslot,              // Handle of the slot
            pszBuffer,              // Buffer to receive data
            cbMessageBytes,         // Size of buffer in bytes
            &cbBytesRead,           // Number of bytes read
            NULL                    // Not overlapped I/O
            );
        if (!fSucceeded) 
        { 
            wprintf(L"ReadFile failed w/err 0x%08lx\n", GetLastError()); 
            LocalFree(pszBuffer);
            break;
        }

        // Display the message. 
        wprintf(L"Message #%ld: %s\n", nMessageId, pszBuffer); 

        LocalFree(pszBuffer);

        // Get the current number of un-read messages in the slot. The number
        // may not equal the initial message number because new messages may 
        // arrive while we are reading the items in the slot.
        fSucceeded = GetMailslotInfo(
            hMailslot,              // Handle of the mailslot
            NULL,                   // No maximum message size 
            &cbMessageBytes,        // Size of next message 
            &cMessages,             // Number of messages 
            NULL                    // No read time-out 
            );
        if (!fSucceeded) 
        {
            wprintf(L"GetMailslotInfo failed w/err 0x%08lx\n", GetLastError());
            break;
        }
    }

    return fSucceeded; 
}


//
//   FUNCTION: CreateMailslotSecurity(PSECURITY_ATTRIBUTES *)
//
//   PURPOSE: The CreateMailslotSecurity function creates and initializes a 
//   new SECURITY_ATTRIBUTES structure to allow Authenticated Users read and 
//   write access to a mailslot, and to allow the Administrators group full 
//   access to the mailslot.
//
//   PARAMETERS:
//   * ppSa - output a pointer to a SECURITY_ATTRIBUTES structure that allows 
//     Authenticated Users read and write access to a mailslot, and allows  
//     the Administrators group full access to the mailslot. The structure 
//     must be freed by calling FreeMailslotSecurity.
//
//   RETURN VALUE: Returns TRUE if the function succeeds.
//
//   EXAMPLE CALL:
//
//     PSECURITY_ATTRIBUTES pSa = NULL;
//     if (CreateMailslotSecurity(&pSa))
//     {
//         // Use the security attributes
//         // ...
//
//         FreeMailslotSecurity(pSa);
//     }
//
BOOL CreateMailslotSecurity(PSECURITY_ATTRIBUTES *ppSa)
{
    BOOL fSucceeded = TRUE;
    DWORD dwError = ERROR_SUCCESS;

    PSECURITY_DESCRIPTOR pSd = NULL;
    PSECURITY_ATTRIBUTES pSa = NULL;

    // Define the SDDL for the security descriptor.
    PCWSTR szSDDL = L"D:"       // Discretionary ACL
        L"(A;OICI;GRGW;;;AU)"   // Allow read/write to authenticated users
        L"(A;OICI;GA;;;BA)";    // Allow full control to administrators

    if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSDDL, 
        SDDL_REVISION_1, &pSd, NULL))
    {
        fSucceeded = FALSE;
        dwError = GetLastError();
        goto Cleanup;
    }
    
    // Allocate the memory of SECURITY_ATTRIBUTES.
    pSa = (PSECURITY_ATTRIBUTES)LocalAlloc(LPTR, sizeof(*pSa));
    if (pSa == NULL)
    {
        fSucceeded = FALSE;
        dwError = GetLastError();
        goto Cleanup;
    }

    pSa->nLength = sizeof(*pSa);
    pSa->lpSecurityDescriptor = pSd;
    pSa->bInheritHandle = FALSE;

    *ppSa = pSa;

Cleanup:
    // Clean up the allocated resources if something is wrong.
    if (!fSucceeded)
    {
        if (pSd)
        {
            LocalFree(pSd);
            pSd = NULL;
        }
        if (pSa)
        {
            LocalFree(pSa);
            pSa = NULL;
        }

        SetLastError(dwError);
    }
    
    return fSucceeded;
}


//
//   FUNCTION: FreeMailslotSecurity(PSECURITY_ATTRIBUTES)
//
//   PURPOSE: The FreeMailslotSecurity function frees a SECURITY_ATTRIBUTES 
//   structure that was created by the CreateMailslotSecurity function. 
//
//   PARAMETERS:
//   * pSa - pointer to a SECURITY_ATTRIBUTES structure that was created by 
//     the CreateMailslotSecurity function. 
//
void FreeMailslotSecurity(PSECURITY_ATTRIBUTES pSa)
{
    if (pSa)
    {
        if (pSa->lpSecurityDescriptor)
        {
            LocalFree(pSa->lpSecurityDescriptor);
        }
        LocalFree(pSa);
    }
}