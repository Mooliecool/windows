//------------------------------------------------------------------------------
// <copyright file="AsyncOperationManager.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.ComponentModel
{
    using System.Collections;
    using System.Threading;
    using System.Diagnostics;
    using System.Security.Permissions;

    [HostProtection(SharedState = true)]
    public static class AsyncOperationManager
    {
        public static AsyncOperation CreateOperation(object userSuppliedState)
        {
            return AsyncOperation.CreateOperation(userSuppliedState, SynchronizationContext);
        }

        /// <include file='doc\AsyncOperationManager.uex' path='docs/doc[@for="AsyncOperationManager.SynchronizationContext"]/*' />
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        public static SynchronizationContext SynchronizationContext
        {
            get
            {
                if (SynchronizationContext.Current == null)
                {
                    SynchronizationContext.SetSynchronizationContext(new SynchronizationContext());
                }

                return SynchronizationContext.Current;
            }

            // a thread should set this to null  when it is done, else the context will never be disposed/GC'd
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name="FullTrust")]
            set
            {
                SynchronizationContext.SetSynchronizationContext(value);
            }
        }
    }
}

        
