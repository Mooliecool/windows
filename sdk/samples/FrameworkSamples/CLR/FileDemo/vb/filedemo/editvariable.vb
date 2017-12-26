'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
'-----------------------------------------------------------------------
Namespace Microsoft.Samples.FileDemo
    Public NotInheritable Class EditVariable
        Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

        Public Sub New()
            MyBase.New()

            'This call is required by the Windows Form Designer.
            InitializeComponent()

            'Add any initialization after the InitializeComponent() call

        End Sub

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer
        Friend WithEvents Label1 As System.Windows.Forms.Label
        Friend WithEvents Label2 As System.Windows.Forms.Label
        Friend WithEvents buttonOK As System.Windows.Forms.Button

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.

        Friend WithEvents textName As System.Windows.Forms.TextBox
        Friend WithEvents textValue As System.Windows.Forms.TextBox
        Friend WithEvents buttonCancel As System.Windows.Forms.Button
        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.Label1 = New System.Windows.Forms.Label
            Me.Label2 = New System.Windows.Forms.Label
            Me.buttonOK = New System.Windows.Forms.Button
            Me.ButtonCancel = New System.Windows.Forms.Button
            Me.textName = New System.Windows.Forms.TextBox
            Me.textValue = New System.Windows.Forms.TextBox
            Me.SuspendLayout()
            '
            'Label1
            '
            Me.Label1.Location = New System.Drawing.Point(8, 8)
            Me.Label1.Name = "Label1"
            Me.Label1.Size = New System.Drawing.Size(144, 23)
            Me.Label1.TabIndex = 0
            Me.Label1.Text = "Variable Name:"
            '
            'Label2
            '
            Me.Label2.Location = New System.Drawing.Point(8, 40)
            Me.Label2.Name = "Label2"
            Me.Label2.Size = New System.Drawing.Size(144, 23)
            Me.Label2.TabIndex = 2
            Me.Label2.Text = "Variable Value:"
            '
            'buttonOK
            '
            Me.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK
            Me.buttonOK.Location = New System.Drawing.Point(473, 79)
            Me.buttonOK.Name = "buttonOK"
            Me.buttonOK.Size = New System.Drawing.Size(88, 32)
            Me.buttonOK.TabIndex = 4
            Me.buttonOK.Text = "OK"
            '
            'buttonCancel
            '
            Me.ButtonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.ButtonCancel.Location = New System.Drawing.Point(569, 79)
            Me.ButtonCancel.Name = "buttonCancel"
            Me.ButtonCancel.Size = New System.Drawing.Size(88, 32)
            Me.ButtonCancel.TabIndex = 5
            Me.ButtonCancel.Text = "Cancel"
            '
            'textName
            '
            Me.textName.Enabled = False
            Me.textName.Location = New System.Drawing.Point(152, 8)
            Me.textName.Name = "textName"
            Me.textName.Size = New System.Drawing.Size(505, 29)
            Me.textName.TabIndex = 1
            '
            'textValue
            '
            Me.textValue.Location = New System.Drawing.Point(152, 40)
            Me.textValue.Name = "textValue"
            Me.textValue.Size = New System.Drawing.Size(505, 29)
            Me.textValue.TabIndex = 3
            '
            'EditVariable
            '
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.CancelButton = Me.ButtonCancel
            Me.ClientSize = New System.Drawing.Size(669, 119)
            Me.Controls.Add(Me.textValue)
            Me.Controls.Add(Me.textName)
            Me.Controls.Add(Me.ButtonCancel)
            Me.Controls.Add(Me.buttonOK)
            Me.Controls.Add(Me.Label2)
            Me.Controls.Add(Me.Label1)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "EditVariable"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "Edit Variable"
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub

#End Region

        Private Sub EditVariable_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
            textValue.SelectAll()
        End Sub

        Private Sub buttonCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonCancel.Click
            NewName = ""
            Me.Close()
        End Sub

        Private Sub buttonOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonOK.Click
            Try
                EnvironmentChanged = True
                NewName = textName.Text.Trim()
                NewValue = TextValue.Text.Trim()
            Finally
                Me.Close()
            End Try
        End Sub
    End Class
End Namespace

