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
** Class: InvalidOleVariantTypeException
**
** Purpose: The type of an OLE variant that was passed into the runtime is 
**			invalid.
**
=============================================================================*/

namespace System.Runtime.InteropServices {
    
    using System;
	using System.Runtime.Serialization;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class InvalidOleVariantTypeException : SystemException {
        public InvalidOleVariantTypeException() 
            : base(Environment.GetResourceString("Arg_InvalidOleVariantTypeException")) {
    		SetErrorCode(__HResults.COR_E_INVALIDOLEVARIANTTYPE);
        }
    
        public InvalidOleVariantTypeException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_INVALIDOLEVARIANTTYPE);
        }
    
        public InvalidOleVariantTypeException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_INVALIDOLEVARIANTTYPE);
        }

        protected InvalidOleVariantTypeException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
