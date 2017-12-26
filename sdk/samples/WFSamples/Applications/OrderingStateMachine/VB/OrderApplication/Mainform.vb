'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Reflection
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.Runtime

Public Class Mainform
    Private WithEvents runtime As WorkflowRuntime
    Private orderService As OrderService
    Private stateMachineInstances As Dictionary(Of String, StateMachineWorkflowInstance)

    Private Delegate Sub UpdateListItemDelegate(ByVal workflowInstance As WorkflowInstance, ByVal workflowState As String, ByVal workflowStatus As String)
    Private Delegate Sub UpdateButtonStatusDelegate()

    Private Const WorkflowInstanceIdColumnIndex As Integer = 0
    Private Const OrderIdColumnIndex As Integer = 1
    Private Const OrderStateColumnIndex As Integer = 2
    Private Const WorkflowStatusColumnIndex As Integer = 3

    Public Sub New()
        InitializeComponent()
    End Sub


    Private Sub Mainform_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load

        ' Create and start the WorkflowRuntime
        Me.StartWorkflowRuntime()

        ' Create a new dictionary to store the StateMachineInstance objects
        Me.stateMachineInstances = New Dictionary(Of String, StateMachineWorkflowInstance)()

        ' Disable all of the event buttons
        Me.DisableUI()

        'Hook up event handler to buttons
        AddHandler buttonOrderUpdated.Click, AddressOf Me.ButtonOrderEvent_Click
        AddHandler buttonOrderProcessed.Click, AddressOf Me.ButtonOrderEvent_Click
        AddHandler buttonOrderCanceled.Click, AddressOf Me.ButtonOrderEvent_Click
        AddHandler buttonOrderShipped.Click, AddressOf Me.ButtonOrderEvent_Click

    End Sub

    Private Sub StartWorkflowRuntime()

        ' Create a new Workflow Runtime for me application
        runtime = New WorkflowRuntime()

        ' Add the External Data Exchange Service
        Dim dataExchangeService As New ExternalDataExchangeService()
        runtime.AddService(dataExchangeService)

        ' Add a new instance of the OrderService to the External Data Exchange Service
        orderService = New OrderService()
        dataExchangeService.AddService(orderService)

        ' Start the Workflow services
        runtime.StartRuntime()
    End Sub

    Private Sub ButtonOrderCreated_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonOrderCreated.Click

        ' Get the OrderId that was entered by the user
        Dim orderId As String = Me.textBoxOrderID.Text

        ' Start the Order Workflow
        Dim workflowInstanceId As Guid = Me.StartOrderWorkflow()

        ' Raise an OrderCreated event using the Order Local Service
        orderService.RaiseOrderCreatedEvent(orderId, workflowInstanceId)

        ' Add a new item to the ListView for the new workflow
        AddListViewItem(orderId, workflowInstanceId)

        ' Reset the form for adding another Order
        Me.textBoxOrderID.Text = ""
    End Sub

    Private Sub ButtonOrderEvent_Click(ByVal sender As Object, ByVal e As EventArgs)
        ' Get the Name for the button that was clicked
        Dim buttonName As String = (CType(sender, Button)).Name

        ' Get the workflowInstanceId for the selected order
        Dim workflowInstanceId As Guid = Me.GetSelectedWorkflowInstanceID()

        ' Get the OrderID for the selected order
        Dim orderId As String = Me.GetSelectedOrderId()

        ' Disable all of the event buttons
        Me.DisableUI()

        Select Case buttonName
            Case "buttonOrderShipped"
                ' Raise an OrderShipped event using the Order Local Service
                orderService.RaiseOrderShippedEvent(orderId, workflowInstanceId)

            Case "buttonOrderUpdated"
                ' Raise an OrderUpdated event using the Order Local Service
                orderService.RaiseOrderUpdatedEvent(orderId, workflowInstanceId)

            Case "buttonOrderCanceled"
                ' Raise an OrderCanceled event using the Order Local Service
                orderService.RaiseOrderCanceledEvent(orderId, workflowInstanceId)

            Case "buttonOrderProcessed"
                ' Raise an OrderProcessed event using the Order Local Service
                orderService.RaiseOrderProcessedEvent(orderId, workflowInstanceId)
        End Select
    End Sub

    Private Function StartOrderWorkflow() As Guid
        ' NOTE:  "Late-binding to the OrderWorkflows" assembly so we
        ' ...can easily copy this exe and project into another soluton

        ' Load the OrderWorkflows assembly
        Dim orderWorkflowsAssembly As Assembly = System.Reflection.Assembly.Load("OrderWorkflows")

        ' Get a reference to the System.Type for the OrderWorkflows.Workflow1
        Dim workflowType As Type = orderWorkflowsAssembly.GetType("Microsoft.Samples.Workflow.OrderingStateMachine.SampleWorkflow")

        If workflowType Is Nothing Then Return Guid.Empty

        Dim instance As WorkflowInstance = runtime.CreateWorkflow(workflowType)

        Dim stateMachineInstance As New StateMachineWorkflowInstance(runtime, instance.InstanceId)

        instance.Start()

        ' Add the StateMachineInstance object for our Workflow to our dictionary
        Me.stateMachineInstances.Add(instance.InstanceId.ToString(), stateMachineInstance)

        ' Return the WorkflowInstanceId
        Return instance.InstanceId
    End Function

    Private Function AddListViewItem(ByVal orderId As String, ByVal workflowInstanceId As Guid) As ListViewItem
        ' Add a new item to the Listview control using the WorkflowInstanceId
        ' ...as the Text and Key value
        Dim item As ListViewItem = Me.listViewOrders.Items.Add(workflowInstanceId.ToString(), workflowInstanceId.ToString(), "")

        ' add the OrderId, Workflow State, and Workflow Status columns
        item.SubItems.Add(orderId)
        item.SubItems.Add("")
        item.SubItems.Add("")
        item.Tag = workflowInstanceId.ToString()

        ' Select the new ListItem, which will cause the buttons to refresh.
        item.Selected = True

        ' Return the new ListViewItem
        Return item
    End Function


    Private Sub Runtime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs) Handles runtime.WorkflowCompleted
        Me.UpdateListItem(e.WorkflowInstance, "", "Completed")
    End Sub


    Private Sub Runtime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs) Handles runtime.WorkflowTerminated
        Me.UpdateListItem(e.WorkflowInstance, "", "Terminated")
    End Sub


    Private Function GetSelectedWorkflowInstanceID() As Guid
        If Me.listViewOrders.SelectedItems.Count = 0 Then
            Throw New ApplicationException("No Orders are selected")
        End If

        ' Get the WorkflowInstanceId for the selected ListItem
        Dim workflowInstanceId As String = Me.listViewOrders.SelectedItems(WorkflowInstanceIdColumnIndex).Text

        ' Create a new GUID for the WorkflowInstanceID
        Return New Guid(workflowInstanceId)
    End Function


    Private Function GetSelectedOrderId() As String

        If Me.listViewOrders.SelectedItems.Count = 0 Then
            Throw New ApplicationException("No Orders are selected")
        End If

        ' Get the OrderID for the selected order
        Return Me.listViewOrders.SelectedItems(0).SubItems(OrderIdColumnIndex).Text
    End Function

    Private Sub Runtime_WorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs) Handles runtime.WorkflowIdled
        ' Get the underlying WorkflowInstance
        Dim stateMachineInstance As New StateMachineWorkflowInstance(runtime, e.WorkflowInstance.InstanceId)

        ' Update the Workflow State & Status on the ListItem 
        Me.UpdateListItem(stateMachineInstance.WorkflowInstance, stateMachineInstance.CurrentState.Name, "Running")

        ' Update the status of the buttons for the selected ListItem
        Me.UpdateUI()
    End Sub

    Private Function GetSelectedStateMachineInstance() As StateMachineWorkflowInstance
        ' Return the StateMachineInstance object.  If the workflow has completed, return null.
        Try
            Return New StateMachineWorkflowInstance(runtime, Me.GetSelectedWorkflowInstanceID())
        Catch ex As InvalidOperationException
            Return Nothing
        End Try
    End Function

    Private Sub ListViewOrders_ItemSelectionChanged(ByVal sender As Object, ByVal e As ListViewItemSelectionChangedEventArgs) Handles listViewOrders.ItemSelectionChanged
        If (e.Item.Selected) Then
            Me.UpdateUI()
        Else
            Me.DisableUI()
        End If
    End Sub

    Private Sub UpdateListItem(ByVal workflowInstance As WorkflowInstance, ByVal workflowState As String, ByVal workflowStatus As String)

        If (Me.listViewOrders.InvokeRequired) Then

            ' this code is executing on a different thread than the one that
            ' ...created the ListView, so we need to use the Invoke method.

            ' Create an instance of the delegate for invoking me method
            Dim updateListViewItem As New UpdateListItemDelegate(AddressOf UpdateListItem)

            ' Create the array of parameters for me method
            Dim args As Object() = New Object(2) {workflowInstance, workflowState, workflowStatus}


            ' Invoke me method on the UI thread
            Me.listViewOrders.Invoke(updateListViewItem, args)

        Else

            ' Get the ListViewItem for the specified WorkflowInstance
            Dim instanceId As String = workflowInstance.InstanceId.ToString()
            Dim itemOrder As ListViewItem = listViewOrders.Items(instanceId)

            If itemOrder Is Nothing Then Return

            ' Update the Workflow State & Status column values
            itemOrder.SubItems(OrderStateColumnIndex).Text = workflowState
            itemOrder.SubItems(WorkflowStatusColumnIndex).Text = workflowStatus
        End If
    End Sub



    Private Sub UpdateUI()
        If (Me.InvokeRequired) Then

            ' this code is executing on a different thread than the one that
            ' ...created the ListView, so we need to use the Invoke method.

            ' Create an instance of the delegate for invoking me method
            Dim update As New UpdateButtonStatusDelegate(AddressOf Me.UpdateUI)

            ' Invoke me method on the UI thread
            Me.Invoke(update)
        Else
            DisableUI()
            EnableUI()
        End If
    End Sub



    Private Sub EnableUI()

        ' Make sure an item in the ListView is selected
        If Me.listViewOrders.SelectedItems.Count = 0 Then Return

        ' If the workflow is Completed or Terminated, then don't enable any buttons
        Dim workflowStatus As String = Me.listViewOrders.SelectedItems(0).SubItems(WorkflowStatusColumnIndex).Text

        If workflowStatus.Equals("Completed") Or workflowStatus.Equals("Terminated") Then Return

        ' Return the StateMachineInstance object 
        Dim stateMachineInstance As StateMachineWorkflowInstance = Me.GetSelectedStateMachineInstance()

        ' If the State is not set, then don't enable any buttons
        If stateMachineInstance Is Nothing OrElse stateMachineInstance.CurrentState Is Nothing Then Return

        ' Enable the buttons on me form, based on the Messages (events)
        ' ...allowed into the State Machine workflow, based on its current state

        Dim subActivities As ReadOnlyCollection(Of WorkflowQueueInfo) = stateMachineInstance.WorkflowInstance.GetWorkflowQueueData()
        Dim MessagesAllowed As New Collection(Of String)()
        For Each queueInfo As WorkflowQueueInfo In subActivities

            ' Cast the Queue Name to the class Event Queue Name to get the event name string
            Dim queuename As EventQueueName = TryCast(queueInfo.QueueName, EventQueueName)

            If queuename IsNot Nothing Then
                MessagesAllowed.Add(queuename.MethodName)
            End If
        Next

        If (MessagesAllowed.Contains("OrderCanceled")) Then
            Me.buttonOrderCanceled.Enabled = True
        End If

        If (MessagesAllowed.Contains("OrderProcessed")) Then
            Me.buttonOrderProcessed.Enabled = True
        End If

        If (MessagesAllowed.Contains("OrderShipped")) Then
            Me.buttonOrderShipped.Enabled = True
        End If

        If (MessagesAllowed.Contains("OrderUpdated")) Then
            Me.buttonOrderUpdated.Enabled = True
        End If

        ' Load the list of available states into the SetState combo box and enable it
        Me.comboBoxWorkflowStates.Enabled = True
        Me.buttonSetState.Enabled = True
        Me.PopulateSetStateComboBox()

        Me.addOnHoldStateToolStripMenuItem.Enabled = True

    End Sub


    Private Sub DisableUI()
        Me.buttonOrderCanceled.Enabled = False
        Me.buttonOrderProcessed.Enabled = False
        Me.buttonOrderShipped.Enabled = False
        Me.buttonOrderUpdated.Enabled = False

        ' Disable the SetState combobox & button
        Me.comboBoxWorkflowStates.Enabled = False
        Me.buttonSetState.Enabled = False

        Me.addOnHoldStateToolStripMenuItem.Enabled = False

    End Sub



    Private Sub ButtonSetState_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonSetState.Click

        ' If nothing is selected, return.
        If comboBoxWorkflowStates.SelectedItem Is Nothing Then Return

        ' Get a reference to the StateMachineInstance for the selected workflow
        Dim targetStateMachineInstance As StateMachineWorkflowInstance = Me.GetSelectedStateMachineInstance()

        If targetStateMachineInstance Is Nothing Then
            MessageBox.Show("Please select a order and try again.", Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        Else
            ' Get the ID of the state that was selected in the drop-down list
            Dim selectedStateId As String = Me.comboBoxWorkflowStates.SelectedItem.ToString()

            ' Change the state of the workflow to the selected state
            targetStateMachineInstance.SetState(selectedStateId)
        End If
        UpdateUI()
    End Sub

    Private Sub PopulateSetStateComboBox()

        ' clear the items in the combo box and reload them
        comboBoxWorkflowStates.Items.Clear()

        ' Get a reference to the StateMachineInstance for the selected workflow
        Dim targetStateMachineInstance As StateMachineWorkflowInstance = Me.GetSelectedStateMachineInstance()

        ' Get the list of states for the selected StateMachine WorkflowInstance
        Dim states As ReadOnlyCollection(Of StateActivity) = targetStateMachineInstance.States

        ' Add the Id for each possible state to the comboBox
        For Each possibleState As StateActivity In states
            Me.comboBoxWorkflowStates.Items.Add(possibleState.Name)
        Next
    End Sub

    Private Sub ContextMenuOrdersList_ItemClicked(ByVal sender As Object, ByVal e As ToolStripItemClickedEventArgs) Handles contextMenuOrdersList.ItemClicked

        If Me.listViewOrders.SelectedItems.Count = 0 Then Return

        If e.ClickedItem.Name.Equals("addOnHoldStateToolStripMenuItem") Then

            ' Perform a dynamic update by adding a state to the workflow
            AddOrderOnHoldState()

            ' Refresh the list of states in the SetState dropdown lit
            Me.PopulateSetStateComboBox()
        End If
    End Sub

    Private Sub AddOrderOnHoldState()

        ' Get a reference to the WorkflowInstance for the selected workflow
        Dim instance As WorkflowInstance = Me.runtime.GetWorkflow(Me.GetSelectedWorkflowInstanceID())

        ' Get a reference to the root activity for the workflow
        Dim root As Activity = instance.GetWorkflowDefinition()

        ' Create a new instance of the WorkflowChanges class for managing
        ' the in-memory changes to the workflow
        Dim changes As New WorkflowChanges(root)

        ' Create a new State activity to the workflow
        Dim orderOnHoldState As New StateActivity()
        orderOnHoldState.Name = "OrderOnHoldState"

        ' Add a new EventDriven activity to the State
        Dim eventDrivenDelay As New EventDrivenActivity()
        eventDrivenDelay.Name = "DelayOrderEvent"
        orderOnHoldState.Activities.Add(eventDrivenDelay)

        ' Add a new Delay, initialized to 5 seconds
        Dim delayOrder As New DelayActivity()
        delayOrder.Name = "delayOrder"
        delayOrder.TimeoutDuration = New TimeSpan(0, 0, 5)
        eventDrivenDelay.Activities.Add(delayOrder)

        ' Add a new SetState to the OrderOpenState
        Dim setStateOrderOpen As New SetStateActivity()
        setStateOrderOpen.TargetStateName = "OrderOpenState"
        eventDrivenDelay.Activities.Add(setStateOrderOpen)

        ' Add the OnHoldState to the workflow
        changes.TransientWorkflow.Activities.Add(orderOnHoldState)

        ' Apply the changes to the workflow instance
        Try
            instance.ApplyWorkflowChanges(changes)
        Catch e As WorkflowValidationFailedException
            ' New state has already been added
            MessageBox.Show("On Hold state has already been added to this workflow.")
        End Try
    End Sub

    Private Sub Mainform_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs) Handles Me.FormClosing
        Me.runtime.StopRuntime()
        Me.runtime.Dispose()
    End Sub
End Class
