/****************************** Module Header ******************************\
Module Name:    PlatformDetector.cpp
Project:        CppPlatformDetector
Copyright (c) Microsoft Corporation.

Implements the helper functions for detecting the platform. 

GetOSName - Gets the name of the currently running operating system. For 
            example, "Microsoft Windows 7 Enterprise".
GetOSVersionString - Gets the concatenated string representation of the 
            platform identifier, version, and service pack that are currently 
            installed on the operating system. 
Is64BitOS - Determines whether the current operating system is a 64-bit 
            operating system.
Is64BitProcess - Determines whether the currently running process or an 
            arbitrary process running on the system is a 64-bit process.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "PlatformDetector.h"
#include <strsafe.h>

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#include <comdef.h>
#include <wincred.h>


//
//   FUNCTION: GetOSName(PWSTR, DWORD)
//
//   PURPOSE: Gets the name of the currently running operating system. For 
//   example, "Microsoft Windows 7 Enterprise".
//
//   PARAMETERS:
//   * pszName - The buffer for receiving the OS name. 
//   * cch - The size of buffer pointed by pszName, in characters.
//
//   RETURN VALUE: The function returns TRUE if it succeeds.
//
BOOL GetOSName(PWSTR pszName, DWORD cch)
{
    HRESULT hr = S_OK;
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    PCWSTR pszMachineName = L".";

    // Initialize COM parameters with a call to CoInitializeEx.
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return FALSE;
    }

    // Initialize COM process security by calling CoInitializeSecurity. 
    hr = CoInitializeSecurity(
        NULL, 
        -1,                             // COM authentication
        NULL,                           // Authentication services
        NULL,                           // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,      // Default authentication 
        RPC_C_IMP_LEVEL_IDENTIFY,       // Default Impersonation  
        NULL,                           // Authentication info
        EOAC_NONE,                      // Additional capabilities 
        NULL                            // Reserved
        );
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Obtain the initial locator to WMI by calling CoCreateInstance. 
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, reinterpret_cast<LPVOID *>(&pLoc));
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method.
    wchar_t szPath[200];
    hr = StringCchPrintf(szPath, ARRAYSIZE(szPath), L"\\\\%s\\root\\cimv2", 
        pszMachineName);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    hr = pLoc->ConnectServer(
        bstr_t(szPath),                 // Path of the WMI namespace
        NULL,                           // User name
        NULL,                           // User password
        NULL,                           // Locale
        NULL,                           // Security flags
        NULL,                           // Authority
        NULL,                           // Context object 
        &pSvc                           // IWbemServices proxy
        );
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Set security levels on the WMI connection.
    hr = CoSetProxyBlanket(
        pSvc,                           // Indicates the proxy to set
        RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
        COLE_DEFAULT_PRINCIPAL,         // Server principal name 
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
        NULL,                           // Client identity
        EOAC_NONE                       // Proxy capabilities 
        );
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Use the IWbemServices pointer to make requests of WMI. Query 
    // Win32_OperatingSystem.Caption which dicates the name of the current 
    // operating system. 
    hr = pSvc->ExecQuery(bstr_t(L"WQL"), 
        bstr_t(L"SELECT Caption FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL, &pEnumerator);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Secure the enumerator proxy.
    hr = CoSetProxyBlanket(
        pEnumerator,                    // Indicates the proxy to set
        RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
        COLE_DEFAULT_PRINCIPAL,         // Server principal name 
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
        NULL,                           // Client identity
        EOAC_NONE                       // Proxy capabilities 
        );
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Get the data from the above query.
    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        // Get one object.
        HRESULT hrTmp = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Win32_OperatingSystem.Caption property.
        hrTmp = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrTmp))
        {
            hr = StringCchCopy(pszName, cch, vtProp.bstrVal);

            VariantClear(&vtProp);
        }

        pclsObj->Release();
        pclsObj = NULL;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pLoc)
    {
        pLoc->Release();
        pLoc = NULL;
    }
    if (pSvc)
    {
        pSvc->Release();
        pSvc = NULL;
    }
    if (pEnumerator)
    {
        pEnumerator->Release();
        pEnumerator = NULL;
    }

    CoUninitialize();

    return SUCCEEDED(hr);
}


//
//   FUNCTION: GetOSVersionString(PWSTR, DWORD)
//
//   PURPOSE: Gets the concatenated string representation of the platform 
//   identifier, version, and service pack that are currently installed on 
//   the operating system. For example, 
//   "Microsoft Windows NT 6.1.7600.0 Workstation"
//
//   PARAMETERS:
//   * pszVersionString - The buffer for receiving the OS version string. 
//   * cch - The size of buffer pointed by pszVersionString, in characters.
//
//   RETURN VALUE: The function returns TRUE if it succeeds.
//
BOOL GetOSVersionString(PWSTR pszVersionString, DWORD cch)
{
    // Call GetVersionEx to detect the current OS version. If compatibility 
    // mode is in effect, the GetVersionEx function reports the operating 
    // system as it identifies itself, which may not be the operating system 
    // that is installed. For example, if compatibility mode is in effect, 
    // GetVersionEx reports the operating system that is selected for 
    // application compatibility.
    OSVERSIONINFOEX osvi = { sizeof(osvi) };
    if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi)))
    {
        return FALSE;
    }

    PCWSTR pszPlatform = NULL;
    PCWSTR pszProductType = L"";

    switch (osvi.dwPlatformId)
    {
    case VER_PLATFORM_WIN32s:
        // Microsoft Win32S
        pszPlatform = L"Microsoft Win32S";
        break;

    case VER_PLATFORM_WIN32_WINDOWS:
        if (osvi.dwMajorVersion > 4 || 
            (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion > 0))
        {
            // Microsoft Windows 98
            pszPlatform = L"Microsoft Windows 98";
        }
        else
        {
            // Microsoft Windows 95
            pszPlatform = L"Microsoft Windows 95";
        }
        break;

    case VER_PLATFORM_WIN32_NT:
        // Microsoft Windows NT
        pszPlatform = L"Microsoft Windows NT";

        switch (osvi.wProductType)
        {
        case VER_NT_WORKSTATION:
            pszProductType = L"Workstation";
            break;
        case VER_NT_DOMAIN_CONTROLLER:
            pszProductType = L"DC";
            break;
        case VER_NT_SERVER:
            pszProductType = L"Server";
            break;
        }
        break;

    default:
        // Unknown
        pszPlatform = L"<unknown>";
    }

    HRESULT hr = StringCchPrintf(pszVersionString, cch, 
        L"%s %d.%d.%d.%d %s", 
        pszPlatform, 
        osvi.dwMajorVersion, 
        osvi.dwMinorVersion, 
        osvi.dwBuildNumber, 
        osvi.wServicePackMajor << 0x10 | osvi.wServicePackMinor, 
        pszProductType);

    return (SUCCEEDED(hr));
}


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
//   PURPOSE: Determines whether the current operating system is a 64-bit 
//   operating system.
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
//   PURPOSE: Determines whether the currently running process is a 64-bit 
//   process.
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
//   PURPOSE: Determines whether the specified process is a 64-bit process.
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
        // On 64-bit OS, if a process is not running under Wow64 mode, the 
        // process must be a 64-bit process.
        f64bitProc = !(SafeIsWow64Process(hProcess, &f64bitProc) && f64bitProc);
    }

    return f64bitProc;
}