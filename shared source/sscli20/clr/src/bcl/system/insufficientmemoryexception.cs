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
** Class: InsufficientMemoryException
**
**
** Purpose: The exception class for running out of memory
** but most likely in a non-fatal way that shouldn't 
** be affected by escalation policy.  Use this for cases
** like MemoryFailPoint or a TryAllocate method, where you 
** expect OOM's with no shared state corruption and you
** want to recover from these errors.
**
**
=============================================================================*/

namespace System {
    
    using System;
    using System.Runtime.Serialization;

    [Serializable()]
    public sealed class InsufficientMemoryException : OutOfMemoryException
    {
        public InsufficientMemoryException() 
            : base(GetMessageFromNativeResources(ExceptionMessageKind.OutOfMemory)) {
    		SetErrorCode(__HResults.COR_E_INSUFFICIENTMEMORY);
        }
    
        public InsufficientMemoryException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_INSUFFICIENTMEMORY);
        }
    	
        public InsufficientMemoryException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_INSUFFICIENTMEMORY);
        }

        private InsufficientMemoryException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
