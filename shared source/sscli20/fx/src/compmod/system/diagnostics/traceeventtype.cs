//------------------------------------------------------------------------------
// <copyright file="TraceEventType.cs" company="Microsoft">
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

using System;
using System.ComponentModel;

namespace System.Diagnostics {
    public enum TraceEventType {
        Critical    = 0x01,
        Error       = 0x02,
        Warning     = 0x04,
        Information = 0x08,
        Verbose     = 0x10,

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        Start       = 0x0100,
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        Stop        = 0x0200,
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        Suspend     = 0x0400,
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        Resume      = 0x0800,
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        Transfer    = 0x1000,
    }
}

