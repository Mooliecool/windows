//------------------------------------------------------------------------------
// <copyright file="CollectionChangeAction.cs" company="Microsoft">
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

namespace System.ComponentModel {
    using System.ComponentModel;

    using System.Diagnostics;

    using System;

    /// <devdoc>
    ///    <para>Specifies how the collection is changed.</para>
    /// </devdoc>
    public enum CollectionChangeAction {
        /// <devdoc>
        ///    <para> Specifies that an element is added to the collection.</para>
        /// </devdoc>
        Add = 1,

        /// <devdoc>
        ///    <para>Specifies that an element is removed from the collection.</para>
        /// </devdoc>
        Remove = 2,

        /// <devdoc>
        ///    <para>Specifies that the entire collection has changed.</para>
        /// </devdoc>
        Refresh = 3
    }
}
