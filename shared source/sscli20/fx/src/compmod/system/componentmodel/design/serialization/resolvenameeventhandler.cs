//------------------------------------------------------------------------------
// <copyright file="ResolveNameEventHandler.cs" company="Microsoft">
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
    using System.Security.Permissions;
    /// <devdoc>
    ///     This delegate is used to resolve object names when performing
    ///     serialization and deserialization.
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public delegate void ResolveNameEventHandler(object sender, ResolveNameEventArgs e);
}

