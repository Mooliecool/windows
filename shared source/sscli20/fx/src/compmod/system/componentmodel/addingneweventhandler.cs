//------------------------------------------------------------------------------
// <copyright file="AddingNewEventHandler.cs" company="Microsoft">
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
    using System.Security.Permissions;

    /// <devdoc>
    ///     Represents the method that will handle the AddingNew event on a list,
    ///     and provide the new object to be added to the list.
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public delegate void AddingNewEventHandler(object sender, AddingNewEventArgs e);
}
