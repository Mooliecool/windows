'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBWin7ShellLibrary
' Copyright (c) Microsoft Corporation.
' 
' Libraries are the new entry points to user data in Windows 7. Libraries are 
' a natural evolution of the My Documents folder concept that blends into the 
' Windows Explorer user experience. A library is a common store of user 
' defined locations that applications can leverage to manage user content as 
' their part of the user experience. Because libraries are not file system 
' locations, you will need to update some applications to work with them like 
' folders. 
' 
' The VBWin7ShellLibrary example demonstrates how to create, open, delete, 
' rename and manage shell libraries. It also shows how to add, remove and 
' list folders in a shell library.
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
Imports Microsoft.WindowsAPICodePack.Dialogs
Imports System.IO

#End Region


Module MainModule

    Sub Main()

        Dim libraryName As String = "All-In-One Code Framework"


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Create a shell library.
        ' 

        Console.WriteLine("Create shell library: {0}", libraryName)
        Using library As New ShellLibrary(libraryName, True)
        End Using

        Console.WriteLine("Press ENTER to continue...")
        Console.ReadLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Show Manage Library UI.
        ' 

        Console.WriteLine("Show Manage Library UI")

        ' ShowManageLibraryUI requires that the library is not currently 
        ' opened with write permission. 
        ShellLibrary.ShowManageLibraryUI(libraryName, IntPtr.Zero, _
            "CSWin7ShellLibrary", "Manage Library folders and settings", True)

        Console.WriteLine("Press ENTER to continue...")
        Console.ReadLine()

        ' Open the shell libary
        Console.WriteLine("Open shell library: {0}", libraryName)

        Using library As ShellLibrary = ShellLibrary.Load(libraryName, False)

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Add a folder to the shell library.
            ' 

            Console.WriteLine("Add a folder to the shell library")

            Dim folderPath As String

            ' Display common dialog for selecting the folder to be added
            Dim fileDlg As New CommonOpenFileDialog
            fileDlg.IsFolderPicker = True
            If fileDlg.ShowDialog() = CommonFileDialogResult.Cancel Then
                Return
            End If

            folderPath = fileDlg.FileName
            Console.WriteLine("The selected folder is {0}", folderPath)

            ' Add the folder to the shell library
            library.Add(folderPath)
            library.DefaultSaveFolder = folderPath

            Console.WriteLine("Press ENTER to continue...")
            Console.ReadLine()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' List all folders in the library.
            ' 

            Console.WriteLine("List all folders in the library")

            For Each folder As ShellFolder In library
                Console.WriteLine(folder)
            Next

            Console.WriteLine("Press ENTER to continue...")
            Console.ReadLine()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Remove a folder from the shell library.
            ' 

            Console.WriteLine("Remove a folder from the shell library")

            library.Remove(folderPath)

            Console.WriteLine("Press ENTER to continue...")
            Console.ReadLine()

        End Using


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Delete the shell library.
        ' 

        Console.WriteLine("Delete the shell library")

        Dim librariesPath As String = Path.Combine(Environment.GetFolderPath( _
            Environment.SpecialFolder.ApplicationData), _
            ShellLibrary.LibrariesKnownFolder.RelativePath)

        Dim libraryPath As String = Path.Combine(librariesPath, libraryName)
        Dim libraryFullPath As String = Path.ChangeExtension(libraryPath, _
                                                             "library-ms")

        File.Delete(libraryFullPath)

    End Sub

End Module
