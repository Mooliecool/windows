/************************************* Module Header **************************************\
* Module Name:  CSVSMonitorFileChange.cs
* Project:      CSVSPackageMonitorFileChange
* Copyright (c) Microsoft Corporation.
* 
* Visual Studio provides SVsFileChangeEx service enables arbitrary components 
* to register to be notified when a file is modified outside of the Environment.
*
* This service is useful when you are performing some operation which will be 
* interupted by file changes from outside environment.
*
* The service is similar with FileSystemWatcher Class.
* http://msdn.microsoft.com/en-us/library/system.io.filesystemwatcher.aspx
*
* To use this sample, follow the steps as below:
*
* 1. Start experimental VS instance
* 2. The package will be autoloaded automatically when there is no any solution 
* loaded.
* 3. The demo monitors user's desktop directory, so please do file or directory 
* changes in desktop.
* 4. Visual Studio will popup window whenever a change is made.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 1/6/2010 5:25PM Hongye Sun Created
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Shell.Interop;
using System.Windows.Forms;
using Microsoft.VisualStudio;

namespace CSVSPackageMonitorFileChange
{
    public class CSVSMonitorFileChange : IVsFileChangeEvents
    {
        bool dialogDisplayed = false;

        #region IVsFileChangeEvents Members

        /// <summary>
        /// Notifies clients of changes made to a directory.
        /// </summary>
        /// <param name="pszDirectory">
        /// Name of the directory that had a change.
        /// </param>
        /// <returns>
        /// If the method succeeds, it returns S_OK. 
        /// If it fails, it returns an error code.
        /// </returns>
        public int DirectoryChanged(string pszDirectory)
        {
            if (!dialogDisplayed)
            {
                dialogDisplayed = true;
                MessageBox.Show("Directory Change: " + pszDirectory);
                dialogDisplayed = false;
            }

            return VSConstants.S_OK;
        }

        /// <summary>
        /// Notifies clients of changes made to one or more files.
        /// </summary>
        /// <param name="cChanges">
        /// Number of files changed.
        /// </param>
        /// <param name="rgpszFile">
        /// Array of file names.
        /// </param>
        /// <param name="rggrfChange">
        /// Array of flags indicating the type of changes. See _VSFILECHANGEFLAGS.
        /// For detail about _VSFILECHANGEFLAGS, please visit:
        /// http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop._vsfilechangeflags.aspx
        /// </param>
        /// <returns>
        /// If the method succeeds, it returns S_OK. 
        /// If it fails, it returns an error code.
        /// </returns>
        public int FilesChanged(uint cChanges, string[] rgpszFile, uint[] rggrfChange)
        {
            if (!dialogDisplayed)
            {
                dialogDisplayed = true;
                MessageBox.Show("File Change: " + string.Join(";", rgpszFile));
                dialogDisplayed = false;
            }
            return VSConstants.S_OK; ;
        }

        #endregion
    }
}
