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
// CustomAttributeFormatException is thrown when the binary format of a 
//	custom attribute is invalid.
//
// Date: March 98
//
namespace System.Reflection {
	using System;
	using ApplicationException = System.ApplicationException;
	using System.Runtime.Serialization;
	[Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public class CustomAttributeFormatException  : FormatException {
    
        public CustomAttributeFormatException()
	        : base(Environment.GetResourceString("Arg_CustomAttributeFormatException")) {
    		SetErrorCode(__HResults.COR_E_CUSTOMATTRIBUTEFORMAT);
        }
    
        public CustomAttributeFormatException(String message) : base(message) {
    		SetErrorCode(__HResults.COR_E_CUSTOMATTRIBUTEFORMAT);
        }
    	
        public CustomAttributeFormatException(String message, Exception inner) : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_CUSTOMATTRIBUTEFORMAT);
        }

        protected CustomAttributeFormatException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
