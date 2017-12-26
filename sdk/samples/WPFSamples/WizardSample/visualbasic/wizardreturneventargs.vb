Imports System

Public Class WizardReturnEventArgs

    Public Sub New(ByVal result As WizardResult, ByVal data As Object)
        Me._result = result
        Me._data = data
    End Sub

    Public ReadOnly Property Data() As Object
        Get
            Return Me._data
        End Get
    End Property

    Public ReadOnly Property Result() As WizardResult
        Get
            Return Me._result
        End Get
    End Property

    Private _data As Object
    Private _result As WizardResult

End Class


