//------------------------------------------------------------------------------
// <copyright file="FieldDirection.cs" company="Microsoft">
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

namespace System.CodeDom {

    using System.Diagnostics;
    using System.Runtime.InteropServices;

    /// <devdoc>
    ///    <para>
    ///       Specifies values used to indicate field and parameter directions.
    ///    </para>
    /// </devdoc>
    [
        ComVisible(true),
        Serializable,
    ]
    public enum FieldDirection {
        /// <devdoc>
        ///    <para>
        ///       Incoming field.
        ///    </para>
        /// </devdoc>
        In,
        /// <devdoc>
        ///    <para>
        ///       Outgoing field.
        ///    </para>
        /// </devdoc>
        Out,
        /// <devdoc>
        ///    <para>
        ///       Field by reference.
        ///    </para>
        /// </devdoc>
        Ref,
    }
}
