/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSIIS7AdminMWA
* Copyright (c) Microsoft Corporation.
* 
* The Microsoft.Web.Administration namespace contains classes that a developer 
* can use to administer IIS Manager. With the classes in this namespace, an 
* administrator can read and write configuration information to ApplicationHost.
* config, Web.config, and Administration.config files.
*
* The classes in the Microsoft.Web.Administration namespace contain a series of 
* convenient top-level objects that allow the developer to perform 
* administrative tasks. The different logical objects available include sites,
* applications, application pools, application domains, virtual directories,
* and worker processes. You can use the API to obtain and work with the 
* configuration and state of these objects and to perform such actions as 
* creating a site, starting or stopping a site, deleting an application pool, 
* recycling an application pool, and even unloading application domains.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/23/2009 8:50 PM Andrew Zhu Created
\***************************************************************************/
using System;
using Microsoft.Web.Administration;
using System.IO;

public class Program
{
    private static string siteName = "MySite";
    private static string appName = "/App1";
    private static string vdName = "/VDir1";
    private static string appPoolName = "MyAppPool";
    private static string physicalAddress = "d:\\MyApp";

    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Create a ServerManager instance: iisManager
        //

        ServerManager iisServerManager = new ServerManager();


        /////////////////////////////////////////////////////////////////////
        // Make sure the directory "d:\\MyApp" existed. 
        //

        if (!Directory.Exists(physicalAddress))
            Directory.CreateDirectory(physicalAddress);


        /////////////////////////////////////////////////////////////////////
        // Reset environment
        //

        // Delete web site
        if (iisServerManager.Sites[siteName] != null)
        {
            iisServerManager.Sites.Remove(iisServerManager.Sites[siteName]);
        }

        // Delete application pool
        if (iisServerManager.ApplicationPools[appPoolName] != null)
        {
            iisServerManager.ApplicationPools.Remove(iisServerManager.
                ApplicationPools[appPoolName]);
        }

        iisServerManager.CommitChanges();


        /////////////////////////////////////////////////////////////////////
        // Create a IIS7 Site 
        //

        Console.WriteLine("Press 'Enter' to create a new web site in IIS7," +
            "and check the new created site in IIS7 Manager Console");
        Console.ReadLine();
        Site site = iisServerManager.Sites.Add(siteName, physicalAddress, 8080);
        iisServerManager.CommitChanges();


        /////////////////////////////////////////////////////////////////////
        // Create a IIS7 Appilication 
        //

        Console.WriteLine("Press 'Enter' to create a new Application " + 
            "and check the new created application in IIS7 Manager Console");
        Console.ReadLine();
        iisServerManager.Sites[siteName].Applications.Add(appName, physicalAddress);
        iisServerManager.CommitChanges();


        /////////////////////////////////////////////////////////////////////
        // Create a IIS7 Virtual Directory 
        //

        Console.WriteLine("Press 'Enter' to create a new Virtual Directory," + 
            " and check the new created Virtual Directory in IIS7 Manager Console");
        Console.ReadLine();
        iisServerManager.Sites[siteName].Applications[appName].
            VirtualDirectories.Add(vdName, physicalAddress);
        iisServerManager.CommitChanges();


        /////////////////////////////////////////////////////////////////////
        // Create a app pool and set the new create site's app pool to this one. 
        //

        Console.WriteLine("Press 'Enter' to create a new Application Pool" +
            " and set the new created site'a app pool to this one , and" +
            " check the new created Application Pool in IIS7 Manager Console");
        Console.ReadLine();
        iisServerManager.ApplicationPools.Add(appPoolName);
        iisServerManager.Sites[siteName].Applications[appName].
            ApplicationPoolName = appPoolName;
        ApplicationPool appPool = iisServerManager.ApplicationPools[appPoolName];
        appPool.ManagedPipelineMode = ManagedPipelineMode.Integrated;
        iisServerManager.CommitChanges();


        ///////////////////////////////////////////////////////////////////////
        // Reset environment
        //

        // Delete web site
        if (iisServerManager.Sites[siteName] != null)
        {
            iisServerManager.Sites.Remove(iisServerManager.Sites[siteName]);
        }

        // Delete application pool
        if (iisServerManager.ApplicationPools[appPoolName] != null)
        {
            iisServerManager.ApplicationPools.Remove(iisServerManager.
                ApplicationPools[appPoolName]);
        }

        iisServerManager.CommitChanges();
    }
}

