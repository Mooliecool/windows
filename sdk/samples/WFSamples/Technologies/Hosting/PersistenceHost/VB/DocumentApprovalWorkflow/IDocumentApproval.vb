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
imports System.Workflow.Activities
imports System.Workflow.ComponentModel

<ExternalDataExchange()> _
Public Interface IDocumentApproval
    ' send document for approval
    Sub RequestDocumentApproval(ByVal documentId As Guid, ByVal approver As String)
    Sub ApproveDocument(ByVal documentId As Guid, ByVal approver As String)

    ' received document for approval
    Event DocumentApproved As EventHandler(Of DocumentEventArgs)
End Interface

