//------------------------------------------------------------------------------
// <copyright file="CollectionChangeEventHandler.cs" company="Microsoft">
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
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Represents the method that will handle the 
    ///    <see langword='CollectionChanged '/>event raised when adding elements to or removing elements from a 
    ///       collection.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public delegate void CollectionChangeEventHandler(object sender, CollectionChangeEventArgs e);
}
