//------------------------------------------------------------------------------
// <copyright file="IWebRequestCreate.cs" company="Microsoft">
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
    using System;

    //
    // IWebRequestCreate - Interface for creating WebRequests.
    //
    /// <devdoc>
    ///    <para>
    ///       The <see cref='System.Net.IWebRequestCreate'/> interface is used by the <see cref='System.Net.WebRequest'/>
    ///       class to create <see cref='System.Net.WebRequest'/>
    ///       instances for a registered scheme.
    ///    </para>
    /// </devdoc>
    public interface IWebRequestCreate {
        /// <devdoc>
        ///    <para>
        ///       Creates a <see cref='System.Net.WebRequest'/>
        ///       instance.
        ///    </para>
        /// </devdoc>
        WebRequest Create(Uri uri);

    } // interface IWebRequestCreate

} // namespace System.Net
