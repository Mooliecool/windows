//------------------------------------------------------------------------------
// <copyright file="IDataErrorInfo.cs" company="Microsoft">
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

    /// <devdoc>
    /// </devdoc>
    // suppose that you have some data that can be indexed by use of string:
    // then there are two types of errors:
    // 1. an error for each piece of data that can be indexed
    // 2. an error that is valid on the entire data
    //
    public interface IDataErrorInfo {

        /// <devdoc>
        /// </devdoc>
        string this[string columnName] {
            get;
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        string Error {
            get;
        }
    }
}
