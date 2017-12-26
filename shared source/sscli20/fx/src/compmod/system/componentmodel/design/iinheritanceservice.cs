//------------------------------------------------------------------------------
// <copyright file="IInheritanceService.cs" company="Microsoft">
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
    ///    <para>Provides a set of utilities
    ///       for analyzing and identifying inherited components.</para>
    /// </devdoc>
    public interface IInheritanceService {
    
        /// <devdoc>
        ///    <para>
        ///       Adds inherited components from the specified component to the specified container.
        ///    </para>
        /// </devdoc>
        void AddInheritedComponents(IComponent component, IContainer container);
        
        /// <devdoc>
        ///    <para>
        ///       Gets the inheritance attribute of the specified
        ///       component. If the component is not being inherited, this method will return the
        ///       value <see cref='System.ComponentModel.InheritanceAttribute.NotInherited'/>. 
        ///       Otherwise it will return the inheritance attribute for this component.      
        ///    </para>
        /// </devdoc>
        InheritanceAttribute GetInheritanceAttribute(IComponent component);
    }
}

