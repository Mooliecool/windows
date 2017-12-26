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
using System.Collections.ObjectModel;
using System.Windows.Forms;
using System.Workflow.Runtime;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SpeechApplication
{
    public partial class Form1 : Form
    {
        private WorkflowRuntime workflowRuntime;
        private SpeechService speechService;
        private StateMachineWorkflowInstance stateMachineInstance;

        private delegate void UpdateListItemDelegate(WorkflowInstance workflowInstance, string workflowState, string workflowStatus);
        private delegate void UpdateButtonStatusDelegate();
        private delegate void UpdateMenuTextDelegate(object sender, UpdatePhoneTextEventArgs e);

        public string MessageText
        {
            get
            {
                return this.txtPhoneMessage.Text;
            }
            set
            {
                this.txtPhoneMessage.Text = value;
            }
        }


        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Create and start the WorkflowRuntime
            this.StartWorkflowRuntime();


            // Disable all of the event buttons
            this.DisableButtons();
        }

        private void StartWorkflowRuntime()
        {
            // Create a new Workflow Runtime for this application
            workflowRuntime = new WorkflowRuntime();

            // Create EventHandlers for the WorkflowRuntime
            workflowRuntime.WorkflowTerminated += new EventHandler<WorkflowTerminatedEventArgs>(WorkflowRuntime_WorkflowTerminated);
            workflowRuntime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(WorkflowRuntime_WorkflowCompleted);
            workflowRuntime.WorkflowIdled += new EventHandler<WorkflowEventArgs>(workflowRuntime_WorkflowIdled);            
            
            // Start the Workflow services
            workflowRuntime.StartRuntime();

            // Add an instance of the External Data Exchange Service
            ExternalDataExchangeService externalDataExchangeService = new ExternalDataExchangeService();
            workflowRuntime.AddService(externalDataExchangeService);
            
            // Add a new instance of the OrderService to the externalDataExchangeService
            speechService = new SpeechService();
            externalDataExchangeService.AddService(speechService);

            // Subscribe to the menu text changed event
            speechService.PhoneTextChangedEventHandler += new SpeechService.UpdatePhoneTextEventHandler(UpdatePhoneTextEventHandler);
        }

        private void btnStartWorkflow_Click(object sender, EventArgs e)
        {
            // Start the Workflow
            stateMachineInstance = this.StartSpeechWorkflow();
        }


        private void DisableButtons()
        {
            this.button0.Enabled = false;
            this.button1.Enabled = false;
            this.button2.Enabled = false;
            this.button3.Enabled = false;
            this.button4.Enabled = false;
            this.button5.Enabled = false;
            this.button6.Enabled = false;
            this.button7.Enabled = false;
            this.button8.Enabled = false;
            this.button9.Enabled = false;
            this.buttonstar.Enabled = false;
            this.buttonpound.Enabled = false;
        }

        private void btnPhone_Click(object sender, EventArgs e)
        {
            // Get the Name for the button that was clicked
            string buttonName = ((Button)sender).Name;


            // Disable all of the event buttons
            this.DisableButtons();

            speechService.RaiseButtonPressed(new SpeechEventArgs(stateMachineInstance.InstanceId, buttonName));
        }

        private StateMachineWorkflowInstance StartSpeechWorkflow()
        {

            WorkflowInstance wi = this.workflowRuntime.CreateWorkflow(typeof(Workflow1));
            wi.Start();
            return new StateMachineWorkflowInstance(this.workflowRuntime, wi.InstanceId);
        }


        void WorkflowRuntime_WorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
        }

        void WorkflowRuntime_WorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine("The workflow has terminated");
        }


        void workflowRuntime_WorkflowIdled(object sender, WorkflowEventArgs e)
        {
            this.UpdateButtonStatus();
        }

        private void UpdatePhoneTextEventHandler(object sender, UpdatePhoneTextEventArgs e)
        {
            if (this.InvokeRequired)
            {
                UpdateMenuTextDelegate updateMenuText = new UpdateMenuTextDelegate(UpdatePhoneTextEventHandler);
                object[] args = new object[2] { this, e };
                this.Invoke(updateMenuText, args);
            }
            else
            {
                this.txtPhoneMessage.Text = e.MenuText;
            }
        }

        private void UpdateButtonStatus()
        {
            if (this.InvokeRequired)
            {
                // This code is executing on a different thread than the one that
                // ...created the ListView, so we need to use the Invoke method.

                // Create an instance of the delegate for invoking this method
                UpdateButtonStatusDelegate updateButtonStatus =
                    new UpdateButtonStatusDelegate(this.UpdateButtonStatus);

                // Invoke this method on the UI thread
                this.Invoke(updateButtonStatus);
            }
            else
            {
                DisableButtons();
                EnableButtons();
            }
        }


        private void EnableButtons()
        {

            // If the State is not set, then don't enable any buttons
            if (stateMachineInstance.CurrentState == null)
            {
                return;
            }

            // Enable the buttons on this form, based on the Messages (events)
            // ...allowed into the State Machine workflow, based on it's current state


            ReadOnlyCollection<WorkflowQueueInfo> queues = stateMachineInstance.WorkflowInstance.GetWorkflowQueueData();

            Collection<string> MessagesAllowed = new Collection<string>();
            
            foreach (WorkflowQueueInfo s in queues)
            {
                EventQueueName eventQueueName = s.QueueName as EventQueueName;
                if (eventQueueName != null)
                {
                    MessagesAllowed.Add(eventQueueName.MethodName);
                }
            }

            if (MessagesAllowed.Contains("Button0Pressed"))
                this.button0.Enabled = true;

            if (MessagesAllowed.Contains("Button1Pressed"))
                this.button1.Enabled = true;

            if (MessagesAllowed.Contains("Button2Pressed"))
                this.button2.Enabled = true;

            if (MessagesAllowed.Contains("Button3Pressed"))
                this.button3.Enabled = true;

            if (MessagesAllowed.Contains("Button4Pressed"))
                this.button4.Enabled = true;

            if (MessagesAllowed.Contains("Button5Pressed"))
                this.button5.Enabled = true;
            
            if (MessagesAllowed.Contains("Button6Pressed"))
                this.button6.Enabled = true;
            
            if (MessagesAllowed.Contains("Button7Pressed"))
                this.button7.Enabled = true;
            
            if (MessagesAllowed.Contains("Button8Pressed"))
                this.button8.Enabled = true;
            
            if (MessagesAllowed.Contains("Button9Pressed"))
                this.button9.Enabled = true;
            
            if (MessagesAllowed.Contains("ButtonStarPressed"))
                this.buttonstar.Enabled = true;
            
            if (MessagesAllowed.Contains("ButtonPoundPressed"))
                this.buttonpound.Enabled = true;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.workflowRuntime.StopRuntime();
            this.workflowRuntime.Dispose();
        }
    }

}