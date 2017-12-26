//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Messaging;
using LoanClient.ServiceReference1;

namespace LoanClient
{
    class Program
    {
        static void Main(string[] args)
        {
            if (!MessageQueue.Exists(@".\private$\LoanService/Service1.xamlx"))
            {
                string queueName = @".\private$\LoanService/Service1.xamlx";
                MessageQueue.Create(queueName, true);
                Console.WriteLine("Message Queue {0} created", queueName);
                Console.WriteLine("Press <enter> to exit");
                Console.ReadLine();
                return;
            }
            LoanServiceClient client = new LoanServiceClient();
            Console.WriteLine("Performing loan approval steps in reverse order ... ");
            Console.WriteLine("STEP3: Send Escrow Acknowledgement");
            client.EscrowAcknowledgement("application1");
            Console.WriteLine("Press <enter> to continue");
            Console.ReadLine();
            Console.WriteLine("STEP2: Send Lender Acknowledgement");
            client.LenderAcknowledgement("application1");
            Console.WriteLine("Press <enter> to continue");
            Console.ReadLine();
            Console.WriteLine("STEP1: Create new Load Request");
            client.CreateRequest("application1");
            Console.WriteLine("Press <enter> to exit");
            Console.ReadLine();

        }
    }
}
