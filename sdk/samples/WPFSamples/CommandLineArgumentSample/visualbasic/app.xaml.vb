Imports System.Text.RegularExpressions

Partial Public Class App
    Inherits System.Windows.Application

    ' Indexed command line args using hash table
    Public Shared CommandLineArgs As Hashtable = New Hashtable

    Private Sub app_Startup(ByVal sender As Object, ByVal e As StartupEventArgs)

        ' Don't bother if no command line args were passed
        ' NOTE: e.Args is never null - if no command line args were passed, 
        '       the length of e.Args is 0.
        If (e.Args.Length = 0) Then Return

        ' Parse command line args for args in the following format:
        '   /argname:argvalue /argname:argvalue /argname:argvalue ...
        '
        ' Note: This sample uses regular expressions to parse the command line arguments.
        ' For regular expressions, see:
        ' http://msdn.microsoft.com/library/en-us/cpgenref/html/cpconRegularExpressionsLanguageElements.asp
        Dim pattern As String = "(?<argname>/\w+):(?<argvalue>\w+)"
        Dim arg As String
        For Each arg In e.Args

            Dim match As Match = Regex.Match(arg, pattern)

            ' If match not found, command line args are improperly formed.
            If Not match.Success Then
                Throw New ArgumentException("The command line arguments are improperly formed. Use /argname:argvalue.")
            End If

            ' Store command line arg and value
            App.CommandLineArgs.Item(match.Groups.Item("argname").Value) = match.Groups.Item("argvalue").Value
        Next

    End Sub

End Class