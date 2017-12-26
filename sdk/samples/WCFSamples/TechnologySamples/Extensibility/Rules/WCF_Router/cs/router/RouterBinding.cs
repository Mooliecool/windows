//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;

namespace Microsoft.ServiceModel.Samples
{
    public enum RouterTransport
    {
        Http = 0,
        Tcp = 1,
        NamedPipe = 2
    }

    public enum MessageEncoding
    {
        Text,
        Binary,
        Mtom,
    }

    public class RouterBinding : Binding, IBindingRuntimePreferences
    {
        MessageEncoding messageEncoding;
        RouterTransport transport;

        HttpTransportBindingElement httpTransport;
        TcpTransportBindingElement tcpTransport;
        NamedPipeTransportBindingElement namedPipeTransport;

        TextMessageEncodingBindingElement textEncoding;
        MtomMessageEncodingBindingElement mtomEncoding;
        BinaryMessageEncodingBindingElement binaryEncoding;

        public RouterBinding()
            : base()
        { 
            Initialize();
        }

        public RouterBinding(string configurationName)
            : this()
        { 
            ApplyConfiguration(configurationName);
        }

        public RouterBinding(RouterTransport transport)
            : this()
        {
            this.Transport = transport;

            if (transport == RouterTransport.NamedPipe || transport == RouterTransport.Tcp)
            {
                this.MessageEncoding = MessageEncoding.Binary;
            }
        }

        public RouterTransport Transport
        {
            get { return this.transport; }
            set
            {
                this.transport = value;
            }
        }

        public MessageEncoding MessageEncoding
        {
            get { return this.messageEncoding; }
            set
            {
                this.messageEncoding = value;
            }
        }

        public HostNameComparisonMode HostNameComparisonMode
        {
            get { return this.tcpTransport.HostNameComparisonMode; }
            set
            {
                this.tcpTransport.HostNameComparisonMode = value;
                this.namedPipeTransport.HostNameComparisonMode = value;
                this.httpTransport.HostNameComparisonMode = value;
            }
        }

        public int ListenBacklog
        {
            get { return this.tcpTransport.ListenBacklog; }
            set { this.tcpTransport.ListenBacklog = value; }
        }

        public long MaxBufferPoolSize
        {
            get { return this.tcpTransport.MaxBufferPoolSize; }
            set
            {
                this.tcpTransport.MaxBufferPoolSize = value;
                this.namedPipeTransport.MaxBufferPoolSize = value;
            }
        }

        public int MaxBufferSize
        {
            get { return this.tcpTransport.MaxBufferSize; }
            set
            {
                this.tcpTransport.MaxBufferSize = value;
                this.namedPipeTransport.MaxBufferSize = value;
            }
        }

        public int MaxConnections
        {
            get { return this.tcpTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint; }
            set
            {
                this.tcpTransport.MaxPendingConnections = value;
                this.namedPipeTransport.MaxPendingConnections = value;
                this.tcpTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint = value;
                this.namedPipeTransport.ConnectionPoolSettings.MaxOutboundConnectionsPerEndpoint = value;
            }
        }

        public long MaxReceivedMessageSize
        {
            get { return this.tcpTransport.MaxReceivedMessageSize; }
            set
            {
                this.tcpTransport.MaxReceivedMessageSize = value;
                this.namedPipeTransport.MaxReceivedMessageSize = value;
                this.httpTransport.MaxReceivedMessageSize = value;
            }
        }

        public bool PortSharingEnabled
        {
            get { return this.tcpTransport.PortSharingEnabled; }
            set { this.tcpTransport.PortSharingEnabled = value; }
        }

        public TransferMode TransferMode
        {
            get { return this.tcpTransport.TransferMode; }
            set
            {
                this.tcpTransport.TransferMode = value;
                this.namedPipeTransport.TransferMode = value;
                this.httpTransport.TransferMode = value;
            }
        }

