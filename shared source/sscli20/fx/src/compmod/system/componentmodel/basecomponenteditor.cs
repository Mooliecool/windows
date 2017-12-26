//------------------------------------------------------------------------------
// <copyright file="BaseComponentEditor.cs" company="Microsoft">
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
    using System;
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para> Provides the base class for a custom component 
    ///       editor.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public abstract class ComponentEditor
    {
    
        /// <devdoc>
        ///    <para>Gets a value indicating whether the component was modified.</para>
        /// </devdoc>
        public bool EditComponent(object component) {
            return EditComponent(null, component);
        }
    
        /// <devdoc>
        ///    <para>Gets a value indicating whether the component was modified.</para>
        /// </devdoc>
        public abstract bool EditComponent(ITypeDescriptorContext context, object component);
    }
}
