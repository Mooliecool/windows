//------------------------------------------------------------------------------
// <copyright file="PropertyChangedEventHandler.cs" company="Microsoft">
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
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Represents the method that will handle the
    ///    <see langword='PropertyChanged'/> event raised when a
    ///       property is changed on a component.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public delegate void PropertyChangedEventHandler(object sender, PropertyChangedEventArgs e);
}
