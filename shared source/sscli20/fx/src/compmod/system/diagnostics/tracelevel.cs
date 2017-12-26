//------------------------------------------------------------------------------
// <copyright file="TraceLevel.cs" company="Microsoft">
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
    using System.Diagnostics;

    using System;

    /// <devdoc>
    ///    <para>Specifies what messages to output for debugging
    ///       and tracing.</para>
    /// </devdoc>
    public enum TraceLevel {
        /// <devdoc>
        ///    <para>
        ///       Output no tracing and debugging
        ///       messages.
        ///    </para>
        /// </devdoc>
        Off     = 0,
        /// <devdoc>
        ///    <para>
        ///       Output error-handling messages.
        ///    </para>
        /// </devdoc>
        Error   = 1,
        /// <devdoc>
        ///    <para>
        ///       Output warnings and error-handling
        ///       messages.
        ///    </para>
        /// </devdoc>
        Warning = 2,
        /// <devdoc>
        ///    <para>
        ///       Output informational messages, warnings, and error-handling messages.
        ///    </para>
        /// </devdoc>
        Info    = 3,
        /// <devdoc>
        ///    Output all debugging and tracing messages.
        /// </devdoc>
        Verbose = 4,
    }

}
