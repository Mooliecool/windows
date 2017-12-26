Imports System
Imports System.Globalization
Imports System.Windows.Controls

    Friend Class InvalidCharacterRule
        Inherits ValidationRule

        ' Methods
        Public Overrides Function Validate(ByVal value As Object, ByVal cultureInfo As CultureInfo) As ValidationResult
            Dim num1 As Double = 0
            Try 
                If (CStr(value).Length > 0) Then
                    num1 = Double.Parse(CStr(value))
                End If
            Catch exception1 As  Exception
                Return New ValidationResult(False, ("Illegal characters or " & exception1.Message))
            End Try
            Return New ValidationResult(True, Nothing)
        End Function

    End Class


