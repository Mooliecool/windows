'****************************** Module Header ******************************\
' Module Name:    Global.asax.vb
' Project:        VBASPNETBackgroundWorker
' Copyright (c) Microsoft Corporation
'
' When application starts up, Application_Start() method will be called. 
' In the Application_Start() method, it creates a BackgroundWorker object and 
' then stores it in Application State. Therefore, the worker_DoWork() will 
' keep executing until application ends. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Web.SessionState
Imports System.Threading

Public Class Global_asax
    Inherits System.Web.HttpApplication

    ''' <summary>
    ''' Create a Background Worker to run the operation
    ''' whenever the application start.
    ''' </summary>
    Protected Sub Application_Start(ByVal sender As Object, ByVal e As EventArgs)
        Dim worker As New BackgroundWorker()
        AddHandler worker.DoWork, AddressOf worker_DoWork
        worker.RunWorker(Nothing)

        ' This Background Worker is Applicatoin Level,
        ' so it will keep working and it is shared by all users.
        Application("worker") = worker
    End Sub

    ''' <summary>
    ''' This operation will work without the end.
    ''' </summary>
    Private Sub worker_DoWork(ByRef progress As Integer, ByRef _result As Object, ByVal ParamArray arguments As Object())
        ' Do the operation every 1 second wihout the end.
        While True
            Thread.Sleep(1000)

            ' This statement will run every 1 second.

            ' Other logic which you want it to keep running.
            ' You can do some scheduled tasks here by checking DateTime.Now.
            progress += 1
        End While
    End Sub

End Class