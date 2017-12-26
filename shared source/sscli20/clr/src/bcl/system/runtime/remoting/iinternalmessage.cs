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
** File:    IInternalMessage.cs
**
**
** Purpose: Defines an interface that allows kitchen sink data to be 
**          set and retrieved from the various kinds of message objects.
**          
**
**
===========================================================*/

namespace System.Runtime.Remoting.Messaging {
	using System.Runtime.Remoting;
	using System.Security.Permissions;
	using System;
    internal interface IInternalMessage
    {
        ServerIdentity ServerIdentityObject
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     set; 
	}
        Identity IdentityObject
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
	     set;
	}
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        void SetURI(String uri);     
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 			
        void SetCallContext(LogicalCallContext callContext);

        // The following should return true, if the property object hasn't
        //   been created.
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)] 		
        bool HasProperties();
    }
}
