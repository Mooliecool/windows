'****************************** Module Header ******************************'
' Module Name:              SampleContext.vb
' Project:                  VBSL3Navigation
' Copyright (c) Microsoft Corporation.
' 
' HyperLinkConverter code file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Windows.Data

' HyperLink coverter
' Replace the '{}' string in parameter url with
' binding value 
Public Class HyperLinkConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        Dim url = TryCast(parameter, String)
        If url IsNot Nothing Then
            Return url.Replace("{}", value.ToString())
        Else
            Return Nothing
        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Throw New NotImplementedException()
    End Function

#End Region
End Class
