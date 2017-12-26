' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Dispatcher
Imports System.Diagnostics
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples/")> _
    Public Interface IEchoService

        <OperationContract(IsOneWay:=False)> _
        Function Echo(ByVal message As String) As String

    End Interface

    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.[Single])> _
    Public Class EchoService
        Implements IEchoService

        Private Function Echo(ByVal message As String) As String Implements IEchoService.Echo

            Return message

        End Function

    End Class

    Public Class ServiceDriver

        Public Shared Sub Main(ByVal args As String())

            Dim serviceHost As New ServiceHost(GetType(EchoService))
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
