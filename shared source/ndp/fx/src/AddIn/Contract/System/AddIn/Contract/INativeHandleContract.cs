//------------------------------------------------------------------------------
/// <copyright '2007' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Security.Permissions;

namespace System.AddIn.Contract
{
    /// <summary>
    /// Used for Windows handles.
    /// </summary>
    public interface INativeHandleContract : IContract
    {
        /// <summary>
        ///   Returns an untyped native handle.
        /// </summary>
        /// <returns>IntPtr</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2135:SecurityRuleSetLevel2MethodsShouldNotBeProtectedWithLinkDemandsFxCopRule")]
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        IntPtr GetHandle();
    }
}

