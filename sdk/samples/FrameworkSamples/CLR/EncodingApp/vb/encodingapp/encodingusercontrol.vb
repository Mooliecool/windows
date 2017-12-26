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
Imports System.Globalization


'/ <summary>
'/ User Control created to display a string and its corresponding representation in bytes
'/ </summary>

Public Class EncodingUserControl
    Inherits System.Windows.Forms.UserControl
    'Format in which the bytes are represented - hexa or decimal
    Private byteStrFormat As String
    Private dataGroupBox As System.Windows.Forms.GroupBox
    Private stringLabel As System.Windows.Forms.Label
    Private bytesLabel As System.Windows.Forms.Label
    Private WithEvents hexadecimalRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents stringTextBox As System.Windows.Forms.TextBox
    Private WithEvents decimalRadioButton As System.Windows.Forms.RadioButton
    Private bytesTextBox As System.Windows.Forms.TextBox
    Private hexFormat As String = "X4"
    
    Private encodedByte() As Byte
    
    
    Public Property EncodedBytes() As Byte() 
        Get
            Return CType(encodedByte.Clone(), Byte())
        End Get
        Set
            encodedByte = value
        End Set
    End Property
    
    '/ <summary>
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.Container = Nothing
    
    
    
    Public Sub New(ByVal isOutputControl As Boolean) 
        ' This call is required by the Windows.Forms Form Designer.
        InitializeComponent()
        encodedByte = Nothing
        
        If isOutputControl Then
            hexFormat = "X2"
            dataGroupBox.Text = "Encoded Output"
            bytesLabel.Text = "Bytes"
        Else
            hexFormat = "X4"
            dataGroupBox.Text = "Input"
            bytesLabel.Text = "Unicode Chars"
        End If
        
        hexadecimalRadioButton.Checked = True
    
    End Sub 'New 
    
    ' Property to get/set the text in the textbox
    
    Public Overrides Property [Text]() As String 
        Get
            Return stringTextBox.Text
        End Get
        Set
            stringTextBox.Text = value
        End Set
    End Property
    
    
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
        Me.bytesLabel = New System.Windows.Forms.Label()
        Me.hexadecimalRadioButton = New System.Windows.Forms.RadioButton()
        Me.decimalRadioButton = New System.Windows.Forms.RadioButton()
        Me.bytesTextBox = New System.Windows.Forms.TextBox()
        Me.stringTextBox = New System.Windows.Forms.TextBox()
        Me.stringLabel = New System.Windows.Forms.Label()
        Me.dataGroupBox.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' dataGroupBox
        ' 
        Me.dataGroupBox.Controls.Add(Me.bytesLabel)
        Me.dataGroupBox.Controls.Add(Me.hexadecimalRadioButton)
        Me.dataGroupBox.Controls.Add(Me.decimalRadioButton)
        Me.dataGroupBox.Controls.Add(Me.bytesTextBox)
        Me.dataGroupBox.Controls.Add(Me.stringTextBox)
        Me.dataGroupBox.Controls.Add(Me.stringLabel)
        Me.dataGroupBox.Location = New System.Drawing.Point(6, 2)
        Me.dataGroupBox.Name = "dataGroupBox"
        Me.dataGroupBox.Size = New System.Drawing.Size(272, 286)
        Me.dataGroupBox.TabIndex = 0
        Me.dataGroupBox.TabStop = False
        ' 
        ' bytesLabel
        ' 
        Me.bytesLabel.Location = New System.Drawing.Point(20, 106)
        Me.bytesLabel.Name = "bytesLabel"
        Me.bytesLabel.Size = New System.Drawing.Size(213, 20)
        Me.bytesLabel.TabIndex = 0
        Me.bytesLabel.Text = "Unicode Chars"
        ' 
        ' hexadecimalRadioButton
        ' 
        Me.hexadecimalRadioButton.Location = New System.Drawing.Point(157, 241)
        Me.hexadecimalRadioButton.Name = "hexadecimalRadioButton"
        Me.hexadecimalRadioButton.Size = New System.Drawing.Size(90, 20)
        Me.hexadecimalRadioButton.TabIndex = 4
        Me.hexadecimalRadioButton.Text = "Hexadecimal"
        ' 
        ' decimalRadioButton
        ' 
        Me.decimalRadioButton.Location = New System.Drawing.Point(20, 241)
        Me.decimalRadioButton.Name = "decimalRadioButton"
        Me.decimalRadioButton.Size = New System.Drawing.Size(63, 19)
        Me.decimalRadioButton.TabIndex = 3
        Me.decimalRadioButton.Text = "Decimal"
        ' 
        ' bytesTextBox
        ' 
        Me.bytesTextBox.BackColor = System.Drawing.Color.White
        Me.bytesTextBox.Location = New System.Drawing.Point(20, 127)
        Me.bytesTextBox.Multiline = True
        Me.bytesTextBox.Name = "bytesTextBox"
        Me.bytesTextBox.ReadOnly = True
        Me.bytesTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.bytesTextBox.Size = New System.Drawing.Size(227, 99)
        Me.bytesTextBox.TabIndex = 2
        ' 
        ' stringTextBox
        ' 
        Me.stringTextBox.Location = New System.Drawing.Point(20, 39)
        Me.stringTextBox.MaxLength = 256
        Me.stringTextBox.Multiline = True
        Me.stringTextBox.Name = "stringTextBox"
        Me.stringTextBox.Size = New System.Drawing.Size(227, 41)
        Me.stringTextBox.TabIndex = 1
        ' 
        ' stringLabel
        ' 
        Me.stringLabel.Location = New System.Drawing.Point(20, 19)
        Me.stringLabel.Name = "stringLabel"
        Me.stringLabel.Size = New System.Drawing.Size(213, 20)
        Me.stringLabel.TabIndex = 5
        Me.stringLabel.Text = "String"
        ' 
        ' EncodingUserControl
        ' 
        Me.Controls.Add(dataGroupBox)
        Me.Name = "EncodingUserControl"
        Me.Size = New System.Drawing.Size(286, 299)
        Me.dataGroupBox.ResumeLayout(False)
        Me.dataGroupBox.PerformLayout()
        Me.ResumeLayout(False)
    
    End Sub 'InitializeComponent 
    #End Region
    
    
    Private Sub stringTextBox_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs)  Handles stringTextBox.TextChanged
        Dim [text] As String = CType(sender, TextBox).Text
        bytesTextBox.Text = ""
        
        If encodedByte Is Nothing Then
            Dim c As Char
            For Each c In  [text].ToCharArray()
                'Convert each char to short and print 
                bytesTextBox.Text += Convert.ToUInt16(c).ToString(byteStrFormat, CultureInfo.CurrentUICulture)
                bytesTextBox.Text += " "
            Next c
        Else
            Dim b As Byte
            For Each b In  encodedByte
                'Convert each char to bytes and print 
                bytesTextBox.Text += b.ToString(byteStrFormat, CultureInfo.CurrentUICulture)
                bytesTextBox.Text += " "
            Next b
        End If
    
    End Sub 'stringTextBox_TextChanged
    
    
    
    Private Sub radioButton_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)  Handles hexadecimalRadioButton.CheckedChanged, decimalRadioButton.CheckedChanged
        If decimalRadioButton.Checked = True Then
            byteStrFormat = "00000"
        Else
            
            byteStrFormat = hexFormat
        End If
        
        Dim [text] As String = stringTextBox.Text
        
        bytesTextBox.Text = ""
        
        If encodedByte Is Nothing Then
            Dim c As Char
            For Each c In  [text].ToCharArray()
                bytesTextBox.Text += Convert.ToUInt16(c).ToString(byteStrFormat, CultureInfo.CurrentUICulture)
                bytesTextBox.Text += " "
            Next c
        Else
            Dim b As Byte
            For Each b In  encodedByte
                'Convert each char to bytes and print 
                bytesTextBox.Text += b.ToString(byteStrFormat, CultureInfo.CurrentUICulture)
                bytesTextBox.Text += " "
            Next b
        End If
    
    End Sub 'radioButton_CheckedChanged
End Class 'EncodingUserControl