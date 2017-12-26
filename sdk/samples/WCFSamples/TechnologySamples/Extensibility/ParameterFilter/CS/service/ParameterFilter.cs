using System;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class ParameterFilterAttribute : Attribute, IOperationBehavior
    {
        double minValue;
        double maxValue;
        public ParameterFilterAttribute()
        {
            this.minValue = double.MinValue;
            this.maxValue = double.MaxValue;
        }

        public double MinValue
        {
            get { return minValue; }
            set { minValue = value; }
        }

        public double MaxValue
        {
            get { return maxValue; }
            set { maxValue = value; }
        }

        void IOperationBehavior.Validate(OperationDescription description)
        {
        }

        void IOperationBehavior.AddBindingParameters(OperationDescription description,
                                              BindingParameterCollection parameters)
        {
        }


        void IOperationBehavior.ApplyClientBehavior(OperationDescription description,
                                              ClientOperation proxy)
        {
        }

        void IOperationBehavior.ApplyDispatchBehavior(OperationDescription description,
                                              DispatchOperation dispatch)
        {
            dispatch.ParameterInspectors.Add(new ParameterFilterBehavior(this.MinValue, this.MaxValue));
        }
    }

    internal class ParameterFilterBehavior : IParameterInspector
    {
        double minValue;
        double maxValue;

        public ParameterFilterBehavior(double minValue, double maxValue)
        {
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public void AfterCall(string operationName, object[] outputs, object returnValue, object correlationState)
        {
        }

        public object BeforeCall(string operationName, object[] inputs)
        {
            // validate parameters before call
            foreach (object input in inputs)
            {
                if ((input != null) && (input.GetType() == typeof(double)))
                {
                    if ((double)input < minValue || (double)input > maxValue)
                    {
                        throw new FaultException("Parameter out of range: " + input.ToString());
                    }
                }
            }
            return null;
        }
    }    
}
