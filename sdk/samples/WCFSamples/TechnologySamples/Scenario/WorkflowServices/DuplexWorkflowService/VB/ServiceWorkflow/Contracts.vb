' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Runtime.Serialization
Imports System.ServiceModel

Namespace Microsoft.WorkflowServices.Samples
	<ServiceContract([Namespace] := "http://Microsoft.WorkflowServices.Samples")> _
	Public Interface IForwardContract
		<OperationContract()> _
		Sub BeginWorkflow(ByVal returnAddress As EndpointAddress10)
		<OperationContract(IsOneWay := True)> _
		Sub BeginWorkItem(ByVal value As String)
		<OperationContract(IsOneWay := True)> _
		Sub ContinueWorkItem(ByVal value As Integer)
		<OperationContract(IsOneWay := True)> _
		Sub CompleteWorkItem(ByVal value As String)
		<OperationContract(IsOneWay := True)> _
		Sub CompleteWorkflow()
	End Interface

	<ServiceContract([Namespace] := "http://Microsoft.WorkflowServices.Samples")> _
	Public Interface IReverseContract
		<OperationContract(IsOneWay := True)> _
		Sub WorkItemComplete(ByVal item As WorkItem)
	End Interface

	<ServiceContract([Namespace] := "http://Microsoft.WorkflowServices.Samples")> _
	Public Interface IHostForwardContract
		<OperationContract()> _
		Sub BeginWork(ByVal returnUri As String)
		<OperationContract(IsOneWay := True)> _
		Sub SubmitWorkItem(ByVal itemName As String)
		<OperationContract(IsOneWay := True)> _
		Sub WorkComplete()
	End Interface

	<Serializable()> _
	<DataContract([Namespace] := "http://Microsoft.WorkflowServices.Samples")> _
	Public Class WorkItem
		<DataMember()> _
		Public FirstPart As String
		<DataMember()> _
		Public PartsList As List(Of Integer)
		<DataMember()> _
		Public LastPart As String
	End Class

End Namespace