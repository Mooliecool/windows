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

' Local service
Public Class DocumentApprovalService
    Implements IDocumentApproval
    Private mainForm As Mainform

    Public Sub New(ByVal main As Mainform)
        Me.mainForm = main
    End Sub
    ' Method to raise a DocumentRequested event to host
    Public Sub RequestDocumentApproval(ByVal documentId As System.Guid, ByVal approver As String) Implements IDocumentApproval.RequestDocumentApproval
        mainForm.DocumentRequested(documentId, approver)
    End Sub

    ' Method to raise a DocumentRequested event to workflow
    Public Sub ApproveDocument(ByVal documentId As System.Guid, ByVal approver As String) Implements IDocumentApproval.ApproveDocument
        RaiseEvent DocumentApproved(Nothing, New DocumentEventArgs(documentId, approver))
    End Sub

    ' Received document approval
    ' Event DocumentApproved As EventHandler(Of DocumentEventArgs) Implements IDocumentApproval.DocumentApproved
    Event DocumentApproved(ByVal sender As Object, ByVal e As DocumentEventArgs) Implements IDocumentApproval.DocumentApproved
End Class
