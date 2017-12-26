'/************************************* Module Header **************************************\
'* Module Name:	ThisAddIn.vb
'* Project:		VBOutlookUIDesigner
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBOutlookUIDesigner sample demonstrates how to use the Ribbon Designer to create
'* customized Ribbon for Outlook 2007 inspectors.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/18/2009 18:00 PM Li ZhenHao Created
'\******************************************************************************************/

#Region "Using directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Xml.Linq
Imports Outlook = Microsoft.Office.Interop.Outlook
Imports Office = Microsoft.Office.Core
Imports Microsoft.Office.Tools
#End Region


Public Class ThisAddIn

    Private Sub ThisAddIn_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup
        ' Register for the NewInspector event
        AddHandler Me.Application.Inspectors.NewInspector, AddressOf Inspectors_NewInspector
    End Sub

    Private Sub Inspectors_NewInspector(ByVal inspector As Outlook.Inspector)

        ' We will add our custom task pane to the new inspector.
        If TypeOf inspector.CurrentItem Is Outlook.MailItem Or _
               TypeOf inspector.CurrentItem Is Outlook.AppointmentItem Or _
               TypeOf inspector.CurrentItem Is Outlook.TaskItem Or _
               TypeOf inspector.CurrentItem Is Outlook.ContactItem Then
            ' Create the custom task pane and make it initially invisible.
            Dim mtp As MyTaskPane = New MyTaskPane()
            Dim ctp As Microsoft.Office.Tools.CustomTaskPane = Me.CustomTaskPanes.Add(mtp, "Browser", inspector)
            ctp.Visible = False
            ctp.DockPosition = Microsoft.Office.Core.MsoCTPDockPosition.msoCTPDockPositionBottom
            mtp.ParentCustomTaskPane = ctp
        End If
    End Sub

    Private Sub ThisAddIn_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub
End Class
