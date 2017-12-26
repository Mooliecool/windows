/****************************** Module Header ******************************\
* Module Name:  CppWin7ShellLibrary.cpp
* Project:      CppWin7ShellLibrary
* Copyright (c) Microsoft Corporation.
* 
* Libraries are the new entry points to user data in Windows 7. Libraries are  
* a natural evolution of the My Documents folder concept that blends into the 
* Windows Explorer user experience. A library is a common store of user 
* defined locations that applications can leverage to manage user content as 
* their part of the user experience. Because libraries are not file system 
* locations, you will need to update some applications to work with them like 
* folders. 
* 
* The CppWin7ShellLibrary example demonstrates how to create, open, delete, 
* rename and manage shell libraries. It also shows how to add, remove and 
* list folders in a shell library.
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
#include <shobjidl.h>	// Define IShellLibrary and other helper functions
#include <shlobj.h>
#include <knownfolders.h>
#include <propkey.h>
#pragma endregion


#pragma region Helper Functions of Shell Library (Create/Open/Delete/Rename/Manage)

HRESULT CreateShellLibrary(LPWSTR pwszLibraryName);
HRESULT OpenShellLibrary(LPWSTR pwszLibraryName, IShellLibrary** ppShellLib);
HRESULT DeleteShellLibrary(LPWSTR pwszLibraryName);
HRESULT RenameShellLibrary(LPWSTR pwszOriginalLibraryName, LPWSTR pwszNewLibraryName);
HRESULT ShowManageLibraryUI(LPWSTR pwszLibraryName);
HRESULT GetShellLibraryItem(LPWSTR pwszLibraryName, IShellItem2** ppShellItem);


/*!
* Create a new shell library under the user's Libraries folder. If a library 
* with the same name already exists, the new one overrides the existing one.
* 
* \param pwszLibraryName
* The name of the shell library to be created. 
*/
HRESULT CreateShellLibrary(LPWSTR pwszLibraryName)
{
	HRESULT hr;

	/////////////////////////////////////////////////////////////////////////
	// Create the shell library COM object.
	// 

	IShellLibrary* pShellLib = NULL;
	hr = SHCreateLibrary(IID_PPV_ARGS(&pShellLib));
	if (FAILED(hr))
	{
		_tprintf(_T("SHCreateLibrary failed to create the shell library ") \
			_T("COM object w/err 0x%08lx\n"), hr);
		return hr;
	}


	////////////////////////////////////////////////////////////////////////
	// Save the new library under the user's Libraries folder.
	// 

	IShellItem* pSavedTo = NULL;
	hr = pShellLib->SaveInKnownFolder(FOLDERID_UsersLibraries, 
		pwszLibraryName, LSF_OVERRIDEEXISTING, &pSavedTo);
	if (SUCCEEDED(hr))
	{
		pSavedTo->Release();
	}
	else
	{
		_tprintf(_T("IShellLibrary::SaveInKnownFolder failed to save the ") \
			_T("library w/err 0x%08lx\n"), hr);
	}

	// Clean up
	pShellLib->Release();

	return hr;
}


/*!
* Open the shell library under the user's Libraries folder according to the 
* specified library name with both read and write permissions.
* 
* \param pwszLibraryName
* The name of the shell library to be opened.
* 
* \param ppShellLib
* If the open operation succeeds, ppShellLib outputs the IShellLibrary 
* interface of the shell library object. The caller is responsible for calling
* Release on the shell library. If the function fails, NULL is returned from 
* *ppShellLib.
*/
HRESULT OpenShellLibrary(LPWSTR pwszLibraryName, IShellLibrary** ppShellLib)
{
	HRESULT hr;
	*ppShellLib = NULL;

	/////////////////////////////////////////////////////////////////////////
	// Get the shell item that represents the library.
	// 

	IShellItem2* pShellItem = NULL;
	hr = GetShellLibraryItem(pwszLibraryName, &pShellItem);
	if (FAILED(hr))
	{
		return hr;
	}

	//// Print file-system full path of the shell item
	//LPWSTR wszLibraryFullPath = NULL;
	//hr = pShellItem->GetString(PKEY_ParsingPath, &wszLibraryFullPath);
	//if (SUCCEEDED(hr))
	//{
	//	wprintf(L"Full path of library: %s\n", wszLibraryFullPath);
	//	CoTaskMemFree(wszLibraryFullPath);
	//}


	/////////////////////////////////////////////////////////////////////////
	// Get the shell library object from the shell item with a read and write 
	// permitions.
	// 

	hr = SHLoadLibraryFromItem(pShellItem, STGM_READWRITE, 
		IID_PPV_ARGS(ppShellLib));
	if (FAILED(hr))
	{
		_tprintf(_T("SHLoadLibraryFromItem failed to get the shell ") \
			_T("library from shell item w/err 0x%08lx\n"), hr);
	}

	// Clean up
	pShellItem->Release();

	return hr;
}


