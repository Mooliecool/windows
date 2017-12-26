' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Drawing
Imports System.Text
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.Runtime

Namespace Microsoft.Rules.Samples
    Partial Public NotInheritable Class Workflow
        Inherits SequentialWorkflowActivity
        Private m_customerName As String ' ie. "John Customer"
        Private m_itemNum As Integer ' ie. 1 => for Vista Ultimate DVD
        Private m_zipCode As String ' ie. "00999"
        Private invalidOrder As OrderError
        Private invalidZipCodeErrorCollection As OrderErrorCollection
        Private invalidItemNumErrorCollection As OrderErrorCollection
        Private invalidOrdersCollection As OrderErrorCollection

        Public Property CustomerName() As String
            Get
                Return Me.m_customerName
            End Get
            Set(ByVal value As String)
                Me.m_customerName = Value
            End Set
        End Property

        Public Property ItemNum() As Integer
            Get
                Return Me.m_itemNum
            End Get
            Set(ByVal value As Integer)
                Me.m_itemNum = value
            End Set
        End Property

        Public Property ZipCode() As String
            Get
                Return Me.m_zipCode
            End Get
            Set(ByVal value As String)
                Me.m_zipCode = Value
            End Set
        End Property

        Public Sub New()
            InitializeComponent()
        End Sub
    End Class
End Namespace