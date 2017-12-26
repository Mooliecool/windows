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
/*============================================================
**
** File:    IMessageSink.cs
**
**
** Purpose: Defines the message sink interface
**
**
===========================================================*/
namespace System.Runtime.Remoting.Messaging {
	using System.Runtime.Remoting;
	using System.Security.Permissions;
	using System;
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IMessageSink
    {
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        IMessage     SyncProcessMessage(IMessage msg);

	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        IMessageCtrl AsyncProcessMessage(IMessage msg, IMessageSink replySink);

        // Retrieves the next message sink held by this sink.
        IMessageSink NextSink
	{
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
	    get;
	}
    }
}
