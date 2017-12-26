Module MainModule

    Sub RunSnippet()

    End Sub


    Sub Main(ByVal args() As String)
        Try
            RunSnippet()
        Catch e As Exception
            Dim [error] As String = String.Format("---" & vbLf & "The following error occurred while executing the snippet:" & vbLf & "{0}" & vbLf & "---", e.ToString())
            Console.WriteLine([error])
        Finally
            Console.Write("Press any key to continue...")
            Console.ReadKey()
        End Try
    End Sub

End Module