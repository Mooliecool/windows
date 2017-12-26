Imports System
Imports System.Globalization
Imports System.Windows.Data

    Public Class DoubleToString
    Implements IValueConverter

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If (Not value Is Nothing) Then
            Return value.ToString
        End If
        Return Nothing
    End Function

    Public Function ConvertBack1(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Dim num1 As Double
        Dim text1 As String = TryCast(value, String)
        If ((Not text1 Is Nothing) AndAlso Double.TryParse(text1, num1)) Then
            Return num1
        End If
        Return Nothing
    End Function
End Class


