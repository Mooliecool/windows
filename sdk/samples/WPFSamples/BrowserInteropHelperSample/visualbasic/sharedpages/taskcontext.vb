Imports System

Namespace SharedPages

    ' Used to manage the state of a particular task, including:
    '    1) If completed, whether it was accepted or canceled (.Result)
    '    2) The data that was collected by the task (.Data).
    Public Class TaskContext

        Public Sub New(ByVal result As TaskResult, ByVal data As Object)
            Me._result = result
            Me._data = data
        End Sub


        ' Properties
        Public Property Data() As Object
            Get
                Return Me._data
            End Get
            Set(ByVal value As Object)
                Me._data = value
            End Set
        End Property

        Public Property Result() As TaskResult
            Get
                Return Me._result
            End Get
            Set(ByVal value As TaskResult)
                Me._result = value
            End Set
        End Property


        Private _data As Object
        Private _result As TaskResult

    End Class
End Namespace


