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
	Public NotInheritable Partial Class ShipperWorkflow
		Inherits SequentialWorkflowActivity
		Public Sub New()
			InitializeComponent()
		End Sub

		Public order As New PurchaseOrder()
        Public supplierContext As IDictionary(Of String, String) = Nothing
		Public supplierAck As String = Nothing

		Private Sub AcceptQuoteRequest(ByVal sender As Object, ByVal e As EventArgs)
			Console.ForegroundColor = ConsoleColor.White
			Console.WriteLine("Received ShippingQuote Request")
			Me.supplierAck = "Working on quote..."
			Console.ResetColor()
			Me.SendShippingQuote.Context = Me.supplierContext
		End Sub

		Public quote As New ShippingQuote()

		Private Sub PrepareQuote(ByVal sender As Object, ByVal e As SendActivityEventArgs)
			Dim rand As New Random()
			quote.ShippingCost = rand.[Next](45, 90)
			quote.EstimatedShippingDate = DateTime.Now.AddDays(rand.[Next](2, 5))
			Console.ForegroundColor = ConsoleColor.White
			Console.WriteLine("Sending ShippingQuote ")
            Console.WriteLine("Cost: $" + quote.ShippingCost.ToString())
            Console.WriteLine("ShipDate: " + quote.EstimatedShippingDate.ToString())
			Console.ForegroundColor = ConsoleColor.Gray
            For Each item As KeyValuePair(Of String, String) In supplierContext
                Console.WriteLine(item.Key & ": " & item.Value.Substring(0, 13))
            Next
			Console.ResetColor()
		End Sub

	End Class

End Namespace