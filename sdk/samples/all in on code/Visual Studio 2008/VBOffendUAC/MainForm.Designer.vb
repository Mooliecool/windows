<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.FileGroupBox = New System.Windows.Forms.GroupBox
        Me.btnDeleteFile = New System.Windows.Forms.Button
        Me.btnReadFile = New System.Windows.Forms.Button
        Me.btnWriteFile = New System.Windows.Forms.Button
        Me.tbReadFile = New System.Windows.Forms.TextBox
        Me.tbWriteFile = New System.Windows.Forms.TextBox
        Me.btnBrowseFile = New System.Windows.Forms.Button
        Me.tbFilePath = New System.Windows.Forms.TextBox
        Me.Label2 = New System.Windows.Forms.Label
        Me.btnBrowseFolder = New System.Windows.Forms.Button
        Me.btnDeleteDirectory = New System.Windows.Forms.Button
        Me.btnCreateDirectory = New System.Windows.Forms.Button
        Me.btnSetCurrentDirectory = New System.Windows.Forms.Button
        Me.tbDirectoryPath = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.RegistryGroupBox = New System.Windows.Forms.GroupBox
        Me.btnCreateRegKey = New System.Windows.Forms.Button
        Me.btnOpenRegKey = New System.Windows.Forms.Button
        Me.btnDeleteRegKey = New System.Windows.Forms.Button
        Me.ddlChildRegKey = New System.Windows.Forms.ComboBox
        Me.ddlRootRegKey = New System.Windows.Forms.ComboBox
        Me.FolderBrowserDialog1 = New System.Windows.Forms.FolderBrowserDialog
        Me.OpenFileDialog1 = New System.Windows.Forms.OpenFileDialog
        Me.IniGroupBox = New System.Windows.Forms.GroupBox
        Me.bnReadWinIni = New System.Windows.Forms.Button
        Me.bnWriteWinIni = New System.Windows.Forms.Button
        Me.bnReadIni = New System.Windows.Forms.Button
        Me.bnWriteIni = New System.Windows.Forms.Button
        Me.TokenGroupBox = New System.Windows.Forms.GroupBox
        Me.btnIsAdmin = New System.Windows.Forms.Button
        Me.PrivilegeGroupBox = New System.Windows.Forms.GroupBox
        Me.ddlPrivilege = New System.Windows.Forms.ComboBox
        Me.btnRemovePrivilege = New System.Windows.Forms.Button
        Me.btnSetPrivilege = New System.Windows.Forms.Button
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.bnRestartWinService = New System.Windows.Forms.Button
        Me.tbIniFileName = New System.Windows.Forms.TextBox
        Me.FileGroupBox.SuspendLayout()
        Me.RegistryGroupBox.SuspendLayout()
        Me.IniGroupBox.SuspendLayout()
        Me.TokenGroupBox.SuspendLayout()
        Me.PrivilegeGroupBox.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'FileGroupBox
        '
        Me.FileGroupBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.FileGroupBox.Controls.Add(Me.btnDeleteFile)
        Me.FileGroupBox.Controls.Add(Me.btnReadFile)
        Me.FileGroupBox.Controls.Add(Me.btnWriteFile)
        Me.FileGroupBox.Controls.Add(Me.tbReadFile)
        Me.FileGroupBox.Controls.Add(Me.tbWriteFile)
        Me.FileGroupBox.Controls.Add(Me.btnBrowseFile)
        Me.FileGroupBox.Controls.Add(Me.tbFilePath)
        Me.FileGroupBox.Controls.Add(Me.Label2)
        Me.FileGroupBox.Controls.Add(Me.btnBrowseFolder)
        Me.FileGroupBox.Controls.Add(Me.btnDeleteDirectory)
        Me.FileGroupBox.Controls.Add(Me.btnCreateDirectory)
        Me.FileGroupBox.Controls.Add(Me.btnSetCurrentDirectory)
        Me.FileGroupBox.Controls.Add(Me.tbDirectoryPath)
        Me.FileGroupBox.Controls.Add(Me.Label1)
        Me.FileGroupBox.Location = New System.Drawing.Point(12, 12)
        Me.FileGroupBox.Name = "FileGroupBox"
        Me.FileGroupBox.Size = New System.Drawing.Size(428, 216)
        Me.FileGroupBox.TabIndex = 0
        Me.FileGroupBox.TabStop = False
        Me.FileGroupBox.Text = "File"
        '
        'btnDeleteFile
        '
        Me.btnDeleteFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnDeleteFile.Location = New System.Drawing.Point(347, 179)
        Me.btnDeleteFile.Name = "btnDeleteFile"
        Me.btnDeleteFile.Size = New System.Drawing.Size(75, 23)
        Me.btnDeleteFile.TabIndex = 15
        Me.btnDeleteFile.Text = "Delete"
        Me.btnDeleteFile.UseVisualStyleBackColor = True
        '
        'btnReadFile
        '
        Me.btnReadFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnReadFile.Location = New System.Drawing.Point(207, 179)
        Me.btnReadFile.Name = "btnReadFile"
        Me.btnReadFile.Size = New System.Drawing.Size(134, 23)
        Me.btnReadFile.TabIndex = 14
        Me.btnReadFile.Text = "Read from File"
        Me.btnReadFile.UseVisualStyleBackColor = True
        '
        'btnWriteFile
        '
        Me.btnWriteFile.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnWriteFile.Location = New System.Drawing.Point(65, 179)
        Me.btnWriteFile.Name = "btnWriteFile"
        Me.btnWriteFile.Size = New System.Drawing.Size(136, 23)
        Me.btnWriteFile.TabIndex = 13
        Me.btnWriteFile.Text = "Write to File"
        Me.btnWriteFile.UseVisualStyleBackColor = True
        '
        'tbReadFile
        '
        Me.tbReadFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbReadFile.Location = New System.Drawing.Point(207, 101)
        Me.tbReadFile.Multiline = True
        Me.tbReadFile.Name = "tbReadFile"
        Me.tbReadFile.Size = New System.Drawing.Size(134, 71)
        Me.tbReadFile.TabIndex = 12
        '
        'tbWriteFile
        '
        Me.tbWriteFile.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbWriteFile.Location = New System.Drawing.Point(65, 101)
        Me.tbWriteFile.Multiline = True
        Me.tbWriteFile.Name = "tbWriteFile"
        Me.tbWriteFile.Size = New System.Drawing.Size(136, 71)
        Me.tbWriteFile.TabIndex = 9
        Me.tbWriteFile.Text = "Sample text written by the VBOffendUAC sample in All-In-One Code Framework"
        '
        'btnBrowseFile
        '
        Me.btnBrowseFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnBrowseFile.Location = New System.Drawing.Point(347, 73)
        Me.btnBrowseFile.Name = "btnBrowseFile"
        Me.btnBrowseFile.Size = New System.Drawing.Size(75, 23)
        Me.btnBrowseFile.TabIndex = 8
        Me.btnBrowseFile.Text = "Browse..."
        Me.btnBrowseFile.UseVisualStyleBackColor = True
        '
        'tbFilePath
        '
        Me.tbFilePath.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbFilePath.Location = New System.Drawing.Point(65, 75)
        Me.tbFilePath.Name = "tbFilePath"
        Me.tbFilePath.Size = New System.Drawing.Size(276, 20)
        Me.tbFilePath.TabIndex = 7
        Me.tbFilePath.Text = "C:\Sample.txt"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(7, 78)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(26, 13)
        Me.Label2.TabIndex = 6
        Me.Label2.Text = "File:"
        '
        'btnBrowseFolder
        '
        Me.btnBrowseFolder.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnBrowseFolder.Location = New System.Drawing.Point(347, 15)
        Me.btnBrowseFolder.Name = "btnBrowseFolder"
        Me.btnBrowseFolder.Size = New System.Drawing.Size(75, 23)
        Me.btnBrowseFolder.TabIndex = 5
        Me.btnBrowseFolder.Text = "Browse..."
        Me.btnBrowseFolder.UseVisualStyleBackColor = True
        '
        'btnDeleteDirectory
        '
        Me.btnDeleteDirectory.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnDeleteDirectory.Location = New System.Drawing.Point(347, 43)
        Me.btnDeleteDirectory.Name = "btnDeleteDirectory"
        Me.btnDeleteDirectory.Size = New System.Drawing.Size(75, 23)
        Me.btnDeleteDirectory.TabIndex = 4
        Me.btnDeleteDirectory.Text = "Delete"
        Me.btnDeleteDirectory.UseVisualStyleBackColor = True
        '
        'btnCreateDirectory
        '
        Me.btnCreateDirectory.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnCreateDirectory.Location = New System.Drawing.Point(266, 43)
        Me.btnCreateDirectory.Name = "btnCreateDirectory"
        Me.btnCreateDirectory.Size = New System.Drawing.Size(75, 23)
        Me.btnCreateDirectory.TabIndex = 3
        Me.btnCreateDirectory.Text = "Create"
        Me.btnCreateDirectory.UseVisualStyleBackColor = True
        '
        'btnSetCurrentDirectory
        '
        Me.btnSetCurrentDirectory.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnSetCurrentDirectory.Location = New System.Drawing.Point(143, 43)
        Me.btnSetCurrentDirectory.Name = "btnSetCurrentDirectory"
        Me.btnSetCurrentDirectory.Size = New System.Drawing.Size(117, 23)
        Me.btnSetCurrentDirectory.TabIndex = 2
        Me.btnSetCurrentDirectory.Text = "Set Current Directory"
        Me.btnSetCurrentDirectory.UseVisualStyleBackColor = True
        '
        'tbDirectoryPath
        '
        Me.tbDirectoryPath.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbDirectoryPath.Location = New System.Drawing.Point(65, 17)
        Me.tbDirectoryPath.Name = "tbDirectoryPath"
        Me.tbDirectoryPath.Size = New System.Drawing.Size(276, 20)
        Me.tbDirectoryPath.TabIndex = 1
        Me.tbDirectoryPath.Text = "C:\Program Files\"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(7, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(52, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "Directory:"
        '
        'RegistryGroupBox
        '
        Me.RegistryGroupBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.RegistryGroupBox.Controls.Add(Me.btnCreateRegKey)
        Me.RegistryGroupBox.Controls.Add(Me.btnOpenRegKey)
        Me.RegistryGroupBox.Controls.Add(Me.btnDeleteRegKey)
        Me.RegistryGroupBox.Controls.Add(Me.ddlChildRegKey)
        Me.RegistryGroupBox.Controls.Add(Me.ddlRootRegKey)
        Me.RegistryGroupBox.Location = New System.Drawing.Point(12, 234)
        Me.RegistryGroupBox.Name = "RegistryGroupBox"
        Me.RegistryGroupBox.Size = New System.Drawing.Size(428, 92)
        Me.RegistryGroupBox.TabIndex = 1
        Me.RegistryGroupBox.TabStop = False
        Me.RegistryGroupBox.Text = "Registry"
        '
        'btnCreateRegKey
        '
        Me.btnCreateRegKey.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnCreateRegKey.Location = New System.Drawing.Point(185, 56)
        Me.btnCreateRegKey.Name = "btnCreateRegKey"
        Me.btnCreateRegKey.Size = New System.Drawing.Size(75, 23)
        Me.btnCreateRegKey.TabIndex = 4
        Me.btnCreateRegKey.Text = "Create Key"
        Me.btnCreateRegKey.UseVisualStyleBackColor = True
        '
        'btnOpenRegKey
        '
        Me.btnOpenRegKey.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnOpenRegKey.Location = New System.Drawing.Point(266, 56)
        Me.btnOpenRegKey.Name = "btnOpenRegKey"
        Me.btnOpenRegKey.Size = New System.Drawing.Size(75, 23)
        Me.btnOpenRegKey.TabIndex = 3
        Me.btnOpenRegKey.Text = "Open Key"
        Me.btnOpenRegKey.UseVisualStyleBackColor = True
        '
        'btnDeleteRegKey
        '
        Me.btnDeleteRegKey.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnDeleteRegKey.Location = New System.Drawing.Point(347, 56)
        Me.btnDeleteRegKey.Name = "btnDeleteRegKey"
        Me.btnDeleteRegKey.Size = New System.Drawing.Size(75, 23)
        Me.btnDeleteRegKey.TabIndex = 2
        Me.btnDeleteRegKey.Text = "Delete Key"
        Me.btnDeleteRegKey.UseVisualStyleBackColor = True
        '
        'ddlChildRegKey
        '
        Me.ddlChildRegKey.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ddlChildRegKey.FormattingEnabled = True
        Me.ddlChildRegKey.Items.AddRange(New Object() {"SOFTWARE\MyApp\MyAppSettings\", "SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\", "SOFTWARE\Policies\", "TypeLib\"})
        Me.ddlChildRegKey.Location = New System.Drawing.Point(87, 29)
        Me.ddlChildRegKey.Name = "ddlChildRegKey"
        Me.ddlChildRegKey.Size = New System.Drawing.Size(335, 21)
        Me.ddlChildRegKey.TabIndex = 1
        '
        'ddlRootRegKey
        '
        Me.ddlRootRegKey.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.ddlRootRegKey.FormattingEnabled = True
        Me.ddlRootRegKey.Items.AddRange(New Object() {"HKLM", "HKCR", "HKCU"})
        Me.ddlRootRegKey.Location = New System.Drawing.Point(10, 29)
        Me.ddlRootRegKey.Name = "ddlRootRegKey"
        Me.ddlRootRegKey.Size = New System.Drawing.Size(71, 21)
        Me.ddlRootRegKey.TabIndex = 0
        '
        'FolderBrowserDialog1
        '
        Me.FolderBrowserDialog1.RootFolder = System.Environment.SpecialFolder.ProgramFiles
        '
        'OpenFileDialog1
        '
        Me.OpenFileDialog1.FileName = "OpenFileDialog1"
        '
        'IniGroupBox
        '
        Me.IniGroupBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.IniGroupBox.Controls.Add(Me.tbIniFileName)
        Me.IniGroupBox.Controls.Add(Me.bnReadWinIni)
        Me.IniGroupBox.Controls.Add(Me.bnWriteWinIni)
        Me.IniGroupBox.Controls.Add(Me.bnReadIni)
        Me.IniGroupBox.Controls.Add(Me.bnWriteIni)
        Me.IniGroupBox.Location = New System.Drawing.Point(12, 333)
        Me.IniGroupBox.Name = "IniGroupBox"
        Me.IniGroupBox.Size = New System.Drawing.Size(428, 89)
        Me.IniGroupBox.TabIndex = 2
        Me.IniGroupBox.TabStop = False
        Me.IniGroupBox.Text = "INI"
        '
        'bnReadWinIni
        '
        Me.bnReadWinIni.Location = New System.Drawing.Point(321, 52)
        Me.bnReadWinIni.Name = "bnReadWinIni"
        Me.bnReadWinIni.Size = New System.Drawing.Size(101, 23)
        Me.bnReadWinIni.TabIndex = 3
        Me.bnReadWinIni.Text = "Read WIN.INI"
        Me.bnReadWinIni.UseVisualStyleBackColor = True
        '
        'bnWriteWinIni
        '
        Me.bnWriteWinIni.Location = New System.Drawing.Point(219, 52)
        Me.bnWriteWinIni.Name = "bnWriteWinIni"
        Me.bnWriteWinIni.Size = New System.Drawing.Size(96, 23)
        Me.bnWriteWinIni.TabIndex = 2
        Me.bnWriteWinIni.Text = "Write WIN.INI"
        Me.bnWriteWinIni.UseVisualStyleBackColor = True
        '
        'bnReadIni
        '
        Me.bnReadIni.Location = New System.Drawing.Point(118, 52)
        Me.bnReadIni.Name = "bnReadIni"
        Me.bnReadIni.Size = New System.Drawing.Size(95, 23)
        Me.bnReadIni.TabIndex = 1
        Me.bnReadIni.Text = "Read Ini File"
        Me.bnReadIni.UseVisualStyleBackColor = True
        '
        'bnWriteIni
        '
        Me.bnWriteIni.Location = New System.Drawing.Point(10, 52)
        Me.bnWriteIni.Name = "bnWriteIni"
        Me.bnWriteIni.Size = New System.Drawing.Size(102, 23)
        Me.bnWriteIni.TabIndex = 0
        Me.bnWriteIni.Text = "Write Ini File"
        Me.bnWriteIni.UseVisualStyleBackColor = True
        '
        'TokenGroupBox
        '
        Me.TokenGroupBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TokenGroupBox.Controls.Add(Me.btnIsAdmin)
        Me.TokenGroupBox.Location = New System.Drawing.Point(12, 428)
        Me.TokenGroupBox.Name = "TokenGroupBox"
        Me.TokenGroupBox.Size = New System.Drawing.Size(428, 66)
        Me.TokenGroupBox.TabIndex = 3
        Me.TokenGroupBox.TabStop = False
        Me.TokenGroupBox.Text = "Token"
        '
        'btnIsAdmin
        '
        Me.btnIsAdmin.Location = New System.Drawing.Point(10, 29)
        Me.btnIsAdmin.Name = "btnIsAdmin"
        Me.btnIsAdmin.Size = New System.Drawing.Size(128, 23)
        Me.btnIsAdmin.TabIndex = 0
        Me.btnIsAdmin.Text = "Is User an Admin?"
        Me.btnIsAdmin.UseVisualStyleBackColor = True
        '
        'PrivilegeGroupBox
        '
        Me.PrivilegeGroupBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.PrivilegeGroupBox.Controls.Add(Me.ddlPrivilege)
        Me.PrivilegeGroupBox.Controls.Add(Me.btnRemovePrivilege)
        Me.PrivilegeGroupBox.Controls.Add(Me.btnSetPrivilege)
        Me.PrivilegeGroupBox.Location = New System.Drawing.Point(12, 501)
        Me.PrivilegeGroupBox.Name = "PrivilegeGroupBox"
        Me.PrivilegeGroupBox.Size = New System.Drawing.Size(428, 65)
        Me.PrivilegeGroupBox.TabIndex = 4
        Me.PrivilegeGroupBox.TabStop = False
        Me.PrivilegeGroupBox.Text = "Privilege"
        '
        'ddlPrivilege
        '
        Me.ddlPrivilege.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ddlPrivilege.FormattingEnabled = True
        Me.ddlPrivilege.Items.AddRange(New Object() {"SeDebugPrivilege", "SeShutdownPrivilege", "SeBackupPrivilege", "SeCreateGlobalPrivilege"})
        Me.ddlPrivilege.Location = New System.Drawing.Point(10, 30)
        Me.ddlPrivilege.Name = "ddlPrivilege"
        Me.ddlPrivilege.Size = New System.Drawing.Size(245, 21)
        Me.ddlPrivilege.TabIndex = 3
        Me.ddlPrivilege.Text = "SeDebugPrivilege"
        '
        'btnRemovePrivilege
        '
        Me.btnRemovePrivilege.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRemovePrivilege.Location = New System.Drawing.Point(347, 28)
        Me.btnRemovePrivilege.Name = "btnRemovePrivilege"
        Me.btnRemovePrivilege.Size = New System.Drawing.Size(75, 23)
        Me.btnRemovePrivilege.TabIndex = 2
        Me.btnRemovePrivilege.Text = "Remove"
        Me.btnRemovePrivilege.UseVisualStyleBackColor = True
        '
        'btnSetPrivilege
        '
        Me.btnSetPrivilege.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnSetPrivilege.Location = New System.Drawing.Point(261, 28)
        Me.btnSetPrivilege.Name = "btnSetPrivilege"
        Me.btnSetPrivilege.Size = New System.Drawing.Size(80, 23)
        Me.btnSetPrivilege.TabIndex = 0
        Me.btnSetPrivilege.Text = "Set"
        Me.btnSetPrivilege.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.bnRestartWinService)
        Me.GroupBox1.Location = New System.Drawing.Point(12, 572)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(428, 63)
        Me.GroupBox1.TabIndex = 5
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Other Objects"
        '
        'bnRestartWinService
        '
        Me.bnRestartWinService.Location = New System.Drawing.Point(10, 28)
        Me.bnRestartWinService.Name = "bnRestartWinService"
        Me.bnRestartWinService.Size = New System.Drawing.Size(154, 23)
        Me.bnRestartWinService.TabIndex = 0
        Me.bnRestartWinService.Text = "Restart Windows Service"
        Me.bnRestartWinService.UseVisualStyleBackColor = True
        '
        'tbIniFileName
        '
        Me.tbIniFileName.Location = New System.Drawing.Point(12, 26)
        Me.tbIniFileName.Name = "tbIniFileName"
        Me.tbIniFileName.Size = New System.Drawing.Size(201, 20)
        Me.tbIniFileName.TabIndex = 4
        Me.tbIniFileName.Text = "VBOffendUAC.ini"
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(452, 647)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.PrivilegeGroupBox)
        Me.Controls.Add(Me.TokenGroupBox)
        Me.Controls.Add(Me.IniGroupBox)
        Me.Controls.Add(Me.RegistryGroupBox)
        Me.Controls.Add(Me.FileGroupBox)
        Me.Name = "MainForm"
        Me.Text = "VBOffendUAC"
        Me.FileGroupBox.ResumeLayout(False)
        Me.FileGroupBox.PerformLayout()
        Me.RegistryGroupBox.ResumeLayout(False)
        Me.IniGroupBox.ResumeLayout(False)
        Me.IniGroupBox.PerformLayout()
        Me.TokenGroupBox.ResumeLayout(False)
        Me.PrivilegeGroupBox.ResumeLayout(False)
        Me.GroupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents FileGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents RegistryGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents btnBrowseFile As System.Windows.Forms.Button
    Friend WithEvents tbFilePath As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents btnBrowseFolder As System.Windows.Forms.Button
    Friend WithEvents btnDeleteDirectory As System.Windows.Forms.Button
    Friend WithEvents btnCreateDirectory As System.Windows.Forms.Button
    Friend WithEvents btnSetCurrentDirectory As System.Windows.Forms.Button
    Friend WithEvents tbDirectoryPath As System.Windows.Forms.TextBox
    Friend WithEvents btnDeleteFile As System.Windows.Forms.Button
    Friend WithEvents btnReadFile As System.Windows.Forms.Button
    Friend WithEvents btnWriteFile As System.Windows.Forms.Button
    Friend WithEvents tbReadFile As System.Windows.Forms.TextBox
    Friend WithEvents tbWriteFile As System.Windows.Forms.TextBox
    Friend WithEvents FolderBrowserDialog1 As System.Windows.Forms.FolderBrowserDialog
    Friend WithEvents OpenFileDialog1 As System.Windows.Forms.OpenFileDialog
    Friend WithEvents ddlRootRegKey As System.Windows.Forms.ComboBox
    Friend WithEvents btnCreateRegKey As System.Windows.Forms.Button
    Friend WithEvents btnOpenRegKey As System.Windows.Forms.Button
    Friend WithEvents btnDeleteRegKey As System.Windows.Forms.Button
    Friend WithEvents ddlChildRegKey As System.Windows.Forms.ComboBox
    Friend WithEvents IniGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents TokenGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents btnIsAdmin As System.Windows.Forms.Button
    Friend WithEvents PrivilegeGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents btnSetPrivilege As System.Windows.Forms.Button
    Friend WithEvents btnRemovePrivilege As System.Windows.Forms.Button
    Friend WithEvents ddlPrivilege As System.Windows.Forms.ComboBox
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents bnRestartWinService As System.Windows.Forms.Button
    Friend WithEvents bnReadWinIni As System.Windows.Forms.Button
    Friend WithEvents bnWriteWinIni As System.Windows.Forms.Button
    Friend WithEvents bnReadIni As System.Windows.Forms.Button
    Friend WithEvents bnWriteIni As System.Windows.Forms.Button
    Friend WithEvents tbIniFileName As System.Windows.Forms.TextBox

End Class
