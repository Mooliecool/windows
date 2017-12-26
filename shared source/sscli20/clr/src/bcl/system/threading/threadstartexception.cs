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
    public sealed class ThreadStartException : SystemException 
    {
    	private ThreadStartException() 
            : base(Environment.GetResourceString("Arg_ThreadStartException")) 
        {
    		SetErrorCode(__HResults.COR_E_THREADSTART);
        }

        private ThreadStartException(Exception reason)
            : base(Environment.GetResourceString("Arg_ThreadStartException"), reason)
        {
            SetErrorCode(__HResults.COR_E_THREADSTART);
        }

        //required for serialization
        internal ThreadStartException(SerializationInfo info, StreamingContext context) 
            : base(info, context) 
        {
        }
   
    }
}


