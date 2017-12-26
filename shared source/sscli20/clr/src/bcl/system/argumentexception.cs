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
** Class: ArgumentException
**
**
** Purpose: Exception class for invalid arguments to a method.
**
**
=============================================================================*/

namespace System {
    
    using System;
    using System.Runtime.Remoting;
    using System.Runtime.Serialization;
    using System.Globalization;
    using System.Security.Permissions;
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public class ArgumentException : SystemException, ISerializable {
        private String m_paramName;
        
        // Creates a new ArgumentException with its message 
        // string set to the empty string. 
        public ArgumentException() 
            : base(Environment.GetResourceString("Arg_ArgumentException")) {
            SetErrorCode(__HResults.COR_E_ARGUMENT);
        }
        
        // Creates a new ArgumentException with its message 
        // string set to message. 
        // 
        public ArgumentException(String message) 
            : base(message) {
            SetErrorCode(__HResults.COR_E_ARGUMENT);
        }
        
        public ArgumentException(String message, Exception innerException) 
            : base(message, innerException) {
            SetErrorCode(__HResults.COR_E_ARGUMENT);
        }

        public ArgumentException(String message, String paramName, Exception innerException) 
            : base(message, innerException) {
            m_paramName = paramName;
            SetErrorCode(__HResults.COR_E_ARGUMENT);
        }
        
        public ArgumentException (String message, String paramName)
        
            : base (message) {
            m_paramName = paramName;
            SetErrorCode(__HResults.COR_E_ARGUMENT);
        }

        protected ArgumentException(SerializationInfo info, StreamingContext context) : base(info, context) {
            m_paramName = info.GetString("ParamName");
        }
        
        public override String Message
        {
            get {
                String s = base.Message;
                if (! ((m_paramName == null) ||
                       (m_paramName.Length == 0)) )
                    return s + Environment.NewLine + String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Arg_ParamName_Name"), m_paramName);
                else
                    return s;
            }
        }
                
        public virtual String ParamName {
            get { return m_paramName; }
        }
    
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }
            base.GetObjectData(info, context);
            info.AddValue("ParamName", m_paramName, typeof(String));
        }
    }
}