        bool IBindingRuntimePreferences.ReceiveSynchronously
        {
            get { return false; }
        }

        public override string Scheme
        {
            get { return this.TransportElement.Scheme; }
        }

        void Initialize()
        {
            this.httpTransport = new HttpTransportBindingElement();
            this.tcpTransport = new TcpTransportBindingElement();
            this.namedPipeTransport = new NamedPipeTransportBindingElement();

            this.textEncoding = new TextMessageEncodingBindingElement();
            this.mtomEncoding = new MtomMessageEncodingBindingElement();
            this.binaryEncoding = new BinaryMessageEncodingBindingElement();

            this.httpTransport.ManualAddressing = true;
            this.tcpTransport.ManualAddressing = true;
            this.namedPipeTransport.ManualAddressing = true;

            this.transport = RouterTransport.Http;
            this.messageEncoding = MessageEncoding.Text;
        }

        void ApplyConfiguration(string configurationName)
        {
            RouterBindingCollectionElement bindingCollectionElement = RouterBindingCollectionElement.GetBindingCollectionElement();
            RouterBindingElement element = bindingCollectionElement.Bindings[configurationName];

            if (element == null)
            {
                throw new ConfigurationErrorsException(string.Format("ConfigInvalidBindingConfigurationName", configurationName, bindingCollectionElement.BindingName));
            }
            else
            {
                element.ApplyConfiguration(this);
            }
        }

        TransportBindingElement TransportElement
        {
            get
            {
                switch (this.transport)
                {
                    case RouterTransport.Http:
                        return this.httpTransport;

                    case RouterTransport.Tcp:
                        return this.tcpTransport;

                    case RouterTransport.NamedPipe:
                        return this.namedPipeTransport;
                }

                return null;
            }
        }

        MessageEncodingBindingElement EncodingElement
        {
            get
            {
                switch (this.messageEncoding)
                {
                    case MessageEncoding.Text:
                        return this.textEncoding;

                    case MessageEncoding.Mtom:
                        return this.mtomEncoding;

                    case MessageEncoding.Binary:
                        return this.binaryEncoding;
                }

                return null;
            }
        }

        public override BindingElementCollection CreateBindingElements()
        {
            BindingElementCollection elements = new BindingElementCollection();
            elements.Add(this.EncodingElement);
            elements.Add(this.TransportElement);

            return elements;
        }
    }

    public partial class RouterBindingCollectionElement : StandardBindingCollectionElement<RouterBinding, RouterBindingElement>
    {
        public static RouterBindingCollectionElement GetBindingCollectionElement()
        {
            RouterBindingCollectionElement retval = null;

            BindingsSection bindingsSection = (BindingsSection)ConfigurationManager.GetSection("system.serviceModel/bindings");
            if (null != bindingsSection)
            {
                retval = bindingsSection["routerBinding"] as RouterBindingCollectionElement;
            }
            
            return retval;
        }
    }

    public partial class RouterBindingElement : StandardBindingElement
    {
        const string TransportPropertyName = "transport";
        const string MessageVersionPropertyName = "messageVersion";
        const string MessageEncodingPropertyName = "messageEncoding";
        const string HostNameComparisonModePropertyName = "hostNameComparisonMode";
        const string ListenBacklogPropertyName = "listenBacklog";
        const string MaxBufferPoolSizePropertyName = "maxBufferPoolSize";
        const string MaxBufferSizePropertyName = "maxBufferSize";
        const string MaxConnectionsPropertyName = "maxConnections";
        const string MaxReceivedMessageSizePropertyName = "MaxReceivedMessageSize";
        const string PortSharingEnabledPropertyName = "portSharingEnabled";
        const string TransferModePropertyName = "transferMode";

