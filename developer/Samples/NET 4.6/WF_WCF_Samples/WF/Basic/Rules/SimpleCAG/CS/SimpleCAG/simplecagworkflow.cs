//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleConditionedActivityGroup
{
    public sealed partial class SimpleConditionedActivityGroupWorkflow : SequentialWorkflowActivity
    {
        private ArrayList travelNeeds = new ArrayList();
        public SimpleConditionedActivityGroupWorkflow()
        {
            InitializeComponent();
            travelNeeds.Add(TravelNeed.Airline);
            travelNeeds.Add(TravelNeed.Car);
        }

        private void Car_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Booking car reservation");
            travelNeeds.Remove(TravelNeed.Car);
        }

        private void Airline_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Booking airline reservation");
            travelNeeds.Remove(TravelNeed.Airline);
        }

        private void CarCondition(object sender, ConditionalEventArgs e)
        {
            e.Result = travelNeeds.Contains(TravelNeed.Car);
        }

        private void AirlineCondition(object sender, ConditionalEventArgs e)
        {
            e.Result = travelNeeds.Contains(TravelNeed.Airline);
        }
    }

    public enum TravelNeed
    {
        None = 0,
        Car = 1,
        Airline = 2
    }
}

