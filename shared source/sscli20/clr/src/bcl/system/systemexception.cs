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
namespace System {
 
	using System;
	using System.Runtime.Serialization;
    [Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
	public class SystemException : Exception
    {
        public SystemException() 
            : base(Environment.GetResourceString("Arg_SystemException")) {
    		SetErrorCode(__HResults.COR_E_SYSTEM);
        }
        
        public SystemException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_SYSTEM);
        }
    	
        public SystemException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_SYSTEM);
        }

        protected SystemException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
