/****************************** Module Header *************************************\
* Module Name:  CSVSServiceProviderPackage.cs
* Project:      CSVSService
* Copyright (c) Microsoft Corporation.
* 
* The CSVSService example demonstrates how to provide, register, consume 
* service in VSPackage. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/08/2009 6:02 PM Wesley Yao Created
\**********************************************************************************/

using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using Microsoft.Win32;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;

namespace VSX.CSVSService
{
    // This attribute tells the registration utility (regpkg.exe) that
    // this class needs to be registered as package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // A Visual Studio component can be registered under different regitry
    // roots; for instance when you debug your package you want to register
    // it in the experimental hive. This attribute specifies the registry
    // root to use if no one is provided to regpkg.exe with the /root switch.
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]
    // This attribute is used to register the informations needed to show
    // the this package in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration(false,
        "#110",
        "#112",
        "1.0",
        IconResourceID = 400)]
    // In order be loaded inside Visual Studio in a machine that has not the
    // VS SDK installed, package needs to have a valid load key (it can be
    // requested at http://msdn.microsoft.com/vstudio/extend/). This
    // attributes tells the shell that this package has a load key embedded
    // in its resources.
    [ProvideLoadKey("Standard", "1.0", "Service", "VSX", 1)]
    [Guid(GuidList.guidCSVSServiceProviderPackageString)]
    // Register the global service.
    [ProvideService(typeof(SCSGlobalService))]
    public sealed class CSVSServiceProviderPackage : Package
    {
        ////////////////////////////////////////////////////////////////////////////
        // Default constructor of the package.
        // Inside this method we adding the callback method to the service
        // container.
        //
        public CSVSServiceProviderPackage()
        {
            IServiceContainer serviceContainer = this as IServiceContainer;
            ServiceCreatorCallback callback
                = new ServiceCreatorCallback(CreateService);
            // We specify the promote parameter to "true" to promote this
            // request to any parent service containers, it makes the
            // SCSVSSGlobalService to be a global service.
            serviceContainer.AddService(typeof(SCSGlobalService), callback, true);
            serviceContainer.AddService(typeof(SCSLocalService), callback);
        }

        ////////////////////////////////////////////////////////////////////////////
        // Implementation of callback method, which would be invoked when
        // query for the services.
        //
        private object CreateService(IServiceContainer container, Type serviceType)
        {
            // We create either GlobalService or LocalService on demand.
            if (typeof(SCSGlobalService) == serviceType)
            {
                // Create a GlobalService object with this package as service
                // provider, then return it.
                return new GlobalService(this);
            }

            else if (typeof(SCSLocalService) == serviceType)
            {
                // Create a LocalService object with this package as service
                // provider, then return it.
                return new LocalService(this);
            }

            else
            {
                // Return null if the query doesn't match our services.
                return null;
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        //
        #region Package Members

        ////////////////////////////////////////////////////////////////////////////
        // Initialization of the package; this method is called right after the
        // package is sited, so this is the place where you can put all the
        // initilaization code that rely on services provided by VisualStudio.
        //
        protected override void Initialize()
        {
            Trace.WriteLine (string.Format(CultureInfo.CurrentCulture,
                "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();
        }
        #endregion
    }
}