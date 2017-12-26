'****************************** Module Header ******************************\
' Module Name:  IntToBoolValueConverter.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' As the name implies, this IntToBoolValueConverter class converts an int value to a bool value. 
' This converter is used by the MenuItem on the main window that indicate the dimension of 
' the game.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Globalization

Public Class IntToBoolValueConverter
    Implements IValueConverter

    Private Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object,
                             ByVal culture As CultureInfo) As Object Implements IValueConverter.Convert
        Return (System.Convert.ToInt32(value) = System.Convert.ToInt32(parameter.ToString))
    End Function

    Private Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object,
                                 ByVal culture As CultureInfo) As Object Implements IValueConverter.ConvertBack
        Throw New NotImplementedException
    End Function

End Class


