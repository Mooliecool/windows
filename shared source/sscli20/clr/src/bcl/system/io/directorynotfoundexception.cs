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
** Class:  DirectoryNotFoundException
**
**
** Purpose: Exception for accessing a path that doesn't exist.
**
**
===========================================================*/
using System;
using System.Runtime.Serialization;

namespace System.IO {
    /*
     * Thrown when trying to access a directory that doesn't exist on disk.
     * From COM Interop, this exception is thrown for 2 HRESULTS: 
     * the Win32 errorcode-as-HRESULT ERROR_PATH_NOT_FOUND (0x80070003) 
     * and STG_E_PATHNOTFOUND (0x80030003).
     */
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public class DirectoryNotFoundException : IOException {
        public DirectoryNotFoundException() 
            : base(Environment.GetResourceString("Arg_DirectoryNotFoundException")) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
    
        public DirectoryNotFoundException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
    
        public DirectoryNotFoundException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
        
        protected DirectoryNotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
