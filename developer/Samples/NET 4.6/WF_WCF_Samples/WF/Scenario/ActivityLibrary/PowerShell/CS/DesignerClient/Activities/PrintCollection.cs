//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Collections.Generic;

namespace Microsoft.Samples.Activities.PowerShell.Activities
{

    public sealed class PrintCollection<T> : CodeActivity
    {
        public InArgument<ICollection<T>> Collection { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            ICollection<T> underlyingCollection = this.Collection.Get<ICollection<T>>(context);

            if (underlyingCollection.Count == 0)
            {
                Console.WriteLine("The collection is empty");
            }
            else
            {
                foreach (T obj in underlyingCollection)
                {
                    Console.WriteLine(obj.ToString());
                }
            }
        }
    }
}
