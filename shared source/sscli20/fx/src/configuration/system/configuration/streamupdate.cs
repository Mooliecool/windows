//------------------------------------------------------------------------------
// <copyright file="StreamUpdate.cs" company="Microsoft">
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

namespace System.Configuration {

    //
    // Tracks updates to a stream.
    //
    internal class StreamUpdate {
        private string  _newStreamname;
        private bool    _writeCompleted;

        internal StreamUpdate(string newStreamname) {
            _newStreamname = newStreamname;
        }

        // desired new stream name
        internal string NewStreamname {
            get {return _newStreamname;}
        }

        // indicates whether the change from the old stream name
        // to the new stream name has been completed.
        internal bool WriteCompleted {
            get {return _writeCompleted;}
            set {_writeCompleted = value;}
        }
    }
}
