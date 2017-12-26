/****************************** Module Header ******************************\
* Module Name:  FileDragAndDropExt.cpp
* Project:      ATLShellExtDragAndDropHandler
* Copyright (c) Microsoft Corporation.
* 
* FileDragAndDropExt is an example of drag-and-drop handler for file objects. 
* After the setup of the handler, when you right-click any files to drag the 
* files to a directory or the desktop, a context menu with "All-In-One Code 
* Framework" menu item will be displayed. Clicking the menu item prompts a 
* message box that shows the files being dragged and the target location that 
* the files are dropped to.
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
#include "FileDragAndDropExt.h"
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CFileDragAndDropExt IShellExtInit methods.
// 

//
//   FUNCTION: CFileDragAndDropExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, 
//             HKEY)
//
//   PURPOSE: Initializes a property sheet extension, shortcut menu extension, 
//            or drag-and-drop handler.
//
IFACEMETHODIMP CFileDragAndDropExt::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	// Get the name of the directory where the files were dropped
	if (!SHGetPathFromIDList(pidlFolder, m_szFolderDroppedIn))
		return E_FAIL;

	// Get a list of all the objects that were dropped

	if (NULL == pDataObj)
    {
		return E_INVALIDARG;
    }

	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm;

	// pDataObj contains the objects being acted upon. In this example, 
	// we get an HDROP handle for enumerating the selected files.
	if (FAILED(pDataObj->GetData(&fe, &stm)))
		return E_INVALIDARG;

	// Get an HDROP handle.
	HDROP hDrop = (HDROP)GlobalLock(stm.hGlobal);
	if (NULL == hDrop)
    {
		ReleaseStgMedium(&stm);
		return E_INVALIDARG;
	}

	// Determine how many files are involved in this operation
	UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	if (nFiles == 0)
	{
		GlobalUnlock(stm.hGlobal);
		ReleaseStgMedium(&stm);
		return E_INVALIDARG;
	}

	// Enumerates through the selected files and directories
	TCHAR szFileName[MAX_PATH];
	for (UINT i = 0; i < nFiles; i++)
	{
		// Get the next filename
		if (0 == DragQueryFile(hDrop, i, szFileName, MAX_PATH))
			continue;

		m_lsFiles.push_back(szFileName);
	}

	// Release resources
	GlobalUnlock(stm.hGlobal);
    ReleaseStgMedium(&stm);

	// If we found any files we can work with, return S_OK. Otherwise return 
	// E_INVALIDARG so we don't get called again for this drag-and-drop 
	// operation.
	return (m_lsFiles.size() > 0) ? S_OK : E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CFileDragAndDropExt IContextMenu methods.
// 

#define IDM_SAMPLE			0			// The command's identifier offset. 


//
//   FUNCTION: CFileContextMenuExt::OnSample(HWND)
//
//   PURPOSE: OnSample handles the "Sample" verb of the shell extension.
//
void CFileDragAndDropExt::OnSample(HWND hWnd)
{
	// Concat the dragged files to a string
	DWORD dwBufferSize = m_lsFiles.size() * MAX_PATH;
	TCHAR* pszFiles = new TCHAR[dwBufferSize];
	ZeroMemory(pszFiles, dwBufferSize);

	string_list::const_iterator end;
	for (string_list::const_iterator it = m_lsFiles.begin(), 
		end = m_lsFiles.end();
		it != end; it++)
	{
		_tcscat_s(pszFiles, dwBufferSize, it->c_str());
		_tcscat_s(pszFiles, dwBufferSize, _T("\n"));
	}

	TCHAR* pszMessage = new TCHAR[512 + dwBufferSize];
	_stprintf_s(pszMessage, 512 + dwBufferSize, _T("Drag-and-drop handler") \
		_T("\n\nSource file(s):\n%s\nTarge location:\n%s"), pszFiles, 
		m_szFolderDroppedIn);

	MessageBox(hWnd, pszMessage, _T("ATLShellExtDragAndDropHandler"), 
		MB_ICONINFORMATION);

	delete[] pszFiles;
	delete[] pszMessage;
}


//
//   FUNCTION: CFileDragAndDropExt::QueryContextMenu(HMENU, UINT, UINT, UINT, 
//             UINT)
//
//   PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
//            context menu handler to add its menu items to the menu. It 
//            passes in the HMENU handle in the hmenu parameter. The 
//            indexMenu parameter is set to the index to be used for the 
//            first menu item that is to be added.
//
IFACEMETHODIMP CFileDragAndDropExt::QueryContextMenu(
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

	// Return an HRESULT value with the severity set to SEVERITY_SUCCESS. 
	// Set the code value to the offset of the largest command identifier 
	// that was assigned, plus one (1)
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_SAMPLE + 1));
}


//
//   FUNCTION: CFileDragAndDropExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//   PURPOSE: This method is called when a user clicks a menu item to tell 
//            the handler to run the associated command. The lpcmi parameter 
//            points to a structure that contains the needed information.
//
IFACEMETHODIMP CFileDragAndDropExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	// The high-word of lpcmi->lpVerb must be NULL because we did not 
	// implement IContextMenu::GetCommandString to specify any verb for the 
	// command
	if (NULL != HIWORD(lpcmi->lpVerb))
	{
		return E_INVALIDARG;
	}

	// Then, the low-word of lpcmi->lpVerb should contain the command's 
	// identifier offset
	if (LOWORD(lpcmi->lpVerb) == IDM_SAMPLE)
	{
		OnSample(lpcmi->hwnd);
	}
	else
	{
		// If the verb is not recognized by the drag-and-drop handler, it 
		// must return E_FAIL to allow it to be passed on to the other 
		// drag-and-drop handlers that might implement that verb.
		return E_FAIL;
	}

	return S_OK;
}