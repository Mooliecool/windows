' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Channels
Imports System.Collections
Imports System.Collections.Generic
Imports System.Configuration
Imports System.Security
Imports System.IdentityModel.Claims
Imports System.IdentityModel.Policy
Imports System.IdentityModel.Tokens
Imports System.IdentityModel.Selectors
Imports System.Security.Cryptography.X509Certificates
Imports System.ServiceModel
Imports System.ServiceModel.Security
Imports System.ServiceModel.Security.Tokens

' M:N broadcast application that enables senders to send announcements to multiple receivers
' using Peer Channel (a multi-party channel). The sender is implemented by a different program.

' If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IBroadcast

        <OperationContract(IsOneWay:=True)> _
        Sub Announce(ByVal msg As String)

    End Interface

    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples.PeerChannel")> _
    Public Interface IQuoteChange

        <OperationContract(IsOneWay:=True)> _
        Sub PriceChange(ByVal item As String, ByVal change As Double, ByVal price As Double)

    End Interface

    Class PublisherValidator
        Inherits X509CertificateValidator

        Private senderThumbprint As String
        Public Sub New(ByVal sender As X509Certificate2)

            If sender Is Nothing Then
                Throw New ArgumentException("sender")
            End If
            Me.senderThumbprint = sender.Thumbprint

        End Sub

        Public Overloads Overrides Sub Validate(ByVal certificate As X509Certificate2)

            If 0 <> [String].CompareOrdinal(certificate.Thumbprint, senderThumbprint) Then
                Throw New SecurityTokenValidationException([String].Format("Certificate '{0}' failed validation", certificate))
            End If

        End Sub

    End Class

    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.[Single])> _
    Public Class BroadcastReceiver
        Implements IQuoteChange

        ' PeerNode event handlers
        Private Shared Sub OnOnline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("** Online")

        End Sub

        Private Shared Sub OnOffline(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("** Offline")

        End Sub

        Private Function FindClaim(ByVal context As ServiceSecurityContext) As Claim

            Dim result As Claim = Nothing
            For Each [set] As ClaimSet In context.AuthorizationContext.ClaimSets

                Dim claims As IEnumerator(Of Claim) = [set].FindClaims(ClaimTypes.Name, Nothing).GetEnumerator()
                If claims.MoveNext() Then

                    result = claims.Current
                    Exit For

                End If

            Next
            Return result

        End Function

        Public Sub PriceChange(ByVal item As String, ByVal change As Double, ByVal price As Double) Implements IQuoteChange.PriceChange

            Dim context As ServiceSecurityContext = ServiceSecurityContext.Current
            Dim claim As Claim = FindClaim(context)
            Dim source As String = "unknown"
            If claim IsNot Nothing Then

                source = TryCast(claim.Resource, String)

            End If
            Console.WriteLine("{0}=>(item: {1}, change: {2}, price: {3})", source, item, change.ToString("C"), price.ToString("C"))

        End Sub

        ' Host the receiver within this EXE console application.
        Public Shared Sub Main()
            Dim recognizedPublisherName As String = ConfigurationManager.AppSettings("publisherQName")

            Dim receiver As New ServiceHost(New BroadcastReceiver())

            'this settings specifies that only messages signed with above cert should be accepted.
            Dim binding As New NetPeerTcpBinding("Binding1")

            ' set peer credentials
            Dim certificate As X509Certificate2 = GetCertificate(StoreName.TrustedPeople, StoreLocation.CurrentUser, recognizedPublisherName, X509FindType.FindBySubjectDistinguishedName)
            receiver.Credentials.Peer.MessageSenderAuthentication.CertificateValidationMode = X509CertificateValidationMode.[Custom]
            receiver.Credentials.Peer.MessageSenderAuthentication.CustomCertificateValidator = New PublisherValidator(certificate)

            ' Open the ServiceHostBase to create listeners and start listening for messages.
            receiver.Open()

            ' Retrieve the PeerNode associated with the receiver and register for online/offline events
            ' PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.

            ' Use the first base address
            Dim baseAddress As String = receiver.BaseAddresses(0).ToString()

            Dim lookFor As New EndpointAddress(baseAddress.ToString() & "announcements")
            For i As Integer = 0 To receiver.ChannelDispatchers.Count - 1

                Dim channelDispatcher As ChannelDispatcher = TryCast(receiver.ChannelDispatchers(i), ChannelDispatcher)
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

            ' Close the ServiceHostBase to shutdown the receiver
            receiver.Close()

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