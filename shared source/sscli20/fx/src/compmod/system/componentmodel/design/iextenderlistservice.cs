//------------------------------------------------------------------------------
// <copyright file="IExtenderListService.cs" company="Microsoft">
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
    using System.Diagnostics;
    using System;
    using System.ComponentModel;
    using Microsoft.Win32;

    /// <devdoc>
    ///    <para>Provides an interface to list extender providers.</para>
    /// </devdoc>
    public interface IExtenderListService {

        /// <devdoc>
        ///    <para>Gets the set of extender providers for the component.</para>
        /// </devdoc>
        IExtenderProvider[] GetExtenderProviders();
    }

}
