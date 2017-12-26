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
Imports Microsoft.VisualBasic
Imports System.Globalization
Imports System.Text


'/ <summary>
'/ Summary description for DecodeBytesUserControl.
'/ </summary>

Public Class DecodeBytesUserControl
    Inherits System.Windows.Forms.UserControl
    '/ <summary> 
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.IContainer = Nothing
    Private inputDecodeControl As DecodeControl
    Private outputDecodeControl As DecodeControl
    Private WithEvents decodeButton As System.Windows.Forms.Button
    
    'Callback to main form
    Private encodeForm As EncodeForm
    
    
    Public Sub New(ByVal encodeForm As EncodeForm) 
        ' This call is required by the Windows.Forms Form Designer.
        InitializeComponent()
        Me.encodeForm = encodeForm
        outputDecodeControl.Visible = False
    
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
    
    
    Private Sub decodeButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles decodeButton.Click
        Dim bytes() As String
        Dim byteString As String = inputDecodeControl.Bytes.Trim()
        
        'Encoded bytes 
        Dim encodedBytesList As New ArrayList()
        
        'Set number styles according to number base selected
        Dim o As Object = IIf(inputDecodeControl.ByteFormat(0) = "X"c, NumberStyles.HexNumber, NumberStyles.Integer)
        Dim byteFrmt As NumberStyles = CType(o, NumberStyles) 'TODO: For performance reasons this should be changed to nested IF statements

        'Separate bytes in byte string
        bytes = byteString.Split(New Char() {" "c})
        
        Try
            'Parse encoded bytes
            Dim s As String
            For Each s In  bytes
                encodedBytesList.Add(Byte.Parse(s, byteFrmt))
            Next s
            
            Dim enc As Encoding = encodeForm.EncodeInfo.CurrentEncoding
            
            'Get decoder from Encoding object
            Dim dec As Decoder = enc.GetDecoder()
            
            'Convert arraylist of bytes to byte array
            Dim encodedBytes As Byte() = CType(encodedBytesList.ToArray(GetType(Byte)), Byte())
            'Allot necessary memory by calling the GetCharCount function to obtain number of characters 
            'that the byte array will be decoded to
            Dim decodedChars(dec.GetCharCount(encodedBytes, 0, encodedBytes.Length)) As Char
            
            'Get decoded characters from the encoded byte array
            dec.GetChars(encodedBytes, 0, encodedBytes.Length, decodedChars, 0)
            
            'Alternate implementation using Convert API
            'int b = 0, c = 0;
'				bool comp = false;
'
'				//Decode the byte array
'				while (comp != true)
'				{
'					dec.Convert(encodedBytes, 0, encodedBytes.Length, decodedChars, 0, decodedChars.Length, true, out b, out c, out comp);
'				}
            
            outputDecodeControl.Text = New String(decodedChars)
            Dim outputUnicodeChars As String = ""
            Dim c As Char
            For Each c In  outputDecodeControl.Text.ToCharArray()
                'Convert each char to short and print 
                outputUnicodeChars += Convert.ToUInt16(c).ToString(outputDecodeControl.ByteFormat, CultureInfo.CurrentUICulture)
                outputUnicodeChars += " "
            Next c
            outputDecodeControl.Bytes = outputUnicodeChars
            outputDecodeControl.Visible = True
        Catch fexc As FormatException
            MessageBox.Show(fexc.Message + vbLf + "Please enter bytes in right format")
            Return
        Catch oexc As OverflowException
            MessageBox.Show(oexc.Message)
            Return
        Catch dexc As DecoderFallbackException
            MessageBox.Show(dexc.Message)
        End Try
    
    End Sub 'decodeButton_Click
    
    #Region "Component Designer generated code"
    
    '/ <summary> 
    '/ Required method for Designer support - do not modify 
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent() 
        Me.decodeButton = New System.Windows.Forms.Button()
        Me.inputDecodeControl = New DecodeControl(False)
        Me.outputDecodeControl = New DecodeControl(True)
        Me.SuspendLayout()
        ' 
        ' inputDecodeControl
        ' 
        Me.inputDecodeControl.Bytes = ""
        Me.inputDecodeControl.Location = New System.Drawing.Point(0, 0)
        Me.inputDecodeControl.Name = "inputDecodeControl"
        Me.inputDecodeControl.Size = New System.Drawing.Size(288, 344)
        Me.inputDecodeControl.TabIndex = 0
        ' 
        ' outputDecodeControl
        ' 
        Me.outputDecodeControl.Bytes = ""
        Me.outputDecodeControl.Location = New System.Drawing.Point(289, 0)
        Me.outputDecodeControl.Name = "outputDecodeControl"
        Me.outputDecodeControl.Size = New System.Drawing.Size(288, 344)
        Me.outputDecodeControl.TabIndex = 1
        ' 
        ' decodeButton
        ' 
        Me.decodeButton.Location = New System.Drawing.Point(121, 339)
        Me.decodeButton.Name = "decodeButton"
        Me.decodeButton.TabIndex = 2
        Me.decodeButton.Text = "Decode"
        ' 
        ' DecodeBytesUserControl
        ' 
        Me.Controls.Add(decodeButton)
        Me.Controls.Add(outputDecodeControl)
        Me.Controls.Add(inputDecodeControl)
        Me.Name = "DecodeBytesUserControl"
        Me.Size = New System.Drawing.Size(600, 404)
        Me.ResumeLayout(False)
    
    End Sub 'InitializeComponent 
    #End Region
End Class 'DecodeBytesUserControl