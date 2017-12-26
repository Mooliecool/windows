'****************************** Module Header ******************************'
' Module Name:  Converters.vb
' Project:      VBWPFDataBinding
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to use DataBinding in WPF
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Public Class SalaryFormmatingConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.Convert
        Dim dolloars As Double = 0
        Dim formattedSalary As String = String.Empty

        If value Is Nothing Then
            Throw New NullReferenceException("value can not be null")
        Else
            [Double].TryParse(value.ToString(), dolloars)
        End If
        formattedSalary = [String].Format("Total={0}$", dolloars)
        Return formattedSalary
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        Dim dolloars As Double = 0
        If value Is Nothing Then
            Throw New NullReferenceException("value can not be null")
        End If
        [Double].TryParse(value.ToString().TrimStart(New Char() {"T"c, "t"c, "o"c, "a"c, "l"c, "="c}).TrimEnd("$"c), dolloars)
        Return dolloars
    End Function

#End Region
End Class