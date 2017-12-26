' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Reflection
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.WorkflowServices.Samples
	Partial Class CustomerWorkflow
#Region "Designer generated code"

        ''' <summary> 
        ''' Required method for Designer support - do not modify 
        ''' the contents of this method with the code editor.
        ''' </summary>
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim activitybind1 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding1 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind2 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding2 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind3 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding3 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo1 As New System.Workflow.Activities.TypedOperationInfo()
            Dim endpoint1 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind4 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding4 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind5 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding5 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind6 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding6 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo2 As New System.Workflow.Activities.TypedOperationInfo()
            Dim workflowserviceattributes1 As New System.Workflow.Activities.WorkflowServiceAttributes()
            Me.DoReviewOrder = New System.Workflow.Activities.CodeActivity()
            Me.ReceiveOrderDetails = New System.Workflow.Activities.ReceiveActivity()
            Me.DoDisplayStatus = New System.Workflow.Activities.CodeActivity()
            Me.SendSubmitOrder = New System.Workflow.Activities.SendActivity()
            ' 
            ' DoReviewOrder
            ' 
            Me.DoReviewOrder.Name = "DoReviewOrder"
            AddHandler Me.DoReviewOrder.ExecuteCode, AddressOf Me.ReviewOrder
            ' 
            ' ReceiveOrderDetails
            ' 
            Me.ReceiveOrderDetails.Activities.Add(Me.DoReviewOrder)
            Me.ReceiveOrderDetails.Name = "ReceiveOrderDetails"
            activitybind1.Name = "CustomerWorkflow"
            activitybind1.Path = "orderDetails"
            workflowparameterbinding1.ParameterName = "po"
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind1), System.Workflow.ComponentModel.ActivityBind))
            activitybind2.Name = "CustomerWorkflow"
            activitybind2.Path = "shippingQuote"
            workflowparameterbinding2.ParameterName = "quote"
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind2), System.Workflow.ComponentModel.ActivityBind))
            activitybind3.Name = "CustomerWorkflow"
            activitybind3.Path = "customerAck"
            workflowparameterbinding3.ParameterName = "(ReturnValue)"
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind3), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding1)
            Me.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding2)
            Me.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding3)
            typedoperationinfo1.ContractType = GetType(Microsoft.WorkflowServices.Samples.IOrderDetails)
            typedoperationinfo1.Name = "OrderDetails"
            Me.ReceiveOrderDetails.ServiceOperationInfo = typedoperationinfo1
            ' 
            ' DoDisplayStatus
            ' 
            Me.DoDisplayStatus.Name = "DoDisplayStatus"
            AddHandler Me.DoDisplayStatus.ExecuteCode, AddressOf Me.DisplayStatus
            ' 
            ' SendSubmitOrder
            ' 
            endpoint1.EndpointName = "SupplierEndPoint"
            endpoint1.Name = "SupplierEndPoint"
            endpoint1.OwnerActivityName = "CustomerWorkflow"
            Me.SendSubmitOrder.ChannelToken = endpoint1
            Me.SendSubmitOrder.Name = "SendSubmitOrder"
            activitybind4.Name = "CustomerWorkflow"
            activitybind4.Path = "order"
            workflowparameterbinding4.ParameterName = "po"
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind4), System.Workflow.ComponentModel.ActivityBind))
            activitybind5.Name = "CustomerWorkflow"
            activitybind5.Path = "contextToSend"
            workflowparameterbinding5.ParameterName = "context"
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind5), System.Workflow.ComponentModel.ActivityBind))
            activitybind6.Name = "CustomerWorkflow"
            activitybind6.Path = "supplierAck"
            workflowparameterbinding6.ParameterName = "(ReturnValue)"
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind6), System.Workflow.ComponentModel.ActivityBind))
            Me.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding4)
            Me.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding5)
            Me.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding6)
            typedoperationinfo2.ContractType = GetType(Microsoft.WorkflowServices.Samples.IOrder)
            typedoperationinfo2.Name = "SubmitOrder"
            Me.SendSubmitOrder.ServiceOperationInfo = typedoperationinfo2
            AddHandler Me.SendSubmitOrder.BeforeSend, AddressOf Me.PrepareOrder
            ' 
            ' CustomerWorkflow
            ' 
            Me.Activities.Add(Me.SendSubmitOrder)
            Me.Activities.Add(Me.DoDisplayStatus)
            Me.Activities.Add(Me.ReceiveOrderDetails)
            Me.Name = "CustomerWorkflow"
            Me.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1)
            Me.CanModifyActivities = False

        End Sub

#End Region

        Private DoDisplayStatus As CodeActivity
        Private DoReviewOrder As CodeActivity
        Private ReceiveOrderDetails As ReceiveActivity
        Private SendSubmitOrder As SendActivity











	End Class
End Namespace