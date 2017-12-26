/************************************* Module Header *********************************\
 * Module Name:        ProjectFolder.cs
 * Project :           CSVSXSaveProject
 * Copyright (c)       Microsoft Corporation
 *
 * This class is used to get the files in the project folder.
 *
 * The source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Microsoft.CSVSXSaveProject.Files
{

    public static class ProjectFolder
    { 
        /// <summary>
        /// Get files in the project folder.
        /// </summary>
        /// <param name="projectFilePath">
        /// The path of the project file. 
        /// </param>
        public static List<ProjectFileItem> GetFilesInProjectFolder(string projectFilePath)
        {
            // Get the folder that includes the project file.
            FileInfo projFile = new FileInfo(projectFilePath);
            DirectoryInfo projFolder = projFile.Directory;

            if (projFolder.Exists)
            {
                // Get all files information in project folder.
                var files = projFolder.GetFiles("*", SearchOption.AllDirectories);

                return files.Select(f => new ProjectFileItem { Fileinfo = f,
                    IsUnderProjectFolder = true }).ToList();
            }
            else
            {
                // The project folder does not exist.
                return null;
            }
        }

    }
}



