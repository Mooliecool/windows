//------------------------------------------------------------------------------
// <copyright file="IDesignerLoaderHost.cs" company="Microsoft">
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

namespace System.ComponentModel.Design.Serialization {

    using System;
    using System.Collections;
    using System.ComponentModel.Design;

    /// <devdoc>
    ///     IDesignerLoaderHost.  This is an extension of IDesignerHost that is passed
    ///     to the designer loader in the BeginLoad method.  It is isolated from
    ///     IDesignerHost to emphasize that all loading and reloading of the design
    ///     document actually should be initiated by the designer loader, and not by
    ///     the designer host.  However, the loader must inform the designer host that
    ///     it wishes to invoke a load or reload.
    /// </devdoc>
    public interface IDesignerLoaderHost : IDesignerHost {
    
        /// <devdoc>
        ///     This is called by the designer loader to indicate that the load has 
        ///     terminated.  If there were errors, they should be passed in the errorCollection
        ///     as a collection of exceptions (if they are not exceptions the designer
        ///     loader host may just call ToString on them).  If the load was successful then
        ///     errorCollection should either be null or contain an empty collection.
        /// </devdoc>
        void EndLoad(string baseClassName, bool successful, ICollection errorCollection);
    
        /// <devdoc>
        ///     This is called by the designer loader when it wishes to reload the
        ///     design document.  The reload will happen immediately so the caller
        ///     should ensure that it is in a state where BeginLoad may be called again.
        /// </devdoc>
        void Reload();
    }
}

