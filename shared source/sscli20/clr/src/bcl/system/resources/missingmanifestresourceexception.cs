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
** Class:  MissingManifestResourceException
**
**
** Purpose: Exception for a missing assembly-level resource 
**
**
===========================================================*/

using System;
using System.Runtime.Serialization;

namespace System.Resources {
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public class MissingManifestResourceException : SystemException
    {
        public MissingManifestResourceException() 
            : base(Environment.GetResourceString("Arg_MissingManifestResourceException")) {
    		SetErrorCode(__HResults.COR_E_MISSINGMANIFESTRESOURCE);
        }
        
        public MissingManifestResourceException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MISSINGMANIFESTRESOURCE);
        }
    	
        public MissingManifestResourceException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MISSINGMANIFESTRESOURCE);
        }

        protected MissingManifestResourceException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}
