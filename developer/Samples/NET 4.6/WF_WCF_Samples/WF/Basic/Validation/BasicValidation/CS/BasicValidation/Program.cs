//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.BasicValidation
{

    class Program
    {
        static void Main()
        {
            WorkflowInvoker.Invoke(new Sequence1());                 
        }
                
    }
}
