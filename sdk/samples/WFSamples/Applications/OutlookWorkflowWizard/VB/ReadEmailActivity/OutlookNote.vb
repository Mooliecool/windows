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
Imports System.Workflow.Activities

Public Class OutlookNote
    Inherits BaseOutlookItem

    Protected Overrides Function Execute(ByVal executionContext As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus

        ' Create an Outlook Application object. 
        Dim outlookApp As Outlook.Application = New Outlook.Application()
        ' Create a new TaskItem.
        Dim NewNote As Outlook.NoteItem = CType(outlookApp.CreateItem(Outlook.OlItemType.olNoteItem), Outlook.NoteItem)
        ' Configure the task at hand and save it.
        Dim dummy As Activity

        If TypeOf Me.Parent.Parent Is ParallelActivity Then
            dummy = Me.Parent.Parent.Parent.Activities.Item(1)
            If Not (CType(dummy, DummyActivity).Title = "") Then
                MessageBox.Show("Creating Outlook Note")
                NewNote.Body = CType(dummy, DummyActivity).Title
                NewNote.Save()
            End If
        End If
        If TypeOf Me.Parent.Parent Is SequentialWorkflowActivity Then
            dummy = Me.Parent.Parent.Activities.Item(1)
            If Not (CType(dummy, DummyActivity).Title = "") Then
                MessageBox.Show("Creating Outlook Note")
                NewNote.Body = CType(dummy, DummyActivity).Title
                NewNote.Save()
            End If
        End If

        Return ActivityExecutionStatus.Closed
    End Function
End Class
