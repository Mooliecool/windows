/****************************** Module Header ******************************\
* Module Name:  FileColumnExt.cpp
* Project:      ATLShellExtColumnHandler
* Copyright (c) Microsoft Corporation.
* 
* The Details view in the Microsoft Windows Windows Explorer normally 
* displays several standard columns. Each column lists information, such as 
* the file size or type, for each file in the current folder. By implementing
* and registering a column handler, you can make custom columns available for 
* display on Windows 2000, Windows XP and Windows 2003. 
* 
* Note: Support for column handler (IColumnProvider) has been removed from 
* Windows Vista. So the sample does not work on Windows Vista, and later 
* operating systems. The new property system should be used in its place. See 
* Property System (http://msdn.microsoft.com/en-us/library/bb776859.aspx) for 
* conceptual materials that explain the use of the new system.
* 
* FileColumnExt in ATLShellExtColumnHandler demonstrates a column handler 
* that provides three columns:
* 
* 	1. Sample C1 - display the file path when enabled
* 	2. Sample C2 - display the file size when enabled
* 	3. Title (Predefined column) - display the file name when enabled
* 
* The three columns process only .cfx files.
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
#include "FileColumnExt.h"
#pragma endregion


/////////////////////////////////////////////////////////////////////////////
// CFileColumnExt IColumnProvider methods.
// 

//
//   FUNCTION: CFileColumnExt::Initialize(LPCSHCOLUMNINIT psci)
//
//   PURPOSE: Windows Explorer calls Initialize to initialize the column 
//            handler. The wszFolder parameter is set to the folder whose 
//            Details view is about to be displayed. You can store the folder 
//            name for later use and initialize the handler object as needed.
//
IFACEMETHODIMP CFileColumnExt::Initialize(LPCSHCOLUMNINIT psci)
{
	MessageBox(NULL, L"test", L"test", NULL);
	return S_OK;
}


//
//   FUNCTION: CFileColumnExt::GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci)
//
//   PURPOSE: Windows Explorer next calls GetColumnInfo to request the 
//            column's identifier and characteristics. It passes in an index 
//            for the column in the dwIndex parameter. This index is an 
//            arbitrary value that is used to enumerate columns. Windows 
//            Explorer also passes in a pointer to an SHCOLUMNINFO structure. 
//            This structure is used to return the column's identifier and 
//            characteristics. GetColumnInfo should assign appropriate values 
//            to the members of the structure and return.
//
IFACEMETHODIMP CFileColumnExt::GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci)
{
	MessageBox(NULL, L"test", L"test", NULL);
	// The column handler provides three columns, so if dwIndex is greater 
	// than 2, return S_FALSE to indicate we have enumerated all the columns.
	if (dwIndex > 2)
		return S_FALSE;

	switch (dwIndex)
	{
	case 0:		// Column 1: Sample C1
		psci->scid.fmtid = CLSID_FileColumnExt;	// Use our CLSID as format ID
		psci->scid.pid = 0;						// Use the column ID
		psci->vt = VT_LPSTR;					// Return the data as a string
		psci->fmt = LVCFMT_LEFT;				// Text is left-aligned
		psci->csFlags = SHCOLSTATE_TYPE_STR;	// Data is sorted as strings
		psci->cChars = 32;						// Column width in chars

		// Set column title and description
		wcsncpy_s(psci->wszTitle, MAX_COLUMN_NAME_LEN, 
			L"Sample C1", MAX_COLUMN_NAME_LEN);
		wcsncpy_s(psci->wszDescription, MAX_COLUMN_DESC_LEN,
			L"Sample Column 1", MAX_COLUMN_DESC_LEN);
		break;

	case 1:		// Column 2: Sample C2
		psci->scid.fmtid = CLSID_FileColumnExt;	// Use our CLSID as format ID
		psci->scid.pid = 1;						// Use the column ID
		psci->vt = VT_I8;						// Return the data as an int64
		psci->fmt = LVCFMT_LEFT;				// Text is left-aligned
		psci->csFlags = SHCOLSTATE_TYPE_INT;	// Data is sorted as ints
		psci->cChars = 32;						// Column width in chars

		// Set column title and description
		wcsncpy_s(psci->wszTitle, MAX_COLUMN_NAME_LEN, 
			L"Sample C2", MAX_COLUMN_NAME_LEN);
		wcsncpy_s(psci->wszDescription, MAX_COLUMN_DESC_LEN, 
			L"Sample Column 2", MAX_COLUMN_DESC_LEN);
		break;

	case 2:		// Column 3: Predefined Title column
		// See: http://msdn.microsoft.com/en-us/library/aa380060(VS.85).aspx
		psci->scid.fmtid = FMTID_SummaryInformation;// Use predefined FMTID
		psci->scid.pid = PIDSI_TITLE;			// Predefined ID for Title
		psci->vt = VT_LPSTR;					// Return the data as a string
		psci->fmt = LVCFMT_LEFT;				// Text is left-aligned
		psci->csFlags = SHCOLSTATE_TYPE_STR;	// Data is sorted as strings
		psci->cChars = 32;						// Column width in chars
		break;

		DEFAULT_UNREACHABLE;
	}

	return S_OK;
}


//
//   FUNCTION: CFileColumnExt::GetItemData(LPCSHCOLUMNID pscid, 
//             LPCSHCOLUMNDATA pscd, VARIANT* pvarData)
//
//   PURPOSE: If the column handler's column has been selected, Windows 
//            Explorer calls GetItemData for each file in the folder that is 
//            to be displayed. The pscid parameter is a pointer to an 
//            SHCOLUMNID structure that identifies the column. The pscd 
//            parameter points to an SHCOLUMNDATA structure that identifies 
//            the particular file. The pvarData parameter returns the data 
//            that should be displayed in the handler's column for the file 
//            specified by pscd. If that file is supported by your column 
//            handler, assign its data value to pvarData and return S_OK. If 
//            the file is not supported by your column handler, return 
//            S_FALSE without assigning a value to pvarData.
//
IFACEMETHODIMP CFileColumnExt::GetItemData(
	LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT* pvarData)
{
	/////////////////////////////////////////////////////////////////////////
	// Precheck to avoid unnecessary processing.
	// 

	// Verify the format ID and the column numbers
	if ((pscid->fmtid == CLSID_FileColumnExt && pscid->pid > 1) ||
		(pscid->fmtid == FMTID_SummaryInformation && pscid->pid != PIDSI_TITLE))
		return FALSE;

	// If it is a directory, instead of a file, return directly.
	if (pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return S_FALSE;

	// Many folders will contain a number of files that are not supported by 
	// any particular column handler. To improve efficiency, GetItemData 
	// should first check the pwszExt member of the structure pointed to by 
	// pscd. This member holds the file name extension. If the extension 
	// indicates that the file is not a member of a file class supported by 
	// your handler, avoid unnecessary processing by immediately returning 
	// S_FALSE.
	if (0 != _wcsicmp(pscd->pwszExt, L".cfx"))
		return S_FALSE;


	/////////////////////////////////////////////////////////////////////////
	// Set item data (pvarData) of the selected column 
	// (pscid->fmtid/pscid->pid) for the current file (pscd->wszFile).
	// 

	CComVariant vData;
	if (pscid->fmtid == CLSID_FileColumnExt)
	{
		switch (pscid->pid)
		{
		case 0:		// Sample C1
			vData = pscd->wszFile;
			break;
		case 1:		// Sample C2
			{
				// Open the file and read its file size
				LARGE_INTEGER liSize;
				HANDLE hFile = CreateFileW(pscd->wszFile, GENERIC_READ, 
					FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
					return S_FALSE;
				GetFileSizeEx(hFile, &liSize);
				vData = liSize.QuadPart;
			}
			break;
			DEFAULT_UNREACHABLE;
		}
	}
	else if (pscid->fmtid == FMTID_SummaryInformation)
	{
		switch (pscid->pid)
		{
		case PIDSI_TITLE:	// Predefined Title column
			{
				// Get the file name from the file path
				WCHAR wszFileName[_MAX_FNAME];
				_wsplitpath_s(pscd->wszFile, NULL, 0, NULL, 0, wszFileName, 
					_MAX_FNAME, NULL, 0);
				vData = wszFileName;
			}
			break;
			DEFAULT_UNREACHABLE;
		}
	}
	vData.Detach(pvarData);
	
	return S_OK;
}