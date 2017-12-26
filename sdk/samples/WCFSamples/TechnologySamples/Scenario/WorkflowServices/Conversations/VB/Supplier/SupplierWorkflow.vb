' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections.Generic
Imports System.Drawing
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules
Imports System.Threading
Imports System.Xml

Namespace Microsoft.WorkflowServices.Samples
	Public NotInheritable Partial Class SupplierWorkflow
		Inherits SequentialWorkflowActivity
		Public Sub New()
			InitializeComponent()
		End Sub

		Public order As New PurchaseOrder()
		Public supplierAck As String = Nothing
        Public customerContext As IDictionary(Of String, String) = Nothing

		Private Sub AcceptOrder(ByVal sender As Object, ByVal e As EventArgs)
			Console.WriteLine("Order Received...")
			Me.supplierAck = "Order Received on " + DateTime.Now
			Me.SendOrderDetails.Context = Me.customerContext
		End Sub

        Public contextShipper2 As IDictionary(Of String, String) = Nothing
		Public quoteShipper2 As New ShippingQuote()

		Private Sub PrepareShipper2Request(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Console.ForegroundColor = ConsoleColor.Green
			Console.WriteLine("RequestShippingQuote from Shipper2")
			Console.ResetColor()
			Me.contextShipper2 = Me.ReceiveQuoteFromShipper2.Context
		End Sub

        Public contextShipper1 As IDictionary(Of String, String) = Nothing
		Public quoteShipper1 As New ShippingQuote()

		Private Sub PrepareShipper1Request(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Console.ForegroundColor = ConsoleColor.Red
			Console.WriteLine("RequestShippingQuote from Shipper1")
			Console.ResetColor()
			Me.contextShipper1 = Me.ReceiveQuoteFromShipper1.Context
		End Sub

        Public contextShipper3 As IDictionary(Of String, String) = Nothing
		Public quoteShipper3 As New ShippingQuote()

		Private Sub PrepareShipper3Request(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Console.ForegroundColor = ConsoleColor.Blue
			Console.WriteLine("RequestShippingQuote from Shipper3")
			Console.ResetColor()
			Me.contextShipper3 = Me.ReceiveQuoteFromShipper3.Context
		End Sub

		Public confirmedOrder As New PurchaseOrder()
		Public confirmedQuote As New ShippingQuote()

		Private Sub PrepareOrderConfirmation(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Console.WriteLine("Send OrderConfirmation to Customer")
			confirmedQuote = quoteShipper2
			If confirmedQuote.ShippingCost > quoteShipper1.ShippingCost Then
				confirmedQuote = quoteShipper1
			End If
			If confirmedQuote.ShippingCost > quoteShipper3.ShippingCost Then
				confirmedQuote = quoteShipper3
			End If
		End Sub

		Public ackShipper2 As String = Nothing
		Public ackShipper1 As String = Nothing
		Public ackShipper3 As String = Nothing

		Private Sub ReceiveShipper2ShippingQuote(ByVal sender As Object, ByVal e As EventArgs)
			Console.ForegroundColor = ConsoleColor.Green
			Console.WriteLine("Received Shipper2 ShippingQuote")
            Console.WriteLine("Cost: $" + quoteShipper2.ShippingCost.ToString())
            Console.WriteLine("ShipDate: " + quoteShipper2.EstimatedShippingDate.ToString())
			Console.ResetColor()
		End Sub

		Private Sub ReceiveShipper1ShippingQuote(ByVal sender As Object, ByVal e As EventArgs)
			Console.ForegroundColor = ConsoleColor.Red
			Console.WriteLine("Received Shipper1 ShippingQuote")
            Console.WriteLine("Cost: $" + quoteShipper1.ShippingCost.ToString())
            Console.WriteLine("ShipDate: " + quoteShipper1.EstimatedShippingDate.ToString())
			Console.ResetColor()
		End Sub

		Private Sub ReceiveShipper3ShippingQuote(ByVal sender As Object, ByVal e As EventArgs)
			Console.ForegroundColor = ConsoleColor.Blue
			Console.WriteLine("Received Shipper3 ShippingQuote")
            Console.WriteLine("Cost: $" + quoteShipper3.ShippingCost.ToString())
            Console.WriteLine("ShipDate: $" + quoteShipper3.EstimatedShippingDate.ToString())
			Console.ResetColor()
		End Sub





	End Class

End Namespace