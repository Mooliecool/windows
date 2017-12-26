'************************************* Module Header **************************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormLocalization
' Copyright (c) Microsoft Corporation.
' 
' The Windows Forms Localization example demonstrates how to localize Windows Forms 
' application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Threading
Imports System.Globalization

Public Class MainForm
    Public Sub New()
        Thread.CurrentThread.CurrentUICulture = New CultureInfo("zh-CHS")
        InitializeComponent()
    End Sub
End Class
