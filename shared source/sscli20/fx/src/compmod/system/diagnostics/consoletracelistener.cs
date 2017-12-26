//------------------------------------------------------------------------------
// <copyright file="TextWriterTraceListener.cs" company="Microsoft">
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

/*
 */
namespace System.Diagnostics {
    using System;
    using System.IO;
    using System.Security.Permissions;
    using Microsoft.Win32;

    [HostProtection(Synchronization=true)]
    public class ConsoleTraceListener : TextWriterTraceListener {

        public ConsoleTraceListener() : base (Console.Out) {
        }

        public ConsoleTraceListener(bool useErrorStream) : base (useErrorStream ? Console.Error : Console.Out) {
        }
    }
}
 

        
