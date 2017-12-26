'************************************* Module Header **************************************\
' Module Name:	EditorForm.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The CustomEditor sample demonstrates how to use a custom editor for a specific property 
' at design time. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.

' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Namespace CustomEditors
    Public Class EditorForm
        Private _subCls As SubClass = New SubClass

        Public Sub New()
            Me.InitializeComponent()
            AddHandler MyBase.Load, New EventHandler(AddressOf Me.EditorForm_Load)
        End Sub

        Private Sub button1_Click(ByVal sender As Object, ByVal e As EventArgs)
            MyBase.DialogResult = DialogResult.OK
        End Sub

        Private Sub button2_Click(ByVal sender As Object, ByVal e As EventArgs)
            MyBase.DialogResult = DialogResult.Cancel
        End Sub

        Private Sub EditorForm_Load(ByVal sender As Object, ByVal e As EventArgs)
            Me.textBox1.Text = Me._subCls.Name
            Me.monthCalendar1.SelectionStart = Me._subCls.MyDate
            Me.monthCalendar1.SelectionEnd = Me._subCls.MyDate
        End Sub

        Public Property SubCls() As SubClass
            Get
                Me._subCls.Name = Me.textBox1.Text
                Me._subCls.MyDate = Me.monthCalendar1.SelectionStart
                Return Me._subCls
            End Get
            Set(ByVal value As SubClass)
                Me._subCls = value
            End Set
        End Property

    End Class
End Namespace