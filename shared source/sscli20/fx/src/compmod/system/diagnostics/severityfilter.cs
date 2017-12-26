//------------------------------------------------------------------------------
// <copyright file="SeverityFilter.cs" company="Microsoft">
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
    public class EventTypeFilter : TraceFilter {
        private SourceLevels level;
        
        public EventTypeFilter(SourceLevels level) {
            this.level = level;
        }

        public override bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, 
                                         object[] args, object data1, object[] data) {
                                         
             return ((int) eventType & (int) level) != 0;
        }

        public SourceLevels EventType {
            get {
                return level;
            }
            set {
                level = value;
            }
        }
    }
}
