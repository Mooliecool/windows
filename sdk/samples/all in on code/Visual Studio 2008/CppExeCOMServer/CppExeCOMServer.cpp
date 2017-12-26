/****************************** Module Header ******************************\
Module Name:  CppExeCOMServer.cpp
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <windows.h>
#include <strsafe.h>
#include "CppExeCOMServer_i.c"      // For component GUIDs
#include "Reg.h"


STDAPI ExeRegisterServer(void);
STDAPI ExeUnregisterServer(void);



int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int nCmdShow)
{
    HRESULT hr; 

    if (*lpCmdLine == L'-' || *lpCmdLine == L'/')
    {
        if (_wcsicmp(L"regserver", lpCmdLine + 1) == 0)
        {
            // Register the COM server.
            hr = ExeRegisterServer();
            if (FAILED(hr))
            {
            }
        }
        else if (_wcsicmp(L"unregserver", lpCmdLine + 1) == 0)
        {
            // Unregister the COM server.
            hr = ExeUnregisterServer();
            if (FAILED(hr))
            {
            }
        }
    }
    else
    {
        // Call PreMessageLoop to initialize the member variables and 
        // register the class factories.
        hr = PreMessageLoop();
        
        if (SUCCEEDED(hr))
        {
            __try
            {
                // Run the message loop.
                RunMessageLoop();
            }
            __finally
            {
                // Call PostMessageLoop to revoke the registration.
                hr = PostMessageLoop();
            }
        }
    }

	return hr;
}


// Register the COM server.
STDAPI ExeRegisterServer(void)
{
    HRESULT hr;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(NULL, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Register the component.
    hr = RegisterLocalServer(szModule, 
        CLSID_SimpleObject, 
        L"CppExeCOMServer.SimpleObject Class", 
        LIBID_CppExeCOMServerLib,
        L"CppExeCOMServer.SimpleObject.1", 
        L"CppExeCOMServer.SimpleObject");
    if (SUCCEEDED(hr))
    {
        // Register the type library.
        hr = RegisterTypeLib(szModule);
    }

    return hr;
}


// Unregister the COM server.
STDAPI ExeUnregisterServer(void)
{
    HRESULT hr = S_OK;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(NULL, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Unregister the component.
    hr = UnregisterLocalServer(CLSID_SimpleObject, 
        L"CppExeCOMServer.SimpleObject.1", 
        L"CppExeCOMServer.SimpleObject");
    if (SUCCEEDED(hr))
    {
        // Unregister the type library.
        hr = UnregisterTypeLib(szModule);
    }

    return hr;
}