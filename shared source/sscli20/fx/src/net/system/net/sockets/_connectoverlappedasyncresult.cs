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
    //  ConnectOverlappedAsyncResult - used to take care of storage for async Socket BeginAccept call.
    //
    internal class ConnectOverlappedAsyncResult : BaseOverlappedAsyncResult {

        internal ConnectOverlappedAsyncResult(Socket socket, Object asyncState, AsyncCallback asyncCallback):
            base(socket,asyncState,asyncCallback)
        {
        }



        //
        // This method is called by base.CompletionPortCallback base.OverlappedCallback as part of IO completion
        //
        internal override object PostCompletion(int numBytes) {
            SocketError errorCode = (SocketError)ErrorCode;
            Socket socket = (Socket)AsyncObject;

            if (errorCode==SocketError.Success) {

                //set the socket context
                try
                {
                    errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                        socket.SafeHandle,
                        SocketOptionLevel.Socket,
                        SocketOptionName.UpdateConnectContext,
                        null,
                        0);
                    if (errorCode == SocketError.SocketError) errorCode = (SocketError) Marshal.GetLastWin32Error();
                }
                catch (ObjectDisposedException)
                {
                    errorCode = SocketError.OperationAborted;
                }

                ErrorCode = (int) errorCode;
            }

            if (errorCode==SocketError.Success) {
                socket.SetToConnected();
                return socket;
            }
            return null;
        }

    }


}
