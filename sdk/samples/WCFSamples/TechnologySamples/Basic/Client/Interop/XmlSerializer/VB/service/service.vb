' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Runtime.Serialization
Imports System.Xml.Serialization
Imports System.Xml.Schema

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples"), XmlSerializerFormat()> _
    Public Interface IXmlSerializerCalculator

        <OperationContract()> _
        Function Add(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Subtract(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Multiply(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber
        <OperationContract()> _
        Function Divide(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber

    End Interface

    Public Class ComplexNumber

        Private m_real As Double
        Private m_imaginary As Double

        <XmlAttribute()> _
        Public Property Real() As Double

            Get

                Return m_real

            End Get

            Set(ByVal value As Double)

                m_real = value

            End Set

        End Property

        <XmlAttribute()> _
        Public Property Imaginary() As Double

            Get

                Return m_imaginary

            End Get

            Set(ByVal value As Double)

                m_imaginary = value

            End Set

        End Property

        Public Sub New(ByVal real As Double, ByVal imaginary As Double)

            Me.Real = real
            Me.Imaginary = imaginary

        End Sub

        Public Sub New()

        End Sub

    End Class

    ' Service class which implements the service contract.
    Public Class XmlSerializerCalculatorService
        Implements IXmlSerializerCalculator

        Public Function Add(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
            Implements IXmlSerializerCalculator.Add

            Return New ComplexNumber(n1.Real + n2.Real, n1.Imaginary + n2.Imaginary)

        End Function

        Public Function Subtract(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
            Implements IXmlSerializerCalculator.Subtract

            Return New ComplexNumber(n1.Real - n2.Real, n1.Imaginary - n2.Imaginary)

        End Function

        Public Function Multiply(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
            Implements IXmlSerializerCalculator.Multiply

            Dim real1 As Double = n1.Real * n2.Real
            Dim imaginary1 As Double = n1.Real * n2.Imaginary
            Dim imaginary2 As Double = n2.Real * n1.Imaginary
            Dim real2 As Double = n1.Imaginary * n2.Imaginary * -1
            Return New ComplexNumber(real1 + real2, imaginary1 + imaginary2)

        End Function

        Public Function Divide(ByVal n1 As ComplexNumber, ByVal n2 As ComplexNumber) As ComplexNumber _
            Implements IXmlSerializerCalculator.Divide

            Dim conjugate As New ComplexNumber(n2.Real, -1 * n2.Imaginary)
            Dim numerator As ComplexNumber = Multiply(n1, conjugate)
            Dim denominator As ComplexNumber = Multiply(n2, conjugate)
            Return New ComplexNumber(numerator.Real / denominator.Real, numerator.Imaginary / denominator.Real)

        End Function

    End Class

End Namespace