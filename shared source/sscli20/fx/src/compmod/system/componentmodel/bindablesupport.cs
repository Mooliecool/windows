//------------------------------------------------------------------------------
// <copyright file="BindableSupport.cs" company="Microsoft">
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
namespace System.ComponentModel {
    

    using System.Diagnostics;
    using System;

    /// <devdoc>
    ///    <para>Specifies which values to say if property or event value can be bound to a data
    ///          element or another property or event's value.</para>
    /// </devdoc>
    public enum BindableSupport {
        /// <devdoc>
        ///    <para>
        ///       The property or event is bindable.
        ///    </para>
        /// </devdoc>
        No        = 0x00,
        /// <devdoc>
        ///    <para>
        ///       The property or event is not bindable.
        ///    </para>
        /// </devdoc>
        Yes = 0x01,
        /// <devdoc>
        ///    <para>
        ///       The property or event is the default.
        ///    </para>
        /// </devdoc>
        Default        = 0x02,
    }
}
