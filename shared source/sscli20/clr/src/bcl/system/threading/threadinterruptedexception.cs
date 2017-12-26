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
** Class: ThreadInterruptedException
**
**
** Purpose: An exception class to indicate that the thread was interrupted
**          from a waiting state.
**
**
=============================================================================*/
namespace System.Threading {
	using System.Threading;
	using System;
	using System.Runtime.Serialization;

    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ThreadInterruptedException : SystemException {
        public ThreadInterruptedException() 
	        : base(GetMessageFromNativeResources(ExceptionMessageKind.ThreadInterrupted)) {
    		SetErrorCode(__HResults.COR_E_THREADINTERRUPTED);
        }
    	
        public ThreadInterruptedException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_THREADINTERRUPTED);
        }
    
        public ThreadInterruptedException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_THREADINTERRUPTED);
        }

        protected ThreadInterruptedException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}
