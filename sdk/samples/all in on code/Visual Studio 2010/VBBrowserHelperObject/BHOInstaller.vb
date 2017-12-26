'*************************** Module Header ******************************'
' Module Name:  BHOInstaller.vb
' Project:	    VBBrowserHelperObject
' Copyright (c) Microsoft Corporation.
' 
' The class BHOInstaller inherits the class System.Configuration.Install.Installer.
' The methods Install and Uninstall will be run when this application is being 
' installed or uninstalled.
' 
' This action has to be added the the custom actions of the installer to take effect. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.ComponentModel
Imports System.Configuration.Install
Imports System.Runtime.InteropServices

<RunInstaller(True), ComVisible(False)>
Partial Public Class BHOInstaller
    Inherits Installer
    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' This is called when installer's custom action executes and
    ''' registers the toolbar which is extending a bandobject, as COM server.
    ''' </summary>
    ''' <param name="stateSaver"></param>     
    Public Overrides Sub Install(ByVal stateSaver As System.Collections.IDictionary)
        MyBase.Install(stateSaver)


        Dim regsrv As New RegistrationServices()
        If Not regsrv.RegisterAssembly(Me.GetType().Assembly,
                                       AssemblyRegistrationFlags.SetCodeBase) Then
            Throw New InstallException("Failed To Register for COM")
        End If
    End Sub

    ''' <summary>
    ''' This is called when installer's custom action executes and
    ''' registers the toolbar which is extending a bandobject, as COM server.
    ''' </summary>
    ''' <param name="stateSaver"></param>     
    Public Overrides Sub Uninstall(ByVal savedState As System.Collections.IDictionary)
        MyBase.Uninstall(savedState)
        Dim regsrv As New RegistrationServices()
        If Not regsrv.UnregisterAssembly(Me.GetType().Assembly) Then
            Throw New InstallException("Failed To Unregister for COM")
        End If
    End Sub

End Class
