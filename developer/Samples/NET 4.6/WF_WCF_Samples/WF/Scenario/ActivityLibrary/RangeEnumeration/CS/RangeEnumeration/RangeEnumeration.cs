//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ComponentModel;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.Statements
{
    /// <summary>
    /// This activity enumerates through a series of integers
    /// </summary>
    [ContentProperty("Body")]
    public sealed class RangeEnumeration : NativeActivity
    {
        Variable<int> loopVariable;
        InvokeAction<int> invokeBody;

        public RangeEnumeration()
        {
            this.loopVariable = new Variable<int>("LoopVariable");
            this.invokeBody = new InvokeAction<int>
            {
                Argument = this.loopVariable,
            };

        }

        [RequiredArgument]
        public InArgument<int> Start { get; set; }

        [RequiredArgument]
        public InArgument<int> Stop { get; set; }

        [RequiredArgument]
        public InArgument<int> Step { get; set; }

        [DefaultValue(null)]
        public ActivityAction<int> Body { get; set; }

       
        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            // The Body introduces another layer of scope. Method metadata.AddImplementationChild
            // (instead of metadata.AddChild) enable access to LoopVariable from the inner
            // block scope of Body.
            if (this.Body != null && this.Body.Handler != null)
            {
                this.invokeBody.Action = this.Body;
                metadata.AddImplementationChild(this.invokeBody);
            }

            // Need to bind the arguments to the custom activity variables explicitly
            // and then add them to the metadata.
            RuntimeArgument startArg = new RuntimeArgument("Start", typeof(int), ArgumentDirection.In, true);
            metadata.Bind(this.Start, startArg);
            metadata.AddArgument(startArg);
            RuntimeArgument stopArg = new RuntimeArgument("Stop", typeof(int), ArgumentDirection.In, true);
            metadata.Bind(this.Stop, stopArg);
            metadata.AddArgument(stopArg);
            RuntimeArgument stepArg = new RuntimeArgument("Step", typeof(int), ArgumentDirection.In, true);
            metadata.Bind(this.Step, stepArg);
            metadata.AddArgument(stepArg);

            // Register variables used in the custom activity.
            metadata.AddImplementationVariable(this.loopVariable);
        } 
         

        protected override void Execute(NativeActivityContext context)
        {
            int start = this.Start.Get(context);
            int stop = this.Stop.Get(context);
            int step = this.Step.Get(context);

            // Raise exception when step is 0 as this causes infinite loop.
            if (step == 0)
            {
                throw new InvalidOperationException("Setting step = 0 causes an infinite loop.");
            }
            // If you're running this sample in debug mode in Visual Studio, it may breakpoint
            // near here because the sample intentionally passes in invalid input to demonstrate
            // this check.

            // Only run when there is an action activity set in the body, so that at each step 
            // an action is taken. There will be no execution with invalid range and step values.
            if (this.Body != null && this.Body.Handler != null && Condition(start, stop, step))
            {
                this.loopVariable.Set(context, start);
                context.ScheduleActivity(this.invokeBody, this.AddStep);
            }
        }

        // Add step value to loop variable after loop body is executed.
        void AddStep(NativeActivityContext context, ActivityInstance completedInstance)
        {
            int value = this.loopVariable.Get(context);
            int step = this.Step.Get(context);
            value += step;
            this.loopVariable.Set(context, value);
            int stop = this.Stop.Get(context);

            if (Condition(value, stop, step))
            {
                context.ScheduleActivity(this.invokeBody, this.AddStep);
            }
        }

        // Check that enumeration parameters make sense.
        bool Condition(int start, int stop, int step)
        {
            return ((step > 0 && start <= stop) || (step < 0 && start >= stop));
        }
    }
}
