' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.ServiceModel
Imports System.ServiceModel.Dispatcher

' M:N broadcast application that enables senders to send announcements to multiple receivers
' using Peer Channel (a multi-party channel). The sender is implemented by a different program.

' If you are unfamiliar with WCF concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IBroadcast

        <OperationContract(IsOneWay:=True)> _
        Sub Announce(ByVal msg As String)

    End Interface

    ' Implements the receiver of M:N broadcast app
    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.[Single])> _
    Public Class BroadcastReceiver
        Implements IBroadcast

        ' IBroadcast implementation
        Public Sub Announce(ByVal msg As String) Implements IBroadcast.Announce

            Console.WriteLine("Received {0}", msg)

        End Sub

        ' PeerNode event handlers
        Private Shared Sub OnOnline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("**  Online")

        End Sub

        Private Shared Sub OnOffline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("**  Offline")

        End Sub

        ' Host the receiver within this EXE console application.
        Public Shared Sub Main()

            ' Create a ServiceHost for the BroadcastReceiver type
            Using serviceHost As New ServiceHost(GetType(BroadcastReceiver))

                serviceHost.Open()

                ' Open the ServiceHostBase to create listeners and start listening for messages.
                Dim lookFor As New EndpointAddress(serviceHost.BaseAddresses(0).ToString() + "announcements")
                For i As Integer = 0 To serviceHost.ChannelDispatchers.Count - 1

                    Dim channelDispatcher As ChannelDispatcher = TryCast(serviceHost.ChannelDispatchers(i), ChannelDispatcher)
                    If channelDispatcher IsNot Nothing Then

                        For j As Integer = 0 To channelDispatcher.Endpoints.Count - 1

                            Dim endpointDispatcher As EndpointDispatcher = channelDispatcher.Endpoints(j)
                            If endpointDispatcher.EndpointAddress = lookFor Then

                                Dim ostat As IOnlineStatus = DirectCast(channelDispatcher.Listener.GetProperty(Of IOnlineStatus)(), IOnlineStatus)
                                If ostat IsNot Nothing Then

                                    AddHandler ostat.Online, AddressOf OnOnline
                                    AddHandler ostat.Offline, AddressOf OnOffline
                                    If ostat.IsOnline Then

                                        Console.WriteLine("**  Online")

                                    End If

                                End If

                            End If

                        Next

                    End If

                Next

                ' The receiver can now receive broadcast announcements
                Console.WriteLine("**  The receiver is ready")
                Console.WriteLine("Press <ENTER> to terminate receiver.")
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace
