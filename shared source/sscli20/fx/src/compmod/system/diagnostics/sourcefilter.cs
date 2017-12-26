//------------------------------------------------------------------------------
// <copyright file="SourceFilter.cs" company="Microsoft">
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
using System.Collections;

namespace System.Diagnostics {
    public class SourceFilter : TraceFilter {
        private string src;
        
        public SourceFilter(string source) {
            Source = source;
        }

        public override bool ShouldTrace(TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, 
                                         object[] args, object data1, object[] data) {
             if (source == null)
                throw new ArgumentNullException("source");
             
             return String.Equals(src, source);
        }

        public String Source {
            get { 
                return src; 
            }
            set {
                if (value == null)
                   throw new ArgumentNullException("source");
                src = value;
            }
        }
    }
}
                
