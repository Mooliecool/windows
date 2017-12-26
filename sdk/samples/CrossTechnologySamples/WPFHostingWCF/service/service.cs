
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface ICalculator
    {
        [OperationContract]
        double Add(double n1, double n2);
        [OperationContract]
        double Subtract(double n1, double n2);
        [OperationContract]
        double Multiply(double n1, double n2);
        [OperationContract]
        double Divide(double n1, double n2);
    }

    // Service class which implements the service contract.
    //ConcurrencyMode.Single means only one request will be serviced at a time.  This is an easy way to make sure two messages are not being
    //processed simultaneously when interacting with the ui.  ConcurrencyMode.Single is the default
    //
    //InstanceContextMode.Single means only one instance of the service will be used to respond to incoming messages.  This is the only mode allowed when
    //controlling the instance of a service that is used by the host.  This is quite typical for a service that needs to be initialized with a reference
    //to a UI element
    //
    //UseSynchronizationContext makes sure messages come in on the thread that the service host was opened on.  This ensures synchronization
    //with the ui thread.  Default is true.
    [ServiceBehavior(ConcurrencyMode=ConcurrencyMode.Single, InstanceContextMode=InstanceContextMode.Single, UseSynchronizationContext=true)]
    public class CalculatorService : ICalculator
    {
        IDisplay display;

        public CalculatorService(IDisplay display)
        {
            this.display = display;
        }

        public double Add(double n1, double n2)
        {
            double result = n1 + n2;
            display.Write(string.Format("received Add: {0} + {1}\nreturning:{2:0.00}\n\n", n1, n2, result));
            return result;
        }

        public double Subtract(double n1, double n2)
        {
            double result = n1 - n2;
            display.Write(string.Format("received Subtract: {0} - {1}\nreturning:{2:0.00}\n\n", n1, n2, result));
            return result;
        }

        public double Multiply(double n1, double n2)
        {
            double result = n1 * n2;
            display.Write(string.Format("received Multiply: {0} * {1}\nreturning:{2:0.00}\n\n", n1, n2, result));
            return result;
        }

        public double Divide(double n1, double n2)
        {
            double result = n1 / n2;
            display.Write(string.Format("received Divide: {0} / {1}\nreturning:{2:0.00}\n\n", n1, n2, result));
            return result;
        }
    }

    public interface IDisplay
    {
        void Write(string value);
    }
}
