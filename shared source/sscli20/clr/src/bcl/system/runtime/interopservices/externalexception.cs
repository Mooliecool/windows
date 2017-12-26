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
** Class: ExternalException
**
**
** Purpose: Exception base class for all errors from Interop or Structured 
**          Exception Handling code.
**
**
=============================================================================*/

namespace System.Runtime.InteropServices {

	using System;
	using System.Runtime.Serialization;
    // Base exception for COM Interop errors &; Structured Exception Handler
    // exceptions.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ExternalException : SystemException {
        public ExternalException() 
            : base(Environment.GetResourceString("Arg_ExternalException")) {
    		SetErrorCode(__HResults.E_FAIL);
        }
    	
        public ExternalException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.E_FAIL);
        }
    	
        public ExternalException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.E_FAIL);
        }

		public ExternalException(String message,int errorCode) 
            : base(message) {
    		SetErrorCode(errorCode);
        }

        protected ExternalException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

		public virtual int ErrorCode {
    		get { return HResult; }
        }
    }
}
