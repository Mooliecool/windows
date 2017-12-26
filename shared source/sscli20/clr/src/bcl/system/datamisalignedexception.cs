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
** Class: DataMisalignedException
**
** Purpose: The exception class for a misaligned access exception
**
=============================================================================*/

namespace System 
{
    using System;
    using System.Runtime.Serialization;

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class DataMisalignedException : SystemException 
    {
        public DataMisalignedException() 
            : base(Environment.GetResourceString("Arg_DataMisalignedException")) 
        {
            SetErrorCode(__HResults.COR_E_DATAMISALIGNED);
        }
    
        public DataMisalignedException(String message) 
            : base(message) 
        {
            SetErrorCode(__HResults.COR_E_DATAMISALIGNED);
        }

        public DataMisalignedException(String message, Exception innerException) 
            : base(message, innerException) 
        {
            SetErrorCode(__HResults.COR_E_DATAMISALIGNED);
        }

        internal DataMisalignedException(SerializationInfo info, StreamingContext context) 
            : base (info, context) 
        {
        }
    }

}
