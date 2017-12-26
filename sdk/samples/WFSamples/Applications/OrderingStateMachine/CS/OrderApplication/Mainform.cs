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
using System.Collections.ObjectModel;
using System.Reflection;
using System.Windows.Forms;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Diagnostics;

namespace Microsoft.Samples.Workflow.OrderApplication
{
    public partial class Mainform : Form
    {
        private WorkflowRuntime runtime;
        private OrderService orderService;
        private Dictionary<string, StateMachineWorkflowInstance> stateMachineInstances;

        private delegate void UpdateListItemDelegate(WorkflowInstance workflowInstance, string workflowState, string workflowStatus);
        private delegate void UpdateButtonStatusDelegate();

        private const int WorkflowInstanceIdColumnIndex = 0;
        private const int OrderIdColumnIndex = 1;
        private const int OrderStateColumnIndex = 2;
        private const int WorkflowStatusColumnIndex = 3;

        public Mainform()
        {
            InitializeComponent();
        }

        private void Mainform_Load(object sender, EventArgs e)
        {
            // Create and start the WorkflowRuntime
            this.StartWorkflowRuntime();

            // Create a new dictionary to store the StateMachineInstance objects
            this.stateMachineInstances = new Dictionary<string, StateMachineWorkflowInstance>();

            // Disable all of the event buttons
            this.DisableUI();
        }

        private void StartWorkflowRuntime()
        {
            // Create a new Workflow Runtime for this application
            runtime = new WorkflowRuntime();

            // Create EventHandlers for the WorkflowRuntime
            runtime.WorkflowTerminated += new EventHandler<WorkflowTerminatedEventArgs>(Runtime_WorkflowTerminated);
            runtime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(Runtime_WorkflowCompleted);
            runtime.WorkflowIdled += new EventHandler<WorkflowEventArgs>(Runtime_WorkflowIdled);            
            // Add the External Data Exchange Service
            ExternalDataExchangeService dataExchangeService = new ExternalDataExchangeService();
            runtime.AddService(dataExchangeService);

            // Add a new instance of the OrderService to the External Data Exchange Service
            orderService = new OrderService();
            dataExchangeService.AddService(orderService);

            // Start the Workflow services
            runtime.StartRuntime();
        }


        private void ButtonOrderCreated_Click(object sender, EventArgs e)
        {
            // Get the OrderId that was entered by the user
            string orderId = this.textBoxOrderID.Text;

            // Start the Order Workflow
            Guid workflowInstanceId =
                this.StartOrderWorkflow();


            // Raise an OrderCreated event using the Order Local Service
            orderService.RaiseOrderCreatedEvent(orderId, workflowInstanceId);

            // Add a new item to the ListView for the new workflow
            AddListViewItem(orderId, workflowInstanceId);

            // Reset the form for adding another Order
            this.textBoxOrderID.Text = "";
        }

        private void ButtonOrderEvent_Click(object sender, EventArgs e)
        {
            // Get the Name for the button that was clicked
            string buttonName = ((Button)sender).Name;

            // Get the workflowInstanceId for the selected order
            Guid workflowInstanceId = this.GetSelectedWorkflowInstanceID();

            // Get the OrderID for the selected order
            string orderId = this.GetSelectedOrderId();

            // Disable all of the event buttons
            this.DisableUI();

            switch (buttonName)
            {
                case "buttonOrderShipped":
                    // Raise an OrderShipped event using the Order Local Service
                    orderService.RaiseOrderShippedEvent(orderId, workflowInstanceId);
                    break;

                case "buttonOrderUpdated":
                    // Raise an OrderUpdated event using the Order Local Service
                    orderService.RaiseOrderUpdatedEvent(orderId, workflowInstanceId);
                    break;

                case "buttonOrderCanceled":
                    // Raise an OrderCanceled event using the Order Local Service
                    orderService.RaiseOrderCanceledEvent(orderId, workflowInstanceId);
                    break;

                case "buttonOrderProcessed":
                    // Raise an OrderProcessed event using the Order Local Service
                    orderService.RaiseOrderProcessedEvent(orderId, workflowInstanceId);
                    break;
            }
        }


