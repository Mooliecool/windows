//------------------------------------------------------------------------------
// <copyright file="ITypeDescriptorFilterService.cs" company="Microsoft">
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
    using System.Runtime.InteropServices;
    using System.Diagnostics;
    using System;
    using System.Collections;
    using System.ComponentModel;

    /// <devdoc>
    ///    <para>
    ///       Modifies the set of type descriptors that a component
    ///       provides.
    ///    </para>
    /// </devdoc>
    public interface ITypeDescriptorFilterService {

        /// <devdoc>
        ///    <para>
        ///       Provides a way to filter the attributes from a component that are displayed to the user.
        ///    </para>
        /// </devdoc>
        bool FilterAttributes(IComponent component, IDictionary attributes);

        /// <devdoc>
        ///    <para>
        ///       Provides a way to filter the events from a component that are displayed to the user.
        ///    </para>
        /// </devdoc>
        bool FilterEvents(IComponent component, IDictionary events);

        /// <devdoc>
        ///    <para>
        ///       Provides a way to filter the properties from a component that are displayed to the user.
        ///    </para>
        /// </devdoc>
        bool FilterProperties(IComponent component, IDictionary properties);
    }
}

