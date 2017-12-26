// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------
namespace Microsoft.ServiceModel.Samples
{
    using System;
	using System.ServiceModel.Dispatcher;
	using System.ServiceModel.Description;
    using System.ServiceModel;
    using System.ServiceModel.Channels;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Configuration;

    class SoapRouterExtension : IExtension<ServiceHostBase>
    {
        IDictionary<string, Binding> bindings = new Dictionary<string, Binding>(3);

        IDictionary<EndpointAddress, ISimplexDatagramRouter> simplexDatagramChannels = new Dictionary<EndpointAddress, ISimplexDatagramRouter>();
        IDictionary<EndpointAddress, IRequestReplyDatagramRouter> requestReplyDatagramChannels = new Dictionary<EndpointAddress, IRequestReplyDatagramRouter>();

        RoutingTable routingTable = new RoutingTable();

        public SoapRouterExtension()
        {
            this.bindings.Add("http", new RouterBinding(RouterTransport.Http));
            this.bindings.Add("net.tcp", new RouterBinding(RouterTransport.Tcp));
            this.bindings.Add("net.pipe", new RouterBinding(RouterTransport.NamedPipe));
        }

        public IDictionary<string, Binding> Bindings
        {
            get { return this.bindings; }
        }

        public IDictionary<EndpointAddress, ISimplexDatagramRouter> SimplexDatagramChannels
        {
            get { return this.simplexDatagramChannels; }
        }

        public IDictionary<EndpointAddress, IRequestReplyDatagramRouter> RequestReplyDatagramChannels
        {
            get { return this.requestReplyDatagramChannels; }
        }

        public RoutingTable RoutingTable
        {
            get { return this.routingTable; }
        }

        public void Attach(ServiceHostBase owner)
        { }

        public void Detach(ServiceHostBase owner)
        { }
    }

    sealed class SoapRouterServiceBehavior : Attribute, IServiceBehavior
    {
        void IServiceBehavior.Validate(ServiceDescription description, ServiceHostBase serviceHostBase)
        { }

        void IServiceBehavior.AddBindingParameters(ServiceDescription description, ServiceHostBase serviceHostBase, Collection<ServiceEndpoint> endpoints, BindingParameterCollection parameters)
        { }

        void IServiceBehavior.ApplyDispatchBehavior(ServiceDescription description, ServiceHostBase serviceHostBase)
        {
            SoapRouterExtension extension = new SoapRouterExtension();
            serviceHostBase.Extensions.Add(extension);
        }
    }

