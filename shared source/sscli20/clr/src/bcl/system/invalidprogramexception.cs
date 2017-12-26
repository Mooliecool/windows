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
** Class: InvalidProgramException
**
**
** Purpose: The exception class for programs with invalid IL or bad metadata.
**
**
=============================================================================*/

namespace System {

	using System;
	using System.Runtime.Serialization;
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class InvalidProgramException : SystemException {
        public InvalidProgramException() 
            : base(Environment.GetResourceString("InvalidProgram_Default")) {
    		SetErrorCode(__HResults.COR_E_INVALIDPROGRAM);
        }
    
        public InvalidProgramException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_INVALIDPROGRAM);
        }
    
        public InvalidProgramException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_INVALIDPROGRAM);
        }

        internal InvalidProgramException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }

}
