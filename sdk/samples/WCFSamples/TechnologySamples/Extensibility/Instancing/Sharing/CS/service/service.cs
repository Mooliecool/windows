
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;
using System.Threading;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples", SessionMode = SessionMode.Required)]
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
        [OperationContract]
        String GetUniqueInstanceContextId();

    }

    // Define a service contract to inspect instance state
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples", SessionMode = SessionMode.Required)]
    public interface ICalculatorInstance : ICalculator
    {
        [OperationContract]
        int GetInstanceId();
        [OperationContract]
        int GetOperationCount();
    }

    // Enable one of the following instance modes to compare instancing behaviors.
    // This requires a binding that supports session
    //[ServiceBehavior(InstanceContextMode=InstanceContextMode.PerSession)]

    // PerCall creates a new instance for each operation
    //[ServiceBehavior(InstanceContextMode = InstanceContextMode.PerCall)]

    // Singleton creates a single instance for application lifetime
    //[ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]

	[Shareable]
    public class CalculatorService : ICalculatorInstance
    {
        static Object syncObject = new object();
        static int instanceCount;
        int instanceId;
        int operationCount;

        public CalculatorService()
        {
            lock (syncObject)
            {
                instanceCount++;
                instanceId = instanceCount;
            }
        }

        public double Add(double n1, double n2)
        {
            operationCount++;
            return n1 + n2;
        }

        public double Subtract(double n1, double n2)
        {
            Interlocked.Increment(ref operationCount);
            return n1 - n2;
        }

        public double Multiply(double n1, double n2)
        {
            Interlocked.Increment(ref operationCount);
            return n1 * n2;
        }

        public double Divide(double n1, double n2)
        {
            Interlocked.Increment(ref operationCount);
            return n1 / n2;
        }

        public int GetInstanceId()
        {   // Return the id for this instance
            return instanceId;
        }

        public int GetOperationCount()
        {   // Return the number of ICalculator operations performed on this instance
            lock (syncObject)
            {
                return operationCount;
            }
        }

        public String GetUniqueInstanceContextId()
        {
            InstanceContext instanceContext = OperationContext.Current.InstanceContext;
            AddressableInstanceContextInfo info = instanceContext.Extensions.Find<AddressableInstanceContextInfo>();
            return (info != null) ? info.InstanceId : null;
        }
    }

}
