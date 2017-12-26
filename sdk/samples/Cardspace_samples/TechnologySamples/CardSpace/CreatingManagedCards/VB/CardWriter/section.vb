Imports System
Imports System.Reflection

Namespace ManagedCardWriter
    <DefaultMember("Item")> _
    Public Class section
        ' Methods
        Public Sub New(ByVal sectionname As String, ByVal keys As key())
            Me.keys = keys
            Me.name = sectionname
        End Sub


        ' Properties
        Public ReadOnly Property Item(ByVal index As String) As key
            Get
                Dim key1 As key
                For Each key1 In Me.keys
                    If key1.name.Equals(index, StringComparison.CurrentCultureIgnoreCase) Then
                        Return key1
                    End If
                Next
                Return New key("", "")
            End Get
        End Property


        ' Fields
        Public keys As key()
        Public name As String
    End Class
End Namespace


