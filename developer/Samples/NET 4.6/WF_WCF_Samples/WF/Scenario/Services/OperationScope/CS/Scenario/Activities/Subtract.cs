//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.Scenario.Activities
{
    public sealed class Subtract : CodeActivity
    {
        public InArgument<double> X { get; set; }
        public InArgument<double> Y { get; set; }
        public OutArgument<double> Result { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            context.SetValue(this.Result, context.GetValue(this.X) - context.GetValue(this.Y));
        }
    }
}