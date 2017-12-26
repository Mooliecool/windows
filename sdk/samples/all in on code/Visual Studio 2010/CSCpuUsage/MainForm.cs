/****************************** Module Header ******************************\
 * Module Name:  MainForm.cs
 * Project:      CSCpuUsage
 * Copyright (c) Microsoft Corporation.
 * 
 * This is the main form of this application. It is used to handle the UI
 * event and display the CPU usage charts.
 * 
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Windows.Forms;

namespace CSCpuUsage
{
    public partial class MainForm : Form
    {

        int[] xValues=new int[100];

        // The monitor of the total CPU usage.
        TotalCpuUsageMonitor totalCpuUsageMonitor;

        // The CPU usage monitor of a specified process.
        ProcessCpuUsageMonitor processCpuUsageMonitor;

        public MainForm()
        {
            InitializeComponent();
            for (int i = 0; i < 100; i++)
            {
                xValues[i] = i;
            }
        }

        /// <summary>
        ///  Add the available processes to the combobox.
        /// </summary>
        private void cmbProcess_DropDown(object sender, EventArgs e)
        {
            cmbProcess.DataSource = ProcessCpuUsageMonitor.GetAvailableProcesses();
            cmbProcess.SelectedIndex = 0;
        }

        /// <summary>
        /// Handle the btnStart Click event,
        /// </summary>
        private void btnStart_Click(object sender, EventArgs e)
        {
            if (!chkTotalUsage.Checked && !chkProcessCpuUsage.Checked)
            {
                return;
            }

            try
            {
                StartMonitor();
            }
            catch(Exception ex)
            {
                StopMonitor();
                MessageBox.Show(ex.Message);             
            }
        }

        /// <summary>
        /// Handle the btnStop Click event.
        /// If this button is clicked, dispose totalCpuUsageMonitor and processCpuUsageMonitor.
        /// </summary>
        private void btnStop_Click(object sender, EventArgs e)
        {
            StopMonitor();
        }

        void StartMonitor()
        {
            // Initialize the totalCpuUsageMonitor and register the CpuUsageValueArrayChanged
            // event.
            if (chkTotalUsage.Checked)
            {
                totalCpuUsageMonitor = new TotalCpuUsageMonitor(1000, 100);
                totalCpuUsageMonitor.CpuUsageValueArrayChanged +=
                    new EventHandler<CpuUsageValueArrayChangedEventArg>(
                        totalCpuUsageMonitor_CpuUsageValueArrayChanged);
                totalCpuUsageMonitor.ErrorOccurred += new EventHandler<ErrorEventArgs>(
                    totalCpuUsageMonitor_ErrorOccurred);

            }

            // Initialize the processCpuUsageMonitor and register the CpuUsageValueArrayChanged
            // event.
            if (chkProcessCpuUsage.Checked && !string.IsNullOrEmpty(cmbProcess.SelectedItem as string))
            {
                processCpuUsageMonitor =
                    new ProcessCpuUsageMonitor(cmbProcess.SelectedItem as string, 1000, 100);
                processCpuUsageMonitor.CpuUsageValueArrayChanged +=
                    new EventHandler<CpuUsageValueArrayChangedEventArg>(
                        processCpuUsageMonitor_CpuUsageValueArrayChanged);
                processCpuUsageMonitor.ErrorOccurred += new EventHandler<ErrorEventArgs>(
                    processCpuUsageMonitor_ErrorOccurred);
            }

            // Update the UI.
            btnStart.Enabled = false;
            btnStop.Enabled = true;
        }

       

        void StopMonitor()
        {
            if (totalCpuUsageMonitor != null)
            {
                totalCpuUsageMonitor.Dispose();
                totalCpuUsageMonitor = null;
            }

            if (processCpuUsageMonitor != null)
            {
                processCpuUsageMonitor.Dispose();
                processCpuUsageMonitor = null;
            }

            // Update the UI.
            btnStart.Enabled = true;
            btnStop.Enabled = false;
        }

        /// <summary>
        /// Invoke the processCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
        /// the CpuUsageValueArrayChanged event of processCpuUsageMonitor.
        /// </summary>
        void processCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                processCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }

        void processCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender,
            CpuUsageValueArrayChangedEventArg e)
        {
            var processCpuUsageSeries = chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
            var values = e.Values;

            // Display the process CPU usage in the chart.
            processCpuUsageSeries.Points.DataBindY(e.Values);

        }

        /// <summary>
        /// Invoke the processCpuUsageMonitor_ErrorOccurredHandler to handle
        /// the ErrorOccurred event of processCpuUsageMonitor.
        /// </summary>
        void processCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                processCpuUsageMonitor_ErrorOccurredHandler), sender, e);
        }

        void processCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (processCpuUsageMonitor != null)
            {
                processCpuUsageMonitor.Dispose();
                processCpuUsageMonitor = null;

                var processCpuUsageSeries = chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
                processCpuUsageSeries.Points.Clear();
            }
            MessageBox.Show(e.Error.Message);
        }



        /// <summary>
        /// Invoke the totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
        /// the CpuUsageValueArrayChanged event of totalCpuUsageMonitor.
        /// </summary>
        void totalCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }
        void totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            var totalCpuUsageSeries = chartTotalCpuUsage.Series["TotalCpuUsageSeries"];
            var values = e.Values;

            // Display the total CPU usage in the chart.
            totalCpuUsageSeries.Points.DataBindY(e.Values);

        }

        /// <summary>
        /// Invoke the totalCpuUsageMonitor_ErrorOccurredHandler to handle
        /// the ErrorOccurred event of totalCpuUsageMonitor.
        /// </summary>
        void totalCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                totalCpuUsageMonitor_ErrorOccurredHandler),sender,e);
        }

        void totalCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (totalCpuUsageMonitor != null)
            {
                totalCpuUsageMonitor.Dispose();
                totalCpuUsageMonitor = null;

                var totalCpuUsageSeries = chartTotalCpuUsage.Series["TotalCpuUsageSeries"];
                totalCpuUsageSeries.Points.Clear();
            }
            MessageBox.Show(e.Error.Message);
        }
    }
}
