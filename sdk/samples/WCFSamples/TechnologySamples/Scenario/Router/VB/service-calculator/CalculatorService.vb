' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel
Imports System.Diagnostics
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples/", SessionMode:=SessionMode.Required)> _
    Public Interface ICalculatorService

        <OperationContract(IsOneWay:=False)> _
        Function Add(ByVal number As Integer) As Integer
        <OperationContract(IsOneWay:=False)> _
        Function Subtract(ByVal number As Integer) As Integer

    End Interface

    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.PerSession)> _
    Public Class CalculatorService
        Implements ICalculatorService

        Private total As Integer = 0

        Private Function Add(ByVal number As Integer) As Integer Implements ICalculatorService.Add

            Me.total += number
            Return Me.total

        End Function

        Private Function Subtract(ByVal number As Integer) As Integer Implements ICalculatorService.Subtract

            Me.total -= number
            Return Me.total

        End Function

    End Class

    Public Class ServiceDriver

        Public Shared Sub Main(ByVal args As String())

            Dim serviceHost As New ServiceHost(GetType(CalculatorService))
            serviceHost.Open()

            For i As Integer = 0 To serviceHost.ChannelDispatchers.Count - 1

                Dim channelDispatcher As ChannelDispatcher = TryCast(serviceHost.ChannelDispatchers(i), ChannelDispatcher)
                If channelDispatcher IsNot Nothing Then

                    For j As Integer = 0 To channelDispatcher.Endpoints.Count - 1

                        Dim endpointDispatcher As EndpointDispatcher = channelDispatcher.Endpoints(j)
                        Console.WriteLine("Listening on " & endpointDispatcher.EndpointAddress.ToString() & "...")

                    Next

                End If

            Next

            Console.WriteLine()
            Console.WriteLine("Press Enter to exit...")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
