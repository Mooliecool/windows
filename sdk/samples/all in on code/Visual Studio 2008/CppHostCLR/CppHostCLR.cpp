/****************************** Module Header ******************************\
Module Name:  CppHostCLR.cpp
Project:      CppHostCLR
Copyright (c) Microsoft Corporation.

The Common Language Runtime (CLR) allows a level of integration between 
itself and a host. This C++ code sample demonstrates using the Hosting 
Interfaces of .NET Framework 1.0, 1.1 and 2.0 to host a specific version of 
CLR in the process, load a .NET assembly, and invoke the types in the 
assembly.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.h"


// Demo of .NET Framework 1.0 and 1.1 Hosting Interfaces
HRESULT RuntimeHostV1Demo();

// Demo of .NET Framework 2.0 Hosting Interfaces
HRESULT RuntimeHostV2Demo();


int wmain(int argc, wchar_t *argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"v1", argv[1] + 1) == 0)
        {
            // If the command parameter is '-v1', demo .NET Framework 1.0 and 
            // 1.1 Hosting Interfaces.
            RuntimeHostV1Demo();
        }
        else if (_wcsicmp(L"v2", argv[1] + 1) == 0)
        {
            // If the command parameter is '-v2', demo .NET Framework 2.0 
            // Hosting Interfaces.
            RuntimeHostV2Demo();
        }
    }
    else
    {
        // By default, demo .NET Framework 2.0 Hosting Interfaces.
        RuntimeHostV2Demo();
    }

	return 0;
}