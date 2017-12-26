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
** Class: ArrayTypeMismatchException
**
**
** Purpose: The arrays are of different primitive types.
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ArrayTypeMismatchException : SystemException {
    	
        // Creates a new ArrayMismatchException with its message string set to
        // the empty string, its HRESULT set to COR_E_ARRAYTYPEMISMATCH, 
        // and its ExceptionInfo reference set to null. 
        public ArrayTypeMismatchException() 
            : base(Environment.GetResourceString("Arg_ArrayTypeMismatchException")) {
    		SetErrorCode(__HResults.COR_E_ARRAYTYPEMISMATCH);
        }
    	
        // Creates a new ArrayMismatchException with its message string set to
        // message, its HRESULT set to COR_E_ARRAYTYPEMISMATCH, 
        // and its ExceptionInfo reference set to null. 
        // 
        public ArrayTypeMismatchException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_ARRAYTYPEMISMATCH);
        }
    	
        public ArrayTypeMismatchException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_ARRAYTYPEMISMATCH);
        }

        protected ArrayTypeMismatchException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