/*!
* Delete the shell library under the user's Libraries folder according to the 
* specified library name. 
* 
* \param pwszLibraryName
* The name of the shell library to be deleted.
*/
HRESULT DeleteShellLibrary(LPWSTR pwszLibraryName)
{
	HRESULT hr;

	/////////////////////////////////////////////////////////////////////////
	// Get the shell item that represents the library.
	// 

	IShellItem2* pShellItem = NULL;
	hr = GetShellLibraryItem(pwszLibraryName, &pShellItem);
	if (FAILED(hr))
	{
		return hr;
	}


	/////////////////////////////////////////////////////////////////////////
	// Get the file-system full path of the shell item and delete the file.
	// 

	// Get the file-system full path of the shell item
	LPWSTR wszLibraryFullPath = NULL;
	hr = pShellItem->GetString(PKEY_ParsingPath, &wszLibraryFullPath);
	if (SUCCEEDED(hr))
	{
		// Delete the file
		if (!DeleteFileW(wszLibraryFullPath))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		CoTaskMemFree(wszLibraryFullPath);
	}
	else
	{
		_tprintf(_T("IShellItem2::GetString failed w/err 0x%08lx\n"), hr);
	}

	// Clean up
	pShellItem->Release();

	return hr;
}


/*!
* Rename the specified shell library under the user's Libraries.
* 
* \param pwszOriginalLibraryName
* The original name of the shell library to be renamed.
* 
* \param pwszNewLibraryName
* The new name of the shell library
*/
HRESULT RenameShellLibrary(LPWSTR pwszOriginalLibraryName, 
						   LPWSTR pwszNewLibraryName)
{
	HRESULT hr;

	/////////////////////////////////////////////////////////////////////////
	// Open the original shell library.
	// 

	IShellLibrary* pShellLib = NULL;
	hr = OpenShellLibrary(pwszOriginalLibraryName, &pShellLib);
	if (FAILED(hr))
	{
		return hr;
	}


	/////////////////////////////////////////////////////////////////////////
	// Save a new copy of the library under the user's Libraries folder with 
	// the new name.
	// 

	IShellItem* pSavedTo = NULL;
	hr = pShellLib->SaveInKnownFolder(FOLDERID_UsersLibraries, 
		pwszNewLibraryName, LSF_OVERRIDEEXISTING, &pSavedTo);
	if (SUCCEEDED(hr))
	{
		pSavedTo->Release();
	}
	else
	{
		_tprintf(_T("IShellLibrary::SaveInKnownFolder failed to save the ") \
			_T("library w/err 0x%08lx\n"), hr);
	}

	// Clean up
	pShellLib->Release();


	/////////////////////////////////////////////////////////////////////////
	// Delete the old copy of the shell library.
	// 

	if (SUCCEEDED(hr))
	{
		hr = DeleteShellLibrary(pwszOriginalLibraryName);
	}

	return hr;
}


/*!
* Shows the library management dialog box of the specified library, which 
* enables users to manage the library folders and default save location.
* 
* \param pwszLibraryName
* The name of the shell library
*/
HRESULT ShowManageLibraryUI(LPWSTR pwszLibraryName)
{
	// Get the shell item that represents the library.
	IShellItem2* pShellItem = NULL;
	HRESULT hr = GetShellLibraryItem(pwszLibraryName, &pShellItem);
	if (SUCCEEDED(hr))
	{
		hr = SHShowManageLibraryUI(pShellItem, NULL, 
			L"CppWin7ShellLibrary", L"Manage Library folders and settings", 
			LMD_ALLOWUNINDEXABLENETWORKLOCATIONS);

		pShellItem->Release();
	}
	return hr;
}


/*!
* Get the shell item that represents the library.
* 
* \param pwszLibraryName
* The name of the shell library
* 
* \param ppShellItem
* If the operation succeeds, ppShellItem outputs the IShellItem2 interface  
* that represents the library. The caller is responsible for calling 
* Release on the shell item. If the function fails, NULL is returned from 
* *ppShellItem.
*/
HRESULT GetShellLibraryItem(LPWSTR pwszLibraryName, IShellItem2** ppShellItem)
{
	HRESULT hr;
	*ppShellItem = NULL;

	// Create the real library file name
	WCHAR wszRealLibraryName[MAX_PATH];
	swprintf_s(wszRealLibraryName, MAX_PATH, L"%s%s", pwszLibraryName, 
		L".library-ms");

	// Get the shell item corresponding to the library file name
	IShellItem2* pShellItem = NULL;
	hr = SHCreateItemInKnownFolder(FOLDERID_UsersLibraries, 
		KF_FLAG_DEFAULT_PATH | KF_FLAG_NO_ALIAS, wszRealLibraryName, 
		IID_PPV_ARGS(ppShellItem));
	if (FAILED(hr))
	{
		_tprintf(_T("SHCreateItemInKnownFolder failed to get the shell ") \
			_T("item that represents the library w/err 0x%08lx\n"), hr);
	}
	
	return hr;
}

