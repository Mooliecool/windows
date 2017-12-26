========================================================================
    CONSOLE APPLICATION : VBWin7ShellLibrary Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Libraries are the new entry points to user data in Windows 7. Libraries are a 
natural evolution of the My Documents folder concept that blends into the 
Windows Explorer user experience. A library is a common store of user defined 
locations that applications can leverage to manage user content as their part 
of the user experience. Because libraries are not file system locations, you 
will need to update some applications to work with them like folders. 

The VBWin7ShellLibrary example demonstrates how to create, open, delete, 
rename and manage shell libraries. It also shows how to add, remove and list 
folders in a shell library.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Windows 7
http://www.microsoft.com/windows/windows-7/

Windows API Code Pack for Microsoft .NET Framework
http://code.msdn.microsoft.com/WindowsAPICodePack


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. In Visual Studio 2008, add a new Visual Basic / Windows / Console 
Application project named VBWin7ShellLibrary.

Step2. Add the reference to Microsoft.WindowsAPICodePack.dll and 
Microsoft.WindowsAPICodePack.Shell.dll available in Windows API Code Pack for 
Microsoft .NET Framework (http://code.msdn.microsoft.com/WindowsAPICodePack).

Step3. In MainModule, call the classes in the code pack to create a shell 
library, display Manage Library UI, add a folder to the library, list all 
folders in the library, remove a folder from the shell library, and delete 
the shell library. 

	1) Create a shell library
	Using library As New ShellLibrary(libraryName, True)
	End Using
	
	2) Show Manage Library UI
	ShellLibrary.ShowManageLibraryUI(libraryName, IntPtr.Zero, _
		"CSWin7ShellLibrary", "Manage Library folders and settings", True)
	
	3) Open the shell libary
	Using library As ShellLibrary = ShellLibrary.Load(libraryName, False)
	End Using
	
	4) Add a folder to the shell library
	library.Add(folderPath)
	
	5) List all folders in the library
	For Each folder As ShellFolder In library
	Next
	
	6) Remove a folder from the shell library
	library.Remove(folderPath)
	
	7) Delete the shell library
	Dim librariesPath As String = Path.Combine(Environment.GetFolderPath( _
		Environment.SpecialFolder.ApplicationData), _
		ShellLibrary.LibrariesKnownFolder.RelativePath)
	Dim libraryPath As String = Path.Combine(librariesPath, libraryName)
	Dim libraryFullPath As String = Path.ChangeExtension(libraryPath, _
		"library-ms")
	File.Delete(libraryFullPath)


/////////////////////////////////////////////////////////////////////////////
References:

Windows 7 Programming Guide – Libraries
http://windowsteamblog.com/blogs/developers/archive/2009/06/11/windows-7-programming-guide-libraries.aspx

Introducing Libraries
http://msdn.microsoft.com/en-us/magazine/dd861346.aspx

KB: How to programmatically manipulate Windows 7 shell libraries
http://support.microsoft.com/kb/976027/en-us


/////////////////////////////////////////////////////////////////////////////