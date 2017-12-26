//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Text;

namespace Microsoft.Samples.HttpAckChannel
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Press <ENTER> when the service has started");
            Console.ReadLine();

            string remoteAddress = "http://localhost/HttpAckChannelSample";

            CustomBinding binding = new CustomBinding(
                new HttpAcknowledgmentBindingElement(),
                new TextMessageEncodingBindingElement(MessageVersion.None, Encoding.UTF8),
                new HttpTransportBindingElement());

            ChannelFactory<IProcessEmployeeData> factory
                = new ChannelFactory<IProcessEmployeeData>(binding, remoteAddress);
            IProcessEmployeeData channel = factory.CreateChannel();

            int existingEmployeeId = 25;
            int nonExistingEmployeeId = 51;

            // SUCCESS case
            try
            {
                channel.ProcessData(existingEmployeeId);
                Console.WriteLine(String.Format("Processing data for Employee Id {0} succeeded!", existingEmployeeId));
            }
            catch (CommunicationException ex)
            {
                Console.WriteLine(String.Format("Processing data for Employee Id {0} failed!", existingEmployeeId));
                Console.WriteLine(String.Format("Reason - {0}", ex.Message));
            }

            // FAILURE case
            try
            {
                channel.ProcessData(nonExistingEmployeeId);
                Console.WriteLine(String.Format("Error! Processing data for non-Existing EmployeeId {0} succeeded!", nonExistingEmployeeId));
            }
            catch (CommunicationException ex)
            {
                // The exception here is expected, when the reply Http status code is 500 - internal server error
                // we throw an "Acknowledgement failed" communication exception
                Console.WriteLine(String.Format("Processing data for Employee Id {0} failed!", nonExistingEmployeeId));
                Console.WriteLine(String.Format("Reason - {0}", ex.Message));
            }

            Console.WriteLine("\nPress <ENTER> to close the client...");
            Console.ReadLine();
        }
    }
}
