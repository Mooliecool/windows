using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using CSVSXProjectSubType.ProjectFlavor;
using Microsoft.VisualStudio.Shell;

namespace CSVSXProjectSubType
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
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // Register the PropertyPage.
    [ProvideObject(
        typeof(CustomPropertyPage),
        RegisterUsing = RegistrationMethod.CodeBase)]
    // Register the project (note that we do not specify the extension as we use the
    // same one as the base project).
    [ProvideProjectFactory(typeof(CustomPropertyPageProjectFactory),
        "Task Project",
        null,
        null,
        null,
        @"..\Templates\Projects")]
    [Guid(GuidList.guidCSVSXProjectSubTypePkgString)]
    public sealed class VSXProjectSubTypePackage : Package
    {
        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public VSXProjectSubTypePackage()
        {
            Trace.WriteLine(
                string.Format(
                "Entering constructor for: {0}",
                this.ToString()));
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
            Trace.WriteLine(string.Format("Entering Initialize() of: {0}",
                this.ToString()));
            base.Initialize();
            this.RegisterProjectFactory(new CustomPropertyPageProjectFactory(this));
        }
        #endregion

        /// <summary>
        /// Allow a component such as project, factory, toolwindow,... to
        /// get access to VS services.
        /// </summary>
        internal object GetVsService(Type type)
        {
            return this.GetService(type);
        }

    }
}
