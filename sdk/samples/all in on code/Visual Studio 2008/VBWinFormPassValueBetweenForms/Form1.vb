'************************************* Module Header **************************************\
' Module Name:  Form1.vb
' Project:      VBWinFormPassValueBetweenForms
' Copyright (c) Microsoft Corporation.
' 
' This sample demonstrates how to pass value between forms.
'  
' There're two common ways to pass value between forms:
' 
' 1. Use a property.
'    Create a public property on the target form class, then we can pass value to the target 
'    form by setting value for the property.
' 
' 2. Use a method.
'    Create a public method on the target form class, then we can pass value to the target 
'    form by passing the value as parameter to the method.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************/

Public Class Form1
    Inherits Form

    Public Sub New()
        MyBase.New()
        InitializeComponent()
    End Sub

    Private Sub button1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles Button1.Click
        ' Method 1 - Use A Property
        Dim f2 As Form2 = New Form2
        f2.ValueToPassBetweenForms = Me.TextBox1.Text
        f2.ShowDialog()
    End Sub

    Private Sub button2_Click(ByVal sender As Object, ByVal e As EventArgs) Handles Button2.Click
        ' Method 2 - Use A Method
        Dim f2 As Form2 = New Form2
        f2.SetValueFromAnotherForm(Me.TextBox1.Text)
        f2.ShowDialog()
    End Sub
End Class
