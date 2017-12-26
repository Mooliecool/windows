'/************************************* Module Header **************************************\
'* Module Name:	MyRibbon.vb
'* Project:		VBOutlookUIDesigner
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBOutlookRibbonDesigner sample demonstrates how to use the Ribbon Designer to create
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
Imports System.Diagnostics
Imports System.Windows.Forms
Imports Microsoft.Office.Tools
Imports Microsoft.Office.Tools.Ribbon
Imports Outlook = Microsoft.Office.Interop.Outlook
#End Region


Public Class MyRibbon


    Private Sub MyRibbon_Load(ByVal sender As Object, ByVal e As RibbonUIEventArgs) Handles MyBase.Load

        ' Trying to get current Inspector object

        Dim inspector As Outlook.Inspector = Me.Context

        If Not inspector Is Nothing Then

            If TypeOf inspector.CurrentItem Is Outlook.MailItem Then

                ' MailItem object, set the Mail group on second tab visible
                grpMail.Visible = True

                Dim item As Outlook.MailItem = inspector.CurrentItem
                ' If the EntryID is null, this is a new mail, otherwise
                ' this should be in Reading mode.
                lblMailMode.Label = String.Format("Current Mode: {0}", _
                                        IIf(item.EntryID Is Nothing, "Drafting", "Reading"))
                Marshal.ReleaseComObject(item)

            ElseIf TypeOf inspector.CurrentItem Is Outlook.AppointmentItem Then

                ' Appointment object, set the Appointment group on second tab visible
                grpAppointmentItem.Visible = True

            ElseIf TypeOf inspector.CurrentItem Is Outlook.TaskItem Then

                ' Task object, set the Task group on second tab visible
                grpTaskItem.Visible = True

            ElseIf TypeOf inspector.CurrentItem Is Outlook.ContactItem Then

                'Contact object, set the Contact group on second tab visible
                grpContactItem.Visible = True


                ' Release our reference to the inspector
                Marshal.ReleaseComObject(inspector)
                inspector = Nothing
                GC.Collect()
            End If
        End If
    End Sub

    Private Sub cboMyList_ItemsLoading(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles cboMyList.ItemsLoading

        ' Add ComboBox items via code
        Dim item As RibbonDropDownItem = New RibbonDropDownItem()
        item.Label = String.Format("Item Added via code at time {0}", DateTime.Now)
        cboMyList.Items.Add(item)
    End Sub

    Private Sub btnWeb_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles btnWeb.Click

        'Go through the CustomTaskPanes and find out the one on current inspector
        For Each ctp As CustomTaskPane In Globals.ThisAddIn.CustomTaskPanes

            If ctp.Window Is Me.Context Then

                ' Make the CustomTaskPane visible.
                ctp.Visible = True
            End If
        Next
    End Sub

    Private Sub tbSecondTab_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles tbSecondTab.Click

        ' Show / Hide the second customized tab on ribbon
        mySecondTab.Visible = tbSecondTab.Checked
    End Sub

    Private Sub chkShowGroup_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles chkShowGroup.Click

        ' Show / Hide the third group on first customized tab
        grpThree.Visible = chkShowGroup.Checked
    End Sub

    Private Sub splitButton_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles splitButton.Click

        ' SplitButton itself clicked
        MessageBox.Show("SplitButton itself clicked", _
                        "Event", _
                        MessageBoxButtons.OK, _
                        MessageBoxIcon.Information)

    End Sub

    Private Sub btnAlign_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles btnAlignLeft.Click, btnAlignCenter.Click, btnAlignRight.Click

        ' SplitButton item clicked
        ' Get the reference to the clicked button
        Dim rb As RibbonButton = sender

        If Not rb Is Nothing Then
            MessageBox.Show(rb.Label, _
                            "SplitButton Item Clicked", _
                            MessageBoxButtons.OK, _
                            MessageBoxIcon.Information)
        End If
    End Sub

    Private Sub myFirstGroup_DialogLauncherClick(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles grpOne.DialogLauncherClick

        ' Dialog launcher clicked
        ' Open a dialog...
        If colorDlg.ShowDialog() = DialogResult.OK Then
            MessageBox.Show(colorDlg.Color.ToString(), _
                            "Selected Color", _
                            MessageBoxButtons.OK, _
                            MessageBoxIcon.Information)

        End If

    End Sub

    Private Sub txtEdit_TextChanged(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles txtEdit.TextChanged

        ' Edit control text changed,
        'set new text to label control (if not empty)
        If txtEdit.Text.Trim().Length > 0 Then
            lblSample.Label = txtEdit.Text
        End If
    End Sub
End Class
