//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.SuppressTransactionSample
{

    public class SuppressTransactionScope : NativeActivity
    {
        Variable<RuntimeTransactionHandle> rth;

        public SuppressTransactionScope()
        {
            this.rth = new Variable<RuntimeTransactionHandle>();
        }

        public Activity Body { get; set; }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            metadata.AddImplementationVariable(this.rth); 
            metadata.AddChild(this.Body);
        }

        protected override void Execute(NativeActivityContext context)
        {
            if (this.Body != null)
            {
                RuntimeTransactionHandle handle = rth.Get(context);

                // Must set before adding the handle to the execution properties
                handle.SuppressTransaction = true; 

                // Adding a new RuntimeTransactionHanlde rather than using the existing one so that suppression is scoped to this activity. Also,
                // because modifying the property of a registered (added to execution properties) RuntimeTransactionHandle is not allowed.
                context.Properties.Add(handle.ExecutionPropertyName, handle);  
                context.ScheduleActivity(this.Body); 
            }
        }
    }
}
