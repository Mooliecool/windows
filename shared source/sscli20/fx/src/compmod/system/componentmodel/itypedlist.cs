//------------------------------------------------------------------------------
// <copyright file="ITypedList.cs" company="Microsoft">
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

    using System;
    using System.ComponentModel;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public interface ITypedList {
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        string GetListName(PropertyDescriptor[] listAccessors);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        PropertyDescriptorCollection GetItemProperties(PropertyDescriptor[] listAccessors);
    }
}
