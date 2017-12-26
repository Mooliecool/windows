Imports System
Imports System.IO
Imports System.Runtime.Serialization
Imports System.Runtime.Serialization.Formatters.Binary
Imports Microsoft.Samples.Test

    Class TestV2

    'Entry point which delegates to C-style main Private Function
    Public Overloads Shared Sub Main()
        Dim formatter As New BinaryFormatter()
        Dim aPerson As Person = Nothing
        Dim v1File As FileStream = Nothing
        Try
            v1File = New FileStream("..\..\..\Output\v1Output.bin", FileMode.Open)
            aPerson = CType(formatter.Deserialize(v1File), Person)
            aPerson.Age = 98
        Catch e As FileNotFoundException
            Console.WriteLine(("An error occured:" & ControlChars.Lf & e.ToString()))
        Finally
            If Not (v1File Is Nothing) Then
                v1File.Close()
            End If
        End Try


        Dim v2File As FileStream = Nothing

        Try

            v2File = New FileStream("..\..\..\Output\v2Output.bin", FileMode.Create)
            If Not (aPerson Is Nothing) Then
                formatter.Serialize(v2File, aPerson)

                Console.WriteLine()
                Console.WriteLine("Person v2 written out to ..\..\..\Output\v2Output.bin")
            End If

        Catch e As FileNotFoundException
            Console.WriteLine(("An error occured:" & ControlChars.Lf & e.ToString()))
        Finally
            If Not (v2File Is Nothing) Then
                v2File.Close()
            End If
        End Try
    End Sub 'Main 
End Class 'TestV2
