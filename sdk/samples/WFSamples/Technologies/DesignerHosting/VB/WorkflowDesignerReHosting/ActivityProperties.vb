'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Windows.Forms

Public Class ActivityProperties
    Private resultValue As DialogResult

    Public Sub New()
        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        Me.resultValue = Windows.Forms.DialogResult.Cancel
    End Sub

    Public ReadOnly Property ActivityName() As String
        Get
            Return nameTextBox.Text
        End Get
    End Property

    Public ReadOnly Property Description() As String
        Get
            Return descriptionTextBox.Text
        End Get
    End Property

    Public ReadOnly Property Result() As DialogResult
        Get
            Return Me.resultValue
        End Get
    End Property

    Private Sub okButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles okButton.Click
        Me.resultValue = Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub
End Class