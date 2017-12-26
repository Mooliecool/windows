/*********************************** Module Header ***********************************\
* Module Name:  ServiceInstaller.cs
* Project:      CSTriggerStartWindowsService
* Copyright (c) Microsoft Corporation.
* 
* In ServiceInstaller, we configure the service to start when a generic USB disk 
* becomes available. It also shows how to trigger-start when the first IP address 
* becomes available, and trigger-stop when the last IP address becomes unavailable. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.ServiceProcess;
using System.Runtime.InteropServices;
#endregion


namespace CSTriggerStartWindowsService
{
    [RunInstaller(true)]
    public partial class ProjectInstaller : System.Configuration.Install.Installer
    {
        public ProjectInstaller()
        {
            InitializeComponent();
        }


        private void serviceInstaller1_AfterInstall(object sender, InstallEventArgs e)
        {
            // If Service Trigger Start is supported on the current system, configure 
            // the service to trigger start.
            if (ServiceTriggerStart.IsSupported)
            {
                Console.WriteLine("Configuring trigger-start service...");

                try
                {
                    // Set the service to trigger-start when a generic USB disk becomes 
                    // available.
                    ServiceTriggerStart.SetServiceTriggerStartOnUSBArrival(
                        this.serviceInstaller1.ServiceName);

                    // [-or-]

                    // Set the service to trigger-start when the first IP address 
                    // becomes available, and trigger-stop when the last IP address 
                    // becomes unavailable.
                    //ServiceTriggerStart.SetServiceTriggerStartOnIPAddressArrival(
                    //    this.serviceInstaller1.ServiceName);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Service Trigger Start configuration failed with " +
                        ex.Message);
                }
            }
            else
            {
                Console.WriteLine("The current system does not support trigger-start service.");
            }
        }
    }
}
