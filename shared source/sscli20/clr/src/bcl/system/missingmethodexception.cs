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
** Class: MissingMethodException
**
**
** Purpose: The exception class for class loading failures.
**
**
=============================================================================*/

namespace System {
    
	using System;
	using System.Runtime.Remoting;
	using System.Runtime.Serialization;
	using System.Runtime.CompilerServices;
	using System.Globalization;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class MissingMethodException : MissingMemberException, ISerializable {
        public MissingMethodException() 
            : base(Environment.GetResourceString("Arg_MissingMethodException")) {
    		SetErrorCode(__HResults.COR_E_MISSINGMETHOD);
        }
    
        public MissingMethodException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MISSINGMETHOD);
        }
    
        public MissingMethodException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MISSINGMETHOD);
        }

        protected MissingMethodException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    
    	public override String Message
        {
    		get {
    			if (ClassName == null) {
    				return base.Message;
    			} else {
    				// do any desired fixups to classname here.
                    return String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("MissingMethod_Name",
                                                                       ClassName + "." + MemberName +
                                                                       (Signature != null ? " " + FormatSignature(Signature) : "")));
    			}
    		}
        }
    
        // Called from the EE
        private MissingMethodException(String className, String methodName, byte[] signature)
        {
            ClassName   = className;
            MemberName  = methodName;
            Signature   = signature;
        }
    
        public MissingMethodException(String className, String methodName)
        {
            ClassName   = className;
            MemberName  = methodName;
        }
    
        // If ClassName != null, Message will construct on the fly using it
        // and the other variables. This allows customization of the
        // format depending on the language environment.
    }
}
