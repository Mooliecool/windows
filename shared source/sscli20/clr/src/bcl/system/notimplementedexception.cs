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
/*=============================================================================
**
** Class: NotImplementedException
**
**
** Purpose: Exception thrown when a requested method or operation is not 
**			implemented.
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class NotImplementedException : SystemException
    {
    	public NotImplementedException() 
            : base(Environment.GetResourceString("Arg_NotImplementedException")) {
    		SetErrorCode(__HResults.E_NOTIMPL);
        }
        public NotImplementedException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.E_NOTIMPL);
        }
        public NotImplementedException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.E_NOTIMPL);
        }

        protected NotImplementedException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
