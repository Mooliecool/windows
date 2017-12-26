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
** Class: TimeoutException
**
**
** Purpose: Exception class for Timeout
**
**
=============================================================================*/

namespace System 
{
    using System.Runtime.Serialization;

    [Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public class TimeoutException : SystemException {
    	
        public TimeoutException() 
            : base(Environment.GetResourceString("Arg_TimeoutException")) {
            SetErrorCode(__HResults.COR_E_TIMEOUT);
        }
    
        public TimeoutException(String message) 
            : base(message) {
            SetErrorCode(__HResults.COR_E_TIMEOUT);
        }
        
        public TimeoutException(String message, Exception innerException)
            : base(message, innerException) {
            SetErrorCode(__HResults.COR_E_TIMEOUT);
        }
    
        //
        //This constructor is required for serialization.
        //
        protected TimeoutException(SerializationInfo info, StreamingContext context) 
            : base(info, context) {
        }
    }
}

