/****************************** Module Header ******************************\
* Module Name:	WorkerRole.cs
* Project:		CSWorkerRoleHostingWCF
* Copyright (c) Microsoft Corporation.
* 
* This Worker Role hosts a WCF service that exposes two tcp endpoints. One is
* for metadata. Another is for MyService.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using Microsoft.WindowsAzure.Diagnostics;
using Microsoft.WindowsAzure.ServiceRuntime;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;

namespace WorkerRoleHostingWCF
{
    public class WorkerRole : RoleEntryPoint
    {
        public override void Run()
        {
            // This is a sample worker implementation. Replace with your logic.
            // Trace.WriteLine("WorkerRoleHostingWCF entry point called", "Information");

            // Main logic

            using (ServiceHost host = new ServiceHost(typeof(MyService)))
            {

                string ip = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints["tcpinput"].IPEndpoint.Address.ToString();
                int tcpport = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints["tcpinput"].IPEndpoint.Port;
                int mexport = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints["mexinput"].IPEndpoint.Port;

                // Add a metadatabehavior for client proxy generation
                // The metadata is exposed via net.tcp
                ServiceMetadataBehavior metadatabehavior = new ServiceMetadataBehavior();
                host.Description.Behaviors.Add(metadatabehavior);
                Binding mexBinding = MetadataExchangeBindings.CreateMexTcpBinding();
                string mexlistenurl = string.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", ip, mexport);
                string mexendpointurl = string.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", RoleEnvironment.GetConfigurationSettingValue("Domain"), 8001);
                host.AddServiceEndpoint(typeof(IMetadataExchange), mexBinding, mexendpointurl, new Uri(mexlistenurl));

                // Add the endpoint for MyService
                string listenurl = string.Format("net.tcp://{0}:{1}/MyServiceEndpoint", ip, tcpport);
                string endpointurl = string.Format("net.tcp://{0}:{1}/MyServiceEndpoint", RoleEnvironment.GetConfigurationSettingValue("Domain"), 9001);
                host.AddServiceEndpoint(typeof(IMyService), new NetTcpBinding(SecurityMode.None), endpointurl, new Uri(listenurl));
                host.Open();

                while (true)
                {
                    Thread.Sleep(1800000);
                    //Trace.WriteLine("Working", "Information");
                }
            }
        }

        public override bool OnStart()
        {
            // Set the maximum number of concurrent connections 
            ServicePointManager.DefaultConnectionLimit = 12;
            // To avoid potential errors diagnostic is disabled.
            // DiagnosticMonitor.Start("DiagnosticsConnectionString");

            // For information on handling configuration changes
            // see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
            RoleEnvironment.Changing += RoleEnvironmentChanging;

            return base.OnStart();
        }

        private void RoleEnvironmentChanging(object sender, RoleEnvironmentChangingEventArgs e)
        {
            // If a configuration setting is changing
            if (e.Changes.Any(change => change is RoleEnvironmentConfigurationSettingChange))
            {
                // Set e.Cancel to true to restart this role instance
                e.Cancel = true;
            }
        }
    }
}
