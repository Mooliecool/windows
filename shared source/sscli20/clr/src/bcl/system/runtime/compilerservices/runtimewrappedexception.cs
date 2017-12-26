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
** Class: RuntimeWrappedException
**
**
** Purpose: The exception class uses to wrap all non-CLS compliant exceptions.
**
**
=============================================================================*/

namespace System.Runtime.CompilerServices {
    using System;
    using System.Runtime.Serialization;
    using System.Runtime.Remoting;
    using System.Security.Permissions;
        
    [Serializable()] 
    public sealed class RuntimeWrappedException : Exception
    {
        private RuntimeWrappedException(Object thrownObject)
            : base(Environment.GetResourceString("RuntimeWrappedException")) {
            SetErrorCode(System.__HResults.COR_E_RUNTIMEWRAPPED);
            m_wrappedException = thrownObject;
        }
    
        public Object WrappedException {
            get { return m_wrappedException; }
        }

        private Object m_wrappedException;

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }
            base.GetObjectData(info, context);
            info.AddValue("WrappedException", m_wrappedException, typeof(Object));
        }

        internal RuntimeWrappedException(SerializationInfo info, StreamingContext context)
            : base(info, context) {
            m_wrappedException = info.GetValue("WrappedException", typeof(Object));
        }
    }
}