        private Guid StartOrderWorkflow()
        {
            // NOTE:  "Late-binding to the OrderWorkflows" assembly so we
            // ...can easily copy this exe and project into another soluton

            // Load the OrderWorkflows assembly
            Assembly orderWorkflowsAssembly =
                Assembly.Load("OrderWorkflows");

            // Get a reference to the System.Type for the OrderWorkflows.Workflow1
            Type workflowType = orderWorkflowsAssembly.GetType("Microsoft.Samples.Workflow.OrderApplication.SampleWorkflow");

            if (workflowType == null) return Guid.Empty;

            WorkflowInstance instance = runtime.CreateWorkflow(workflowType);

            StateMachineWorkflowInstance stateMachineInstance = new StateMachineWorkflowInstance(runtime, instance.InstanceId);

            instance.Start();

            // Add the StateMachineInstance object for our Workflow to our dictionary
            this.stateMachineInstances.Add(instance.InstanceId.ToString(), stateMachineInstance);

            // Return the WorkflowInstanceId
            return instance.InstanceId;
        }


        private ListViewItem AddListViewItem(string orderId, Guid workflowInstanceId)
        {
            // Add a new item to the Listview control using the WorkflowInstanceId
            // ...as the Text and Key value
            ListViewItem item =
                this.listViewOrders.Items.Add(workflowInstanceId.ToString(),
                workflowInstanceId.ToString(), "");

            // add the OrderId, Workflow State, and Workflow Status columns
            item.SubItems.Add(orderId);
            item.SubItems.Add("");
            item.SubItems.Add("");
            item.Tag = workflowInstanceId.ToString();

            // Select the new ListItem, which will cause the buttons to refresh.
            item.Selected = true;

            // Return the new ListViewItem
            return item;
        }

        void Runtime_WorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            this.UpdateListItem(e.WorkflowInstance, "", "Completed");
        }

