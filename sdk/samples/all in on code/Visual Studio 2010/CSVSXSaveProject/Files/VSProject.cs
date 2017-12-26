/************************************* Module Header *********************************\
 * Module Name:        VSProject.cs
 * Project :           CSVSXSaveProject
 * Copyright (c)       Microsoft Corporation
 * 
 * This class represents a project in the solution. It supplies method to get the all
 * the files included in the project. 
 * 
 * The source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

using System;
using System.Collections.Generic;
using System.IO;
using EnvDTE;

namespace Microsoft.CSVSXSaveProject.Files
{

    public class VSProject
    {
        /// <summary>
        /// A project of a solution.
        /// </summary>
        public Project Project { get; private set; }

        /// <summary>
        /// The folder that contains the project file.
        /// </summary>
        public DirectoryInfo ProjectFolder{get; private set;}

        /// <summary>
        /// Initialize the Project and ProjectFolder properties.
        /// </summary>
        public VSProject(Project proj)
        {
            // Initial the project object.
            this.Project = proj;
            
            // Get the current project directory.
            this.ProjectFolder = new FileInfo(Project.FullName).Directory;
        }

        /// <summary>
        /// Get all the files included in the project. 
        /// </summary>
        public List<ProjectFileItem> GetIncludedFiles()
        {
            var files = new List<ProjectFileItem>();

            // Add the project file (*.csproj or *.vbproj...) to the list of files.
            files.Add(new ProjectFileItem
            {
                Fileinfo = new FileInfo(Project.FullName),
                NeedCopy = true,
                IsUnderProjectFolder = true
            });

            // Add the files included in the project.
            foreach (ProjectItem item in Project.ProjectItems)
            {
                GetProjectItem(item, files);
            }

            return files;
        }

        /// <summary>
        /// Get all the files included in the project. 
        /// </summary>
        void GetProjectItem(ProjectItem item, List<ProjectFileItem> files)
        {
            // Gets the files associated with a ProjectItem.
            // Most project items consist of only one file, but some can have more than
            // one, as with forms in Visual Basic that are saved as both .frm (text) and
            // .frx (binary) files.
            // See http://msdn.microsoft.com/en-us/library/envdte.projectitem.filecount.aspx
            for (short i = 0; i < item.FileCount; i++)
            {
                if (File.Exists(item.FileNames[i]))
                {
                    ProjectFileItem fileItem = new ProjectFileItem();

                    fileItem.Fileinfo = new FileInfo(item.FileNames[i]);

                    if (fileItem.FullName.StartsWith(this.ProjectFolder.FullName,
                        StringComparison.OrdinalIgnoreCase))
                    {
                        fileItem.IsUnderProjectFolder = true;
                        fileItem.NeedCopy = true;
                    }

                    files.Add(fileItem);
                }
            }

            // Get the files of sub node under this node.
            foreach (ProjectItem subItem in item.ProjectItems)
            {
                GetProjectItem(subItem, files);
            }
        }      
    }
}
