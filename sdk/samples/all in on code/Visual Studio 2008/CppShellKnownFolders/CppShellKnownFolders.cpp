/****************************** Module Header ******************************\
* Module Name:  CppShellKnownFolders.cpp
* Project:      CppShellKnownFolders
* Copyright (c) Microsoft Corporation.
* 
* The Known Folder system provides a way to interact with certain 
* high-profile folders that are present by default in Microsoft Windows. It 
* also allows those same interactions with folders installed and registered 
* with the Known Folder system by applications. This sample demonstrates 
* those possible interactions as they are provided by the Known Folder APIs.
* 
* A. Enumerate and print all known folders. (PrintAllKnownFolders)
* 
* B. Print some built-in known folders like FOLDERID_ProgramFiles in three 
* different ways. (PrintSomeDefaultKnownFolders)
* 
* C. Extend known folders with custom folders.
* 
*   1 Register and create a known folder named "Sample KnownFolder" under 
*   the user profile folder: C:\Users\<username>\SampleKnownFolder. The 
*   known folder displays the localized name "Sample KnownFolder 
*   LocalizedName", and shows a special folder icon. (CreateKnownFolder, 
*   RegisterKnownFolder)
*   
*   2 Print the known folder. (PrintKnownFolder)
*   
*   3 Remove and unregister the known folder. 
*   (RemoveKnownFolder, UnregisterKnownFolder)
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
#include "resource.h"
#include <shlobj.h>
#include <strsafe.h>
#pragma endregion


#define GUID_SIZE               128


/*!
* Enumerate and print all known folders.
*/
void PrintAllKnownFolders()
{
	HRESULT hr;
	PWSTR pszPath = NULL;

	// Create an IKnownFolderManager instance
	IKnownFolderManager* pkfm = NULL;
    hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
	if (SUCCEEDED(hr))
	{
		KNOWNFOLDERID *rgKFIDs = NULL;
		UINT cKFIDs = 0;
		// Get the IDs of all known folders
		hr = pkfm->GetFolderIds(&rgKFIDs, &cKFIDs);
		if (SUCCEEDED(hr))
		{
			IKnownFolder *pkfCurrent = NULL;
			// Enumerate the known folders. rgKFIDs[i] has the KNOWNFOLDERID
			for (UINT i = 0; i < cKFIDs; ++i)
			{
				hr = pkfm->GetFolder(rgKFIDs[i], &pkfCurrent);
				if (SUCCEEDED(hr))
				{
					// Get the non-localized, canonical name for the known 
					// folder from KNOWNFOLDER_DEFINITION
					KNOWNFOLDER_DEFINITION kfd;
					hr = pkfCurrent->GetFolderDefinition(&kfd);
					if (SUCCEEDED(hr))
					{
						// Next, get the path of the known folder
						hr = pkfCurrent->GetPath(0, &pszPath);
						if (SUCCEEDED(hr))
						{
							wprintf(L"%s: %s\n", kfd.pszName, pszPath);
							CoTaskMemFree(pszPath);
						}
						FreeKnownFolderDefinitionFields(&kfd);
					}
					pkfCurrent->Release();
				}
			}
			CoTaskMemFree(rgKFIDs);
		}
		pkfm->Release();
	}
}


