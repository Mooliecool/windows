' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.Xml
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Configuration
Imports System.Net.Security
Imports System.Configuration
Imports System.Text

Namespace Microsoft.ServiceModel.Samples

    Public Enum RouterTransport

        Http = 0
        Tcp = 1
        NamedPipe = 2

    End Enum

    Public Enum MessageEncoding

        Text
        Binary
        Mtom

    End Enum

    Public Class RouterBinding
        Inherits Binding
        Implements IBindingRuntimePreferences

        Private m_messageEncoding As MessageEncoding
        Private m_transport As RouterTransport

        Private httpTransport As HttpTransportBindingElement
        Private tcpTransport As TcpTransportBindingElement
        Private namedPipeTransport As NamedPipeTransportBindingElement

        Private textEncoding As TextMessageEncodingBindingElement
        Private mtomEncoding As MtomMessageEncodingBindingElement
        Private binaryEncoding As BinaryMessageEncodingBindingElement

        Public Sub New()

            MyBase.New()
            Initialize()

        End Sub

        Public Sub New(ByVal configurationName As String)

            Me.New()
            ApplyConfiguration(configurationName)

        End Sub

        Public Sub New(ByVal transport As RouterTransport)

            Me.New()
            Me.Transport = transport
            If transport = RouterTransport.NamedPipe OrElse transport = RouterTransport.Tcp Then

                Me.MessageEncoding = MessageEncoding.Binary

            End If

        End Sub

        Public Property Transport() As RouterTransport

            Get

                Return Me.m_transport

            End Get
            Set(ByVal value As RouterTransport)

                Me.m_transport = value

            End Set

        End Property

        Public Property MessageEncoding() As MessageEncoding

            Get

                Return Me.m_messageEncoding

            End Get
            Set(ByVal value As MessageEncoding)

                Me.m_messageEncoding = value

            End Set

        End Property

        Public Property HostNameComparisonMode() As HostNameComparisonMode

            Get

                Return Me.tcpTransport.HostNameComparisonMode

            End Get
            Set(ByVal value As HostNameComparisonMode)

                Me.tcpTransport.HostNameComparisonMode = value
                Me.namedPipeTransport.HostNameComparisonMode = value
                Me.httpTransport.HostNameComparisonMode = value

            End Set

        End Property

        Public Property ListenBacklog() As Integer

            Get

                Return Me.tcpTransport.ListenBacklog

            End Get
            Set(ByVal value As Integer)

                Me.tcpTransport.ListenBacklog = value

            End Set

        End Property

        Public Property MaxBufferPoolSize() As Long

            Get

                Return Me.tcpTransport.MaxBufferPoolSize

            End Get
            Set(ByVal value As Long)

                Me.tcpTransport.MaxBufferPoolSize = value
                Me.namedPipeTransport.MaxBufferPoolSize = value

            End Set

        End Property

        Public Property MaxBufferSize() As Integer

            Get

                Return Me.tcpTransport.MaxBufferSize

            End Get
            Set(ByVal value As Integer)

                Me.tcpTransport.MaxBufferSize = value
                Me.namedPipeTransport.MaxBufferSize = value

            End Set

        End Property

        Public Property MaxConnections() As Integer

            Get

                Return Me.tcpTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint

            End Get
            Set(ByVal value As Integer)

                Me.tcpTransport.MaxPendingConnections = value
                Me.namedPipeTransport.MaxPendingConnections = value
                Me.tcpTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint = value
                Me.namedPipeTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint = value

            End Set

        End Property

        Public Property MaxReceivedMessageSize() As Long

            Get

                Return Me.tcpTransport.MaxReceivedMessageSize

            End Get
            Set(ByVal value As Long)

                Me.tcpTransport.MaxReceivedMessageSize = value
                Me.namedPipeTransport.MaxReceivedMessageSize = value
                Me.httpTransport.MaxReceivedMessageSize = value

            End Set

        End Property

        Public Property PortSharingEnabled() As Boolean

            Get

                Return Me.tcpTransport.PortSharingEnabled

            End Get
            Set(ByVal value As Boolean)

                Me.tcpTransport.PortSharingEnabled = value

            End Set

        End Property

        Public Property TransferMode() As TransferMode

            Get

                Return Me.tcpTransport.TransferMode

            End Get
            Set(ByVal value As TransferMode)

                Me.tcpTransport.TransferMode = value
                Me.namedPipeTransport.TransferMode = value
                Me.httpTransport.TransferMode = value

            End Set

        End Property

        Private ReadOnly Property ReceiveSynchronously() As Boolean Implements IBindingRuntimePreferences.ReceiveSynchronously

            Get

                Return False

            End Get

        End Property

        Public Overloads Overrides ReadOnly Property Scheme() As String

            Get

                Return Me.TransportElement.Scheme

            End Get

        End Property

        Private Sub Initialize()

            Me.httpTransport = New HttpTransportBindingElement()
            Me.tcpTransport = New TcpTransportBindingElement()
            Me.namedPipeTransport = New NamedPipeTransportBindingElement()
            Me.textEncoding = New TextMessageEncodingBindingElement()
            Me.mtomEncoding = New MtomMessageEncodingBindingElement()
            Me.binaryEncoding = New BinaryMessageEncodingBindingElement()
            Me.httpTransport.ManualAddressing = True
            Me.tcpTransport.ManualAddressing = True
            Me.namedPipeTransport.ManualAddressing = True
            Me.m_transport = RouterTransport.Http
            Me.m_messageEncoding = MessageEncoding.Text

        End Sub

        Private Sub ApplyConfiguration(ByVal configurationName As String)

            Dim bindingCollectionElement As RouterBindingCollectionElement = RouterBindingCollectionElement.GetBindingCollectionElement()
            Dim element As RouterBindingElement = bindingCollectionElement.Bindings(configurationName)

            If element Is Nothing Then

                Throw New ConfigurationErrorsException(String.Format("ConfigInvalidBindingConfigurationName", configurationName, bindingCollectionElement.BindingName))

            Else

                element.ApplyConfiguration(Me)

            End If

        End Sub

        Private ReadOnly Property TransportElement() As TransportBindingElement

            Get

                Select Case Me.m_transport

                    Case RouterTransport.Http
                        Return Me.httpTransport
                    Case RouterTransport.Tcp
                        Return Me.tcpTransport
                    Case RouterTransport.NamedPipe
                        Return Me.namedPipeTransport

                End Select

                Return Nothing

            End Get

        End Property

        Private ReadOnly Property EncodingElement() As MessageEncodingBindingElement

            Get

                Select Case Me.m_messageEncoding

                    Case MessageEncoding.Text
                        Return Me.textEncoding
                    Case MessageEncoding.Mtom
                        Return Me.mtomEncoding
                    Case MessageEncoding.Binary
                        Return Me.binaryEncoding

                End Select
                Return Nothing

            End Get

        End Property

        Public Overloads Overrides Function CreateBindingElements() As BindingElementCollection

            Dim elements As New BindingElementCollection()
            elements.Add(Me.EncodingElement)
            elements.Add(Me.TransportElement)
            Return elements

        End Function

    End Class

    Partial Public Class RouterBindingCollectionElement
        Inherits StandardBindingCollectionElement(Of RouterBinding, RouterBindingElement)

        Public Shared Function GetBindingCollectionElement() As RouterBindingCollectionElement

            Dim retval As RouterBindingCollectionElement = Nothing
            Dim bindingsSection As BindingsSection = DirectCast(ConfigurationManager.GetSection("system.serviceModel/bindings"), BindingsSection)
            If bindingsSection IsNot Nothing Then

                retval = TryCast(bindingsSection("routerBinding"), RouterBindingCollectionElement)
            End If
            Return retval

        End Function

    End Class

    Partial Public Class RouterBindingElement
        Inherits StandardBindingElement

        Const TransportPropertyName As String = "transport"
        Const MessageVersionPropertyName As String = "messageVersion"
        Const MessageEncodingPropertyName As String = "messageEncoding"
        Const HostNameComparisonModePropertyName As String = "hostNameComparisonMode"
        Const ListenBacklogPropertyName As String = "listenBacklog"
        Const MaxBufferPoolSizePropertyName As String = "maxBufferPoolSize"
        Const MaxBufferSizePropertyName As String = "maxBufferSize"
        Const MaxConnectionsPropertyName As String = "maxConnections"
        Const MaxReceivedMessageSizePropertyName As String = "MaxReceivedMessageSize"
        Const PortSharingEnabledPropertyName As String = "portSharingEnabled"
        Const TransferModePropertyName As String = "transferMode"

        Const DefaultTransport As RouterTransport = RouterTransport.Http
        Const DefaultMessageVersion As String = "Soap12WSAddressing10"
        Const DefaultMessageEncoding As MessageEncoding = MessageEncoding.Text
        Const DefaultHostNameComparisonMode As HostNameComparisonMode = HostNameComparisonMode.StrongWildcard
        Const DefaultListenBacklog As Integer = 10
        Const DefaultMaxBufferPoolSize As Long = 524288
        Const DefaultMaxBufferSize As Integer = 65536
        Const DefaultMaxConnections As Integer = 10
        Const DefaultMaxReceivedMessageSize As Long = 65536
        Const DefaultPortSharingEnabled As Boolean = False
        Const DefaultTransferMode As TransferMode = TransferMode.Buffered

        Private m_properties As ConfigurationPropertyCollection

        Public Sub New(ByVal name As String)

            MyBase.New(name)

        End Sub

        Public Sub New()

            MyBase.New()

        End Sub

        Protected Overloads Overrides ReadOnly Property Properties() As ConfigurationPropertyCollection
            Get

                If Me.m_properties Is Nothing Then

                    Dim p As ConfigurationPropertyCollection = MyBase.Properties
                    p.Add(New ConfigurationProperty("transport", GetType(Microsoft.ServiceModel.Samples.RouterTransport), Microsoft.ServiceModel.Samples.RouterTransport.Http, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("messageVersion", GetType(String), "Soap12WSAddressing10", Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("messageEncoding", GetType(MessageEncoding), MessageEncoding.Text, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("hostNameComparisonMode", GetType(System.ServiceModel.HostNameComparisonMode), System.ServiceModel.HostNameComparisonMode.StrongWildcard, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("listenBacklog", GetType(Integer), 10, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("maxBufferPoolSize", GetType(Long), CLng(524288), Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("maxBufferSize", GetType(Integer), 65536, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("maxConnections", GetType(Integer), 10, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("MaxReceivedMessageSize", GetType(Long), CLng(65536), Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("portSharingEnabled", GetType(Boolean), False, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))
                    p.Add(New ConfigurationProperty("transferMode", GetType(System.ServiceModel.TransferMode), System.ServiceModel.TransferMode.Buffered, Nothing, Nothing, System.Configuration.ConfigurationPropertyOptions.None))

                    Me.m_properties = p

                End If
                Return Me.m_properties

            End Get

        End Property

        Protected Overloads Overrides ReadOnly Property BindingElementType() As Type

            Get

                Return GetType(RouterBinding)

            End Get

        End Property

        <ConfigurationProperty(RouterBindingElement.TransportPropertyName, DefaultValue:=RouterBindingElement.DefaultTransport)> _
        Public Property Transport() As RouterTransport

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.TransportPropertyName), RouterTransport)

            End Get
            Set(ByVal value As RouterTransport)

                MyBase.Item(RouterBindingElement.TransportPropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.MessageVersionPropertyName, DefaultValue:=RouterBindingElement.DefaultMessageVersion)> _
        Public Property MessageVersion() As MessageVersion

            Get

                Dim content As String = DirectCast(MyBase.Item(RouterBindingElement.MessageVersionPropertyName), String)
                Dim propertyInfo As System.Reflection.PropertyInfo = GetType(MessageVersion).GetProperty(content)
                Return DirectCast(propertyInfo.GetValue(Nothing, Nothing), MessageVersion)

            End Get
            Set(ByVal value As MessageVersion)

                MyBase.Item(RouterBindingElement.MessageVersionPropertyName) = value.ToString()

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.MessageEncodingPropertyName, DefaultValue:=RouterBindingElement.DefaultMessageEncoding)> _
        Public Property MessageEncoding() As MessageEncoding

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.MessageEncodingPropertyName), MessageEncoding)

            End Get
            Set(ByVal value As MessageEncoding)

                MyBase.Item(RouterBindingElement.MessageEncodingPropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.HostNameComparisonModePropertyName, DefaultValue:=RouterBindingElement.DefaultHostNameComparisonMode)> _
        Public Property HostNameComparisonMode() As HostNameComparisonMode

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.HostNameComparisonModePropertyName), HostNameComparisonMode)

            End Get
            Set(ByVal value As HostNameComparisonMode)

                MyBase.Item(RouterBindingElement.HostNameComparisonModePropertyName) = value

            End Set

        End Property
        <ConfigurationProperty(RouterBindingElement.ListenBacklogPropertyName, DefaultValue:=RouterBindingElement.DefaultListenBacklog)> _
        Public Property ListenBacklog() As Integer

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.ListenBacklogPropertyName), Integer)

            End Get
            Set(ByVal value As Integer)

                MyBase.Item(RouterBindingElement.ListenBacklogPropertyName) = value

            End Set

        End Property
        <ConfigurationProperty(RouterBindingElement.MaxBufferPoolSizePropertyName, DefaultValue:=RouterBindingElement.DefaultMaxBufferPoolSize)> _
        Public Property MaxBufferPoolSize() As Long

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.MaxBufferPoolSizePropertyName), Long)

            End Get
            Set(ByVal value As Long)

                MyBase.Item(RouterBindingElement.MaxBufferPoolSizePropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.MaxBufferSizePropertyName, DefaultValue:=RouterBindingElement.DefaultMaxReceivedMessageSize)> _
        Public Property MaxBufferSize() As Integer

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.MaxBufferSizePropertyName), Integer)

            End Get
            Set(ByVal value As Integer)

                MyBase.Item(RouterBindingElement.MaxBufferSizePropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.MaxConnectionsPropertyName, DefaultValue:=RouterBindingElement.DefaultMaxConnections)> _
        Public Property MaxConnections() As Integer

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.MaxConnectionsPropertyName), Integer)

            End Get
            Set(ByVal value As Integer)

                MyBase.Item(RouterBindingElement.MaxConnectionsPropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.MaxReceivedMessageSizePropertyName, DefaultValue:=RouterBindingElement.DefaultMaxReceivedMessageSize)> _
        Public Property MaxReceivedMessageSize() As Long

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.MaxReceivedMessageSizePropertyName), Long)

            End Get
            Set(ByVal value As Long)

                MyBase.Item(RouterBindingElement.MaxReceivedMessageSizePropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.PortSharingEnabledPropertyName, DefaultValue:=RouterBindingElement.DefaultPortSharingEnabled)> _
        Public Property PortSharingEnabled() As Boolean

            Get

                Return DirectCast(MyBase.Item(RouterBindingElement.PortSharingEnabledPropertyName), Boolean)

            End Get
            Set(ByVal value As Boolean)

                MyBase.Item(RouterBindingElement.PortSharingEnabledPropertyName) = value

            End Set

        End Property

        <ConfigurationProperty(RouterBindingElement.TransferModePropertyName, DefaultValue:=RouterBindingElement.DefaultTransferMode)> _
        Public Property TransferMode() As TransferMode

            Get
                Return DirectCast(MyBase.Item(RouterBindingElement.TransferModePropertyName), TransferMode)

            End Get
            Set(ByVal value As TransferMode)

                MyBase.Item(RouterBindingElement.TransferModePropertyName) = value

            End Set

        End Property

        Protected Overloads Overrides Sub InitializeFrom(ByVal binding As Binding)

            MyBase.InitializeFrom(binding)
            Dim routerBinding As RouterBinding = DirectCast(binding, RouterBinding)

            Me.Transport = routerBinding.Transport
            Me.MessageEncoding = routerBinding.MessageEncoding
            Me.HostNameComparisonMode = routerBinding.HostNameComparisonMode
            Me.ListenBacklog = routerBinding.ListenBacklog
            Me.MaxBufferPoolSize = routerBinding.MaxBufferPoolSize
            Me.MaxBufferSize = routerBinding.MaxBufferSize
            Me.MaxConnections = routerBinding.MaxConnections
            Me.MaxReceivedMessageSize = routerBinding.MaxReceivedMessageSize
            Me.PortSharingEnabled = routerBinding.PortSharingEnabled
            Me.TransferMode = routerBinding.TransferMode

        End Sub

        Protected Overloads Overrides Sub OnApplyConfiguration(ByVal binding As Binding)

            Dim routerBinding As RouterBinding = DirectCast(binding, RouterBinding)

            routerBinding.Transport = Me.Transport
            routerBinding.MessageEncoding = Me.MessageEncoding
            routerBinding.HostNameComparisonMode = Me.HostNameComparisonMode
            routerBinding.ListenBacklog = Me.ListenBacklog
            routerBinding.MaxBufferPoolSize = Me.MaxBufferPoolSize
            routerBinding.MaxBufferSize = Me.MaxBufferSize
            routerBinding.MaxConnections = Me.MaxConnections
            routerBinding.MaxReceivedMessageSize = Me.MaxReceivedMessageSize
            routerBinding.PortSharingEnabled = Me.PortSharingEnabled
            routerBinding.TransferMode = Me.TransferMode

        End Sub

    End Class

End Namespace