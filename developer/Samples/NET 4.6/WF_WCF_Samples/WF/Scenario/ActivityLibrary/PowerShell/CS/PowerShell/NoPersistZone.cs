//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.Activities;
using System.ComponentModel;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.PowerShell
{
    
    [ContentProperty("Body")]        
    class NoPersistZone : NativeActivity
    {
        Variable<NoPersistHandle> noPersistHandle;
        
        public NoPersistZone()
        {
            this.noPersistHandle = new Variable<NoPersistHandle>();
        }

        [DefaultValue(null)]
        public Activity Body 
        {
            get;
            set;
        }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            metadata.AddChild(this.Body);
            metadata.AddImplementationVariable(this.noPersistHandle);
        }
        
        protected override void Execute(NativeActivityContext context)
        {
            if (this.Body != null)
            {
                NoPersistHandle handle = this.noPersistHandle.Get(context);
                handle.Enter(context);
                context.ScheduleActivity(this.Body);
            }
        }
    }
}
