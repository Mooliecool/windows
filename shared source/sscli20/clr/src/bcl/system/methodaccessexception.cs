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
** Class: MethodAccessException
**
** Purpose: The exception class for class loading failures.
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class MethodAccessException : MemberAccessException {
        public MethodAccessException() 
            : base(Environment.GetResourceString("Arg_MethodAccessException")) {
    		SetErrorCode(__HResults.COR_E_METHODACCESS);
        }
    
        public MethodAccessException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_METHODACCESS);
        }
    
        public MethodAccessException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_METHODACCESS);
        }

        protected MethodAccessException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
