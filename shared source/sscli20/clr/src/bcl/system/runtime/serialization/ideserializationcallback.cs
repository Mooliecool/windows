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
** Interface: IDeserializationEventListener
**
**
** Purpose: Implemented by any class that wants to indicate that
**          it wishes to receive deserialization events.
**
**
===========================================================*/
namespace System.Runtime.Serialization {
	using System;

    // Interface does not need to be marked with the serializable attribute
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IDeserializationCallback {
        void OnDeserialization(Object sender);
    
    }
}
