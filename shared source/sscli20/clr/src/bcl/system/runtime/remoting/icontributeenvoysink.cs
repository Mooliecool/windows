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
//  The IContributeEnvoySink interface is implemented by properties
//  in the ServerContext that contribute sinks to the serverContext
//  and serverObject chains. The sinks contributed through this 
//  interface are expected to act as Envoys for the corresponding
//  property sinks in the ServerContext and ServerObject chains.
//
namespace System.Runtime.Remoting.Contexts {

    using System.Runtime.InteropServices;
    using System.Runtime.Remoting.Messaging;
    using System.Security.Permissions;
    using System;
    /// <internalonly/>
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IContributeEnvoySink
    {
	/// <internalonly/>
        //  Chain your message sink in front of the chain formed thus far and 
        //  return the composite sink chain. This method is used when creating
        //  the sink chain for X-Context cases. The server object is provided
        //  for the interest of object-specific 
        // 
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        IMessageSink GetEnvoySink(MarshalByRefObject obj, IMessageSink nextSink);
    }
}
