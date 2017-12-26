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
Imports System.Collections
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.Activities
Imports System.ComponentModel.Design.Serialization
Imports System.Runtime.InteropServices

<DesignerSerializer(GetType(QueueActivitySerializer), GetType(WorkflowMarkupSerializer))> _
<ComVisible(False)> _
Public Class QueueActivity
    Inherits Activity
    Public Sub New()
        MyBase.New()
        InitializeComponent()
        nameQueueValue = New Queue()
    End Sub

    Private nameQueueValue As Queue
    Public Property NameQueue() As Queue
        Get
            Return nameQueueValue
        End Get
        Set(ByVal value As Queue)
            nameQueueValue = value
        End Set
    End Property

    Protected Overrides Function Execute(ByVal executionContext As ActivityExecutionContext) As ActivityExecutionStatus
        While nameQueueValue.Count > 0
            Console.WriteLine("Queue item: " + nameQueueValue.Dequeue().ToString())
        End While

        Return ActivityExecutionStatus.Closed
    End Function
End Class
