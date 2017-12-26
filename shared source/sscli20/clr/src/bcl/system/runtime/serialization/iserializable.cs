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
** Interface: ISerializable
**
**
** Purpose: Implemented by any object that needs to control its
**          own serialization.
**
**
===========================================================*/

namespace System.Runtime.Serialization {
	using System.Runtime.Remoting;
	using System.Runtime.Serialization;
	using System.Security.Permissions;
	using System;
	using System.Reflection;
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISerializable {
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        void GetObjectData(SerializationInfo info, StreamingContext context);
    }

}




