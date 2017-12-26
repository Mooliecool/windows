// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
//+----------------------------------------------------------------------------
//
// Microsoft Windows
// File:        ISponsor.cs
//
// Contents:    Interface for Sponsors
//
// History:     1/5/00                                 Created
//
//+----------------------------------------------------------------------------

namespace System.Runtime.Remoting.Lifetime
{
    using System;
    using System.Security.Permissions;

[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISponsor
    {
	        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
		TimeSpan Renewal(ILease lease);
    }
} 
