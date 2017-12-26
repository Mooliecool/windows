//------------------------------------------------------------------------------
// <copyright file="IReferenceService.cs" company="Microsoft">
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
    using System.ComponentModel;

    using System.Diagnostics;
    using System;

    /// <devdoc>
    ///    <para>
    ///       Provides an interface to get names and references to objects. These
    ///       methods can search using the specified name or reference.
    ///    </para>
    /// </devdoc>
    public interface IReferenceService {
        
        /// <devdoc>
        ///    <para>
        ///       Gets the base component that anchors this reference.
        ///    </para>
        /// </devdoc>
        IComponent GetComponent(object reference);

        /// <devdoc>
        ///    <para>
        ///       Gets a reference for the specified name.
        ///    </para>
        /// </devdoc>
        object GetReference(string name);
    
        /// <devdoc>
        ///    <para>
        ///       Gets the name for this reference.
        ///    </para>
        /// </devdoc>
        string GetName(object reference);
    
        /// <devdoc>
        ///    <para>
        ///       Gets all available references.
        ///    </para>
        /// </devdoc>
        object[] GetReferences();
    
        /// <devdoc>
        ///    <para>
        ///       Gets all available references of this type.
        ///    </para>
        /// </devdoc>
        object[] GetReferences(Type baseType);
    }
}