/*!
* Print some default known folders in Windows.
*/
void PrintSomeDefaultKnownFolders()
{
	HRESULT hr;
	PWSTR pszPath = NULL;

	///////////////////////////////////////////////////////////////////////// 
	// Print the "ProgramFiles" known folder in three ways.
	// 

	// Method 1: SHGetKnownFolderPath (The function is new in Windows Vista)
	hr = SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, NULL, 
		&pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_ProgramFiles: %s\n", pszPath);

		// The calling application is responsible for calling CoTaskMemFree 
		// to free this resource after use.
		CoTaskMemFree(pszPath);
	}

	// Method 2: IKnownFolderManager::GetGetFolder, IKnownFolder::GetPath 
	// (The functions are new in Windows Vista)
	IKnownFolderManager* pkfm = NULL;
    hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
	if (SUCCEEDED(hr))
	{
		IKnownFolder* pkf = NULL;
		hr = pkfm->GetFolder(FOLDERID_ProgramFiles, &pkf);
		if (SUCCEEDED(hr))
		{
			hr = pkf->GetPath(0, &pszPath);
			if (SUCCEEDED(hr))
			{
				wprintf(L"FOLDERID_ProgramFiles: %s\n", pszPath);

				// The calling application is responsible for calling 
				// CoTaskMemFree to free this resource after use.
				CoTaskMemFree(pszPath);
			}
			pkf->Release();
		}
		pkfm->Release();
	}

	// Method 3: SHGetFolderPath (The function is deprecated. As of Windows 
	// Vista, this function is merely a wrapper for SHGetKnownFolderPath.)
	TCHAR szFolderPath[MAX_PATH];
	hr = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, 
		szFolderPath);
	if (SUCCEEDED(hr))
	{
		_tprintf(_T("FOLDERID_ProgramFiles: %s\n"), szFolderPath);
	}


	///////////////////////////////////////////////////////////////////////// 
	// Print known folders for per-computer program data.
	// 

	// The user would never want to browse here in Explorer, and settings 
	// changed here should affect every user on the machine. The default 
	// location is %systemdrive%\ProgramData, which is a hidden folder, on an 
	// installation of Windows Vista. You'll want to create your directory 
	// and set the ACLs you need at install time.
	hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_ProgramData: %s\n", pszPath);
		CoTaskMemFree(pszPath);
	}

	// The user would want to browse here in Explorer and double click to 
	// open the file. The default location is %public%, which has explicit 
	// links throughout Explorer, on an installation of Windows Vista. You'll 
	// want to create your directory and set the ACLs you need at install 
	// time.
	hr = SHGetKnownFolderPath(FOLDERID_Public, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_Public: %s\n", pszPath);
		CoTaskMemFree(pszPath);
	}


	///////////////////////////////////////////////////////////////////////// 
	// Print known folders for per-user program data.
	// 

	// The user would never want to browse here in Explorer, and settings 
	// changed here should roam with the user. The default location is 
	// %appdata%, which is a hidden folder, on an installation of Windows 
	// Vista.
	hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_RoamingAppData: %s\n", pszPath);
		CoTaskMemFree(pszPath);
	}

	// The user would never want to browse here in Explorer, and settings 
	// changed here should stay local to the computer. The default location 
	// is %localappdata%, which is a hidden folder, on an installation of 
	// Windows Vista.
	hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_LocalAppData: %s\n", pszPath);
		CoTaskMemFree(pszPath);
	}

	// The user would want to browse here in Explorer and double click to 
	// open the file. The default location is %userprofile%\documents, which 
	// has explicit links throughout Explorer, on an installation of Windows 
	// Vista.
	hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_Documents: %s\n", pszPath);
		CoTaskMemFree(pszPath);
	}
}


#pragma region Extending Known Folders with Custom Folders

HRESULT CreateKnownFolder(REFKNOWNFOLDERID kfid);
HRESULT RegisterKnownFolder(REFKNOWNFOLDERID kfid);

HRESULT RemoveKnownFolder(REFKNOWNFOLDERID kfid);
HRESULT UnregisterKnownFolder(REFKNOWNFOLDERID kfid);

void PrintKnownFolder(REFKNOWNFOLDERID kfid);


/*!
* Register and create a known folder named "Sample KnownFolder" under the 
* user profile folder: C:\Users\<username>\SampleKnownFolder. The known 
* folder displays the localized name "Sample KnownFolder LocalizedName", and 
* shows a special folder icon. 
* 
* CreateKnownFolder calls RegisterKnownFolder to register a known folder. In 
* RegisterKnownFolder, first define the known folder through a 
* KNOWNFOLDER_DEFINITION structure. You can specify the known folder's 
* canonical name, localized name, tooltip, folder icon, etc. Then register 
* the known folder through a call to IKnownFolderManager::RegisterFolder. 
* IKnownFolderManager::RegisterFolder requires that the current process is 
* run as administrator to succeed. 
* 
* After the known folder is register, CreateKnownFolder initializes and 
* creates the folder with SHGetKnownFolderPath with the flags KF_FLAG_CREATE 
* | KF_FLAG_INIT so that the Shell will write desktop.ini in the folder. This 
* is how our customizations (i.e. pszIcon, pszTooltip, pszLocalizedName) get 
* picked up by the Shell. If SHGetKnownFolderPath fails, the function 
* UnregisterKnownFolder is invoked to undo the registration.
*/
HRESULT CreateKnownFolder(REFKNOWNFOLDERID kfid)
{
	// Register the known folder
	HRESULT hr = RegisterKnownFolder(kfid);
	if (SUCCEEDED(hr))
	{
		// Create the known folder with SHGetKnownFolderPath with the flags 
		// KF_FLAG_CREATE | KF_FLAG_INIT so that the Shell will write 
		// desktop.ini in the folder. This is how our customizations (i.e. 
		// pszIcon, pszTooltip, pszLocalizedName) get picked up by the Shell.

		PWSTR pszPath = NULL;
		hr = SHGetKnownFolderPath(kfid, KF_FLAG_CREATE | KF_FLAG_INIT, NULL, 
			&pszPath);
		if (FAILED(hr))
		{
			// Failed to initialize and create the known folder
			_tprintf(_T("SHGetKnownFolderPath failed w/err 0x%08lx\n"), hr);

			// Unregister the known folder because of the failure
			UnregisterKnownFolder(kfid);
		}
		else
		{
			wprintf(L"The known folder is registered and created:\n%s\n", 
				pszPath);

			// Must free the pszPath output of SHGetKnownFolderPath
			CoTaskMemFree(pszPath);
		}
	}
	return hr;
}


