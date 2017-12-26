' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Configuration
Imports System.Net
Imports System.Net.Sockets
Imports System.ServiceModel
Imports System.ServiceModel.Configuration
Imports System.ServiceModel.PeerResolvers

' Custom peer resolver client-side implementation. Derives from PeerResolver and delegates the actual
' registration, unregistration, and resolution to the resolver service (which is a separate application).

' If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.
Namespace Microsoft.ServiceModel.Samples

    ' CustomPeerResolver Service contract. It is manually generated since svcutil currently does not
    ' support WSDL generation when EndpointAddress[] is used in the service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICustomPeerResolver

        <OperationContract()> _
        Function Register(ByVal meshId As String, ByVal nodeAddresses As PeerNodeAddress) As Integer
        <OperationContract()> _
        Sub Unregister(ByVal registrationId As Integer)
        <OperationContract()> _
        Sub Update(ByVal registrationId As Integer, ByVal updatedNodeAddress As PeerNodeAddress)
        <OperationContract()> _
        Function Resolve(ByVal meshId As String, ByVal maxAddresses As Integer) As PeerNodeAddress()

    End Interface

    Public Interface ICustomPeerResolverChannel
        Inherits ICustomPeerResolver
        Inherits IClientChannel

    End Interface

    Public Class CustomPeerResolver
        Inherits PeerResolver

        Const config As String = "CustomPeerResolverEndpoint"

        ' Allow the clients to share referals
        Public Overloads Overrides ReadOnly Property CanShareReferrals() As Boolean

            Get

                Return True

            End Get

        End Property

        ' Register address for a node participating in a mesh identified by meshId with the resolver service
        Public Overloads Overrides Function Register(ByVal meshId As String, ByVal nodeAddress As PeerNodeAddress, ByVal timeout As TimeSpan) As Object

            Dim factory As New ChannelFactory(Of ICustomPeerResolverChannel)(config)

            Dim client As ICustomPeerResolverChannel = factory.CreateChannel()

            MaskScopeId(nodeAddress.IPAddresses)
            Dim registrationId As Integer = client.Register(meshId, nodeAddress)

            client.Close()

            factory.Close()

            Return registrationId

        End Function

        ' Unregister address for a node from the resolver service
        Public Overloads Overrides Sub Unregister(ByVal registrationId As Object, ByVal timeout As TimeSpan)

            Dim factory As New ChannelFactory(Of ICustomPeerResolverChannel)(config)

            Dim client As ICustomPeerResolverChannel = factory.CreateChannel()
            client.Unregister(DirectCast(registrationId, Integer))
            client.Close()

            factory.Close()

        End Sub
        ' Updates a node's registration with the resolver service.
        Public Overloads Overrides Sub Update(ByVal registrationId As Object, ByVal updatedNodeAddress As PeerNodeAddress, ByVal timeout As TimeSpan)

            Dim factory As New ChannelFactory(Of ICustomPeerResolverChannel)(config)

            Dim client As ICustomPeerResolverChannel = factory.CreateChannel()

            MaskScopeId(updatedNodeAddress.IPAddresses)
            client.Update(DirectCast(registrationId, Integer), updatedNodeAddress)
            client.Close()

            factory.Close()

        End Sub

        ' Query the resolver service for addresses associated with a mesh ID
        Public Overloads Overrides Function Resolve(ByVal meshId As String, ByVal maxAddresses As Integer, ByVal timeout As TimeSpan) As ReadOnlyCollection(Of PeerNodeAddress)

            Dim addresses As PeerNodeAddress() = Nothing

            Dim factory As New ChannelFactory(Of ICustomPeerResolverChannel)(config)

            Dim client As ICustomPeerResolverChannel = factory.CreateChannel()
            addresses = client.Resolve(meshId, maxAddresses)
            client.Close()

            factory.Close()

            ' If addresses couldn't be obtained, return empty collection
            If addresses Is Nothing Then
                addresses = New PeerNodeAddress(0) {}
            End If

            Return New ReadOnlyCollection(Of PeerNodeAddress)(addresses)

        End Function

        ' Since we send the IP addresses to the service, mask the scope ID (scopeIDs only have local significance)
        Private Sub MaskScopeId(ByVal ipAddresses As ReadOnlyCollection(Of IPAddress))

            For Each address As IPAddress In ipAddresses

                If address.AddressFamily = AddressFamily.InterNetworkV6 Then
                    address.ScopeId = 0
                End If

            Next

        End Sub

    End Class

    ' Custom resolver binding element implementation
    Public Class CustomPeerResolverBindingElement
        Inherits PeerResolverBindingElement

        Private peerReferralPolicy As PeerReferralPolicy = peerReferralPolicy.Share
        Shared resolverClient As New CustomPeerResolver()

        Public Sub New()

        End Sub

        Protected Sub New(ByVal other As CustomPeerResolverBindingElement)

            MyBase.New(other)

        End Sub

        Public Overloads Overrides Property ReferralPolicy() As PeerReferralPolicy

            Get

                Return peerReferralPolicy

            End Get
            Set(ByVal value As PeerReferralPolicy)

                peerReferralPolicy = value

            End Set

        End Property

        Public Overloads Overrides Function Clone() As BindingElement

            Return New CustomPeerResolverBindingElement(Me)

        End Function

        Public Overloads Overrides Function BuildChannelFactory(Of TChannel)(ByVal context As BindingContext) As IChannelFactory(Of TChannel)

            context.BindingParameters.Add(Me)
            Return context.BuildInnerChannelFactory(Of TChannel)()

        End Function

        Public Overloads Overrides Function CanBuildChannelFactory(Of TChannel)(ByVal context As BindingContext) As Boolean

            context.BindingParameters.Add(Me)
            Return context.CanBuildInnerChannelFactory(Of TChannel)()

        End Function

        Public Overloads Overrides Function BuildChannelListener(Of TChannel As {Class, IChannel})(ByVal context As BindingContext) As IChannelListener(Of TChannel)

            context.BindingParameters.Add(Me)
            Return context.BuildInnerChannelListener(Of TChannel)()

        End Function

        Public Overloads Overrides Function CanBuildChannelListener(Of TChannel As {Class, IChannel})(ByVal context As BindingContext) As Boolean

            context.BindingParameters.Add(Me)
            Return context.CanBuildInnerChannelListener(Of TChannel)()

        End Function

        ' Returns reference to the singleton resolver client
        Public Overloads Overrides Function CreatePeerResolver() As PeerResolver

            Return resolverClient

        End Function

        Public Overloads Overrides Function GetProperty(Of T As Class)(ByVal context As BindingContext) As T

            Return context.GetInnerProperty(Of T)()

        End Function

    End Class

    ' Custom resolver configuration binding element implementation
    Public Class CustomPeerResolverConfigurationBindingElement
        Inherits BindingElementExtensionElement

        Public Overloads Overrides ReadOnly Property BindingElementType() As Type

            Get

                Return GetType(CustomPeerResolverBindingElement)

            End Get

        End Property

        Protected Overloads Overrides Function CreateBindingElement() As BindingElement

            Return New CustomPeerResolverBindingElement()

        End Function

    End Class

End Namespace
