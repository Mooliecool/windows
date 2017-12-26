//------------------------------------------------------------------------------
// <copyright file="URIFormatException.cs" company="Microsoft">
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

namespace System {
    using System.Runtime.Serialization;
    /// <devdoc>
    ///    <para>
    ///       An exception class used when an invalid Uniform Resource Identifier is detected.
    ///    </para>
    /// </devdoc>
    [Serializable]
    public class UriFormatException : FormatException, ISerializable {

        // constructors

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public UriFormatException() : base() {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public UriFormatException(string textString) : base(textString) {
        }

        protected UriFormatException(SerializationInfo serializationInfo, StreamingContext streamingContext)
            : base(serializationInfo, streamingContext) {
        }

        /// <internalonly/>
        void ISerializable.GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext) {
            base.GetObjectData(serializationInfo, streamingContext);
        }

        // accessors

        // methods

        // data

    }; // class UriFormatException


} // namespace System
