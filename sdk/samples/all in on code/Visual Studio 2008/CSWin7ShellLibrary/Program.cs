/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSWin7ShellLibrary
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
* The CSWin7ShellLibrary example demonstrates how to create, open, delete, 
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

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Microsoft.WindowsAPICodePack.Shell;
using Microsoft.WindowsAPICodePack.Dialogs;
#endregion


class Program
{
    static void Main(string[] args)
    {
        string libraryName = "All-In-One Code Framework";


        /////////////////////////////////////////////////////////////////////
        // Create a shell library.
        // 

        Console.WriteLine("Create shell library: {0}", libraryName);
        using (ShellLibrary library = new ShellLibrary(libraryName, true))
        {
        }

        Console.WriteLine("Press ENTER to continue...");
        Console.ReadLine();


        /////////////////////////////////////////////////////////////////////
        // Show Manage Library UI.
        // 

        Console.WriteLine("Show Manage Library UI");

        // ShowManageLibraryUI requires that the library is not currently 
        // opened with write permission. 
        ShellLibrary.ShowManageLibraryUI(libraryName, IntPtr.Zero,
            "CSWin7ShellLibrary", "Manage Library folders and settings", true);

        Console.WriteLine("Press ENTER to continue...");
        Console.ReadLine();


        // Open the shell libary
        Console.WriteLine("Open shell library: {0}", libraryName);
        using (ShellLibrary library = ShellLibrary.Load(libraryName, false))
        {
            /////////////////////////////////////////////////////////////////
            // Add a folder to the shell library.
            // 

            Console.WriteLine("Add a folder to the shell library");

            string folderPath;

            // Display common dialog for selecting the folder to be added
            CommonOpenFileDialog fileDlg = new CommonOpenFileDialog();
            fileDlg.IsFolderPicker = true;
            if (fileDlg.ShowDialog() == CommonFileDialogResult.Cancel)
                return;

            folderPath = fileDlg.FileName;
            Console.WriteLine("The selected folder is {0}", folderPath);

            // Add the folder to the shell library
            library.Add(folderPath);
            library.DefaultSaveFolder = folderPath;

            Console.WriteLine("Press ENTER to continue...");
            Console.ReadLine();
            

            /////////////////////////////////////////////////////////////////
            // List all folders in the library.
            // 

            Console.WriteLine("List all folders in the library");

            foreach (ShellFolder folder in library)
            {
                Console.WriteLine(folder);
            }

            Console.WriteLine("Press ENTER to continue...");
            Console.ReadLine();


            /////////////////////////////////////////////////////////////////
            // Remove a folder from the shell library.
            // 

            Console.WriteLine("Remove a folder from the shell library");

            library.Remove(folderPath);

            Console.WriteLine("Press ENTER to continue...");
            Console.ReadLine();
        }


        /////////////////////////////////////////////////////////////////////
        // Delete the shell library.
        // 

        Console.WriteLine("Delete the shell library");

        string librariesPath = Path.Combine(Environment.GetFolderPath(
            Environment.SpecialFolder.ApplicationData), 
            ShellLibrary.LibrariesKnownFolder.RelativePath);

        string libraryPath = Path.Combine(librariesPath, libraryName);
        string libraryFullPath = Path.ChangeExtension(libraryPath, "library-ms");

        File.Delete(libraryFullPath);
    }
}
