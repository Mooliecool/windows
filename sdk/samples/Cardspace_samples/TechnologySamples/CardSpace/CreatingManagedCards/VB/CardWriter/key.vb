Imports System

Namespace ManagedCardWriter
    Public Class key
        ' Methods
        Public Sub New(ByVal keyname As String, ByVal value As String)
            Me.name = keyname
            Me.value = value
        End Sub


        ' Fields
        Public name As String
        Public value As String
    End Class
End Namespace


