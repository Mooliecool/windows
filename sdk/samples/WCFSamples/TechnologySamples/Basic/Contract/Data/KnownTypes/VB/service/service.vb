' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Runtime.Serialization

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IDataContractCalculator

        <OperationContract()> _
        Function Add(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Subtract(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Multiply(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Divide(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber

    End Interface

    'Define the data contract for a complex number
    'Indicate that ComplexNumberWithMagnitude may be used in place of ComplexNumber
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    <KnownType(GetType(ComplexNumberWithMagnitude))> _
    Public Class ComplexNumber

        <DataMember()> _
        Private realField As Double
        <DataMember()> _
        Private imaginaryField As Double

        Public Sub New(ByVal r1 As Double, ByVal i1 As Double)

            Me.Real = r1
            Me.Imaginary = i1

        End Sub

        Public Property Real() As Double

            Get

                Return realField

            End Get

            Set(ByVal value As Double)

                realField = value

            End Set

        End Property

        Public Property Imaginary() As Double

            Get

                Return imaginaryField

            End Get

            Set(ByVal value As Double)

                imaginaryField = value

            End Set

        End Property

    End Class

    'Define the data contract for ComplexNumberWithMagnitude
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class ComplexNumberWithMagnitude
        Inherits ComplexNumber

        Public Sub New(ByVal real As Double, ByVal imaginary As Double)

            MyBase.New(real, imaginary)

        End Sub

        <DataMember()> _
        Public Property Magnitude() As Double

            Get

                Return Math.Sqrt(Imaginary * Imaginary + Real * Real)

            End Get

            Set(ByVal value As Double)

                Throw New NotImplementedException()

            End Set

        End Property

    End Class

    ' Service class which implements the service contract.
    Public Class DataContractCalculatorService
        Implements IDataContractCalculator

        Public Function Add(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
              Implements IDataContractCalculator.Add

            'Return the derived type
            Return New ComplexNumberWithMagnitude(n1.Real + n2.Real, n1.Imaginary + n2.Imaginary)

        End Function

        Public Function Subtract(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
              Implements IDataContractCalculator.Subtract

            'Return the derived type
            Return New ComplexNumberWithMagnitude(n1.Real - n2.Real, n1.Imaginary - n2.Imaginary)

        End Function

        Public Function Multiply(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
              Implements IDataContractCalculator.Multiply

            Dim real1 As Double = n1.Real * n2.Real
            Dim imaginary1 As Double = n1.Real * n2.Imaginary
            Dim imaginary2 As Double = n2.Real * n1.Imaginary
            Dim real2 As Double = n1.Imaginary * n2.Imaginary * -1
            'Return the base type
            Return New ComplexNumber(real1 + real2, imaginary1 + imaginary2)

        End Function

        Public Function Divide(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
              Implements IDataContractCalculator.Divide

            Dim conjugate As New ComplexNumber(n2.Real, -1 * n2.Imaginary)
            Dim numerator As ComplexNumber = Multiply(n1, conjugate)
            Dim denominator As ComplexNumber = Multiply(n2, conjugate)
            'Return the base type
            Return New ComplexNumber(numerator.Real / denominator.Real, numerator.Imaginary)

        End Function

    End Class

End Namespace