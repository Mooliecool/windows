/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormTimeConsumingOpr
* Copyright (c) Microsoft Corporation.
* 
* The Time-consuming Operation sample demonstrates how to use the BackgroundWorker 
* component to execute a time-consuming operation in the background.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/25/2009 3:00 PM Zhi-Xin Ye Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
#endregion


namespace CSWinFormTimeConsumingOpr
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void frmTimeConsumingOpr_Load(object sender, EventArgs e)
        {
            //To set up for a background operation, add an event handler for the DoWork event.
            this.backgroundWorker1.DoWork += 
                new DoWorkEventHandler(backgroundWorker1_DoWork);

            //To receive notifications of progress updates, handle the ProgressChanged event.
            this.backgroundWorker1.ProgressChanged += 
                new ProgressChangedEventHandler(backgroundWorker1_ProgressChanged);

            //To receive a notification when the operation is completed, handle the 
            //RunWorkerCompleted event. 
            this.backgroundWorker1.RunWorkerCompleted += 
                new RunWorkerCompletedEventHandler(backgroundWorker1_RunWorkerCompleted);

            this.progressBar1.Maximum = 100;

            // This line ensures that we can report progress from the BackgroundWorker.  
            // By default progress reporting is disabled.
            this.backgroundWorker1.WorkerReportsProgress = true;   
        }

        void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
            // Do the time-consuming work here
            for (int i = 1; i <= 100; i++)
            {
                // Suspend the thread to simulate time-consuming operation.
                Thread.Sleep(100);
                // Report the current progress, calling this method will fire the
                // ProgressChanged event.
                this.backgroundWorker1.ReportProgress(i,i.ToString());
            }
        }

        void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            // Display the progress.
            this.progressBar1.Value = e.ProgressPercentage;
            this.lbResult.Text = e.UserState.ToString();
        }

        void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            MessageBox.Show("Working done!");
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            // Start the asynchronous operation.
            if (! this.backgroundWorker1.IsBusy)
            {
                this.backgroundWorker1.RunWorkerAsync();
            }
        }
    }
}
