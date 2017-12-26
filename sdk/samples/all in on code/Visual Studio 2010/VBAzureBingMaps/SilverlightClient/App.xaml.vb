'***************************** Module Header ******************************\
'* Module Name:	App.xaml.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Code behind for App.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Windows

Partial Public Class App
    Inherits Application
    Friend Shared IsAuthenticated As Boolean = False
    Friend Shared WelcomeMessage As String = Nothing

    Public Sub New()
        AddHandler Me.Startup, AddressOf Me.Application_Startup
        AddHandler Me.[Exit], AddressOf Me.Application_Exit
        AddHandler Me.UnhandledException, AddressOf Me.Application_UnhandledException

        InitializeComponent()
    End Sub

    Private Sub Application_Startup(ByVal sender As Object, ByVal e As StartupEventArgs)
        IsAuthenticated = Boolean.Parse(e.InitParams("IsAuthenticated"))
        If IsAuthenticated Then
            WelcomeMessage = e.InitParams("WelcomeMessage")
        End If
        Me.RootVisual = New MainPage()
    End Sub

    Private Sub Application_Exit(ByVal sender As Object, ByVal e As EventArgs)

    End Sub

    Private Sub Application_UnhandledException(ByVal sender As Object, ByVal e As ApplicationUnhandledExceptionEventArgs)
        ' If the app is running outside of the debugger then report the exception using
        ' the browser's exception mechanism. On IE this will display it a yellow alert 
        ' icon in the status bar and Firefox will display a script error.
        If Not System.Diagnostics.Debugger.IsAttached Then

            ' NOTE: This will allow the application to continue running after an exception has been thrown
            ' but not handled. 
            ' For production applications this error handling should be replaced with something that will 
            ' report the error to the website and stop the application.
            e.Handled = True
            Deployment.Current.Dispatcher.BeginInvoke(Function()
                                                          ReportErrorToDOM(e)
                                                          Return Nothing
                                                      End Function)
        End If
		End Sub

    Private Sub ReportErrorToDOM(ByVal e As ApplicationUnhandledExceptionEventArgs)
        Try
            Dim errorMsg As String = e.ExceptionObject.Message + e.ExceptionObject.StackTrace
            errorMsg = errorMsg.Replace(""""c, "'"c).Replace(vbCr & vbLf, "\n")

            System.Windows.Browser.HtmlPage.Window.Eval("throw new Error(""Unhandled Error in Silverlight Application " & errorMsg & """);")
        Catch generatedExceptionName As Exception
        End Try
    End Sub
End Class