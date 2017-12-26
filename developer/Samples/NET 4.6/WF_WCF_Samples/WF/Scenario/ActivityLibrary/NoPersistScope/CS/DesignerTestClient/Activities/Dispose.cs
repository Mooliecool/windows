//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Activities.Statements
{

    public sealed class Dispose : CodeActivity
    {
        [RequiredArgument]
        public InArgument<IDisposable> Target { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            IDisposable disposable = this.Target.Get<IDisposable>(context);
            if (disposable != null)
            {
                disposable.Dispose();
            }
        }
    }
}
