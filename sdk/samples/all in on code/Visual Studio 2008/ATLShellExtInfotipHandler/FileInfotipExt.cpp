/****************************** Module Header ******************************\
* Module Name:  FileInfotipExt.cpp
* Project:      ATLShellExtInfotipHandler
* Copyright (c) Microsoft Corporation.
* 
* The implementation of CFileInfotipExt for the FileInfotipExt infotip 
* handler. When mouse is placed over a .txt file item in Windows Explorer, 
* shell queries for the registry entry
* 
* 	HKEY_CLASSES_ROOT\.txt\ShellEx\{00021500-0000-0000-C000-000000000046}
* 
* It then checks the default value at this key. If it is a valid CLSID, shell 
* creates an object of that class. Once the object is created, shell queries 
* for IPersistFile and calls its Load method passing the file name of the 
* item over which mouse is placed. After that, IQueryInfo is queried for and 
* the method GetInfoTip is called. GetInfoTip has an out parameter *ppwszTip 
* which recieves the address of the tool tip buffer. Shell finally displays 
* the info tip using the value in the tool tip buffer.
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
#include "stdafx.h"
#include "FileInfotipExt.h"
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CFileInfotipExt IPersistFile methods.
// 

//
//   FUNCTION: CFileInfotipExt::Load(LPCOLESTR, DWORD)
//
//   PURPOSE: Opens the specified file and initializes an object from the 
//            file contents.
//
IFACEMETHODIMP CFileInfotipExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
	// pszFileName contains the absolute path of the file to be opened
	wcscpy_s(m_szFileName, MAX_PATH, pszFileName);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileInfotipExt IQueryInfo methods.
// 

//
//   FUNCTION: CFileInfotipExt::GetInfoTip(DWORD, LPWSTR)
//
//   PURPOSE: Gets the info tip text for an item.
//
IFACEMETHODIMP CFileInfotipExt::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip)
{
	// ppwszTip is the address of a Unicode string pointer that receives the 
	// tip string pointer. Extensions that implement this method must 
	// allocate memory for ppwszTip by calling CoTaskMemAlloc. Shell knows to 
	// free the memory when the info tip is no longer needed.
	*ppwszTip = (LPWSTR)CoTaskMemAlloc(512 * sizeof(WCHAR));
	swprintf_s(*ppwszTip, 512, L"All-In-One Code Framework Shell Extension " \
		L"Example\nATLShellExtInfotipHandler - FileInfotipExt\nFile name: %s", 
		m_szFileName);

	return S_OK;
}