#pragma endregion


#pragma region Helper Functions of Folder in Shell Library (Add/Remove/List)

HRESULT AddFolderToShellLibrary(IShellLibrary* pShellLib, LPWSTR pwszFolderPath, BOOL bSaveLocation);
HRESULT RemoveFolderFromShellLibrary(IShellLibrary* pShellLib, LPWSTR pwszFolderPath);
HRESULT PrintAllFoldersInShellLibrary(IShellLibrary* pShellLib);


/*!
* Add a folder to an existing shell library.
* 
* \param pShellLib
* The IShellLibrary interface of the shell library
* 
* \param pwszFolderPath
* The path of the folder to be added into the shell library
* 
* \param bSaveLocation
* If bSaveLocation is TRUE, set the folder as the save location of the shell 
* library
*/
HRESULT AddFolderToShellLibrary(IShellLibrary* pShellLib, 
								LPWSTR pwszFolderPath, BOOL bSaveLocation)
{
	HRESULT hr;

	// Attempt to add the folder path to the shell library
	hr = SHAddFolderPathToLibrary(pShellLib, pwszFolderPath);
	if (FAILED(hr))
	{
		_tprintf(_T("SHAddFolderPathToLibrary failed to add a folder ") \
			_T("to the shell library w/err 0x%08lx\n"), hr);
		return hr;
	}

	// Set the folder as the save location of the shell library
	if (bSaveLocation)
	{
		// Get the shell item from the folder path
		IShellItem2* pShellItemSaveFolder = NULL;
		hr = SHCreateItemFromParsingName(pwszFolderPath, 0, 
			IID_PPV_ARGS(&pShellItemSaveFolder));
		if (SUCCEEDED(hr))
		{
			// Set the folder as the save location
			hr = pShellLib->SetDefaultSaveFolder(DSFT_DETECT, 
				pShellItemSaveFolder);
			if (FAILED(hr))
			{
				_tprintf(_T("IShellLibrary::SetDefaultSaveFolder failed ") \
					_T("w/err 0x%08lx\n"), hr);
			}
			pShellItemSaveFolder->Release();
		}
		else
		{
			_tprintf(_T("SHCreateItemFromParsingName failed w/err ") \
				_T("0x%08lx\n"), hr);
		}
	}

	if (SUCCEEDED(hr))
	{
		// Commit the change of the shell library
		hr = pShellLib->Commit();
	}

	return hr;
}


/*!
* Remove a folder from an existing shell library.
* 
* \param pShellLib
* The IShellLibrary interface of the shell library
* 
* \param pwszFolderPath
* The path of the folder to be removed from the shell library
*/
HRESULT RemoveFolderFromShellLibrary(IShellLibrary* pShellLib, 
									 LPWSTR pwszFolderPath)
{
	HRESULT hr = SHRemoveFolderPathFromLibrary(pShellLib, pwszFolderPath);
	if (SUCCEEDED(hr))
	{
		// Commit the change of the shell library
		pShellLib->Commit();
	}
	else
	{
		_tprintf(_T("SHRemoveFolderPathFromLibrary failed to remove a ") \
			_T("folder from the shell library w/err 0x%08lx\n"), hr);
	}
	return hr;
}


