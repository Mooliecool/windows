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
Imports System.IO
Imports System.Diagnostics
Imports System.Globalization


'/ <summary>
'/ User control to encapsulate file encode UI
'/ </summary>

Public Class EncodeFileUserControl
    Inherits System.Windows.Forms.UserControl
    Private WithEvents browseButton As System.Windows.Forms.Button
    Private fileNameTextBox As System.Windows.Forms.TextBox
    Private openFileDialog As System.Windows.Forms.OpenFileDialog
    Private WithEvents encodeButton As System.Windows.Forms.Button
    Private openFilePanel As System.Windows.Forms.GroupBox
    Private encodeFileGroupBox As System.Windows.Forms.GroupBox
    Private captionLabel As System.Windows.Forms.Label
    Private WithEvents outputLinkLabel As System.Windows.Forms.LinkLabel
    Private decodeComboBox As System.Windows.Forms.ComboBox
    Private decodeLabel As System.Windows.Forms.Label
    Private intermediateLabel As System.Windows.Forms.Label
    Private WithEvents intermediateLinkLabel As System.Windows.Forms.LinkLabel
    Private decoding As Encoding
    Private encodings() As EncodingInfo
    
    'Callback to main form
    Private encodeForm As EncodeForm
    
    '/ <summary> 
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.Container = Nothing
    
    
    Public Sub New(ByVal encodeForm As EncodeForm) 
        ' This call is required by the Windows.Forms Form Designer.
        InitializeComponent()
        
        'Initialize callback 
        Me.encodeForm = encodeForm
        Me.decoding = Encoding.Default
    
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
        Me.browseButton = New System.Windows.Forms.Button()
        Me.fileNameTextBox = New System.Windows.Forms.TextBox()
        Me.openFileDialog = New System.Windows.Forms.OpenFileDialog()
        Me.encodeButton = New System.Windows.Forms.Button()
        Me.openFilePanel = New System.Windows.Forms.GroupBox()
        Me.encodeFileGroupBox = New System.Windows.Forms.GroupBox()
        Me.outputLinkLabel = New System.Windows.Forms.LinkLabel()
        Me.captionLabel = New System.Windows.Forms.Label()
        Me.intermediateLabel = New System.Windows.Forms.Label()
        Me.intermediateLinkLabel = New System.Windows.Forms.LinkLabel()
        Me.decodeComboBox = New ComboBox()
        Me.decodeLabel = New Label()
        Me.openFilePanel.SuspendLayout()
        Me.encodeFileGroupBox.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' browseButton
        ' 
        Me.browseButton.BackColor = System.Drawing.SystemColors.ButtonFace
        Me.browseButton.Location = New System.Drawing.Point(255, 19)
        Me.browseButton.Name = "browseButton"
        Me.browseButton.Size = New System.Drawing.Size(60, 23)
        Me.browseButton.TabIndex = 19
        Me.browseButton.Text = "Browse..."
        ' 
        ' fileNameTextBox
        ' 
        Me.fileNameTextBox.Location = New System.Drawing.Point(55, 48)
        Me.fileNameTextBox.Name = "fileNameTextBox"
        Me.fileNameTextBox.Size = New System.Drawing.Size(225, 20)
        Me.fileNameTextBox.TabIndex = 18
        ' 
        ' encodeButton
        ' 
        Me.encodeButton.BackColor = System.Drawing.SystemColors.ButtonFace
        Me.encodeButton.Location = New System.Drawing.Point(24, 144)
        Me.encodeButton.Name = "encodeButton"
        Me.encodeButton.Size = New System.Drawing.Size(90, 25)
        Me.encodeButton.TabIndex = 20
        Me.encodeButton.Text = "Encode"
        ' 
        ' openFilePanel
        ' 
        'this.openFilePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
        Me.openFilePanel.Controls.Add(Me.browseButton)
        Me.openFilePanel.Location = New System.Drawing.Point(39, 27)
        Me.openFilePanel.Name = "openFilePanel"
        Me.openFilePanel.Size = New System.Drawing.Size(329, 55)
        Me.openFilePanel.TabIndex = 21
        ' 
        ' encodeFileGroupBox
        ' 
        Me.encodeFileGroupBox.Controls.Add(Me.outputLinkLabel)
        Me.encodeFileGroupBox.Controls.Add(Me.captionLabel)
        Me.encodeFileGroupBox.Controls.Add(Me.encodeButton)
        Me.encodeFileGroupBox.Controls.Add(Me.decodeLabel)
        Me.encodeFileGroupBox.Controls.Add(Me.decodeComboBox)
        Me.encodeFileGroupBox.Controls.Add(Me.intermediateLabel)
        Me.encodeFileGroupBox.Controls.Add(Me.intermediateLinkLabel)
        Me.encodeFileGroupBox.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.encodeFileGroupBox.Location = New System.Drawing.Point(15, 8)
        Me.encodeFileGroupBox.Name = "encodeFileGroupBox"
        Me.encodeFileGroupBox.Size = New System.Drawing.Size(383, 380)
        Me.encodeFileGroupBox.TabIndex = 24
        Me.encodeFileGroupBox.TabStop = False
        Me.encodeFileGroupBox.Text = "Encode file"
        ' 
        ' outputLinkLabel
        ' 
        Me.outputLinkLabel.Links.Add(New System.Windows.Forms.LinkLabel.Link(0, 0))
        Me.outputLinkLabel.Location = New System.Drawing.Point(17, 200)
        Me.outputLinkLabel.Name = "outputLinkLabel"
        Me.outputLinkLabel.Size = New System.Drawing.Size(336, 50)
        Me.outputLinkLabel.TabIndex = 22
        ' 
        ' intermediateLinkLabel
        ' 
        Me.intermediateLinkLabel.Links.Add(New System.Windows.Forms.LinkLabel.Link(0, 0))
        Me.intermediateLinkLabel.Location = New System.Drawing.Point(17, 272)
        Me.intermediateLinkLabel.Name = "intermediateLinkLabel"
        Me.intermediateLinkLabel.Size = New System.Drawing.Size(336, 50)
        Me.intermediateLinkLabel.TabIndex = 53
        ' 
        ' captionLabel
        ' 
        Me.captionLabel.Location = New System.Drawing.Point(17, 180)
        Me.captionLabel.Name = "captionLabel"
        Me.captionLabel.Size = New System.Drawing.Size(300, 20)
        Me.captionLabel.TabIndex = 21
        Me.captionLabel.Text = "The encoded file has been saved at the location: "
        ' 
        ' intermediateLabel
        ' 
        Me.intermediateLabel.Location = New System.Drawing.Point(17, 252)
        Me.intermediateLabel.Name = "intermediateLabel"
        Me.intermediateLabel.Size = New System.Drawing.Size(300, 20)
        Me.intermediateLabel.TabIndex = 51
        Me.intermediateLabel.Text = "The intermediate file has been saved at the location: "
        ' 
        ' decodeLabel
        ' 
        Me.decodeLabel.Location = New System.Drawing.Point(20, 100)
        Me.decodeLabel.Name = "decodeLabel"
        Me.decodeLabel.Size = New System.Drawing.Size(120, 20)
        Me.decodeLabel.TabIndex = 21
        Me.decodeLabel.Text = "Input file's encoding"
        ' 
        ' decodeComboBox
        ' 
        Me.decodeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.decodeComboBox.FormattingEnabled = True
        Me.decodeComboBox.Location = New System.Drawing.Point(150, 95)
        Me.decodeComboBox.Name = "decodeComboBox"
        Me.decodeComboBox.Size = New System.Drawing.Size(202, 21)
        Me.decodeComboBox.TabIndex = 25
        ' 
        ' EncodeFileUserControl
        ' 
        Me.Controls.Add(fileNameTextBox)
        Me.Controls.Add(openFilePanel)
        Me.Controls.Add(encodeFileGroupBox)
        Me.Name = "EncodeFileUserControl"
        Me.Size = New System.Drawing.Size(409, 400)
        Me.openFilePanel.ResumeLayout(False)
        Me.encodeFileGroupBox.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()
    
    End Sub 'InitializeComponent
    #End Region
    
    
    'File is decoded into an intermediate file with the encoding picked off the dropdown or
    'default setting. Anyhow, this process becomes considerably easier using a StreamReader where
    'the decoding can be passed as a parameter. Same with StreamWriter - it has a parameter using
    'which target encoding can be passed. The following code is to illustrate the usage of the 
    'Convert API
    Private Function DecodeFile() As String 
        'Check if file name has been entered
        If fileNameTextBox.Text = "" Then
            MessageBox.Show("Enter file name")
            Return Nothing
        End If
        'Check if file exists
        If Not File.Exists(fileNameTextBox.Text) Then
            MessageBox.Show("File does not exist. Please try again!")
            'Clear textbox
            fileNameTextBox.Text = ""
            Return Nothing
        End If
        Dim fileWriter As StreamWriter = Nothing
        Try
            'Extract input filename and add prefix of "Intermediate_" to filename
            Dim intermediateFileName As String = Path.GetDirectoryName(fileNameTextBox.Text) + Path.DirectorySeparatorChar + "Intermediate_" + Path.GetFileName(fileNameTextBox.Text)
            
            fileWriter = New StreamWriter(intermediateFileName, False, Encoding.Unicode, 512)
            
            Dim fileReader As New BinaryReader(New FileStream(fileNameTextBox.Text, FileMode.Open))
            Try
                'Instantiate decoder from the encoding selected
                Dim dstDecoder As Decoder = decoding.GetDecoder()
                'buffers
                Dim dataBlock(1023) As Byte
                Dim decodedData(511) As Char
                'Read the next block spanning 1024 chars
                Dim bytesRead As Integer = 0
                While True
                    bytesRead = fileReader.Read(dataBlock, 0, dataBlock.Length)
                    If bytesRead <= 0 Then
                        Exit While
                    End If

                    'Number of bytes used in decoding and number of characters used
                    Dim bytesUsed As Integer = 0
                    Dim charsUsed As Integer = 0
                    Dim byteStart As Integer = 0
                    Dim completed As Boolean = False

                    While Not completed
                        'Decode the data block 
                        dstDecoder.Convert(dataBlock, byteStart, bytesRead - byteStart, decodedData, 0, 512, True, bytesUsed, charsUsed, completed)
                        byteStart += bytesUsed
                        'Write decoded bytes into the intermediate file
                        fileWriter.Write(decodedData, 0, bytesUsed)
                    End While
                End While
            Finally
                fileReader.Close()
            End Try
            Return intermediateFileName
        Catch ioExc As IOException
            MessageBox.Show("IOException:" + vbLf + ioExc.Message)
            Return Nothing
        Catch fbExc As EncoderFallbackException
            MessageBox.Show("EncoderFallbackException:" + vbLf + fbExc.Message)
            Return Nothing
        Catch aExc As ArgumentException
            MessageBox.Show(aExc.Message)
            Return Nothing
        Finally
            fileWriter.Close()
        End Try
    
    End Function 'DecodeFile
    
    
    Private Sub encodeButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles encodeButton.Click
        Dim decodedFileName As String = DecodeFile()
        If decodedFileName Is Nothing Then
            Return
        End If 
        Dim fileWriter As FileStream = Nothing
        Try
            'Extract input filename and add prefix of "Encoded_" to filename
            Dim outputFileName As String = Path.GetDirectoryName(fileNameTextBox.Text) + Path.DirectorySeparatorChar + "Encoded_" + Path.GetFileName(fileNameTextBox.Text)
            
            fileWriter = New FileStream(outputFileName, FileMode.Create)
            
            Dim fileReader As New StreamReader(decodedFileName)
            Try
                'Get encoding from encoderInfo object
                Dim dstEncoding As Encoding = encodeForm.EncodeInfo.CurrentEncoding
                
                'Instantiate encoder from the encoding selected
                Dim dstEncoder As Encoder = dstEncoding.GetEncoder()
                
                'buffers
                Dim dataBlock(1023) As Char
                Dim encodedData(511) As Byte
                
                'Read the next block spanning 1024 chars
                Dim charsRead As Integer = 0
                While True
                    charsRead = fileReader.Read(dataBlock, 0, dataBlock.Length)
                    If charsRead <= 0 Then
                        Exit While
                    End If
                    'Number of characters used in encoding and number of bytes used
                    Dim charsUsed As Integer = 0
                    Dim bytesUsed As Integer = 0
                    Dim charStart As Integer = 0
                    Dim completed As Boolean = False

                    While Not completed
                        'Encode the data block 
                        dstEncoder.Convert(dataBlock, charStart, charsRead - charStart, encodedData, 0, 512, True, charsUsed, bytesUsed, completed)
                        charStart += charsUsed
                        'Write encoded bytes into the output file
                        fileWriter.Write(encodedData, 0, bytesUsed)
                    End While
                End While
            Finally
                fileReader.Close()
            End Try
            
            'Display the intermediate file name
            captionLabel.Visible = True
            outputLinkLabel.Visible = True
            intermediateLabel.Visible = True
            intermediateLinkLabel.Visible = True
            
            'Display the output file name
            outputLinkLabel.Text = outputFileName
            outputLinkLabel.Links.Clear()
            outputLinkLabel.Links.Add(0, outputFileName.Length)
            intermediateLinkLabel.Text = decodedFileName
            intermediateLinkLabel.Links.Clear()
            intermediateLinkLabel.Links.Add(0, decodedFileName.Length)
        Catch ioExc As IOException
            MessageBox.Show("IOException:" + vbLf + ioExc.Message)
        Catch fbExc As EncoderFallbackException
            MessageBox.Show("EncoderFallbackException:" + vbLf + fbExc.Message)
        Catch aExc As ArgumentException
            MessageBox.Show(aExc.Message)
        Finally
            fileWriter.Close()
        End Try
    
    End Sub 'encodeButton_Click
    
    
    'Open file specified
    Private Sub browseButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles browseButton.Click
        captionLabel.Visible = False
        outputLinkLabel.Visible = False
        intermediateLabel.Visible = False
        intermediateLinkLabel.Visible = False
        
        openFileDialog.ShowDialog()
        fileNameTextBox.Text = openFileDialog.FileName
    
    End Sub 'browseButton_Click
    
    
    'Called at startup
    Private Sub EncodeFileUserControl_Load(ByVal sender As Object, ByVal e As System.EventArgs)  Handles MyBase.Load
        captionLabel.Visible = False
        outputLinkLabel.Visible = False
        intermediateLabel.Visible = False
        intermediateLinkLabel.Visible = False
        
        'Initialize encoding schemes
        encodings = Encoding.GetEncodings()
        Dim encList As New ArrayList(encodings)
        Dim encComp As EncodingSortClass = New EncodingSortClass()
        encList.Sort(encComp)
        encodings = CType(encList.ToArray(GetType(EncodingInfo)), EncodingInfo())
        

        Dim ei As EncodingInfo
        For Each ei In  encodings
            decodeComboBox.Items.Add(ei.DisplayName)
        Next ei
        decodeComboBox.Items.Insert(0, "Default")
        
        decodeComboBox.SelectedIndex = 0
        AddHandler Me.decodeComboBox.SelectedIndexChanged, AddressOf Me.decodeComboBox_SelectedIndexChanged
    
    End Sub 'EncodeFileUserControl_Load
    
    
    Private Sub decodeComboBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) 
        If CType(sender, ComboBox).SelectedIndex <= 0 Then
            decoding = Encoding.Default
        Else
            decoding = Encoding.GetEncoding(encodings((CType(sender, ComboBox).SelectedIndex - 1)).CodePage)
        End If
    
    End Sub 'decodeComboBox_SelectedIndexChanged
     
    Private Sub intermediateLinkLabel_LinkClicked(ByVal sender As Object, ByVal e As System.Windows.Forms.LinkLabelLinkClickedEventArgs)  Handles intermediateLinkLabel.LinkClicked
        Try
            'Create a new process to open editor to display intermediate file
            Dim openFile As New Process()
            openFile.StartInfo.FileName = intermediateLinkLabel.Text
            openFile.StartInfo.RedirectStandardOutput = False
            openFile.StartInfo.UseShellExecute = True
            openFile.Start()
        Catch exc As Exception
            MessageBox.Show("Error trying to open file!" + vbLf + exc.Message)
        End Try
    
    End Sub 'intermediateLinkLabel_LinkClicked
    
    
    Private Sub outputLinkLabel_LinkClicked(ByVal sender As Object, ByVal e As System.Windows.Forms.LinkLabelLinkClickedEventArgs)  Handles outputLinkLabel.LinkClicked
        Try
            'Create a new process to open editor to display output file
            Dim openFile As New Process()
            openFile.StartInfo.FileName = outputLinkLabel.Text
            openFile.StartInfo.RedirectStandardOutput = False
            openFile.StartInfo.UseShellExecute = True
            openFile.Start()
        Catch exc As Exception
            MessageBox.Show("Error trying to open file!" + vbLf + exc.Message)
        End Try
    
    End Sub 'outputLinkLabel_LinkClicked
End Class 'EncodeFileUserControl