/****************************** Module Header ******************************\
* Module Name:  FileIconExt.cpp
* Project:      ATLShellExtIconHandler
* Copyright (c) Microsoft Corporation.
* 
* An shell icon handler is a type of Shell extension handler that allows you 
* to dynamically assign icons to the members of a file class and replace the 
* file's default icon with a custom icon on a file-by-file basis. Every time 
* a file from the class is displayed, the Shell queries the handler for the 
* appropriate icon. For instance, an icon handler can assign different icons 
* to different members of the class, or vary the icon based on the current 
* state of the file. 
* 
* FileIconExt in ATLShellExtIconHandler demonstrates a shell icon handler for 
* .cfx files. After installing the icon handler, "C#.cfx" files show Sample 
* C# icon, "VB.cfx" files show Sample VB icon, "C++.cfx" files show Sample 
* C++ icon, and other .cfx files show a Sample icon in Microsoft Windows 
* Explorer. 
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
#include "FileIconExt.h"
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CFileIconExt IPersistFile methods.
//  

//
//   FUNCTION: CFileIconExt::Load(LPCOLESTR, DWORD)
//
//   PURPOSE: Opens the specified file and initializes an object from the 
//            file contents.
//
IFACEMETHODIMP CFileIconExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
	// pszFileName contains the absolute path of the file to be opened
	wcscpy_s(m_szFileName, MAX_PATH, pszFileName);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileIconExt IExtractIcon methods.
//  
// Option1.
// GetIconLocation() can return the icon file name (szIconFile) and the 
// zero-based index of the icon if there is more than one icon in the file.
// Extract() does not have to do anything but return S_FALSE to tell the 
// Shell to extract the icon itself. When using this option, the Shell 
// normally attempts to load the icon from its cache. The Shell keeps an icon 
// cache which holds recently-used icons. 
// 

//
//   FUNCTION: CFileIconExt::GetIconLocation(UINT, LPTSTR, UINT, int*, UINT*)
//
//   PURPOSE: Gets the location and index of an icon. 
//
IFACEMETHODIMP CFileIconExt::GetIconLocation(
	UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
{
	// The icons are in this shell extension DLL, so get the full path of the 
	// module and return through the szIconFile parameter.
	GetModuleFileName(_AtlBaseModule.GetResourceInstance(), szIconFile, 
		MAX_PATH);

	// Determine which icon to use, depending on the file name.

	// Get the file name from the file path
	TCHAR szFileName[_MAX_FNAME];
	_tsplitpath_s(m_szFileName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, 
		NULL, 0);

	// Check the file name and set piIndex to the correct index of icon
	if (_tcscmp(szFileName, _T("C#")) == 0)
	{
		// C#.cfx -> the 2nd icon
		*piIndex = 1;
	}
	else if (_tcscmp(szFileName, _T("VB")) == 0)
	{
		// VB.cfx -> the 3rd icon
		*piIndex = 2;
	}
	else if (_tcscmp(szFileName, _T("C++")) == 0)
	{
		// C++.cfx -> the 4th icon
		*piIndex = 3;
	}
	else
	{
		// Other file name.cfx -> the 1st icon
		*piIndex = 0;
	}

	// Set pwFlags to 0 to get the default behavior from the Shell, i.e.
	// the Shell will attempt to load the icon from its cache.
	*pwFlags = 0;

	return S_OK;
}


//
//   FUNCTION: CFileIconExt::Extract(LPCTSTR, UINT, HICON*, HICON*, UINT)
//
//   PURPOSE: Extracts an icon image from the specified location. Returns 
//            S_OK if the function extracted the icon, or S_FALSE if the 
//            calling application should extract the icon.
//
IFACEMETHODIMP CFileIconExt::Extract(LPCTSTR pszFile, UINT nIconIndex, 
									 HICON* phiconLarge, HICON* phiconSmall, 
									 UINT nIconSize)
{
	// Because a file name is provided in GetIconLocation, the handler can 
	// return S_FALSE to have the Shell extract the icon by itself.
	return S_FALSE;
}


////////////////////////////////// [-or-] ///////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CFileIconExt IExtractIcon methods.
// 
// Option2. 
// To prevent the loading of a cached icon, set the GIL_DONTCACHE flag in the 
// pwFlags parameter of GetIconLocation(). This tells the Shell not to look 
// in the icon cache. The Shell will always call Extract(), which is then 
// responsible for loading the icons and returning handles to those icons 
// that the Shell will show.
// 

////
////   FUNCTION: CFileIconExt::GetIconLocation(UINT, LPTSTR, UINT, int*, UINT*)
////
////   PURPOSE: Gets the location and index of an icon. 
////
//IFACEMETHODIMP CFileIconExt::GetIconLocation(
//	UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
//{
//	// Tell the shell not to look at the icon file name or index, and do not 
//	// check the icon cache.
//	*pwFlags = GIL_NOTFILENAME | GIL_DONTCACHE;
//
//	return S_OK;
//}
//
//
////
////   FUNCTION: CFileIconExt::Extract(LPCTSTR, UINT, HICON*, HICON*, UINT)
////
////   PARAMETERS: 
////          - phiconLarge: [out] A pointer to an HICON value that receives 
////            the handle to the large icon. 
////          - phiconSmall: [out] A pointer to an HICON value that receives 
////            the handle to the small icon.
////          - nIconSize: [in] The desired size of the icon, in pixels. The 
////            low word contains the size of the large icon, and the high 
////            word contains the size of the small icon. The size specified 
////            can be the width or height. The width of an icon always equals 
////            its height.
//// 
////   PURPOSE: Extracts an icon image from the specified location. Returns 
////            S_OK if the function extracted the icon, or S_FALSE if the 
////            calling application should extract the icon.
////
//IFACEMETHODIMP CFileIconExt::Extract(LPCTSTR pszFile, UINT nIconIndex, 
//									 HICON* phiconLarge, HICON* phiconSmall, 
//									 UINT nIconSize)
//{
//	// Determine which icon to use, depending on the file name.
//	UINT uIconID;
//
//	// Get the file name from the file path
//	TCHAR szFileName[_MAX_FNAME];
//	_tsplitpath_s(m_szFileName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, 
//		NULL, 0);
//
//	// Check the file name and set piIndex to the correct icon id
//	if (_tcscmp(szFileName, _T("C#")) == 0)
//	{
//		// C#.cfx -> IDI_SAMPLECS
//		uIconID = IDI_SAMPLECS;
//	}
//	else if (_tcscmp(szFileName, _T("VB")) == 0)
//	{
//		// VB.cfx -> IDI_SAMPLECS
//		uIconID = IDI_SAMPLEVB;
//	}
//	else if (_tcscmp(szFileName, _T("C++")) == 0)
//	{
//		// C++.cfx -> IDI_SAMPLECPP
//		uIconID = IDI_SAMPLECPP;
//	}
//	else
//	{
//		// Other file name.cfx -> IDI_SAMPLE
//		uIconID = IDI_SAMPLE;
//	}
//
//	// Determine the icon sizes
//	WORD wSmallIconSize = HIWORD(nIconSize);
//	WORD wLargeIconSize = LOWORD(nIconSize);
//
//	// Load the icons
//	if (NULL != phiconLarge)
//	{
//		*phiconLarge = (HICON)LoadImage(
//			_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(uIconID), 
//			IMAGE_ICON, wLargeIconSize, wLargeIconSize, LR_DEFAULTCOLOR);
//	}
//	if (NULL != phiconSmall)
//	{
//		*phiconSmall = (HICON)LoadImage(
//			_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(uIconID), 
//			IMAGE_ICON, wSmallIconSize, wSmallIconSize, LR_DEFAULTCOLOR);
//	}
//
//	return S_OK;
//}