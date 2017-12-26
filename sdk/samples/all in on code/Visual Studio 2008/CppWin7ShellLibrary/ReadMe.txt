========================================================================
    CONSOLE APPLICATION : CppWin7ShellLibrary Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Libraries are the new entry points to user data in Windows 7. Libraries are a 
natural evolution of the My Documents folder concept that blends into the 
Windows Explorer user experience. A library is a common store of user defined 
locations that applications can leverage to manage user content as their part 
of the user experience. Because libraries are not file system locations, you 
will need to update some applications to work with them like folders. 

The CppWin7ShellLibrary example demonstrates how to create, open, delete, 
rename and manage shell libraries. It also shows how to add, remove and list 
folders in a shell library.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1
http://www.microsoft.com/downloads/details.aspx?FamilyID=c17ba869-9671-4330-a63e-1fd44e0e2505&displaylang=en

Windows 7
http://www.microsoft.com/windows/windows-7/


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. In Visual Studio 2008, add a new Visual C++ / Win32 / Win32 Console 
Application project named CppWin7ShellLibrary.

Step2. In targetver.h, change _WIN32_WINNT from the default value 0x0600 
(Vista) to 0x0601 (Windows 7).

	#define _WIN32_WINNT 0x0601

Step3. Include the following header files in Windows 7 SDK

	#include <shobjidl.h>	// Define IShellLibrary and other helper functions
	#include <shlobj.h>
	#include <knownfolders.h>
	#include <propkey.h>

Step4. Add the following helper functions of shell library:

	CreateShellLibrary		- Create shell library
	OpenShellLibrary		- Open an existing shell library
	DeleteShellLibrary		- Delete a shell library
	RenameShellLibrary		- Rename a shell library
	ShowManageLibraryUI		- Show Manage Library UI
	
and the helper functions about folders in shell library:

	AddFolderToShellLibrary		- Add a folder to the specified shell library
	RemoveFolderFromShellLibrary- Remove a folder from a shell library
	PrintAllFoldersInShellLibrary- Print all folders in a shell library

These helper functions rely on the IShellLibrary interface and the shell 
libary APIs:

	SHAddFolderPathToLibrary (adds a folder to a library) 
	SHCreateLibrary (creates an IShellLibrary object) 
	SHLoadLibraryFromItem (creates and loads an IShellLibrary object from a 
		specified library definition file) 
	SHLoadLibraryFromKnownFolder (creates and loads an IShellLibrary object 
		for a specified KNOWNFOLDERID) 
	SHLoadLibraryFromParsingName (creates and loads an IShellLibrary object 
		for a specified path) 
	SHRemoveFolderPathFromLibrary (removes a folder from a library) 
	SHResolveFolderPathInLibrary (attempts to resolve the target location of 
		a library folder that has been moved or renamed) 
	SHSaveLibraryInFolderPath (saves an IShellLibrary object to disk) 

Step5. In _tmain, call the above helper functions to create a shell library, 
display Manage Library UI, add a folder to the library, list all folders in 
the library, remove a folder from the shell library, and delete the shell 
library.

	1) Create a shell library
	CreateShellLibrary(pwszLibraryName);
	
	2) Show Manage Library UI
	ShowManageLibraryUI(pwszLibraryName);
	
	3) Open the shell libary
	IShellLibrary* pShellLib = NULL;
	OpenShellLibrary(pwszLibraryName, &pShellLib);
	
	4) Add a folder to the shell library
	AddFolderToShellLibrary(pShellLib, pwszFolderPath, TRUE);
	
	5) List all folders in the library
	ListFoldersInShellLibrary(pShellLib);
	
	6) Remove a folder from the shell library
	RemoveFolderFromShellLibrary(pShellLib, pwszFolderPath);
	
	7) Delete the shell library
	DeleteShellLibrary(pwszLibraryName);


/////////////////////////////////////////////////////////////////////////////
References:

Windows 7 Programming Guide ¨C Libraries
http://windowsteamblog.com/blogs/developers/archive/2009/06/11/windows-7-programming-guide-libraries.aspx

Introducing Libraries
http://msdn.microsoft.com/en-us/magazine/dd861346.aspx

KB: How to programmatically manipulate Windows 7 shell libraries
http://support.microsoft.com/kb/976027/en-us


/////////////////////////////////////////////////////////////////////////////