//------------------------------------------------------------------------------
// <copyright file="RequestCacheManager.cs" company="Microsoft">
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

//
// The file contains two streams used in conjunction with caching.
// The first class will combine two streams for reading into just one continues stream.
// The second class will forward (as writes) to external stream all reads issued on a "this" stream.
// 

namespace System.Net {

    [Flags]
    internal enum CloseExState {
        Normal  = 0x0,          // just a close
        Abort   = 0x1,          // unconditionaly release resources
        Silent  = 0x2           // do not throw on close if possible
    }
    internal interface ICloseEx {
        void CloseEx(CloseExState closeState);
    }
}
