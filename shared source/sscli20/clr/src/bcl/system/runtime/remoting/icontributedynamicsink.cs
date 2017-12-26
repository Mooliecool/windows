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
//
//   The IContributeDynamicSink interface is implemented by properties
//   that are registered at run-time through the RemotingServices.
//   RegisterDynamicProperty API. These properties can contribute sinks
//   that are notified when remoting calls start/finish. 
//
//   See also RemotingServices.RegisterDynamicProperty API.
//
namespace System.Runtime.Remoting.Contexts {
    using System.Runtime.Remoting;
    using System.Runtime.Remoting.Messaging;
    using System.Security.Permissions;
    using System;
    /// <internalonly/>
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IContributeDynamicSink
    {
	/// <internalonly/>
       //   Returns the message sink that will be notified of call start/finish events
       //   through the IDynamicMessageSink interface.
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        IDynamicMessageSink GetDynamicSink();
    }
}
