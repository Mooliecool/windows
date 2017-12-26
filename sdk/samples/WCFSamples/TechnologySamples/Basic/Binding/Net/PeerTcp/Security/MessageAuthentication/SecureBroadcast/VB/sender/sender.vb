' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.ServiceModel
Imports System.IdentityModel.Claims
Imports System.IdentityModel.Policy
Imports System.IdentityModel.Selectors
Imports System.IdentityModel.Tokens
Imports System.Security.Cryptography.X509Certificates
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Security

' M:N broadcast application that enables senders to send announcements to multiple receivers
' using Peer Channel (a multi-party channel). The receiver is implemented by a different program.

' If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Service contract for Broadcast. It's manually generated since Peer Transport currently does not
    ' support WSDL generation.
    ' Applying [PeerBehavior] attribute on the service contract enables retrieval of PeerNode from IClientChannel.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples.PeerChannel")> _
    Public Interface IQuoteChange

        <OperationContract(IsOneWay:=True)> _
        Sub PriceChange(ByVal item As String, ByVal change As Double, ByVal price As Double)

    End Interface

    Public Interface IQuoteChannel
        Inherits IQuoteChange
        Inherits IClientChannel

    End Interface

    Class SenderValidator
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
                Throw New SecurityTokenValidationException("Unrecognized sender")
            End If

        End Sub

    End Class

    ' Sender implementation code.
    ' Host the sender within this EXE console application.
    Public Class BroadcastSender

        Public Shared Sub Main()

            ' Get the sender ID from configuration
            Dim senderId As String = ConfigurationManager.AppSettings("sender")
            Dim recognizedSender As String = "CN=" + senderId

            ' Create the sender with the given endpoint configuration
            ' Sender is an instance of the sender side of the broadcast application that has opened a channel to mesh
            Using cf As New ChannelFactory(Of IQuoteChannel)("BroadcastEndpoint")

                Dim senderCredentials As X509Certificate2 = GetCertificate(StoreName.My, StoreLocation.CurrentUser, recognizedSender, X509FindType.FindBySubjectDistinguishedName)
                cf.Credentials.Peer.Certificate = senderCredentials
                cf.Credentials.Peer.MessageSenderAuthentication.CertificateValidationMode = X509CertificateValidationMode.[Custom]
                cf.Credentials.Peer.MessageSenderAuthentication.CustomCertificateValidator = New SenderValidator(senderCredentials)

                Using sender As IQuoteChannel = DirectCast(cf.CreateChannel(), IQuoteChannel)

                    ' Retrieve the PeerNode associated with the sender and register for online/offline events
                    ' PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
                    Dim ostat As IOnlineStatus = sender.GetProperty(Of IOnlineStatus)()
                    AddHandler ostat.Online, AddressOf OnOnline
                    AddHandler ostat.Offline, AddressOf OnOffline

                    ' Open the sender 
                    sender.Open()
                    Console.WriteLine("** {0} is ready", senderId)

                    'Info that sender sends out to the receivers (mesh))
                    Dim name As String = "InfoSolo"
                    Dim currentValue As Double = [Double].Parse("100")
                    Dim change As Double = [Double].Parse("45")

                    Console.WriteLine("Enter stock value and pass <ENTER> to broadcast update to receivers.")
                    Console.WriteLine("Enter a blank value to terminate application")

                    While True

                        Console.WriteLine("New Value: ")
                        Dim newvalue As Double = 0
                        Dim value As String = Console.ReadLine()

                        If [String].IsNullOrEmpty(value) Then

                            Exit While

                        End If

                        If Not [Double].TryParse(value, newvalue) Then

                            Console.WriteLine("Invalid value entered.  Please enter a numeric value.")
                            Continue While

                        End If

                        change = newvalue - currentValue
                        currentValue = newvalue
                        sender.PriceChange(name, change, currentValue)

                        Console.WriteLine("Updated value sent.")

                    End While

                End Using

            End Using

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
