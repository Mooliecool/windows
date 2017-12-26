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

// WARNING:
//
// This is just an IObjectReference proxy for the former V1.1 Surrogate Encoder
// All this does is make an encoder of the correct type, it DOES NOT maintain state.
namespace System.Text
{
    using System;
    using System.Runtime.Serialization;
    using System.Security.Permissions;

    /*=================================SurrogateEncoder==================================
    ** This class is here only to deserialize the SurrogateEncoder class from Everett (V1.1) into
    ** Appropriate Whidbey (V2.0) objects.
    ==============================================================================*/

    [Serializable()]
    internal sealed class SurrogateEncoder : ISerializable, IObjectReference
    {
        // Might need this when GetRealObjecting
        [NonSerialized]
        private Encoding realEncoding = null;

        // Constructor called by serialization.
        internal SurrogateEncoder(SerializationInfo info, StreamingContext context)
        {
            // Any info?
            if (info==null) throw new ArgumentNullException("info");

            // All versions have a code page
            this.realEncoding = (Encoding)info.GetValue("m_encoding", typeof(Encoding));
        }

        // Just get it from GetEncoding
        public Object GetRealObject(StreamingContext context)
        {
            // Need to get our Encoding's Encoder
            return this.realEncoding.GetEncoder();
        }

        // ISerializable implementation
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
        {
            // We cannot ever call this.
            BCLDebug.Assert(false, "Didn't expect to make it to SurrogateEncoder.GetObjectData");
            throw new ArgumentException(Environment.GetResourceString("Arg_ExecutionEngineException"));
        }
    }
}

