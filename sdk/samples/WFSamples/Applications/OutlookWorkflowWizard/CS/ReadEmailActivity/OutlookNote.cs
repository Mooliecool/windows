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
using System.Workflow.ComponentModel;
using System.Windows.Forms;
using Outlook = Microsoft.Office.Interop.Outlook;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class OutlookNote: BaseOutlookItem
    {
        public OutlookNote()
        {
            InitializeComponent();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            // Create an Outlook Application object. 
            Outlook.Application outlookApp = new Outlook.Application();
            // Create a new TaskItem.
            Outlook.NoteItem newNote = (Outlook.NoteItem)outlookApp.CreateItem(Outlook.OlItemType.olNoteItem);
            // Configure the task at hand and save it.
            if (this.Parent.Parent is ParallelActivity)
            {
                newNote.Body = (this.Parent.Parent.Parent.Activities[1] as DummyActivity).TitleProperty;
                if ((this.Parent.Parent.Parent.Activities[1] as DummyActivity).TitleProperty != "")
                {
                    MessageBox.Show("Creating Outlook Note");
                    newNote.Save();
                }
            }
            else if (this.Parent.Parent is SequentialWorkflowActivity)
            {
                newNote.Body = (this.Parent.Parent.Activities[1] as DummyActivity).TitleProperty;
                if ((this.Parent.Parent.Activities[1] as DummyActivity).TitleProperty != "")
                {
                    MessageBox.Show("Creating Outlook Note");
                    newNote.Save();
                }
            }
            return ActivityExecutionStatus.Closed;
        }
    }
}
