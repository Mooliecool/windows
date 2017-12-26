//------------------------------------------------------------------------------
// <copyright file="_AcceptOverlappedAsyncResult.cs" company="Microsoft">
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
    using System.Net;
    using System.Runtime.InteropServices;
    using System.Threading;
    using Microsoft.Win32;

    //
    //  AcceptOverlappedAsyncResult - used to take care of storage for async Socket BeginAccept call.
    //
    internal class AcceptOverlappedAsyncResult : BaseOverlappedAsyncResult {

        //
        // internal class members
        //

        private int             m_LocalBytesTransferred;
        private Socket          m_ListenSocket;
        private Socket          m_AcceptSocket;

        private     int         m_AddressBufferLength;
        private     byte[]      m_Buffer;

        // Constructor. We take in the socket that's creating us, the caller's
        // state object, and the buffer on which the I/O will be performed.
        // We save the socket and state, pin the callers's buffer, and allocate
        // an event for the WaitHandle.
        //
        internal AcceptOverlappedAsyncResult(Socket listenSocket, Object asyncState, AsyncCallback asyncCallback) :
            base(listenSocket, asyncState, asyncCallback)
        {
            m_ListenSocket = listenSocket;
        }



        //
        // SetUnmanagedStructures -
        // Fills in Overlapped Structures used in an Async Overlapped Winsock call
        //   these calls are outside the runtime and are unmanaged code, so we need
        //   to prepare specific structures and ints that lie in unmanaged memory
        //   since the Overlapped calls can be Async
        //
        internal void SetUnmanagedStructures(byte[] buffer, int addressBufferLength) {

            // has to be called first to pin memory
            base.SetUnmanagedStructures(buffer);

            //
            // Fill in Buffer Array structure that will be used for our send/recv Buffer
            //
            m_AddressBufferLength = addressBufferLength;
            m_Buffer = buffer;
        }

        /*
        //                                    
        internal void SetUnmanagedStructures(byte[] buffer, int addressBufferLength, ref OverlappedCache overlappedCache)
        {
            SetupCache(ref overlappedCache);
            SetUnmanagedStructures(buffer, addressBufferLength);
        }
        */

        void LogBuffer(long size) {
            GlobalLog.Assert(Logging.On, "AcceptOverlappedAsyncResult#{0}::LogBuffer()|Logging is off!", ValidationHelper.HashString(this));
            IntPtr pinnedBuffer = Marshal.UnsafeAddrOfPinnedArrayElement(m_Buffer, 0);
            if (pinnedBuffer != IntPtr.Zero) {
                if (size > -1) {
                    Logging.Dump(Logging.Sockets, m_ListenSocket, "PostCompletion", pinnedBuffer, (int)Math.Min(size, (long)m_Buffer.Length));
                }
                else {
                    Logging.Dump(Logging.Sockets, m_ListenSocket, "PostCompletion", pinnedBuffer, (int)m_Buffer.Length);
                }
            }
        }

        internal byte[] Buffer {
            get {
                return m_Buffer;
            }
        }

        internal int BytesTransferred {
            get {
                return m_LocalBytesTransferred;
            }
        }

        internal Socket AcceptSocket
        {
            set
            {
                m_AcceptSocket = value;
            }
        }
    }
}
