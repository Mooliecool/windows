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

//NOTE: When changing the namespace; please update XmlnsDefinitionAttribute in AssemblyInfo.cs
namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class OutlookTask: BaseOutlookItem 
    {
        public OutlookTask()
        {
            InitializeComponent();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            // Create an Outlook Application object. 
            Outlook.Application outlookApp = new Outlook.Application();
            // Create a new TaskItem.
            Outlook.TaskItem newTask = (Outlook.TaskItem)outlookApp.CreateItem(Outlook.OlItemType.olTaskItem);
            // Configure the task at hand and save it.
            newTask.Body = "Workflow Generated Task";
            newTask.DueDate = DateTime.Now;
            newTask.Importance = Outlook.OlImportance.olImportanceHigh;

            if (this.Parent.Parent is ParallelActivity)
            {
                newTask.Subject = (this.Parent.Parent.Parent.Activities[1] as DummyActivity).TitleProperty;
                if ((this.Parent.Parent.Parent.Activities[1] as DummyActivity).TitleProperty != "")
                {
                    MessageBox.Show("Creating Outlook Task");
                    newTask.Save();
                }
            }
            else if (this.Parent.Parent is SequentialWorkflowActivity)
            {
                newTask.Subject = (this.Parent.Parent.Activities[1] as DummyActivity).TitleProperty;
                if ((this.Parent.Parent.Activities[1] as DummyActivity).TitleProperty != "")
                {
                    MessageBox.Show("Creating Outlook Task");
                    newTask.Save();
                }
            }
            return ActivityExecutionStatus.Closed;
        }
    }
}
