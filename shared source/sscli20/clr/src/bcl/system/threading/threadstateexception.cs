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
** Class: ThreadStateException
**
**
** Purpose: An exception class to indicate that the Thread class is in an
**          invalid state for the method.
**
**
=============================================================================*/

namespace System.Threading {
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ThreadStateException : SystemException {
    	public ThreadStateException() 
	        : base(Environment.GetResourceString("Arg_ThreadStateException")) {
    		SetErrorCode(__HResults.COR_E_THREADSTATE);
        }
    
        public ThreadStateException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_THREADSTATE);
        }
    	
        public ThreadStateException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_THREADSTATE);
        }
        
        protected ThreadStateException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }

}
