'****************************** Module Header ******************************\
' Module Name:    SharedSessionModule.vb
' Project:        VBASPNETShareSessionBetweenSubDomains
' Copyright (c) Microsoft Corporation
'
' This project demonstrates how to configure a SQL Server as SessionState and 
' make a module to share Session between two Web Sites with the same root domain.
' 
' SharedSessionModule is used to make Web Sites use the same Application Id and 
' Session Id to achieve sharing Session.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Web
Imports System.Configuration
Imports System.Reflection

''' <summary>
''' A HttpModule used for sharing the session between Applications in 
''' sub domains.
''' </summary>
Public Class SharedSessionModule
    Implements IHttpModule
    ' Cache settings on memory.
    Protected Shared applicationName As String = ConfigurationManager.AppSettings("ApplicationName")
    Protected Shared rootDomain As String = ConfigurationManager.AppSettings("RootDomain")

#Region "IHttpModule Members"
    ''' <summary>
    ''' Initializes a module and prepares it to handle requests.
    ''' </summary>
    ''' <param name="context">
    ''' An System.Web.HttpApplication
    ''' that provides access to the methods,
    ''' properties, and events common to all application objects within 
    ''' an ASP.NET application.
    ''' </param>
    Public Sub Init(ByVal context As HttpApplication) Implements IHttpModule.Init
        ' This module requires both Application Name and Root Domain to work.
        If String.IsNullOrEmpty(applicationName) OrElse String.IsNullOrEmpty(rootDomain) Then
            Return
        End If

        ' Change the Application Name in runtime.
        Dim runtimeInfo As FieldInfo = GetType(HttpRuntime).GetField("_theRuntime", BindingFlags.[Static] Or BindingFlags.NonPublic)
        Dim theRuntime As HttpRuntime = DirectCast(runtimeInfo.GetValue(Nothing), HttpRuntime)
        Dim appNameInfo As FieldInfo = GetType(HttpRuntime).GetField("_appDomainAppId", BindingFlags.Instance Or BindingFlags.NonPublic)

        appNameInfo.SetValue(theRuntime, applicationName)

        ' Subscribe Events.
        AddHandler context.PostRequestHandlerExecute, AddressOf context_PostRequestHandlerExecute
    End Sub

    ''' <summary>
    ''' Disposes of the resources (other than memory) used by the module
    ''' that implements.
    ''' </summary>
    Public Sub Dispose() Implements IHttpModule.Dispose
    End Sub
#End Region

    ''' <summary>
    ''' Before sending response content to client, change the Cookie to Root Domain
    ''' and store current Session Id.
    ''' </summary>
    ''' <param name="sender">
    ''' An instance of System.Web.HttpApplication that provides access to
    ''' the methods, properties, and events common to all application
    ''' objects within an ASP.NET application.
    ''' </param>
    Private Sub context_PostRequestHandlerExecute(ByVal sender As Object, ByVal e As EventArgs)
        Dim context As HttpApplication = DirectCast(sender, HttpApplication)

        ' ASP.NET store a Session Id in cookie to specify current Session.
        Dim cookie As HttpCookie = context.Response.Cookies("ASP.NET_SessionId")

        If context.Session IsNot Nothing AndAlso Not String.IsNullOrEmpty(context.Session.SessionID) Then
            ' Need to store current Session Id during every request.
            cookie.Value = context.Session.SessionID

            ' All Applications use one root domain to store this Cookie
            ' So that it can be shared.
            If rootDomain <> "localhost" Then
                cookie.Domain = rootDomain
            End If

            ' All Virtual Applications and Folders share this Cookie too.
            cookie.Path = "/"
        End If
    End Sub
End Class
