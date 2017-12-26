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
////////////////////////////////////////////////////////////////////////////////
// MemberAccessException
// Thrown when we try accessing a member that we cannot
// access, due to it being removed, private or something similar.
////////////////////////////////////////////////////////////////////////////////

namespace System {
    
	using System;
	using System.Runtime.Serialization;
    // The MemberAccessException is thrown when trying to access a class
    // member fails.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class MemberAccessException : SystemException {
    	
        // Creates a new MemberAccessException with its message string set to
        // the empty string, its HRESULT set to COR_E_MEMBERACCESS, 
        // and its ExceptionInfo reference set to null. 
    	public MemberAccessException() 
            : base(Environment.GetResourceString("Arg_AccessException")) {
    		SetErrorCode(__HResults.COR_E_MEMBERACCESS);
        }
    	
        // Creates a new MemberAccessException with its message string set to
        // message, its HRESULT set to COR_E_ACCESS, 
        // and its ExceptionInfo reference set to null. 
        // 
        public MemberAccessException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MEMBERACCESS);
        }
    	
        public MemberAccessException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MEMBERACCESS);
        }

        protected MemberAccessException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
