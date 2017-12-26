//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Reflection;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.WorkflowServices.Samples
{
	partial class SupplierWorkflow
	{
		#region Designer generated code
		
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
		private void InitializeComponent()
		{
            this.CanModifyActivities = true;
            System.Workflow.Activities.ContextToken contexttoken1 = new System.Workflow.Activities.ContextToken();
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo1 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ChannelToken channeltoken1 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind3 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding3 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo2 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ContextToken contexttoken2 = new System.Workflow.Activities.ContextToken();
            System.Workflow.ComponentModel.ActivityBind activitybind5 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding5 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo3 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ChannelToken channeltoken2 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind6 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding6 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind7 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding7 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind8 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding8 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo4 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ContextToken contexttoken3 = new System.Workflow.Activities.ContextToken();
            System.Workflow.ComponentModel.ActivityBind activitybind9 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding9 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo5 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ChannelToken channeltoken3 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind10 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding10 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind11 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding11 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind12 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding12 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo6 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ChannelToken channeltoken4 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind13 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding13 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind14 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding14 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo7 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.ComponentModel.ActivityBind activitybind15 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding15 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind16 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding16 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind17 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding17 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo8 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.WorkflowServiceAttributes workflowserviceattributes1 = new System.Workflow.Activities.WorkflowServiceAttributes();
            this.Shipper3ShippingQuote = new System.Workflow.Activities.CodeActivity();
            this.Shipper2ShippingQuote = new System.Workflow.Activities.CodeActivity();
            this.Shipper1ShippingQuote = new System.Workflow.Activities.CodeActivity();
            this.ReceiveQuoteFromShipper3 = new System.Workflow.Activities.ReceiveActivity();
            this.RequestQuoteFromShipper3 = new System.Workflow.Activities.SendActivity();
            this.ReceiveQuoteFromShipper2 = new System.Workflow.Activities.ReceiveActivity();
            this.RequestQuoteFromShipper2 = new System.Workflow.Activities.SendActivity();
            this.ReceiveQuoteFromShipper1 = new System.Workflow.Activities.ReceiveActivity();
            this.RequestQuoteFromShipper1 = new System.Workflow.Activities.SendActivity();
            this.Shipper3Quote = new System.Workflow.Activities.SequenceActivity();
            this.Shipper2Quote = new System.Workflow.Activities.SequenceActivity();
            this.Shipper1Quote = new System.Workflow.Activities.SequenceActivity();
            this.DoAcceptOrder = new System.Workflow.Activities.CodeActivity();
            this.SendOrderDetails = new System.Workflow.Activities.SendActivity();
            this.GetShippingQuotes = new System.Workflow.Activities.ParallelActivity();
            this.ReceiveSubmitOrder = new System.Workflow.Activities.ReceiveActivity();
            // 
            // Shipper3ShippingQuote
            // 
            this.Shipper3ShippingQuote.Name = "Shipper3ShippingQuote";
            this.Shipper3ShippingQuote.ExecuteCode += new System.EventHandler(this.ReceiveShipper3ShippingQuote);
            // 
            // Shipper2ShippingQuote
            // 
            this.Shipper2ShippingQuote.Name = "Shipper2ShippingQuote";
            this.Shipper2ShippingQuote.ExecuteCode += new System.EventHandler(this.ReceiveShipper2ShippingQuote);
            // 
            // Shipper1ShippingQuote
            // 
            this.Shipper1ShippingQuote.Name = "Shipper1ShippingQuote";
            this.Shipper1ShippingQuote.ExecuteCode += new System.EventHandler(this.ReceiveShipper1ShippingQuote);
            // 
            // ReceiveQuoteFromShipper3
            // 
            this.ReceiveQuoteFromShipper3.Activities.Add(this.Shipper3ShippingQuote);
            contexttoken1.Name = "Shipper3Context";
            this.ReceiveQuoteFromShipper3.ContextToken = contexttoken1;
            this.ReceiveQuoteFromShipper3.Name = "ReceiveQuoteFromShipper3";
            activitybind1.Name = "SupplierWorkflow";
            activitybind1.Path = "quoteShipper3";
            workflowparameterbinding1.ParameterName = "quote";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.ReceiveQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding1);
            typedoperationinfo1.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingQuote);
            typedoperationinfo1.Name = "ShippingQuote";
            this.ReceiveQuoteFromShipper3.ServiceOperationInfo = typedoperationinfo1;
            // 
            // RequestQuoteFromShipper3
            // 
            channeltoken1.EndpointName = "Shipper3Endpoint";
            channeltoken1.Name = "Shipper3Endpoint";
            channeltoken1.OwnerActivityName = "GetShippingQuotes";
            this.RequestQuoteFromShipper3.ChannelToken = channeltoken1;
            this.RequestQuoteFromShipper3.Name = "RequestQuoteFromShipper3";
            activitybind2.Name = "SupplierWorkflow";
            activitybind2.Path = "order";
            workflowparameterbinding2.ParameterName = "po";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            activitybind3.Name = "SupplierWorkflow";
            activitybind3.Path = "contextShipper3";
            workflowparameterbinding3.ParameterName = "context";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            activitybind4.Name = "SupplierWorkflow";
            activitybind4.Path = "ackShipper3";
            workflowparameterbinding4.ParameterName = "(ReturnValue)";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            this.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding2);
            this.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding3);
            this.RequestQuoteFromShipper3.ParameterBindings.Add(workflowparameterbinding4);
            typedoperationinfo2.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingRequest);
            typedoperationinfo2.Name = "RequestShippingQuote";
            this.RequestQuoteFromShipper3.ServiceOperationInfo = typedoperationinfo2;
            this.RequestQuoteFromShipper3.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareShipper3Request);
            // 
            // ReceiveQuoteFromShipper2
            // 
            this.ReceiveQuoteFromShipper2.Activities.Add(this.Shipper2ShippingQuote);
            contexttoken2.Name = "Shipper2Context";
            contexttoken2.OwnerActivityName = "GetShippingQuotes";
            this.ReceiveQuoteFromShipper2.ContextToken = contexttoken2;
            this.ReceiveQuoteFromShipper2.Name = "ReceiveQuoteFromShipper2";
            activitybind5.Name = "SupplierWorkflow";
            activitybind5.Path = "quoteShipper2";
            workflowparameterbinding5.ParameterName = "quote";
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind5)));
            this.ReceiveQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding5);
            typedoperationinfo3.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingQuote);
            typedoperationinfo3.Name = "ShippingQuote";
            this.ReceiveQuoteFromShipper2.ServiceOperationInfo = typedoperationinfo3;
            // 
            // RequestQuoteFromShipper2
            // 
            channeltoken2.EndpointName = "Shipper2Endpoint";
            channeltoken2.Name = "Shipper2Endpoint";
            channeltoken2.OwnerActivityName = "GetShippingQuotes";
            this.RequestQuoteFromShipper2.ChannelToken = channeltoken2;
            this.RequestQuoteFromShipper2.Name = "RequestQuoteFromShipper2";
            activitybind6.Name = "SupplierWorkflow";
            activitybind6.Path = "ackShipper2";
            workflowparameterbinding6.ParameterName = "(ReturnValue)";
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind6)));
            activitybind7.Name = "SupplierWorkflow";
            activitybind7.Path = "order";
            workflowparameterbinding7.ParameterName = "po";
            workflowparameterbinding7.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind7)));
            activitybind8.Name = "SupplierWorkflow";
            activitybind8.Path = "contextShipper2";
            workflowparameterbinding8.ParameterName = "context";
            workflowparameterbinding8.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind8)));
            this.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding6);
            this.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding7);
            this.RequestQuoteFromShipper2.ParameterBindings.Add(workflowparameterbinding8);
            typedoperationinfo4.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingRequest);
            typedoperationinfo4.Name = "RequestShippingQuote";
            this.RequestQuoteFromShipper2.ServiceOperationInfo = typedoperationinfo4;
            this.RequestQuoteFromShipper2.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareShipper2Request);
            // 
            // ReceiveQuoteFromShipper1
            // 
            this.ReceiveQuoteFromShipper1.Activities.Add(this.Shipper1ShippingQuote);
            contexttoken3.Name = "Shipper1Context";
            contexttoken3.OwnerActivityName = "GetShippingQuotes";
            this.ReceiveQuoteFromShipper1.ContextToken = contexttoken3;
            this.ReceiveQuoteFromShipper1.Name = "ReceiveQuoteFromShipper1";
            activitybind9.Name = "SupplierWorkflow";
            activitybind9.Path = "quoteShipper1";
            workflowparameterbinding9.ParameterName = "quote";
            workflowparameterbinding9.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind9)));
            this.ReceiveQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding9);
            typedoperationinfo5.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingQuote);
            typedoperationinfo5.Name = "ShippingQuote";
            this.ReceiveQuoteFromShipper1.ServiceOperationInfo = typedoperationinfo5;
            // 
            // RequestQuoteFromShipper1
            // 
            channeltoken3.EndpointName = "Shipper1Endpoint";
            channeltoken3.Name = "Shipper1Endpoint";
            channeltoken3.OwnerActivityName = "GetShippingQuotes";
            this.RequestQuoteFromShipper1.ChannelToken = channeltoken3;
            this.RequestQuoteFromShipper1.Name = "RequestQuoteFromShipper1";
            activitybind10.Name = "SupplierWorkflow";
            activitybind10.Path = "order";
            workflowparameterbinding10.ParameterName = "po";
            workflowparameterbinding10.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind10)));
            activitybind11.Name = "SupplierWorkflow";
            activitybind11.Path = "contextShipper1";
            workflowparameterbinding11.ParameterName = "context";
            workflowparameterbinding11.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind11)));
            activitybind12.Name = "SupplierWorkflow";
            activitybind12.Path = "ackShipper1";
            workflowparameterbinding12.ParameterName = "(ReturnValue)";
            workflowparameterbinding12.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind12)));
            this.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding10);
            this.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding11);
            this.RequestQuoteFromShipper1.ParameterBindings.Add(workflowparameterbinding12);
            typedoperationinfo6.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingRequest);
            typedoperationinfo6.Name = "RequestShippingQuote";
            this.RequestQuoteFromShipper1.ServiceOperationInfo = typedoperationinfo6;
            this.RequestQuoteFromShipper1.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareShipper1Request);
            // 
            // Shipper3Quote
            // 
            this.Shipper3Quote.Activities.Add(this.RequestQuoteFromShipper3);
            this.Shipper3Quote.Activities.Add(this.ReceiveQuoteFromShipper3);
            this.Shipper3Quote.Name = "Shipper3Quote";
            // 
            // Shipper2Quote
            // 
            this.Shipper2Quote.Activities.Add(this.RequestQuoteFromShipper2);
            this.Shipper2Quote.Activities.Add(this.ReceiveQuoteFromShipper2);
            this.Shipper2Quote.Name = "Shipper2Quote";
            // 
            // Shipper1Quote
            // 
            this.Shipper1Quote.Activities.Add(this.RequestQuoteFromShipper1);
            this.Shipper1Quote.Activities.Add(this.ReceiveQuoteFromShipper1);
            this.Shipper1Quote.Name = "Shipper1Quote";
            // 
            // DoAcceptOrder
            // 
            this.DoAcceptOrder.Name = "DoAcceptOrder";
            this.DoAcceptOrder.ExecuteCode += new System.EventHandler(this.AcceptOrder);
            // 
            // SendOrderDetails
            // 
            channeltoken4.EndpointName = "CustomerEndpoint";
            channeltoken4.Name = "CustomerEndpoint";
            channeltoken4.OwnerActivityName = "SupplierWorkflow";
            this.SendOrderDetails.ChannelToken = channeltoken4;
            this.SendOrderDetails.Name = "SendOrderDetails";
            activitybind13.Name = "SupplierWorkflow";
            activitybind13.Path = "confirmedQuote";
            workflowparameterbinding13.ParameterName = "quote";
            workflowparameterbinding13.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind13)));
            activitybind14.Name = "SupplierWorkflow";
            activitybind14.Path = "confirmedOrder";
            workflowparameterbinding14.ParameterName = "po";
            workflowparameterbinding14.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind14)));
            this.SendOrderDetails.ParameterBindings.Add(workflowparameterbinding13);
            this.SendOrderDetails.ParameterBindings.Add(workflowparameterbinding14);
            typedoperationinfo7.ContractType = typeof(Microsoft.WorkflowServices.Samples.IOrderDetails);
            typedoperationinfo7.Name = "OrderDetails";
            this.SendOrderDetails.ServiceOperationInfo = typedoperationinfo7;
            this.SendOrderDetails.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareOrderConfirmation);
            // 
            // GetShippingQuotes
            // 
            this.GetShippingQuotes.Activities.Add(this.Shipper1Quote);
            this.GetShippingQuotes.Activities.Add(this.Shipper2Quote);
            this.GetShippingQuotes.Activities.Add(this.Shipper3Quote);
            this.GetShippingQuotes.Name = "GetShippingQuotes";
            // 
            // ReceiveSubmitOrder
            // 
            this.ReceiveSubmitOrder.Activities.Add(this.DoAcceptOrder);
            this.ReceiveSubmitOrder.CanCreateInstance = true;
            this.ReceiveSubmitOrder.Name = "ReceiveSubmitOrder";
            activitybind15.Name = "SupplierWorkflow";
            activitybind15.Path = "order";
            workflowparameterbinding15.ParameterName = "po";
            workflowparameterbinding15.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind15)));
            activitybind16.Name = "SupplierWorkflow";
            activitybind16.Path = "customerContext";
            workflowparameterbinding16.ParameterName = "context";
            workflowparameterbinding16.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind16)));
            activitybind17.Name = "SupplierWorkflow";
            activitybind17.Path = "supplierAck";
            workflowparameterbinding17.ParameterName = "(ReturnValue)";
            workflowparameterbinding17.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind17)));
            this.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding15);
            this.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding16);
            this.ReceiveSubmitOrder.ParameterBindings.Add(workflowparameterbinding17);
            typedoperationinfo8.ContractType = typeof(Microsoft.WorkflowServices.Samples.IOrder);
            typedoperationinfo8.Name = "SubmitOrder";
            this.ReceiveSubmitOrder.ServiceOperationInfo = typedoperationinfo8;
            // 
            // SupplierWorkflow
            // 
            this.Activities.Add(this.ReceiveSubmitOrder);
            this.Activities.Add(this.GetShippingQuotes);
            this.Activities.Add(this.SendOrderDetails);
            this.Name = "SupplierWorkflow";
            this.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1);
            this.CanModifyActivities = false;

		}

		#endregion

        private CodeActivity Shipper3ShippingQuote;
        private CodeActivity Shipper2ShippingQuote;
        private CodeActivity Shipper1ShippingQuote;
        private SendActivity SendOrderDetails;
        private ReceiveActivity ReceiveQuoteFromShipper3;
        private SendActivity RequestQuoteFromShipper3;
        private ReceiveActivity ReceiveQuoteFromShipper1;
        private SendActivity RequestQuoteFromShipper1;
        private ReceiveActivity ReceiveQuoteFromShipper2;
        private SendActivity RequestQuoteFromShipper2;
        private SequenceActivity Shipper3Quote;
        private SequenceActivity Shipper1Quote;
        private SequenceActivity Shipper2Quote;
        private ParallelActivity GetShippingQuotes;
        private CodeActivity DoAcceptOrder;
        private ReceiveActivity ReceiveSubmitOrder;















































    }
}
