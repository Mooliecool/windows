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
** Class: ArgumentNullException
**
**
** Purpose: Exception class for null arguments to a method.
**
**
=============================================================================*/

namespace System {
    
    using System;
    using System.Runtime.Serialization;
    using System.Runtime.Remoting;
    using System.Security.Permissions;
    
    // The ArgumentException is thrown when an argument 
    // is null when it shouldn't be.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class ArgumentNullException : ArgumentException
    {
        // Creates a new ArgumentNullException with its message 
        // string set to a default message explaining an argument was null.
       public ArgumentNullException() 
            : base(Environment.GetResourceString("ArgumentNull_Generic")) {
                // Use E_POINTER - COM used that for null pointers.  Description is "invalid pointer"
                SetErrorCode(__HResults.E_POINTER);
        }

        public ArgumentNullException(String paramName) 
            : base(Environment.GetResourceString("ArgumentNull_Generic"), paramName) {
            SetErrorCode(__HResults.E_POINTER);
        }

        public ArgumentNullException(String message, Exception innerException) 
            : base(message, innerException) {
            SetErrorCode(__HResults.E_POINTER);
        }
            
        public ArgumentNullException(String paramName, String message) 
            : base(message, paramName) {
            SetErrorCode(__HResults.E_POINTER);   
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        protected ArgumentNullException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
