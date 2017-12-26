//------------------------------------------------------------------------------
// <copyright file="ProviderException.cs" company="Microsoft">
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

namespace System.Configuration.Provider {
    using  System.Collections.Specialized;
    using  System.Runtime.Serialization;

    [Serializable]
    public class ProviderException : Exception
    {
        public ProviderException() {}

        public ProviderException( string message )
            : base( message )
        {}

        public ProviderException( string message, Exception innerException )
            : base( message, innerException )
        {}

        protected ProviderException( SerializationInfo info, StreamingContext context )
            : base( info, context )
        {}
    }
}
