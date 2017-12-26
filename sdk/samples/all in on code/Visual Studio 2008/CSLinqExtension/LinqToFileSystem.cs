/****************************** Module Header ******************************\
* Module Name:    LinqToFileSystem.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* It is a simple LINQ to File System library to return the FileInfor of all 
* the files under one folder, and to get the largest files among these files.
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
#endregion


namespace CSLinqExtension
{
    // Static class LinqToCSV to make extension methods
    public static class LinqToFileSystem
    {
        /// <summary>
        /// Get the FileInfo of all the files under one folder
        /// </summary>
        /// <param name="path">The path of the certain folder</param>
        /// <returns>IEnumerable collection of FileInfo</returns>
        public static IEnumerable<FileInfo> GetFiles(string path)
        {
            // Check if the folder path exists
            if (Directory.Exists(path))
            {
                // Get all file names under the foler
                var fileNames = Directory.GetFiles(path, "*.*", 
                    SearchOption.AllDirectories);

                // Create new FileInfos to based on the file names
                foreach (var name in fileNames)
                {
                    yield return new FileInfo(name);
                }
            }
            else
            {
                throw new DirectoryNotFoundException();
            }
        }

        /// <summary>
        /// Extension method of IEnumerable(FileInfo) to get the largest 
        /// files inside.
        /// </summary>
        /// <returns>IEnumerable collection of largest files' FileInfo</returns>
        public static IEnumerable<FileInfo> LargestFiles(
            this IEnumerable<FileInfo> files)
        {
            List<FileInfo> fileLists = new List<FileInfo>();

            long maxLength = 0;

            // Check each file to get the largest files
            foreach (var file in files)
            {
                if (file.Length > maxLength)
                {
                    fileLists.Clear();
                    fileLists.Add(file);
                    maxLength = file.Length;
                }
                else if (file.Length == maxLength)
                {
                    fileLists.Add(file);
                }
            }

            return fileLists;
        }
    }
}
