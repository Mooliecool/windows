//------------------------------------------------------------------------------
// <copyright file="IExtenderProviderService.cs" company="Microsoft">
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
    ///    <para>Provides an interface to add and remove extender providers.</para>
    /// </devdoc>
    public interface IExtenderProviderService {

        /// <devdoc>
        ///    <para>
        ///       Adds an extender provider.
        ///    </para>
        /// </devdoc>
        void AddExtenderProvider(IExtenderProvider provider);

        /// <devdoc>
        ///    <para>
        ///       Removes
        ///       an extender provider.
        ///    </para>
        /// </devdoc>
        void RemoveExtenderProvider(IExtenderProvider provider);
    }
}

