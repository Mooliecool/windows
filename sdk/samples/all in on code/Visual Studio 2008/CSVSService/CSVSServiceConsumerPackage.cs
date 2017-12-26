/****************************** Module Header *************************************\
* Module Name:  CSVSServiceConsumerPackage.cs
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
    [PackageRegistration(UseManagedResourcesOnly = true)]
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]
    [Guid(GuidList.guidCSVSServiceConsumerPackageString)]
    // This attribute is needed to let the shell know that this package exposes
    // some menus.
    [ProvideMenuResource(1000, 1)]
    public sealed class CSVSServiceConsumerPackage : Package
    {
        ////////////////////////////////////////////////////////////////////////////
        // Default constructor of the package.
        //
        public CSVSServiceConsumerPackage()
        {
        }

        ////////////////////////////////////////////////////////////////////////////
        // This method is the event handler for the command defined by this package,
        // it will call the method of the global service that will get a reference
        // to local service, then call the method of the local service.
        //
        private void LocalServiceCallback(object sender, EventArgs e)
        {
            // Get a reference of the global service.
            ICSGlobalService globalService
                = GetService(typeof(SCSGlobalService)) as ICSGlobalService;
            if (null != globalService)
            {
                // Call the method of global service that call into the local
                // service.
                globalService.CallLocalServiceMethod();
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // This method is the event handler for the command defined by this package
        // and is the consumer of the service exposed by the
        // CSVSServiceProviderPackage package.
        //
        private void GlobalServiceCallback(object sender, EventArgs e)
        {
            // Get a reference of the global service.
            ICSGlobalService globalService
                = GetService(typeof(SCSGlobalService)) as ICSGlobalService;
            if (null != globalService)
            {
                // Call the method exposed by the global service.
                globalService.GlobalServiceMethod();
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
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture,
                "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add our command handlers for menu (commands must exist in the .vsct
            // file)
            OleMenuCommandService mcs
                = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if (null != mcs)
            {
                // Create the command for the menu item.
                CommandID callLocalServiceCommandID
                    = new CommandID(GuidList.guidCSVSServiceCmdSet,
                        (int)PkgCmdIDList.cmdidCallLocalService);
                MenuCommand callLocalServiceItem
                    = new MenuCommand(LocalServiceCallback,
                        callLocalServiceCommandID);
                mcs.AddCommand(callLocalServiceItem);

                CommandID callGlobalServiceCommandID
                    = new CommandID(GuidList.guidCSVSServiceCmdSet,
                        (int)PkgCmdIDList.cmdidCallGlobalService);
                MenuCommand callGlobalServiceItem
                    = new MenuCommand(GlobalServiceCallback,
                        callGlobalServiceCommandID);
                mcs.AddCommand(callGlobalServiceItem);
            }
        }
        #endregion
    }
}