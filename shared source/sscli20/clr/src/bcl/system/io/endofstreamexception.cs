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
/*============================================================
**
** Class:  EndOfStreamException
**
**
** Purpose: Exception to be thrown when reading past end-of-file.
**
**
===========================================================*/

using System;
using System.Runtime.Serialization;

namespace System.IO {
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class EndOfStreamException : IOException
    {
        public EndOfStreamException() 
            : base(Environment.GetResourceString("Arg_EndOfStreamException")) {
    		SetErrorCode(__HResults.COR_E_ENDOFSTREAM);
        }
        
        public EndOfStreamException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_ENDOFSTREAM);
        }
    	
        public EndOfStreamException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_ENDOFSTREAM);
        }

        protected EndOfStreamException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }

}
