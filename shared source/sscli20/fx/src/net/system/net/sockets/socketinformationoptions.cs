//------------------------------------------------------------------------------
// <copyright file="SocketInformationOptions.cs" company="Microsoft">
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


    [Flags]
    public enum SocketInformationOptions{
        NonBlocking          = 0x1,
        //Even though getpeername can give a hint that we're connected, this needs to be passed because 
        //disconnect doesn't update getpeername to return a failure.
        Connected         = 0x2,  
        Listening         = 0x4,
        UseOnlyOverlappedIO = 0x8,
    }
}
