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
    //  DisconnectOverlappedAsyncResult - used to take care of storage for async Socket BeginAccept call.
    //
    internal class DisconnectOverlappedAsyncResult : BaseOverlappedAsyncResult {

        
        internal DisconnectOverlappedAsyncResult(Socket socket, Object asyncState, AsyncCallback asyncCallback): 
            base(socket,asyncState,asyncCallback)
        {
        }

         //
        // This method will be called by us when the IO completes synchronously and
        // by the ThreadPool when the IO completes asynchronously. (only called on WinNT)
        //

        internal override object PostCompletion(int numBytes) {
            if (ErrorCode == (int)SocketError.Success) {
                Socket socket = (Socket)AsyncObject;
                socket.SetToDisconnected();
                socket.m_RemoteEndPoint = null;
            }
            return base.PostCompletion(numBytes);
        }
    }
}
