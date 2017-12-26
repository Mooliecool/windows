// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------
namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.ServiceModel.Dispatcher;
    using System.ServiceModel.Channels;
    using System.Configuration;
    using System.Xml;
    using System.Xml.Serialization;
    using System.ServiceModel;
    using System.Runtime.Serialization;
    using System.Collections.Generic;

    public class RoutingTable
    {
        XPathMessageFilterTable<EndpointAddress> filterTable;
        Random randomNumberGenerator;

        public RoutingTable()
        {
            this.filterTable = new XPathMessageFilterTable<EndpointAddress>();
            this.randomNumberGenerator = new Random();

            XmlNamespaceManager manager = new XPathMessageContext();

            XmlReader routingTableDataFileReader = XmlReader.Create(ConfigurationManager.AppSettings["routingTableXmlFile"]);
            RoutingTableData routingTableData = (RoutingTableData)new XmlSerializer(typeof(RoutingTableData)).Deserialize(routingTableDataFileReader);

            foreach (RouterNamespace ns in routingTableData.RouterNamespacesSection.RouterNamespaces)
            {
                manager.AddNamespace(ns.Prefix, ns.NamespaceUri);
            }

            foreach (Route route in routingTableData.RoutesSection.Routes)
            {
                this.filterTable.Add(new XPathMessageFilter(route.XPath, manager), new EndpointAddress(route.Uri));
            }
        }

        public XPathMessageFilterTable<EndpointAddress> FilterTable
        {
            get { return this.filterTable; }
        }

        public EndpointAddress SelectDestination(Message message)
        {
            EndpointAddress selectedAddress = null;

            IList<EndpointAddress> results = new List<EndpointAddress>();
            this.filterTable.GetMatchingValues(message, results);

            if (results.Count == 1)
            {
                selectedAddress = results[0];
            }
            else if (results.Count > 0)
            {
                selectedAddress = results[this.randomNumberGenerator.Next(0, results.Count)];
            }

            return selectedAddress;
        }
    }

    [Serializable]
    [XmlType(Namespace = "http://Microsoft.ServiceModel.Samples/Router")]
    [XmlRoot("routingTable", Namespace = "http://Microsoft.ServiceModel.Samples/Router", IsNullable = false)]
    public class RoutingTableData
    {
        RoutesSection routes = new RoutesSection();
        RouterNamespacesSection namespaces = new RouterNamespacesSection();

        public RoutingTableData()
        { }

        [XmlElement("routes", Type = typeof(RoutesSection))]
        public RoutesSection RoutesSection
        {
            get { return this.routes; }
            set { this.routes = value; }
        }

        [XmlElement("namespaces", Type = typeof(RouterNamespacesSection))]
        public RouterNamespacesSection RouterNamespacesSection
        {
            get { return this.namespaces; }
            set { this.namespaces = value; }
        }
    }

    [XmlType(Namespace = "http://Microsoft.ServiceModel.Samples/Router")]
    public class RouterNamespacesSection
    {
        List<RouterNamespace> namespaces = new List<RouterNamespace>();

        public RouterNamespacesSection()
        { }

        [XmlElement("namespace", Type = typeof(RouterNamespace))]
        public List<RouterNamespace> RouterNamespaces
        {
            get { return this.namespaces; }
            set { this.namespaces = value; }
        }
    }

    [XmlType(Namespace = "http://Microsoft.ServiceModel.Samples/Router")]
    public class RoutesSection
    {
        List<Route> routes = new List<Route>();

        public RoutesSection()
        { }

        [XmlElement("route", Type = typeof(Route))]
        public List<Route> Routes
        {
            get { return this.routes; }
            set { this.routes = value; }
        }
    }

    [XmlType(Namespace = "http://Microsoft.ServiceModel.Samples/Router")]
    public class Route
    {
        string xPath;
        string uri;

        public Route()
        { }

        public Route(string xPath, string uri)
        {
            this.xPath = xPath;
            this.uri = uri;
        }

        [XmlElement("xPath")]
        public string XPath
        {
            get { return this.xPath; }
            set { this.xPath = value; }
        }

        [XmlElement("uri")]
        public string Uri
        {
            get { return this.uri; }
            set { this.uri = value; }
        }
    }

    [XmlType(Namespace = "http://Microsoft.ServiceModel.Samples/Router")]
    public class RouterNamespace
    {
        string prefix;
        string ns;

        public RouterNamespace()
        { }

        public RouterNamespace(string prefix, string ns)
        {
            this.prefix = prefix;
            this.ns = ns;
        }

        [XmlElement("prefix")]
        public string Prefix
        {
            get { return this.prefix; }
            set { this.prefix = value; }
        }

        [XmlElement("namespace")]
        public string NamespaceUri
        {
            get { return this.ns; }
            set { this.ns = value; }
        }
    }
}
