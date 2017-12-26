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
** Class: MarshalDirectiveException
**
** Purpose: This exception is thrown when the marshaller encounters a signature
**          that has an invalid MarshalAs CA for a given argument or is not
**          supported.
**
=============================================================================*/

namespace System.Runtime.InteropServices {

	using System;
	using System.Runtime.Serialization;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class MarshalDirectiveException : SystemException {
        public MarshalDirectiveException() 
            : base(Environment.GetResourceString("Arg_MarshalDirectiveException")) {
    		SetErrorCode(__HResults.COR_E_MARSHALDIRECTIVE);
        }
    
        public MarshalDirectiveException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MARSHALDIRECTIVE);
        }
    
        public MarshalDirectiveException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MARSHALDIRECTIVE);
        }

        protected MarshalDirectiveException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
