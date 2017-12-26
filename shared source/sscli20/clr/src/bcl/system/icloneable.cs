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
** Class:  ICloneable
**
** This interface is implemented by classes that support cloning.
**
===========================================================*/
namespace System {
    
	using System;
    // Defines an interface indicating that an object may be cloned.  Only objects 
    // that implement ICloneable may be cloned. The interface defines a single 
    // method which is called to create a clone of the object.   Object defines a method
    // MemberwiseClone to support default clone operations.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ICloneable
    {
		// Interface does not need to be marked with the serializable attribute
        // Make a new object which is a copy of the object instanced.  This object may be either
        // deep copy or a shallow copy depending on the implementation of clone.  The default
        // Object support for clone does a shallow copy.
        // 
    	Object Clone();
    }
}
