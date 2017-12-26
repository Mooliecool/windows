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
** Class: ContextMarshalException
**
**
** Purpose: Exception class for attempting to pass an instance through a context
**          boundary, when the formal type and the instance's marshal style are
**          incompatible.
**
**
=============================================================================*/

namespace System {
	using System.Runtime.InteropServices;
	using System.Runtime.Remoting;
	using System;
	using System.Runtime.Serialization;
    [Obsolete("ContextMarshalException is obsolete.")]
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ContextMarshalException : SystemException {
        public ContextMarshalException() 
            : base(Environment.GetResourceString("Arg_ContextMarshalException")) {
    		SetErrorCode(__HResults.COR_E_CONTEXTMARSHAL);
        }
    
        public ContextMarshalException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_CONTEXTMARSHAL);
        }
    	
        public ContextMarshalException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_CONTEXTMARSHAL);
        }

        protected ContextMarshalException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
