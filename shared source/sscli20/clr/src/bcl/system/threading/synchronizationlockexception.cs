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
** Class: SynchronizationLockException
**
**
** Purpose: Wait(), Notify() or NotifyAll() was called from an unsynchronized
**          block of code.
**
**
=============================================================================*/

namespace System.Threading {

	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class SynchronizationLockException : SystemException {
        public SynchronizationLockException() 
            : base(Environment.GetResourceString("Arg_SynchronizationLockException")) {
    		SetErrorCode(__HResults.COR_E_SYNCHRONIZATIONLOCK);
        }
    
        public SynchronizationLockException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_SYNCHRONIZATIONLOCK);
        }
    
    	public SynchronizationLockException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_SYNCHRONIZATIONLOCK);
        }

        protected SynchronizationLockException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }

}


