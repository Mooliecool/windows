/************************************* Module Header *********************************\
 * Module Name:        ProjectFileItem.cs
 * Project :           CSVSXSaveProject
 * Copyright (c)       Microsoft Corporation
 * 
 * Get the project files information. the flag of both IsUnderProjectFolder and
 * Included are used to set the options about selected copy files item.
 * 
 * The source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

using System.IO;

namespace Microsoft.CSVSXSaveProject.Files
{
    public class ProjectFileItem
    {
        /// <summary>
        /// The file information.
        /// </summary>
        public FileInfo Fileinfo { get; set; }

        /// <summary>
        /// File name.
        /// </summary>
        public string FileName 
        {
            get
            {
                return Fileinfo.Name;
            }
        }

        /// <summary>
        /// Full path of the file.
        /// </summary>
        public string FullName
        {
            get
            {
                return Fileinfo.FullName;
            }
        }

        /// <summary>
        /// Specify whether the file is in project folder.
        /// </summary>
        public bool IsUnderProjectFolder { get; set; }

        /// <summary>
        /// Specify whether the file should be copied.
        /// </summary>
        public bool NeedCopy { get; set; }
    }
}
