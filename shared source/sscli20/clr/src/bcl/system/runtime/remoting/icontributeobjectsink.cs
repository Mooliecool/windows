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
//  The IContributeObjectSink interface is implemented by 
//  context properties in a Context that wish to contribute 
//  an object specific interception sink on the server end of 
//  a remoting call.
//
namespace System.Runtime.Remoting.Contexts {

    using System.Runtime.InteropServices;
    using System.Runtime.Remoting.Messaging;   
    using System.Security.Permissions;
    using System;
    /// <internalonly/>
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IContributeObjectSink
    {
	/// <internalonly/>
        // Chain your message sink in front of the chain formed thus far and 
        // return the composite sink chain.
        // 
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        IMessageSink GetObjectSink(MarshalByRefObject obj, 
                                          IMessageSink nextSink);
    }
}
