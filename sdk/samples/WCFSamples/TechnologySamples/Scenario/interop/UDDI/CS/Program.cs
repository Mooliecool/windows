#region Copyright Notice
// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

#endregion

using System;
using System.IO;
using System.Configuration;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{

    class Program
    {
        static string username;
        static string password;
                
        static void Main(string[] args)
        {
            GetCredentials();

            string sampleBusinessName = "My WCF Sample Business";
            string sampleBusinessDescription = "My Sample Business published using WCF";

            UddiClient client = new UddiClient();
        
            try
            {
                Console.WriteLine("Logging in as user " + username + "...");
                client.Login(username, password);

                Console.WriteLine("Publish " + sampleBusinessName + "...");
                //Check if "My Business" was already published by me and delete it...
                businessInfo bInfo = client.GetMyBusinessByName(sampleBusinessName);
                if (bInfo != null)
                {
                    client.DeleteMyBusiness(bInfo.businessKey);
                }
                //Publish My Business
                string businessKey = "";
                
                businessKey = client.PublishBusiness(sampleBusinessName, sampleBusinessDescription);                

                businessInfo[] bInfos = null;

                Console.WriteLine("Query for all businesses published by me....");
                bInfos = client.GetMyBusinesses();
                if (bInfos != null)
                {
                    Console.WriteLine("The following businesses are published by me:");
                    foreach (businessInfo info in bInfos)
                    {
                        client.PrintBusinessInfo(info);
                    }
                }

                Console.WriteLine("Query businesses by name '" + sampleBusinessName + "' in the registry...");
                bInfos = client.GetBusinessByName(sampleBusinessName);
                if (bInfos != null)
                {
                    Console.WriteLine("The following businesses are published under the name '" + sampleBusinessName + "':\n");
                    foreach (businessInfo info in bInfos)
                    {
                        client.PrintBusinessInfo(info);
                    }
                }

                if (businessKey != string.Empty)
                {
                    Console.WriteLine("Deleting business \"" + sampleBusinessName + "\" created earlier");
                    client.DeleteMyBusiness(businessKey);
                }
            }
            catch (FaultException fault)
            {
                string faultReason = fault.Reason.Translations[0].Text;
                Console.WriteLine("Received Fault: " + faultReason);
                if (faultReason.Contains("Publisher limit") == true)
                {
                    Console.WriteLine("You have a limited number of businesses you can publish.\n" +
                                      "Use GetMyBusinesses() to query your published businesses \n" +
                                      " and DeleteMyBusiness(businessKey) to delete them.");
                }
                Console.WriteLine();
            }

            //Closing the clients gracefully closes the connection and cleans up resources
            client.Close();

            Console.WriteLine("Press <ENTER> to exit.");
            Console.ReadLine();
        }

        private static void GetCredentials()
        {
            username = ConfigurationManager.AppSettings["username"];
            password = "";

            Console.WriteLine("Please enter username and password to log in to UDDI registry");
            Console.Write("Username[" + username + "]:");
            string input = Console.ReadLine();
            if (input != string.Empty)
            {
                username = input;
            }
            Console.Write("Password:");
            ConsoleKeyInfo key = Console.ReadKey(true);
            while (key.Key != ConsoleKey.Enter)
            {
                Console.Write("*");
                password += key.KeyChar;
                key = Console.ReadKey(true);
            }
            Console.WriteLine();

        }                

    }
}
