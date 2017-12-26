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
** Class: ArithmeticException
**
**
** Purpose: Exception class for bad arithmetic conditions!
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;
    // The ArithmeticException is thrown when overflow or underflow
    // occurs.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class ArithmeticException : SystemException
    {    	
        // Creates a new ArithmeticException with its message string set to
        // the empty string, its HRESULT set to COR_E_ARITHMETIC, 
        // and its ExceptionInfo reference set to null. 
        public ArithmeticException() 
            : base(Environment.GetResourceString("Arg_ArithmeticException")) {
    		SetErrorCode(__HResults.COR_E_ARITHMETIC);
        }
    	
        // Creates a new ArithmeticException with its message string set to
        // message, its HRESULT set to COR_E_ARITHMETIC, 
        // and its ExceptionInfo reference set to null. 
        // 
        public ArithmeticException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_ARITHMETIC);
        }
    	
        public ArithmeticException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_ARITHMETIC);
        }

        protected ArithmeticException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
