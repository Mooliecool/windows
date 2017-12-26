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
using System.Drawing;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Workflow.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.Runtime;
using System.CodeDom.Compiler;
using System.Xml;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class WizardForm : Form
    {
        FolderType folderTypeSelection;
        IfConditionType ifConditionTypeSelection;
        ActionTypes actionTypeSelection;
        WorkflowViewWrapper workflowDesigner;
        Activity eventActivity;
        WorkflowCompilerResults results;
        string xamlFile;

        public WizardForm()
        {
            InitializeComponent();
            folderSelectionLabel.Text = conditionSelectionLabel.Text = ifParameterLinkLabel.Text = actionsSelectionLabel.Text = string.Empty;
            workflowDesigner = new WorkflowViewWrapper();
            this.splitContainer1.Panel2.Controls.Add(workflowDesigner);
            workflowDesigner.Show();
        }

        private void FolderSelectionChanged(object sender, EventArgs e)
        {
            if (inboxRadioButton.Checked)
            {
                this.folderSelectionLabel.Text = "Look in Inbox Folder";
                folderTypeSelection = FolderType.Inbox;
            }
            else if (sentRadioButton.Checked)
            {
                this.folderSelectionLabel.Text = "Look in Sent Items Folder";
                folderTypeSelection = FolderType.Sent;
            }
            else if (outboxRadioButton.Checked)
            {
                this.folderSelectionLabel.Text = "Look in Outbox Folder";
                folderTypeSelection = FolderType.Outbox;
            }
            else
            {
                this.folderSelectionLabel.Text = string.Empty;
                folderTypeSelection = FolderType.None;
            }
            BuildWorkflow(WizardStep.None);
            ResetButtons();
        }

        private void ConditionSelectionChanged(object sender, EventArgs e)
        {
            if (subjectRadioButton.Checked)
            {
                this.conditionSelectionLabel.Text = "If Subject Equals";
                ifConditionTypeSelection = IfConditionType.Subject;
            }
            else if (fromRadioButton.Checked)
            {
                this.conditionSelectionLabel.Text = "If From Equals";
                ifConditionTypeSelection = IfConditionType.From;
            }
            else if (toRadioButton.Checked)
            {
                this.conditionSelectionLabel.Text = "If To Equals";
                ifConditionTypeSelection = IfConditionType.To;
            }
            else if (ccRadioButton.Checked)
            {
                this.conditionSelectionLabel.Text = "If CC Equals";
                ifConditionTypeSelection = IfConditionType.CC;
            }
            else if (bccRadioButton.Checked)
            {
                this.conditionSelectionLabel.Text = "If BCC Equals";
                ifConditionTypeSelection = IfConditionType.Bcc;
            }
            else
            {
                this.conditionSelectionLabel.Text = string.Empty;
                ifConditionTypeSelection = IfConditionType.None;
            }
            
            Size size = this.conditionSelectionLabel.Size;
            this.ifParameterLinkLabel.Location = new Point(conditionSelectionLabel.Location.X + size.Width, conditionSelectionLabel.Location.Y);
            if (string.IsNullOrEmpty(this.ifParameterLinkLabel.Text))
                this.ifParameterLinkLabel.Text = "Enter Text Here";
            BuildWorkflow(WizardStep.Folder);
            ResetButtons();
        }

        private void ActionSelectionChanged(object sender, EventArgs e)
        {
            CheckedListBox.CheckedIndexCollection indexCollection = actionsCheckedListBox.CheckedIndices;
            this.actionsSelectionLabel.Text = string.Empty;
            this.actionTypeSelection = ActionTypes.None;
            foreach (int index in indexCollection)
            {
                switch (index)
                {
                    case (0):
                        this.actionsSelectionLabel.Text += "Send Auto-Reply Email to Sender\n";
                        this.actionTypeSelection = this.actionTypeSelection | ActionTypes.SendAutoReply;
                        break;
                    case (1):
                        this.actionsSelectionLabel.Text += "Create Outlook Note\n";
                        this.actionTypeSelection = this.actionTypeSelection | ActionTypes.CreateNote;
                        break;
                    case (2):
                        this.actionsSelectionLabel.Text += "Create Outlook Task\n";
                        this.actionTypeSelection = this.actionTypeSelection | ActionTypes.CreateTask;
                        break;
                }
            }
            this.BuildWorkflow(WizardStep.Condition);
            ResetButtons();
        }

        private void BuildWorkflow(WizardStep wizardStep)
        {
            if (ValidateSelections(wizardStep))
            {
                if (workflowDesigner.SequentialWorkflow.Activities.Count > 0)
                    workflowDesigner.SequentialWorkflow.Activities.Clear();

                ProcessEventSelection(this.folderTypeSelection);
                ProcessFilterSelection(this.ifConditionTypeSelection);
                ProcessActivitySelection(this.actionTypeSelection);

                generateWorkflowButton.Enabled = false;
                compileWorkflowButton.Enabled = true;
            }
        }

        private void ProcessActivitySelection(ActionTypes actionType)
        {
            if (actionType == ActionTypes.None)
                return;

            ParallelActivity parallelActivity = null;
            // Check for multi activity selection
            if (actionType != ActionTypes.SendAutoReply & actionType != ActionTypes.CreateNote & actionType != ActionTypes.CreateTask)
            {
                parallelActivity = new ParallelActivity();
            }
            // Process each selected activity
            if ((actionType & ActionTypes.SendAutoReply) == ActionTypes.SendAutoReply)
                ProcessAutoReplyEmailActivity(parallelActivity);                        
            if ((actionType & ActionTypes.CreateNote) == ActionTypes.CreateNote)
                ProcessOutlookNoteActivity(parallelActivity);
            if ((actionType & ActionTypes.CreateTask) == ActionTypes.CreateTask)
                ProcessOutlookTaskActivity(parallelActivity);                
            // Add ParallelActivity to the workflow
            if (parallelActivity != null)
            {
                workflowDesigner.SequentialWorkflow.Activities.Add(parallelActivity);
                workflowDesigner.Host.RootComponent.Site.Container.Add(parallelActivity);
            }
        }

        private void ProcessOutlookTaskActivity(ParallelActivity activityList)
        {
            OutlookTask outlookActivity = new OutlookTask();
            outlookActivity.Name = "CreateOutlookTask";
            SequenceActivity branch = new SequenceActivity();
            branch.Name = "branch3";
            branch.Activities.Add(outlookActivity);

            if (activityList != null)
                activityList.Activities.Add(branch);
            else
                workflowDesigner.SequentialWorkflow.Activities.Add(branch);

            workflowDesigner.Host.RootComponent.Site.Container.Add(outlookActivity);
            workflowDesigner.Host.RootComponent.Site.Container.Add(branch);
        }

        private void ProcessOutlookNoteActivity(ParallelActivity activityList)
        {
            OutlookNote outlookNoteActivity = new OutlookNote();
            outlookNoteActivity.Name = "CreateOutlookNote";
            SequenceActivity branch = new SequenceActivity();
            branch.Name = "branch2";
            branch.Activities.Add(outlookNoteActivity);

            if (activityList != null)
                activityList.Activities.Add(branch);
            else
                workflowDesigner.SequentialWorkflow.Activities.Add(branch);

            workflowDesigner.Host.RootComponent.Site.Container.Add(outlookNoteActivity);
            workflowDesigner.Host.RootComponent.Site.Container.Add(branch);
        }

        private void ProcessAutoReplyEmailActivity(ParallelActivity activityList)
        {
            AutoReplyEmail emailActivity = new AutoReplyEmail();
            emailActivity.Name = "AutoReplyEmail";
            SequenceActivity branch = new SequenceActivity();
            branch.Name = "branch1";
            branch.Activities.Add(emailActivity);

            if (activityList != null)
                activityList.Activities.Add(branch);
            else
                workflowDesigner.SequentialWorkflow.Activities.Add(branch);
            
            workflowDesigner.Host.RootComponent.Site.Container.Add(emailActivity);
            workflowDesigner.Host.RootComponent.Site.Container.Add(branch);
        }

        private void ProcessFilterSelection(IfConditionType ifConditionType)
        {
            BaseMailbox emailActivity = eventActivity as BaseMailbox;
            switch (ifConditionType)
            {
                case IfConditionType.Subject:
                    emailActivity.Filter = FilterOption.Subject;
                    emailActivity.FilterValue = this.ifParameterField.Text;
                    break;
                case IfConditionType.From:
                    emailActivity.Filter = FilterOption.From;
                    emailActivity.FilterValue = this.ifParameterField.Text;
                    break;
                case IfConditionType.To:
                    emailActivity.Filter = FilterOption.To;
                    emailActivity.FilterValue = this.ifParameterField.Text;
                    break;
                case IfConditionType.CC:
                    emailActivity.Filter = FilterOption.CC;
                    emailActivity.FilterValue = this.ifParameterField.Text;
                    break;
                case IfConditionType.Bcc:
                    emailActivity.Filter = FilterOption.Bcc;
                    emailActivity.FilterValue = this.ifParameterField.Text;
                    break;
            }
        }

        private void ProcessEventSelection(FolderType folderType)
        {
            switch (folderType)
            {
                case FolderType.Inbox:
                    eventActivity = new EvaluateInboxItems();
                    workflowDesigner.SequentialWorkflow.Activities.Add(eventActivity);
                    workflowDesigner.SequentialWorkflow.Activities.Add(new DummyActivity());
                    workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity);
                    break;
                case FolderType.Sent:
                    eventActivity = new EvaluateSentItems();
                    workflowDesigner.SequentialWorkflow.Activities.Add(eventActivity);
                    workflowDesigner.SequentialWorkflow.Activities.Add(new DummyActivity());
                    workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity);
                    break;
                case FolderType.Outbox:
                    eventActivity = new EvaluateOutboxItems();
                    workflowDesigner.SequentialWorkflow.Activities.Add(eventActivity);
                    workflowDesigner.SequentialWorkflow.Activities.Add(new DummyActivity());
                    workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity);
                    break;
            }
        }

        private void ifParameterLinkLabel_Click(object sender, LinkLabelLinkClickedEventArgs e)
        {
            this.ifParameterLinkLabel.Visible = false;
            Point ifParameterLocation = this.ifParameterLinkLabel.Location;
            this.ifParameterField.Visible = true;
            this.ifParameterField.Text = ifParameterLinkLabel.Text;
            this.ifParameterField.Location = ifParameterLocation;
            this.ifParameterField.Focus();
        }

        void ifParameterField_Leave(object sender, EventArgs e)
        {
            this.ifParameterField.Visible = false;
            this.ifParameterLinkLabel.Text = ifParameterField.Text;
            this.ifParameterLinkLabel.Visible = true;
            this.ifParameterLinkLabel.Focus();
        }

        private void ifParameterField_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                this.ifParameterField_Leave(null, null);
        }

        private void ResetButtons()
        {
            generateWorkflowButton.Enabled = true;
            compileWorkflowButton.Enabled = startWorkflowButton.Enabled = false;
        }

        private void nextStepButton_Click(object sender, EventArgs e)
        {
            if (this.ValidateSelections((WizardStep)tabControl1.SelectedIndex + 1))
            {
                if (tabControl1.SelectedIndex != tabControl1.TabCount - 1)
                    tabControl1.SelectedIndex++;
            }
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            nextStepButton.Enabled = (tabControl1.SelectedIndex != 3);
        }

        private bool ValidateSelections(WizardStep wizardStep)
        {
            if (this.folderTypeSelection == FolderType.None & (int)wizardStep > 0)
            {
                MessageBox.Show(this, "You need to select the Outlook folder.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                tabControl1.SelectTab(0);
                inboxRadioButton.Checked = false;
                return false;
            }
            if (this.ifConditionTypeSelection == IfConditionType.None & (int)wizardStep > 1)
            {
                MessageBox.Show(this, "You need to select the IF condition.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                tabControl1.SelectTab(1);
                return false;
            }
            if (this.actionTypeSelection == ActionTypes.None & (int)wizardStep > 2)
            {
                MessageBox.Show(this, "You need to select at least one action.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                tabControl1.SelectTab(2);
                return false;
            }
            return true;
        }

        private void GenerateWorkflowButton(object sender, EventArgs e)
        {
            if (!ValidateSelections(WizardStep.Action))
                return;
            generateWorkflowButton.Enabled = false;
            Cursor cursor = this.Cursor;
            this.Cursor = Cursors.WaitCursor;
            try
            {
                // Save the workflow first, and capture the filePath of the workflow
                this.workflowDesigner.XamlFile = "CustomOutlookWorkflow.xoml";
                this.workflowDesigner.PerformSave();
                this.xamlFile = this.workflowDesigner.XamlFile;

                XmlDocument doc = new XmlDocument();
                doc.Load(this.workflowDesigner.XamlFile);
                XmlAttribute attrib = doc.CreateAttribute("x", "Class", "http://schemas.microsoft.com/winfx/2006/xaml");
                attrib.Value = this.GetType().Namespace + ".CustomOutlookWorkflow";
                doc.DocumentElement.Attributes.Append(attrib);
                doc.Save(this.workflowDesigner.XamlFile);

                compileWorkflowButton.Enabled = true;
                MessageBox.Show(this, "Workflow generated successfully. Generated Workflow XAML file:\n" + Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location), xamlFile), this.Text, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch
            {
                generateWorkflowButton.Enabled = true;
            }
            finally
            {
                this.Cursor = cursor;
            }
        }

        private void CompileWorkflowButton(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(this.xamlFile))
                return;
            if (!File.Exists(this.xamlFile))
            {
                MessageBox.Show(this, "Cannot locate XAML file: " + Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location), xamlFile), this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            compileWorkflowButton.Enabled = false;
            Cursor cursor = this.Cursor;
            this.Cursor = Cursors.WaitCursor;
            try
            {
                // Compile the workflow
                String[] assemblyNames = { "ReadEmailActivity.dll" };
                WorkflowCompiler compiler = new WorkflowCompiler();
                WorkflowCompilerParameters parameters = new WorkflowCompilerParameters(assemblyNames);
                parameters.LibraryPaths.Add(Path.GetDirectoryName(typeof(BaseMailbox).Assembly.Location));
                parameters.OutputAssembly = "CustomOutlookWorkflow" +  Guid.NewGuid().ToString()  + ".dll";
                results = compiler.Compile(parameters, this.xamlFile);

                StringBuilder errors = new StringBuilder();
                foreach (CompilerError compilerError in results.Errors)
                {
                    errors.Append(compilerError.ToString() + '\n');
                }

                if (errors.Length != 0)
                {
                    MessageBox.Show(this, errors.ToString(), this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    compileWorkflowButton.Enabled = true;
                }
                else
                {
                    MessageBox.Show(this, "Workflow compiled successfully. Compiled assembly:\n" + results.CompiledAssembly.GetName(), this.Text, MessageBoxButtons.OK, MessageBoxIcon.Information);
                    startWorkflowButton.Enabled = true;
                }
            }
            finally
            {
                this.Cursor = cursor;
            }
        }

        private void StartWorkflowButton_Click(object sender, EventArgs e)
        {
            startWorkflowButton.Enabled = false;
            // Start the runtime engine
            WorkflowRuntime workflowRuntime = new WorkflowRuntime();
            workflowRuntime.StartRuntime();
            // Add workflow event handlers
            workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
            // Start the workflow
            workflowRuntime.CreateWorkflow(results.CompiledAssembly.GetType(this.GetType().Namespace + ".CustomOutlookWorkflow")).Start();
        }

        private void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            MessageBox.Show("The Workflow Completed");
        }

        enum FolderType
        {
            None = 0,
            Inbox = 1,
            Sent = 2,
            Outbox = 3,
        }

        enum IfConditionType
        {
            None = 0,
            Subject = 1,
            From = 2,
            To = 3,
            CC = 4,
            Bcc = 5,
        }

        [Flags]
        enum ActionTypes
        {
            None = 0,
            SendAutoReply = 1,
            CreateNote = 2,
            CreateTask = 4,
        }

        enum WizardStep
        { 
            None = 0,
            Folder = 1,
            Condition = 2,
            Action = 3,
        }
    }
}