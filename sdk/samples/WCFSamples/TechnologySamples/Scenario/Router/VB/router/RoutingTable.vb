' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Xml
Imports System.Xml.Serialization
Imports System.ServiceModel
Imports System.Runtime.Serialization
Imports System.Collections.Generic

Namespace Microsoft.ServiceModel.Samples

    Public Class RoutingTable

        Private m_filterTable As XPathMessageFilterTable(Of EndpointAddress)
        Private randomNumberGenerator As Random

        Public Sub New()

            Me.m_filterTable = New XPathMessageFilterTable(Of EndpointAddress)()
            Me.randomNumberGenerator = New Random()

            Dim manager As XmlNamespaceManager = New XPathMessageContext()

            Dim routingTableDataFileReader As XmlReader = XmlReader.Create(ConfigurationManager.AppSettings("routingTableXmlFile"))
            Dim routingTableData As RoutingTableData = DirectCast(New XmlSerializer(GetType(RoutingTableData)).Deserialize(routingTableDataFileReader), RoutingTableData)

            For Each ns As [RouterNamespace] In routingTableData.RouterNamespacesSection.RouterNamespaces

                manager.AddNamespace(ns.Prefix, ns.NamespaceUri)

            Next

            For Each route As Route In routingTableData.RoutesSection.Routes

                Me.m_filterTable.Add(New XPathMessageFilter(route.XPath, manager), New EndpointAddress(route.Uri))

            Next

        End Sub

        Public ReadOnly Property FilterTable() As XPathMessageFilterTable(Of EndpointAddress)

            Get

                Return Me.m_filterTable

            End Get

        End Property

        Public Function SelectDestination(ByVal message As Message) As EndpointAddress

            Dim selectedAddress As EndpointAddress = Nothing

            Dim results As IList(Of EndpointAddress) = New List(Of EndpointAddress)()
            Me.m_filterTable.GetMatchingValues(message, results)

            If results.Count = 1 Then

                selectedAddress = results(0)

            ElseIf results.Count > 0 Then

                selectedAddress = results(Me.randomNumberGenerator.[Next](0, results.Count))

            End If

            Return selectedAddress

        End Function

    End Class

    <Serializable()> _
    <XmlType([Namespace]:="http://Microsoft.ServiceModel.Samples/Router")> _
    <XmlRoot("routingTable", [Namespace]:="http://Microsoft.ServiceModel.Samples/Router", IsNullable:=False)> _
    Public Class RoutingTableData

        Private routes As New RoutesSection()
        Private namespaces As New RouterNamespacesSection()

        Public Sub New()
        End Sub

        <XmlElement("routes", Type:=GetType(RoutesSection))> _
        Public Property RoutesSection() As RoutesSection

            Get

                Return Me.routes

            End Get
            Set(ByVal value As RoutesSection)

                Me.routes = value

            End Set

        End Property

        <XmlElement("namespaces", Type:=GetType(RouterNamespacesSection))> _
        Public Property RouterNamespacesSection() As RouterNamespacesSection

            Get

                Return Me.namespaces

            End Get
            Set(ByVal value As RouterNamespacesSection)

                Me.namespaces = value

            End Set

        End Property

    End Class

    <XmlType([Namespace]:="http://Microsoft.ServiceModel.Samples/Router")> _
    Public Class RouterNamespacesSection

        Private m_namespaces As New List(Of [RouterNamespace])()

        Public Sub New()

        End Sub

        <XmlElement("namespace", Type:=GetType([RouterNamespace]))> _
        Public Property RouterNamespaces() As List(Of [RouterNamespace])

            Get

                Return Me.m_namespaces

            End Get
            Set(ByVal value As List(Of [RouterNamespace]))

                Me.m_namespaces = value

            End Set

        End Property

    End Class

    <XmlType([Namespace]:="http://Microsoft.ServiceModel.Samples/Router")> _
    Public Class RoutesSection

        Private m_routes As New List(Of Route)()

        Public Sub New()

        End Sub

        <XmlElement("route", Type:=GetType(Route))> _
        Public Property Routes() As List(Of Route)

            Get

                Return Me.m_routes

            End Get
            Set(ByVal value As List(Of Route))

                Me.m_routes = value

            End Set

        End Property

    End Class

    <XmlType([Namespace]:="http://Microsoft.ServiceModel.Samples/Router")> _
    Public Class Route

        Private m_xPath As String
        Private m_uri As String

        Public Sub New()

        End Sub

        Public Sub New(ByVal xPath As String, ByVal uri As String)

            Me.m_xPath = xPath
            Me.m_uri = uri

        End Sub

        <XmlElement("xPath")> _
        Public Property XPath() As String

            Get

                Return Me.m_xPath

            End Get
            Set(ByVal value As String)

                Me.m_xPath = value

            End Set

        End Property

        <XmlElement("uri")> _
        Public Property Uri() As String

            Get

                Return Me.m_uri

            End Get
            Set(ByVal value As String)

                Me.m_uri = value

            End Set

        End Property

    End Class

    <XmlType([Namespace]:="http://Microsoft.ServiceModel.Samples/Router")> _
    Public Class [RouterNamespace]

        Private m_prefix As String
        Private ns As String

        Public Sub New()

        End Sub

        Public Sub New(ByVal prefix As String, ByVal ns As String)

            Me.m_prefix = prefix
            Me.ns = ns

        End Sub

        <XmlElement("prefix")> _
        Public Property Prefix() As String

            Get

                Return Me.m_prefix

            End Get
            Set(ByVal value As String)

                Me.m_prefix = value

            End Set

        End Property

        <XmlElement("namespace")> _
        Public Property NamespaceUri() As String

            Get

                Return Me.ns

            End Get
            Set(ByVal value As String)

                Me.ns = value

            End Set

        End Property

    End Class

End Namespace
