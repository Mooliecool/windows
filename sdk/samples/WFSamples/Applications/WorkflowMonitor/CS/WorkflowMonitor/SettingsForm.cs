//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Windows.Forms;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //Settings dialog implementation - allows user to set application settings
    // such as server and database names
    internal partial class SettingsForm : Form
    {
        private ApplicationSettings monitorSettings;

        internal SettingsForm(Mainform owner)
        {
            InitializeComponent();

            this.Owner = owner;
            this.monitorSettings = owner.MonitorSettings;

            this.Database_Textbox.Text = monitorSettings.DatabaseName;
            this.Machine_TextBox.Text = monitorSettings.ServerName;
            this.Polling_TextBox.Text = monitorSettings.PollingInterval.ToString();
            this.AutoSelectLatest_Checkbox.Checked = monitorSettings.AutoSelectLatest;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            int previousPollingInterval = monitorSettings.PollingInterval;

            //Try to convert pollling interval to an int32 - if an error, revert to previous setting
            try
            {
                monitorSettings.DatabaseName = this.Database_Textbox.Text;
                monitorSettings.ServerName = this.Machine_TextBox.Text;
                monitorSettings.AutoSelectLatest = this.AutoSelectLatest_Checkbox.Checked;
                int pollingInterval = System.Convert.ToInt32(Polling_TextBox.Text);
                if (pollingInterval > 0)
                    monitorSettings.PollingInterval = pollingInterval;
                else
                {
                    MessageBox.Show("Polling interval must be greater than 0.", "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            catch
            {
                monitorSettings.PollingInterval = previousPollingInterval;
            }

            ((Mainform)Owner).MonitorDatabaseService.DatabaseName = monitorSettings.DatabaseName;
            ((Mainform)Owner).MonitorDatabaseService.ServerName = monitorSettings.ServerName;

            this.Close();
        }

        private void Cancel_Button_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}