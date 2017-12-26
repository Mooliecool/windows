//------------------------------------------------------------------------------
// <copyright file="InvalidAsynchronousStateException.cs" company="Microsoft">
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

namespace System.ComponentModel {
    using Microsoft.Win32;
    using System;
    using System.Diagnostics;
    using System.Globalization;
    using System.Runtime.Serialization;
    using System.Security.Permissions;
    
    /// <devdoc>
    ///    <para>The exception that is thrown when a thread that an operation should execute on no longer exists or is not pumping messages</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    [Serializable]
    public class InvalidAsynchronousStateException : ArgumentException {

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.InvalidAsynchronousStateException'/> class without a message.</para>
        /// </devdoc>
        public InvalidAsynchronousStateException() : this(null) {
        }

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.InvalidAsynchronousStateException'/> class with 
        ///    the specified message.</para>
        /// </devdoc>
        public InvalidAsynchronousStateException(string message)
            : base(message) {
        }

        /// <devdoc>
        ///     Initializes a new instance of the Exception class with a specified error message and a 
        ///     reference to the inner exception that is the cause of this exception.
        /// </devdoc>
        public InvalidAsynchronousStateException(string message, Exception innerException ) 
            : base(message, innerException) {
        }
        /// <devdoc>
        ///     Need this constructor since Exception implements ISerializable. We don't have any fields,
        ///     so just forward this to base.
        /// </devdoc>
        protected InvalidAsynchronousStateException(SerializationInfo info, StreamingContext context) : base(info, context) {
        }
    }
}
