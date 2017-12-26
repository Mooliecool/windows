'****************************** Module Header ******************************\
' Module Name:  Rect.cs
' Project:      VBAzureXbap
' Copyright (c) Microsoft Corporation.
' 
' The WPF Rect struct cannot be serialized by DataContractSerializer, so let's create our own.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class Rect
    Public Sub New()
    End Sub

    Public Sub New(ByVal x As Double, ByVal y As Double, ByVal width As Double, ByVal height As Double)
        Me.X = x
        Me.Y = y
        Me.Width = width
        Me.Height = height
    End Sub


    Public Property Height As Double
    Public Property Width As Double
    Public Property X As Double
    Public Property Y As Double

End Class
