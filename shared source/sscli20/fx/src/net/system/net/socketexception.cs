//------------------------------------------------------------------------------
// <copyright file="SocketException.cs" company="Microsoft">
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

namespace System.Net.Sockets {
    using System;
    using System.ComponentModel;
    using System.Runtime.Serialization;
    using System.Runtime.InteropServices;

    /// <devdoc>
    ///    <para>
    ///       Provides socket exceptions to the application.
    ///    </para>
    /// </devdoc>
    [Serializable]
    public class SocketException : Win32Exception {
        /// <devdoc>
        ///    <para>
        ///       Creates a new instance of the <see cref='System.Net.Sockets.SocketException'/> class with the default error code.
        ///    </para>
        /// </devdoc>
        public SocketException() : base(Marshal.GetLastWin32Error()) {
            GlobalLog.Print("SocketException::.ctor() " + NativeErrorCode.ToString() + ":" + Message);
        }

        /// <devdoc>
        ///    <para>
        ///       Creates a new instance of the <see cref='System.Net.Sockets.SocketException'/> class with the specified error code.
        ///    </para>
        /// </devdoc>
        public SocketException(int errorCode) : base(errorCode) {
            GlobalLog.Print("SocketException::.ctor(int) " + NativeErrorCode.ToString() + ":" + Message);
        }

        /// <devdoc>
        ///    <para>
        ///       Creates a new instance of the <see cref='System.Net.Sockets.SocketException'/> class with the specified error code as SocketError.
        ///    </para>
        /// </devdoc>
        internal SocketException(SocketError socketError) : base((int)socketError) {
        }

        protected SocketException(SerializationInfo serializationInfo, StreamingContext streamingContext)
            : base(serializationInfo, streamingContext) {
            GlobalLog.Print("SocketException::.ctor(serialized) " + NativeErrorCode.ToString() + ":" + Message);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override int ErrorCode {
            //
            // the base class returns the HResult with this property
            // we need the Win32 Error Code, hence the override.
            //
            get {
                return NativeErrorCode;
            }
        }

        public SocketError SocketErrorCode {
            //
            // the base class returns the HResult with this property
            // we need the Win32 Error Code, hence the override.
            //
            get {
                return (SocketError)NativeErrorCode;
            }
        }


    }; // class SocketException
    

} // namespace System.Net
