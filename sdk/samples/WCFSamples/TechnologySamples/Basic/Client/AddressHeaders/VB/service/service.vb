' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Xml
Imports System.ServiceModel
Imports System.ServiceModel.Channels

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IHello

        <OperationContract()> _
        Function Hello() As String

    End Interface

    ' Service class which implements the service contract.
    Public Class HelloService
        Implements IHello

        Public Shared ReadOnly IDName As String = "ID"
        Public Shared ReadOnly IDNamespace As String = "http://Microsoft.ServiceModel.Samples"

        Public Function Hello() As String Implements IHello.Hello

            Dim id As String = Nothing
            ' look at headers on incoming message
            For i As Integer = 0 To OperationContext.Current.IncomingMessageHeaders.Count - 1

                Dim h As MessageHeaderInfo = OperationContext.Current.IncomingMessageHeaders(i)
                ' for any reference parameters with the correct name & namespace ...
                If h.IsReferenceParameter AndAlso h.Name = IDName AndAlso h.[Namespace] = IDNamespace Then

                    ' ... read the value of that header
                    Dim xr As XmlReader = OperationContext.Current.IncomingMessageHeaders.GetReaderAtHeader(i)
                    id = xr.ReadElementContentAsString()

                End If

            Next

            ' return a value that includes the info from the reference parameter
            Return "Hello, " + id

        End Function

    End Class

End Namespace