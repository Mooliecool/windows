'/************************************* Module Header **************************************\
'* Module Name:	MyFormRegion.vb
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
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports Office = Microsoft.Office.Core
Imports Outlook = Microsoft.Office.Interop.Outlook
#End Region


Public Class MyFormRegion

#Region "Form Region Factory"

    <Microsoft.Office.Tools.Outlook.FormRegionMessageClass(Microsoft.Office.Tools.Outlook.FormRegionMessageClassAttribute.Note)> _
    <Microsoft.Office.Tools.Outlook.FormRegionName("VBOutlookRibbonDesigner.MyFormRegion")> _
    Partial Public Class MyFormRegionFactory

        ' Occurs before the form region is initialized.
        ' To prevent the form region from appearing, set e.Cancel to true.
        ' Use e.OutlookItem to get a reference to the current Outlook item.
        Private Sub MyFormRegionFactory_FormRegionInitializing(ByVal sender As Object, ByVal e As Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs) Handles Me.FormRegionInitializing

        End Sub

    End Class

#End Region

    'Occurs before the form region is displayed. 
    'Use Me.OutlookItem to get a reference to the current Outlook item.
    'Use Me.OutlookFormRegion to get a reference to the form region.
    Private Sub MyFormRegion_FormRegionShowing(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.FormRegionShowing
        Dim mail As Outlook.MailItem = Me.OutlookItem
        If Not mail Is Nothing Then

            txtContent.Text = mail.HTMLBody

            ' If this is a new mail, we'll update the HTML view every 1 second.
            If mail.EntryID Is Nothing Then
                tmr.Enabled = True

                Marshal.ReleaseComObject(mail)
            End If
        End If
    End Sub

    'Occurs when the form region is closed.   
    'Use Me.OutlookItem to get a reference to the current Outlook item.
    'Use Me.OutlookFormRegion to get a reference to the form region.
    Private Sub MyFormRegion_FormRegionClosed(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.FormRegionClosed

    End Sub
    Private Sub tmr_Tick(ByVal sender As Object, ByVal e As EventArgs) Handles tmr.Tick

        ' Get the mail item and update the HTML view.
        Dim mail As Outlook.MailItem = Me.OutlookItem
        txtContent.Text = mail.HTMLBody
        Marshal.ReleaseComObject(mail)
    End Sub
End Class
