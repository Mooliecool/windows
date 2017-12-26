' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Description
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    Class SoapRouterExtension
        Implements IExtension(Of ServiceHostBase)

        Private m_bindings As IDictionary(Of String, Binding) = New Dictionary(Of String, Binding)(3)
        Private m_simplexDatagramChannels As IDictionary(Of EndpointAddress, ISimplexDatagramRouter) = New Dictionary(Of EndpointAddress, ISimplexDatagramRouter)()
        Private m_requestReplyDatagramChannels As IDictionary(Of EndpointAddress, IRequestReplyDatagramRouter) = New Dictionary(Of EndpointAddress, IRequestReplyDatagramRouter)()
        Private m_routingTable As New RoutingTable()

        Public Sub New()

            Me.m_bindings.Add("http", New RouterBinding(RouterTransport.Http))
            Me.m_bindings.Add("net.tcp", New RouterBinding(RouterTransport.Tcp))
            Me.m_bindings.Add("net.pipe", New RouterBinding(RouterTransport.NamedPipe))

        End Sub

        Public ReadOnly Property Bindings() As IDictionary(Of String, Binding)

            Get

                Return Me.m_bindings

            End Get

        End Property

        Public ReadOnly Property SimplexDatagramChannels() As IDictionary(Of EndpointAddress, ISimplexDatagramRouter)
            Get

                Return Me.m_simplexDatagramChannels

            End Get

        End Property

        Public ReadOnly Property RequestReplyDatagramChannels() As IDictionary(Of EndpointAddress, IRequestReplyDatagramRouter)
            Get

                Return Me.m_requestReplyDatagramChannels

            End Get

        End Property

        Public ReadOnly Property RoutingTable() As RoutingTable
            Get

                Return Me.m_routingTable

            End Get

        End Property

        Public Sub Attach(ByVal owner As ServiceHostBase) Implements IExtension(Of System.ServiceModel.ServiceHostBase).Attach

        End Sub

        Public Sub Detach(ByVal owner As ServiceHostBase) Implements IExtension(Of System.ServiceModel.ServiceHostBase).Detach

        End Sub

    End Class

    NotInheritable Class SoapRouterServiceBehavior
	
        Inherits Attribute
        Implements IServiceBehavior

        Private Sub Validate(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase) Implements IServiceBehavior.Validate

        End Sub

        Private Sub AddBindingParameters(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase, ByVal endpoints As Collection(Of ServiceEndpoint), ByVal parameters As BindingParameterCollection) Implements IServiceBehavior.AddBindingParameters

        End Sub

        Private Sub ApplyDispatchBehavior(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase) Implements IServiceBehavior.ApplyDispatchBehavior

            Dim extension As New SoapRouterExtension()
            serviceHostBase.Extensions.Add(extension)

        End Sub

    End Class

    <SoapRouterServiceBehavior()> _
    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.PerSession, ConcurrencyMode:=ConcurrencyMode.Multiple, ValidateMustUnderstand:=False)> _
    Public NotInheritable Class SoapRouter
        Implements ISimplexDatagramRouter
        Implements ISimplexSessionRouter
        Implements IRequestReplyDatagramRouter
        Implements IDuplexSessionRouter
        Implements IDisposable

        Private extension As SoapRouterExtension
        Private sessionSyncRoot As New Object()
        Private simplexSessionChannel As ISimplexSessionRouter
        Private duplexSessionChannel As IDuplexSessionRouter

        Public Sub New()

            Dim host As ServiceHostBase = OperationContext.Current.Host
            Me.extension = host.Extensions.Find(Of SoapRouterExtension)()

        End Sub

#Region "SoapIntermediary Simplex Datagram"
        Private Sub SimplexDatagramRouterProcessMessage(ByVal message As Message) Implements ISimplexDatagramRouter.ProcessMessage

            Dim [to] As EndpointAddress = Me.extension.RoutingTable.SelectDestination(message)
            If [to] Is Nothing Then
                message.Close()
                Return
            End If

            ' If the router doesn't already have a one-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
            Dim forwardingChannel As ISimplexDatagramRouter = Nothing
            If Not Me.extension.SimplexDatagramChannels.TryGetValue([to], forwardingChannel) OrElse (DirectCast(forwardingChannel, IClientChannel)).State <> CommunicationState.Opened Then

                SyncLock Me.extension.SimplexDatagramChannels

                    If Not Me.extension.SimplexDatagramChannels.TryGetValue([to], forwardingChannel) OrElse (DirectCast(forwardingChannel, IClientChannel)).State <> CommunicationState.Opened Then

                        forwardingChannel = New ChannelFactory(Of ISimplexDatagramRouter)(Me.extension.Bindings([to].Uri.Scheme), [to]).CreateChannel()
                        Me.extension.SimplexDatagramChannels([to]) = forwardingChannel

                    End If

                End SyncLock

            End If

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...")
            forwardingChannel.ProcessMessage(message)

        End Sub
#End Region

