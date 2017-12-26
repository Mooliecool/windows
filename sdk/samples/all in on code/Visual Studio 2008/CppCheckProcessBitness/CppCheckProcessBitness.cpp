/****************************** Module Header ******************************\
Module Name:  CppCheckProcessBitness.cpp
Project:      CppCheckProcessBitness
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to determine whether the given process is
a 64-bit process or not.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

//
//   FUNCTION: SafeIsWow64Process(HANDLE, PBOOL)
//
//   PURPOSE: This is a wrapper of the IsWow64Process API. It determines 
//   whether the specified process is running under WOW64. IsWow64Process 
//   does not exist prior to Windows XP with SP2 and Window Server 2003  with 
//   SP1. For compatibility with operating systems that do not support 
//   IsWow64Process, call GetProcAddress to detect whether IsWow64Process is 
///  implemented in Kernel32.dll. If GetProcAddress succeeds, it is safe to 
//   call IsWow64Process dynamically. Otherwise, WOW64 is not present.
//
//   PARAMETERS:
//   * hProcess - A handle to the process. 
//   * Wow64Process - A pointer to a value that is set to TRUE if the process 
//     is running under WOW64. If the process is running under 32-bit Windows, 
//     the value is set to FALSE. If the process is a 64-bit application 
//     running under 64-bit Windows, the value is also set to FALSE.
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE.If 
//   IsWow64Process does not exist in kernel32.dll, or the function fails, 
//   the return value is FALSE. 
//
BOOL WINAPI SafeIsWow64Process(HANDLE hProcess, PBOOL Wow64Process)
{
    if (fnIsWow64Process == NULL)
    {
        // IsWow64Process is not available on all supported versions of 
        // Windows. Use GetModuleHandle to get a handle to the DLL that 
        // contains the function, and GetProcAddress to get a pointer to the 
        // function if available.
        HMODULE hModule = GetModuleHandle(L"kernel32.dll");
        if (hModule == NULL)
        {
            return FALSE;
        }
        
        fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(
            GetProcAddress(hModule, "IsWow64Process"));
        if (fnIsWow64Process == NULL)
        {
            return FALSE;
        }
    }
    return fnIsWow64Process(hProcess, Wow64Process);
}


//
//   FUNCTION: Is64BitOS()
//
//   PURPOSE: The function determines whether the current operating system is 
//   a 64-bit operating system.
//
//   RETURN VALUE: The function returns TRUE if the operating system is 
//   64-bit; otherwise, it returns FALSE.
//
BOOL Is64BitOS()
{
#if defined(_WIN64)
    return TRUE;   // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    BOOL f64bitOS = FALSE;
    return (SafeIsWow64Process(GetCurrentProcess(), &f64bitOS) && f64bitOS);
#else
    return FALSE;  // 64-bit Windows does not support Win16
#endif
}


//
//   FUNCTION: Is64BitProcess(void)
//   
//   PURPOSE: The function determines whether the currently running process 
//   is a 64-bit process.
//
//   RETURN VALUE: The function returns TRUE if the currently running process 
//   is 64-bit; otherwise, it returns FALSE.
//
BOOL Is64BitProcess(void)
{
#if defined(_WIN64)
    return TRUE;   // 64-bit program
#else
    return FALSE;
#endif
}


//
//   FUNCTION: Is64BitProcess(HANDLE)
//   
//   PURPOSE: The function determines whether the specified process is a 
//   64-bit process.
//
//   PARAMETERS:
//   * hProcess - the process handle.
//
//   RETURN VALUE: The function returns TRUE if the given process is 64-bit;
//   otherwise, it returns FALSE.
//
BOOL Is64BitProcess(HANDLE hProcess)
{
    BOOL f64bitProc = FALSE;

    if (Is64BitOS())
    {
        // On 64bit OS, if a process is not running under Wow64 mode, the 
        // process must be a 64bit process.
        f64bitProc = !(SafeIsWow64Process(hProcess, &f64bitProc) && f64bitProc);
    }

    return f64bitProc;
}


int wmain(int argc, wchar_t* argv[])
{
    if (argc > 1)
    {
        // If a process ID is specified in the command line, get the process 
        // id, and open the process handle.
        DWORD dwProcessId = _wtoi(argv[1]);
        if (dwProcessId != 0 /*conversion succeeds*/)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
                dwProcessId);
            if (hProcess != NULL)
            {
                // Detect whether the specified process is a 64-bit.
                BOOL f64bitProc = Is64BitProcess(hProcess);
                wprintf(L"Process %d is %s64-bit\n", dwProcessId, 
                    f64bitProc ? L"" : L"not ");

                CloseHandle(hProcess);
            }
            else
            {
                wprintf(L"OpenProcess(%d) failed w/err 0x%08lx\n", 
                    dwProcessId, GetLastError());
            }
        }
        else
        {
            wprintf(L"Invalid process ID: %s\n", argv[1]);
        }
    }
    else
    {
        // If no process id was specified, use the ID of the current process.
        BOOL f64bitProc = Is64BitProcess();
        wprintf(L"Current process is %s64-bit\n", f64bitProc ? L"" : L"not ");
    }

    return 0;
}