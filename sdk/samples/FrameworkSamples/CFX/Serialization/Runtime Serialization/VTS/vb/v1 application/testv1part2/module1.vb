Imports System
Imports System.IO
Imports System.Runtime.Serialization
Imports System.Runtime.Serialization.Formatters.Binary
Imports Microsoft.Samples.Test


Class TestV1Part2

    'Entry point which delegates to C-style main Private Function
    Public Overloads Shared Sub Main()
    Dim formatter As New BinaryFormatter()
        Dim v2File As FileStream = Nothing
        Try

            v2File = New FileStream("..\..\..\Output\v2Output.bin", FileMode.Open)
            Dim aV2Person As Person = CType(formatter.Deserialize(v2File), Person)

            Console.WriteLine(ControlChars.Lf & "Person values after deserialize from v2:")
            Console.WriteLine((ControlChars.Tab & "Name: " & aV2Person.Name))
            Console.WriteLine((ControlChars.Tab & "Address: " & aV2Person.Address))
            Console.WriteLine((ControlChars.Tab & "Birth Date: " & aV2Person.BirthDate.ToShortDateString()))
        Catch e As FileNotFoundException
            Console.WriteLine(("An error occurred:" & ControlChars.Lf & e.ToString()))
        Finally
            If Not (v2File Is Nothing) Then
                v2File.Close()
            End If
        End Try
    End Sub 'Main
End Class 'TestV1Part2
