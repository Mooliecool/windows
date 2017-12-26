========================================================================
    CONSOLE APPLICATION : CSShellKnownFolders Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Known Folder system provides a way to interact with certain high-profile 
folders that are present by default in Microsoft Windows. It also allows 
those same interactions with folders installed and registered with the Known 
Folder system by applications. This sample demonstrates those possible 
interactions in Visual C# as they are provided by the Known Folder APIs.

A. Enumerate and print all known folders.

B. Print some built-in known folders like FOLDERID_ProgramFiles in two 
different ways.

C. Extend known folders with custom folders. (The feature is not demonstrated 
in the current sample, because the APIs for extending known folders with 
custom folders have not been exposed from Windows API Code Pack for Microsoft 
.NET Framework.)


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Windows API Code Pack for Microsoft .NET Framework 
http://code.msdn.microsoft.com/WindowsAPICodePack


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Enumerate and print all known folders. 

	foreach (IKnownFolder kf in KnownFolders.All)
	{
		Console.WriteLine("{0}: {1}", kf.CanonicalName, kf.Path);
	}

B. Print some built-in known folders like FOLDERID_ProgramFiles in two  
different ways.

  Method 1. Use KnownFolders.ProgramFiles
  
	folder = KnownFolders.ProgramFiles.Path;
  
  Method 2. Use .NET BCL Environment.GetFolderPath

	folder = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

C. Extend known folders with custom folders. (The feature is not demonstrated 
in the current sample, because the APIs for extending known folders with 
custom folders have not been exposed from Windows API Code Pack for Microsoft 
.NET Framework.)


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Working with Known Folders in Applications
http://msdn.microsoft.com/en-us/library/bb776912(VS.85).aspx

MSDN: Default Known Folders in Windows
http://msdn.microsoft.com/en-us/library/dd378457(VS.85).aspx

MSDN: Extending Known Folders with Custom Folders
http://msdn.microsoft.com/en-us/library/bb776910(VS.85).aspx

Windows API Code Pack for Microsoft .NET Framework 
http://code.msdn.microsoft.com/WindowsAPICodePack


/////////////////////////////////////////////////////////////////////////////