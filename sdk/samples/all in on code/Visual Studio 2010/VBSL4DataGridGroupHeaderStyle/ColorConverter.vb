'***************************** Module Header *******************************\
' Module Name:  ColorConverter.vb
' Project:      VBSL4DataGridGroupHeaderStyle
' Copyright (c) Microsoft Corporation.
' 
' Convert GroupHeader's background according to Group Name.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/
Imports System.Windows.Media
Imports System.Windows.Data

Public Class ColorConverter
    Implements IValueConverter

    Public Function Convert(ByVal value As Object, ByVal targetType As Type, _
    ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object _
    Implements IValueConverter.Convert
        If value.Equals("Kid") Then
            Return New SolidColorBrush(Colors.Yellow)
        End If
        If value.Equals("Adult") Then
            Return New SolidColorBrush(Colors.Orange)
        Else
            Return New SolidColorBrush(Colors.Gray)
        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, _
    ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object _
    Implements IValueConverter.ConvertBack
        Throw New InvalidOperationException("Converter cannot convert back.")
    End Function

End Class