    [SoapRouterServiceBehavior]
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession, ConcurrencyMode = ConcurrencyMode.Multiple, ValidateMustUnderstand = false)]
    public sealed class SoapRouter : ISimplexDatagramRouter, ISimplexSessionRouter, IRequestReplyDatagramRouter, IDuplexSessionRouter, IDisposable
    {
        SoapRouterExtension extension;
        object sessionSyncRoot = new object();

        ISimplexSessionRouter simplexSessionChannel;
        IDuplexSessionRouter duplexSessionChannel;

        public SoapRouter()
        {
            ServiceHostBase host = OperationContext.Current.Host;
            this.extension = host.Extensions.Find<SoapRouterExtension>();
        }

        #region SoapIntermediary Simplex Datagram
        void ISimplexDatagramRouter.ProcessMessage(Message message)
        {
            EndpointAddress to = this.extension.RoutingTable.SelectDestination(message);
            if (to == null)
            {
                message.Close();
                return;
            }

            // If the router doesn't already have a one-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
            ISimplexDatagramRouter forwardingChannel;
            if (!this.extension.SimplexDatagramChannels.TryGetValue(to, out forwardingChannel) || ((IClientChannel)forwardingChannel).State != CommunicationState.Opened)
            {
                lock (this.extension.SimplexDatagramChannels)
                {
                    if (!this.extension.SimplexDatagramChannels.TryGetValue(to, out forwardingChannel) || ((IClientChannel)forwardingChannel).State != CommunicationState.Opened)
                    {
                        forwardingChannel = new ChannelFactory<ISimplexDatagramRouter>(this.extension.Bindings[to.Uri.Scheme], to).CreateChannel();
                        this.extension.SimplexDatagramChannels[to] = forwardingChannel;
                    }
                }
            }

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...");
            forwardingChannel.ProcessMessage(message);
        }
        #endregion

        #region SoapIntermediary Request-Reply Datagram
        Message IRequestReplyDatagramRouter.ProcessMessage(Message message)
        {
            EndpointAddress to = this.extension.RoutingTable.SelectDestination(message);
            if (to == null)
            {
                message.Close();
                return null;
            }

            // If the router doesn't already have a two-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
            IRequestReplyDatagramRouter forwardingChannel;
            if (!this.extension.RequestReplyDatagramChannels.TryGetValue(to, out forwardingChannel) || ((IClientChannel)forwardingChannel).State != CommunicationState.Opened)
            {
                lock (this.extension.RequestReplyDatagramChannels)
                {
                    if (!this.extension.RequestReplyDatagramChannels.TryGetValue(to, out forwardingChannel) || ((IClientChannel)forwardingChannel).State != CommunicationState.Opened)
                    {
                        ChannelFactory<IRequestReplyDatagramRouter> factory = new ChannelFactory<IRequestReplyDatagramRouter>(this.extension.Bindings[to.Uri.Scheme], to);
                        // Add a channel behavior that will turn off validation of @mustUnderstand on the reply's headers.
                        factory.Endpoint.Behaviors.Add(new MustUnderstandBehavior(false));
                        forwardingChannel = factory.CreateChannel();

                        this.extension.RequestReplyDatagramChannels[to] = forwardingChannel;
                    }
                }
            }

            Console.WriteLine("Forwarding request " + message.Headers.Action + "...");
            Message response = forwardingChannel.ProcessMessage(message);

            Console.WriteLine("Forwarding response " + response.Headers.Action + "...");
            return response;
        }
        #endregion

        #region SoapIntermediary Simplex Session
        void ISimplexSessionRouter.ProcessMessage(Message message)
        {
            // One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
            if (this.simplexSessionChannel == null)
            {
                lock (this.sessionSyncRoot)
                {
                    if (this.simplexSessionChannel == null)
                    {
                        EndpointAddress forwardingAddress = this.extension.RoutingTable.SelectDestination(message);
                        if (forwardingAddress == null)
                        {
                            message.Close();
                            return;
                        }

                        // Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.
                        using (new OperationContextScope((OperationContext)null))
                        {
                            ChannelFactory<ISimplexSessionRouter> factory = new ChannelFactory<ISimplexSessionRouter>(this.extension.Bindings[forwardingAddress.Uri.Scheme], forwardingAddress);
                            this.simplexSessionChannel = factory.CreateChannel();
                        }
                    }
                }
            }

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...");
            this.simplexSessionChannel.ProcessMessage(message);
        }
        #endregion

        #region SoapIntermediary Duplex Session
        void IDuplexSessionRouter.ProcessMessage(Message message)
        {
            // One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
            if (this.duplexSessionChannel == null)
            {
                lock (this.sessionSyncRoot)
                {
                    if (this.duplexSessionChannel == null)
                    {
                        EndpointAddress forwardingAddress = this.extension.RoutingTable.SelectDestination(message);
                        if (forwardingAddress == null)
                        {
                            message.Close();
                            return;
                        }

                        ISimplexSessionRouter callbackChannel = OperationContext.Current.GetCallbackChannel<ISimplexSessionRouter>();
                        // Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.
                        using (new OperationContextScope((OperationContext)null))
                        {
                            ChannelFactory<IDuplexSessionRouter> factory = new DuplexChannelFactory<IDuplexSessionRouter>(new InstanceContext(null, new ReturnMessageHandler(callbackChannel)),this.extension.Bindings[forwardingAddress.Uri.Scheme], forwardingAddress);
                            // Add a channel behavior that will turn off validation of @mustUnderstand on the headers belonging to messages flowing the opposite direction.
                            factory.Endpoint.Behaviors.Add(new MustUnderstandBehavior(false));
                            this.duplexSessionChannel = factory.CreateChannel();
                        }
                    }
                }
            }

            Console.WriteLine("Forwarding message " + message.Headers.Action + "...");
            this.duplexSessionChannel.ProcessMessage(message);
        }

        class ReturnMessageHandler : ISimplexSessionRouter
        {
            ISimplexSessionRouter returnChannel;

            public ReturnMessageHandler(ISimplexSessionRouter returnChannel)
            {
                this.returnChannel = returnChannel;
            }

            void ISimplexSessionRouter.ProcessMessage(Message message)
            {
                Console.WriteLine("Forwarding return message " + message.Headers.Action + "...");
                this.returnChannel.ProcessMessage(message);
            }
        }
        #endregion

        void IDisposable.Dispose()
        {
            IClientChannel channel = null;

            if (this.simplexSessionChannel != null)
            {
                channel = (IClientChannel)this.simplexSessionChannel;
            }
            else if (this.duplexSessionChannel != null)
            {
                channel = (IClientChannel)this.duplexSessionChannel;
            }

            if (channel != null && channel.State != CommunicationState.Closed)
            {
                try
                {
                    // Close will abort the channel should an exception be thrown.
                    channel.Close();
                }
                catch (CommunicationException)
                { }
            }
        }
    }

    public class ServiceDriver
    {
        public static void Main(string[] args)
        {
            ServiceHost serviceHost = new ServiceHost(typeof(SoapRouter));
            serviceHost.Open();

            for (int i = 0; i < serviceHost.ChannelDispatchers.Count; i++)
            {
                ChannelDispatcher channelDispatcher = serviceHost.ChannelDispatchers[i] as ChannelDispatcher;
                if (channelDispatcher != null)
                {
                    for (int j = 0; j < channelDispatcher.Endpoints.Count; j++)
                    {
                        EndpointDispatcher endpointDispatcher = channelDispatcher.Endpoints[j];
                        Console.WriteLine("Listening on " + endpointDispatcher.EndpointAddress + "...");
                    }
                }
            }

            Console.WriteLine();
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }
    }
}
