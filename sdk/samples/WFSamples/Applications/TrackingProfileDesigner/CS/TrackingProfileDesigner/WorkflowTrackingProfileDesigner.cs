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
using System.ComponentModel.Design;
using System.IO;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using Microsoft.Samples.Workflow.TrackingProfileDesigner.TrackingStore;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner
{
    internal partial class WorkflowTrackingProfileDesignerForm : Form
    {
        TrackingProfileManager profileManager = new TrackingProfileManager();

        public WorkflowTrackingProfileDesignerForm()
        {
            InitializeComponent();           

            this.conditionEqualityButton.Tag = System.Workflow.Runtime.Tracking.ComparisonOperator.Equals;
            this.toolStrip1.Enabled = false;
            this.conditionToolStrip.Visible = false;
            this.annotationToolStrip.Visible = false;
            eventsDropDown.DropDown = new EnumDropDown(typeof(ActivityExecutionStatus));
            workflowEventsDropDown.DropDown = new EnumDropDown(typeof(TrackingWorkflowEvent));

            workflowEventsDropDown.DropDown.ItemClicked += this.OnWorkflowEventClicked;
            eventsDropDown.DropDown.ItemClicked += this.OnActivityEventClicked;

            AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(CurrentDomain_AssemblyResolve);
        }

        #region Assembly Resolve
        /// <summary>
        /// This assembly resolver will get called when the user tries to load an assembly 
        /// that references other assemblies, and the other assemblies cannot be found 
        /// in the default search paths.  A file open dialog is shown to allow the user 
        /// to select the referenced assemblies.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            try
            {
                MessageBox.Show(string.Format("The required assembly either cannot be found, or references additional assemblies.  \n Please find and select the assembly '{0}'.", args.Name), string.Format("Referenced Assembly '{0}' not found", args.Name));

                openAssemblyDialog.Title = string.Format("Open assembly {0}", args.Name);
                DialogResult res = openAssemblyDialog.ShowDialog();
                if (res == DialogResult.OK)
                {
                    Assembly asm = Assembly.LoadFrom(openAssemblyDialog.FileName);
                    if (!asm.FullName.Equals(args.Name))
                    {
                        MessageBox.Show(string.Format("The assembly specified does not match the assembly requested, '{0}'", args.Name), "Error Loading Assembly");
                        return CurrentDomain_AssemblyResolve(sender, args);
                    }
                    return asm;
                }
                if (res == DialogResult.Cancel)
                {
                    return null;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("There was an error loading the assembly: " + ex.Message, "Error Loading Assembly");
            }
            return null;

        }
        #endregion
        #region Menu Events
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void saveProfileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = saveFileDialog1.ShowDialog();
            if (result != DialogResult.OK)
            {
                return;
            }
            try
            {
                using (StreamWriter writer = new StreamWriter(saveFileDialog1.FileName, false))
                {
                    profileManager.SerializeProfile(writer);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("There were errors serializing the tracking profile. " + ex.Message);
                return;
            }
        }

        private void loadProfileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (profileManager == null || profileManager.TrackingProfile == null)
            {
                MessageBox.Show("Please load a workflow before attempting to load a profile.");
                return;
            }
            DialogResult result = openProfileDialog.ShowDialog();
            if (result != DialogResult.OK || !openProfileDialog.CheckFileExists)
            {
                return;
            }
            try
            {
                profileManager.ReadProfile(openProfileDialog.FileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error reading tracking profile: " + ex.Message);
                return;
            }
        }

        private void loadWorkflowToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Assembly assemblyToLoad;
            DialogResult result = openWorkflowDialog.ShowDialog();
            if (result != DialogResult.OK || !openWorkflowDialog.CheckPathExists || openWorkflowDialog.FileName == null || openWorkflowDialog.FileName.Length <= 0)
            {
                return;
            }
            try
            {
                assemblyToLoad = Assembly.LoadFrom(openWorkflowDialog.FileName);
            }
            catch
            {
                MessageBox.Show("Error loading assembly " + openWorkflowDialog.FileName);
                return;
            }
            List<Type> workflows = new List<Type>();
            Type selectedWorkflow;

            foreach (Type t in assemblyToLoad.GetTypes())
            {
                if (t.IsSubclassOf(typeof(Activity)))
                {
                    workflows.Add(t);
                }
            }
            if (workflows.Count == 0)
            {
                MessageBox.Show("No workflows found!");
                return;
            }
            else if (workflows.Count == 1)
            {
                selectedWorkflow = workflows[0];
            }
            else //Multiple workflows to choose from - show selection dialog
            {
                Microsoft.Samples.Workflow.WorkflowChooser.WorkflowSelectionForm selectionForm = new WorkflowChooser.WorkflowSelectionForm();
                selectionForm.SetWorkflowTypes(workflows);
                selectionForm.ShowDialog();
                selectedWorkflow = selectionForm.SelectedWorkflow;
            }
            if (selectedWorkflow != null)
            {
                InitializeProfileDesigner(selectedWorkflow, null);
            }
        }

        private void InitializeProfileDesigner(Type workflowType, TrackingProfile profile)
        {
            this.toolStrip1.Visible = true;
            if (workflowType != null)
            {
                profileManager = new TrackingProfileManager();
                if (profile != null)
                {
                    profileManager.TrackingProfile = profile;
                }
                this.workflowDesignerControl1.WorkflowType = workflowType;
            }

            this.workflowDesignerControl1.SelectionChanged += this.OnActivitySelected;
            
            //Once a workflow is loaded, ensure our glyph provider is added
            IDesignerGlyphProviderService glyphManager = this.workflowDesignerControl1.GetService(typeof(IDesignerGlyphProviderService)) as IDesignerGlyphProviderService;
            if (glyphManager == null)
            {
                MessageBox.Show("There was an error loading the workflow type " + workflowType.AssemblyQualifiedName);
                return;
            }
            glyphManager.AddGlyphProvider(new TrackingGlyphProvider(profileManager));
            
            WorkflowType = workflowType;
        }


        #endregion
        #region Properties
        private Activity selectedActivityValue = null;

        /// <summary>
        /// The currently selected activity
        /// </summary>
        public Activity SelectedActivity
        {
            get { return selectedActivityValue; }
            set { selectedActivityValue = value; }
        }

        /// <summary>
        /// Gets the current root workflow activity
        /// </summary>
        public Activity RootActivity
        {
            get
            {
                Activity root = selectedActivityValue;
                while (root != null && root.Parent != null) { root = root.Parent; }
                return root;
            }
        }

        private Type workflowTypeValue;

        /// <summary>
        /// The current workflow
        /// </summary>
        public Type WorkflowType
        {
            get { return workflowTypeValue; }
            set
            {
                workflowTypeValue = value;
            }
        }

        #endregion
        #region Refresh
        public void RefreshView()
        {
            workflowDesignerControl1.Refresh();
            RefreshToolStrip();
            RefreshExtractDropDown();
            RefreshEventsDropDown();
            RefreshConditionsDropDown();
            RefreshMarkupTab();
        }

        private void RefreshToolStrip()
        {
            if (SelectedActivity == null)
            {
                this.toolStrip1.Enabled = false;
                return;
            }
            
            this.toolStrip1.Enabled = true;
            trackButton.Visible = true;
            workflowEventsDropDown.Visible = false;
            trackButton.Checked = profileManager.IsTracked(SelectedActivity);
            matchDerivedTypes.Checked = profileManager.MatchesDerivedTypes(selectedActivityValue);
            conditionsDropDown.Visible = matchDerivedTypes.Visible = eventsSeparator.Visible = eventsDropDown.Visible = annotateButton.Visible = extractDropDown.Visible = trackButton.Checked;

            conditionMemberDropDown.DropDown = GetExtractableMembers(SelectedActivity, OnConditionMemberClicked, false);
            conditionMemberDropDown.DropDown.Text = "Choose member...";
            
            trackButton.Text = SelectedActivity == null ? "Track" : string.Format("Track {0}", SelectedActivity.GetType().Name);
            
            if (SelectedActivity == RootActivity)
            {
                workflowEventsDropDown.Visible = true;
                RefreshWorkflowEvents();
            }
        }
        #endregion
        #region Conditions
        private void OnConditionMemberClicked(object sender, EventArgs e)
        {
            ToolStripButton button = (sender as ToolStripButton);
            button.Owner.Name = button.Owner.Text = button.Text;
        }

        private void conditionCancelButton_Click(object sender, EventArgs e)
        {
            conditionToolStrip.Visible = false;
            conditionsDropDown.Enabled = true;
            ResetConditionStrip();
        }

        private void conditionMemberDropDown_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            conditionMemberDropDown.Name = conditionMemberDropDown.Text = e.ClickedItem.Text;
            ValidateCondition();
        }

        private void conditionEqualityButton_Click(object sender, EventArgs e)
        {
            ToolStripButton button = (sender as ToolStripButton);
            if (button != null)
            {
                if (button.Text == "==")
                {
                    button.Tag = ComparisonOperator.NotEquals;
                    button.Text = "!=";
                }
                else
                {
                    button.Tag = ComparisonOperator.Equals;
                    button.Text = "==";
                }
            }
        }

        private void ValidateCondition()
        {
            conditionSaveButton.Enabled = true;
            if (conditionMemberDropDown.Text.Contains(" ")) conditionSaveButton.Enabled = false;
        }

        private void conditionSaveButton_Click(object sender, EventArgs e)
        {
            ActivityTrackingCondition condition = conditionToolStrip.Tag as ActivityTrackingCondition;
            profileManager.SaveTrackingCondition(
                selectedActivityValue, ref condition,
                conditionMemberDropDown.Text, ((ComparisonOperator)conditionEqualityButton.Tag), conditionValue.Text);
            conditionToolStrip.Tag = condition;
            conditionToolStrip.Visible = false;
            conditionsDropDown.Enabled = true;
            ResetConditionStrip();
            RefreshConditionsDropDown();
        }


        private void conditionsDropDown_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            conditionToolStrip.Tag = e.ClickedItem.Tag;
            conditionsDropDown.Enabled = false;
            conditionToolStrip.Visible = true;
            ResetConditionStrip();
            if (conditionToolStrip.Tag is ActivityTrackingCondition)
            {
                conditionMemberDropDown.Name = conditionMemberDropDown.Text = ((ActivityTrackingCondition)conditionToolStrip.Tag).Member;
                conditionValue.Text = ((ActivityTrackingCondition)conditionToolStrip.Tag).Value;
            }
            ValidateCondition();
        }

        private void conditionDeleteButton_Click(object sender, EventArgs e)
        {
            profileManager.DeleteTrackingCondition(SelectedActivity, (ActivityTrackingCondition)conditionToolStrip.Tag);
            conditionsDropDown.Enabled = true;
            conditionToolStrip.Visible = false;
            this.RefreshConditionsDropDown();
        }

        private void ResetConditionStrip()
        {
            conditionMemberDropDown.Text = "Select member";
            conditionEqualityButton.Name = conditionEqualityButton.Text = "==";
            conditionValue.Text = string.Empty;
        }

        public void RefreshConditionsDropDown()
        {
            conditionsDropDown.DropDownItems.Clear();
            conditionsDropDown.DropDownItems.Add("Add new condition...");
            TrackingConditionCollection conditions = profileManager.GetTrackingConditions(SelectedActivity);
            if (conditions != null)
            {
                conditionsDropDown.DropDownItems.Add(new ToolStripSeparator());
                foreach (ActivityTrackingCondition condition in conditions)
                {
                    ToolStripButton button = new ToolStripButton();
                    button.Name = button.Text = string.Format("{0} {1} {2}", condition.Member, condition.Operator == ComparisonOperator.Equals ? "==" : "!=", condition.Value);
                    button.Tag = condition;
                    conditionsDropDown.DropDownItems.Add(button);
                }
            }
        }
        #endregion
        #region Events
        private void workflowEventsDropDown_Click(object sender, EventArgs e)
        {
            (workflowEventsDropDown.DropDown as EnumDropDown).UpdateToggleAll();
        }


        private void eventsDropDown_Click(object sender, EventArgs e)
        {
            (eventsDropDown.DropDown as EnumDropDown).UpdateToggleAll();
        }

        private void OnActivityEventClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            ToolStripButton button = e.ClickedItem as ToolStripButton;
            if (button != null)
                profileManager.ToggleEventStatus(SelectedActivity, (ActivityExecutionStatus)Enum.Parse(typeof(ActivityExecutionStatus), button.Name));
            workflowDesignerControl1.Refresh();
        }

        private void OnWorkflowEventClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            ToolStripButton button = e.ClickedItem as ToolStripButton;
            if (button != null)
                profileManager.ToggleEventStatus((TrackingWorkflowEvent)Enum.Parse(typeof(TrackingWorkflowEvent), button.Name));
            workflowDesignerControl1.Refresh();
        }
        
        private void RefreshWorkflowEvents()
        {
            if (profileManager.WorkflowTrackPoint != null)
            {
                WorkflowTrackingLocation wtl = profileManager.WorkflowTrackPoint.MatchingLocation;
                foreach (TrackingWorkflowEvent we in wtl.Events)
                {
                    if (this.eventsDropDown.DropDownItems.ContainsKey(we.ToString()))
                    {
                        ((ToolStripButton)this.eventsDropDown.DropDownItems[we.ToString()]).Checked = true;
                    }
                }
            }
        }


        public void RefreshEventsDropDown()
        {
            ActivityTrackPoint trackPoint = profileManager.GetTrackPointForActivity(SelectedActivity);
            if (trackPoint != null)
            {
                if (trackPoint.MatchingLocations.Count > 0)
                {
                    ActivityTrackingLocation atl = trackPoint.MatchingLocations[0];
                    foreach (ActivityExecutionStatus status in atl.ExecutionStatusEvents)
                    {
                        if (this.eventsDropDown.DropDownItems.ContainsKey(status.ToString()))
                        {
                            ((ToolStripButton)this.eventsDropDown.DropDownItems[status.ToString()]).Checked = true;
                        }
                    }
                }
            }
        }

        private void RefreshMarkupTab()
        {
            if (profileManager != null && profileManager.TrackingProfile != null)
            {
                try
                {
                    if (profileManager.TrackingProfile != null)
                    {
                        profileMarkup.Text = profileManager.SerializeProfile();
                    }
                }
                catch (Exception ex)
                {
                    profileMarkup.Text = string.Format("Could not serialize tracking profile: {0}", ex.Message);
                }
            }
        }

        private void tabControl1_Selected(object sender, TabControlEventArgs e)
        {
            RefreshMarkupTab();
        }

        #endregion
        #region Extracts

        public ToolStripDropDown GetExtractableMembers(Activity activity, EventHandler onClick, bool checkOnClick)
        {
            return GetExtractableMembers(activity, onClick, checkOnClick, true);
        }
        
        /// <summary>
        /// Gets a tool strip drop down containing a clickable set of extract members
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="onClick"></param>
        /// <param name="checkOnClick"></param>
        /// <param name="activityExtract">True if this is an activity extract, false if it is a workflow extract</param>
        /// <returns></returns>
        public ToolStripDropDown GetExtractableMembers(Activity activity, EventHandler onClick, bool checkOnClick, bool activityExtract)
        {
            ToolStripDropDown dropDown = new ToolStripDropDown();

            PropertyInfo[] properties = activity.GetType().GetProperties();
            FieldInfo[] fields = activity.GetType().GetFields();
            List<MemberInfo> members = new List<MemberInfo>(properties);
            members.AddRange(fields);
            
            members.Sort(delegate(MemberInfo lhs, MemberInfo rhs) { return lhs.Name.CompareTo(rhs.Name); });

            foreach (MemberInfo member in members)
            {
                ToolStripButton memberItem = new ToolStripButton();
                memberItem.Name = memberItem.Text = member.Name;
                memberItem.CheckOnClick = checkOnClick;
                memberItem.Tag = activityExtract;
                if (onClick != null)
                    memberItem.Click += onClick;
                dropDown.Items.Add(memberItem);
            }
            return dropDown;
        }

        private void OnExtractItemClicked(object sender, EventArgs e)
        {
            ToolStripButton button = (sender as ToolStripButton);            
            profileManager.ToggleExtract(SelectedActivity, (bool)button.Tag, button.Name);
        }

        private void RefreshExtractDropDown()
        {            
            this.activityMembersToolStripMenuItem.DropDown = GetExtractableMembers(SelectedActivity, OnExtractItemClicked, true,true);
            this.activityMembersToolStripMenuItem.Text = SelectedActivity.GetType().Name;
            this.workflowMembersToolStripMenuItem.DropDown = GetExtractableMembers(RootActivity, OnExtractItemClicked, true, false);
            //Only display both activity and workflow members if this is not the root activity; otherwise only display the activity members
            this.workflowMembersToolStripMenuItem.Visible = (SelectedActivity != RootActivity);

            ActivityTrackPoint trackPoint = profileManager.GetTrackPointForActivity(SelectedActivity);
            if (trackPoint != null)
            {
                foreach (TrackingExtract te in trackPoint.Extracts)
                {
                    if (this.extractDropDown.DropDownItems.ContainsKey(te.Member))
                    {
                        ((ToolStripButton)this.extractDropDown.DropDownItems[te.Member]).Checked = true;
                    }
                }
            }
        }
        #endregion
        #region Annotation
        private void annotateSaveButton_Click(object sender, EventArgs e)
        {
            profileManager.SetAnnotation(SelectedActivity, annotationText.Text);
            annotateButton.Enabled = true;
            annotationToolStrip.Visible = false;
        }

        private void annotateCancelButton_Click(object sender, EventArgs e)
        {
            annotateButton.Enabled = true;
            annotationToolStrip.Visible = false;
        }
        #endregion
        #region Activity Selection
        private void CleanupLastActivity()
        {
            //Cleanup Annotation
            this.annotationText.Text = string.Empty;
            this.annotationToolStrip.Visible = false;
            this.annotateButton.Enabled = true;

            //Cleanup Conditions
            this.conditionToolStrip.Visible = false;
            this.conditionsDropDown.Enabled = true;

            //Reset the drop downs            
            (workflowEventsDropDown.DropDown as EnumDropDown).Reset();
            (eventsDropDown.DropDown as EnumDropDown).Reset();
        }

        private void OnActivitySelected(object sender, EventArgs e)
        {
            ISelectionService selectionService = this.workflowDesignerControl1.GetService(typeof(ISelectionService)) as ISelectionService;
            if (selectionService != null)
            {
                SelectedActivity = selectionService.PrimarySelection as Activity;
                if (SelectedActivity != null)
                {
                    CleanupLastActivity();
                    RefreshView();
                }
            }
        }
        #endregion
        #region Toolstrip
        private void trackButton_Click(object sender, EventArgs e)
        {
            if (SelectedActivity != null)
            {
                CleanupLastActivity();
                profileManager.ToggleActivityTrackPoint(SelectedActivity);
                RefreshView();
            }
        }

        private void annotateButton_Click(object sender, EventArgs e)
        {
            annotationText.Text = profileManager.GetAnnotation(SelectedActivity);
            annotationToolStrip.Visible = true;
            annotateButton.Enabled = false;
        }

        private void matchDerivedTypes_Click(object sender, EventArgs e)
        {
            ActivityTrackPoint trackPoint = profileManager.GetTrackPointForActivity(SelectedActivity);
            if (trackPoint != null && trackPoint.MatchingLocations.Count > 0)
            {
                matchDerivedTypes.Checked = trackPoint.MatchingLocations[0].MatchDerivedTypes = !trackPoint.MatchingLocations[0].MatchDerivedTypes;
                RefreshView();
            }
        }
        #endregion

        #region Sql Events
        /// <summary>
        /// Saves a tracking profile to the Sql store
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveProfileToSql_Click(object sender, EventArgs e)
        {
            TrackingProfileStore profileStore = new TrackingProfileStore();
            try
            {
                if (profileManager == null) throw new ApplicationException("Need to load a profile first");
                profileManager.SerializeProfile();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error serializing profile");
                return;
            }
            string error;
            if (!profileStore.ValidateConnection(out error))
            {
                MessageBox.Show("Error establishing connection to SQL: " + error, "Error connecting to SQL");
                return;
            }
            profileStore.SaveProfile(WorkflowType, profileManager.TrackingProfile);
        }


        private void fromSqlToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TrackingProfileStore profileStore = new TrackingProfileStore();
            string error;
            if (!profileStore.ValidateConnection(out error))
            {
                MessageBox.Show("Error establishing connection to SQL: " + error, "Error connecting to SQL");
                return;
            }
            Type workflowType;
            TrackingProfile profile;
            profileStore.LoadWorkflowAndProfile(out workflowType, out profile);
            if (workflowType != null && profile != null)
            {
                WorkflowType = workflowType;
                profileManager.TrackingProfile = profile;
                InitializeProfileDesigner(workflowType, profile);
            }
        }
        #endregion



    }
}