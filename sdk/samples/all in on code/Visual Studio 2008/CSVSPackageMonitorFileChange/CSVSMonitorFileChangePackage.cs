/************************************* Module Header **************************************\
* Module Name:  CSVSMonitorFileChangePackage.cs
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
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using Microsoft.Win32;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio;
using System.Windows.Forms;

namespace CSVSPackageMonitorFileChange
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    ///
    /// The minimum requirement for a class to be considered a valid package for Visual Studio
    /// is to implement the IVsPackage interface and register itself with the shell.
    /// This package uses the helper classes defined inside the Managed Package Framework (MPF)
    /// to do it: it derives from the Package class that provides the implementation of the 
    /// IVsPackage interface and uses the registration attributes defined in the framework to 
    /// register itself and its components with the shell.
    /// </summary>
    // This attribute tells the registration utility (regpkg.exe) that this class needs
    // to be registered as package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // A Visual Studio component can be registered under different regitry roots; for instance
    // when you debug your package you want to register it in the experimental hive. This
    // attribute specifies the registry root to use if no one is provided to regpkg.exe with
    // the /root switch.
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]
    // This attribute is used to register the informations needed to show the this package
    // in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration(false, "#110", "#112", "1.0", IconResourceID = 400)]
    // In order be loaded inside Visual Studio in a machine that has not the VS SDK installed, 
    // package needs to have a valid load key (it can be requested at 
    // http://msdn.microsoft.com/vstudio/extend/). This attributes tells the shell that this 
    // package has a load key embedded in its resources.
    [ProvideLoadKey("Standard", "1.0", "CSVSMonitorFileChange", "Microsoft", 1)]
    [Guid(GuidList.guidCSVSMonitorFileChangePkgString)]
    [ProvideAutoLoad("{adfc4e64-0397-11d1-9f4e-00a0c911004f}")]
    public sealed class CSVSMonitorFileChangePackage : Package
    {
        CSVSMonitorFileChange monitor;

        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public CSVSMonitorFileChangePackage()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", this.ToString()));
        }



        /////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initilaization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            IVsFileChangeEx fileChangeService =
                GetService(typeof(SVsFileChangeEx)) as IVsFileChangeEx;
            monitor = new CSVSMonitorFileChange();
            uint cookie;

            // Enables a client to receive notifications of changes to a directory.
            fileChangeService.AdviseDirChange(

                // String form of the moniker identifier of 
                // the directory in the project system.
                Environment.GetFolderPath(
                    Environment.SpecialFolder.Desktop),

                // If true, then events should also be fired 
                // for changes to sub directories. If false, 
                // then events should not be fired for changes 
                // to sub directories.
                Convert.ToInt32(true),

                // IVsFileChangeEvents Interface on the object 
                // requesting notification of file change events.
                monitor,

                // Unique identifier for the file that is 
                // associated with the event sink.
                out cookie
            );

        }
        #endregion

    }


}