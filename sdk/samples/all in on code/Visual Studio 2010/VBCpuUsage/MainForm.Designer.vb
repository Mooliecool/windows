
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

        If disposing AndAlso (_processCpuUsageMonitor IsNot Nothing) Then
            _processCpuUsageMonitor.Dispose()
        End If

        If disposing AndAlso (_totalCpuUsageMonitor IsNot Nothing) Then
            _totalCpuUsageMonitor.Dispose()
        End If


        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Dim ChartArea3 As System.Windows.Forms.DataVisualization.Charting.ChartArea = New System.Windows.Forms.DataVisualization.Charting.ChartArea()
        Dim Series3 As System.Windows.Forms.DataVisualization.Charting.Series = New System.Windows.Forms.DataVisualization.Charting.Series()
        Dim Title3 As System.Windows.Forms.DataVisualization.Charting.Title = New System.Windows.Forms.DataVisualization.Charting.Title()
        Dim ChartArea4 As System.Windows.Forms.DataVisualization.Charting.ChartArea = New System.Windows.Forms.DataVisualization.Charting.ChartArea()
        Dim Series4 As System.Windows.Forms.DataVisualization.Charting.Series = New System.Windows.Forms.DataVisualization.Charting.Series()
        Dim Title4 As System.Windows.Forms.DataVisualization.Charting.Title = New System.Windows.Forms.DataVisualization.Charting.Title()
        Me.pnlCounter = New System.Windows.Forms.Panel()
        Me.btnStop = New System.Windows.Forms.Button()
        Me.btnStart = New System.Windows.Forms.Button()
        Me.lbErrorMessage = New System.Windows.Forms.Label()
        Me.cmbProcess = New System.Windows.Forms.ComboBox()
        Me.chkProcessCpuUsage = New System.Windows.Forms.CheckBox()
        Me.chkTotalUsage = New System.Windows.Forms.CheckBox()
        Me.pnlChart = New System.Windows.Forms.Panel()
        Me.chartProcessCupUsage = New System.Windows.Forms.DataVisualization.Charting.Chart()
        Me.chartTotalCpuUsage = New System.Windows.Forms.DataVisualization.Charting.Chart()
        Me.pnlCounter.SuspendLayout()
        Me.pnlChart.SuspendLayout()
        CType(Me.chartProcessCupUsage, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chartTotalCpuUsage, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'pnlCounter
        '
        Me.pnlCounter.Controls.Add(Me.btnStop)
        Me.pnlCounter.Controls.Add(Me.btnStart)
        Me.pnlCounter.Controls.Add(Me.lbErrorMessage)
        Me.pnlCounter.Controls.Add(Me.cmbProcess)
        Me.pnlCounter.Controls.Add(Me.chkProcessCpuUsage)
        Me.pnlCounter.Controls.Add(Me.chkTotalUsage)
        Me.pnlCounter.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlCounter.Location = New System.Drawing.Point(0, 0)
        Me.pnlCounter.Name = "pnlCounter"
        Me.pnlCounter.Size = New System.Drawing.Size(834, 57)
        Me.pnlCounter.TabIndex = 0
        '
        'btnStop
        '
        Me.btnStop.Enabled = False
        Me.btnStop.Location = New System.Drawing.Point(108, 27)
        Me.btnStop.Name = "btnStop"
        Me.btnStop.Size = New System.Drawing.Size(75, 23)
        Me.btnStop.TabIndex = 3
        Me.btnStop.Text = "Stop"
        Me.btnStop.UseVisualStyleBackColor = True
        '
        'btnStart
        '
        Me.btnStart.Location = New System.Drawing.Point(12, 27)
        Me.btnStart.Name = "btnStart"
        Me.btnStart.Size = New System.Drawing.Size(75, 23)
        Me.btnStart.TabIndex = 3
        Me.btnStart.Text = "Start"
        Me.btnStart.UseVisualStyleBackColor = True
        '
        'lbErrorMessage
        '
        Me.lbErrorMessage.AutoSize = True
        Me.lbErrorMessage.Location = New System.Drawing.Point(4, 76)
        Me.lbErrorMessage.Name = "lbErrorMessage"
        Me.lbErrorMessage.Size = New System.Drawing.Size(0, 13)
        Me.lbErrorMessage.TabIndex = 2
        '
        'cmbProcess
        '
        Me.cmbProcess.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbProcess.FormattingEnabled = True
        Me.cmbProcess.Location = New System.Drawing.Point(379, 3)
        Me.cmbProcess.Name = "cmbProcess"
        Me.cmbProcess.Size = New System.Drawing.Size(201, 21)
        Me.cmbProcess.TabIndex = 1
        '
        'chkProcessCpuUsage
        '
        Me.chkProcessCpuUsage.AutoSize = True
        Me.chkProcessCpuUsage.Location = New System.Drawing.Point(175, 4)
        Me.chkProcessCpuUsage.Name = "chkProcessCpuUsage"
        Me.chkProcessCpuUsage.Size = New System.Drawing.Size(198, 17)
        Me.chkProcessCpuUsage.TabIndex = 0
        Me.chkProcessCpuUsage.Text = "Display the CPU Usage of a process"
        Me.chkProcessCpuUsage.UseVisualStyleBackColor = True
        '
        'chkTotalUsage
        '
        Me.chkTotalUsage.AutoSize = True
        Me.chkTotalUsage.Checked = True
        Me.chkTotalUsage.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkTotalUsage.Location = New System.Drawing.Point(4, 4)
        Me.chkTotalUsage.Name = "chkTotalUsage"
        Me.chkTotalUsage.Size = New System.Drawing.Size(142, 17)
        Me.chkTotalUsage.TabIndex = 0
        Me.chkTotalUsage.Text = "Display total CPU Usage"
        Me.chkTotalUsage.UseVisualStyleBackColor = True
        '
        'pnlChart
        '
        Me.pnlChart.Controls.Add(Me.chartProcessCupUsage)
        Me.pnlChart.Controls.Add(Me.chartTotalCpuUsage)
        Me.pnlChart.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlChart.Location = New System.Drawing.Point(0, 57)
        Me.pnlChart.Name = "pnlChart"
        Me.pnlChart.Size = New System.Drawing.Size(834, 533)
        Me.pnlChart.TabIndex = 1
        '
        'chartProcessCupUsage
        '
        ChartArea3.AxisX.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number
        ChartArea3.AxisX.LabelStyle.Enabled = False
        ChartArea3.AxisX.MajorGrid.Enabled = False
        ChartArea3.AxisX.MajorTickMark.Enabled = False
        ChartArea3.AxisX.Maximum = 100.0R
        ChartArea3.AxisX.Minimum = 0.0R
        ChartArea3.AxisY.IsMarginVisible = False
        ChartArea3.Name = "ChartAreaProcessCpuUsage"
        Me.chartProcessCupUsage.ChartAreas.Add(ChartArea3)
        Me.chartProcessCupUsage.Dock = System.Windows.Forms.DockStyle.Fill
        Me.chartProcessCupUsage.Location = New System.Drawing.Point(0, 279)
        Me.chartProcessCupUsage.Name = "chartProcessCupUsage"
        Series3.ChartArea = "ChartAreaProcessCpuUsage"
        Series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line
        Series3.Name = "ProcessCpuUsageSeries"
        Series3.ToolTip = "TotalCpuUsageSeries"
        Series3.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32
        Series3.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.[Double]
        Me.chartProcessCupUsage.Series.Add(Series3)
        Me.chartProcessCupUsage.Size = New System.Drawing.Size(834, 254)
        Me.chartProcessCupUsage.SuppressExceptions = True
        Me.chartProcessCupUsage.TabIndex = 1
        Me.chartProcessCupUsage.Text = "Process CPU Usage"
        Title3.Name = "TitleProcess"
        Title3.Text = "Process CPU Usage"
        Me.chartProcessCupUsage.Titles.Add(Title3)
        '
        'chartTotalCpuUsage
        '
        ChartArea4.AxisX.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number
        ChartArea4.AxisX.LabelStyle.Enabled = False
        ChartArea4.AxisX.MajorGrid.Enabled = False
        ChartArea4.AxisX.MajorTickMark.Enabled = False
        ChartArea4.AxisX.Maximum = 100.0R
        ChartArea4.AxisX.Minimum = 0.0R
        ChartArea4.AxisY.IsMarginVisible = False
        ChartArea4.Name = "ChartAreaTotalCpuUsage"
        Me.chartTotalCpuUsage.ChartAreas.Add(ChartArea4)
        Me.chartTotalCpuUsage.Dock = System.Windows.Forms.DockStyle.Top
        Me.chartTotalCpuUsage.Location = New System.Drawing.Point(0, 0)
        Me.chartTotalCpuUsage.Name = "chartTotalCpuUsage"
        Series4.ChartArea = "ChartAreaTotalCpuUsage"
        Series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line
        Series4.Name = "TotalCpuUsageSeries"
        Series4.ToolTip = "TotalCpuUsageSeries"
        Series4.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32
        Series4.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.[Double]
        Me.chartTotalCpuUsage.Series.Add(Series4)
        Me.chartTotalCpuUsage.Size = New System.Drawing.Size(834, 279)
        Me.chartTotalCpuUsage.SuppressExceptions = True
        Me.chartTotalCpuUsage.TabIndex = 0
        Me.chartTotalCpuUsage.Text = "Total CPU Usage"
        Title4.Name = "TitleTotal"
        Title4.Text = "Total CPU Usage"
        Me.chartTotalCpuUsage.Titles.Add(Title4)
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(834, 590)
        Me.Controls.Add(Me.pnlChart)
        Me.Controls.Add(Me.pnlCounter)
        Me.Name = "MainForm"
        Me.Text = "VBCpuUsageMonitor"
        Me.pnlCounter.ResumeLayout(False)
        Me.pnlCounter.PerformLayout()
        Me.pnlChart.ResumeLayout(False)
        CType(Me.chartProcessCupUsage, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chartTotalCpuUsage, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlCounter As Panel
    Private lbErrorMessage As Label
    Private WithEvents cmbProcess As ComboBox
    Private chkProcessCpuUsage As CheckBox
    Private chkTotalUsage As CheckBox
    Private pnlChart As Panel
    Private chartTotalCpuUsage As DataVisualization.Charting.Chart
    Private WithEvents btnStart As Button
    Private chartProcessCupUsage As DataVisualization.Charting.Chart
    Private WithEvents btnStop As Button
End Class

