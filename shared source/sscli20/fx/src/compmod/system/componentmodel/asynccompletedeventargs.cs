//------------------------------------------------------------------------------
// <copyright file="AsyncCompletedEventArgs.cs" company="Microsoft">
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
    using System.Diagnostics.CodeAnalysis;
    using System.Security.Permissions;
    using System.Reflection;

    [HostProtection(SharedState = true)]
    public class AsyncCompletedEventArgs : System.EventArgs
    {
        private readonly Exception error;
        private readonly bool cancelled;
        private readonly object userState;
        
        public AsyncCompletedEventArgs(Exception error, bool cancelled, object userState)
        {
            this.error = error;
            this.cancelled = cancelled;
            this.userState = userState;
        }
        
        [ SRDescription(SR.Async_AsyncEventArgs_Cancelled) ]
        public bool Cancelled
        {
            get { return cancelled; }
        }
        
        [ SRDescription(SR.Async_AsyncEventArgs_Error) ]
        public Exception Error
        {
            get { return error; }
        }

        [ SRDescription(SR.Async_AsyncEventArgs_UserState) ]
        public object UserState
        {
            get { return userState; }
        }

        // Call from every result 'getter'. Will throw if there's an error or operation was cancelled
        //
        [SuppressMessage("Microsoft.Design", "CA1030:UseEventsWhereAppropriate")]
        protected void RaiseExceptionIfNecessary()
        {
            if (Error != null)
            {
                throw new TargetInvocationException(SR.GetString(SR.Async_ExceptionOccurred), Error);
            }
            else if (Cancelled)
            {
                throw new InvalidOperationException(SR.GetString(SR.Async_OperationCancelled));
            }
            
        }
        
    }
}
