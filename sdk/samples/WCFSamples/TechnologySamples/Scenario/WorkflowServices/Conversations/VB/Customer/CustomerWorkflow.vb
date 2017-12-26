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
Imports System.Xml

Namespace Microsoft.WorkflowServices.Samples
	Public NotInheritable Partial Class CustomerWorkflow
		Inherits SequentialWorkflowActivity
		Public Sub New()
			InitializeComponent()
		End Sub

		Public order As New PurchaseOrder()
		Public supplierAck As String = Nothing
        Public contextToSend As IDictionary(Of String, String) = Nothing

		Private Sub PrepareOrder(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Me.order.Amount = 1000
			Me.order.OrderId = 1234
			Me.contextToSend = Me.ReceiveOrderDetails.Context
		End Sub

		Public orderDetails As New PurchaseOrder()
		Public shippingQuote As New ShippingQuote()
		Public customerAck As String = Nothing

		Private Sub ReviewOrder(ByVal sender As Object, ByVal e As EventArgs)
			Me.customerAck = "Order Details Received"
			Console.ForegroundColor = ConsoleColor.Green
			Console.WriteLine("Order Accepted.")
            Console.WriteLine("Cost: $" + Me.shippingQuote.ShippingCost.ToString())
            Console.WriteLine("ShipDate: " + Me.shippingQuote.EstimatedShippingDate.ToString())
			Console.ResetColor()
		End Sub

		Private Sub DisplayStatus(ByVal sender As Object, ByVal e As EventArgs)
			Console.WriteLine(Me.supplierAck)
		End Sub
	End Class

End Namespace