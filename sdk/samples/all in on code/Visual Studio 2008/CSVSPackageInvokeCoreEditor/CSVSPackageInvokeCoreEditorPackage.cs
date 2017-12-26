/************************************* Module Header **************************************\
* Module Name:  CSVSPackageInvokeCoreEditorPackage.cs
* Project:      CSVSPackageInvokeCoreEditorPackage
* Copyright (c) Microsoft Corporation.
* 
* The Visual Studio core editor is the default editor of Visual Studio. 
* The editor supports text-editing functions such as insert, delete, 
* copy, and paste. Its functionality combines with that provided by the 
* language that it is currently editing, such as text colorization, 
* indentation, and IntelliSense statement completion.
*
* This sample demostrates the basic operations on Core Editor, which
* includes:
* 1. Initiate core editor, include IVsTextBuffer and IVsCodeWindow
* 2. Associating core editor with file extension: .aio
* 3. Providing an options page in Tools / Options to let user to choose
* languages (VB, CS and XML) in the core editor.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/19/2009 12:00PM Hongye Sun Created
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

namespace AllInOne.CSVSPackageInvokeCoreEditor
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
    [ProvideLoadKey("Standard", "1.0", "VSPackageInvokeCoreEditor", "All-In-One", 1)]
    [Guid(GuidList.guidCSVSPackageInvokeCoreEditorPkgString)]

    // Register EditorFactory with extension .aio, this enables VS to trigger editor
    // creation when the file is opened
    [ProvideEditorExtension(typeof(EditorFactory),
          ".aio", 32, NameResourceID = 101)]

    // The options page which let user to select language associating with editor
    [ProvideOptionPage(typeof(LanguageServiceOptionsPage),
        "CSVSPackageInvokeCoreEditor Options", "Language Service Settings", 0, 0, true)] 
    public sealed class CSVSPackageInvokeCoreEditorPackage : Package
    {
        private EditorFactory editorFactory;

        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public CSVSPackageInvokeCoreEditorPackage()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", 
                this.ToString()));
        }

        public LanguageServiceOptionsPage GetLanguageServiceOptions()
        {
            return this.GetDialogPage(typeof(LanguageServiceOptionsPage)) 
                as LanguageServiceOptionsPage;
        }



        /////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, 
        /// so this is the place
        /// where you can put all the initilaization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Trace.WriteLine (string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", 
                this.ToString()));
            base.Initialize();

            // Register the editor factory when the package is initializing
            this.editorFactory = new EditorFactory(this);
            base.RegisterEditorFactory(this.editorFactory);

        }
        #endregion

    }
}