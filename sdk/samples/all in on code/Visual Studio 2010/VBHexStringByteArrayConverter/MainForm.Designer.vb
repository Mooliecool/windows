Partial Public Class MainForm
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.IContainer = Nothing

    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    ''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso (components IsNot Nothing) Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.group1 = New System.Windows.Forms.GroupBox()
        Me.label1 = New System.Windows.Forms.Label()
        Me.lbHexTip = New System.Windows.Forms.Label()
        Me.btnConvertHexStringToBytes = New System.Windows.Forms.Button()
        Me.cmbByteArrayResult = New System.Windows.Forms.ComboBox()
        Me.btnReverseCopytoClipboard = New System.Windows.Forms.Button()
        Me.tbHexStringInput = New System.Windows.Forms.TextBox()
        Me.lbForwardBypteArray = New System.Windows.Forms.Label()
        Me.lbForwardHexString = New System.Windows.Forms.Label()
        Me.btnPasteFromClipboard = New System.Windows.Forms.Button()
        Me.group2 = New System.Windows.Forms.GroupBox()
        Me.label3 = New System.Windows.Forms.Label()
        Me.label2 = New System.Windows.Forms.Label()
        Me.lbAnotherHexTip = New System.Windows.Forms.Label()
        Me.btnClearBytes = New System.Windows.Forms.Button()
        Me.tbByteToAdd = New System.Windows.Forms.TextBox()
        Me.btnAddByte = New System.Windows.Forms.Button()
        Me.btnConvertBytesToHexString = New System.Windows.Forms.Button()
        Me.cmbByteArrayInput = New System.Windows.Forms.ComboBox()
        Me.tbHexStringResult = New System.Windows.Forms.TextBox()
        Me.lbByteArrayInput = New System.Windows.Forms.Label()
        Me.lbReverseHexString = New System.Windows.Forms.Label()
        Me.group1.SuspendLayout()
        Me.group2.SuspendLayout()
        Me.SuspendLayout()
        '
        'group1
        '
        Me.group1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.group1.Controls.Add(Me.label1)
        Me.group1.Controls.Add(Me.lbHexTip)
        Me.group1.Controls.Add(Me.btnConvertHexStringToBytes)
        Me.group1.Controls.Add(Me.cmbByteArrayResult)
        Me.group1.Controls.Add(Me.btnReverseCopytoClipboard)
        Me.group1.Controls.Add(Me.tbHexStringInput)
        Me.group1.Controls.Add(Me.lbForwardBypteArray)
        Me.group1.Controls.Add(Me.lbForwardHexString)
        Me.group1.Location = New System.Drawing.Point(12, 12)
        Me.group1.Name = "group1"
        Me.group1.Size = New System.Drawing.Size(424, 104)
        Me.group1.TabIndex = 0
        Me.group1.TabStop = False
        Me.group1.Text = "Hex String -> Byte Array"
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(100, 65)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(18, 13)
        Me.label1.TabIndex = 10
        Me.label1.Text = "0x"
        '
        'lbHexTip
        '
        Me.lbHexTip.AutoSize = True
        Me.lbHexTip.Location = New System.Drawing.Point(100, 33)
        Me.lbHexTip.Name = "lbHexTip"
        Me.lbHexTip.Size = New System.Drawing.Size(18, 13)
        Me.lbHexTip.TabIndex = 6
        Me.lbHexTip.Text = "0x"
        '
        'btnConvertHexStringToBytes
        '
        Me.btnConvertHexStringToBytes.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnConvertHexStringToBytes.Location = New System.Drawing.Point(271, 28)
        Me.btnConvertHexStringToBytes.Name = "btnConvertHexStringToBytes"
        Me.btnConvertHexStringToBytes.Size = New System.Drawing.Size(136, 23)
        Me.btnConvertHexStringToBytes.TabIndex = 4
        Me.btnConvertHexStringToBytes.Text = "Hex String -> Byte Array"
        Me.btnConvertHexStringToBytes.UseVisualStyleBackColor = True
        '
        'cmbByteArrayResult
        '
        Me.cmbByteArrayResult.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmbByteArrayResult.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbByteArrayResult.FormatString = "X2"
        Me.cmbByteArrayResult.FormattingEnabled = True
        Me.cmbByteArrayResult.Location = New System.Drawing.Point(118, 61)
        Me.cmbByteArrayResult.Name = "cmbByteArrayResult"
        Me.cmbByteArrayResult.Size = New System.Drawing.Size(147, 21)
        Me.cmbByteArrayResult.TabIndex = 3
        '
        'btnReverseCopytoClipboard
        '
        Me.btnReverseCopytoClipboard.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnReverseCopytoClipboard.Location = New System.Drawing.Point(271, 60)
        Me.btnReverseCopytoClipboard.Name = "btnReverseCopytoClipboard"
        Me.btnReverseCopytoClipboard.Size = New System.Drawing.Size(136, 23)
        Me.btnReverseCopytoClipboard.TabIndex = 9
        Me.btnReverseCopytoClipboard.Text = "Copy to Clipboard"
        Me.btnReverseCopytoClipboard.UseVisualStyleBackColor = True
        '
        'tbHexStringInput
        '
        Me.tbHexStringInput.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbHexStringInput.Location = New System.Drawing.Point(118, 30)
        Me.tbHexStringInput.Name = "tbHexStringInput"
        Me.tbHexStringInput.Size = New System.Drawing.Size(147, 20)
        Me.tbHexStringInput.TabIndex = 2
        Me.tbHexStringInput.Text = "FF00EE11"
        '
        'lbForwardBypteArray
        '
        Me.lbForwardBypteArray.AutoSize = True
        Me.lbForwardBypteArray.Location = New System.Drawing.Point(29, 64)
        Me.lbForwardBypteArray.Name = "lbForwardBypteArray"
        Me.lbForwardBypteArray.Size = New System.Drawing.Size(58, 13)
        Me.lbForwardBypteArray.TabIndex = 1
        Me.lbForwardBypteArray.Text = "Byte Array:"
        '
        'lbForwardHexString
        '
        Me.lbForwardHexString.AutoSize = True
        Me.lbForwardHexString.Location = New System.Drawing.Point(29, 33)
        Me.lbForwardHexString.Name = "lbForwardHexString"
        Me.lbForwardHexString.Size = New System.Drawing.Size(59, 13)
        Me.lbForwardHexString.TabIndex = 0
        Me.lbForwardHexString.Text = "Hex String:"
        '
        'btnPasteFromClipboard
        '
        Me.btnPasteFromClipboard.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnPasteFromClipboard.Location = New System.Drawing.Point(271, 56)
        Me.btnPasteFromClipboard.Name = "btnPasteFromClipboard"
        Me.btnPasteFromClipboard.Size = New System.Drawing.Size(136, 23)
        Me.btnPasteFromClipboard.TabIndex = 5
        Me.btnPasteFromClipboard.Text = "Paste From Clipboard"
        Me.btnPasteFromClipboard.UseVisualStyleBackColor = True
        '
        'group2
        '
        Me.group2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.group2.Controls.Add(Me.label3)
        Me.group2.Controls.Add(Me.label2)
        Me.group2.Controls.Add(Me.lbAnotherHexTip)
        Me.group2.Controls.Add(Me.btnPasteFromClipboard)
        Me.group2.Controls.Add(Me.btnClearBytes)
        Me.group2.Controls.Add(Me.tbByteToAdd)
        Me.group2.Controls.Add(Me.btnAddByte)
        Me.group2.Controls.Add(Me.btnConvertBytesToHexString)
        Me.group2.Controls.Add(Me.cmbByteArrayInput)
        Me.group2.Controls.Add(Me.tbHexStringResult)
        Me.group2.Controls.Add(Me.lbByteArrayInput)
        Me.group2.Controls.Add(Me.lbReverseHexString)
        Me.group2.Location = New System.Drawing.Point(12, 122)
        Me.group2.Name = "group2"
        Me.group2.Size = New System.Drawing.Size(424, 166)
        Me.group2.TabIndex = 1
        Me.group2.TabStop = False
        Me.group2.Text = "Byte Array -> Hex String"
        '
        'label3
        '
        Me.label3.AutoSize = True
        Me.label3.Location = New System.Drawing.Point(100, 126)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(18, 13)
        Me.label3.TabIndex = 13
        Me.label3.Text = "0x"
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(100, 62)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(18, 13)
        Me.label2.TabIndex = 11
        Me.label2.Text = "0x"
        '
        'lbAnotherHexTip
        '
        Me.lbAnotherHexTip.AutoSize = True
        Me.lbAnotherHexTip.Location = New System.Drawing.Point(100, 30)
        Me.lbAnotherHexTip.Name = "lbAnotherHexTip"
        Me.lbAnotherHexTip.Size = New System.Drawing.Size(18, 13)
        Me.lbAnotherHexTip.TabIndex = 12
        Me.lbAnotherHexTip.Text = "0x"
        '
        'btnClearBytes
        '
        Me.btnClearBytes.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnClearBytes.Location = New System.Drawing.Point(209, 25)
        Me.btnClearBytes.Name = "btnClearBytes"
        Me.btnClearBytes.Size = New System.Drawing.Size(57, 23)
        Me.btnClearBytes.TabIndex = 6
        Me.btnClearBytes.Text = "Clear"
        Me.btnClearBytes.UseVisualStyleBackColor = True
        '
        'tbByteToAdd
        '
        Me.tbByteToAdd.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbByteToAdd.Location = New System.Drawing.Point(118, 27)
        Me.tbByteToAdd.Name = "tbByteToAdd"
        Me.tbByteToAdd.Size = New System.Drawing.Size(26, 20)
        Me.tbByteToAdd.TabIndex = 11
        '
        'btnAddByte
        '
        Me.btnAddByte.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnAddByte.Location = New System.Drawing.Point(149, 25)
        Me.btnAddByte.Name = "btnAddByte"
        Me.btnAddByte.Size = New System.Drawing.Size(58, 23)
        Me.btnAddByte.TabIndex = 10
        Me.btnAddByte.Text = "Add"
        Me.btnAddByte.UseVisualStyleBackColor = True
        '
        'btnConvertBytesToHexString
        '
        Me.btnConvertBytesToHexString.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnConvertBytesToHexString.Location = New System.Drawing.Point(271, 87)
        Me.btnConvertBytesToHexString.Name = "btnConvertBytesToHexString"
        Me.btnConvertBytesToHexString.Size = New System.Drawing.Size(136, 23)
        Me.btnConvertBytesToHexString.TabIndex = 8
        Me.btnConvertBytesToHexString.Text = "Byte Array -> Hex String"
        Me.btnConvertBytesToHexString.UseVisualStyleBackColor = True
        '
        'cmbByteArrayInput
        '
        Me.cmbByteArrayInput.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmbByteArrayInput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbByteArrayInput.FormatString = "X2"
        Me.cmbByteArrayInput.FormattingEnabled = True
        Me.cmbByteArrayInput.Location = New System.Drawing.Point(118, 58)
        Me.cmbByteArrayInput.Name = "cmbByteArrayInput"
        Me.cmbByteArrayInput.Size = New System.Drawing.Size(147, 21)
        Me.cmbByteArrayInput.TabIndex = 7
        '
        'tbHexStringResult
        '
        Me.tbHexStringResult.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbHexStringResult.Location = New System.Drawing.Point(118, 123)
        Me.tbHexStringResult.Name = "tbHexStringResult"
        Me.tbHexStringResult.ReadOnly = True
        Me.tbHexStringResult.Size = New System.Drawing.Size(147, 20)
        Me.tbHexStringResult.TabIndex = 6
        '
        'lbByteArrayInput
        '
        Me.lbByteArrayInput.AutoSize = True
        Me.lbByteArrayInput.Location = New System.Drawing.Point(29, 31)
        Me.lbByteArrayInput.Name = "lbByteArrayInput"
        Me.lbByteArrayInput.Size = New System.Drawing.Size(58, 13)
        Me.lbByteArrayInput.TabIndex = 5
        Me.lbByteArrayInput.Text = "Byte Array:"
        '
        'lbReverseHexString
        '
        Me.lbReverseHexString.AutoSize = True
        Me.lbReverseHexString.Location = New System.Drawing.Point(29, 126)
        Me.lbReverseHexString.Name = "lbReverseHexString"
        Me.lbReverseHexString.Size = New System.Drawing.Size(59, 13)
        Me.lbReverseHexString.TabIndex = 4
        Me.lbReverseHexString.Text = "Hex String:"
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(448, 300)
        Me.Controls.Add(Me.group2)
        Me.Controls.Add(Me.group1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "MainForm"
        Me.Text = "VBHexStringByteArrayConverter"
        Me.group1.ResumeLayout(False)
        Me.group1.PerformLayout()
        Me.group2.ResumeLayout(False)
        Me.group2.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private group1 As GroupBox
    Private WithEvents btnConvertHexStringToBytes As Button
    Private cmbByteArrayResult As ComboBox
    Private tbHexStringInput As TextBox
    Private lbForwardBypteArray As Label
    Private lbForwardHexString As Label
    Private group2 As GroupBox
    Private WithEvents btnReverseCopytoClipboard As Button
    Private WithEvents btnConvertBytesToHexString As Button
    Private cmbByteArrayInput As ComboBox
    Private tbHexStringResult As TextBox
    Private lbByteArrayInput As Label
    Private lbReverseHexString As Label
    Private WithEvents btnAddByte As Button
    Private tbByteToAdd As TextBox
    Private WithEvents btnClearBytes As Button
    Private WithEvents btnPasteFromClipboard As Button
    Private lbHexTip As Label
    Private lbAnotherHexTip As Label
    Private label1 As Label
    Private label3 As Label
    Private label2 As Label
End Class

