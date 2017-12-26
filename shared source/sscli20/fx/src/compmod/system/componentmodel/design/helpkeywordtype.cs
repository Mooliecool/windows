//------------------------------------------------------------------------------
// <copyright file="HelpKeywordType.cs" company="Microsoft">
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
    using System.Runtime.Remoting;
    using System.ComponentModel;

    using System.Diagnostics;
    using System;

    /// <devdoc>
    ///    <para>
    ///       Specifies identifiers that can be
    ///       used to indicate the type of a help keyword.
    ///    </para>
    /// </devdoc>
    public enum HelpKeywordType {
        /// <devdoc>
        ///    <para>
        ///       Indicates the keyword is a word F1 was pressed to request help regarding.
        ///    </para>
        /// </devdoc>
        F1Keyword,
        /// <devdoc>
        ///    <para>
        ///       Indicates the keyword is a general keyword.
        ///    </para>
        /// </devdoc>
        GeneralKeyword,
        /// <devdoc>
        ///    <para>
        ///       Indicates the keyword is a filter keyword.
        ///    </para>
        /// </devdoc>
        FilterKeyword
    }
}
