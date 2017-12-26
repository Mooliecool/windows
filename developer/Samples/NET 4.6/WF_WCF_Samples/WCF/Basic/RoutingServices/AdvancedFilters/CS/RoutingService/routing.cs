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
using Microsoft.Samples.AdvancedFilters.RoutingServiceFilters;

namespace Microsoft.Samples.AdvancedFilters
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
                //Rename the provided App.config to App.config.example and  
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
            //This code sets up the Routing Sample via code.  Rename the provided app.config
            //to App.config.example and uncomment this method call to run a config-based Routing Service

            //set up some communication defaults
            //note that some of these are a little artifical for the purpose of demonstrating 
            //different filter types and how to define them

            //the regular calculator service is located at net.tcp://localhost:9090/servicemodelsamples/service/
            string calcDestinationAddress = "net.tcp://localhost:9090/servicemodelsamples/service/";

            //the rounding calc service is located at net.tcp://localhost:8080/servicemodelsamples/service/
            string roundingDestinationAddress = "net.tcp://localhost:8080/servicemodelsamples/service/";
            
            //the "Default" router address
            string routerAddress = "http://localhost/routingservice/router/general";

            //the virtualized address of the regular calculator
            string virtualCalculatorAddress = "http://localhost/routingservice/router/regular/calculator";
            
            //the virtualized address of the rounding calculator
            string virtualRoundingCalculatorAddress = "http://localhost/routingservice/router/rounding/calculator";

            //set up the bindings for the Routing Service's communication with the client
            Binding routerBinding = new WSHttpBinding();

            //set up the bindings for the Routing Service's communication with the Calculator Services
            Binding clientBinding = new NetTcpBinding();

            //use the IRequestReplyRouter since the client and services are expecting request/response communication
            ContractDescription contract = ContractDescription.GetContract(typeof(IRequestReplyRouter));

            //set up the default Router endpoint 
            ServiceEndpoint routerEndpoint = new ServiceEndpoint(contract, routerBinding, new EndpointAddress(routerAddress));
            routerEndpoint.Name = "routerEndpoint";

            //create the virtual endpoint for the regular CalculatorSerivice
            ServiceEndpoint calcEndpoint = new ServiceEndpoint(contract, routerBinding, new EndpointAddress(virtualCalculatorAddress));
            calcEndpoint.Name = "calculatorEndpoint";

            //create the virtual endpoint for the rounding CalculatorSerivice
            ServiceEndpoint roundingEndpoint = new ServiceEndpoint(contract, routerBinding, new EndpointAddress(virtualRoundingCalculatorAddress));
            roundingEndpoint.Name = "roundingEndpoint";

            //add the inbound endpoints that the Routing Service will listen for
            serviceHost.AddServiceEndpoint(routerEndpoint);
            serviceHost.AddServiceEndpoint(calcEndpoint);
            serviceHost.AddServiceEndpoint(roundingEndpoint);
            
            //create the client endpoints the router will route messages to
            ServiceEndpoint RegularCalcEndpoint = new ServiceEndpoint(contract, new NetTcpBinding(), new EndpointAddress(calcDestinationAddress));
            ServiceEndpoint RoundingCalcEndpoint = new ServiceEndpoint(contract, new NetTcpBinding(), new EndpointAddress(roundingDestinationAddress));
            
            //create the endpoint lists that contains the service endpoints we want to route to
            List<ServiceEndpoint> RegularCalcs = new List<ServiceEndpoint>();
            List<ServiceEndpoint> RoundingCalcs = new List<ServiceEndpoint>();

            //add the endpoints in the order we want the Routing Service to try sending to them
            RegularCalcs.Add(RegularCalcEndpoint);
            RoundingCalcs.Add(RoundingCalcEndpoint);
            
            //create the default RoutingConfiguration 
            RoutingConfiguration rc = new RoutingConfiguration();

            //create all of the necessary filters

            //create a new XPathMessageFilter that will look for the custom header
            //Unfortunately, the default namespace manager doesn't have the custom namespace
            // that we use defined so we have to define that prefix ourselves.
            //Any message that shows up with this header will match this filter.
            XPathMessageContext namespaceManager = new XPathMessageContext();
            namespaceManager.AddNamespace("custom", "http://my.custom.namespace/");

            XPathMessageFilter xpathFilter = new XPathMessageFilter("sm:header()/custom:RoundingCalculator = 1", namespaceManager);

            //create a new Endpoint Name Message Filter, which will match any message that was received
            //on the calculator Endpoint.  The Endpoint name was defined when we created the service endpoint object
            EndpointNameMessageFilter endpointNameFilter = new EndpointNameMessageFilter("calculatorEndpoint");

            //Create a new Prefix Endpoint Address Message Filter.  This will match any message that showed up on an endpoint
            //with an address that matches the address -prefix- (or front portion) provided.  In this example we define
            //the address prefix as "http://localhost/routingservice/router/rounding/".  This means that any messages that arrive
            //addressed to http://localhost/routingservice/router/rounding/* will be matched by this filter.  In this case, that
            //will be messages that show up on the rounding calculator endpoint, which has the address of 
            //http://localhost/routingservice/router/rounding/calculator.
            PrefixEndpointAddressMessageFilter prefixAddressFilter = new PrefixEndpointAddressMessageFilter(new EndpointAddress("http://localhost/routingservice/router/rounding/"));

            //create two new Custom message filters.  In this example, we're going to use a "RoundRobin" message filter
            //this message filter is created in the provided RoundRobinMessageFilter.cs file.  These filters, when set
            //to the same group, will alternate between reporting that they match the message and that they don't, such that
            //only one of them will respond true at a time.
            RoundRobinMessageFilter roundRobinFilter1 = new RoundRobinMessageFilter("group1");
            RoundRobinMessageFilter roundRobinFilter2 = new RoundRobinMessageFilter("group1");

            
            //Now let's add all of those Message Filters to the Message Filter Table
            //note the use of priorities to influence the order in which the MessageFilter Table
            //executes the filters.  The higher the priority, the sooner the filter will be
            //executed, the lower the priority, the later a filter will be executed.  Thus a filter
            //at priority 2 runs before a filter at priority 1.  The default priority level
            //if one isn't specified is 0.  A Message Filter Table executes all of the filters
            //at a given priority level before moving to the next lowest priority level.
            //If a match is found at a particular priority, then the Message Filter Table doesn't
            //continue trying to find matches at the next lower priority.  
            //
            //While this example shows how to use Message Filter priorities, in general it is
            //more performant and better design to design and configure your filters such that they
            //don't require prioritization in order to function correctly.  
            

            //The first filter we add is the XPath filter, and we set its priority to 2.
            //Thus this will be the first MessageFilter that executes.  If it finds the custom
            //header, regardless of what the results of the other filters would be, the message
            //will be routed to the Rounding Calculator endpoint.
            
            //catch messages that showed up with the custom header
            rc.FilterTable.Add(xpathFilter, RoundingCalcs, 2);

            //At priority 1, we'll add two filters.  These will only run if the xpath filter
            //at priority 2 doesn't match the message.  These two filters show two different ways to 
            //determine where the message was addressed when it showed up.  Because they effectively check
            //to see if the message arrived at one of the two endpoints, we can run them 
            //at the same priority level since they're never going to both return true.

            //find messages that showed up addressed to the specific virtual endpoints
            rc.FilterTable.Add(endpointNameFilter, RegularCalcs, 1);
            rc.FilterTable.Add(prefixAddressFilter, RoundingCalcs, 1);

            //Finally, run the RoundRobin message filters.  Since we configured the filters
            //with the same group name, only one of them will match at a time.  Since we've already
            //Routed all the messages with the custom header, and then those addressed to the specific
            //virtualized endpoints, these will only be messages that showed up addressed to the
            //default router endpoint without the custom header.  Since these will switch based
            //on a per message call, half of the operations will go to the regular calculator, and 
            //half will go to the Rounding calculator.
            rc.FilterTable.Add(roundRobinFilter1, RegularCalcs, 0);
            rc.FilterTable.Add(roundRobinFilter2, RoundingCalcs, 0);
                        
            //create the Routing Behavior with the Routing Configuration and add it to the 
            //serviceHost's Description.
            serviceHost.Description.Behaviors.Add(new RoutingBehavior(rc));
                                 

        }
    }
    

}
