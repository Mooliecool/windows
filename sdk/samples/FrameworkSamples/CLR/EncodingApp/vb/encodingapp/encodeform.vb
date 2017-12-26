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
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Text
Imports System.Globalization


'/ <summary>
'/ Main form to display the UI
'/ </summary>

Public Class EncodeForm
    Inherits System.Windows.Forms.Form
    Private optionsPanel As System.Windows.Forms.Panel
    Private WithEvents encodingLabel As System.Windows.Forms.Label
    Private WithEvents stringLabel As System.Windows.Forms.Label
    Private WithEvents decodeStringLabel As System.Windows.Forms.Label
    Private WithEvents fileLabel As System.Windows.Forms.Label
    Private fallBackLabel As System.Windows.Forms.Label
    Private encodingSchemeComboBox As System.Windows.Forms.ComboBox
    Private WithEvents normalizationComboBox As System.Windows.Forms.ComboBox
    Private WithEvents fallbackSchemeComboBox As System.Windows.Forms.ComboBox
    Private normalizationLabel As System.Windows.Forms.Label
    Private encdingLabel As System.Windows.Forms.Label
    
    'Variable to contain encoding options
    Private encInfo As EncoderInfo
    
    'User controls displaying string encode and file encode
    Private stringControl As EncodeStringUserControl
    Private fileControl As EncodeFileUserControl
    Private decodeStringControl As DecodeBytesUserControl
    Private descriptionPanel As System.Windows.Forms.Panel
    Private descFallbacklabel As System.Windows.Forms.Label
    Private descNormalizationlabel As System.Windows.Forms.Label
    Private titleNormalizationLabel As System.Windows.Forms.Label
    Private titleFallbackLabel As System.Windows.Forms.Label
    Private encDescLabel As System.Windows.Forms.Label
    Private titleLabel As System.Windows.Forms.Label
    
    '/ <summary>
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.Container = Nothing
    
    
    Public Sub New() 
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
        
        'Initialize user controls
        InitializeUserControls()
    
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
    
    #Region "Windows Form Designer generated code"
    
    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent() 
        Me.optionsPanel = New System.Windows.Forms.Panel()
        Me.encodingSchemeComboBox = New System.Windows.Forms.ComboBox()
        Me.normalizationComboBox = New System.Windows.Forms.ComboBox()
        Me.normalizationLabel = New System.Windows.Forms.Label()
        Me.fallbackSchemeComboBox = New System.Windows.Forms.ComboBox()
        Me.fallBackLabel = New System.Windows.Forms.Label()
        Me.encdingLabel = New System.Windows.Forms.Label()
        Me.fileLabel = New System.Windows.Forms.Label()
        Me.stringLabel = New System.Windows.Forms.Label()
        Me.decodeStringLabel = New System.Windows.Forms.Label()
        Me.encodingLabel = New System.Windows.Forms.Label()
        Me.descriptionPanel = New System.Windows.Forms.Panel()
        Me.descFallbacklabel = New System.Windows.Forms.Label()
        Me.descNormalizationlabel = New System.Windows.Forms.Label()
        Me.titleNormalizationLabel = New System.Windows.Forms.Label()
        Me.titleFallbackLabel = New System.Windows.Forms.Label()
        Me.encDescLabel = New System.Windows.Forms.Label()
        Me.titleLabel = New System.Windows.Forms.Label()
        Me.optionsPanel.SuspendLayout()
        Me.descriptionPanel.SuspendLayout()
        Me.SuspendLayout()
        
        ' 
        ' encodingSchemeComboBox
        ' 
        Me.encodingSchemeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.encodingSchemeComboBox.FormattingEnabled = True
        Me.encodingSchemeComboBox.Location = New System.Drawing.Point(14, 155)
        Me.encodingSchemeComboBox.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
        Me.encodingSchemeComboBox.Name = "encodingSchemeComboBox"
        Me.encodingSchemeComboBox.Size = New System.Drawing.Size(182, 21)
        Me.encodingSchemeComboBox.TabIndex = 25
        ' 
        ' normalizationComboBox
        ' 
        Me.normalizationComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.normalizationComboBox.FormattingEnabled = True
        Me.normalizationComboBox.Location = New System.Drawing.Point(14, 261)
        Me.normalizationComboBox.Margin = New System.Windows.Forms.Padding(3, 0, 3, 3)
        Me.normalizationComboBox.Name = "normalizationComboBox"
        Me.normalizationComboBox.Size = New System.Drawing.Size(182, 21)
        Me.normalizationComboBox.TabIndex = 27
        ' 
        ' normalizationLabel
        ' 
        Me.normalizationLabel.Location = New System.Drawing.Point(14, 247)
        Me.normalizationLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.normalizationLabel.Name = "normalizationLabel"
        Me.normalizationLabel.Size = New System.Drawing.Size(143, 13)
        Me.normalizationLabel.TabIndex = 30
        Me.normalizationLabel.Text = "Normalization:"
        ' 
        ' fallbackSchemeComboBox
        ' 
        Me.fallbackSchemeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.fallbackSchemeComboBox.FormattingEnabled = True
        Me.fallbackSchemeComboBox.Location = New System.Drawing.Point(14, 208)
        Me.fallbackSchemeComboBox.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
        Me.fallbackSchemeComboBox.Name = "fallbackSchemeComboBox"
        Me.fallbackSchemeComboBox.Size = New System.Drawing.Size(182, 21)
        Me.fallbackSchemeComboBox.TabIndex = 26
        ' 
        ' fallBackLabel
        ' 
        Me.fallBackLabel.Location = New System.Drawing.Point(14, 193)
        Me.fallBackLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.fallBackLabel.Name = "fallBackLabel"
        Me.fallBackLabel.Size = New System.Drawing.Size(143, 13)
        Me.fallBackLabel.TabIndex = 29
        Me.fallBackLabel.Text = "FallBack:"
        ' 
        ' encdingLabel
        ' 
        Me.encdingLabel.Location = New System.Drawing.Point(14, 139)
        Me.encdingLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 2)
        Me.encdingLabel.Name = "encdingLabel"
        Me.encdingLabel.Size = New System.Drawing.Size(143, 13)
        Me.encdingLabel.TabIndex = 28
        Me.encdingLabel.Text = "Encoding:"
        ' 
        ' fileLabel
        ' 
        Me.fileLabel.Cursor = System.Windows.Forms.Cursors.Hand
        Me.fileLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.fileLabel.ForeColor = System.Drawing.SystemColors.ControlText
        Me.fileLabel.Location = New System.Drawing.Point(14, 90)
        Me.fileLabel.Name = "fileLabel"
        Me.fileLabel.Size = New System.Drawing.Size(113, 18)
        Me.fileLabel.TabIndex = 2
        Me.fileLabel.Text = "Encode File"
        ' 
        ' stringLabel
        ' 
        Me.stringLabel.Cursor = System.Windows.Forms.Cursors.Hand
        Me.stringLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.stringLabel.ForeColor = System.Drawing.SystemColors.ControlText
        Me.stringLabel.Location = New System.Drawing.Point(14, 70)
        Me.stringLabel.Name = "stringLabel"
        Me.stringLabel.Size = New System.Drawing.Size(113, 18)
        Me.stringLabel.TabIndex = 1
        Me.stringLabel.Text = "Encode String"
        ' 
        ' decodeStringLabel
        ' 
        Me.decodeStringLabel.Cursor = System.Windows.Forms.Cursors.Hand
        Me.decodeStringLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.decodeStringLabel.ForeColor = System.Drawing.SystemColors.ControlText
        Me.decodeStringLabel.Location = New System.Drawing.Point(14, 50)
        Me.decodeStringLabel.Name = "decodeStringLabel"
        Me.decodeStringLabel.Size = New System.Drawing.Size(113, 18)
        Me.decodeStringLabel.TabIndex = 3
        Me.decodeStringLabel.Text = "Decode Bytes"
        ' 
        ' encodingLabel
        ' 
        Me.encodingLabel.Cursor = System.Windows.Forms.Cursors.Hand
        Me.encodingLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.encodingLabel.ForeColor = System.Drawing.Color.FromArgb(Fix(System.Convert.ToByte(64)), Fix(System.Convert.ToByte(0)), Fix(System.Convert.ToByte(64)))
        Me.encodingLabel.Location = New System.Drawing.Point(4, 4)
        Me.encodingLabel.Name = "encodingLabel"
        Me.encodingLabel.Size = New System.Drawing.Size(109, 28)
        Me.encodingLabel.TabIndex = 0
        Me.encodingLabel.Text = "Encoding"
        ' 
        ' optionsPanel
        ' 
        Me.optionsPanel.BackColor = System.Drawing.Color.FromArgb(Fix(System.Convert.ToByte(128)), Fix(System.Convert.ToByte(128)), Fix(System.Convert.ToByte(250)))
        Me.optionsPanel.Controls.Add(Me.encodingSchemeComboBox)
        Me.optionsPanel.Controls.Add(Me.normalizationComboBox)
        Me.optionsPanel.Controls.Add(Me.normalizationLabel)
        Me.optionsPanel.Controls.Add(Me.fallbackSchemeComboBox)
        Me.optionsPanel.Controls.Add(Me.fallBackLabel)
        Me.optionsPanel.Controls.Add(Me.encdingLabel)
        Me.optionsPanel.Controls.Add(Me.fileLabel)
        Me.optionsPanel.Controls.Add(Me.stringLabel)
        Me.optionsPanel.Controls.Add(Me.decodeStringLabel)
        Me.optionsPanel.Controls.Add(Me.encodingLabel)
        Me.optionsPanel.Dock = System.Windows.Forms.DockStyle.Left
        Me.optionsPanel.Location = New System.Drawing.Point(0, 0)
        Me.optionsPanel.Name = "optionsPanel"
        Me.optionsPanel.Size = New System.Drawing.Size(217, 500)
        Me.optionsPanel.TabIndex = 0
        ' 
        ' descriptionPanel
        ' 
        Me.descriptionPanel.Controls.Add(Me.descFallbacklabel)
        Me.descriptionPanel.Controls.Add(Me.descNormalizationlabel)
        Me.descriptionPanel.Controls.Add(Me.titleNormalizationLabel)
        Me.descriptionPanel.Controls.Add(Me.titleFallbackLabel)
        Me.descriptionPanel.Controls.Add(Me.encDescLabel)
        Me.descriptionPanel.Controls.Add(Me.titleLabel)
        Me.descriptionPanel.Dock = System.Windows.Forms.DockStyle.Left
        Me.descriptionPanel.Location = New System.Drawing.Point(217, 0)
        Me.descriptionPanel.Name = "descriptionPanel"
        Me.descriptionPanel.Size = New System.Drawing.Size(580, 500)
        Me.descriptionPanel.TabIndex = 7
        ' 
        ' descFallbacklabel
        ' 
        Me.descFallbacklabel.Location = New System.Drawing.Point(18, 129)
        Me.descFallbacklabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.descFallbacklabel.Name = "descFallbacklabel"
        Me.descFallbacklabel.Size = New System.Drawing.Size(510, 84)
        Me.descFallbacklabel.TabIndex = 5
        Me.descFallbacklabel.Text = "Fallback mechanism of the encoding determines how any character that the encoder is unable to " + vbCr + vbLf + "encode is treated" + vbCr + vbLf + "Replacement fallback replaces that character with a pre-determined string, usually ?" + vbCr + vbLf + "Exception fallback throws an exception when it encounters such a character" + vbCr + vbLf + "Best Fit fallback allows the user to implement her own encoding"
        ' 
        ' descNormalizationlabel
        ' 
        Me.descNormalizationlabel.Location = New System.Drawing.Point(18, 230)
        Me.descNormalizationlabel.Name = "descNormalizationlabel"
        Me.descNormalizationlabel.Size = New System.Drawing.Size(510, 46)
        Me.descNormalizationlabel.TabIndex = 4
        Me.descNormalizationlabel.Text = "Normalization is done on Unicode strings in order to eliminate ambiguity between " + "different representations" + vbCr + vbLf + " of special characters"
        ' 
        ' titleNormalizationLabel
        ' 
        Me.titleNormalizationLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, CType(System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Underline, System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.titleNormalizationLabel.Location = New System.Drawing.Point(18, 213)
        Me.titleNormalizationLabel.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
        Me.titleNormalizationLabel.Name = "titleNormalizationLabel"
        Me.titleNormalizationLabel.Size = New System.Drawing.Size(446, 17)
        Me.titleNormalizationLabel.TabIndex = 3
        Me.titleNormalizationLabel.Text = "Normalization"
        ' 
        ' titleFallbackLabel
        ' 
        Me.titleFallbackLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, CType(System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Underline, System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.titleFallbackLabel.Location = New System.Drawing.Point(18, 112)
        Me.titleFallbackLabel.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
        Me.titleFallbackLabel.Name = "titleFallbackLabel"
        Me.titleFallbackLabel.Size = New System.Drawing.Size(446, 17)
        Me.titleFallbackLabel.TabIndex = 2
        Me.titleFallbackLabel.Text = "FallBack"
        ' 
        ' encDescLabel
        ' 
        Me.encDescLabel.Location = New System.Drawing.Point(18, 59)
        Me.encDescLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 2)
        Me.encDescLabel.Name = "encDescLabel"
        Me.encDescLabel.Size = New System.Drawing.Size(529, 53)
        Me.encDescLabel.TabIndex = 1
        Me.encDescLabel.Text = "Encoding is the process of converting a given set of characters to a set of bytes" + " that uniquely represent the " + vbCr + vbLf + "characters in a particular encoding scheme." + vbCr + vbLf + "UTF " + "-8 is the most popular encoding scheme and also the most recommended one."
        ' 
        ' titleLabel
        ' 
        Me.titleLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, CType(System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Underline, System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, System.Convert.ToByte(0))
        Me.titleLabel.Location = New System.Drawing.Point(18, 42)
        Me.titleLabel.Name = "titleLabel"
        Me.titleLabel.Size = New System.Drawing.Size(446, 17)
        Me.titleLabel.TabIndex = 0
        Me.titleLabel.Text = "Encoding"
        ' 
        ' EncodeForm
        ' 
        Me.BackColor = System.Drawing.Color.White
        Me.ClientSize = New System.Drawing.Size(800, 500)
        Me.Controls.Add(descriptionPanel)
        Me.Controls.Add(optionsPanel)
        Me.Name = "EncodeForm"
        Me.Text = "Encode"
        Me.optionsPanel.ResumeLayout(False)
        Me.descriptionPanel.ResumeLayout(False)
        Me.ResumeLayout(False)
    
    End Sub 'InitializeComponent 
    
    #End Region
    
    
    Private Sub InitializeUserControls() 
        
        Me.encInfo = New EncoderInfo()
        Me.decodeStringControl = New DecodeBytesUserControl(Me)
        Me.stringControl = New EncodeStringUserControl(Me)
        Me.fileControl = New EncodeFileUserControl(Me)
        ' 
        ' stringControl
        ' 
        Me.stringControl.Location = New System.Drawing.Point(217, 0)
        Me.stringControl.Name = "stringControl"
        Me.stringControl.Size = New System.Drawing.Size(581, 418)
        Me.stringControl.TabIndex = 5
        ' 
        ' fileControl
        ' 
        Me.fileControl.Location = New System.Drawing.Point(217, 0)
        Me.fileControl.Name = "fileControl"
        Me.fileControl.Size = New System.Drawing.Size(409, 420)
        Me.fileControl.TabIndex = 4
        
        '
        ' decodeStringControl
        '
        Me.decodeStringControl.Location = New Point(217, 0)
        Me.decodeStringControl.Name = "decodeStringControl"
        
        
        Me.Controls.Add(Me.fileControl)
        Me.Controls.Add(Me.stringControl)
        Me.Controls.Add(Me.decodeStringControl)
    
    End Sub 'InitializeUserControls
    
    
    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    <STAThread()>  _
    Shared Sub Main() 
        Application.EnableVisualStyles()
        Application.Run(New EncodeForm())
    
    End Sub 'Main
    
    'Property to get the object encapsulating the encoding options
    
    Public ReadOnly Property EncodeInfo() As EncoderInfo 
        Get
            Return encInfo
        End Get
    End Property
    
    
    'Display string encode UI
    Private Sub stringLabel_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles stringLabel.Click
        descriptionPanel.Visible = False
        fileControl.Visible = False
        stringControl.Visible = True
        decodeStringControl.Visible = False
    
    End Sub 'stringLabel_Click
    
    
    'Display file encode UI
    Private Sub fileLabel_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles fileLabel.Click
        descriptionPanel.Visible = False
        fileControl.Visible = True
        stringControl.Visible = False
        decodeStringControl.Visible = False
    
    End Sub 'fileLabel_Click
    
    
    'Display string decode UI
    Private Sub decodeStringLabel_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles decodeStringLabel.Click
        descriptionPanel.Visible = False
        fileControl.Visible = False
        stringControl.Visible = False
        decodeStringControl.Visible = True
    
    End Sub 'decodeStringLabel_Click
    
    
    Private Sub EncodeForm_Load(ByVal sender As Object, ByVal e As System.EventArgs)  Handles MyBase.Load
        ' Initialize encoding schemes
        Dim encodings As EncodingInfo() = Encoding.GetEncodings()
	Dim encList As New ArrayList(encodings)
        Dim encComp As EncodingSortClass = New EncodingSortClass()
        encList.Sort(encComp)
        encodings = CType(encList.ToArray(GetType(EncodingInfo)), EncodingInfo())
        
        encodingSchemeComboBox.DataSource = encodings
        encodingSchemeComboBox.DisplayMember = "DisplayName"
        encodingSchemeComboBox.ValueMember = "CodePage"
        encodingSchemeComboBox.SelectedValue = Encoding.UTF8.CodePage
        AddHandler Me.encodingSchemeComboBox.SelectedIndexChanged, AddressOf Me.encodingSchemeComboBox_SelectedIndexChanged
        
        ' Initialize fallback schemes
        Me.fallbackSchemeComboBox.Items.AddRange(EncoderInfo.FallbackSchemes)
        fallbackSchemeComboBox.SelectedIndex = 0
        
        ' Initialize normalization form
        Me.normalizationComboBox.Items.Add(EncoderInfo.NormalizationNone)
        Me.normalizationComboBox.Items.AddRange([Enum].GetNames(GetType(NormalizationForm)))
        normalizationComboBox.SelectedIndex = 0
    
    End Sub 'EncodeForm_Load
    
    
    'Set the normalization scheme 
    Private Sub normalizationComboBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs)  Handles normalizationComboBox.SelectedIndexChanged
        encInfo.SetNormalizationForm(CStr(CType(sender, ComboBox).SelectedItem))
    
    End Sub 'normalizationComboBox_SelectedIndexChanged
    
    
    'Set the fallback scheme
    Private Sub fallbackSchemeComboBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs)  Handles fallbackSchemeComboBox.SelectedIndexChanged
        encInfo.SetEncoderFallback(CStr(CType(sender, ComboBox).SelectedItem))
    
    End Sub 'fallbackSchemeComboBox_SelectedIndexChanged
    
    
    'Set the encoding scheme
    Private Sub encodingSchemeComboBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) 
        encInfo.SetEncodingCodePage(Integer.Parse(CType(sender, ComboBox).SelectedValue.ToString(), CultureInfo.CurrentUICulture))
    
    End Sub 'encodingSchemeComboBox_SelectedIndexChanged
    
    
    Private Sub encodingLabel_Click(ByVal sender As Object, ByVal e As System.EventArgs)  Handles encodingLabel.Click
        descriptionPanel.Visible = True
        fileControl.Visible = False
        stringControl.Visible = False
        decodeStringControl.Visible = False
    
    End Sub 'encodingLabel_Click
End Class 'EncodeForm
Friend Class EncodingSortClass
    Implements IComparer
    
    Function Compare(ByVal x As Object, ByVal y As Object) As Integer  Implements IComparer.Compare
        If TypeOf x Is EncodingInfo AndAlso TypeOf y Is EncodingInfo Then
            Return New CaseInsensitiveComparer().Compare(CType(x, EncodingInfo).DisplayName, CType(y, EncodingInfo).DisplayName)
        Else
            Throw New ArgumentException("Object passed not EncodingInfo!")
        End If
    
    End Function 'IComparer.Compare
End Class 'EncodingSortClass