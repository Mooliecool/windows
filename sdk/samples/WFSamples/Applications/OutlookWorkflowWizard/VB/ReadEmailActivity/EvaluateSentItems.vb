'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Workflow.ComponentModel
Imports System.Windows.Forms
Imports Outlook = Microsoft.Office.Interop.Outlook

Public Class EvaluateSentItems
    Inherits BaseMailbox

    Protected Overrides Function Execute(ByVal executionContext As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus
        MessageBox.Show("Checking your Sent Items folder")
        Dim outlookApp As Outlook.Application = New Outlook.Application()
        Dim oSentItems As Outlook.MAPIFolder = outlookApp.GetNamespace("MAPI").GetDefaultFolder(Outlook.OlDefaultFolders.olFolderSentMail)
        Dim oItems As Outlook.Items = oSentItems.Items
        For Each rawItem As Object In oItems
            If TypeOf rawItem Is Outlook.MailItem Then
                Dim item As Outlook.MailItem = CType(rawItem, Outlook.MailItem)
                Dim dummy As Activity
                Select Case Filter
                    Case (FilterOption.Subject)
                        If (item.Subject <> Nothing) AndAlso item.Subject.Equals(FilterValue) Then
                            MessageBox.Show("Found message with Subject filter value[" + FilterValue + "]:" + item.Body)
                            dummy = Me.Parent.Activities.Item(1)
                            CType(dummy, DummyActivity).Title = item.Subject
                        End If
                        Exit For
                    Case (FilterOption.FromEmail)
                        If (item.SenderEmailAddress <> Nothing) AndAlso item.SenderEmailAddress.Equals(FilterValue) Then
                            MessageBox.Show("Found message with From filter value[" + FilterValue + "]:" + item.Body)
                            dummy = Me.Parent.Activities.Item(1)
                            CType(dummy, DummyActivity).Title = item.Subject
                        End If
                        Exit For
                    Case (FilterOption.To)
                        If (item.To <> Nothing) AndAlso item.To.Equals(FilterValue) Then
                            MessageBox.Show("Found message with To filter value[" + FilterValue + "]:" + item.Body)
                            dummy = Me.Parent.Activities.Item(1)
                            CType(dummy, DummyActivity).Title = item.Subject
                        End If
                        Exit For
                    Case (FilterOption.CC)
                        If (item.CC <> Nothing) AndAlso item.CC.Equals(FilterValue) Then
                            MessageBox.Show("Found message with CC filter value[" + FilterValue + "]:" + item.Body)
                            dummy = Me.Parent.Activities.Item(1)
                            CType(dummy, DummyActivity).Title = item.Subject
                        End If
                        Exit For
                    Case (FilterOption.Bcc)
                        If (item.BCC <> Nothing) AndAlso item.BCC.Equals(FilterValue) Then
                            MessageBox.Show("Found message with BCC filter value[" + FilterValue + "]:" + item.Body)
                            dummy = Me.Parent.Activities.Item(1)
                            CType(dummy, DummyActivity).Title = item.Subject
                        End If
                        Exit For
                End Select
            End If
        Next
        MessageBox.Show("Done with Execute in EvaluateSentItems")
        Return ActivityExecutionStatus.Closed
    End Function
End Class
