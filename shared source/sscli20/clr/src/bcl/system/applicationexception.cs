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
** Class: ApplicationException
**
**
** Purpose: The base class for all "less serious" exceptions that must be
**          declared or caught.
**
**
=============================================================================*/

namespace System {
    
	using System.Runtime.Serialization;
    // The ApplicationException is the base class for nonfatal, 
    // application errors that occur.  These exceptions are generated 
    // (i.e., thrown) by an application, not the Runtime. Applications that need 
    // to create their own exceptions do so by extending this class. 
    // ApplicationException extends but adds no new functionality to 
    // RecoverableException.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ApplicationException : Exception {
    	
        // Creates a new ApplicationException with its message string set to
        // the empty string, its HRESULT set to COR_E_APPLICATION, 
        // and its ExceptionInfo reference set to null. 
        public ApplicationException() 
            : base(Environment.GetResourceString("Arg_ApplicationException")) {
    		SetErrorCode(__HResults.COR_E_APPLICATION);
        }
    	
        // Creates a new ApplicationException with its message string set to
        // message, its HRESULT set to COR_E_APPLICATION, 
        // and its ExceptionInfo reference set to null. 
        // 
        public ApplicationException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_APPLICATION);
        }
    	
        public ApplicationException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_APPLICATION);
        }

        protected ApplicationException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
