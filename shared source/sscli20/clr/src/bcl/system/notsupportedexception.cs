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
** Class: NotSupportedException
**
**
** Purpose: For methods that should be implemented on subclasses.
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class NotSupportedException : SystemException
    {
    	public NotSupportedException() 
            : base(Environment.GetResourceString("Arg_NotSupportedException")) {
    		SetErrorCode(__HResults.COR_E_NOTSUPPORTED);
        }
    
        public NotSupportedException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_NOTSUPPORTED);
        }
    	
        public NotSupportedException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_NOTSUPPORTED);
        }

        protected NotSupportedException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
