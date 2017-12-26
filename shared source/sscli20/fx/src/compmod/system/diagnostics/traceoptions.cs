//------------------------------------------------------------------------------
// <copyright file="TraceOptions.cs" company="Microsoft">
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

namespace System.Diagnostics {

    [Flags]
    public enum TraceOptions {
        None =             0,
        LogicalOperationStack = 0x01,
        DateTime=       0x02,
        Timestamp=      0x04,
        ProcessId=      0x08,
        ThreadId=       0x10,
        Callstack=      0x20,
    }
}
