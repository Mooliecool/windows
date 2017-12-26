//------------------------------------------------------------------------------
// <copyright file="BindingDirection.cs" company="Microsoft">
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

    /// <devdoc>
    /// <para>Specifies whether the template can be bound one-way or two-way.</para>
    /// </devdoc>
    public enum BindingDirection {
        
        /// <devdoc>
        /// <para>The template can only accept property values.  Used with a generic ITemplate.</para>
        /// </devdoc>
        OneWay = 0,
        
        /// <devdoc>
        /// <para>The template can accept and expose property values.  Used with an IBindableTemplate.</para>
        /// </devdoc>
        TwoWay = 1
    }
}
