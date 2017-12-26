'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormSingleInstanceApp
' Copyright (c) Microsoft Corporation.
' 
' The  sample demonstrates how to achieve the goal that only 
' one instance of the application is allowed in Windows Forms application..
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class MainForm

#Region "Constructor"
    Public Sub New()
        MyBase.New()
        InitializeComponent()

        Me.StartPosition = FormStartPosition.CenterScreen

    End Sub
#End Region
End Class
