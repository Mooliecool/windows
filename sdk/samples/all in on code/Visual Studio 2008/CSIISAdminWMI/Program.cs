/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSIISAdminWMI
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to use Windows Management Instrumentation(WMI) 
* to configure IIS by using .Net System.Management namespace to access IIS 
* WMI Provider. The classes, methods, and properties of the IIS WMI provider 
* can be used to configure IIS from scripts or executables. 
* 
* The IIS WMI provider, like the IIS ADSI provider, provides a standard syntax 
* for accessing IIS configuration data through the use of the IIS admin 
* objects. Any script or code that attempts to manage IIS using Windows 
* Management Instrumentation (WMI) needs to access the IIS WMI provider 
* objects. For example, if you want to change a Web site property in the 
* metabase, you must instantiate the class called IIsWebServerSetting which 
* is a child of the CIM_Setting class. The IIS WMI provider cannot be used 
* without certain methods of the Windows WMI classes. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/23/2009 5:50 PM WenJun Zhang Created
* * 10/24/2009 10:23 PM Jialiang Ge Reviewed
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Management;
using System.Threading;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Create a new web site.
        // 

        Console.WriteLine("Creating a new web site ...");

        // Connect to IIS WMI provider.
        ManagementScope managementScope = new ManagementScope(
            @"\\.\root\MicrosoftIISv2");
        managementScope.Connect();

        // Create a new server binding.
        ManagementClass serverBindingClass = new ManagementClass(
            managementScope, new ManagementPath("ServerBinding"), null);
        ManagementObject[] serverBindings = new ManagementObject[1];
        serverBindings[0] = serverBindingClass.CreateInstance();
        serverBindings[0].Properties["Hostname"].Value = "";
        serverBindings[0].Properties["IP"].Value = "";
        serverBindings[0].Properties["Port"].Value = "83";
        serverBindings[0].Put();

        // Get a W3SVC object.
        ManagementObject w3svc = new ManagementObject(managementScope,
            new ManagementPath(@"IIsWebService='W3SVC'"), null);

        // Set parameters of new web site.
        ManagementBaseObject siteParameters = w3svc.GetMethodParameters(
            "CreateNewSite");
        siteParameters["ServerComment"] = "IISWMIDemo";
        siteParameters["ServerBindings"] = serverBindings;
        siteParameters["PathOfRootVirtualDir"] = "C:\\inetpub\\IISWMIDemo";

        // Create the new web site.
        ManagementBaseObject ret = w3svc.InvokeMethod("CreateNewSite",
            siteParameters, null);

        // Retrieve metabase path and site ID.
        string sitePath = ret.Properties["ReturnValue"].Value.ToString();
        string siteID = sitePath.Replace("IIsWebServer='W3SVC/", "").
            Replace("'", "");

        // Set properties of the site.
        ManagementObject siteRoot = new ManagementObject(managementScope,
            new ManagementPath(
                string.Format(@"IIsWebVirtualDirSetting.Name='W3SVC/{0}/root'", siteID)
                ), null);
        siteRoot.Properties["AppFriendlyName"].Value = "IISWMIDemo";
        siteRoot.Properties["AccessRead"].Value = true;
        siteRoot.Properties["AuthFlags"].Value = 5;
        siteRoot.Properties["AccessScript"].Value = true;
        siteRoot.Properties["AuthAnonymous"].Value = true;
        siteRoot.Properties["AppPoolId"].Value = "DefaultAppPool";
        siteRoot.Put();

        // Wait for a while to allow WMI finish the above operations
        // and then start the web site.
        Thread.Sleep(1000);

        Console.WriteLine("The web site 'IISWMIDemo' is created.\n");


        /////////////////////////////////////////////////////////////////////
        // Start the web site.
        // 

        Console.WriteLine("Starting the web site ...");

        ManagementObject site = new ManagementObject(managementScope,
            new ManagementPath(sitePath), null);
        site.InvokeMethod("Start", null);

        Console.WriteLine("The web site 'IISWMIDemo' is started.\n");


        /////////////////////////////////////////////////////////////////////
        // Create a new virtual directory.
        // 

        Console.WriteLine("Creating a new virtual directory ...");

        // Create virtual directory
        string vdirPath = string.Format("W3SVC/{0}/Root/vdir1", siteID);

        ManagementClass virtualDirClass = new ManagementClass(managementScope,
            new ManagementPath("IIsWebVirtualDirSetting"), null);
        ManagementObject virtualDirObj = virtualDirClass.CreateInstance();

        virtualDirObj.Properties["Name"].Value = vdirPath;
        virtualDirObj.Properties["Path"].Value = "C:\\inetpub\\IISWMIDemo\\vdir1";
        virtualDirObj.Properties["AuthFlags"].Value = 5;
        virtualDirObj.Properties["EnableDefaultDoc"].Value = true;
        virtualDirObj.Properties["DirBrowseFlags"].Value = 0x4000003E;
        virtualDirObj.Properties["AccessFlags"].Value = 513;
        virtualDirObj.Put();

        // Create web application for this virtual directory
        ManagementObject virtualDir = new ManagementObject(managementScope,
            new System.Management.ManagementPath(
                String.Format("IIsWebVirtualDir='{0}'", vdirPath)), null);
        ManagementBaseObject appParameters = virtualDir.GetMethodParameters(
            "AppCreate2");
        appParameters["AppMode"] = 2;
        virtualDir.InvokeMethod("AppCreate2", appParameters, null);
        ManagementObject virtualDirSettings = new ManagementObject(
            managementScope, new System.Management.ManagementPath(
                String.Format("IIsWebVirtualDirSetting='{0}'", vdirPath)),
                null);
        virtualDirSettings.Properties["AppFriendlyName"].Value = "vdir1App";
        virtualDirSettings.Put();

        Console.WriteLine("The virtual directory 'vdir1' is created.\n");


        /////////////////////////////////////////////////////////////////////
        // List all web sites on the server.
        // 

        Console.WriteLine("List all web sites on server: \n");

        ObjectQuery queryObject = new ObjectQuery(
            "SELECT * FROM IISWebServerSetting");
        ManagementObjectSearcher searchObject = new ManagementObjectSearcher(
            managementScope, queryObject);
        foreach (ManagementObject result in searchObject.Get())
        {
            Console.WriteLine(
                result["ServerComment"] + " (" + result["Name"] + ")");
        }
    }
}