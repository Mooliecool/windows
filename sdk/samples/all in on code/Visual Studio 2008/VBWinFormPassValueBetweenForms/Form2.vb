'************************************* Module Header **************************************\
' Module Name:  Form2.vb
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

Public Class Form2
    Inherits Form

    Public Sub New()
        MyBase.New()
        InitializeComponent()

        AddHandler Load, AddressOf Me.FrmPassValueBetweenForms2_Load

    End Sub

    Private Sub FrmPassValueBetweenForms2_Load(ByVal sender As Object, ByVal e As EventArgs)
        Me.lbDisplay.Text = Me._valueToPassBetweenForms
    End Sub

    Private _valueToPassBetweenForms As String

    Public Property ValueToPassBetweenForms() As String
        Get
            Return Me._valueToPassBetweenForms
        End Get
        Set(ByVal value As String)
            Me._valueToPassBetweenForms = value
        End Set
    End Property


    Public Sub SetValueFromAnotherForm(ByVal val As String)
        Me._valueToPassBetweenForms = val
    End Sub

End Class