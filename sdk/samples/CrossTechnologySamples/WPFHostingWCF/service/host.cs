
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    internal class MyServiceHost
    {
        internal static ServiceHost myServiceHost = null;

        internal static void StartService(CalculatorService instance)
        {
            Uri baseAddress = new Uri(System.Configuration.ConfigurationSettings.AppSettings["baseAddress"]);

            //Instantiate new ServiceHost with the singleton instance being passed in
            myServiceHost = new ServiceHost(instance, baseAddress);

            //Open myServiceHost
            myServiceHost.Open();
        }

        internal static void StopService()
        {
            //Call StopService from your shutdown logic
            if (myServiceHost.State != CommunicationState.Closed)
                myServiceHost.Close();
        }
    }
}
