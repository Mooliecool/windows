//------------------------------------------------------------------------------
// <copyright file="NetworkAccess.cs" company="Microsoft">
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

namespace System.Net {
    /// <devdoc>
    ///    <para>
    ///       Defines network access permissions.
    ///    </para>
    /// </devdoc>
    [FlagsAttribute]
    public  enum    NetworkAccess {
        /// <devdoc>
        ///    <para>
        ///       An application is allowed to accept connections from the Internet.
        ///    </para>
        /// </devdoc>
        Accept  = 0x80,
        /// <devdoc>
        ///    <para>
        ///       An application is allowed to connect to Internet resources.
        ///    </para>
        /// </devdoc>
        Connect = 0x40

    } // enum NetworkAccess

} // namespace System.Net
