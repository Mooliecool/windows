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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace Microsoft.Samples.Workflow.WorkflowChooser
{
    public partial class WorkflowSelectionForm : Form
    {
        private Microsoft.Samples.Workflow.DesignerControl.WorkflowDesignerControl workflowView = new Microsoft.Samples.Workflow.DesignerControl.WorkflowDesignerControl();
        public WorkflowSelectionForm()
        {
            InitializeComponent();
            workflowView.AutoSize = true;
            workflowView.Dock = DockStyle.Fill;            
        }

        private List<Type> workflowTypesValue;

        /// <summary>
        /// The list of workflow types that need to be displayed
        /// </summary>
        public List<Type> WorkflowTypes
        {
            get { return workflowTypesValue; }
        }
        public void SetWorkflowTypes(List<Type> workflowTypes)
        {
            workflowTypesValue = workflowTypes;
        }

        private Type selectedWorkflowValue =null;
       
        /// <summary>
        /// Holds selected workflow after the dialog is closed
        /// </summary>
        public Type SelectedWorkflow
        {
            get { return selectedWorkflowValue; }
        }

        Dictionary<ListViewItem, Type> listViewWorkflowMap = new Dictionary<ListViewItem, Type>();

        private void WorkflowSelectionForm_Load(object sender, EventArgs e)
        {
            if (WorkflowTypes.Count == 1)
            {
                this.workflowListView.Visible = false;
                this.descriptionHeader.Location = this.workflowListView.Location;
                this.workflowDescription.Location = new Point(this.workflowDescription.Location.X, this.workflowDescription.Location.Y + this.workflowDescription.Height + 10);
                selectedWorkflowValue = workflowTypesValue[0];
                this.workflowDescription.Text = selectedWorkflowValue.FullName;
            }
            else
            {
                foreach (Type t in WorkflowTypes)
                {
                    ListViewItem item = new ListViewItem(t.Name);
                    listViewWorkflowMap.Add(item, t);
                    workflowListView.Items.Add(item);
                }
                workflowListView.Items[0].Selected = true;
            }
            
            this.workflowViewPanel.Controls.Add(workflowView);
            this.workflowView.WorkflowType = SelectedWorkflow;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.selectedWorkflowValue = null;
            this.Close();
        }

        private void workflowListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (workflowListView.SelectedItems.Count == 0) return;

            selectedWorkflowValue = listViewWorkflowMap[workflowListView.SelectedItems[0]];
            this.workflowDescription.Text = selectedWorkflowValue.FullName;
            this.workflowDescription.Visible = true;
            workflowView.WorkflowType = SelectedWorkflow;
        }
    }
}