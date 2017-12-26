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
** Class: SemaphoreFullException
**
**
=============================================================================*/
namespace System.Threading {
    using System;
    using System.Runtime.Serialization;
    using System.Runtime.InteropServices;

    [Serializable()]
    [ComVisibleAttribute(false)]
    public class SemaphoreFullException : SystemException {
    
        public SemaphoreFullException() : base(SR.GetString(SR.Threading_SemaphoreFullException)){
        }
    
        public SemaphoreFullException(String message) : base(message) {
        }

        public SemaphoreFullException(String message, Exception innerException) : base(message, innerException) {
        }
        
        protected SemaphoreFullException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}

