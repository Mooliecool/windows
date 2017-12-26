//------------------------------------------------------------------------------
// <copyright file="_NestedMultipleAsyncResult.cs" company="Microsoft">
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

namespace System.Net {

    //
    // The NestedAsyncResult - used to wrap async requests
    //      this is used to hold another async result made
    //      through a call to another Begin call within.
    //
    internal class NestedMultipleAsyncResult : LazyAsyncResult {
        //
        // this is usually for operations on streams/buffers,
        // we save information passed in on the Begin call:
        // since some calls might need several completions, we
        // need to save state on the user's IO request
        //
        internal BufferOffsetSize[] Buffers;
        internal int Size;

        //
        // Constructor:
        //
        internal NestedMultipleAsyncResult(Object asyncObject, Object asyncState, AsyncCallback asyncCallback, BufferOffsetSize[] buffers)
        : base( asyncObject, asyncState, asyncCallback ) {
            Buffers = buffers;
            Size = 0;
            for (int i = 0; i < Buffers.Length; i++) {
                Size += Buffers[i].Size;
            }
        }

    }; // class NestedMultipleAsyncResult
} // namespace System.Net
