'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Workflow.ComponentModel

Namespace Microsoft.Samples.WF.Migration
    Public Class WriteLine
        Inherits Activity

        Public Sub New()
            MyBase.New()
        End Sub

        Public Shared ReadOnly TextProperty As DependencyProperty = DependencyProperty.Register("Text", GetType(String), GetType(WriteLine))

        Public Property Text() As String
            Get
                Return CStr(MyBase.GetValue(TextProperty))
            End Get
            Set(ByVal value As String)
                MyBase.SetValue(TextProperty, value)
            End Set
        End Property

        Protected Overrides Function Execute(ByVal context As ActivityExecutionContext) As ActivityExecutionStatus
            Console.WriteLine(Me.Text)
            Return ActivityExecutionStatus.Closed
        End Function
    End Class
End Namespace
