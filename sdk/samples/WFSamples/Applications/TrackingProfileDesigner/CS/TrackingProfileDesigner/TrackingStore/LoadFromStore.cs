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
using System.Workflow.Runtime.Tracking;
using System.Reflection;
using System.IO;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner.TrackingStore
{
    public partial class LoadFromStore : Form
    {
        const string NoProfilesAvailable = "No profiles available.";
        const string NoWorkflowsAvailable = "No workflows available.";
        private DataTable workflowProfilesValue;
        private Type selectedWorkflowValue;
        private Version selectedProfileVersionValue;

        public LoadFromStore()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Gets or sets the table used to display the workflow and tracking profile information
        /// </summary>
        public DataTable WorkflowProfiles
        {
            get { return workflowProfilesValue; }
            set { workflowProfilesValue = value; }
        }

        /// <summary>
        /// The selected workflow
        /// </summary>
        public Type SelectedWorkflow
        {
            get { return selectedWorkflowValue; }
            set { selectedWorkflowValue = value; }
        }

        /// <summary>
        /// The selected tracking profile
        /// </summary>
        public Version SelectedProfileVersion
        {
            get { return selectedProfileVersionValue; }
            set { selectedProfileVersionValue = value; }        
        }

        /// <summary>
        /// Contains information the workflow type info stored in the tracking database
        /// </summary>
        class WorkflowInfo
        {
            public string TypeFullName;
            public string AssemblyFullName;
            public override bool Equals(object obj)
            {
                WorkflowInfo other = obj as WorkflowInfo;
                if (other != null)
                {
                    return (other.AssemblyFullName.Equals(this.AssemblyFullName) && other.TypeFullName.Equals(this.TypeFullName));
                }
                else
                {
                    return false;
                }
            }

            public override string ToString()
            {
                //We could possibly be listing the same type twice
                //if an identical name is located in multiple assemblies.
                //However, for simplicity and brevity, we only use the type name
                return TypeFullName; 
            }
            
            public override int GetHashCode()
            {
                //Always a good idea to override GetHashCode when implementing Equals()
                return TypeFullName.GetHashCode() ^ AssemblyFullName.GetHashCode();
            }

        }

        private void LoadFromStore_Load(object sender, EventArgs e)
        {
            if (WorkflowProfiles.Rows.Count > 0)
            {
                foreach (DataRow row in WorkflowProfiles.Rows)
                {
                    //Add a row for each workflow type found in the store
                    WorkflowInfo wi = new WorkflowInfo();
                    wi.AssemblyFullName = (string)row["AssemblyFullName"];
                    wi.TypeFullName = (string)row["TypeFullName"];
                    if (!workflowList.Items.Contains(wi))
                    {
                        workflowList.Items.Add(wi);
                    }
                }
            }
            else
            {
                workflowList.Items.Add(NoWorkflowsAvailable);
            }
        }

        private void workflowList_SelectedIndexChanged(object sender, EventArgs e)
        {
            WorkflowInfo wi = workflowList.SelectedItem as WorkflowInfo;
            if (wi != null)
            {
                //Update the profile list with the available profile versions for the selected workflow type
                profileVersionList.Items.Clear();

                DataRow[] versionRows = WorkflowProfiles.Select(string.Format("TypeFullName = '{0}' AND AssemblyFullName = '{1}'", wi.TypeFullName, wi.AssemblyFullName));
                if (versionRows == null || versionRows.Length == 0)
                {
                    //If no profiles exist
                    profileVersionList.Items.Add(NoProfilesAvailable);
                }
                else
                {
                    foreach (DataRow row in versionRows)
                    {
                        profileVersionList.Items.Add(new Version((string)row["Version"]));
                    }
                }
            }
        }

        private void save_Click(object sender, EventArgs e)
        {
            if (workflowList.SelectedItem != null && profileVersionList.SelectedItem != null)
            {
                WorkflowInfo wi = workflowList.SelectedItem as WorkflowInfo;
                if (wi != null)
                {
                    Assembly typeAsm;
                    try
                    {
                        typeAsm = Assembly.Load(wi.AssemblyFullName);
                    }
                    catch (FileNotFoundException)
                    {
                        //Assembly not found; return
                        return;
                    }
                    SelectedWorkflow = typeAsm.GetType(wi.TypeFullName);
                    SelectedProfileVersion = profileVersionList.SelectedItem as Version;
                    this.DialogResult = DialogResult.OK;
                    this.Close();
                    return;
                }
            }
            MessageBox.Show("Please select a workflow and version to continue.","Error.");            
        }

        private void cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }


    }
}