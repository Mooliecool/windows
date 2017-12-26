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
** Class: TypeInitializationException
**
**
** Purpose: The exception class to wrap exceptions thrown by
**          a type's class initializer (.cctor).  This is sufficiently
**          distinct from a TypeLoadException, which means we couldn't
**          find the type.
**
**
=============================================================================*/
using System;
using System.Runtime.Serialization;
using System.Globalization;
using System.Security.Permissions;

namespace System {
	[Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class TypeInitializationException : SystemException {
        private String _typeName;

		// This exception is not creatable without specifying the
		//	inner exception.
    	private TypeInitializationException()
	        : base(Environment.GetResourceString("TypeInitialization_Default")) {
    		SetErrorCode(__HResults.COR_E_TYPEINITIALIZATION);
    	}

		// This is called from within the runtime.  I believe this is necessary
        // for Interop only, though it's not particularly useful.
        private TypeInitializationException(String message) : base(message) {
    		SetErrorCode(__HResults.COR_E_TYPEINITIALIZATION);
        }
    	
        public TypeInitializationException(String fullTypeName, Exception innerException) : base(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("TypeInitialization_Type"), fullTypeName), innerException) {
            _typeName = fullTypeName;
    		SetErrorCode(__HResults.COR_E_TYPEINITIALIZATION);
        }

        internal TypeInitializationException(SerializationInfo info, StreamingContext context) : base(info, context) {
            _typeName = info.GetString("TypeName");
        }

        public String TypeName
        {
            get {
                if (_typeName == null) {
                    return String.Empty;
                }
                return _typeName;
            }
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            base.GetObjectData(info, context);
            info.AddValue("TypeName",TypeName,typeof(String));
        }

    }
}
