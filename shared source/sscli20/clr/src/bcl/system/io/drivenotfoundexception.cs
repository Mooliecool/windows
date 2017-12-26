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
//============================================================
//
//  Class:  DriveNotFoundException
//
//  Purpose: Exception for accessing a drive that is not available.
//
//
//============================================================
using System;
using System.Runtime.Serialization;

namespace System.IO {

    //Thrown when trying to access a drive that is not availabe.
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class DriveNotFoundException : IOException {
        public DriveNotFoundException() 
            : base(Environment.GetResourceString("Arg_DriveNotFoundException")) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
    
        public DriveNotFoundException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
    
        public DriveNotFoundException(String message, Exception innerException) 
            : base(message, innerException) {
    		SetErrorCode(__HResults.COR_E_DIRECTORYNOTFOUND);
        }
        
        protected DriveNotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
