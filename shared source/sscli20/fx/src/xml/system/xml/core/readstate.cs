//------------------------------------------------------------------------------
// <copyright file="ReadState.cs" company="Microsoft">
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

namespace System.Xml
{
    // Specifies the state of the XmlReader.
    public enum ReadState
    {
        // The Read method has not been called yet.
        Initial      = 0,

        // Reading is in progress.
        Interactive  = 1,

        // An error occurred that prevents the XmlReader from continuing.
        Error        = 2,

        // The end of the stream has been reached successfully.
        EndOfFile    = 3,

        // The Close method has been called and the XmlReader is closed.
        Closed        = 4
    }
}
