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
namespace System {
    
    using System;
    using System.Runtime.Serialization;
    using System.Security.Permissions;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class NotFiniteNumberException : ArithmeticException {
        private double _offendingNumber;	
    
        public NotFiniteNumberException() 
            : base(Environment.GetResourceString("Arg_NotFiniteNumberException")) {
            _offendingNumber = 0;
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }

        public NotFiniteNumberException(double offendingNumber) 
            : base() {
            _offendingNumber = offendingNumber;
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }

        public NotFiniteNumberException(String message) 
            : base(message) {
            _offendingNumber = 0;
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }

        public NotFiniteNumberException(String message, double offendingNumber) 
            : base(message) {
            _offendingNumber = offendingNumber;
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }

        public NotFiniteNumberException(String message, Exception innerException) 
            : base(message, innerException) {
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }
        
        public NotFiniteNumberException(String message, double offendingNumber, Exception innerException) 
            : base(message, innerException) {
            _offendingNumber = offendingNumber;
            SetErrorCode(__HResults.COR_E_NOTFINITENUMBER);
        }

        protected NotFiniteNumberException(SerializationInfo info, StreamingContext context) : base(info, context) {
            _offendingNumber = info.GetInt32("OffendingNumber");
        }

        public double OffendingNumber {
            get { return _offendingNumber; }
        }

	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }
            base.GetObjectData(info, context);
            info.AddValue("OffendingNumber", _offendingNumber, typeof(Int32));
        }
    }
}
