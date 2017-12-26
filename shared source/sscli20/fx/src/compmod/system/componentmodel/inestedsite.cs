//------------------------------------------------------------------------------
// <copyright file="INestedSite.cs" company="Microsoft">
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
    ///     Nested containers site objects using INestedSite.  A nested
    ///     site is simply a site with an additional property that can
    ///     retrieve the full nested name of a component.
    /// </devdoc>
    public interface INestedSite : ISite {

        /// <devdoc>
        ///     Returns the full name of the component in this site in the format
        ///     of <owner>.<component>.  If this component's site has a null
        ///     name, FullName also returns null.
        /// </devdoc>
        string FullName { get; }
    }
}