        void Runtime_WorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            this.UpdateListItem(e.WorkflowInstance, "", "Terminated");
        }

        private Guid GetSelectedWorkflowInstanceID()
        {
            if (this.listViewOrders.SelectedItems.Count == 0)
            {
                throw new ApplicationException("No Orders are selected");
            }

            // Get the WorkflowInstanceId for the selected ListItem
            string workflowInstanceId =
                this.listViewOrders.SelectedItems[WorkflowInstanceIdColumnIndex].Text;

            // Create a new GUID for the WorkflowInstanceID
            return new Guid(workflowInstanceId);
        }

        private string GetSelectedOrderId()
        {
            if (this.listViewOrders.SelectedItems.Count == 0)
            {
                throw new ApplicationException("No Orders are selected");
            }

            // Get the OrderID for the selected order
            return this.listViewOrders.SelectedItems[0].SubItems[OrderIdColumnIndex].Text;
        }


        private void Runtime_WorkflowIdled(object sender, WorkflowEventArgs e)
        {
            // Get the underlying WorkflowInstance
            StateMachineWorkflowInstance stateMachineInstance = new StateMachineWorkflowInstance(runtime, e.WorkflowInstance.InstanceId);

            // Update the Workflow State & Status on the ListItem 
            this.UpdateListItem(stateMachineInstance.WorkflowInstance,stateMachineInstance.CurrentState.Name, "Running");

            // Update the status of the buttons for the selected ListItem
            this.UpdateUI();
        }

        private StateMachineWorkflowInstance GetSelectedStateMachineInstance()
        {
            // Return the StateMachineInstance object.  If the workflow has completed, return Null.
            try
            {
                return new StateMachineWorkflowInstance(runtime, this.GetSelectedWorkflowInstanceID());
            }
            catch (InvalidOperationException)
            {
                return null;
            }
        }

        private void ListViewOrders_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            if (e.Item.Selected)
            {
                this.UpdateUI();
            }
            else
            {
                this.DisableUI();
            }
        }


        private void UpdateListItem(WorkflowInstance workflowInstance, string workflowState, string workflowStatus)
        {
            if (this.listViewOrders.InvokeRequired)
            {
                // This code is executing on a different thread than the one that
                // ...created the ListView, so we need to use the Invoke method.
                
                // Create an instance of the delegate for invoking this method
                UpdateListItemDelegate updateListViewItem =
                    new UpdateListItemDelegate(this.UpdateListItem);

                // Create the array of parameters for this method
                object[] args = new object[3] { workflowInstance, workflowState, workflowStatus };

                // Invoke this method on the UI thread
                this.listViewOrders.Invoke(updateListViewItem, args);
            }
            else
            {
                // Get the ListViewItem for the specified WorkflowInstance
                string instanceId = workflowInstance.InstanceId.ToString();
                ListViewItem itemOrder = listViewOrders.Items[instanceId];

                if (itemOrder == null)
                {
                    return;
                }

                // Update the Workflow State & Status column values
                itemOrder.SubItems[OrderStateColumnIndex].Text = workflowState;
                itemOrder.SubItems[WorkflowStatusColumnIndex].Text = workflowStatus;
            }
        }

        private void UpdateUI()
        {
            if (this.InvokeRequired)
            {
                // This code is executing on a different thread than the one that
                // ...created the ListView, so we need to use the Invoke method.

                // Create an instance of the delegate for invoking this method
                UpdateButtonStatusDelegate update =
                    new UpdateButtonStatusDelegate(this.UpdateUI);

                // Invoke this method on the UI thread
                this.Invoke(update);
            }
            else
            {
                DisableUI();
                EnableUI();
            }
        }

        private void EnableUI()
        {
            // Make sure an item in the ListView is selected
            if (this.listViewOrders.SelectedItems.Count == 0)
            {
                return;
            }

            // If the workflow is Completed or Terminated, then don't enable any buttons
            string workflowStatus = 
                this.listViewOrders.SelectedItems[0].SubItems[WorkflowStatusColumnIndex].Text;

            if ((workflowStatus.Equals("Completed"))
                || (workflowStatus.Equals("Terminated")))
            {
                return;
            }

            // Return the StateMachineInstance object 
            StateMachineWorkflowInstance stateMachineInstance = this.GetSelectedStateMachineInstance();
            
            // If the State is not set, then don't enable any buttons
            if (stateMachineInstance == null || stateMachineInstance.CurrentState == null)
            {
                return;
            }

            // Enable the buttons on this form, based on the Messages (events)
            // ...allowed into the State Machine workflow, based on its current state


            ReadOnlyCollection<WorkflowQueueInfo> subActivities = stateMachineInstance.WorkflowInstance.GetWorkflowQueueData();
            Collection<string> MessagesAllowed = new Collection<string>();
            foreach (WorkflowQueueInfo queueInfo in subActivities)
            {
                // Cast the Queue Name to the class Event Queue Name to get the event name string
                EventQueueName queuename = queueInfo.QueueName as EventQueueName;

                if (queuename != null)
                {
                    MessagesAllowed.Add(queuename.MethodName);
                }
            }

            if (MessagesAllowed.Contains("OrderCanceled"))
                this.buttonOrderCanceled.Enabled = true;

            if (MessagesAllowed.Contains("OrderProcessed"))
                this.buttonOrderProcessed.Enabled = true;

            if (MessagesAllowed.Contains("OrderShipped"))
                this.buttonOrderShipped.Enabled = true;

            if (MessagesAllowed.Contains("OrderUpdated"))
                this.buttonOrderUpdated.Enabled = true;

            // Load the list of available states into the SetState combo box and enable it
            this.comboBoxWorkflowStates.Enabled = true;
            this.buttonSetState.Enabled = true;
            this.PopulateSetStateComboBox();

            this.addOnHoldStateToolStripMenuItem.Enabled = true;
        }

        private void DisableUI()
        {
            this.buttonOrderCanceled.Enabled = false;
            this.buttonOrderProcessed.Enabled = false;
            this.buttonOrderShipped.Enabled = false;
            this.buttonOrderUpdated.Enabled = false;

            // Disable the SetState combobox & button
            this.comboBoxWorkflowStates.Enabled = false;
            this.buttonSetState.Enabled = false;

            this.addOnHoldStateToolStripMenuItem.Enabled = false;
        }

        private void ButtonSetState_Click(object sender, EventArgs e)
        {
            // If nothing is selected, return.
            if (comboBoxWorkflowStates.SelectedItem == null)
                return;

            // Get a reference to the StateMachineInstance for the selected workflow
            StateMachineWorkflowInstance targetStateMachineInstance = 
                this.GetSelectedStateMachineInstance();

            
            if (targetStateMachineInstance == null)
            {
                MessageBox.Show("Please select a order and try again.", this.Text, 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            else
            {
                // Get the ID of the state that was selected in the drop-down list
                string selectedStateId = this.comboBoxWorkflowStates.SelectedItem.ToString();

                // Change the state of the workflow to the selected state
                targetStateMachineInstance.SetState(selectedStateId);
            }
            UpdateUI();
        }

        private void PopulateSetStateComboBox()
        {
            // clear the items in the combo box and reload them
            comboBoxWorkflowStates.Items.Clear();

            // Get a reference to the StateMachineInstance for the selected workflow
            StateMachineWorkflowInstance targetStateMachineInstance = this.GetSelectedStateMachineInstance();

            // Get the list of states for the selected StateMachine WorkflowInstance
            ReadOnlyCollection<StateActivity> states = targetStateMachineInstance.States;

            // Add the Id for each possible state to the comboBox
            foreach (StateActivity possibleState in states)
            {
                this.comboBoxWorkflowStates.Items.Add(possibleState.Name);
            }
        }

        private void ContextMenuOrdersList_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            if (this.listViewOrders.SelectedItems.Count == 0)
                return;
            if (e.ClickedItem.Name.Equals("addOnHoldStateToolStripMenuItem"))
            {
                // Perform a dynamic update by adding a state to the workflow
                AddOrderOnHoldState();

                // Refresh the list of states in the SetState dropdown lit
                this.PopulateSetStateComboBox();
            }
        }

        private void AddOrderOnHoldState()
        {
            // Get a reference to the WorkflowInstance for the selected workflow
            WorkflowInstance instance =
                this.runtime.GetWorkflow(this.GetSelectedWorkflowInstanceID());

            // Get a reference to the root activity for the workflow
            Activity root = instance.GetWorkflowDefinition();

            // Create a new instance of the WorkflowChanges class for managing
            // the in-memory changes to the workflow
            WorkflowChanges changes = new WorkflowChanges(root);

            // Create a new State activity to the workflow
            StateActivity orderOnHoldState = new StateActivity();
            orderOnHoldState.Name = "OrderOnHoldState";

            // Add a new EventDriven activity to the State
            EventDrivenActivity eventDrivenDelay = new EventDrivenActivity();
            eventDrivenDelay.Name = "DelayOrderEvent";
            orderOnHoldState.Activities.Add(eventDrivenDelay);

            // Add a new Delay, initialized to 5 seconds
            DelayActivity delayOrder = new DelayActivity();
            delayOrder.Name = "delayOrder";
            delayOrder.TimeoutDuration = new TimeSpan(0, 0, 5);
            eventDrivenDelay.Activities.Add(delayOrder);

            // Add a new SetState to the OrderOpenState
            SetStateActivity setStateOrderOpen = new SetStateActivity();
            setStateOrderOpen.TargetStateName = "OrderOpenState";
            eventDrivenDelay.Activities.Add(setStateOrderOpen);

            // Add the OnHoldState to the workflow
            changes.TransientWorkflow.Activities.Add(orderOnHoldState);
            
            // Apply the changes to the workflow instance
            try
            {
                instance.ApplyWorkflowChanges(changes);
            }
            catch (WorkflowValidationFailedException)
            {
                // New state has already been added
                MessageBox.Show("On Hold state has already been added to this workflow.");
            }
        }

        private void Mainform_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Stop the workflow runtime engine.
            this.runtime.StopRuntime();
            this.runtime.Dispose();
        }
    }
}