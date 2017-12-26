/****************************** Module Header ******************************\
* Module Name:  CppRegisterTypeLib.cpp
* Project:      CppRegisterTypeLib
* Copyright (c) Microsoft Corporation.
* 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#pragma endregion


int Register(LPCSTR pName)
{
    wchar_t     szFile[_MAX_PATH];
    LPTSTR      lpMsgBuf = NULL;
    DWORD       dwStatus = 0;
    ITypeLib    *pTLB=0;
    HRESULT     hr;

    MultiByteToWideChar(CP_ACP, 0, pName,-1, szFile,_MAX_PATH);
    hr = LoadTypeLibEx(szFile, REGKIND_REGISTER, &pTLB);

    if (pTLB)
        pTLB->Release();

    if (SUCCEEDED(hr))
        return 0;

    dwStatus = FormatMessageA( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            (DWORD)hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,  // (see MSDN docs for FORMAT_MESSAGE_ALLOCATE_BUFFER)
            0,
            NULL 
        );

    if (dwStatus) 
    {
        printf("Register %ls : (0x%X) %s", szFile, hr, (LPTSTR) lpMsgBuf);
        LocalFree( lpMsgBuf );
    }
    else
        printf("Register %ls: return code 0x%X\n", szFile, hr);

    return 1;
}


int _tmain(int argc, _TCHAR* argv[])
{
	Register(argv[1]);
	return 0;
}

