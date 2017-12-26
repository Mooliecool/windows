//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Routing;

namespace Microsoft.Samples.RouterBridgingAndErrorHandling
{
    public class Router
    {

        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type.
            using (ServiceHost serviceHost =
                new ServiceHost(typeof(RoutingService)))
            {
                //Rename or delete the provided App.config and  
                //uncomment this method call to run a code-based Routing Service
                //ConfigureRouterViaCode(serviceHost);
                
                // Open the ServiceHost to create listeners         
                // and start listening for messages.
                Console.WriteLine("The Routing Service configured, opening....");
                serviceHost.Open();
                Console.WriteLine("The Routing Service is now running.");
                Console.WriteLine("Press <ENTER> to terminate router.");
                
                // The service can now be accessed.
                Console.ReadLine();
            }
        }

        private static void ConfigureRouterViaCode(ServiceHost serviceHost)
        {
            //This code sets up the Routing Sample via code.  Rename or delete the App.config file
            //and comment out this method call to run a config-based Routing Service

            //set up some communication defaults
            string deadAddress = "net.tcp://localhost:9090/servicemodelsamples/fakeDestination";
            string realAddress = "net.tcp://localhost:8080/servicemodelsamples/service";
            string routerAddress = "http://localhost/routingservice/router";

            //note that the calculator client will be communicating to the Routing Service via basic
            //HTTP, while the Routing Service is using Net.TCP to communicate to the calculator service.
            //This demonstrates the Routing Service's capability of bridging between different message
            //transports, formats, bindings, etc.  This automatic message conversion is governed by
            //whether or not SoapProcessing (enabled by default) is enabled on the Routing Configuration. 
            Binding routerBinding = new BasicHttpBinding();
            Binding clientBinding = new NetTcpBinding();
            
            //add the endpoint the router will use to recieve messages
            serviceHost.AddServiceEndpoint(typeof(IRequestReplyRouter), routerBinding, routerAddress);
            
            //create the client endpoint the router will route messages to
            //note that the contract description on the client endpoints is actually unused, so
            //this could be any string.  The contract specified here goes unused
            //because the Routing Service replaces this contract with one of the Router
            //contracts at runtime, depending on the contract that a message was received with.
            ContractDescription contract = new ContractDescription("IRequestReplyRouter");
            ServiceEndpoint fakeDestination = new ServiceEndpoint(contract, clientBinding, new EndpointAddress(deadAddress));
            ServiceEndpoint realDestination = new ServiceEndpoint(contract, clientBinding, new EndpointAddress(realAddress));
            
            //create the endpoint list that contains the service endpoints we want to route to
            List<ServiceEndpoint> backupList = new List<ServiceEndpoint>();

            //add the endpoints in the order that the Routing Service should contact them
            //first add the endpoint that we know will be down
            //clearly, normally you wouldn't know that this endpoint was down by default
            backupList.Add(fakeDestination);

            //then add the endpoint that will work
            //the Routing Service will attempt to send to this endpoint only if it 
            //encounters a TimeOutException or CommunicationException when sending
            //to the previous endpoint in the list.
            backupList.Add(realDestination);
            
            //create the default RoutingConfiguration option            
            RoutingConfiguration rc = new RoutingConfiguration();
            
            //add a MatchAll filter to the Routing Configuration's filter table
            //map it to the list of endpoints defined above
            //when a message matches this filter, it will be sent to the endpoints in the list in order
            //if an endpoint is down or doesn't respond (which the first client won't
            //since no service exists at that endpoint), the Routing Service will automatically move the message
            //to the next endpoint in the list and try again.
            rc.FilterTable.Add(new MatchAllMessageFilter(), backupList);
            
            //create the Routing Behavior with the Routing Configuration and add it to the 
            //serviceHost's Description.
            serviceHost.Description.Behaviors.Add(new RoutingBehavior(rc));
                        
        }
    }
    

}
