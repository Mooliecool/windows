'***************************** Module Header ******************************\ 
'* Module Name: Program.cs 
'* Project: VBIISAdminWMI 
'* Copyright (c) Microsoft Corporation. 
'* 
'* This sample demonstrates how to use Windows Management Instrumentation (WMI) 
'* to configure IIS by using .Net System.Management namespace to access IIS WMI 
'* Provider. The classes, methods, and properties of the IIS WMI provider can be 
'* used to configure IIS from scripts or executables. 
'* 
'* The IIS WMI provider, like the IIS ADSI provider, provides a standard syntax 
'* for accessing IIS configuration data through the use of the IIS admin objects. 
'* Any script or code that attempts to manage IIS using Windows Management 
'* Instrumentation (WMI) needs to access the IIS WMI provider objects. For example, 
'* if you want to change a Web site property in the metabase, you must instantiate 
'* the class called IIsWebServerSetting which is a child of the CIM_Setting class. 
'* The IIS WMI provider cannot be used without certain methods of the Windows WMI 
'* classes. 
'* 
'* This source is subject to the Microsoft Public License. 
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL. 
'* All other rights reserved. 
'* 
'* History: 
'* * 12/24/2009 5:50 PM WenJun Zhang Created 
'************************************************************************** 


Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Management


Module VBIISAdminWMI

    Sub Main()

        Console.WriteLine("Press any key to create the new web site.")
        Console.ReadLine()

        ' Connect to IIS WMI provider. 
        Dim managementScope As New ManagementScope("\\.\root\MicrosoftIISv2")
        managementScope.Connect()

        ' Create new server binding. 
        Dim serverBindingClass As New ManagementClass(managementScope, New ManagementPath("ServerBinding"), Nothing)
        Dim serverBindings As ManagementObject() = New ManagementObject(0) {}
        serverBindings(0) = serverBindingClass.CreateInstance()
        serverBindings(0).Properties("Hostname").Value = ""
        serverBindings(0).Properties("IP").Value = ""
        serverBindings(0).Properties("Port").Value = "83"
        serverBindings(0).Put()

        ' Get W3SVC object. 
        Dim w3svc As New ManagementObject(managementScope, New ManagementPath("IIsWebService='W3SVC'"), Nothing)

        ' Set parameters of new web site. 
        Dim siteParameters As ManagementBaseObject = w3svc.GetMethodParameters("CreateNewSite")
        siteParameters("ServerComment") = "IISWMIDemo"
        siteParameters("ServerBindings") = serverBindings
        siteParameters("PathOfRootVirtualDir") = "C:\inetpub\IISWMIDemo"

        ' Create the new web site. 
        Dim ret As ManagementBaseObject = w3svc.InvokeMethod("CreateNewSite", siteParameters, Nothing)

        ' Retrieve metabase path and site ID. 
        Dim sitePath As String = Convert.ToString(ret.Properties("ReturnValue").Value)
        Dim siteID As String = sitePath.Replace("IIsWebServer='W3SVC/", "").Replace("'", "")

        ' Set properties. 
        Dim siteRoot As New ManagementObject(managementScope, New ManagementPath(String.Format("IIsWebVirtualDirSetting.Name='W3SVC/{0}/root'", siteID)), Nothing)
        siteRoot.Properties("AppFriendlyName").Value = "IISWMIDemo"
        siteRoot.Properties("AccessRead").Value = True
        siteRoot.Properties("AuthFlags").Value = 5
        siteRoot.Properties("AccessScript").Value = True
        siteRoot.Properties("AuthAnonymous").Value = True
        siteRoot.Properties("AppPoolId").Value = "DefaultAppPool"
        siteRoot.Put()

        Console.WriteLine("Web site created. Starting IISWMIDemo...")

        ' Wait for a while to allow WMI finish the above operations 
        ' and then start the web site. 
        System.Threading.Thread.Sleep(1000)
        Dim site As New ManagementObject(managementScope, New ManagementPath(sitePath), Nothing)
        site.InvokeMethod("Start", Nothing)

        Console.WriteLine("Started " & vbLf)

        Console.WriteLine("Create new virtual directory.")

        ' Create virtual directory 
        Dim vdirPath As String = String.Format("W3SVC/{0}/Root/vdir1", siteID)

        Dim virtualDirClass As New ManagementClass(managementScope, New ManagementPath("IIsWebVirtualDirSetting"), Nothing)
        Dim virtualDirObj As ManagementObject = virtualDirClass.CreateInstance()

        virtualDirObj.Properties("Name").Value = vdirPath
        virtualDirObj.Properties("Path").Value = "C:\inetpub\IISWMIDemo\vdir1"
        virtualDirObj.Properties("AuthFlags").Value = 5
        virtualDirObj.Properties("EnableDefaultDoc").Value = True
        virtualDirObj.Properties("DirBrowseFlags").Value = &H4000003E
        virtualDirObj.Properties("AccessFlags").Value = 513
        virtualDirObj.Put()

        ' Create web application for this virtual directory 
        Dim virtualDir As New ManagementObject(managementScope, New System.Management.ManagementPath("IIsWebVirtualDir='" & vdirPath & "'"), Nothing)
        Dim appParameters As ManagementBaseObject = virtualDir.GetMethodParameters("AppCreate2")
        appParameters("AppMode") = 2
        virtualDir.InvokeMethod("AppCreate2", appParameters, Nothing)
        Dim virtualDirSettings As New ManagementObject(managementScope, New System.Management.ManagementPath("IIsWebVirtualDirSetting='" & vdirPath & "'"), Nothing)
        virtualDirSettings.Properties("AppFriendlyName").Value = "vdir1App"
        virtualDirSettings.Put()

        Console.WriteLine("vdir1 created. " & vbLf)

        ' List all web sites on the server. 
        Console.WriteLine("List all web sites on server: " & vbLf)

        Dim queryObject As New ObjectQuery("select * from IISWebServerSetting")
        Dim searchObject As New ManagementObjectSearcher(managementScope, queryObject)
        For Each result As ManagementObject In searchObject.[Get]()
            Console.WriteLine((result("ServerComment") & " (") + result("Name") & ")")
        Next


        Console.WriteLine(vbLf & "End.")

        Console.Read()


    End Sub

End Module
