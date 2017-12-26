Imports System
Imports System.IO
Imports System.Runtime.Serialization
Imports System.Runtime.Serialization.Formatters.Binary
Imports Microsoft.Samples.Test


Class TestV1Part1

    'Entry point which delegates to C-style main Private Function
    Public Overloads Shared Sub Main()

        Dim formatter As New BinaryFormatter()
        Dim v1File As FileStream = Nothing
        Try

            v1File = New FileStream("..\..\..\Output\v1Output.bin", FileMode.Create)
            Dim aPerson As New Person()
            aPerson.Name = "Johnathan"
            aPerson.Address = "102 Main Street"
            aPerson.BirthDate = New DateTime(1980, 1, 31, 1, 0, 0, DateTimeKind.Local)

            formatter.Serialize(v1File, aPerson)

        Catch e As FileNotFoundException
            Console.WriteLine(("An error occurred:" & ControlChars.Lf & e.ToString()))
        Finally
            If Not (v1File Is Nothing) Then
                v1File.Close()
            End If
        End Try

        Console.WriteLine()
        Console.WriteLine("Person v1 written out to ..\..\..\Output\v1Output.bin")
    End Sub 'Main '

End Class 'TestV1 '
