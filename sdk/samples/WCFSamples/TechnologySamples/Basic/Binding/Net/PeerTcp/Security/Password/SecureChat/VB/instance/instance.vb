' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Security
Imports System.Security.Cryptography
Imports System.Security.Cryptography.X509Certificates
Imports System.ServiceModel

' Multi-party chat application using Peer Channel (a multi-party channel)
' If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Chat service contract
    ' Applying [PeerBehavior] attribute on the service contract enables retrieval of PeerNode from IClientChannel.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples", CallbackContract:=GetType(IChat))> _
    Public Interface IChat

        <OperationContract(IsOneWay:=True)> _
        Sub Join(ByVal member As String)
        <OperationContract(IsOneWay:=True)> _
        Sub Chat(ByVal member As String, ByVal msg As String)
        <OperationContract(IsOneWay:=True)> _
        Sub Leave(ByVal member As String)

    End Interface

    Public Interface IChatChannel
        Inherits IChat
        Inherits IClientChannel
    End Interface

    Public Class ChatApp
        Implements IChat

        ' Host the chat instance within this EXE console application.
        Public Shared Sub Main(ByVal args As String())

            Console.WriteLine("Enter your nickname [default=DefaultName]: ")
            Dim member As String = Console.ReadLine()
            Console.WriteLine("Enter the mesh password: ")
            Dim password As String = Console.ReadLine()

            If member = "" Then member = "DefaultName"

            ' Construct InstanceContext to handle messages on callback interface. 
            ' An instance of ChatApp is created and passed to the InstanceContext.
            Dim site As New InstanceContext(New ChatApp())

            ' Create the participant with the given endpoint configuration
            ' Each participant opens a duplex channel to the mesh
            ' participant is an instance of the chat application that has opened a channel to the mesh
            Dim binding As New NetPeerTcpBinding("SecureChatBinding")
            Dim cf As ChannelFactory(Of IChatChannel) = New DuplexChannelFactory(Of IChatChannel)(site, "SecureChatEndpoint")

            'for PeerAuthenticationMode.Password, you need to specify a password
            cf.Credentials.Peer.MeshPassword = password
            Dim participant As IChatChannel = cf.CreateChannel()

            ' Retrieve the PeerNode associated with the participant and register for online/offline events
            ' PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
            Dim ostat As IOnlineStatus = participant.GetProperty(Of IOnlineStatus)()
            AddHandler ostat.Online, AddressOf OnOnline
            AddHandler ostat.Offline, AddressOf OnOffline

            ' Print instructions to user
            Console.WriteLine("{0} is ready", member)
            Console.WriteLine("Type chat messages after going Online")
            Console.WriteLine("Press q<ENTER> to terminate this instance.")

            ' Announce self to other participants
            participant.Join(member)

            While True

                Dim line As String = Console.ReadLine()
                If line = "q" Then
                    Exit While
                End If
                participant.Chat(member, line)

            End While
            ' Leave the mesh and close the proxy
            participant.Leave(member)

            DirectCast(participant, IChannel).Close()
            cf.Close()

        End Sub

        ' IChat implementation
        Public Sub Join(ByVal member As String) Implements IChat.Join

            Console.WriteLine("{0} joined", member)

        End Sub

        Public Sub Chat(ByVal member As String, ByVal msg As String) Implements IChat.Chat

            Console.WriteLine("[{0}] {1}", member, msg)

        End Sub

        Public Sub Leave(ByVal member As String) Implements IChat.Leave

            Console.WriteLine("[{0} left]", member)

        End Sub

        ' PeerNode event handlers
        Private Shared Sub OnOnline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("** Online")

        End Sub

        Private Shared Sub OnOffline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("** Offline")

        End Sub

        Friend Shared Function GetCertificate(ByVal storeName As StoreName, ByVal storeLocation As StoreLocation, ByVal key As String, ByVal findType As X509FindType) As X509Certificate2

            Dim result As X509Certificate2

            Dim store As New X509Store(storeName, storeLocation)
            store.Open(OpenFlags.[ReadOnly])
            Try

                Dim matches As X509Certificate2Collection
                matches = store.Certificates.Find(findType, key, False)
                If matches.Count > 1 Then
                    Throw New InvalidOperationException([String].Format("More than one certificate with key '{0}' found in the store.", key))
                End If
                If matches.Count = 0 Then
                    Throw New InvalidOperationException([String].Format("No certificates with key '{0}' found in the store.", key))
                End If
                result = matches(0)

            Finally

                store.Close()

            End Try

            Return result

        End Function

    End Class

End Namespace