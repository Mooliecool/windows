//------------------------------------------------------------------------------
// <copyright file="IResourceService.cs" company="Microsoft">
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

    using System.Globalization;
    using System.Resources;

    /// <devdoc>
    ///    <para> 
    ///       Provides designers a way to
    ///       access a resource for the current design-time
    ///       object.</para>
    /// </devdoc>
    public interface IResourceService {
    
        /// <devdoc>
        ///    <para> 
        ///       Locates the resource reader for the specified culture and
        ///       returns it.</para>
        /// </devdoc>
        IResourceReader GetResourceReader(CultureInfo info);
    
        /// <devdoc>
        ///    <para>Locates the resource writer for the specified culture
        ///       and returns it. This will create a new resource for
        ///       the specified culture and destroy any existing resource,
        ///       should it exist.</para>
        /// </devdoc>
        IResourceWriter GetResourceWriter(CultureInfo info);
    }
}