/*!
* Print all folders in the shell library.
* 
* \param pShellLib
* The IShellLibrary interface of the shell library
*/
HRESULT PrintAllFoldersInShellLibrary(IShellLibrary* pShellLib)
{
	HRESULT hr;

	IShellItemArray* pShellItemArray = NULL;
	hr = pShellLib->GetFolders(LFF_ALLITEMS, IID_PPV_ARGS(&pShellItemArray));
	if (FAILED(hr))
	{
		_tprintf(_T("IShellLibrary::GetFolders failed to get the folders ") \
			_T("of the shell library w/err 0x%08lx\n"), hr);
		return hr;
	}

	DWORD dwFolderCount;
	hr = pShellItemArray->GetCount(&dwFolderCount);

	// Iterate through all folders of the shell library
	for (DWORD i = 0; i < dwFolderCount; i++)
	{
		IShellItem* pShellItem = NULL;
		hr = pShellItemArray->GetItemAt(i, &pShellItem);
		if (FAILED(hr))
			continue;

		// Convert IShellItem to IShellItem2
		IShellItem2* pShellItem2 = NULL;
		pShellItem->QueryInterface(IID_PPV_ARGS(&pShellItem2));
		pShellItem->Release();

		// Fix folder path changes
		IShellItem2* pShellItemResolvedFolder = NULL;
		hr = pShellLib->ResolveFolder(pShellItem2, 5000, IID_PPV_ARGS(
			&pShellItemResolvedFolder));
		if (SUCCEEDED(hr))
		{
			// Point to the fixed folder
			pShellItem2->Release();
			pShellItem2 = pShellItemResolvedFolder;
		}
		// Else we will show the unfixed folder

		// Print the folder path
		LPWSTR wszFolderPath = NULL;
		hr = pShellItem2->GetString(PKEY_ParsingPath, &wszFolderPath);
		if (SUCCEEDED(hr))
		{
			_putws(wszFolderPath);
			CoTaskMemFree(wszFolderPath);
		}

		// Clean up
		pShellItem2->Release();
	}

	pShellItemArray->Release();

	return hr;
}

#pragma endregion



int _tmain(int argc, _TCHAR* argv[])
{
	// Initiate COM
	CoInitialize(NULL);

	LPWSTR pwszLibraryName = L"All-In-One Code Framework";


	/////////////////////////////////////////////////////////////////////////
	// Create a shell library.
	// 

	wprintf(L"Create shell library: %s\n", pwszLibraryName);
	if (FAILED(CreateShellLibrary(pwszLibraryName)))
		return 1;

	_putts(_T("Press ENTER to continue..."));
	getchar();


	/////////////////////////////////////////////////////////////////////////
	// Show Manage Library UI.
	// 

	_putts(_T("Show Manage Library UI"));

	// ShowManageLibraryUI requires that the library is not currently opened 
	// with write permission
	if (FAILED(ShowManageLibraryUI(pwszLibraryName)))
		return 1;

	_putts(_T("Press ENTER to continue..."));
	getchar();


	/////////////////////////////////////////////////////////////////////////
	// Open the shell library with read and write permissions.
	// 

	wprintf(L"Open shell library: %s\n", pwszLibraryName);

	IShellLibrary* pShellLib = NULL;
	if (FAILED(OpenShellLibrary(pwszLibraryName, &pShellLib)))
		return 1;


	/////////////////////////////////////////////////////////////////////////
	// Add a folder to the shell library.
	// 

	_putts(_T("Add a folder to the shell library"));

	WCHAR wszFolderPath[MAX_PATH] = {};

	// Display common dialog for selecting the folder to be added
	IFileDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		hr = pfd->SetOptions(FOS_PICKFOLDERS);
		if (SUCCEEDED(hr))
		{
			// Show the open folder dialog
			hr = pfd->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// Obtain the result of the user interaction.
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr))
				{
					LPWSTR pwszFolderPath = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, 
						&pwszFolderPath);
					if (SUCCEEDED(hr))
					{
						// Copy pwszFolderPath to the local wszFolderPath
						wcscpy_s(wszFolderPath, MAX_PATH, pwszFolderPath);

						CoTaskMemFree(pwszFolderPath);
					}
					psiResult->Release();
				}
			}
		}
		pfd->Release();
	}

	if (FAILED(hr))
	{
		_putts(_T("No folder is selected"));
		return 1;
	}

	wprintf(L"The selected folder is %s\n", wszFolderPath);

	// Add the folder to the shell library
	if (FAILED(AddFolderToShellLibrary(pShellLib, wszFolderPath, TRUE)))
		return 1;

	_putts(_T("Press ENTER to continue..."));
	getchar();


	/////////////////////////////////////////////////////////////////////////
	// Print all folders in the library.
	// 

	_putts(_T("Print all folders in the library"));

	PrintAllFoldersInShellLibrary(pShellLib);

	_putts(_T("Press ENTER to continue..."));
	getchar();


	/////////////////////////////////////////////////////////////////////////
	// Remove a folder from the shell library.
	// 

	_putts(_T("Remove a folder from the shell library"));

	if (FAILED(RemoveFolderFromShellLibrary(pShellLib, wszFolderPath)))
		return 1;

	_putts(_T("Press ENTER to continue..."));
	getchar();


	/////////////////////////////////////////////////////////////////////////
	// Delete the shell library.
	// 

	// Clean up the previous reference of the shell library
	pShellLib->Release();

	_putts(_T("Delete the shell library"));

	if (FAILED(DeleteShellLibrary(pwszLibraryName)))
		return 1;


	CoUninitialize();
	return 0;
}