/*!
* Register a known folder. The function requires administrative privilege, 
* so please make sure that the routine is run as administrator.
*/
HRESULT RegisterKnownFolder(REFKNOWNFOLDERID kfid)
{
	HRESULT hr;

	/////////////////////////////////////////////////////////////////////////
	// Define your known folder through a KNOWNFOLDER_DEFINITION structure.
	// 

	KNOWNFOLDER_DEFINITION kfd = {};
	kfd.category = KF_CATEGORY_PERUSER;
	kfd.pszName = L"Sample KnownFolder";	// Known folder canonical name
	kfd.pszDescription= L"This is a sample known folder";

	// fidParent and pszRelativePath work together. pszRelativePath specifies
	// a path relative to the parent folder specified in fidParent.
	kfd.fidParent = FOLDERID_Profile;
	kfd.pszRelativePath = L"SampleKnownFolder";

	// pszParsingName points to Shell namespace folder path of the folder, 
	// stored as a null-terminated Unicode string. Applies to virtual folders 
	// only. For example, ::%CLSID_MyComputer%\::%CLSID_ControlPanel% is the 
	// parsing name of Control Panel.
	GUID guid;
	CoCreateGuid(&guid);
	kfd.pszParsingName = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * GUID_SIZE);
	if (kfd.pszParsingName)
	{
		StringFromGUID2(guid, kfd.pszParsingName, GUID_SIZE);
	}

	// Get the current exe module path for the pszTooltip, pszLocalizedName, 
	// and pszIcon fields.
	WCHAR szExePath[MAX_PATH] = {};
	GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath));
	size_t cch = ARRAYSIZE(szExePath) + 10;	// +10 as a flexible buffer

	// pszTooltip points to the default tool tip resource used for this known 
	// folder when it is created. This is a null-terminated Unicode string in 
	// this form: @Module name, Resource ID
	// Here we use the current exe module to store the string resource.
	kfd.pszTooltip = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * cch);
	if (kfd.pszTooltip)
	{
		ZeroMemory(kfd.pszTooltip, sizeof(WCHAR) * cch);
		StringCchPrintfW(kfd.pszTooltip, cch, L"@%s,-%d", szExePath, 
			IDS_SAMPLEKF_TOOLTIP);
	}

	// pszLocalizedName points to the default localized name resource used 
	// when the folder is created. This is a null-terminated Unicode string 
	// in this form: @Module name, Resource ID
	// Here we use the current exe module to store the string resource.
	kfd.pszLocalizedName = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * cch);
	if (kfd.pszLocalizedName)
	{
		ZeroMemory(kfd.pszLocalizedName, sizeof(WCHAR) * cch);
		StringCchPrintfW(kfd.pszLocalizedName, cch, L"@%s,-%d", szExePath, 
			IDS_SAMPLEKF_LOCALIZEDNAME);
	}

	// pszIcon points to the default icon resource used when the folder is 
	// created. This is a null-terminated Unicode string in this form: 
	// Module name, Resource ID
	// Here we use the current exe module to store the icon resource.
	kfd.pszIcon = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * cch);
	if (kfd.pszIcon)
	{
		ZeroMemory(kfd.pszIcon, sizeof(WCHAR) * cch);
		StringCchPrintfW(kfd.pszIcon, cch, L"%s,-%d", szExePath, 
			IDI_SAMPLEKF_ICON);
	}


	/////////////////////////////////////////////////////////////////////////
	// Register the known folder through a call to RegisterFolder.
	// 

	// Create IKnownFolderManager instance
	IKnownFolderManager* pkfm = NULL;
	hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
	if (SUCCEEDED(hr))
	{
		hr = pkfm->RegisterFolder(kfid, &kfd);
		if (FAILED(hr))
		{
			_tprintf(_T("IKnownFolderManager::RegisterFolder failed w/err ") \
				_T("0x%08lx\nPlease run as admin to register a known folder\n"), 
				hr);
		}
		pkfm->Release();
	}

	return hr;
}


