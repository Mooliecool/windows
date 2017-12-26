//------------------------------------------------------------------------------
// <copyright file="RunWorkerCompletedEventArgs.cs" company="Microsoft">
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
    using System.Security.Permissions;

    [HostProtection(SharedState = true)]
    public class RunWorkerCompletedEventArgs : AsyncCompletedEventArgs
    {
        private object result;

        public RunWorkerCompletedEventArgs(object result, 
                                           Exception error,
                                           bool cancelled)
            : base(error, cancelled, null)
        {
            this.result = result;
        }
        
        public object Result
        {
            get
            {
                base.RaiseExceptionIfNecessary();
                return result;
            }
        }
        
        // Hide from editor, since never used.
        [ Browsable(false), EditorBrowsable(EditorBrowsableState.Never) ]
        public new object UserState
        {
            get
            {
                return base.UserState;
            }
        }
    }
}

