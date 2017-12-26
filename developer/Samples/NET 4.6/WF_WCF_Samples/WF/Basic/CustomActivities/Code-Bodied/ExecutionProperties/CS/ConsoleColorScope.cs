//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.ExecutionProperties
{

    public sealed class ConsoleColorScope : NativeActivity
    {
        public ConsoleColorScope()
            : base()
        {
        }

        public ConsoleColor Color { get; set; }
        public Activity Body { get; set; }

        protected override void Execute(NativeActivityContext context)
        {
            context.Properties.Add(ConsoleColorProperty.Name, new ConsoleColorProperty(this.Color));

            if (this.Body != null)
            {
                context.ScheduleActivity(this.Body);
            }
        }

        class ConsoleColorProperty : IExecutionProperty
        {
            public const string Name = "ConsoleColorProperty";

            ConsoleColor original;
            ConsoleColor color;

            public ConsoleColorProperty(ConsoleColor color)
            {
                this.color = color;
            }

            void IExecutionProperty.SetupWorkflowThread()
            {
                original = Console.ForegroundColor;
                Console.ForegroundColor = color;
            }

            void IExecutionProperty.CleanupWorkflowThread()
            {
                Console.ForegroundColor = original;
            }
        }
    }
}
