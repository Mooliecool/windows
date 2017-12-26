/****************************** Module Header ******************************\
* Module Name:  CSVSPackageAddReferenceTabPackage.cs
* Project:      CSVSPackageAddReferenceTab
* Copyright (c) Microsoft Corporation.
* 
* Visual Studio supports to extend the Add Reference dialog and add custom
* tab page into the dialog.
*
* This sample demostrate you how to add a custom .NET user control as a tab 
* page into the add reference dialog, and how to enable select button and
*  handle item selection events.
*
* All the sample code is based on MPF.
*
* The sample is initiated by the thread on the forum:
* http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/ddb0f935-b8ac-400d-9e3d-64d74be85031
* 
* History:
* * 1/12/2010 1:00 AM Hongye Sun Created
\***************************************************************************/

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

namespace Microsoft.CSVSPackageAddReferenceTab
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
    [ProvideLoadKey("Standard", "1.0", "CSVSPackageAddReferenceTab", "Microsoft", 1)]
    [Guid(GuidList.guidCSVSPackageAddReferenceTabPkgString)]

    // Write registry information to let Visual Studio load the tab page when Add Reference dialog is popped up.
    [ProvideReferencePageAttribute(typeof(ReferencePageDialog), typeof(CSVSPackageAddReferenceTabPackage), "Reference Tab")]
    public sealed class CSVSPackageAddReferenceTabPackage : Package, IVsComponentSelectorProvider
    {
        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public CSVSPackageAddReferenceTabPackage()
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
            Trace.WriteLine (string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

        }
        #endregion
        
        #region IVsComponentSelectorProvider Members

        public int GetComponentSelectorPage(ref Guid rguidPage, VSPROPSHEETPAGE[] ppage)
        {
            if (ppage == null || rguidPage != GuidList.guidReferencePage)
            {
                return VSConstants.E_INVALIDARG;
            }
            else
            {
                ppage[0].dwSize = (uint)Marshal.SizeOf(typeof(VSPROPSHEETPAGE));
                ppage[0].hwndDlg = ReferencePageDialog.DialogPointer;

                ppage[0].dwFlags = 0;
                ppage[0].HINSTANCE = 0;
                ppage[0].dwTemplateSize = 0;
                ppage[0].pTemplate = IntPtr.Zero;
                ppage[0].pfnDlgProc = IntPtr.Zero;
                ppage[0].lParam = IntPtr.Zero;
                ppage[0].pfnCallback = IntPtr.Zero;
                ppage[0].pcRefParent = IntPtr.Zero;
                ppage[0].dwReserved = 0;
                ppage[0].wTemplateId = (ushort)0;
                return VSConstants.S_OK;
            }
        }

        #endregion
    }
}