/****************************** Module Header ******************************\
* Module Name:  FileIconOverlayExt.cpp
* Project:      ATLShellExtIconOverlayHandler
* Copyright (c) Microsoft Corporation.
* 
* Icon overlays are small images placed at the lower-left corner of the icon 
* that represents a Shell object. They are normally added to an object's icon 
* to provide some extra information. For instance, a commonly used icon 
* overlay is the small arrow that indicates that the icon represents a link, 
* rather than the actual file or folder. In addition to the standard icon 
* overlays that are provided by the system, you can request custom icon 
* overlays for specified Shell objects by implementing and registering an 
* icon overlay handler.
* 
* FileIconOverlayExt in ATLShellExtIconOverlayHandler demonstrates a shell 
* icon overlay handler for files with "Sample" in the file paths. After 
* installing the icon overlay handler, any file with "Sample" in its file 
* path, e.g. "D:\Sample.txt", shows an icon overlay in Windows Explorer. 
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
#include "FileIconOverlayExt.h"
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CFileIconOverlayExt IShellIconOverlayIdentifier methods.
//  

//
//   FUNCTION: CFileIconOverlayExt::GetOverlayInfo(LPWSTR, int, int*, DWORD*)
//
//   PURPOSE: Provides the location of the icon overlay's bitmap. This method 
//            is first called during initialization, it returns the fully 
//            qualified path of the file containing the icon overlay image, 
//            and its zero-based index within the file. The icon can be 
//            contained in any of the standard file types, including .exe, 
//            .dll, and .ico. After initialization is complete, the Shell 
//            calls GetOverlayInfo when it needs to display the handler's 
//            icon overlay. The method should return the same file name and 
//            index that it did during initialization. Although the Shell 
//            uses the image that is cached in the system image list rather 
//            than loading the image from the file, an icon overlay is still 
//            identified by its file name and index.
//
IFACEMETHODIMP CFileIconOverlayExt::GetOverlayInfo(
	LPWSTR pwszIconFile, int cchMax, int* pIndex, DWORD* pdwFlags)
{
	// Get the module's full path
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, 
		cchMax);

	// Use the first icon in the resource
	*pIndex = 0;

	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

	return S_OK;
}


//
//   FUNCTION: CFileIconOverlayExt::GetPriority(int*)
//
//   PURPOSE: Specifies the priority of an icon overlay. This method is 
//            called only during initialization. It assigns a priority value 
//            to the handler's icon overlay. The value can range from zero to 
//            100, where 100 is the lowest priority. The purpose of this 
//            priority value is to help the Shell resolve the conflict that 
//            arises when multiple icon overlays are specified for a single 
//            object. The Shell first uses an internal set of rules to 
//            determine the highest priority icon overlay. If these rules do 
//            not resolve the conflict, the values assigned to the icon 
//            overlays by GetPriority determine priority. The priority value 
//            set by GetPriority is not a reliable way to resolve conflicts 
//            between unrelated icon overlay handlers. There is no way for 
//            your handler to determine what priority values other handlers 
//            are using. Normally, you should set the value to zero. However, 
//            the priority value is useful when you have implemented two or 
//            more icon overlay handlers that can request icon overlay icons 
//            for the same object. By setting the priority values 
//            appropriately, you can specify which of the requested icon 
//            overlays will be displayed.
//
IFACEMETHODIMP CFileIconOverlayExt::GetPriority(int* pPriority)
{
	// Request the highest priority 
	*pPriority = 0;

	return S_OK;
}


//
//   FUNCTION: CFileIconOverlayExt::IsMemberOf(LPCWSTR, DWORD)
//
//   PURPOSE: Specifies whether an icon overlay should be added to a Shell 
//            object's icon or not. The Shell calls this method to determine 
//            whether it should display a handler's icon overlay for a 
//            particular object. It specifies the object by passing its name 
//            to the method. If a handler wants to have its icon overlay 
//            displayed, IsMemberOf returns S_OK. If not, it returns S_FALSE.
//            Icon overlay handlers are normally intended to work with a 
//            particular group of files. A typical example is a file class, 
//            identified by a specific file name extension. An icon overlay 
//            handler can request an icon overlay for all members of the file 
//            class. Some handlers request an icon overlay only if a member 
//            of the file class is in a particular state. However, icon 
//            overlay handlers are free to request their icon overlay for any 
//            object that they want.
//
IFACEMETHODIMP CFileIconOverlayExt::IsMemberOf(LPCWSTR pwszPath, 
											   DWORD dwAttrib)
{
	// If the file name contains "Sample", add the overlay
	if (wcsstr(pwszPath, L"Sample") != 0)
		return S_OK;
	else
		return S_FALSE;
}