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
// MulticastNotSupportedException
// This is thrown when you add multiple callbacks to a non-multicast delegate.
////////////////////////////////////////////////////////////////////////////////

namespace System {
    
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public sealed class MulticastNotSupportedException : SystemException {
    	
        public MulticastNotSupportedException() 
            : base(Environment.GetResourceString("Arg_MulticastNotSupportedException")) {
    		SetErrorCode(__HResults.COR_E_MULTICASTNOTSUPPORTED);
        }
    
        public MulticastNotSupportedException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MULTICASTNOTSUPPORTED);
        }
    	
    	public MulticastNotSupportedException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MULTICASTNOTSUPPORTED);
        }

        internal MulticastNotSupportedException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
