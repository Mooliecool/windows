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
namespace System.Threading 
{
    using System;
    using System.Runtime.Serialization;
    using System.Runtime.InteropServices;

    [Serializable()]
    [ComVisibleAttribute(false)]
    public class WaitHandleCannotBeOpenedException : ApplicationException 
    {
        public WaitHandleCannotBeOpenedException() : base(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException")) 
        {
            SetErrorCode(__HResults.COR_E_WAITHANDLECANNOTBEOPENED);
        }
    
        public WaitHandleCannotBeOpenedException(String message) : base(message)
        {
            SetErrorCode(__HResults.COR_E_WAITHANDLECANNOTBEOPENED);
        }

        public WaitHandleCannotBeOpenedException(String message, Exception innerException) : base(message, innerException)
        {
            SetErrorCode(__HResults.COR_E_WAITHANDLECANNOTBEOPENED);
        }
        
        protected WaitHandleCannotBeOpenedException(SerializationInfo info, StreamingContext context) : base (info, context) 
        {
        }
    }
}

