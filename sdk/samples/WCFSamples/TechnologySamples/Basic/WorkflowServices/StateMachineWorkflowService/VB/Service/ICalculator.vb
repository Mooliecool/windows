' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.WorkflowServices.Samples
	<ServiceContract([Namespace] := "http://Microsoft.WorkflowServices.Samples")> _
	Public Interface ICalculator
		<OperationContract()> _
		Function PowerOn() As Integer
		<OperationContract()> _
		Function Add(ByVal value As Integer) As Integer
		<OperationContract()> _
		Function Subtract(ByVal value As Integer) As Integer
		<OperationContract()> _
		Function Multiply(ByVal value As Integer) As Integer
		<OperationContract()> _
		Function Divide(ByVal value As Integer) As Integer
		<OperationContract()> _
		Sub PowerOff()
	End Interface
End Namespace