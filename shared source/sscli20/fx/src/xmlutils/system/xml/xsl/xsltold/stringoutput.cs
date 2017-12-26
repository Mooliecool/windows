//------------------------------------------------------------------------------
// <copyright file="StringOutput.cs" company="Microsoft">
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
    using System.Xml;
    using System.Text;

    internal class StringOutput : SequentialOutput {
        private StringBuilder builder;
        private string        result;

        internal string Result {
            get {
                return this.result;
            }
        }

        internal StringOutput(Processor processor)
        : base(processor) {
            this.builder  = new StringBuilder();
        }

        internal override void Write(char outputChar) {
            this.builder.Append(outputChar);

#if DEBUG
            this.result = this.builder.ToString();
#endif
        }

        internal override void Write(string outputText) {
            this.builder.Append(outputText);

#if DEBUG
            this.result = this.builder.ToString();
#endif
        }

        internal override void Close() {
            this.result = this.builder.ToString();
        }
    }

}
