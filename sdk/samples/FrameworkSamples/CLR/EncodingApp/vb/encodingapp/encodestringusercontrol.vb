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


'/ <summary>
'/ User control encapsulating the controls for encoding a string
'/ </summary>

Public Class EncodeStringUserControl
    Inherits System.Windows.Forms.UserControl
    Private inputControl As EncodingUserControl
    
    Private outputControl As EncodingUserControl
    
    Private WithEvents encodeButton As System.Windows.Forms.Button
    
    Private components As System.ComponentModel.Container = Nothing
    
    'Callback to main form
    Private encodeForm As EncodeForm
    
    
    Public Sub New(ByVal encodeForm As EncodeForm) 
        ' This call is required by the Windows.Forms Form Designer.
        InitializeComponent()
        Me.encodeForm = encodeForm
    
    End Sub 'New
    
    
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
        Me.inputControl = New EncodingUserControl(False)
        Me.outputControl = New EncodingUserControl(True)
        Me.encodeButton = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        ' 
        ' inputControl
        ' 
        Me.inputControl.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.inputControl.Location = New System.Drawing.Point(15, 8)
        Me.inputControl.Name = "inputControl"
        Me.inputControl.Size = New System.Drawing.Size(285, 296)
        Me.inputControl.TabIndex = 0
        ' 
        ' encodeButton
        ' 
        Me.encodeButton.BackColor = System.Drawing.SystemColors.ButtonFace
        Me.encodeButton.Location = New System.Drawing.Point(101, 325)
        Me.encodeButton.Name = "encodeButton"
        Me.encodeButton.TabIndex = 1
        Me.encodeButton.Text = "Encode"
        ' 
        ' outputControl
        ' 
        Me.outputControl.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.outputControl.Location = New System.Drawing.Point(298, 8)
        Me.outputControl.Name = "outputControl"
        Me.outputControl.Size = New System.Drawing.Size(285, 296)
        Me.outputControl.TabIndex = 2
        ' 
        ' EncodeStringUserControl
        ' 
        Me.Controls.Add(encodeButton)
        Me.Controls.Add(inputControl)
        Me.Controls.Add(outputControl)
        Me.Name = "EncodeStringUserControl"
        Me.Size = New System.Drawing.Size(596, 418)
        Me.ResumeLayout(False)
    
    End Sub 'InitializeComponent 
    #End Region
    
    
    Private Sub okButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles encodeButton.Click
        Try
            ' Receive encoding from the encoderInfo object in the main form
            Dim dstEncoding As Encoding = encodeForm.EncodeInfo.CurrentEncoding
            'Normalize string according to normalization form chosen
            Dim inputString As String = encodeForm.EncodeInfo.GetNormalizedString(inputControl.Text)
            Dim encodedString As String = ""
            
            'GetBytes returns the byte array after encoding
            Dim encodedBytes As Byte() = dstEncoding.GetBytes(inputString)
            
            Dim b As Byte
            For Each b In  encodedBytes
                If b < 32 Then
                    'add a space to show the empty byte
                    encodedString += " "
                Else
                    encodedString += ChrW(b)
                End If 'outputControl.EncodedBytes.Add(b);
            Next b
            
            outputControl.EncodedBytes = encodedBytes
            outputControl.Visible = True
            outputControl.Text = encodedString
        Catch exc As EncoderFallbackException
            MessageBox.Show("EncoderFallbackException:" + vbLf + exc.Message)
        End Try
    
    End Sub 'okButton_Click
    
    
    Private Sub EncodeStringUserControl_Load(ByVal sender As Object, ByVal e As System.EventArgs)  Handles MyBase.Load
        encodeButton.Visible = True
        outputControl.Visible = False
    
    End Sub 'EncodeStringUserControl_Load
End Class 'EncodeStringUserControl