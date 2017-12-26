//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Activities;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace Microsoft.Samples.IActivityToolboxService
{
    [Designer(typeof(ActivityDesigner))]
    public sealed class CustomActivity : CodeActivity
    {

        public Collection<Activity> Activities { get; set; }

        public CustomActivity()
        {
            Activities = new Collection<Activity>();
        }

        
        protected override void Execute(CodeActivityContext context)
        {    
            
        }
    }
}
