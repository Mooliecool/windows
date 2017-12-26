//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Collections.Generic;

namespace Microsoft.Samples.ContainmentValidation
{

    public sealed class CreateCountry: NativeActivity
    {
        public CreateCountry(): base()
        {
            this.States = new List<Activity>();            
        }

        public List<Activity> States { get; set; }
        public string Name { get; set; }

        protected override void Execute(NativeActivityContext context)
        {
            // not needed for the sample
        }
    }  
}

