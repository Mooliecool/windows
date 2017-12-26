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
	Partial Class ShipperWorkflow
#Region "Designer generated code"

        ''' <summary> 
        ''' Required method for Designer support - do not modify 
        ''' the contents of this method with the code editor.
        ''' </summary>
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim endpoint1 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind1 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding1 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo1 As New System.Workflow.Activities.TypedOperationInfo()
            Dim activitybind2 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding2 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind3 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding3 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind4 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding4 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo2 As New System.Workflow.Activities.TypedOperationInfo()
            Dim workflowserviceattributes1 As New System.Workflow.Activities.WorkflowServiceAttributes()
            Me.DoAcceptQuoteRequest = New System.Workflow.Activities.CodeActivity()
            Me.SendShippingQuote = New System.Workflow.Activities.SendActivity()
            Me.Delay = New System.Workflow.Activities.DelayActivity()
            Me.ReceiveRequestShippingQuote = New System.Workflow.Activities.ReceiveActivity()
            ' 
            ' DoAcceptQuoteRequest
            ' 
            Me.DoAcceptQuoteRequest.Name = "DoAcceptQuoteRequest"
            AddHandler Me.DoAcceptQuoteRequest.ExecuteCode, AddressOf Me.AcceptQuoteRequest
            ' 
            ' SendShippingQuote
            ' 
            endpoint1.EndpointName = "SupplierEndpoint"
            endpoint1.Name = "SupplierEndpoint"
            Me.SendShippingQuote.ChannelToken = endpoint1
            Me.SendShippingQuote.Name = "SendShippingQuote"
            activitybind1.Name = "ShipperWorkflow"
            activitybind1.Path = "quote"
            workflowparameterbinding1.ParameterName = "quote"
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind1), System.Workflow.ComponentModel.ActivityBind))
            Me.SendShippingQuote.ParameterBindings.Add(workflowparameterbinding1)
            typedoperationinfo1.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingQuote)
            typedoperationinfo1.Name = "ShippingQuote"
            Me.SendShippingQuote.ServiceOperationInfo = typedoperationinfo1
            AddHandler Me.SendShippingQuote.BeforeSend, AddressOf Me.PrepareQuote
            ' 
            ' Delay
            ' 
            Me.Delay.Name = "Delay"
            Me.Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:04")
            ' 
            ' ReceiveRequestShippingQuote
            ' 
            Me.ReceiveRequestShippingQuote.Activities.Add(Me.DoAcceptQuoteRequest)
            Me.ReceiveRequestShippingQuote.CanCreateInstance = True
            Me.ReceiveRequestShippingQuote.Name = "ReceiveRequestShippingQuote"
            activitybind2.Name = "ShipperWorkflow"
            activitybind2.Path = "supplierAck"
            workflowparameterbinding2.ParameterName = "(ReturnValue)"
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind2), System.Workflow.ComponentModel.ActivityBind))
            activitybind3.Name = "ShipperWorkflow"
            activitybind3.Path = "order"
            workflowparameterbinding3.ParameterName = "po"
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind3), System.Workflow.ComponentModel.ActivityBind))
            activitybind4.Name = "ShipperWorkflow"
            activitybind4.Path = "supplierContext"
            workflowparameterbinding4.ParameterName = "context"
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind4), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding2)
            Me.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding3)
            Me.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding4)
            typedoperationinfo2.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingRequest)
            typedoperationinfo2.Name = "RequestShippingQuote"
            Me.ReceiveRequestShippingQuote.ServiceOperationInfo = typedoperationinfo2
            ' 
            ' ShipperWorkflow
            ' 
            Me.Activities.Add(Me.ReceiveRequestShippingQuote)
            Me.Activities.Add(Me.Delay)
            Me.Activities.Add(Me.SendShippingQuote)
            Me.Name = "ShipperWorkflow"
            Me.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1)
            Me.CanModifyActivities = False

        End Sub

#End Region

		Private Delay As DelayActivity
		Private DoAcceptQuoteRequest As CodeActivity
		Private SendShippingQuote As SendActivity
		Private ReceiveRequestShippingQuote As ReceiveActivity








	End Class
End Namespace