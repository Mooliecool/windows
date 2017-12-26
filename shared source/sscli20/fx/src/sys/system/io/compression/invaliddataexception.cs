//------------------------------------------------------------------------------
// <copyright file="InvalidDataException.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.IO {
    
    using System;
    using System.Runtime.Serialization;

    [Serializable()] public sealed class InvalidDataException  : SystemException
    {
        public InvalidDataException () 
            : base(SR.GetString(SR.GenericInvalidData)) {
        }

        public InvalidDataException (String message) 
            : base(message) {
        }
    
        public InvalidDataException (String message, Exception innerException) 
            : base(message, innerException) {
        }
    
        internal InvalidDataException (SerializationInfo info, StreamingContext context) : base(info, context) {
        }

    }
}