#Region "SoapIntermediary Request-Reply Datagram"

        Private Function RequestReplyDatagramRouterProcessMessage(ByVal message As Message) As Message Implements IRequestReplyDatagramRouter.ProcessMessage

            Dim [to] As EndpointAddress = Me.extension.RoutingTable.SelectDestination(message)
            If [to] Is Nothing Then

                message.Close()
                Return Nothing

            End If

            ' If the router doesn't already have a two-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
            Dim forwardingChannel As IRequestReplyDatagramRouter = Nothing
            If Not Me.extension.RequestReplyDatagramChannels.TryGetValue([to], forwardingChannel) OrElse (DirectCast(forwardingChannel, IClientChannel)).State <> CommunicationState.Opened Then

                SyncLock Me.extension.RequestReplyDatagramChannels

                    If Not Me.extension.RequestReplyDatagramChannels.TryGetValue([to], forwardingChannel) OrElse (DirectCast(forwardingChannel, IClientChannel)).State <> CommunicationState.Opened Then

                        Dim factory As New ChannelFactory(Of IRequestReplyDatagramRouter)(Me.extension.Bindings([to].Uri.Scheme), [to])
                        ' Add a channel behavior that will turn off validation of @mustUnderstand on the reply's headers.
                        factory.Endpoint.Behaviors.Add(New MustUnderstandBehavior(False))
                        forwardingChannel = factory.CreateChannel()
                        Me.extension.RequestReplyDatagramChannels([to]) = forwardingChannel

                    End If

                End SyncLock

            End If

            Console.WriteLine("Forwarding request " + message.Headers.Action + "...")
            Dim response As Message = forwardingChannel.ProcessMessage(message)

            Console.WriteLine("Forwarding response " + response.Headers.Action + "...")
            Return response

        End Function

#End Region

#Region "SoapIntermediary Simplex Session"

        Private Sub SimplexSessionRouterProcessMessage(ByVal message As Message) Implements ISimplexSessionRouter.ProcessMessage

            ' One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
            If Me.simplexSessionChannel Is Nothing Then

                SyncLock Me.sessionSyncRoot

                    If Me.simplexSessionChannel Is Nothing Then

                        Dim forwardingAddress As EndpointAddress = Me.extension.RoutingTable.SelectDestination(message)
                        If forwardingAddress Is Nothing Then

                            message.Close()
                            Return

                        End If

                        ' Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.
                        Using New OperationContextScope(DirectCast(Nothing, OperationContext))

                            Dim factory As New ChannelFactory(Of ISimplexSessionRouter)(Me.extension.Bindings(forwardingAddress.Uri.Scheme), forwardingAddress)
                            Me.simplexSessionChannel = factory.CreateChannel()

                        End Using

                    End If

                End SyncLock

            End If

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...")
            Me.simplexSessionChannel.ProcessMessage(message)

        End Sub

#End Region

#Region "SoapIntermediary Duplex Session"

        Private Sub DuplexSessionRouterProcessMessage(ByVal message As Message) Implements IDuplexSessionRouter.ProcessMessage

            ' One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
            If Me.duplexSessionChannel Is Nothing Then

                SyncLock Me.sessionSyncRoot

                    If Me.duplexSessionChannel Is Nothing Then

                        Dim forwardingAddress As EndpointAddress = Me.extension.RoutingTable.SelectDestination(message)
                        If forwardingAddress Is Nothing Then

                            message.Close()
                            Return

                        End If
                        Dim callbackChannel As ISimplexSessionRouter = OperationContext.Current.GetCallbackChannel(Of ISimplexSessionRouter)()
                        ' Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.
                        Using New OperationContextScope(DirectCast(Nothing, OperationContext))

                            Dim factory As ChannelFactory(Of IDuplexSessionRouter) = New DuplexChannelFactory(Of IDuplexSessionRouter)(New InstanceContext(Nothing, New ReturnMessageHandler(callbackChannel)), Me.extension.Bindings(forwardingAddress.Uri.Scheme), forwardingAddress)
                            ' Add a channel behavior that will turn off validation of @mustUnderstand on the headers belonging to messages flowing the opposite direction.
                            factory.Endpoint.Behaviors.Add(New MustUnderstandBehavior(False))
                            Me.duplexSessionChannel = factory.CreateChannel()

                        End Using

                    End If

                End SyncLock

            End If

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...")
            Me.duplexSessionChannel.ProcessMessage(message)

        End Sub

        Class ReturnMessageHandler
            Implements ISimplexSessionRouter

            Private returnChannel As ISimplexSessionRouter

            Public Sub New(ByVal returnChannel As ISimplexSessionRouter)

                Me.returnChannel = returnChannel

            End Sub

            Private Sub ProcessMessage(ByVal message As Message) Implements ISimplexSessionRouter.ProcessMessage

                Console.WriteLine("Forwarding return message " + message.Headers.Action + "...")
                Me.returnChannel.ProcessMessage(message)

            End Sub

        End Class

#End Region

        Private Sub Dispose() Implements IDisposable.Dispose

            Dim channel As IClientChannel = Nothing

            If Me.simplexSessionChannel IsNot Nothing Then

                channel = DirectCast(Me.simplexSessionChannel, IClientChannel)

            ElseIf Me.duplexSessionChannel IsNot Nothing Then

                channel = DirectCast(Me.duplexSessionChannel, IClientChannel)

            End If

            If channel IsNot Nothing AndAlso channel.State <> CommunicationState.Closed Then

                Try

                    ' Close will abort the channel should an exception be thrown.
                    channel.Close()

                Catch generatedExceptionName As CommunicationException

                End Try

            End If

        End Sub

    End Class

    Public Class ServiceDriver

        Public Shared Sub Main(ByVal args As String())

            Dim serviceHost As New ServiceHost(GetType(SoapRouter))
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
