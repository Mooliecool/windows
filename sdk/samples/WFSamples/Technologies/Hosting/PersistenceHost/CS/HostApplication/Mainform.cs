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
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Workflow.Activities;
using System.Workflow.Runtime;
using System.Xml;

namespace Microsoft.Samples.Workflow.PersistenceHost
{
    public partial class Mainform : Form
    {
        private WorkflowRuntime runtime;
        private DocumentApprovalService documentService;
        const string instanceFilename = "workflowInstances.xml";

        public Mainform()
        {
            InitializeComponent();
            
            this.runtime = new WorkflowRuntime();

            runtime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(runtime_WorkflowCompleted);

            // Set up runtime to unload workflow instance from memory to file using FilePersistenceService
            FilePersistenceService filePersistence = new FilePersistenceService(true);
            runtime.AddService(filePersistence);

            // Add document approval service
            ExternalDataExchangeService dataService = new ExternalDataExchangeService();
            runtime.AddService(dataService);
            documentService = new DocumentApprovalService(this);
            dataService.AddService(documentService);

            // Search for workflows that have previously been persisted to file, and load into the listview control.
            // These workflows will be reloaded by the runtime when events are raised against them.
            LoadWorkflowData();

            // Start the runtime
            runtime.StartRuntime();
        }

        internal void DocumentRequested(Guid instanceId, String approver)
        {
            AddListViewItem addListItem = new AddListViewItem(AddListViewItemAsync);
            Invoke(addListItem, instanceId, approver);
        }

        void runtime_WorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            // Remove completed workflow from list view
            RemoveListViewItem remove = new RemoveListViewItem(RemoveListViewItemAsync);
            Invoke(remove, e.WorkflowInstance.InstanceId);

            // Remove completed workflow persistence file
            FileInfo file = new FileInfo(e.WorkflowInstance.InstanceId.ToString());
            file.Delete();
        }

        #region Workflow Data Persistence

        void LoadWorkflowData()
        {
            XmlTextReader reader = new XmlTextReader(instanceFilename);
            try
            {
                while (reader.Read())
                {
                    if (reader.Name.Equals("WorkflowInstance"))
                    {
                        ListViewExistingRequests.Items.Add(
                            new ListViewItem(
                                new String[] { reader.GetAttribute("Approver"),
                                            reader.GetAttribute("InstanceId") }));
                    }
                }
                reader.Close();
            }
            catch (FileNotFoundException) { }
           
        }
        void SaveWorkflowData()
        {
            XmlTextWriter writer = new XmlTextWriter(instanceFilename, Encoding.Unicode);
            writer.WriteStartElement("WorkflowInstances");
            foreach (ListViewItem item in ListViewExistingRequests.Items)
            {
                writer.WriteStartElement("WorkflowInstance");
                writer.WriteAttributeString("Approver", item.SubItems[0].Text);
                writer.WriteAttributeString("InstanceId", item.SubItems[1].Text);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
            writer.Flush();
            writer.Close();
        }
        #endregion

        #region Asynchronous control accessors

        // Accessing the list view control from other threads (such as are created when the workflow raises an event)
        // requires use of Invoke so that the calls are threadsafe 
        private delegate void RemoveListViewItem(Guid instanceId);
        private void RemoveListViewItemAsync(Guid instanceId)
        {
            foreach (ListViewItem item in ListViewExistingRequests.Items)
            {
                if (item.SubItems[1].Text.Equals(instanceId.ToString()))
                    ListViewExistingRequests.Items.Remove(item);
            }
        }

        private delegate void AddListViewItem(Guid instanceId, String approver);
        private void AddListViewItemAsync(Guid instanceId, String approver)
        {
            ListViewExistingRequests.Items.Add(new ListViewItem(new String[] { approver, instanceId.ToString() }));
        }

        #endregion

        #region Form Events
        private void Mainform_FormClosing(object sender, FormClosingEventArgs e)
        {
            SaveWorkflowData();
            runtime.StopRuntime();
            runtime.Dispose();
        }

        private void ButtonCreateNewRequest_Click(object sender, EventArgs e)
        {
            Dictionary<String, Object> parameters = new Dictionary<String, Object>();
            parameters.Add("Approver", TextBoxApprover.Text);
            WorkflowInstance instance = runtime.CreateWorkflow(typeof(DocumentApprovalWorkflow), parameters);
            
            instance.Start();

            TextBoxApprover.Text = String.Empty;
        }

        private void ApproveDocumentToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (ListViewExistingRequests.SelectedItems.Count == 1)
            {
                documentService.ApproveDocument(
                    new Guid(ListViewExistingRequests.SelectedItems[0].SubItems[1].Text),
                    ListViewExistingRequests.SelectedItems[0].SubItems[0].Text
                    );
            }
        }
        #endregion
    }
}