/*!
* Remove and unregister a known folder.
* 
* RemoveKnownFolder is responsible for remove and unregister the specified 
* known folder. It first gets the physical folder path of the known folder, 
* and attempts to delete it. When the deletion succeeds, the function calls 
* UnregisterKnownFolder to unregister the known folder from registry. 
* UnregisterKnownFolder requires administrator privilege, so please make sure 
* that the routine is run as administrator.
*/
HRESULT RemoveKnownFolder(REFKNOWNFOLDERID kfid)
{
	// Get the physical folder of the known folder.
	PWSTR pszPath = NULL;
	HRESULT hr = SHGetKnownFolderPath(kfid, 0, NULL, &pszPath);
	if (FAILED(hr))
	{
		// Failed to get the physical folder of the known folder.
		_tprintf(_T("SHGetKnownFolderPath failed w/err 0x%08lx\n"), hr);
	}
	else
	{
		// Attempt to remove the physical folder of the known folder.
		SHFILEOPSTRUCT fos = {};
		fos.wFunc = FO_DELETE;
		fos.pFrom = pszPath;
		fos.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		int err = SHFileOperation(&fos);
		if (0 != err)
		{
			// Failed to remove the physical folder
			_tprintf(_T("SHFileOperation failed w/err 0x%08lx\n"), err);
			hr = E_FAIL;
		}
		else
		{
			// If the physical folder was deleted successfully, attempt to 
			// unregister the known folder.
			hr = UnregisterKnownFolder(kfid);
			if (SUCCEEDED(hr))
			{
				wprintf(L"The known folder is unregistered and removed:\n%s\n", 
					pszPath);
			}
		}

		// Must free the pszPath output of SHGetKnownFolderPath
		CoTaskMemFree(pszPath);
	}
	return hr;
}


/*!
* Unregister a known folder. The function requires administrator privilege, 
* so please make sure that the routine is run as administrator.
*/
HRESULT UnregisterKnownFolder(REFKNOWNFOLDERID kfid)
{
	IKnownFolderManager* pkfm = NULL;
	HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
	if (SUCCEEDED(hr))
	{
		hr = pkfm->UnregisterFolder(kfid);
		if (FAILED(hr))
		{
			_tprintf(_T("IKnownFolderManager::UnregisterFolder failed w/err") \
				_T(" 0x%08lx\n"), hr);
		}
		pkfm->Release();
	}
	return hr;
}


/*!
* Print a known folder.
*/
void PrintKnownFolder(REFKNOWNFOLDERID kfid)
{
	PWSTR pszPath = NULL;
	HRESULT hr = SHGetKnownFolderPath(kfid, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"The known folder is: %s\n", pszPath);

		// The calling application is responsible for calling CoTaskMemFree 
		// to free this resource after use.
		CoTaskMemFree(pszPath);
	}
	else
	{
		// Failed to get the physical folder of the known folder.
		_tprintf(_T("SHGetKnownFolderPath failed w/err 0x%08lx\n"), hr);
	}
}

#pragma endregion



int _tmain(int argc, _TCHAR* argv[])
{
	// Initialize COM
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Enumerate and print all known folders
	_putts(_T("Print all known folders"));
	PrintAllKnownFolders();

	// Print some default known folders
	_putts(_T("\nPrint some default known folder"));
	PrintSomeDefaultKnownFolders();

#pragma region Extending Known Folders with Custom Folders

	// Create an ID for the known folder
	KNOWNFOLDERID kfid = GUID_NULL;
	CoCreateGuid(&kfid);

	// Register and create a known folder
	_putts(_T("\nRegister and create a known folder"));
	CreateKnownFolder(kfid);
	//getchar();	// Check for the known folder

	// Print the registered known folder
	_putts(_T("\nPrint the known folder"));
	PrintKnownFolder(kfid);

	// Remove and unregister the known folder
	_putts(_T("\nRemove and unregister the known folder"));
	RemoveKnownFolder(kfid);

#pragma endregion

	CoUninitialize();
	return 0;
}