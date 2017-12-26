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
using System.IO;
using System.Windows.Forms;
using System.Collections.ObjectModel;

namespace Microsoft.Samples.Workflow.FileTrackingServiceAndQuerySample
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void ButtonExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void ButtonGetWorkflowEvents_Click(object sender, EventArgs e)
        {
            try
            {
                if ((this.TextBoxInstanceId == null) || (this.TextBoxInstanceId.Text.Length == 0))
                {
                    MessageBox.Show("Please Enter a Valid Workflow Instance ID");
                }
                else
                {
                    string trackingFile = SimpleTrackingChannel.TrackingDataFilePrefix + this.TextBoxInstanceId.Text.ToString() + ".txt";
                    if (File.Exists(trackingFile))
                    {
                        LabelReadingStatus.Text = "reading...";
                        ListViewWorkflowEvents.Clear();
                        Collection<String> workflowEvents = QueryLayer.GetTrackedWorkflowEvents(trackingFile);
                        foreach (string workflowEvent in workflowEvents)
                        {
                            ListViewWorkflowEvents.Items.Add(workflowEvent);
                        }
                        LabelReadingStatus.Text = "";
                    }
                    else
                    {
                        MessageBox.Show("File " + trackingFile + " doesn't exist");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Encountered an exception. Exception Source: " + ex.Source.ToString() + ", Exception Message: " + ex.Message.ToString());
            }
            finally
            {
                LabelReadingStatus.Text = "";
            }
        }
    }
}