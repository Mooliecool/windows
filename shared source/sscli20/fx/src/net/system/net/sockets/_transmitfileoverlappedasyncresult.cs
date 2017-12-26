//------------------------------------------------------------------------------
// <copyright file="_OverlappedAsyncResult.cs" company="Microsoft">
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
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Threading;
    using Microsoft.Win32;

    //
    //  OverlappedAsyncResult - used to take care of storage for async Socket operation
    //   from the BeginSend, BeginSendTo, BeginReceive, BeginReceiveFrom calls.
    //
    internal class TransmitFileOverlappedAsyncResult : BaseOverlappedAsyncResult {

        //
        // internal class members
        //

        private FileStream              m_fileStream;
        private TransmitFileOptions     m_flags;
        private TransmitFileBuffers     m_buffers;

        // Constructor. We take in the socket that's creating us, the caller's
        // state object, and the buffer on which the I/O will be performed.
        // We save the socket and state, pin the callers's buffer, and allocate
        // an event for the WaitHandle.
        //
        internal TransmitFileOverlappedAsyncResult(Socket socket, Object asyncState, AsyncCallback asyncCallback)
        : base(socket, asyncState, asyncCallback) {
        }

        internal TransmitFileOverlappedAsyncResult(Socket socket):base(socket){
        }


        //
        // SetUnmanagedStructures -
        // Fills in Overlapped Structures used in an Async Overlapped Winsock call
        //   these calls are outside the runtime and are unmanaged code, so we need
        //   to prepare specific structures and ints that lie in unmanaged memory
        //   since the Overlapped calls can be Async
        //
        internal void SetUnmanagedStructures(byte[] preBuffer, byte[] postBuffer, FileStream fileStream, TransmitFileOptions flags, bool sync) {
            //
            // fill in flags if we use it.
            //
            m_fileStream = fileStream;
            m_flags = flags;

            //
            // Fill in Buffer Array structure that will be used for our send/recv Buffer
            //
            m_buffers = null;
            int buffsNumber = 0;

            if (preBuffer != null && preBuffer.Length>0)
                ++buffsNumber;

            if (postBuffer != null && postBuffer.Length>0)
                ++buffsNumber;

            object[] objectsToPin = null;
            if (buffsNumber != 0)
            {
                ++buffsNumber;
                objectsToPin = new object[buffsNumber];

                m_buffers = new TransmitFileBuffers();
                
                objectsToPin[--buffsNumber] = m_buffers;

                if (preBuffer != null && preBuffer.Length>0) {
                    m_buffers.preBufferLength = preBuffer.Length;
                    objectsToPin[--buffsNumber] = preBuffer;
                }

                if (postBuffer != null && postBuffer.Length>0) {
                    m_buffers.postBufferLength = postBuffer.Length;
                    objectsToPin[--buffsNumber] = postBuffer;
                }

                if (sync)
                {
                    base.PinUnmanagedObjects(objectsToPin);
                }
                else
                {
                    base.SetUnmanagedStructures(objectsToPin);
                }

                if (preBuffer != null && preBuffer.Length > 0)
                {
                    m_buffers.preBuffer = Marshal.UnsafeAddrOfPinnedArrayElement(preBuffer, 0);
                }

                if (postBuffer != null && postBuffer.Length > 0)
                {
                    m_buffers.postBuffer = Marshal.UnsafeAddrOfPinnedArrayElement(postBuffer, 0);
                }
            }
            else if (!sync)
            {
                base.SetUnmanagedStructures(null);
            }

        } // SetUnmanagedStructures()

        internal void SetUnmanagedStructures(byte[] preBuffer, byte[] postBuffer, FileStream fileStream, TransmitFileOptions flags, ref OverlappedCache overlappedCache)
        {
            SetupCache(ref overlappedCache);
            SetUnmanagedStructures(preBuffer, postBuffer, fileStream, flags, false);
        }

        // Utility cleanup routine. Frees pinned and unmanged memory.
        //
        protected override void ForceReleaseUnmanagedStructures() {
            if (m_fileStream != null ) {
                m_fileStream.Close();
                m_fileStream = null;
            }
            //
            // clenaup base class
            //
            base.ForceReleaseUnmanagedStructures();

        } // CleanupUnmanagedStructures()

        internal void SyncReleaseUnmanagedStructures()
        {
            ForceReleaseUnmanagedStructures();
        }

        internal TransmitFileBuffers TransmitFileBuffers{
            get{
                return m_buffers;
            }
        }

        internal TransmitFileOptions Flags{
            get{
                return m_flags;
            }
        }


    }; // class OverlappedAsyncResult




} // namespace System.Net.Sockets
