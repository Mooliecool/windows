' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel.Description
Imports System.ServiceModel

Namespace Microsoft.WorkflowServices.Samples
	<Serializable()> _
	<DurableService()> _
	Public Class DurableCalculator
		Implements ICalculator
		Private currentValue As Integer = Nothing

        <DurableOperation(CanCreateInstance:=True)> _
        Public Function PowerOn() As Integer Implements ICalculator.PowerOn
            Return currentValue
        End Function

        <DurableOperation()> _
        Public Function Add(ByVal value As Integer) As Integer Implements ICalculator.Add
            currentValue += value
            Return (currentValue)
        End Function

        <DurableOperation()> _
        Public Function Subtract(ByVal value As Integer) As Integer Implements ICalculator.Subtract
            currentValue -= value
            Return (currentValue)
        End Function

        <DurableOperation()> _
        Public Function Multiply(ByVal value As Integer) As Integer Implements ICalculator.Multiply
            currentValue *= value
            Return (currentValue)
        End Function

        <DurableOperation()> _
        Public Function Divide(ByVal value As Integer) As Integer Implements ICalculator.Divide
            currentValue /= value
            Return (currentValue)
        End Function

        <DurableOperation(CompletesInstance:=True)> _
        Public Sub PowerOff() Implements ICalculator.PowerOff
        End Sub

	End Class
End Namespace