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
** Class: KeyNotFoundException
**
**
** Purpose: Exception class for Hashtable and Dictionary.
**
**
=============================================================================*/

namespace System.Collections.Generic {
    
    using System;
    using System.Runtime.Remoting;
    using System.Runtime.Serialization;

    [Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public class KeyNotFoundException  : SystemException, ISerializable {
    
        public KeyNotFoundException () 
            : base(Environment.GetResourceString("Arg_KeyNotFound")) {
            SetErrorCode(System.__HResults.COR_E_KEYNOTFOUND);
        }
        
        public KeyNotFoundException(String message) 
            : base(message) {
            SetErrorCode(System.__HResults.COR_E_KEYNOTFOUND);
        }
        
        public KeyNotFoundException(String message, Exception innerException) 
            : base(message, innerException) {
            SetErrorCode(System.__HResults.COR_E_KEYNOTFOUND);
        }


        protected KeyNotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }        
    }
}
