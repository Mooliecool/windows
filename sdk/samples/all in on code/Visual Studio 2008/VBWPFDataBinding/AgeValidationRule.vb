'****************************** Module Header ******************************'
' Module Name:  AgeValidationRule.vb
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

Public Class AgeValidationRule
    Inherits ValidationRule

    ''' <summary> 
    ''' When overridden in a derived class, performs validation checks on a value. 
    ''' </summary> 
    ''' <param name="value">The value from the binding target to check.</param> 
    ''' <param name="cultureInfo">The culture to use in this rule.</param> 
    ''' <returns> 
    ''' A <see cref="T:System.Windows.Controls.ValidationResult"/> object. 
    ''' </returns> 
    Public Overloads Overrides Function Validate(ByVal value As Object, ByVal cultureInfo As System.Globalization.CultureInfo) As ValidationResult
        Dim age As Integer = 0
        If value IsNot Nothing Then
            Int32.TryParse(value.ToString(), age)
        End If
        If age < 0 OrElse age > 300 Then
            Return New ValidationResult(False, "Please input corrent age")
        Else
            Return New ValidationResult(True, Nothing)
        End If
    End Function
End Class