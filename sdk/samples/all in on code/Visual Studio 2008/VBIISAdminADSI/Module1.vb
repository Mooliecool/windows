'***************************** Module Header ******************************\ 
'* Module Name: Program.cs 
'* Project: VBIISAdminADSI 
'* Copyright (c) Microsoft Corporation. 
'* 
'* This sample demonstrates how to use Active Directory Service Interfaces (ADSI) 
'* to configure IIS by using .Net System.DirectoryServices namespace to access IIS 
'* ADSI Provider. The classes, methods, and properties of the IIS ADSI provider can 
'* be used to configure IIS from scripts or executables. 
'* 
'* Active Directory Service Interfaces (ADSI) is a directory service model and a 
'* set of Component Object Model (COM) interfaces. ADSI enables Windows applications 
'* and Active Directory clients to access several network directory services. The 
'* IIS ADSI provider, like the IIS WMI provider provides a standard syntax for 
'* accessing IIS configuration data. 
'* 
'* This source is subject to the Microsoft Public License. 
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL. 
'* All other rights reserved. 
'* 
'* History: 
'* * 1/12/2010 3:45 PM WenJun Zhang Created 
'\************************************************************************** 


Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.DirectoryServices

Module VBIISAdminADSI

    Sub Main()

        Console.WriteLine("Press any key to create the new web site.")
        Console.ReadLine()

        'Connect to IIS ADSI Provider 
        Dim ServerName As String = "LocalHost"
        Dim W3SVC As New DirectoryEntry("IIS://" & ServerName & "/w3svc")

        ' Create new web site. 
        Dim siteObj As Object
        Dim siteID As String = ""
        Dim siteName As String = "IISADSIDemo"
        Dim hostHeader As String = "www.mytestdomain.com"
        Dim serverBinding1 As String = ":8181:"
        Dim serverBinding2 As String = ":8282:" & hostHeader
        Dim rootPath As String = "C:\inetpub\IISADSIDemo"
        Dim siteProperty As Object() = New Object() {siteName, New Object() {serverBinding1, serverBinding2}, rootPath}

        Try
            siteObj = DirectCast(W3SVC.Invoke("CreateNewSite", siteProperty), Object)
            siteID = siteObj.ToString()
            W3SVC.CommitChanges()
        Catch ex As Exception
            Console.WriteLine("Fail to create web site with the following exception: " & vbLf & "{0}", ex.Message)
        End Try

        ' Wait for a while to allow ADSI finish the above operations 
        ' and then start the web site. 
        Console.WriteLine("Web site created. Starting IISADSIDemo...")
        System.Threading.Thread.Sleep(1000)

        Dim newSite As New DirectoryEntry(("IIS://" & ServerName & "/W3SVC/") + siteID)
        newSite.Invoke("Start")

        Console.WriteLine("Started " & vbLf)

        Console.WriteLine("Create new virtual directory.")

        ' Create new virtual directory. 
        Dim metabasePath As String = String.Format("/W3SVC/{0}/Root", siteID)
        Dim site As New DirectoryEntry(("IIS://" & ServerName) + metabasePath)
        Dim vdirs As DirectoryEntries = site.Children
        Dim newVDir As DirectoryEntry = vdirs.Add("vdir1", "IIsWebVirtualDir")

        newVDir.Properties("Path")(0) = "C:\inetpub\IISADSIDemo\vdir1"
        newVDir.Properties("AccessScript")(0) = True
        newVDir.Properties("AppFriendlyName")(0) = "vdir1"
        newVDir.Properties("AppIsolated")(0) = "1"
        newVDir.Properties("AppRoot")(0) = "/LM" & metabasePath & "/vdir1"

        newVDir.CommitChanges()

        Console.WriteLine("vdir1 created. " & vbLf)

        ' List all web sites on the server. 
        Console.WriteLine("List all web sites on server: " & vbLf)
        Try
            For Each item As DirectoryEntry In W3SVC.Children
                If item.SchemaClassName = "IIsWebServer" Then
                    Console.WriteLine((item.Name & " - ") + item.Properties("ServerComment").Value.ToString())
                End If
            Next
        Catch e As Exception
            Console.WriteLine("Fail to list web sites with the following exception: " & vbLf & "{0}: " & e.ToString())
        End Try

        Console.WriteLine(vbLf & "End.")
        Console.Read()

    End Sub

End Module