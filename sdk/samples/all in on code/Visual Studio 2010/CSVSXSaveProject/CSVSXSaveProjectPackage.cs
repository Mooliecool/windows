/************************************* Module Header *********************************\
 * Module Name:        CSVSXSaveProjectPackage.cs
 * Project :           CSVSXSaveProject
 * Copyright (c)       Microsoft Corporation
 *
 * This package adds menus to IDE, which supply following features 
 * 1. Copy the whole project to a new location.
 * 2. Open the new project in the current IDE.
 * 
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
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using EnvDTE;
using Microsoft.VisualStudio.Shell;

namespace Microsoft.CSVSXSaveProject
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    ///
    /// The minimum requirement for a class to be considered a valid package for Visual
    /// Studio is to implement the IVsPackage interface and register itself with the 
    /// shell. This package uses the helper classes defined inside the Managed Package 
    /// Framework (MPF) to do it: it derives from the Package class that provides the 
    /// implementation of the IVsPackage interface and uses the registration attributes
    /// defined in the framework to register itself and its components with the shell.
    /// </summary>
    // This attribute tells the PkgDef creation utility (CreatePkgDef.exe) that this
    // class is a package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // This attribute is used to register the information needed to show the this
    // package in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)]
    // This attribute is needed to let the shell know that this package exposes some 
    // menus.
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(GuidList.guidCSVSXSaveProjectPkgString)]
    public sealed class CSVSXSaveProjectPackage : Package
    {
        /// <summary>
        /// Specify a DTE Object in this application.
        /// </summary>
        DTE dte;
        internal DTE DTEObject
        {
            get
            {
                if (dte == null)
                {
                    dte = this.GetService(typeof(DTE)) as DTE;
                }
                return dte;
            }
        }

        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not 
        /// require any Visual Studio service because at this point the package object
        /// is created but not sited yet inside Visual Studio environment. The place 
        /// to do all the other initialization is the Initialize method.
        /// </summary>
        public CSVSXSaveProjectPackage()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture,
                "Entering constructor for: {0}", this.ToString()));
        }

        /////////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the
        /// package is sited, so this is the place where you can put all the
        /// initialization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture,
                "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add our command handlers for menu (commands must exist in the .vsct file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService))
                as OleMenuCommandService;
            if (null != mcs)
            {
                // Create the command for the menu item.
                CommandID menuCommandID =
                    new CommandID(GuidList.guidCSVSXSaveProjectCmdSet,
                        (int)PkgCmdIDList.cmdidCSVSXSaveProjectCommandID);
                MenuCommand menuItem =
                    new MenuCommand(MenuItemCallback, menuCommandID);
                mcs.AddCommand(menuItem);

                // Create the command for the VSXSaveProjectCmdSet menu item.
                CommandID cSVSXSaveProjectContextCommandID =
                    new CommandID(GuidList.guidCSVSXSaveProjectContextCmdSet,
                        (int)PkgCmdIDList.cmdidCSVSXSaveProjectContextCommandID);
                OleMenuCommand cSVSXSaveProjectMenuContextCommand =
                    new OleMenuCommand(MenuItemCallback,
                        cSVSXSaveProjectContextCommandID);
                mcs.AddCommand(cSVSXSaveProjectMenuContextCommand);
            }
        }
        #endregion

        /// <summary>
        /// This function is the callback used to execute a command when the a menu 
        /// item is clicked. See the Initialize method to see how the menu item is
        /// associated to this function using the OleMenuCommandService service and 
        /// the MenuCommand class.
        /// </summary>
        private void MenuItemCallback(object sender, EventArgs e)
        {
            try
            {
                // Get current active project object.
                var proj = this.GetActiveProject();

                if (proj != null)
                {
                    // Get the project information.
                    var vsProj = new Files.VSProject(proj);

                    // Get the files included in the project.
                    var includedFiles = vsProj.GetIncludedFiles();

                    // Get the files under the project folder.
                    var projfolderFiles =
                        Files.ProjectFolder.GetFilesInProjectFolder(proj.FullName);

                    // Add the other files such as documents under the project folder, so
                    // the user can choose them.
                    var totalItems = new List<Files.ProjectFileItem>(includedFiles);
                    foreach (Files.ProjectFileItem fileItem in projfolderFiles)
                    {
                        if (includedFiles.Count(f => f.FullName.Equals(fileItem.FullName,
                            StringComparison.OrdinalIgnoreCase)) == 0)
                        {
                            totalItems.Add(fileItem);
                        }
                    }

                    // Display the user interface.
                    using (SaveProjectDialog dialog = new SaveProjectDialog())
                    {
                        // Display the all the files.
                        dialog.FilesItems = totalItems;
                        dialog.OriginalFolderPath = vsProj.ProjectFolder.FullName;

                        var result = dialog.ShowDialog();

                        // Open the new project.
                        if (result == DialogResult.OK && dialog.OpenNewProject)
                        {
                            string newProjectPath = string.Format("{0}\\{1}",
                                dialog.NewFolderPath,
                                proj.FullName.Substring(vsProj.ProjectFolder.FullName.Length));

                            string cmd = string.Format("File.OpenProject \"{0}\"", newProjectPath);

                            this.DTEObject.ExecuteCommand(cmd);                      
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// Get the active project object.
        /// </summary>
        internal Project GetActiveProject()
        {
            Project activeProject = null;

            // Get all project in Solution Explorer.
            Array activeSolutionProjects =
                 this.DTEObject.ActiveSolutionProjects as Array;
            if (activeSolutionProjects != null && activeSolutionProjects.Length > 0)
            {
                // Get the active project.
                activeProject = activeSolutionProjects.GetValue(0) as Project;
            }
            return activeProject;
        }
    }
}

