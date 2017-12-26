//------------------------------------------------------------------------------
// <copyright file="IExtenderProvider.cs" company="Microsoft">
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
    

    using System.Diagnostics;

    using System;

    /// <devdoc>
    ///    <para>
    ///       Defines the interface
    ///       for extending properties to other components in a container.
    ///    </para>
    /// </devdoc>
    public interface IExtenderProvider {

        /// <devdoc>
        ///    <para>
        ///       Specifies
        ///       whether this object can provide its extender properties to
        ///       the specified object.
        ///    </para>
        /// </devdoc>
        bool CanExtend(object extendee);
    }
}
