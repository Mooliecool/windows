/****************************** Module Header ******************************\
* Module Name:  CppStaticallyLinkLib.cpp
* Project:      CppStaticallyLinkLib
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates statically linking CppStaticLibrary.lib and using 
* its functionalities.
* 
* There are several advantages to statically linking libraries with an 
* executable instead of dynamically linking them. The most significant is  
* that the application can be certain that all its libraries are present and 
* that they are the correct version. This avoids dependency problems. In some 
* cases, static linking can result in a performance improvement. Static 
* linking can also allow the application to be contained in a single 
* executable file, simplifying distribution and installation. On the other 
* hand, statically linking libraries with the executable increases its size. 
* This is because the library code is stored within the executable rather 
* than in separate files. 
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

// The header files of the LIB to be linked
#include "CppSimpleClass.h"
#include "CppLibFuncs.h"
#pragma endregion


int _tmain(int argc, _TCHAR* argv[])
{
    PCWSTR pszString = L"HelloWorld";
    int nLength;

	// Call a cdecl method in CppLibFuncs.h
	// int GetStringLength1(PWSTR pszString);
    nLength = GetStringLength1(pszString);
    wprintf(L"Function: GetStringLength1(\"%s\") => %d\n", pszString, nLength);

	// Call a stdcall method in CppLibFuncs.h
	// int GetStringLength2(PWSTR pszString);
    nLength = GetStringLength2(pszString);
    wprintf(L"Function: GetStringLength2(\"%s\") => %d\n", pszString, nLength);
	
	// Use a class in CppSimpleClass.h
	// CppSimpleClass
	{
		CppSimpleClass simpleObj;
		simpleObj.set_FloatProperty(1.2F);
		float result = simpleObj.get_FloatProperty();
		_tprintf(_T("Get FloatProperty = %.2f\n"), result);
	}

	return 0;
}

