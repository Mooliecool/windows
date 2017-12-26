'****************************** Module Header ******************************\
' Module Name:    GlobalBackgroundWorker.aspx.vb
' Project:        VBASPNETBackgroundWorker
' Copyright (c) Microsoft Corporation
'
' This page uses Timer control to display the progress of the BackgroundWorker 
' object which is working in the application level.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class GlobalBackgroundWorker
    Inherits System.Web.UI.Page

    Protected Sub Timer1_Tick(ByVal sender As Object, ByVal e As EventArgs)
        ' Show the progress of the Application Level Background Worker.
        ' A Background Worker has been created in Application_Start() method
        ' in Global.asax.cs file.
        Dim globalWorker As BackgroundWorker = DirectCast(Application("worker"), BackgroundWorker)
        If globalWorker IsNot Nothing Then
            lbGlobalProgress.Text = "Global worker is running: " & globalWorker.Progress.ToString()
        End If
    End Sub

End Class