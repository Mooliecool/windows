
Partial Public Class MainForm
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.IContainer = Nothing

    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    ''' <param name="disposing">
    '''true if managed resources should be disposed; otherwise, false.
    '''</param>
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
        Me.pnlProcess = New System.Windows.Forms.Panel()
        Me.gvProcess = New System.Windows.Forms.DataGridView()
        Me.pnlInfo = New System.Windows.Forms.Panel()
        Me.btnRefresh = New System.Windows.Forms.Button()
        Me.ProcID = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.ProcessName = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.Is64BitProcess = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.IsManaged = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.IsDotNet4 = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.IsWPF = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.IsConsole = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.Remarks = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.pnlProcess.SuspendLayout()
        CType(Me.gvProcess, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlInfo.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnlProcess
        '
        Me.pnlProcess.Controls.Add(Me.gvProcess)
        Me.pnlProcess.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlProcess.Location = New System.Drawing.Point(0, 44)
        Me.pnlProcess.Name = "pnlProcess"
        Me.pnlProcess.Size = New System.Drawing.Size(935, 394)
        Me.pnlProcess.TabIndex = 0
        '
        'gvProcess
        '
        Me.gvProcess.AllowUserToAddRows = False
        Me.gvProcess.AllowUserToDeleteRows = False
        Me.gvProcess.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.gvProcess.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.ProcID, Me.ProcessName, Me.Is64BitProcess, Me.IsManaged, Me.IsDotNet4, Me.IsWPF, Me.IsConsole, Me.Remarks})
        Me.gvProcess.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gvProcess.Location = New System.Drawing.Point(0, 0)
        Me.gvProcess.Name = "gvProcess"
        Me.gvProcess.ReadOnly = True
        Me.gvProcess.RowHeadersVisible = False
        Me.gvProcess.Size = New System.Drawing.Size(935, 394)
        Me.gvProcess.TabIndex = 0
        '
        'pnlInfo
        '
        Me.pnlInfo.Controls.Add(Me.btnRefresh)
        Me.pnlInfo.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlInfo.Location = New System.Drawing.Point(0, 0)
        Me.pnlInfo.Name = "pnlInfo"
        Me.pnlInfo.Size = New System.Drawing.Size(935, 44)
        Me.pnlInfo.TabIndex = 1
        '
        'btnRefresh
        '
        Me.btnRefresh.Location = New System.Drawing.Point(12, 3)
        Me.btnRefresh.Name = "btnRefresh"
        Me.btnRefresh.Size = New System.Drawing.Size(75, 35)
        Me.btnRefresh.TabIndex = 0
        Me.btnRefresh.Text = "Refresh"
        Me.btnRefresh.UseVisualStyleBackColor = True
        '
        'ProcID
        '
        Me.ProcID.DataPropertyName = "Id"
        Me.ProcID.HeaderText = "ProcID"
        Me.ProcID.Name = "ProcID"
        Me.ProcID.ReadOnly = True
        '
        'ProcessName
        '
        Me.ProcessName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill
        Me.ProcessName.DataPropertyName = "ProcessName"
        Me.ProcessName.HeaderText = "Name"
        Me.ProcessName.Name = "ProcessName"
        Me.ProcessName.ReadOnly = True
        '
        'Is64BitProcess
        '
        Me.Is64BitProcess.DataPropertyName = "Is64BitProcess"
        Me.Is64BitProcess.HeaderText = "Is64BitProcess"
        Me.Is64BitProcess.Name = "Is64BitProcess"
        Me.Is64BitProcess.ReadOnly = True
        Me.Is64BitProcess.Resizable = System.Windows.Forms.DataGridViewTriState.[True]
        Me.Is64BitProcess.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic
        '
        'IsManaged
        '
        Me.IsManaged.DataPropertyName = "IsManaged"
        Me.IsManaged.HeaderText = "IsManaged"
        Me.IsManaged.Name = "IsManaged"
        Me.IsManaged.ReadOnly = True
        '
        'IsDotNet4
        '
        Me.IsDotNet4.DataPropertyName = "IsDotNet4"
        Me.IsDotNet4.HeaderText = "IsDotNet4"
        Me.IsDotNet4.Name = "IsDotNet4"
        Me.IsDotNet4.ReadOnly = True
        '
        'IsWPF
        '
        Me.IsWPF.DataPropertyName = "IsWPF"
        Me.IsWPF.HeaderText = "IsWPF"
        Me.IsWPF.Name = "IsWPF"
        Me.IsWPF.ReadOnly = True
        '
        'IsConsole
        '
        Me.IsConsole.DataPropertyName = "IsConsole"
        Me.IsConsole.HeaderText = "IsConsole"
        Me.IsConsole.Name = "IsConsole"
        Me.IsConsole.ReadOnly = True
        '
        'Remarks
        '
        Me.Remarks.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill
        Me.Remarks.DataPropertyName = "Remarks"
        Me.Remarks.HeaderText = "Remarks"
        Me.Remarks.Name = "Remarks"
        Me.Remarks.ReadOnly = True
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(935, 438)
        Me.Controls.Add(Me.pnlProcess)
        Me.Controls.Add(Me.pnlInfo)
        Me.Name = "MainForm"
        Me.Text = "MainForm"
        Me.pnlProcess.ResumeLayout(False)
        CType(Me.gvProcess, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlInfo.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlProcess As Panel
    Private gvProcess As DataGridView
    Private pnlInfo As Panel
    Private WithEvents btnRefresh As Button
    Friend WithEvents ProcID As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents ProcessName As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents Is64BitProcess As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents IsManaged As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents IsDotNet4 As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents IsWPF As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents IsConsole As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents Remarks As System.Windows.Forms.DataGridViewTextBoxColumn
End Class