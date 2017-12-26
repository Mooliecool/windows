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
** Class: PlatformNotSupportedException
**
**
** Purpose: To handle features that don't run on particular platforms
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Serialization;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class PlatformNotSupportedException : NotSupportedException
    {
    	public PlatformNotSupportedException() 
            : base(Environment.GetResourceString("Arg_PlatformNotSupported")) {
    		SetErrorCode(__HResults.COR_E_PLATFORMNOTSUPPORTED);
        }
    
        public PlatformNotSupportedException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_PLATFORMNOTSUPPORTED);
        }
    	
        public PlatformNotSupportedException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_PLATFORMNOTSUPPORTED);
        }

        protected PlatformNotSupportedException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
