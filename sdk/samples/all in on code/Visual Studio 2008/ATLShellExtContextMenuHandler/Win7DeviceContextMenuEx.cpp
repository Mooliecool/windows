/****************************** Module Header ******************************\
* Module Name:  Win7DeviceContextMenuEx.cpp
* Project:      ATLShellExtContextMenuHandler
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
#include "stdafx.h"
#include "Win7DeviceContextMenuEx.h"
#include <strsafe.h>
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CWin7DeviceContextMenuEx IShellExtInit methods.
// 

//
//   FUNCTION: CWin7DeviceContextMenuEx::Initialize(LPCITEMIDLIST, 
//             LPDATAOBJECT, HKEY)
//
//   PURPOSE: Initializes a property sheet extension, shortcut menu extension, 
//            or drag-and-drop handler.
//
IFACEMETHODIMP CWin7DeviceContextMenuEx::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	// If any value other than S_OK is returned from 
	// IShellExtInit::Initialize, the property sheet is not displayed.
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWin7DeviceContextMenuEx IContextMenu methods.
// 

#define IDM_SAMPLE			0			// The command's identifier offset. 
#define VERB_SAMPLEA		"Sample"	// The command's ANSI verb string
#define VERB_SAMPLEW		L"Sample"	// The command's Unicode verb string


//
//   FUNCTION: CWin7DeviceContextMenuEx::OnSample(HWND)
//
//   PURPOSE: OnSample handles the "Sample" verb of the shell extension.
//
void CWin7DeviceContextMenuEx::OnSample(HWND hWnd)
{
	//TCHAR szMessage[300];
	//_stprintf_s(szMessage, 300, _T("The selected file is:\n\n%s"), 
	//	m_szFileName); 
	//
	MessageBox(hWnd, _T("temp"), _T("ATLShellExtContextMenuHandler"), 
		MB_ICONINFORMATION);
}


//
//   FUNCTION: CWin7DeviceContextMenuEx::QueryContextMenu(HMENU, UINT, UINT, 
//             UINT, UINT)
//
//   PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
//            context menu handler to add its menu items to the menu. It 
//            passes in the HMENU handle in the hmenu parameter. The 
//            indexMenu parameter is set to the index to be used for the 
//            first menu item that is to be added.
//
IFACEMETHODIMP CWin7DeviceContextMenuEx::QueryContextMenu(
	HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	// If uFlags include CMF_DEFAULTONLY then we should not do anything
	if (CMF_DEFAULTONLY & uFlags)
	{
		return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
	}

	// Use either InsertMenu or InsertMenuItem to add menu items to the list
	InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + 
		IDM_SAMPLE, _T("&All-In-One Code Framework"));

	// Learn how to add sub-menu from 
	// http://www.codeproject.com/KB/shell/ctxextsubmenu.aspx

	// Return an HRESULT value with the severity set to SEVERITY_SUCCESS. 
	// Set the code value to the offset of the largest command identifier 
	// that was assigned, plus one (1)
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_SAMPLE + 1));
}


//
//   FUNCTION: CWin7DeviceContextMenuEx::GetCommandString(UINT, UINT, LPUINT, 
//             LPSTR, UINT)
//
//   PURPOSE: If a user highlights one of the items added by a context menu 
//            handler, the handler's IContextMenu::GetCommandString method is 
//            called to request a Help text string that will be displayed on 
//            the Windows Explorer status bar. This method can also be called 
//            to request the verb string that is assigned to a command. 
//            Either ANSI or Unicode verb strings can be requested.
//
IFACEMETHODIMP CWin7DeviceContextMenuEx::GetCommandString(
	UINT idCommand, UINT uFlags, LPUINT lpReserved, LPSTR pszName, 
	UINT uMaxNameLen)
{
	HRESULT  hr = E_INVALIDARG;

	// For the command "&All-In-One Code Framework" (IDM_SAMPLE)
	if (idCommand == IDM_SAMPLE)
	{
		switch (uFlags)
		{
		case GCS_HELPTEXTA:
			hr = StringCchCopyNA(pszName, 
				lstrlenA(pszName) / sizeof(pszName[0]), 
				"All-In-One Code Framework Shell Extension Example", 
				uMaxNameLen);
			break;

		case GCS_HELPTEXTW:
			hr = StringCchCopyNW((LPWSTR)pszName, 
				lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
				L"All-In-One Code Framework Shell Extension Example", 
				uMaxNameLen);
			break;

		case GCS_VERBA:
			hr = StringCchCopyNA(pszName, 
				lstrlenA(pszName) / sizeof(pszName[0]), 
				VERB_SAMPLEA, uMaxNameLen);
			break;

		case GCS_VERBW:
			hr = StringCchCopyNW((LPWSTR)pszName, 
				lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
				VERB_SAMPLEW, uMaxNameLen);
			break;

		default:
			hr = S_OK;
		}
	}

	// If the command (idCommand) is not supported by this context menu 
	// extension handler, return E_INVALIDARG.

	return hr;
}


//
//   FUNCTION: CWin7DeviceContextMenuEx::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//   PURPOSE: This method is called when a user clicks a menu item to tell 
//            the handler to run the associated command. The lpcmi parameter 
//            points to a structure that contains the needed information.
//
IFACEMETHODIMP CWin7DeviceContextMenuEx::InvokeCommand(
	LPCMINVOKECOMMANDINFO lpcmi)
{
	BOOL fEx = FALSE;
	BOOL fUnicode = FALSE;

	// Determines which structure is being passed in, CMINVOKECOMMANDINFO or 
	// CMINVOKECOMMANDINFOEX based on the cbSize member of lpcmi. Although 
	// the lpcmi parameter is declared in Shlobj.h as a CMINVOKECOMMANDINFO 
	// structure, in practice it often points to a CMINVOKECOMMANDINFOEX 
	// structure. This struct is an extended version of CMINVOKECOMMANDINFO 
	// and has additional members that allow Unicode strings to be passed.
	if(lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
	{
		fEx = TRUE;
		if((lpcmi->fMask & CMIC_MASK_UNICODE))
		{
			fUnicode = TRUE;
		}
	}

	// Determines whether the command is identified by its offset or verb.
	// There are two ways to identify commands:
	//   1) The command's verb string 
	//   2) The command's identifier offset
	// If the high-order word of lpcmi->lpVerb (for the ANSI case) or 
	// lpcmi->lpVerbW (for the Unicode case) is nonzero, lpVerb or lpVerbW 
	// holds a verb string. If the high-order word is zero, the command 
	// offset is in the low-order word of lpcmi->lpVerb.

	// For the ANSI case, if the high-order word is not zero, the command's 
	// verb string is in lpcmi->lpVerb. 
    if (!fUnicode && HIWORD(lpcmi->lpVerb))
    {
		// Is the verb supported by this context menu extension?
		if (StrCmpIA(lpcmi->lpVerb, VERB_SAMPLEA) == 0)
		{
			OnSample(lpcmi->hwnd);
		}
		else
		{
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
	}

	// For the Unicode case, if the high-order word is not zero, the 
	// command's verb string is in lpcmi->lpVerbW. 
	else if(fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)lpcmi)->lpVerbW))
	{
		// Is the verb supported by this context menu extension?
		if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)lpcmi)->lpVerbW, 
			VERB_SAMPLEW) == 0)
		{
			OnSample(lpcmi->hwnd);
		}
		else
		{
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
	}

	// If the command cannot be identified through the verb string, then 
	// check the identifier offset.
    else
    {
		// Is the command identifier offset supported by this context menu 
		// extension?
        if (LOWORD(lpcmi->lpVerb) == IDM_SAMPLE)
		{
			OnSample(lpcmi->hwnd);
		}
		else
		{
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
    }

    return S_OK;
}