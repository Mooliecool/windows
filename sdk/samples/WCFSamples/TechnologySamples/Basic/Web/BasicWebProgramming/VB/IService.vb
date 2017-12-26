' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.ServiceModel
Imports System.ServiceModel.Web
Imports System.Text
Namespace Microsoft.ServiceModel.Samples.BasicWebProgramming

	<ServiceContract()> _
	Public Interface IService
		<OperationContract()> _
		<WebGet()> _
		Function EchoWithGet(ByVal s As String) As String

		<OperationContract()> _
		<WebInvoke()> _
		Function EchoWithPost(ByVal s As String) As String
	End Interface
End Namespace