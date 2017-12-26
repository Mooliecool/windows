' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.ServiceModel

' M:N broadcast application that enables senders to send announcements to multiple receivers
' using Peer Channel (a multi-party channel). The receiver is implemented by a different program.

' If you are unfamiliar with WCF concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Service contract for Broadcast. It's manually generated since Peer Transport currently does not
    ' support WSDL generation.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IBroadcast

        <OperationContract(IsOneWay:=True)> _
        Sub Announce(ByVal msg As String)

    End Interface

    Public Interface IBroadcastChannel
        Inherits IBroadcast
        Inherits IClientChannel
    End Interface

    ' Sender implementation code.
    ' Host the sender within this EXE console application.
    Public Class BroadcastSender

        Public Shared Sub Main()

            ' Get the sender ID from configuration
            Dim senderId As String = ConfigurationManager.AppSettings("sender")

            ' Create the sender with the given endpoint configuration
            ' Sender is an instance of the sender side of the broadcast application that has opened a channel to mesh
            Dim factory As New ChannelFactory(Of IBroadcastChannel)("BroadcastEndpoint")

            Dim sender As IBroadcastChannel = factory.CreateChannel()

            ' Retrieve the PeerNode associated with the sender and register for online/offline events
            ' PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
            Dim ostat As IOnlineStatus = sender.GetProperty(Of IOnlineStatus)()
            AddHandler ostat.Online, AddressOf OnOnline
            AddHandler ostat.Offline, AddressOf OnOffline

            ' Open the sender so that the instance can join the mesh even before the first message is sent
            sender.Open()
            Console.WriteLine("{0} is ready", senderId)
            Console.WriteLine("Press <ENTER> to send annoucements after starting the receivers and going Online")
            Console.ReadLine()
            Console.WriteLine("Sending Announcement 1")
            sender.Announce("Announcement 1")
            Console.WriteLine("Sending Announcement 2")
            sender.Announce("Announcement 2")
            Console.WriteLine("Sending Announcement 3")
            sender.Announce("Announcement 3")
            Console.WriteLine("Press <ENTER> to terminate the sender.")
            Console.ReadLine()

            sender.Close()
            factory.Close()

        End Sub

        ' PeerNode event handlers
        Private Shared Sub OnOnline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("**  Online")

        End Sub

        Private Shared Sub OnOffline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("**  Offline")

        End Sub

    End Class

End Namespace