'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBWin7ShellLibrary
' Copyright (c) Microsoft Corporation.
' 
' The Known Folder system provides a way to interact with certain 
' high-profile folders that are present by default in Microsoft Windows. It 
' also allows those same interactions with folders installed and registered 
' with the Known Folder system by applications. This sample demonstrates 
' those possible interactions in VB.NET as they are provided by the Known 
' Folder APIs.
' 
' A. Enumerate and print all known folders.
' 
' B. Print some built-in known folders like FOLDERID_ProgramFiles in two 
' different ways.
' 
' C. Extend known folders with custom folders. (The feature is not 
' demonstrated in the current sample, because the APIs for extending known 
' folders with custom folders have not been exposed from Windows API Code 
' Pack for Microsoft .NET Framework.)
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports Microsoft.WindowsAPICodePack.Shell

#End Region


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Enumerate and print all known folders.
        ' 

        Console.WriteLine(vbLf & "Enumerate all known folders")
        For Each kf As IKnownFolder In KnownFolders.All
            Console.WriteLine("{0}: {1}", kf.CanonicalName, kf.Path)
        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Print some known folders in Windows.
        ' 

        Dim folderPath As String

        ' Program Files
        'folderPath = KnownFolders.ProgramFiles.Path
        ' [-or-]
        folderPath = Environment.GetFolderPath( _
        Environment.SpecialFolder.ProgramFiles)
        Console.WriteLine("FOLDERID_ProgramFiles: {0}", folderPath)


        ' 
        ' Known folders for per-computer program data.
        ' 

        ' The user would never want to browse here in Explorer, and settings 
        ' changed here should affect every user on the machine. The default 
        ' location is %systemdrive%\ProgramData, which is a hidden folder, on 
        ' an installation of Windows Vista. You'll want to create your 
        ' directory and set the ACLs you need at install time.
        'folderPath = KnownFolders.ProgramData.Path
        ' [-or-]
        folderPath = Environment.GetFolderPath( _
        Environment.SpecialFolder.CommonApplicationData)
        Console.WriteLine("FOLDERID_ProgramData: {0}", folderPath)

        ' The user would want to browse here in Explorer and double click to 
        ' open the file. The default location is %public%, which has 
        ' explicit links throughout Explorer, on an installation of Windows 
        ' Vista. You'll want to create your directory and set the ACLs you 
        ' need at install time.
        'folderPath = KnownFolders.Public.Path
        ' [-or-]
        folderPath = Environment.GetEnvironmentVariable("public")
        Console.WriteLine("FOLDERID_Public: {0}", folderPath)


        ' 
        ' Known folders for per-user program data.
        ' 

        ' The user would never want to browse here in Explorer, and settings 
        ' changed here should roam with the user. The default location is 
        ' %appdata%, which is a hidden folder, on an installation of Windows 
        ' Vista.
        'folderPath = KnownFolders.RoamingAppData.Path
        ' [-or-]
        folderPath = Environment.GetFolderPath( _
        Environment.SpecialFolder.ApplicationData)
        Console.WriteLine("FOLDERID_RoamingAppData: {0}", folderPath)

        ' The user would never want to browse here in Explorer, and settings 
        ' changed here should stay local to the computer. The default 
        ' location is %localappdata%, which is a hidden folder, on an 
        ' installation of Windows Vista.
        'folderPath = KnownFolders.LocalAppData.Path)
        ' [-or-]
        folderPath = Environment.GetFolderPath( _
        Environment.SpecialFolder.LocalApplicationData)
        Console.WriteLine("FOLDERID_LocalAppData: {0}", folderPath)

        ' The user would want to browse here in Explorer and double click to 
        ' open the file. The default location is %userprofile%\documents, 
        ' which has explicit links throughout Explorer, on an installation 
        ' of Windows Vista.
        'folderPath = KnownFolders.Documents.Path);
        ' [-or-]
        folderPath = Environment.GetFolderPath( _
        Environment.SpecialFolder.MyDocuments)
        Console.WriteLine("FOLDERID_Documents: {0}", folderPath)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Extend known folders with custom folders.
        ' 

        ' TODO: The feature is not demonstrated in the current sample, 
        ' because the APIs for extending known folders with custom folders 
        ' have not been exposed from Windows API Code Pack for Microsoft 
        ' .NET Framework.

    End Sub

End Module