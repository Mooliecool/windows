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
////////////////////////////////////////////////////////////////////////////////
//
// TargetParameterCountException is thrown when the number of parameter to an
//	invocation doesn't match the number expected.
//
// 
// 
//
namespace System.Reflection {

	using System;
	using SystemException = System.SystemException;
	using System.Runtime.Serialization;
	[Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class TargetParameterCountException : ApplicationException {
    	
        public TargetParameterCountException()
	        : base(Environment.GetResourceString("Arg_TargetParameterCountException")) {
    		SetErrorCode(__HResults.COR_E_TARGETPARAMCOUNT);
        }
    
        public TargetParameterCountException(String message) 
			: base(message) {
    		SetErrorCode(__HResults.COR_E_TARGETPARAMCOUNT);
        }
    	
        public TargetParameterCountException(String message, Exception inner)  
			: base(message, inner) {
    		SetErrorCode(__HResults.COR_E_TARGETPARAMCOUNT);
        }

        internal TargetParameterCountException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}
