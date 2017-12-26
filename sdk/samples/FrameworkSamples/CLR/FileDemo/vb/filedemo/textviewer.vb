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
Imports System.IO

Namespace Microsoft.Samples.FileDemo
    Public NotInheritable Class FormTextViewer
        Inherits System.Windows.Forms.Form

        Private _filePath As String
        Private changedText As Boolean

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

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        Friend WithEvents buttonClose As System.Windows.Forms.Button
        Friend WithEvents buttonOK As System.Windows.Forms.Button
        Friend WithEvents textMain As System.Windows.Forms.TextBox
        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.textMain = New System.Windows.Forms.TextBox
            Me.buttonClose = New System.Windows.Forms.Button
            Me.buttonOK = New System.Windows.Forms.Button
            Me.SuspendLayout()
            '
            'textMain
            '
            Me.textMain.Location = New System.Drawing.Point(0, 1)
            Me.textMain.Multiline = True
            Me.textMain.Name = "textMain"
            Me.textMain.Size = New System.Drawing.Size(910, 445)
            Me.textMain.TabIndex = 2
            '
            'buttonClose
            '
            Me.buttonClose.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonClose.Location = New System.Drawing.Point(807, 453)
            Me.buttonClose.Name = "buttonClose"
            Me.buttonClose.Size = New System.Drawing.Size(103, 35)
            Me.buttonClose.TabIndex = 1
            Me.buttonClose.Text = "Close"
            '
            'buttonOK
            '
            Me.buttonOK.Location = New System.Drawing.Point(697, 453)
            Me.buttonOK.Name = "buttonOK"
            Me.buttonOK.Size = New System.Drawing.Size(103, 35)
            Me.buttonOK.TabIndex = 0
            Me.buttonOK.Text = "Save"
            '
            'FormTextViewer
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(11.0!, 24.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.CancelButton = Me.buttonClose
            Me.ClientSize = New System.Drawing.Size(919, 499)
            Me.Controls.Add(Me.buttonOK)
            Me.Controls.Add(Me.buttonClose)
            Me.Controls.Add(Me.textMain)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!)
            Me.Name = "FormTextViewer"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "Text Viewer"
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub

        'Default Instance - temporary plumbing (twhitney)
        Friend Shared ReadOnly Property GetInstance() As FormTextViewer
            Get
                If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                    SyncLock GetType(FormTextViewer)
                        If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                            m_DefaultInstance = New FormTextViewer
                        End If
                    End SyncLock
                End If
                Return m_DefaultInstance
            End Get
        End Property

        Private Shared m_DefaultInstance As FormTextViewer

#End Region

        Public Property FilePath() As String
            Get
                Return _filePath
            End Get
            Set(ByVal Value As String)
                _filePath = Value
            End Set
        End Property

        Private Sub buttonOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonOK.Click

            Try
                If changedText Then

                    'OpenAndWriteToFile()
                    ' new code to open and write contents to a text file: how many lines?
                    File.WriteAllText(FilePath, textMain.Text)
                End If
                changedText = False
            Finally
                Me.Close()
            End Try
        End Sub

        ' old code for opening, and writing to a text file...
        ' note the complexity, and number of lines
        Private Sub OpenAndWriteToFile()
            Dim sw As StreamWriter = New StreamWriter(FilePath)
            With sw
                Try
                    .Write(textMain.Text)
                Finally
                    .Close()
                End Try
            End With
        End Sub

        Private Sub textMain_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles textMain.TextChanged
            changedText = True
        End Sub

        Private Sub buttonClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonClose.Click
            Me.Close()
        End Sub

        Private Sub formTextViewer_VisibleChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.VisibleChanged
            changedText = False
        End Sub

        Private Sub formTextViewer_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
            If changedText Then
                If MsgBox("Text has been changed in the file. Are you sure you want to discard those changes?", MsgBoxStyle.YesNo, "Closing File") = MsgBoxResult.No Then
                    e.Cancel = True
                    Exit Sub
                End If
            End If
        End Sub
    End Class
End Namespace