/****************************** Module Header ******************************\
* Module Name:	WebRole.cs
* Project:		CSAzureTableStorageWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This is the entry point of the Web Role.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Linq;
using Microsoft.WindowsAzure.Diagnostics;
using Microsoft.WindowsAzure.ServiceRuntime;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;
using WebRole.Entities;
using System.Data.Services;

namespace WebRole
{
	public class WebRole : RoleEntryPoint
	{
		public override bool OnStart()
		{
			DiagnosticMonitor.Start("DiagnosticsConnectionString");

			// For information on handling configuration changes
			// see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
			RoleEnvironment.Changing += RoleEnvironmentChanging;

			// This code sets up a handler to update CloudStorageAccount instances when their corresponding
			// configuration settings change in the service configuration file.
			CloudStorageAccount.SetConfigurationSettingPublisher((configName, configSetter) =>
			{
				// Provide the configSetter with the initial value
				configSetter(RoleEnvironment.GetConfigurationSettingValue(configName));

				RoleEnvironment.Changed += (anotherSender, arg) =>
				{
					if (arg.Changes.OfType<RoleEnvironmentConfigurationSettingChange>()
						.Any((change) => (change.ConfigurationSettingName == configName)))
					{
						// The corresponding configuration setting has changed, propagate the value
						if (!configSetter(RoleEnvironment.GetConfigurationSettingValue(configName)))
						{
							// In this case, the change to the storage account credentials in the
							// service configuration is significant enough that the role needs to be
							// recycled in order to use the latest settings. (for example, the 
							// endpoint has changed)
							RoleEnvironment.RequestRecycle();
						}
					}
				};
			});

			var account = CloudStorageAccount.FromConfigurationSetting("DataConnectionString");

			// Create the table and add some dummy data.
			CloudTableClient tableClient = new CloudTableClient(account.TableEndpoint.AbsoluteUri, account.Credentials);
			tableClient.CreateTableIfNotExist("Person");
			PersonTableStorageContext ctx = new PersonTableStorageContext();
			Person person1 = new Person("DefaultPartition", "Row1") { Name = "Ared", Age = 24 };
			Person person2 = new Person("DefaultPartition", "Row2") { Name = "Lante", Age = 24 };
			Person person3 = new Person("DefaultPartition", "Row3") { Name = "Bright", Age = 24 };
			this.InsertEntityIfNotExist(ctx, person1);
			this.InsertEntityIfNotExist(ctx, person2);
			this.InsertEntityIfNotExist(ctx, person3);

			return base.OnStart();
		}

		/// <summary>
		/// Used to insert dummy data. Do not use in production code.
		/// </summary>
		private void InsertEntityIfNotExist(PersonTableStorageContext ctx, Person entity)
		{
			// If an entity already exists and the insertion fails, just catch the exception and go on...
			try
			{
				ctx.AddObject(PersonTableStorageContext.TableName, entity);
				ctx.SaveChanges();
			}
			catch
			{
			}
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
