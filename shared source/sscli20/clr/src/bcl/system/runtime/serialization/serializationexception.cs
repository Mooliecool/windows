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
** Class: SerializationException
**
**
** Purpose: Thrown when something goes wrong during serialization or 
**          deserialization.
**
**
=============================================================================*/

namespace System.Runtime.Serialization {
    
	using System;
	using System.Runtime.Serialization;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class SerializationException : SystemException {
    	
    	private static String _nullMessage = Environment.GetResourceString("Arg_SerializationException");
    	
        // Creates a new SerializationException with its message 
        // string set to a default message.
        public SerializationException() 
            : base(_nullMessage) {
    		SetErrorCode(__HResults.COR_E_SERIALIZATION);
        }
    	
        public SerializationException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_SERIALIZATION);
        }

        public SerializationException(String message, Exception innerException) : base (message, innerException) {
    		SetErrorCode(__HResults.COR_E_SERIALIZATION);
        }

        protected SerializationException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}
