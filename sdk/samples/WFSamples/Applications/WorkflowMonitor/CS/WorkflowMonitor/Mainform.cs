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
using System.Collections.Generic;
using System.Windows.Forms;
using System.Workflow.Runtime.Tracking;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //Main form for the monitor - settings info is read from Application.LocalUserAppDataPath @"\workflowmonitor.config"
    public partial class Mainform : Form
    {
        List<SqlTrackingWorkflowInstance> displayedWorkflows = null;
        string selectedWorkflowEvent = string.Empty;
        Guid workflowInstanceIdToFind = Guid.Empty;

        DateTime statusFromDateTime = new DateTime(2000, 1, 1);
        DateTime statusUntilDateTime = DateTime.Now.AddDays(1);

        TrackingDataItemValue trackingDataItemValue = new TrackingDataItemValue(string.Empty, string.Empty, string.Empty);

        private ApplicationSettings monitorSettingsValue;
        private DatabaseService monitorDatabaseServiceValue;

        private Boolean isMonitoring = false;

        private Dictionary<string, WorkflowStatusInfo> workflowStatusList = null;
        private Dictionary<string, ActivityStatusInfo> activityStatusListValue = null;

        private ViewHost workflowViewHost;

        public Mainform()
        {
            InitializeComponent();

            this.toolStripTextBoxFrom.Text = statusFromDateTime.ToString();
            this.toolStripTextBoxUntil.Text = statusUntilDateTime.ToString();
            this.toolStripTextBoxWorkflowInstanceId.Text = Guid.Empty.ToString();
            this.listViewWorkflows.ListViewItemSorter = new ListViewItemComparer(0, false);
            this.listViewActivities.ListViewItemSorter = new ListViewItemComparer(0, false);

            workflowViewHost = new ViewHost(this);
            workflowViewHost.ZoomChanged += new EventHandler<ViewHost.ZoomChangedEventArgs>(WorkflowViewHost_ZoomChanged);

            this.trackingSurface.Panel2.SuspendLayout();
            this.trackingSurface.Panel2.Controls.Clear();
            this.trackingSurface.Panel2.Controls.Add(viewHostLabel);
            this.trackingSurface.Panel2.Controls.Add(workflowViewHost);
            this.trackingSurface.Panel2.Controls.Add(workflowViewErrorText); 
            this.trackingSurface.Panel2.ResumeLayout(true);

            this.monitorSettingsValue = new ApplicationSettings();
            this.monitorDatabaseServiceValue = new DatabaseService();
            this.workflowStatusList = new Dictionary<string, WorkflowStatusInfo>();
            this.activityStatusListValue = new Dictionary<string, ActivityStatusInfo>();

            this.monitorSettingsValue.DatabaseName = monitorDatabaseServiceValue.DatabaseName;
            this.monitorSettingsValue.ServerName = monitorDatabaseServiceValue.ServerName;


            //Read app settings
            if (monitorSettingsValue.LoadAppSettings(Application.LocalUserAppDataPath + @"\workflowmonitor.config"))
            {
                monitorDatabaseServiceValue.DatabaseName = monitorSettingsValue.DatabaseName;
                monitorDatabaseServiceValue.ServerName = monitorSettingsValue.ServerName;
            }
            else
            {
                //If no application settings default to localhost and Tracking for server and database
                monitorSettingsValue.DatabaseName = "Tracking";
                monitorSettingsValue.ServerName = "LocalHost";
                monitorSettingsValue.PollingInterval = 5000;
                monitorSettingsValue.AutoSelectLatest = false;
                monitorDatabaseServiceValue.DatabaseName = monitorSettingsValue.DatabaseName;
                monitorDatabaseServiceValue.ServerName = monitorSettingsValue.ServerName;
            }

            this.statusLabelDatabaseName.Text = "Connected to: " + monitorDatabaseServiceValue.ServerName + "/" + monitorDatabaseServiceValue.DatabaseName;

            DisplayWorkflows(selectedWorkflowEvent);
        }

        private void DisplayWorkflowById(Guid workflowInstanceId)
        {
            DisplayWorkflows(null, workflowInstanceId, statusFromDateTime, statusUntilDateTime, trackingDataItemValue);
        }

        private void DisplayWorkflows(string selectedWorkflowEvent)
        {
            DisplayWorkflows(selectedWorkflowEvent, Guid.Empty, statusFromDateTime, statusUntilDateTime, trackingDataItemValue);
        }

        private void DisplayWorkflows(string selectedWorkflowEvent, DateTime statusFrom, DateTime statusUntil, TrackingDataItemValue trackingDataItemValue)
        {
            DisplayWorkflows(selectedWorkflowEvent, Guid.Empty, statusFrom, statusUntil, trackingDataItemValue);
        }

        private void DisplayWorkflows(string selectedWorkflowEvent, Guid workflowInstanceId, DateTime statusFrom, DateTime statusUntil, TrackingDataItemValue trackingDataItemValue)
        {
            //Try to get all of the workflows from the tracking database
            try
            {
                if ((null != workflowInstanceId) && (Guid.Empty != workflowInstanceId))
                {
                    displayedWorkflows.Clear();
                    SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance = null;
                    if (true == monitorDatabaseServiceValue.TryGetWorkflow(workflowInstanceId, out sqlTrackingWorkflowInstance))
                        displayedWorkflows.Add(sqlTrackingWorkflowInstance);
                }
                else
                {
                    displayedWorkflows = monitorDatabaseServiceValue.GetWorkflows(selectedWorkflowEvent, statusFrom, statusUntil, trackingDataItemValue);
                }
                listViewWorkflows.Items.Clear();
                workflowStatusList.Clear();

                // For every workflow instance create a new WorkflowStatusInfo object and store in the workflowStatusList
                // Also populate the workflow ListView
                foreach (SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance in displayedWorkflows)
                {
                    string workflowType = (sqlTrackingWorkflowInstance.WorkflowType != null) ? sqlTrackingWorkflowInstance.WorkflowType.ToString() : "XAML";
                    ListViewItem listViewItem = new ListViewItem(new string[] {
                        sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                        workflowType,
                        sqlTrackingWorkflowInstance.Status.ToString()}, -1);
                    
                    listViewWorkflows.Items.Add(listViewItem);

                    workflowStatusList.Add(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                            new WorkflowStatusInfo(
                                sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                                workflowType,
                                sqlTrackingWorkflowInstance.Status.ToString(),
                                sqlTrackingWorkflowInstance.Initialized.ToString(),
                                sqlTrackingWorkflowInstance.WorkflowInstanceId, 
                                listViewItem));
                }

                //If there is at least one workflow, populate the Activities list
                if (listViewWorkflows.Items.Count > 0)
                {
                    this.listViewWorkflows.Focus();
                    ListViewItem listItem = this.listViewWorkflows.Items[0];
                    listItem.Focused = true;
                    listItem.Selected = true;
                    UpdateTitle();
                    UpdateActivities();
                    this.statusLabelMonitoring.Text = String.Empty;
                }

                //Display number of workflow instances
                if (displayedWorkflows.Count > 0)
                {
                    this.workflowsLabel.Text = " Workflows - " + displayedWorkflows.Count + " records";
                    ShowViewHost(true);
                }
                else
                {
                    this.workflowsLabel.Text = " Workflows - no records";
                    ShowViewHost(false);
                }
            }
            //Clear all of the lists and reset the UI if there are errors
            catch (Exception e)
            {
                if (e.InnerException != null)
                {
                    MessageBox.Show(e.Message + "\r\n" + e.InnerException.Message + "\r\n" + "Ensure your settings are correct and that you have run SqlTrackingService database schema and logic scripts", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }  
                else
                    MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                this.statusLabelMonitoring.Text = "Error";
                ShowSettingsDialog();

                workflowStatusList.Clear();
                listViewWorkflows.Items.Clear();

                activityStatusListValue.Clear();
                listViewActivities.Items.Clear();

                this.workflowsLabel.Text = " Workflows - no records";
                ShowViewHost(false);
            }
        }
        internal DatabaseService MonitorDatabaseService
        {
            get { return monitorDatabaseServiceValue; }
        }

        internal Dictionary<string, ActivityStatusInfo> ActivityStatusList
        {
            get { return activityStatusListValue; }
        }

        internal ApplicationSettings MonitorSettings
        {
            get { return monitorSettingsValue; }
        }

        //F5 refresh
        internal void ManualRefresh(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F5)
            {
                DisplayWorkflows(selectedWorkflowEvent);
            }
        }
        //Sort the workflow list if the user clicks the column headers
        private void OnWorkflowsColumnClick(object sender, ColumnClickEventArgs e)
        {
            //If the column is the id column then sort numeric
            if (e.Column == 0)
                this.listViewWorkflows.ListViewItemSorter = new ListViewItemComparer(e.Column, false);
            else
                this.listViewWorkflows.ListViewItemSorter = new ListViewItemComparer(e.Column, true);
        }

        //Sort the activities list if the user clicks the column headers
        private void OnActivitiesColumnClick(object sender, ColumnClickEventArgs e)
        {
            //If the column is the id column then sort numeric
            if (e.Column == 0)
                this.listViewActivities.ListViewItemSorter = new ListViewItemComparer(e.Column, false);
            else
                this.listViewActivities.ListViewItemSorter = new ListViewItemComparer(e.Column, true);
        }

        //Set the window title to be the name of the workflow if one is selected
        private void UpdateTitle()
        {
            if (listViewWorkflows.SelectedItems.Count == 0 || this.displayedWorkflows.Count == 0)
            {
                this.Text = "Workflow Monitor";
                return;
            }

            ListViewItem currentSelectedWorkflow = listViewWorkflows.SelectedItems[0];
            if (currentSelectedWorkflow != null)
                this.Text = currentSelectedWorkflow.SubItems[1].Text + " - Workflow Monitor";
            else
                this.Text = "Workflow Monitor";
        }

        protected override void OnLayout(LayoutEventArgs levent)
        {
            base.OnLayout(levent);

            this.statusStrip.SuspendLayout();
            this.statusLabelDatabaseName.Visible = true;
            this.statusLabelMonitoring.Visible = true;

            if (this.Width < this.statusLabelDatabaseName.Width + this.statusLabelMonitoring.Width + 10)
                this.statusLabelDatabaseName.Visible = false;

            if (this.Width < this.statusLabelMonitoring.Width + 10)
                this.statusLabelMonitoring.Visible = false;

            this.statusStrip.ResumeLayout(true);
        }

        private void ToolStripButtonSettings_Click(object sender, EventArgs e)
        {
            MenuSettings_Click(sender, e);
        }

        private void MenuSettings_Click(object sender, System.EventArgs e)
        {
            ShowSettingsDialog();
        }

        //Display the settings dialog to the user and set the monitor UI
        private void ShowSettingsDialog()
        {
            SettingsForm settings = new SettingsForm(this);
            settings.ShowDialog();

            if (MonitorSettings.PollingInterval > 0)
                timer.Interval = MonitorSettings.PollingInterval;

            this.statusLabelDatabaseName.Text = "Connected to: " + monitorDatabaseServiceValue.ServerName + "/" + monitorDatabaseServiceValue.DatabaseName;
            if (isMonitoring && MonitorSettings.AutoSelectLatest && (listViewWorkflows.Items.Count > 0))
            {
                listViewWorkflows.Focus();
                listViewWorkflows.Items[listViewWorkflows.Items.Count - 1].Selected = true;
            }
        }

        private void ToolStripButtonMonitorOn_Click(object sender, EventArgs e)
        {
            Monitor(true);
        }

        private void ToolStripButtonMonitorOff_Click(object sender, EventArgs e)
        {
            Monitor(false);
        }

        private void MenuStartStop_Click(object sender, System.EventArgs e)
        {
            if (menuStartStop.Text == "Start")
                Monitor(true);
            else
                Monitor(false);
        }

        private void ToolStripButtonCollapse_Click(object sender, EventArgs e)
        {
            this.workflowViewHost.Expand(false);
        }

        private void ToolStripButtonExpand_Click(object sender, EventArgs e)
        {
            this.workflowViewHost.Expand(true);
        }

        //Turn the real-time monitoring on and off
        private void Monitor(bool toggle)
        {
            if (toggle)
            {
                timer.Start();
                menuStartStop.Text = "Stop";
                toolStripButtonMonitorOff.Enabled = true;
                toolStripButtonMonitorOn.Enabled = false;
                this.statusLabelMonitoring.Text = "Monitoring";

                if (MonitorSettings.AutoSelectLatest && (listViewWorkflows.Items.Count > 0))
                {
                    listViewWorkflows.Focus();
                    listViewWorkflows.Items[listViewWorkflows.Items.Count - 1].Selected = true;
                }
                this.isMonitoring = true;
            }
            else
            {
                timer.Stop();
                this.isMonitoring = false;
                menuStartStop.Text = "Start";
                toolStripButtonMonitorOff.Enabled = false;
                toolStripButtonMonitorOn.Enabled = true;
                this.statusLabelMonitoring.Text = String.Empty;
            }
        }

        private void MenuExit_Click(object sender, System.EventArgs e)
        {
            this.Close();
        }

        //Zoom level combo box handling
        private void ToolStripComboBoxZoom_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (workflowViewHost.WorkflowView == null) return;
            //Parse the value and set the WorkflowView zoom - set to 100% if invalid
            string newZoom = this.toolStripComboBoxZoom.Text.Trim();
            if (newZoom.EndsWith("%"))
                newZoom = newZoom.Substring(0, newZoom.Length - 1);

            if (newZoom.Length > 0)
            {
                try
                {
                    this.workflowViewHost.WorkflowView.Zoom = Convert.ToInt32(newZoom);
                }
                catch
                {
                    this.workflowViewHost.WorkflowView.Zoom = 100;
                }
            }
            else
                this.workflowViewHost.WorkflowView.Zoom = 100;
        }


        //Refresh info from the database (invoked by F5 refresh and polling timer)
        private void RefreshView()
        {
            try
            {
                //Get all of the workflow instances from the database and refresh the status if changed
                displayedWorkflows = monitorDatabaseServiceValue.GetWorkflows(selectedWorkflowEvent, statusFromDateTime, statusUntilDateTime, trackingDataItemValue);
                foreach (SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance in displayedWorkflows)
                {
                    WorkflowStatusInfo workflowStatus;
                    if (workflowStatusList.TryGetValue(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), out workflowStatus))
                    {
                        workflowStatus.Status = sqlTrackingWorkflowInstance.Status.ToString();
                        workflowStatus.WorkflowListViewItem.SubItems[2].Text = sqlTrackingWorkflowInstance.Status.ToString();
                    }
                    else
                    {
                    string workflowType = (sqlTrackingWorkflowInstance.WorkflowType != null) ? sqlTrackingWorkflowInstance.WorkflowType.ToString() : "XAML";
                        ListViewItem listViewItem = new ListViewItem(new string[] {
                            sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                            workflowType,
                            sqlTrackingWorkflowInstance.Status.ToString()}, -1);
                        listViewWorkflows.Items.Add(listViewItem);
                        listViewWorkflows.Focus();

                        workflowStatusList.Add(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                                new WorkflowStatusInfo(
                                sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(),
                                workflowType,
                                sqlTrackingWorkflowInstance.Status.ToString(),
                                sqlTrackingWorkflowInstance.Initialized.ToString(),
                                sqlTrackingWorkflowInstance.WorkflowInstanceId,
                                listViewItem));

                        if (MonitorSettings.AutoSelectLatest)
                            listViewItem.Selected = true;
                    }
                }

                if (displayedWorkflows.Count > 0)
                {
                    this.workflowsLabel.Text = " Workflows - " + displayedWorkflows.Count.ToString() + " records";
                    ShowViewHost(true);
                    //Update the activity view since the selection may have changed
                    UpdateActivities();
                }
                else
                {
                    this.workflowsLabel.Text = " Workflows - no records";
                    ShowViewHost(false);
                    this.listViewWorkflows.Items.Clear();
                    this.listViewActivities.Items.Clear();
                    UpdateTitle();
                }

                
            }
            //Show error dialog if anything bad happen - likely a database error
            catch (Exception ex)
            {
                Monitor(false);
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.statusLabelMonitoring.Text = "Error";

                workflowStatusList.Clear();
                listViewWorkflows.Items.Clear();

                activityStatusListValue.Clear();
                listViewActivities.Items.Clear();

                this.workflowsLabel.Text = " Workflows - no records";
            }

            workflowViewHost.Refresh();
        }

        //Refresh the view when the polling timer expires
        private void Timer_Tick(object sender, EventArgs e)
        {
            RefreshView();
        }
        
        //Change the workflow view and activities if the workflow instance selected has changed
        // It can be changed by the user or if during polling if AutoSelectLatest is true
        private void ListViewWorkflows_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            if (listViewWorkflows.SelectedItems.Count == 0)
                return;

            try
            {
                UpdateTitle();

                ListViewItem currentWorkflow = listViewWorkflows.SelectedItems[0];
                try
                {
                    Guid workflowInstanceId = workflowStatusList[(currentWorkflow.SubItems[0]).Text].InstanceId;
                    GetWorkflowDefinition(workflowInstanceId);
                    ShowViewHost(true);
                    this.toolStripComboBoxZoom.Enabled = true;
                    this.toolStripButtonCollapse.Enabled = true;
                    this.toolStripButtonExpand.Enabled = true;
                }
                catch
                {
                    ShowViewHost(false);
                    this.toolStripComboBoxZoom.Enabled = false;
                    this.toolStripButtonCollapse.Enabled = false;
                    this.toolStripButtonExpand.Enabled = false;
                }

                this.toolStripComboBoxZoom.SelectedIndex = 2;

                UpdateActivities();

                if (menuStartStop.Text == "Stop")
                    this.statusLabelMonitoring.Text = "Monitoring";
                else
                    this.statusLabelMonitoring.Text = String.Empty;

                workflowViewHost.Refresh();
            }
            catch (Exception error)
            {
                MessageBox.Show(error.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.statusLabelMonitoring.Text = "Error";
            }
        }

        //Turn on or off the workflows and activities details
        private void MenuActivityDetailsView_Click(object sender, EventArgs e)
        {
            if (menuActivityDetailsView.Checked)
            {
                this.menuActivityDetailsView.Checked = false;
                this.trackingSurface.Panel1Collapsed = true;
            }
            else
            {
                this.menuActivityDetailsView.Checked = true;
                this.trackingSurface.Panel1Collapsed = false;
                this.trackingSurface.Panel2Collapsed = false;
            }
        }

        //Query the database via the databaseService and update the activities ListView and activityStatusList
        // based on the currently selected workflow instance
        private void UpdateActivities()
        {
            if (listViewWorkflows.SelectedItems.Count == 0)
            {
                listViewActivities.Items.Clear();
                activityStatusListValue.Clear();
                return;
            }

            ListViewItem currentWorkflow = listViewWorkflows.SelectedItems[0];
            if (currentWorkflow != null)
            {
                Guid workflowInstanceId = workflowStatusList[(currentWorkflow.SubItems[0]).Text].InstanceId;

                SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance = null;
                if (true == monitorDatabaseServiceValue.TryGetWorkflow(workflowInstanceId, out sqlTrackingWorkflowInstance))
                {
                    listViewActivities.Items.Clear();
                    activityStatusListValue.Clear();

                    //ActivityEvents list received contain all events for activities in orders in event order
                    //Walking down-up on the ActivityEvents list and keeping the last entry

                    for (int index = sqlTrackingWorkflowInstance.ActivityEvents.Count; index >= 1; index--)
                    {
                        ActivityTrackingRecord activityTrackingRecord = sqlTrackingWorkflowInstance.ActivityEvents[index - 1];
                        if (!activityStatusListValue.ContainsKey(activityTrackingRecord.QualifiedName))
                        {
                            ActivityStatusInfo latestActivityStatus = new ActivityStatusInfo(activityTrackingRecord.QualifiedName, activityTrackingRecord.ExecutionStatus.ToString());
                            activityStatusListValue.Add(activityTrackingRecord.QualifiedName, latestActivityStatus);

                            string[] activitiesListViewItems = new string[] {
                        activityTrackingRecord.EventOrder.ToString(),
                        activityTrackingRecord.QualifiedName, 
                        activityTrackingRecord.ExecutionStatus.ToString()};
                            ListViewItem li = new ListViewItem(activitiesListViewItems, -1);
                            listViewActivities.Items.Add(li);
                        }
                    }
                    workflowViewHost.Refresh();
                }
            }
        }

        //Get the workflow definition from the database and load viewhost
        private void GetWorkflowDefinition(Guid workflowInstanceId)
        {
            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance = null;

            if (true == monitorDatabaseServiceValue.TryGetWorkflow(workflowInstanceId, out sqlTrackingWorkflowInstance))
            {
                if ((null != sqlTrackingWorkflowInstance) && (null != sqlTrackingWorkflowInstance.WorkflowDefinition))
                {
                    Activity workflowDefinition = sqlTrackingWorkflowInstance.WorkflowDefinition;
                    if (null != workflowDefinition)
                        workflowViewHost.OpenWorkflow(workflowDefinition);
                }
            }
            else
            {
                throw (new Exception());
            }
        }

        private void Mainform_Closing(object sender, FormClosingEventArgs e)
        {
            string path = Application.LocalUserAppDataPath + @"\workflowmonitor.config";
            monitorSettingsValue.SaveSettings(path);
            base.OnClosed(e);
        }

        private void ToolStripComboBoxWorkflowEvent_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Set the selectedWorkflowEvent to the newly selected Workflow Event
            selectedWorkflowEvent = this.toolStripComboBoxWorkflowEvent.Text.Trim();
        }
        private void ToolStripButtonFindWorkflows_Click(object sender, EventArgs e)
        {
            statusFromDateTime = DateTime.Parse(this.toolStripTextBoxFrom.Text.ToString());
            statusUntilDateTime = DateTime.Parse(this.toolStripTextBoxUntil.Text.ToString());

            trackingDataItemValue = new TrackingDataItemValue(string.Empty, string.Empty, string.Empty);
            trackingDataItemValue.QualifiedName = this.toolStripTextBoxArtifactQualifiedId.Text.ToString();
            trackingDataItemValue.FieldName = this.toolStripTextBoxArtifactKeyName.Text.ToString();
            trackingDataItemValue.DataValue = this.toolStripTextBoxArtifactKeyValue.Text.ToString();

            if (!((string.Empty == trackingDataItemValue.QualifiedName) && (string.Empty == trackingDataItemValue.FieldName) && (string.Empty == trackingDataItemValue.DataValue)))
            {
                if ((string.Empty == trackingDataItemValue.QualifiedName) || (string.Empty == trackingDataItemValue.FieldName) || ((string.Empty == trackingDataItemValue.DataValue)))
                {
                    MessageBox.Show("If you wish to filter by artifacts you need to set the three inputs: Activity Qualified Name, Property Name, and Value.", "Workflow Monitor Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }

            this.listViewWorkflows.Items.Clear();
            DisplayWorkflows(selectedWorkflowEvent, statusFromDateTime, statusUntilDateTime, trackingDataItemValue);
            UpdateActivities();
        }


        private void ToolStripButtonFindWorkflowById_Click(object sender, EventArgs e)
        {
            try
            {
                workflowInstanceIdToFind = new Guid(this.toolStripTextBoxWorkflowInstanceId.Text.ToString());
            }
            catch (System.FormatException exception)
            {
                MessageBox.Show("Please enter a valid Guid for Workflow Instance ID. " + exception.Message, "Workflow Monitor Error");
                return;
            }

            if (Guid.Empty == workflowInstanceIdToFind)
            {
                MessageBox.Show("Please enter a valid Guid for Workflow Instance ID.", "Workflow Monitor Error");
                return;
            }
            listViewWorkflows.Items.Clear();
            DisplayWorkflowById(workflowInstanceIdToFind);
            UpdateActivities();
        }

        private void ToolStripButtonResetOptions_Click(object sender, EventArgs e)
        {
            ResetWorkflowFilterOptions();
        }

        //Reset the workflow filter options
        private void ResetWorkflowFilterOptions()
        {
            //Set selected item in workflow event combo box to All
            this.toolStripComboBoxWorkflowEvent.SelectedItem = this.toolStripComboBoxWorkflowEvent.Items[0];
            selectedWorkflowEvent = this.toolStripComboBoxWorkflowEvent.Text.Trim();

            //Set StatusFrom datetime to 1/1/2000 and StatusUntil to tomorrow
            this.toolStripTextBoxFrom.Text = new DateTime(2000, 1, 1).ToString();
            this.toolStripTextBoxUntil.Text = DateTime.Now.AddDays(1).ToString();
            statusFromDateTime = new DateTime(2000, 1, 1);
            statusUntilDateTime = DateTime.Now.AddDays(1);

            //Set filter by tracking data items options to empty strings
            this.toolStripTextBoxArtifactQualifiedId.Text = string.Empty;
            this.toolStripTextBoxArtifactKeyName.Text = string.Empty;
            this.toolStripTextBoxArtifactKeyValue.Text = string.Empty;
            trackingDataItemValue = new TrackingDataItemValue(string.Empty, string.Empty, string.Empty);

            //Set workflow instance ID to empty Guid
            this.toolStripTextBoxWorkflowInstanceId.Text = Guid.Empty.ToString();

            //clear workflow items
            listViewWorkflows.Items.Clear();
            //display workflows based on workflow event set earlier to All
            DisplayWorkflows(selectedWorkflowEvent);
            //update activities list
            UpdateActivities();
        }

        void WorkflowViewHost_ZoomChanged(object sender, ViewHost.ZoomChangedEventArgs e)
        {
            // Fires when control is resized via combo box or Fit to Size button.
            // If the dropdown is already correct, return.
            if (e.Zoom.ToString().Equals(this.toolStripComboBoxZoom.Text.Trim(new Char[] {'%'})))
                return;

            //Return dropdown to 100 percent
            this.toolStripComboBoxZoom.SelectedIndex = 2;
        }

        private void ListViewActivities_Click(object sender, EventArgs e)
        {
            workflowViewHost.HighlightActivity(
                listViewActivities.SelectedItems[0].SubItems[1].Text);
        }
        private void ShowViewHost(Boolean show)
        {
            this.workflowViewHost.Visible = show;
            if (displayedWorkflows == null)
                this.workflowViewErrorText.Visible = false;
            else
                this.workflowViewErrorText.Visible = displayedWorkflows.Count.Equals(0);
        }

        private void listViewWorkflows_SelectedIndexChanged(object sender, EventArgs e)
        {

        }  
    }
}