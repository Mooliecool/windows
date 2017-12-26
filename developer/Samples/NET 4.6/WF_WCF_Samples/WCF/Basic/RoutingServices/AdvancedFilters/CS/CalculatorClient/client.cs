//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.AdvancedFilters
{
    //The service contract is defined in generatedClient.cs, generated from the service by the svcutil tool.

    //Client implementation code.
    class Client
    {
        static void Main()
        {
            //Print out the welcome text
            Console.WriteLine("This sample routes the Calculator Sample through the new WCF RoutingService");
            Console.WriteLine("Wait for all the services to indicate that they've started, then press");
            Console.WriteLine("<ENTER> to start the client.");

            while (Console.ReadLine() != "quit")
            {
                //Offer the Address configuration for the client
                Console.WriteLine("");
                Console.WriteLine("Welcome to the Calculator Client!");
                Console.WriteLine("Pick a Destination Endpoint");

                //The default router presumes that the Client doesn't know whether it wants a Rounding or Normal calculator
                //By default the routing service is configured to Round Robin between Calculator & Rounding Calculator
                //when it receives messages directed at this endpoint
                Console.WriteLine("[D]efault Router endpoint (\"load balanced\"): http://localhost/routingservice/router/general");

                //The client can also target to a Calculator or Rounding Calculator, if it wants those services
                //By default the Routing Service will forward requests on these endpoints directly to the backend service
                Console.WriteLine("[C]alculator: http://localhost/routingservice/router/regular/calculator");
                Console.WriteLine("[R]ounding Calculator: http://localhost/routingservice/router/rounding/calculator");

                string ep = Console.ReadLine();

                EndpointAddress epa;
                //set the default address as the general router endpoint
                epa = new EndpointAddress("http://localhost/routingservice/router/general");

                //read user input and alter the endpoint address if the client app wants a different calculator
                if (ep != null)
                {
                    if (ep.Equals("R", StringComparison.CurrentCultureIgnoreCase))
                    {
                        epa = new EndpointAddress("http://localhost/routingservice/router/rounding/calculator");
                    }
                    else if (ep.Equals("C", StringComparison.CurrentCultureIgnoreCase))
                    {
                        epa = new EndpointAddress("http://localhost/routingservice/router/regular/calculator");
                    }
                }

                //set up the CalculatorClient with the EndpointAddress, the WSHttpBinding, and the ICalculator contract
                //We use the WSHttpBinding so that the outgoing has a message envelope, which we'll manipulate in a minute
                CalculatorClient client = new CalculatorClient(new WSHttpBinding(), epa);
                                
                //Ask the customer if they want to add a custom header to the outgoing message.
                //The Router will look for this header, and if so ignore the endpoint the message was
                //received on, and instead direct the message to the RoundingCalcService.
                Console.WriteLine("");
                Console.WriteLine("Add a custom header?");
                Console.WriteLine("Yes will add a 'RoundingCalculator' header, no will leave it out.");
                Console.WriteLine("Adding a custom header will cause the message to always be routed to the Rounding Calculator service, ");
                Console.WriteLine("overriding any endpoint address choices you've made. If enter is pressed, the default is No.");
                Console.WriteLine("[Y]es or [N]o?");

                string header = Console.ReadLine();

                //if they wanted to create the header, go ahead and add it to the outgoing message
                if (header != null && header.Equals("Y", StringComparison.CurrentCultureIgnoreCase))
                {

                    //get the current operationContextScope from the client's inner channel
                    using (OperationContextScope ocs = new OperationContextScope((client.InnerChannel)))
                    {
                        //get the outgoing message headers element (collection) from the context
                        MessageHeaders messageHeadersElement = OperationContext.Current.OutgoingMessageHeaders;

                        //create a new header "RoundingCalculator", no specific namespace, and set the value to 1
                        //the Routing Service will look for this header in order to determine if the message
                        //should be routed to the RoundingCalculator
                        messageHeadersElement.Add(MessageHeader.CreateHeader("RoundingCalculator", "http://my.custom.namespace/", "1"));

                        //call the client operations
                        CallClient(client);
                    }

                }
                else //didn't add the custom header
                {
                    //call the client operations
                    CallClient(client);
                }
                
                //close the client to clean it up
                client.Close();
                Console.WriteLine();
                Console.WriteLine("Press <ENTER> to run the client again or type 'quit' to quit.");
            }
        }

        private static void CallClient(CalculatorClient client)
        {
            Console.WriteLine("");
            Console.WriteLine("Sending!");
            // Call the Add service operation.
            double value1 = 100.00D;
            double value2 = 15.99D;
            double result = client.Add(value1, value2);
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result);

            // Call the Subtract service operation.
            value1 = 145.00D;
            value2 = 76.54D;
            result = client.Subtract(value1, value2);
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result);

            // Call the Multiply service operation.
            value1 = 9.00D;
            value2 = 81.25D;
            result = client.Multiply(value1, value2);
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);

            // Call the Divide service operation.
            value1 = 22.00D;
            value2 = 7.00D;
            result = client.Divide(value1, value2);
            Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result);

        }
    }
}
