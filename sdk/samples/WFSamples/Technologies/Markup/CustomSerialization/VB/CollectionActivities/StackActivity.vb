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
imports System.Workflow.ComponentModel.Serialization
imports System.Workflow.ComponentModel
imports System.Workflow.Activities
Imports System.ComponentModel.Design.Serialization
Imports System.Runtime.InteropServices

<DesignerSerializer(GetType(StackActivitySerializer), GetType(WorkflowMarkupSerializer))> _
<ComVisible(False)> _
Public Class StackActivity
    Inherits Activity

    Private nameStackValue As Stack

    Public Property NameStack() As Stack
        Get
            Return nameStackValue
        End Get
        Set(ByVal value As Stack)
            nameStackValue = value
        End Set
    End Property

    Public Sub New()
        MyBase.New()
        InitializeComponent()
        nameStackValue = New Stack()
    End Sub

    Protected Overrides Function Execute(ByVal executionContext As ActivityExecutionContext) As ActivityExecutionStatus
        While nameStackValue.Count > 0
            Console.WriteLine("Stack item: " + nameStackValue.Pop().ToString())
        End While

        Return ActivityExecutionStatus.Closed
    End Function
End Class
