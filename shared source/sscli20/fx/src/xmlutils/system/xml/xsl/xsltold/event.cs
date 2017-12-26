//------------------------------------------------------------------------------
// <copyright file="Event.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Diagnostics;
    using System.Xml;
    using System.Xml.XPath;
    using System.Xml.Xsl.XsltOld.Debugger;

    internal abstract class Event {
        public virtual void ReplaceNamespaceAlias(Compiler compiler) {}        
        public abstract bool Output(Processor processor, ActionFrame frame);

        internal void OnInstructionExecute(Processor processor) {
            Debug.Assert(processor.Debugger != null, "We don't generate calls this function if ! debugger");
            Debug.Assert(DbgData.StyleSheet != null, "We call this function from *EventDbg only");
            processor.OnInstructionExecute();
        }

        internal virtual DbgData DbgData { get { return DbgData.Empty; } }
    }
}
