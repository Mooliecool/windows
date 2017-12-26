Imports System
Imports System.Runtime.Serialization

   <Serializable()> _
    Public Class Person

    Public Sub New()
    End Sub 'New '
    Private _name As String
    Private _address As String
    Private _birthDate As DateTime

    <OptionalField(VersionAdded:=2)> _
    Private _age As Integer

    Public Property Name() As String
        Get
            Return _name
        End Get
        Set(ByVal value As String)
            _name = value
        End Set
    End Property

    Public Property Address() As String
        Get
            Return _address
        End Get
        Set(ByVal value As String)
            _address = value
        End Set
    End Property

    Public Property BirthDate() As DateTime
        Get
            Return _birthDate
        End Get
        Set(ByVal value As DateTime)
            _birthDate = value
        End Set
    End Property

    Public Property Age() As Integer
        Get
            Return _age
        End Get
        Set(ByVal value As Integer)
            _age = value
        End Set
    End Property

    <OnDeserializing()> _
    Private Sub OnDeserializing(ByVal context As StreamingContext) '
        Console.WriteLine(ControlChars.Lf & "Deserializing into a v2 type of Person.")

        _age = 0

        Console.WriteLine(ControlChars.Lf & ControlChars.Tab & "Defaults set:")
        Console.WriteLine((ControlChars.Tab & ControlChars.Tab & "Age: " & _age))
    End Sub 'OnDeserializing


    <OnDeserialized()> _
    Private Sub OnDeserialized(ByVal context As StreamingContext)
        Console.WriteLine(ControlChars.Lf & "Deserialization completed.")
        If _age = 0 Then
            _age = DateTime.Now.Year - _birthDate.Year
            If DateTime.Now.DayOfYear < _birthDate.DayOfYear Then
                _age -= 1
            End If
        End If
        Console.WriteLine(ControlChars.Lf & ControlChars.Tab & "Fixups completed.")
        Console.WriteLine(ControlChars.Tab & "My Person v2 values are: ")
        Console.WriteLine((ControlChars.Tab & ControlChars.Tab & "Name: " & _name))
        Console.WriteLine((ControlChars.Tab & ControlChars.Tab & "Address: " & _address))
        Console.WriteLine((ControlChars.Tab & ControlChars.Tab & "BirthDate: " & _birthDate.ToShortDateString()))
        Console.WriteLine((ControlChars.Tab & ControlChars.Tab & "Age: " & _age))
    End Sub 'OnDeserialized


    <OnSerializing()> _
    Private Sub OnSerializing(ByVal context As StreamingContext)
        Console.WriteLine(ControlChars.Lf & "OnSerializing called.")
    End Sub 'OnSerializing


    <OnSerialized()> _
    Private Sub OnSerialized(ByVal context As StreamingContext)
        Console.WriteLine(ControlChars.Lf & "OnSerialized called.")
    End Sub 'OnSerialized
End Class 'Person 
