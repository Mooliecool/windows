//------------------------------------------------------------------------------
// <copyright file="ProgressChangedEventArgs.cs" company="Microsoft">
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
    public class ProgressChangedEventArgs : EventArgs
    {
        private readonly int progressPercentage;
        private readonly object userState;

        public ProgressChangedEventArgs(int progressPercentage, object userState)
        {
            this.progressPercentage = progressPercentage;
            this.userState = userState;
        }
        
        [ SRDescription(SR.Async_ProgressChangedEventArgs_ProgressPercentage) ]
        public int ProgressPercentage
        {
            get { return progressPercentage; }
        }

        [ SRDescription(SR.Async_ProgressChangedEventArgs_UserState) ]
        public object UserState
        {
            get { return userState; }
        }

    }
}

