namespace CSCpuUsage
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }

            if (disposing && (processCpuUsageMonitor != null))
            {
                processCpuUsageMonitor.Dispose();
            }

            if (disposing && (totalCpuUsageMonitor != null))
            {
                totalCpuUsageMonitor.Dispose();
            }

            
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
            this.pnlCounter = new System.Windows.Forms.Panel();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.lbErrorMessage = new System.Windows.Forms.Label();
            this.cmbProcess = new System.Windows.Forms.ComboBox();
            this.chkProcessCpuUsage = new System.Windows.Forms.CheckBox();
            this.chkTotalUsage = new System.Windows.Forms.CheckBox();
            this.pnlChart = new System.Windows.Forms.Panel();
            this.chartProcessCupUsage = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chartTotalCpuUsage = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.pnlCounter.SuspendLayout();
            this.pnlChart.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartProcessCupUsage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartTotalCpuUsage)).BeginInit();
            this.SuspendLayout();
            // 
            // pnlCounter
            // 
            this.pnlCounter.Controls.Add(this.btnStop);
            this.pnlCounter.Controls.Add(this.btnStart);
            this.pnlCounter.Controls.Add(this.lbErrorMessage);
            this.pnlCounter.Controls.Add(this.cmbProcess);
            this.pnlCounter.Controls.Add(this.chkProcessCpuUsage);
            this.pnlCounter.Controls.Add(this.chkTotalUsage);
            this.pnlCounter.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlCounter.Location = new System.Drawing.Point(0, 0);
            this.pnlCounter.Name = "pnlCounter";
            this.pnlCounter.Size = new System.Drawing.Size(834, 57);
            this.pnlCounter.TabIndex = 0;
            // 
            // btnStop
            // 
            this.btnStop.Enabled = false;
            this.btnStop.Location = new System.Drawing.Point(108, 27);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(75, 23);
            this.btnStop.TabIndex = 3;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(12, 27);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 3;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // lbErrorMessage
            // 
            this.lbErrorMessage.AutoSize = true;
            this.lbErrorMessage.Location = new System.Drawing.Point(4, 76);
            this.lbErrorMessage.Name = "lbErrorMessage";
            this.lbErrorMessage.Size = new System.Drawing.Size(0, 13);
            this.lbErrorMessage.TabIndex = 2;
            // 
            // cmbProcess
            // 
            this.cmbProcess.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProcess.FormattingEnabled = true;
            this.cmbProcess.Location = new System.Drawing.Point(379, 3);
            this.cmbProcess.Name = "cmbProcess";
            this.cmbProcess.Size = new System.Drawing.Size(201, 21);
            this.cmbProcess.TabIndex = 1;
            this.cmbProcess.DropDown += new System.EventHandler(this.cmbProcess_DropDown);
            // 
            // chkProcessCpuUsage
            // 
            this.chkProcessCpuUsage.AutoSize = true;
            this.chkProcessCpuUsage.Location = new System.Drawing.Point(175, 4);
            this.chkProcessCpuUsage.Name = "chkProcessCpuUsage";
            this.chkProcessCpuUsage.Size = new System.Drawing.Size(198, 17);
            this.chkProcessCpuUsage.TabIndex = 0;
            this.chkProcessCpuUsage.Text = "Display the CPU Usage of a process";
            this.chkProcessCpuUsage.UseVisualStyleBackColor = true;
            // 
            // chkTotalUsage
            // 
            this.chkTotalUsage.AutoSize = true;
            this.chkTotalUsage.Checked = true;
            this.chkTotalUsage.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkTotalUsage.Location = new System.Drawing.Point(4, 4);
            this.chkTotalUsage.Name = "chkTotalUsage";
            this.chkTotalUsage.Size = new System.Drawing.Size(142, 17);
            this.chkTotalUsage.TabIndex = 0;
            this.chkTotalUsage.Text = "Display total CPU Usage";
            this.chkTotalUsage.UseVisualStyleBackColor = true;
            // 
            // pnlChart
            // 
            this.pnlChart.Controls.Add(this.chartProcessCupUsage);
            this.pnlChart.Controls.Add(this.chartTotalCpuUsage);
            this.pnlChart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlChart.Location = new System.Drawing.Point(0, 57);
            this.pnlChart.Name = "pnlChart";
            this.pnlChart.Size = new System.Drawing.Size(834, 533);
            this.pnlChart.TabIndex = 1;
            // 
            // chartProcessCupUsage
            // 
            chartArea1.AxisX.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
            chartArea1.AxisX.LabelStyle.Enabled = false;
            chartArea1.AxisX.MajorGrid.Enabled = false;
            chartArea1.AxisX.MajorTickMark.Enabled = false;
            chartArea1.AxisX.Maximum = 100D;
            chartArea1.AxisX.Minimum = 0D;
            chartArea1.AxisX2.Maximum = 100D;
            chartArea1.AxisX2.Minimum = 0D;
            chartArea1.AxisY.IsMarginVisible = false;
            chartArea1.Name = "ChartAreaProcessCpuUsage";
            this.chartProcessCupUsage.ChartAreas.Add(chartArea1);
            this.chartProcessCupUsage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.chartProcessCupUsage.Location = new System.Drawing.Point(0, 279);
            this.chartProcessCupUsage.Name = "chartProcessCupUsage";
            series1.ChartArea = "ChartAreaProcessCpuUsage";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series1.Name = "ProcessCpuUsageSeries";
            series1.ToolTip = "TotalCpuUsageSeries";
            series1.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            series1.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            this.chartProcessCupUsage.Series.Add(series1);
            this.chartProcessCupUsage.Size = new System.Drawing.Size(834, 254);
            this.chartProcessCupUsage.SuppressExceptions = true;
            this.chartProcessCupUsage.TabIndex = 1;
            this.chartProcessCupUsage.Text = "Process CPU Usage";
            title1.Name = "TitleProcess";
            title1.Text = "Process CPU Usage";
            this.chartProcessCupUsage.Titles.Add(title1);
            // 
            // chartTotalCpuUsage
            // 
            chartArea2.AxisX.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
            chartArea2.AxisX.LabelStyle.Enabled = false;
            chartArea2.AxisX.MajorGrid.Enabled = false;
            chartArea2.AxisX.MajorTickMark.Enabled = false;
            chartArea2.AxisX.Maximum = 100D;
            chartArea2.AxisX.Minimum = 0D;
            chartArea2.AxisY.IsMarginVisible = false;
            chartArea2.Name = "ChartAreaTotalCpuUsage";
            this.chartTotalCpuUsage.ChartAreas.Add(chartArea2);
            this.chartTotalCpuUsage.Dock = System.Windows.Forms.DockStyle.Top;
            this.chartTotalCpuUsage.Location = new System.Drawing.Point(0, 0);
            this.chartTotalCpuUsage.Name = "chartTotalCpuUsage";
            series2.ChartArea = "ChartAreaTotalCpuUsage";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series2.Name = "TotalCpuUsageSeries";
            series2.ToolTip = "TotalCpuUsageSeries";
            series2.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            series2.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            this.chartTotalCpuUsage.Series.Add(series2);
            this.chartTotalCpuUsage.Size = new System.Drawing.Size(834, 279);
            this.chartTotalCpuUsage.SuppressExceptions = true;
            this.chartTotalCpuUsage.TabIndex = 0;
            this.chartTotalCpuUsage.Text = "Total CPU Usage";
            title2.Name = "TitleTotal";
            title2.Text = "Total CPU Usage";
            this.chartTotalCpuUsage.Titles.Add(title2);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(834, 590);
            this.Controls.Add(this.pnlChart);
            this.Controls.Add(this.pnlCounter);
            this.Name = "MainForm";
            this.Text = "CSCpuUsageMonitor";
            this.pnlCounter.ResumeLayout(false);
            this.pnlCounter.PerformLayout();
            this.pnlChart.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.chartProcessCupUsage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartTotalCpuUsage)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pnlCounter;
        private System.Windows.Forms.Label lbErrorMessage;
        private System.Windows.Forms.ComboBox cmbProcess;
        private System.Windows.Forms.CheckBox chkProcessCpuUsage;
        private System.Windows.Forms.CheckBox chkTotalUsage;
        private System.Windows.Forms.Panel pnlChart;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartTotalCpuUsage;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartProcessCupUsage;
        private System.Windows.Forms.Button btnStop;
    }
}

