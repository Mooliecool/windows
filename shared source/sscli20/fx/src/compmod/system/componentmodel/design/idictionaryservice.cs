//------------------------------------------------------------------------------
// <copyright file="IDictionaryService.cs" company="Microsoft">
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
namespace System.ComponentModel.Design {
    using System.ComponentModel;

    using System.Diagnostics;
    
    using System;

    /// <devdoc>
    ///    <para>Provides a generic dictionary service that a designer can use
    ///       to store user-defined data on the site.</para>
    /// </devdoc>
    public interface IDictionaryService {
    
        /// <devdoc>
        ///    <para>
        ///       Gets the key corresponding to the specified value.
        ///    </para>
        /// </devdoc>
        object GetKey(object value);
        
        /// <devdoc>
        ///    <para>
        ///       Gets the value corresponding to the specified key.
        ///    </para>
        /// </devdoc>
        object GetValue(object key);
    
        /// <devdoc>
        ///    <para> 
        ///       Sets the specified key-value pair.</para>
        /// </devdoc>
        void SetValue(object key, object value);
    }
}
