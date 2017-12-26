/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSIISAdminADSI
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to use Active Directory Service Interfaces (ADSI) 
* to configure IIS by using .Net System.DirectoryServices namespace to access IIS 
* ADSI Provider. The classes, methods, and properties of the IIS ADSI provider can
* be used to configure IIS from scripts or executables. 
* 
* Active Directory Service Interfaces (ADSI) is a directory service model and a 
* set of Component Object Model (COM) interfaces. ADSI enables Windows applications
* and Active Directory clients to access several network directory services. The
* IIS ADSI provider, like the IIS WMI provider provides a standard syntax for 
* accessing IIS configuration data.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 1/12/2010 3:45 PM WenJun Zhang Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.DirectoryServices;

namespace CSIISAdminADSI
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Press any key to create the new web site.");
            Console.ReadLine();

            //Connect to IIS ADSI Provider                    
            string ServerName = "LocalHost";            
            DirectoryEntry W3SVC = 
                new DirectoryEntry("IIS://" + ServerName + "/w3svc");    
                        
            // Create new web site.            
            object siteObj;
            string siteID = "";
            string siteName = "IISADSIDemo";
            string hostHeader = "www.mytestdomain.com";            
            string serverBinding1 = ":8181:";
            string serverBinding2 = ":8282:" + hostHeader;
            string rootPath = "C:\\inetpub\\IISADSIDemo";
            object[] siteProperty = 
                new object[] { 
                    siteName, 
                    new object[] { serverBinding1, serverBinding2 }, 
                    rootPath 
                };                           
            
            try {
                siteObj = (object)W3SVC.Invoke("CreateNewSite", siteProperty);
                siteID = siteObj.ToString();
                W3SVC.CommitChanges();            
            }
            catch (Exception ex)
            {
                Console.WriteLine(
                    "Fail to create web site with the following exception: \n{0}",
                    ex.Message);
            }

            ///////////////////////////////////////////////////////////////////////
            // Wait for a while to allow ADSI finish the above operations
            // and then start the web site.
            Console.WriteLine("Web site created. Starting IISADSIDemo...");
            System.Threading.Thread.Sleep(1000);

            DirectoryEntry newSite = 
                new DirectoryEntry("IIS://" + ServerName + "/W3SVC/" + siteID);
            newSite.Invoke("Start");

            Console.WriteLine("Started \n");

            Console.WriteLine("Create new virtual directory.");

            // Create new virtual directory.
            string metabasePath = string.Format("/W3SVC/{0}/Root",siteID);
            DirectoryEntry site = 
                new DirectoryEntry("IIS://" + ServerName + metabasePath);
            DirectoryEntries vdirs = site.Children;
            DirectoryEntry newVDir = vdirs.Add("vdir1", "IIsWebVirtualDir");

            newVDir.Properties["Path"][0] = "C:\\inetpub\\IISADSIDemo\\vdir1";
            newVDir.Properties["AccessScript"][0] = true;
            newVDir.Properties["AppFriendlyName"][0] = "vdir1";
            newVDir.Properties["AppIsolated"][0] = "1";
            newVDir.Properties["AppRoot"][0] = "/LM" + metabasePath + "/vdir1";

            newVDir.CommitChanges();

            Console.WriteLine("vdir1 created. \n");

            // List all web sites on the server.         
            Console.WriteLine("List all web sites on server: \n");
            try
            {
                foreach (DirectoryEntry Site in W3SVC.Children)
                {
                    if (Site.SchemaClassName == "IIsWebServer")
                        Console.WriteLine(Site.Name + " - " + 
                            Site.Properties["ServerComment"].Value.ToString());
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(
                    "Fail to list web sites with the following exception: \n{0}: " +
                    e.ToString());
            }

            Console.WriteLine("\nEnd.");
            Console.Read();
        }
    }
}



