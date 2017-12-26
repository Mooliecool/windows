// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------
namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.ServiceModel;

    class Client
    {
        public static void Main(string[] args)
        {
            EchoServiceClient echoClient = new EchoServiceClient();

            Console.WriteLine("Echo(\"Is anyone there?\") returned: " + echoClient.Echo("Is anyone there?"));

            echoClient.Close();

            CalculatorServiceClient calculatorClient = new CalculatorServiceClient();

            Console.WriteLine("Add(5) returned: " + calculatorClient.Add(5));
            Console.WriteLine("Add(-3) returned: " + calculatorClient.Add(-3));

            calculatorClient.Close();

            Console.WriteLine();
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }
    }
}
