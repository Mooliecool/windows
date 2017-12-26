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
	Partial Class SupplierWorkflow
#Region "Designer generated code"

        ''' <summary> 
        ''' Required method for Designer support - do not modify 
        ''' the contents of this method with the code editor.
        ''' </summary>
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim contexttoken1 As New System.Workflow.Activities.ContextToken()
            Dim activitybind1 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding1 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo1 As New System.Workflow.Activities.TypedOperationInfo()
            Dim channeltoken1 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind2 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding2 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind3 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding3 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind4 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding4 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo2 As New System.Workflow.Activities.TypedOperationInfo()
            Dim contexttoken2 As New System.Workflow.Activities.ContextToken()
            Dim activitybind5 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding5 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo3 As New System.Workflow.Activities.TypedOperationInfo()
            Dim channeltoken2 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind6 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding6 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind7 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding7 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind8 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding8 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo4 As New System.Workflow.Activities.TypedOperationInfo()
            Dim contexttoken3 As New System.Workflow.Activities.ContextToken()
            Dim activitybind9 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding9 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo5 As New System.Workflow.Activities.TypedOperationInfo()
            Dim channeltoken3 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind10 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding10 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind11 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding11 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind12 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding12 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo6 As New System.Workflow.Activities.TypedOperationInfo()
            Dim channeltoken4 As New System.Workflow.Activities.ChannelToken()
            Dim activitybind13 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding13 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind14 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding14 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo7 As New System.Workflow.Activities.TypedOperationInfo()
            Dim activitybind15 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding15 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind16 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding16 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim activitybind17 As New System.Workflow.ComponentModel.ActivityBind()
            Dim workflowparameterbinding17 As New System.Workflow.ComponentModel.WorkflowParameterBinding()
            Dim typedoperationinfo8 As New System.Workflow.Activities.TypedOperationInfo()
            Dim workflowserviceattributes1 As New System.Workflow.Activities.WorkflowServiceAttributes()
            Me.Shipper3ShippingQuote = New System.Workflow.Activities.CodeActivity()
            Me.Shipper2ShippingQuote = New System.Workflow.Activities.CodeActivity()
            Me.Shipper1ShippingQuote = New System.Workflow.Activities.CodeActivity()
            Me.ReceiveQuoteFromShipper3 = New System.Workflow.Activities.ReceiveActivity()
            Me.RequestQuoteFromShipper3 = New System.Workflow.Activities.SendActivity()
            Me.ReceiveQuoteFromShipper2 = New System.Workflow.Activities.ReceiveActivity()
            Me.RequestQuoteFromShipper2 = New System.Workflow.Activities.SendActivity()
            Me.ReceiveQuoteFromShipper1 = New System.Workflow.Activities.ReceiveActivity()
            Me.RequestQuoteFromShipper1 = New System.Workflow.Activities.SendActivity()
            Me.Shipper3Quote = New System.Workflow.Activities.SequenceActivity()
            Me.Shipper2Quote = New System.Workflow.Activities.SequenceActivity()
            Me.Shipper1Quote = New System.Workflow.Activities.SequenceActivity()
            Me.DoAcceptOrder = New System.Workflow.Activities.CodeActivity()
            Me.SendOrderDetails = New System.Workflow.Activities.SendActivity()
            Me.GetShippingQuotes = New System.Workflow.Activities.ParallelActivity()
            Me.ReceiveSubmitOrder = New System.Workflow.Activities.ReceiveActivity()
            ' 
            ' Shipper3ShippingQuote
            ' 
            Me.Shipper3ShippingQuote.Name = "Shipper3ShippingQuote"
            AddHandler Me.Shipper3ShippingQuote.ExecuteCode, AddressOf Me.ReceiveShipper3ShippingQuote
            ' 
            ' Shipper2ShippingQuote
            ' 
            Me.Shipper2ShippingQuote.Name = "Shipper2ShippingQuote"
            AddHandler Me.Shipper2ShippingQuote.ExecuteCode, AddressOf Me.ReceiveShipper2ShippingQuote
            ' 
            ' Shipper1ShippingQuote
            ' 
            Me.Shipper1ShippingQuote.Name = "Shipper1ShippingQuote"
            AddHandler Me.Shipper1ShippingQuote.ExecuteCode, AddressOf Me.ReceiveShipper1ShippingQuote
            ' 
            ' ReceiveQuoteFromShipper3
            ' 
            Me.ReceiveQuoteFromShipper3.Activities.Add(Me.Shipper3ShippingQuote)
            contexttoken1.Name = "Shipper3Context"
            Me.ReceiveQuoteFromShipper3.ContextToken = contexttoken1
            Me.ReceiveQuoteFromShipper3.Name = "ReceiveQuoteFromShipper3"
            activitybind1.Name = "SupplierWorkflow"
            activitybind1.Path = "quoteShipper3"
            workflowparameterbinding1.ParameterName = "quote"
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind1), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding1)
            typedoperationinfo1.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingQuote)
            typedoperationinfo1.Name = "ShippingQuote"
            Me.ReceiveQuoteFromShipper3.ServiceOperationInfo = typedoperationinfo1
            ' 
            ' RequestQuoteFromShipper3
            ' 
            channeltoken1.EndpointName = "Shipper3Endpoint"
            channeltoken1.Name = "Shipper3Endpoint"
            channeltoken1.OwnerActivityName = "GetShippingQuotes"
            Me.RequestQuoteFromShipper3.ChannelToken = channeltoken1
            Me.RequestQuoteFromShipper3.Name = "RequestQuoteFromShipper3"
            activitybind2.Name = "SupplierWorkflow"
            activitybind2.Path = "order"
            workflowparameterbinding2.ParameterName = "po"
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind2), System.Workflow.ComponentModel.ActivityBind))
            activitybind3.Name = "SupplierWorkflow"
            activitybind3.Path = "contextShipper3"
            workflowparameterbinding3.ParameterName = "context"
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind3), System.Workflow.ComponentModel.ActivityBind))
            activitybind4.Name = "SupplierWorkflow"
            activitybind4.Path = "ackShipper3"
            workflowparameterbinding4.ParameterName = "(ReturnValue)"
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind4), System.Workflow.ComponentModel.ActivityBind))
            Me.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding2)
            Me.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding3)
            Me.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding4)
            typedoperationinfo2.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingRequest)
            typedoperationinfo2.Name = "RequestShippingQuote"
            Me.RequestQuoteFromShipper3.ServiceOperationInfo = typedoperationinfo2
            AddHandler Me.RequestQuoteFromShipper3.BeforeSend, AddressOf Me.PrepareShipper3Request
            ' 
            ' ReceiveQuoteFromShipper2
            ' 
            Me.ReceiveQuoteFromShipper2.Activities.Add(Me.Shipper2ShippingQuote)
            contexttoken2.Name = "Shipper2Context"
            contexttoken2.OwnerActivityName = "GetShippingQuotes"
            Me.ReceiveQuoteFromShipper2.ContextToken = contexttoken2
            Me.ReceiveQuoteFromShipper2.Name = "ReceiveQuoteFromShipper2"
            activitybind5.Name = "SupplierWorkflow"
            activitybind5.Path = "quoteShipper2"
            workflowparameterbinding5.ParameterName = "quote"
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind5), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding5)
            typedoperationinfo3.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingQuote)
            typedoperationinfo3.Name = "ShippingQuote"
            Me.ReceiveQuoteFromShipper2.ServiceOperationInfo = typedoperationinfo3
            ' 
            ' RequestQuoteFromShipper2
            ' 
            channeltoken2.EndpointName = "Shipper2Endpoint"
            channeltoken2.Name = "Shipper2Endpoint"
            channeltoken2.OwnerActivityName = "GetShippingQuotes"
            Me.RequestQuoteFromShipper2.ChannelToken = channeltoken2
            Me.RequestQuoteFromShipper2.Name = "RequestQuoteFromShipper2"
            activitybind6.Name = "SupplierWorkflow"
            activitybind6.Path = "ackShipper2"
            workflowparameterbinding6.ParameterName = "(ReturnValue)"
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind6), System.Workflow.ComponentModel.ActivityBind))
            activitybind7.Name = "SupplierWorkflow"
            activitybind7.Path = "order"
            workflowparameterbinding7.ParameterName = "po"
            workflowparameterbinding7.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind7), System.Workflow.ComponentModel.ActivityBind))
            activitybind8.Name = "SupplierWorkflow"
            activitybind8.Path = "contextShipper2"
            workflowparameterbinding8.ParameterName = "context"
            workflowparameterbinding8.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind8), System.Workflow.ComponentModel.ActivityBind))
            Me.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding6)
            Me.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding7)
            Me.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding8)
            typedoperationinfo4.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingRequest)
            typedoperationinfo4.Name = "RequestShippingQuote"
            Me.RequestQuoteFromShipper2.ServiceOperationInfo = typedoperationinfo4
            AddHandler Me.RequestQuoteFromShipper2.BeforeSend, AddressOf Me.PrepareShipper2Request
            ' 
            ' ReceiveQuoteFromShipper1
            ' 
            Me.ReceiveQuoteFromShipper1.Activities.Add(Me.Shipper1ShippingQuote)
            contexttoken3.Name = "Shipper1Context"
            contexttoken3.OwnerActivityName = "GetShippingQuotes"
            Me.ReceiveQuoteFromShipper1.ContextToken = contexttoken3
            Me.ReceiveQuoteFromShipper1.Name = "ReceiveQuoteFromShipper1"
            activitybind9.Name = "SupplierWorkflow"
            activitybind9.Path = "quoteShipper1"
            workflowparameterbinding9.ParameterName = "quote"
            workflowparameterbinding9.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind9), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding9)
            typedoperationinfo5.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingQuote)
            typedoperationinfo5.Name = "ShippingQuote"
            Me.ReceiveQuoteFromShipper1.ServiceOperationInfo = typedoperationinfo5
            ' 
            ' RequestQuoteFromShipper1
            ' 
            channeltoken3.EndpointName = "Shipper1Endpoint"
            channeltoken3.Name = "Shipper1Endpoint"
            channeltoken3.OwnerActivityName = "GetShippingQuotes"
            Me.RequestQuoteFromShipper1.ChannelToken = channeltoken3
            Me.RequestQuoteFromShipper1.Name = "RequestQuoteFromShipper1"
            activitybind10.Name = "SupplierWorkflow"
            activitybind10.Path = "order"
            workflowparameterbinding10.ParameterName = "po"
            workflowparameterbinding10.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind10), System.Workflow.ComponentModel.ActivityBind))
            activitybind11.Name = "SupplierWorkflow"
            activitybind11.Path = "contextShipper1"
            workflowparameterbinding11.ParameterName = "context"
            workflowparameterbinding11.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind11), System.Workflow.ComponentModel.ActivityBind))
            activitybind12.Name = "SupplierWorkflow"
            activitybind12.Path = "ackShipper1"
            workflowparameterbinding12.ParameterName = "(ReturnValue)"
            workflowparameterbinding12.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind12), System.Workflow.ComponentModel.ActivityBind))
            Me.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding10)
            Me.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding11)
            Me.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding12)
            typedoperationinfo6.ContractType = GetType(Microsoft.WorkflowServices.Samples.IShippingRequest)
            typedoperationinfo6.Name = "RequestShippingQuote"
            Me.RequestQuoteFromShipper1.ServiceOperationInfo = typedoperationinfo6
            AddHandler Me.RequestQuoteFromShipper1.BeforeSend, AddressOf Me.PrepareShipper1Request
            ' 
            ' Shipper3Quote
            ' 
            Me.Shipper3Quote.Activities.Add(Me.RequestQuoteFromShipper3)
            Me.Shipper3Quote.Activities.Add(Me.ReceiveQuoteFromShipper3)
            Me.Shipper3Quote.Name = "Shipper3Quote"
            ' 
            ' Shipper2Quote
            ' 
            Me.Shipper2Quote.Activities.Add(Me.RequestQuoteFromShipper2)
            Me.Shipper2Quote.Activities.Add(Me.ReceiveQuoteFromShipper2)
            Me.Shipper2Quote.Name = "Shipper2Quote"
            ' 
            ' Shipper1Quote
            ' 
            Me.Shipper1Quote.Activities.Add(Me.RequestQuoteFromShipper1)
            Me.Shipper1Quote.Activities.Add(Me.ReceiveQuoteFromShipper1)
            Me.Shipper1Quote.Name = "Shipper1Quote"
            ' 
            ' DoAcceptOrder
            ' 
            Me.DoAcceptOrder.Name = "DoAcceptOrder"
            AddHandler Me.DoAcceptOrder.ExecuteCode, AddressOf Me.AcceptOrder
            ' 
            ' SendOrderDetails
            ' 
            channeltoken4.EndpointName = "CustomerEndpoint"
            channeltoken4.Name = "CustomerEndpoint"
            channeltoken4.OwnerActivityName = "SupplierWorkflow"
            Me.SendOrderDetails.ChannelToken = channeltoken4
            Me.SendOrderDetails.Name = "SendOrderDetails"
            activitybind13.Name = "SupplierWorkflow"
            activitybind13.Path = "confirmedQuote"
            workflowparameterbinding13.ParameterName = "quote"
            workflowparameterbinding13.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind13), System.Workflow.ComponentModel.ActivityBind))
            activitybind14.Name = "SupplierWorkflow"
            activitybind14.Path = "confirmedOrder"
            workflowparameterbinding14.ParameterName = "po"
            workflowparameterbinding14.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind14), System.Workflow.ComponentModel.ActivityBind))
            Me.SendOrderDetails.ParameterBindings.Add(workflowparameterbinding13)
            Me.SendOrderDetails.ParameterBindings.Add(workflowparameterbinding14)
            typedoperationinfo7.ContractType = GetType(Microsoft.WorkflowServices.Samples.IOrderDetails)
            typedoperationinfo7.Name = "OrderDetails"
            Me.SendOrderDetails.ServiceOperationInfo = typedoperationinfo7
            AddHandler Me.SendOrderDetails.BeforeSend, AddressOf Me.PrepareOrderConfirmation
            ' 
            ' GetShippingQuotes
            ' 
            Me.GetShippingQuotes.Activities.Add(Me.Shipper1Quote)
            Me.GetShippingQuotes.Activities.Add(Me.Shipper2Quote)
            Me.GetShippingQuotes.Activities.Add(Me.Shipper3Quote)
            Me.GetShippingQuotes.Name = "GetShippingQuotes"
            ' 
            ' ReceiveSubmitOrder
            ' 
            Me.ReceiveSubmitOrder.Activities.Add(Me.DoAcceptOrder)
            Me.ReceiveSubmitOrder.CanCreateInstance = True
            Me.ReceiveSubmitOrder.Name = "ReceiveSubmitOrder"
            activitybind15.Name = "SupplierWorkflow"
            activitybind15.Path = "order"
            workflowparameterbinding15.ParameterName = "po"
            workflowparameterbinding15.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind15), System.Workflow.ComponentModel.ActivityBind))
            activitybind16.Name = "SupplierWorkflow"
            activitybind16.Path = "customerContext"
            workflowparameterbinding16.ParameterName = "context"
            workflowparameterbinding16.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind16), System.Workflow.ComponentModel.ActivityBind))
            activitybind17.Name = "SupplierWorkflow"
            activitybind17.Path = "supplierAck"
            workflowparameterbinding17.ParameterName = "(ReturnValue)"
            workflowparameterbinding17.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, DirectCast((activitybind17), System.Workflow.ComponentModel.ActivityBind))
            Me.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding15)
            Me.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding16)
            Me.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding17)
            typedoperationinfo8.ContractType = GetType(Microsoft.WorkflowServices.Samples.IOrder)
            typedoperationinfo8.Name = "SubmitOrder"
            Me.ReceiveSubmitOrder.ServiceOperationInfo = typedoperationinfo8
            ' 
            ' SupplierWorkflow
            ' 
            Me.Activities.Add(Me.ReceiveSubmitOrder)
            Me.Activities.Add(Me.GetShippingQuotes)
            Me.Activities.Add(Me.SendOrderDetails)
            Me.Name = "SupplierWorkflow"
            Me.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1)
            Me.CanModifyActivities = False

        End Sub

#End Region

        Private Shipper3ShippingQuote As CodeActivity
        Private Shipper2ShippingQuote As CodeActivity
        Private Shipper1ShippingQuote As CodeActivity
        Private SendOrderDetails As SendActivity
        Private ReceiveQuoteFromShipper3 As ReceiveActivity
        Private RequestQuoteFromShipper3 As SendActivity
        Private ReceiveQuoteFromShipper1 As ReceiveActivity
        Private RequestQuoteFromShipper1 As SendActivity
        Private ReceiveQuoteFromShipper2 As ReceiveActivity
        Private RequestQuoteFromShipper2 As SendActivity
        Private Shipper3Quote As SequenceActivity
        Private Shipper1Quote As SequenceActivity
        Private Shipper2Quote As SequenceActivity
        Private GetShippingQuotes As ParallelActivity
        Private DoAcceptOrder As CodeActivity
        Private ReceiveSubmitOrder As ReceiveActivity















































	End Class
End Namespace