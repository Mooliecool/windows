========================================================================
    CONSOLE APPLICATION : CppShellKnownFolders Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Known Folder system provides a way to interact with certain high-profile 
folders that are present by default in Microsoft Windows. It also allows 
those same interactions with folders installed and registered with the Known 
Folder system by applications. This sample demonstrates those possible 
interactions as they are provided by the Known Folder APIs.

A. Enumerate and print all known folders. (PrintAllKnownFolders)

B. Print some built-in known folders like FOLDERID_ProgramFiles in three 
different ways. (PrintSomeDefaultKnownFolders)

C. Extend known folders with custom folders.

  1. Register and create a known folder named "Sample KnownFolder" under the 
  user profile folder: C:\Users\<username>\SampleKnownFolder. The known 
  folder displays the localized name "Sample KnownFolder LocalizedName", and 
  shows a folder icon with the Sample logo. 
  (CreateKnownFolder, RegisterKnownFolder)
  
  2 Print the known folder. (PrintKnownFolder)
  
  3 Remove and unregister the known folder. 
  (RemoveKnownFolder, UnregisterKnownFolder)


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Most Known Folder APIs and interfaces demonstrated in this sample, such as 
IKnownFolderManager and SHGetKnownFolderPath, are new in Windows Vista. You 
must run the sample on Windows Vista or later operating systems.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Enumerate and print all known folders. (PrintAllKnownFolders)

  Step1. Create an IKnownFolderManager instance.
  
	IKnownFolderManager* pkfm = NULL;
	hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, 
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
	if (SUCCEEDED(hr))
	{
		...
		pkfm->Release();
	}
	
  Step2. Get the IDs of all known folders by calling 
  IKnownFolderManager::GetFolderIds.
  
	KNOWNFOLDERID *rgKFIDs = NULL;
	UINT cKFIDs = 0;
	hr = pkfm->GetFolderIds(&rgKFIDs, &cKFIDs);
	if (SUCCEEDED(hr))
	{
		...
		CoTaskMemFree(rgKFIDs);
	}
  
  Step3. For each known folder ID, get the IKnownFolder instance.

	IKnownFolder *pkfCurrent = NULL;
	for (UINT i = 0; i < cKFIDs; ++i)
	{
		hr = pkfm->GetFolder(rgKFIDs[i], &pkfCurrent);
		...
		pkfCurrent->Release();
	}

  Step4. Get the non-localized, canonical name and path of the known folder 
  from KNOWNFOLDER_DEFINITION.

	KNOWNFOLDER_DEFINITION kfd;
	hr = pkfCurrent->GetFolderDefinition(&kfd);
	if (SUCCEEDED(hr))
	{
		hr = pkfCurrent->GetPath(0, &pszPath);
		if (SUCCEEDED(hr))
		{
			wprintf(L"%s: %s\n", kfd.pszName, pszPath);
				CoTaskMemFree(pszPath);
		}
		FreeKnownFolderDefinitionFields(&kfd);
	}
  
B. Print some built-in known folders like FOLDERID_ProgramFiles in three 
different ways. (PrintSomeDefaultKnownFolders)

  Method 1. Use SHGetKnownFolderPath (The function is new in Windows Vista)
  The system default KNOWNFOLDERIDs are documented in 
  http://msdn.microsoft.com/en-us/library/bb762584(VS.85).aspx
  
	PWSTR pszPath = NULL;
	hr = SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, NULL, &pszPath);
	if (SUCCEEDED(hr))
	{
		wprintf(L"FOLDERID_ProgramFiles: %s\n", pszPath);

		// The calling application is responsible for calling CoTaskMemFree 
		// to free this resource after use.
		CoTaskMemFree(pszPath);
	}

  Method 2. Use IKnownFolderManager::GetGetFolder, IKnownFolder::GetPath 
  (The functions are new in Windows Vista)

	PWSTR pszPath = NULL;
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

  Method 3. Use SHGetFolderPath (The function is deprecated. As of Windows 
  Vista, this function is merely a wrapper for SHGetKnownFolderPath.)
  The CSIDLs of the system default known folders are documented in 
  http://msdn.microsoft.com/en-us/library/bb762494(VS.85).aspx

	TCHAR szFolderPath[MAX_PATH];
	hr = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, 
		szFolderPath);
	if (SUCCEEDED(hr))
	{
		_tprintf(_T("FOLDERID_ProgramFiles: %s\n"), szFolderPath);
	}

C. Extend known folders with custom folders.

Independent software vendors (ISVs) can extend the set of known folders on a 
system by registering known folders of their own. Once registered, those 
third-party folders are known to the system. They are found by any call to 
GetFolderIds. Note that a known folder must be a per-machine folder. You 
cannot create a per-user known folder. Registering or unregistering a known 
folder requires administrator privilege.

  1 Register and create a known folder named "Sample KnownFolder" under the 
  user profile folder: C:\Users\<username>\SampleKnownFolder. The known 
  folder displays the localized name "Sample KnownFolder LocalizedName", and 
  shows a folder icon with the Sample logo. 
  (CreateKnownFolder, RegisterKnownFolder)
  
  CreateKnownFolder calls RegisterKnownFolder to register a known folder. In 
  RegisterKnownFolder, first define the known folder through a 
  KNOWNFOLDER_DEFINITION structure. You can specify the known folder's 
  canonical name, localized name, tooltip, folder icon, etc. Then register 
  the known folder through a call to IKnownFolderManager::RegisterFolder. 
  IKnownFolderManager::RegisterFolder requires that the current process is 
  run as administrator to succeed. 
  
  After the known folder is register, CreateKnownFolder initializes and 
  creates the folder with SHGetKnownFolderPath with the flags KF_FLAG_CREATE 
  | KF_FLAG_INIT so that the Shell will write desktop.ini in the folder. This 
  is how our customizations (i.e. pszIcon, pszTooltip, pszLocalizedName) get 
  picked up by the Shell. If SHGetKnownFolderPath fails, the function 
  UnregisterKnownFolder is invoked to undo the registration.
  
  2 Print the known folder. (PrintKnownFolder)

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

  3 Remove and unregister the known folder. 
  (RemoveKnownFolder, UnregisterKnownFolder)

  RemoveKnownFolder is responsible for remove and unregister the specified 
  known folder. It first gets the physical folder path of the known folder, 
  and attempts to delete it. When the deletion succeeds, the function calls 
  UnregisterKnownFolder to unregister the known folder from registry. 
  UnregisterKnownFolder requires administrator privilege, so please make sure 
  that the routine is run as administrator.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Working with Known Folders in Applications
http://msdn.microsoft.com/en-us/library/bb776912(VS.85).aspx

MSDN: Default Known Folders in Windows
http://msdn.microsoft.com/en-us/library/dd378457(VS.85).aspx

MSDN: Extending Known Folders with Custom Folders
http://msdn.microsoft.com/en-us/library/bb776910(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////