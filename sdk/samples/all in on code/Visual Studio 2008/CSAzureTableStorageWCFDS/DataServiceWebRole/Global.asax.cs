using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.SessionState;
using WebRole.Entities;
using Microsoft.WindowsAzure.StorageClient;
using Microsoft.WindowsAzure.ServiceRuntime;
using Microsoft.WindowsAzure;

namespace WebRole
{
    public class Global : System.Web.HttpApplication
    {

        protected void Application_Start(object sender, EventArgs e)
        {
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

        protected void Session_Start(object sender, EventArgs e)
        {

        }

        protected void Application_BeginRequest(object sender, EventArgs e)
        {

        }

        protected void Application_AuthenticateRequest(object sender, EventArgs e)
        {

        }

        protected void Application_Error(object sender, EventArgs e)
        {

        }

        protected void Session_End(object sender, EventArgs e)
        {

        }

        protected void Application_End(object sender, EventArgs e)
        {

        }
    }
}