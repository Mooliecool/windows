Imports System

Public Class WizardContext

    Public Sub New(ByVal result As WizardResult, ByVal data As Object)
        Me.Result = result
        Me.Data = data
    End Sub

    Public Property Data() As Object
        Get
            Return Me._data
        End Get
        Set(ByVal value As Object)
            Me._data = value
        End Set
    End Property

    Public Property Result() As WizardResult
        Get
            Return Me._result
        End Get
        Set(ByVal value As WizardResult)
            Me._result = value
        End Set
    End Property

    Private _data As Object
    Private _result As WizardResult

End Class

