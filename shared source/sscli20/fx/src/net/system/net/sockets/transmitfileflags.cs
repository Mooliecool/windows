//------------------------------------------------------------------------------
// <copyright file="SocketFlags.cs" company="Microsoft">
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

namespace System.Net.Sockets {
    using System;

    /// <devdoc>
    ///    <para>
    ///       Provides constant values for socket messages.
    ///    </para>
    /// </devdoc>
    //UEUE

    [Flags] 
    public enum TransmitFileOptions
    {
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>

        UseDefaultWorkerThread=  0x00,
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>
        Disconnect =       0x01,
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>
        ReuseSocket=     0x02,
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>
        WriteBehind=     0x04,
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>
        UseSystemThread=  0x10,
        /// <devdoc>
        ///    <para>
        ///       Use no flags for this call.
        ///    </para>
        /// </devdoc>
        UseKernelApc=     0x20,
    };
} // namespace System.Net.Sockets
