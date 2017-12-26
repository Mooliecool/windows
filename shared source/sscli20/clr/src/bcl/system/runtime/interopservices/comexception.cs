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
** Class: COMException
**
**
** Purpose: Exception class for all errors from COM Interop where we don't
** recognize the HResult.
**
**
=============================================================================*/

namespace System.Runtime.InteropServices {
	using System.Runtime.InteropServices;
	using System;
	using System.Runtime.Serialization;
	using System.Globalization;
    // Exception for COM Interop errors where we don't recognize the HResult.
    // 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class COMException : ExternalException {
        public COMException() 
            : base(Environment.GetResourceString("Arg_COMException"))
        {
    		SetErrorCode(__HResults.E_FAIL);
        }
    	
        public COMException(String message) 
            : base(message)
        {
    		SetErrorCode(__HResults.E_FAIL);
        }
    	
        public COMException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.E_FAIL);
        }
    	
    	public COMException(String message,int errorCode) 
            : base(message) {
    		SetErrorCode(errorCode);
        }
        
        protected COMException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

        public override String ToString() {
            String message = Message;
            String s;
            String _className = GetType().ToString();
            s = _className + " (0x" + HResult.ToString("X8", CultureInfo.InvariantCulture) + ")";

            if (!(message == null || message.Length <= 0)) {
                s = s + ": " + message;
            }

            Exception _innerException = InnerException;

            if (_innerException!=null) {
                s = s + " ---> " + _innerException.ToString();
            }


            if (StackTrace != null)
                s += Environment.NewLine + StackTrace;

            return s;
        }


    }
}
