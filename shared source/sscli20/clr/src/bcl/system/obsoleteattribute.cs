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
** Class:  ObsoleteAttribute
**
**
** Purpose: Attribute for functions, etc that will be removed.
**
** 
===========================================================*/
namespace System {
    
	using System;
	using System.Runtime.Remoting;
    // This attribute is attached to members that are not to be used any longer.
    // Message is some human readable explanation of what to use
    // Error indicates if the compiler should treat usage of such a method as an
    //   error. (this would be used if the actual implementation of the obsolete 
    //   method's implementation had changed).
    // 
    [Serializable(), AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Enum |
        AttributeTargets.Interface | AttributeTargets.Constructor | AttributeTargets.Method| AttributeTargets.Property  | AttributeTargets.Field | AttributeTargets.Event | AttributeTargets.Delegate
        , Inherited = false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ObsoleteAttribute : Attribute
    {
    	private String _message;
    	private bool _error;
    	
    	public ObsoleteAttribute ()
    	{
    		_message = null;
    		_error = false;
    	}
    
    	public ObsoleteAttribute (String message)
    	{
    		_message = message;
    		_error = false;
    	}
    
    	public ObsoleteAttribute (String message, bool error)
    	{
            _message = message;
            _error = error;
    	}
    
        public String Message {
    		get {return _message;}
    	}
    	
        public bool IsError{
    		get {return _error;}
    	}
    	
    }
}
