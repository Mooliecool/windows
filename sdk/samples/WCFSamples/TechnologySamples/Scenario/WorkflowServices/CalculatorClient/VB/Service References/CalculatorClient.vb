' Copyright (c) Microsoft Corporation. All rights reserved.

Namespace Microsoft.WorkflowServices.Samples


    <System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
    <System.ServiceModel.ServiceContractAttribute([Namespace]:="http://Microsoft.WorkflowServices.Samples", ConfigurationName:="Microsoft.WorkflowServices.Samples.ICalculator")> _
    Public Interface ICalculator

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/PowerOn", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/PowerOnResponse")> _
        Function PowerOn() As Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/Add", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/AddResponse")> _
        Function Add(ByVal value As Integer) As Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/Subtract", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/SubtractResponse")> _
        Function Subtract(ByVal value As Integer) As Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/Multiply", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/MultiplyResponse")> _
        Function Multiply(ByVal value As Integer) As Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/Divide", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/DivideResponse")> _
        Function Divide(ByVal value As Integer) As Integer

        <System.ServiceModel.OperationContractAttribute(Action:="http://Microsoft.WorkflowServices.Samples/ICalculator/PowerOff", ReplyAction:="http://Microsoft.WorkflowServices.Samples/ICalculator/PowerOffResponse")> _
        Sub PowerOff()
    End Interface

    <System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
    Public Interface ICalculatorChannel
        Inherits Microsoft.WorkflowServices.Samples.ICalculator
        Inherits System.ServiceModel.IClientChannel
    End Interface

    <System.Diagnostics.DebuggerStepThroughAttribute()> _
    <System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")> _
    Partial Public Class CalculatorClient
        Inherits System.ServiceModel.ClientBase(Of Microsoft.WorkflowServices.Samples.ICalculator)
        Implements Microsoft.WorkflowServices.Samples.ICalculator

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

        Public Function PowerOn() As Integer Implements ICalculator.PowerOn
            Return MyBase.Channel.PowerOn()
        End Function

        Public Function Add(ByVal value As Integer) As Integer Implements ICalculator.Add
            Return MyBase.Channel.Add(value)
        End Function

        Public Function Subtract(ByVal value As Integer) As Integer Implements ICalculator.Subtract
            Return MyBase.Channel.Subtract(value)
        End Function

        Public Function Multiply(ByVal value As Integer) As Integer Implements ICalculator.Multiply
            Return MyBase.Channel.Multiply(value)
        End Function

        Public Function Divide(ByVal value As Integer) As Integer Implements ICalculator.Divide
            Return MyBase.Channel.Divide(value)
        End Function

        Public Sub PowerOff() Implements ICalculator.PowerOff
            MyBase.Channel.PowerOff()
        End Sub
    End Class
End Namespace