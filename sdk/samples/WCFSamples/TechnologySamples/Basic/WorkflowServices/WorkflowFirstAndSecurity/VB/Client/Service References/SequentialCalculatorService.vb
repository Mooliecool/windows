' Copyright (c) Microsoft Corporation. All rights reserved.

Namespace Microsoft.WorkflowServices.Samples.SequentialCalculatorService


    <System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
    <System.ServiceModel.ServiceContractAttribute(ConfigurationName:="Microsoft.WorkflowServices.Samples.SequentialCalculatorService.ICalculator")> _
    Public Interface ICalculator

        <System.ServiceModel.OperationContractAttribute(Action:="http://tempuri.org/ICalculator/Add", ReplyAction:="http://tempuri.org/ICalculator/AddResponse")> _
        Function Add(ByVal value As Integer) As <System.ServiceModel.MessageParameterAttribute(Name:="returnValue")> Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://tempuri.org/ICalculator/Complete", ReplyAction:="http://tempuri.org/ICalculator/CompleteResponse")> _
        Sub Complete()
	End Interface

	<System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
	Public Interface ICalculatorChannel
		Inherits Microsoft.WorkflowServices.Samples.SequentialCalculatorService.ICalculator
		Inherits System.ServiceModel.IClientChannel
	End Interface

	<System.Diagnostics.DebuggerStepThroughAttribute()> _
	<System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
	Public Partial Class CalculatorClient
		Inherits System.ServiceModel.ClientBase(Of Microsoft.WorkflowServices.Samples.SequentialCalculatorService.ICalculator)
		Implements Microsoft.WorkflowServices.Samples.SequentialCalculatorService.ICalculator

        Public Sub New()
        End Sub

        Public Sub New(ByVal endpointConfigurationName As String)
            MyBase.New(endpointConfigurationName)
        End Sub

        Public Sub New(ByVal endpointConfigurationName As String, ByVal remoteAddress As String)
            MyBase.New(endpointConfigurationName, remoteAddress)
        End Sub

        Public Sub New(ByVal endpointConfigurationName As String, ByVal remoteAddress As System.ServiceModel.EndpointAddress)
            MyBase.New(endpointConfigurationName, remoteAddress)
        End Sub

        Public Sub New(ByVal binding As System.ServiceModel.Channels.Binding, ByVal remoteAddress As System.ServiceModel.EndpointAddress)
            MyBase.New(binding, remoteAddress)
        End Sub

        Public Function Add(ByVal value As Integer) As Integer Implements ICalculator.Add
            Return MyBase.Channel.Add(value)
        End Function

        Public Sub Complete() Implements ICalculator.Complete
            MyBase.Channel.Complete()
        End Sub
	End Class
End Namespace