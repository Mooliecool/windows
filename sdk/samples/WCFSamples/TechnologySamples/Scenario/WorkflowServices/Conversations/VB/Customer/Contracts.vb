' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Text
Imports System.Xml

Namespace Microsoft.WorkflowServices.Samples
	<ServiceContract()> _
	Public Interface IOrder
        <OperationContract(IsInitiating:=True)> _
        Function SubmitOrder(ByVal po As PurchaseOrder, ByVal context As IDictionary(Of String, String)) As String
	End Interface

	<ServiceContract()> _
	Public Interface IOrderDetails
		<OperationContract()> _
		Function OrderDetails(ByVal po As PurchaseOrder, ByVal quote As ShippingQuote) As String
	End Interface

	<ServiceContract()> _
	Public Interface IShippingRequest
        <OperationContract(IsInitiating:=True)> _
        Function RequestShippingQuote(ByVal po As PurchaseOrder, ByVal context As IDictionary(Of String, String)) As String
	End Interface

	<ServiceContract()> _
	Public Interface IShippingQuote
		<OperationContract(IsOneWay := True)> _
		Sub ShippingQuote(ByVal quote As ShippingQuote)
	End Interface

	<DataContract()> _
	Public Class PurchaseOrder
		<DataMember()> _
		Public OrderId As Integer
		<DataMember()> _
		Public Amount As Integer
		<DataMember()> _
		Public CustomerName As String
	End Class

	<DataContract()> _
	Public Class ShippingQuote
		<DataMember()> _
		Public ShippingCost As Integer
		<DataMember()> _
		Public EstimatedShippingDate As DateTime
	End Class
End Namespace