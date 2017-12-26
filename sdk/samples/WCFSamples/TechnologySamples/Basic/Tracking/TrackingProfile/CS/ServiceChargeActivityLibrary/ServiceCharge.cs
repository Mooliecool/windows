//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.Workflow.ComponentModel;


namespace Microsoft.Samples.Workflow.ServiceChargeActivityLibrary
{
    public class ServiceCharge : Activity
    {
        private double feeValue;

        [Category("Service Charges")]
        public double Fee
        {
            get { return feeValue; }
            set { feeValue = value; }
        }


        protected override ActivityExecutionStatus Execute(ActivityExecutionContext executionContext)
        {
            Console.WriteLine("You will be charged a service fee of {0}", feeValue);
            return base.Execute(executionContext);
      
        }
    }
}
