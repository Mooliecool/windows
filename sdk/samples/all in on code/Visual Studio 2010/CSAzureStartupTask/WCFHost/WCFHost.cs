/****************************** Module Header ******************************\
Module Name:  WCFHost.cs
Project:      CSAzureStartupTask
Copyright (c) Microsoft Corporation.

A Windows Azure Worker role which hosts reverse string WCF service in
WaWorkerHost.exe

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.Threading;
using ReverseString;
using Microsoft.WindowsAzure.ServiceRuntime;

namespace WCFHost
{
	public class WCFHost : RoleEntryPoint
	{
		private ServiceHost serviceHost;
		
		private void StartService(Int32 retries)
		{
			if (retries == 0)
			{
				RoleEnvironment.RequestRecycle();
				return;
			}

			Uri httpUri = new Uri(String.Format("http://{0}/{1}",
									RoleEnvironment.CurrentRoleInstance.InstanceEndpoints["HttpIn"].IPEndpoint.ToString(),
									"ReverseString"));


			serviceHost = new ServiceHost(typeof(TestWCFService), httpUri);
			
			serviceHost.Faulted += (sender, e) =>
				{
					Trace.TraceError("Host fault occured. Aborting and restarting the host. Retry count: {0}", retries);

					serviceHost.Abort();
					StartService(--retries);
				};

			try
			{
				Trace.TraceInformation("Trying to open service host");
				serviceHost.Open();
				Trace.TraceInformation("Service host started successfully.");
				
			}
			catch (TimeoutException timeoutException)
			{
				Trace.TraceError("The service operation time out. {0}",
							timeoutException.Message);

			}
			catch (CommunicationException communicationException)
			{
				Trace.TraceError("Could not start service host. {0}",
					 communicationException.Message);
			}
		}

		private void StopService()
		{
			if (serviceHost != null)
			{
				try
				{
					serviceHost.Close();
				}
				catch (TimeoutException timeoutException)
				{
					Trace.TraceError("The service close time out. {0}",
								timeoutException.Message);

					serviceHost.Abort();
				}
				catch (CommunicationException communicationException)
				{
					Trace.TraceError("Could not close service host. {0}",
						 communicationException.Message);

					serviceHost.Abort();
				}
			}
		}


		public override void Run()
		{
			// This is a sample worker implementation. Replace with your logic.
			Trace.WriteLine("WorkerRole1 entry point called", "Information");

			while (true)
			{
				Thread.Sleep(10000);
				Trace.WriteLine("Working", "Information");
			}
		}

		public override bool OnStart()
		{
			// Set the maximum number of concurrent connections 
			ServicePointManager.DefaultConnectionLimit = 12;

			StartService(3);

			// For information on handling configuration changes
			// see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
			RoleEnvironment.Changing += RoleEnvironmentChanging;

			return base.OnStart();
		}

		public override void OnStop()
		{
			StopService();
			base.OnStop();
		}


		private void RoleEnvironmentChanging(object sender, RoleEnvironmentChangingEventArgs e)
		{
			if (e.Changes.Any(change => change is RoleEnvironmentConfigurationSettingChange))
			{
				e.Cancel = true;
			}
		}
	}
}
