//------------------------------------------------------------------------------
// <copyright file="TraceFilter.cs" company="Microsoft">
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

using System;

namespace System.Diagnostics {
    public abstract class TraceFilter {
        internal string initializeData;

        public abstract bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, 
                                         object[] args, object data1, object[] data);

        internal bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage) {
            return ShouldTrace(cache, source, eventType, id, formatOrMessage, null, null, null);
        }
        
        internal bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, object[] args) {
            return ShouldTrace(cache, source, eventType, id, formatOrMessage, args, null, null);
        }

        internal bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, object[] args, object data1) {
            return ShouldTrace(cache, source, eventType, id, formatOrMessage, args, data1, null);
        }
    }
}
