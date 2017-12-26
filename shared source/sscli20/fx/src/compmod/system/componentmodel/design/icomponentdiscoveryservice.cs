//------------------------------------------------------------------------------
// <copyright file="IComponentDiscoveryService.cs" company="Microsoft">
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

namespace System.ComponentModel.Design {
    using System;
    using System.Collections;
    using System.ComponentModel.Design;

    /// <devdoc>
    /// This service allows design-time enumeration of components across the toolbox
    /// and other available types at design-time.
    /// </devdoc>
    public interface IComponentDiscoveryService {

        /// <devdoc>
        ///     Retrieves the list of available component types, i.e. types implementing
        ///     IComponent. If baseType is null, all components are retrieved; otherwise
        ///     only component types derived from the specified baseType are returned.
        /// </devdoc>    
        ICollection GetComponentTypes(IDesignerHost designerHost, Type baseType);
    }
}
