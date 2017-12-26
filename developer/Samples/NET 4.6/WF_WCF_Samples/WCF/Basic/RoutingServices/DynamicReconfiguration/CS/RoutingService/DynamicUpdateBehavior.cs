//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Routing;
using System.Threading;

namespace Microsoft.Samples.RoutingDynamicReconfiguration
{


    
    public class UpdateBehavior : BehaviorExtensionElement, IServiceBehavior
    {
        void IServiceBehavior.AddBindingParameters(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase, System.Collections.ObjectModel.Collection<ServiceEndpoint> endpoints, BindingParameterCollection bindingParameters)
        {
        }

        void IServiceBehavior.ApplyDispatchBehavior(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
            RulesUpdateExtension rulesUpdateExtension = new RulesUpdateExtension();
            serviceHostBase.Extensions.Add(rulesUpdateExtension);
        }
        void IServiceBehavior.Validate(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
        }

        class RulesUpdateExtension : IExtension<ServiceHostBase>, IDisposable
        {
            bool primary = false;
            ServiceHostBase owner;
            Timer timer;

            void IExtension<ServiceHostBase>.Attach(ServiceHostBase owner)
            {
                this.owner = owner;
                //Call immediately, then every 5 seconds after that.
                this.timer = new Timer(this.UpdateRules, this, TimeSpan.Zero, TimeSpan.FromSeconds(5));
            }

            void IExtension<ServiceHostBase>.Detach(ServiceHostBase owner)
            {
                this.Dispose();
            }

            public void Dispose()
            {
                if (this.timer != null)
                {
                    this.timer.Dispose();
                    this.timer = null;
                }
            }

            void UpdateRules(object state)
            {
                Console.WriteLine("Updating Routing Configuration");
                RoutingConfiguration rc = new RoutingConfiguration();

                if (this.primary)
                {
                    ServiceEndpoint regularCalc = new ServiceEndpoint(
                    ContractDescription.GetContract(typeof(IRequestReplyRouter)),
                    new NetTcpBinding(),
                    new EndpointAddress("net.tcp://localhost:9090/servicemodelsamples/service/"));
                    rc.FilterTable.Add(new MatchAllMessageFilter(), new List<ServiceEndpoint> { regularCalc });
                    Console.WriteLine("Now routing messages to net.tcp://localhost:9090/servicemodelsamples/service/");
                }
                else
                {
                    ServiceEndpoint roundingCalc = new ServiceEndpoint(
                        ContractDescription.GetContract(typeof(IRequestReplyRouter)),
                        new NetTcpBinding(),
                        new EndpointAddress("net.tcp://localhost:8080/servicemodelsamples/service/"));
                    rc.FilterTable.Add(new MatchAllMessageFilter(), new List<ServiceEndpoint> { roundingCalc });
                    Console.WriteLine("Now routing messages to net.tcp://localhost:8080/servicemodelsamples/service/");
                }

                this.owner.Extensions.Find<RoutingExtension>().ApplyConfiguration(rc);
                Console.WriteLine("config applied\n");

                this.primary = !this.primary;
            }
        }

        public override Type BehaviorType
        {
            get { return typeof(UpdateBehavior); }
        }

        protected override object CreateBehavior()
        {
            return new UpdateBehavior();
        }
    }



}
