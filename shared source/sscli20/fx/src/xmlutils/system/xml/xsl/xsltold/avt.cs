//------------------------------------------------------------------------------
// <copyright file="Avt.cs" company="Microsoft">
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
    using System.Collections;
    using System.Xml;
    using System.Xml.XPath;
    using System.Text;

    internal sealed class Avt {
        private string      constAvt;
        private TextEvent[] events;

        private Avt(string constAvt) {
            Debug.Assert(constAvt != null);
            this.constAvt = constAvt;
        }

        private Avt(ArrayList eventList) {
            Debug.Assert(eventList != null);
            this.events = new TextEvent[eventList.Count];
            for(int i = 0; i < eventList.Count; i ++) {
                this.events[i] = (TextEvent) eventList[i];
            }
        }

        public bool IsConstant {
            get {return this.events == null;}
        }

        internal string Evaluate(Processor processor, ActionFrame frame) {
            if (IsConstant) {
                Debug.Assert(constAvt != null);
                return constAvt;
            }
            else {
                Debug.Assert(processor != null && frame != null);

                StringBuilder builder = processor.GetSharedStringBuilder();

                for(int i = 0; i < events.Length; i ++) {
                    builder.Append(events[i].Evaluate(processor, frame));
                }
                processor.ReleaseSharedStringBuilder();
                return builder.ToString();
            }
        }

        internal static Avt CompileAvt(Compiler compiler, string avtText) {
            Debug.Assert(compiler != null);
            Debug.Assert(avtText != null);

            bool constant;
            ArrayList list = compiler.CompileAvt(avtText, out constant);
            return constant ? new Avt(avtText) : new Avt(list);
        }
    }
}