        const RouterTransport DefaultTransport = RouterTransport.Http;
        const string DefaultMessageVersion = "Soap12WSAddressing10";
        const MessageEncoding DefaultMessageEncoding = MessageEncoding.Text;
        const HostNameComparisonMode DefaultHostNameComparisonMode = HostNameComparisonMode.StrongWildcard;
        const int DefaultListenBacklog = 10;
        const long DefaultMaxBufferPoolSize = 524288;
        const int DefaultMaxBufferSize = 65536;
        const int DefaultMaxConnections = 10;
        const long DefaultMaxReceivedMessageSize = 65536;
        const bool DefaultPortSharingEnabled = false;
        const TransferMode DefaultTransferMode = TransferMode.Buffered;

        ConfigurationPropertyCollection properties;

        public RouterBindingElement(string name)
            : base(name)
        { }

        public RouterBindingElement()
            : base()
        { }

        protected override ConfigurationPropertyCollection Properties
        {
            get
            {
                if (this.properties == null)
                {
                    ConfigurationPropertyCollection properties = base.Properties;
                    properties.Add(new ConfigurationProperty("transport", typeof(Microsoft.ServiceModel.Samples.RouterTransport), Microsoft.ServiceModel.Samples.RouterTransport.Http, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("messageVersion", typeof(System.String), "Soap12WSAddressing10", null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("messageEncoding", typeof(MessageEncoding), MessageEncoding.Text, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("hostNameComparisonMode", typeof(System.ServiceModel.HostNameComparisonMode), System.ServiceModel.HostNameComparisonMode.StrongWildcard, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("listenBacklog", typeof(System.Int32), 10, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("maxBufferPoolSize", typeof(System.Int64), (long)524288, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("maxBufferSize", typeof(System.Int32), 65536, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("maxConnections", typeof(System.Int32), 10, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("MaxReceivedMessageSize", typeof(System.Int64), (long)65536, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("portSharingEnabled", typeof(System.Boolean), false, null, null, System.Configuration.ConfigurationPropertyOptions.None));
                    properties.Add(new ConfigurationProperty("transferMode", typeof(System.ServiceModel.TransferMode), System.ServiceModel.TransferMode.Buffered, null, null, System.Configuration.ConfigurationPropertyOptions.None));

                    this.properties = properties;
                }
                return this.properties;
            }
        }

        protected override Type BindingElementType
        {
            get { return typeof(RouterBinding); }
        }

        [ConfigurationProperty(RouterBindingElement.TransportPropertyName, DefaultValue = RouterBindingElement.DefaultTransport)]
        public RouterTransport Transport
        {
            get { return (RouterTransport)base[RouterBindingElement.TransportPropertyName]; }
            set { base[RouterBindingElement.TransportPropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.MessageVersionPropertyName, DefaultValue = RouterBindingElement.DefaultMessageVersion)]
        public MessageVersion MessageVersion
        {
            get
            {
                string content = (string)base[RouterBindingElement.MessageVersionPropertyName];

                System.Reflection.PropertyInfo propertyInfo = typeof(MessageVersion).GetProperty(content);
                return (MessageVersion)propertyInfo.GetValue(null, null);
            }
            set
            {
                base[RouterBindingElement.MessageVersionPropertyName] = value.ToString();
            }
        }

        [ConfigurationProperty(RouterBindingElement.MessageEncodingPropertyName, DefaultValue = RouterBindingElement.DefaultMessageEncoding)]
        public MessageEncoding MessageEncoding
        {
            get { return (MessageEncoding)base[RouterBindingElement.MessageEncodingPropertyName]; }
            set { base[RouterBindingElement.MessageEncodingPropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.HostNameComparisonModePropertyName, DefaultValue = RouterBindingElement.DefaultHostNameComparisonMode)]
        public HostNameComparisonMode HostNameComparisonMode
        {
            get { return (HostNameComparisonMode)base[RouterBindingElement.HostNameComparisonModePropertyName]; }
            set { base[RouterBindingElement.HostNameComparisonModePropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.ListenBacklogPropertyName, DefaultValue = RouterBindingElement.DefaultListenBacklog)]
        public int ListenBacklog
        {
            get { return (int)base[RouterBindingElement.ListenBacklogPropertyName]; }
            set { base[RouterBindingElement.ListenBacklogPropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.MaxBufferPoolSizePropertyName, DefaultValue = RouterBindingElement.DefaultMaxBufferPoolSize)]
        public long MaxBufferPoolSize
        {
            get { return (long)base[RouterBindingElement.MaxBufferPoolSizePropertyName]; }
            set { base[RouterBindingElement.MaxBufferPoolSizePropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.MaxBufferSizePropertyName, DefaultValue = RouterBindingElement.DefaultMaxReceivedMessageSize)]
        public int MaxBufferSize
        {
            get { return (int)base[RouterBindingElement.MaxBufferSizePropertyName]; }
            set { base[RouterBindingElement.MaxBufferSizePropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.MaxConnectionsPropertyName, DefaultValue = RouterBindingElement.DefaultMaxConnections)]
        public int MaxConnections
        {
            get { return (int)base[RouterBindingElement.MaxConnectionsPropertyName]; }
            set { base[RouterBindingElement.MaxConnectionsPropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.MaxReceivedMessageSizePropertyName, DefaultValue = RouterBindingElement.DefaultMaxReceivedMessageSize)]
        public long MaxReceivedMessageSize
        {
            get { return (long)base[RouterBindingElement.MaxReceivedMessageSizePropertyName]; }
            set { base[RouterBindingElement.MaxReceivedMessageSizePropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.PortSharingEnabledPropertyName, DefaultValue = RouterBindingElement.DefaultPortSharingEnabled)]
        public bool PortSharingEnabled
        {
            get { return (bool)base[RouterBindingElement.PortSharingEnabledPropertyName]; }
            set { base[RouterBindingElement.PortSharingEnabledPropertyName] = value; }
        }

        [ConfigurationProperty(RouterBindingElement.TransferModePropertyName, DefaultValue = RouterBindingElement.DefaultTransferMode)]
        public TransferMode TransferMode
        {
            get { return (TransferMode)base[RouterBindingElement.TransferModePropertyName]; }
            set { base[RouterBindingElement.TransferModePropertyName] = value; }
        }

        protected override void InitializeFrom(Binding binding)
        {
            base.InitializeFrom(binding);
            RouterBinding routerBinding = (RouterBinding)binding;

            this.Transport = routerBinding.Transport;
            this.MessageEncoding = routerBinding.MessageEncoding;
            this.HostNameComparisonMode = routerBinding.HostNameComparisonMode;
            this.ListenBacklog = routerBinding.ListenBacklog;
            this.MaxBufferPoolSize = routerBinding.MaxBufferPoolSize;
            this.MaxBufferSize = routerBinding.MaxBufferSize;
            this.MaxConnections = routerBinding.MaxConnections;
            this.MaxReceivedMessageSize = routerBinding.MaxReceivedMessageSize;
            this.PortSharingEnabled = routerBinding.PortSharingEnabled;
            this.TransferMode = routerBinding.TransferMode;
        }

        protected override void OnApplyConfiguration(Binding binding)
        {
            RouterBinding routerBinding = (RouterBinding)binding;

            routerBinding.Transport = this.Transport;
            routerBinding.MessageEncoding = this.MessageEncoding;
            routerBinding.HostNameComparisonMode = this.HostNameComparisonMode;
            routerBinding.ListenBacklog = this.ListenBacklog;
            routerBinding.MaxBufferPoolSize = this.MaxBufferPoolSize;
            routerBinding.MaxBufferSize = this.MaxBufferSize;
            routerBinding.MaxConnections = this.MaxConnections;
            routerBinding.MaxReceivedMessageSize = this.MaxReceivedMessageSize;
            routerBinding.PortSharingEnabled = this.PortSharingEnabled;
            routerBinding.TransferMode = this.TransferMode;
        }
    }
}
