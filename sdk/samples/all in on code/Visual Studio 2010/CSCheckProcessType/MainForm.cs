/****************************** Module Header ******************************\
 Module Name:  MainForm.cs
 Project:      CSCheckProcessType
 Copyright (c) Microsoft Corporation.
 
 This is the main form of this application. It is used to initialize the UI and 
 handle the events.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows.Forms;

namespace CSCheckProcessType
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            if (!RunningProcess.IsOSVersionSupported)
            {
                MessageBox.Show("This application must run on Vista or later versions.");
                btnRefresh.Enabled = false;
            }
        }

        /// <summary>
        /// Handle btnRefresh_Click event.
        /// </summary>
        private void btnRefresh_Click(object sender, EventArgs e)
        {
            DatabindGridView();
        }

        /// <summary>
        /// Bind the datasource of the data gridview.
        /// </summary>
        void DatabindGridView()
        {
            List<RunningProcess> runningProcesses = new List<RunningProcess>();
            foreach (var proc in Process.GetProcesses().OrderBy(p => p.Id))
            {
                RunningProcess runningProcess = new RunningProcess(proc);
                runningProcesses.Add(runningProcess);
            }
            gvProcess.DataSource = runningProcesses;
        }
    }
}
