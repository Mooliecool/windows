'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
Imports System.Collections
Imports System.ComponentModel
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Text
Imports Microsoft.VisualBasic
Imports System.Globalization



'/ <summary>
'/ UI for decoding string
'/ </summary>

Public Class DecodeControl
    Inherits System.Windows.Forms.UserControl
    Private dataGroupBox As System.Windows.Forms.GroupBox
    Private bytesLabel As System.Windows.Forms.Label
    Private hexadecimalRadioButton As System.Windows.Forms.RadioButton
    Private decimalRadioButton As System.Windows.Forms.RadioButton
    Private bytesTextBox As System.Windows.Forms.TextBox
    Private stringTextBox As System.Windows.Forms.TextBox
    Private stringLabel As System.Windows.Forms.Label
    Private byteStrFormat As String

    ' Property to get/set the text in the textbox

    Public Overrides Property [Text]() As String
        Get
            Return stringTextBox.Text
        End Get
        Set(ByVal Value As String)
            stringTextBox.Text = value
        End Set
    End Property


    Public Property Bytes() As String
        Get
            Return bytesTextBox.Text
        End Get
        Set(ByVal Value As String)
            bytesTextBox.Text = value
        End Set
    End Property


    Public ReadOnly Property ByteFormat() As String
        Get
            Return byteStrFormat
        End Get
    End Property

    '/ <summary>
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.Container = Nothing


    '/ <summary>
    '/ Clean up any resources being used.
    '/ </summary>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)

    End Sub 'Dispose

#Region "Component Designer generated code"

    '/ <summary> 
    '/ Required method for Designer support - do not modify 
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent()
        Me.dataGroupBox = New System.Windows.Forms.GroupBox()
        Me.stringTextBox = New System.Windows.Forms.TextBox()
        Me.stringLabel = New System.Windows.Forms.Label()
        Me.bytesLabel = New System.Windows.Forms.Label()
        Me.hexadecimalRadioButton = New System.Windows.Forms.RadioButton()
        Me.decimalRadioButton = New System.Windows.Forms.RadioButton()
        Me.bytesTextBox = New System.Windows.Forms.TextBox()
        Me.dataGroupBox.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' dataGroupBox
        ' 
        Me.dataGroupBox.Controls.Add(Me.stringTextBox)
        Me.dataGroupBox.Controls.Add(Me.stringLabel)
        Me.dataGroupBox.Controls.Add(Me.bytesLabel)
        Me.dataGroupBox.Controls.Add(Me.hexadecimalRadioButton)
        Me.dataGroupBox.Controls.Add(Me.decimalRadioButton)
        Me.dataGroupBox.Controls.Add(Me.bytesTextBox)
        Me.dataGroupBox.Location = New System.Drawing.Point(15, 8)
        Me.dataGroupBox.Name = "dataGroupBox"
        Me.dataGroupBox.Size = New System.Drawing.Size(272, 284)
        Me.dataGroupBox.TabIndex = 0
        Me.dataGroupBox.TabStop = False
        Me.dataGroupBox.Text = "Decode"
        ' 
        ' stringTextBox
        ' 
        Me.stringTextBox.Location = New System.Drawing.Point(23, 220)
        Me.stringTextBox.MaxLength = 256
        Me.stringTextBox.Multiline = True
        Me.stringTextBox.Name = "stringTextBox"
        Me.stringTextBox.Size = New System.Drawing.Size(227, 41)
        Me.stringTextBox.TabIndex = 6
        Me.stringTextBox.ReadOnly = True
        Me.stringTextBox.BackColor = Color.White
        ' 
        ' stringLabel
        ' 
        Me.stringLabel.Location = New System.Drawing.Point(23, 200)
        Me.stringLabel.Name = "stringLabel"
        Me.stringLabel.Size = New System.Drawing.Size(213, 20)
        Me.stringLabel.TabIndex = 7
        Me.stringLabel.Text = "String"
        ' 
        ' bytesLabel
        ' 
        Me.bytesLabel.Location = New System.Drawing.Point(23, 25)
        Me.bytesLabel.Name = "bytesLabel"
        Me.bytesLabel.Size = New System.Drawing.Size(213, 20)
        Me.bytesLabel.TabIndex = 0
        Me.bytesLabel.Text = "Bytes"
        ' 
        ' hexadecimalRadioButton
        ' 
        Me.hexadecimalRadioButton.Location = New System.Drawing.Point(160, 160)
        Me.hexadecimalRadioButton.Name = "hexadecimalRadioButton"
        Me.hexadecimalRadioButton.Size = New System.Drawing.Size(90, 20)
        Me.hexadecimalRadioButton.TabIndex = 4
        Me.hexadecimalRadioButton.Text = "Hexadecimal"
        AddHandler Me.hexadecimalRadioButton.CheckedChanged, AddressOf radioButton_CheckedChanged
        ' 
        ' decimalRadioButton
        ' 
        Me.decimalRadioButton.Location = New System.Drawing.Point(23, 160)
        Me.decimalRadioButton.Name = "decimalRadioButton"
        Me.decimalRadioButton.Size = New System.Drawing.Size(63, 19)
        Me.decimalRadioButton.TabIndex = 3
        Me.decimalRadioButton.Text = "Decimal"
        AddHandler Me.decimalRadioButton.CheckedChanged, AddressOf radioButton_CheckedChanged
        ' 
        ' bytesTextBox
        ' 
        Me.bytesTextBox.BackColor = System.Drawing.Color.White
        Me.bytesTextBox.Location = New System.Drawing.Point(23, 46)
        Me.bytesTextBox.Multiline = True
        Me.bytesTextBox.Name = "bytesTextBox"
        Me.bytesTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.bytesTextBox.Size = New System.Drawing.Size(227, 99)
        Me.bytesTextBox.TabIndex = 2
        ' 
        ' EncodingUserControl
        ' 
        Me.Controls.Add(dataGroupBox)
        Me.Name = "EncodingUserControl"
        Me.Size = New System.Drawing.Size(300, 344)
        Me.dataGroupBox.ResumeLayout(False)
        Me.dataGroupBox.PerformLayout()
        Me.ResumeLayout(False)

    End Sub 'InitializeComponent
#End Region


    Public Sub New(ByVal isOutputControl As Boolean)
        ' This call is required by the Windows.Forms Form Designer.
        InitializeComponent()
        hexadecimalRadioButton.Checked = True

        If isOutputControl Then
            dataGroupBox.Text = "Decoded Output"
            bytesLabel.Text = "Unicode Chars"
        Else
            dataGroupBox.Text = "Input"
            bytesLabel.Text = "Bytes"
            stringTextBox.Visible = False
            stringLabel.Visible = False
        End If

    End Sub 'New


    Private Sub radioButton_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        If decimalRadioButton.Checked = True Then
            byteStrFormat = "00000"
        Else
            byteStrFormat = "X4"
        End If

        If stringTextBox.Visible Then
            Dim outputUnicodeChars As String = String.Empty
            Dim c As Char
            For Each c In stringTextBox.Text.ToCharArray()
                'Convert each char to short and print 
                outputUnicodeChars += Convert.ToUInt16(c).ToString(byteStrFormat, CultureInfo.CurrentUICulture)
                outputUnicodeChars += " "
            Next c
            bytesTextBox.Text = outputUnicodeChars
        End If

    End Sub 'radioButton_CheckedChanged 
End Class 'DecodeControl