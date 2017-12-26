//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel.Description;

namespace Microsoft.WorkflowServices.Samples
{
    [Serializable]
    [DurableService]
    public class DurableCalculator : ICalculator
    {
        int currentValue = default(int);

        [DurableOperation(CanCreateInstance=true)]
        public int PowerOn()
        {
            return currentValue;
        }

        [DurableOperation()]
        public int Add(int value)
        {

            return (currentValue += value);

        }

        [DurableOperation()]
        public int Subtract(int value)
        {
            return (currentValue -= value);
        }

        [DurableOperation()]
        public int Multiply(int value)
        {
            return (currentValue *= value);
        }

        [DurableOperation()]
        public int Divide(int value)
        {
            return (currentValue /= value);
        }

        [DurableOperation(CompletesInstance = true)]
        public void PowerOff()
        {
        }
    }
}
