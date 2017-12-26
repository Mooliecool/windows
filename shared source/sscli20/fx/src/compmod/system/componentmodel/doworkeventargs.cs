//------------------------------------------------------------------------------
// <copyright file="DoWorkEventArgs.cs" company="Microsoft">
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
    public class DoWorkEventArgs : CancelEventArgs
    {
        private object result;
        private object argument;

        public DoWorkEventArgs(object argument)
        {
            this.argument = argument;
        }

        [ SRDescription(SR.BackgroundWorker_DoWorkEventArgs_Argument) ]
        public object Argument
        {
            get { return argument; }
        }

        [ SRDescription(SR.BackgroundWorker_DoWorkEventArgs_Result) ]
        public object Result
        {
            get { return result; }
            set { result = value; }
        }
    }
}

