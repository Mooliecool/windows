//------------------------------------------------------------------------------
// <copyright file="TextOnlyOutput.cs" company="Microsoft">
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
    using System.IO;
    using System.Xml;
    using System.Xml.XPath;
    using System.Text;
    using System.Collections;

    internal class TextOnlyOutput : RecordOutput {
        private     Processor  processor;
        private     TextWriter writer;

        internal XsltOutput Output {
            get { return this.processor.Output; }
        }

		public TextWriter Writer {
			get { return this.writer; }
		}

        //
        // Constructor
        //

        internal TextOnlyOutput(Processor processor, Stream stream) {
            if (stream == null) {
                throw new ArgumentNullException("stream");
            }

            this.processor  = processor;
            this.writer     = new StreamWriter(stream, Output.Encoding);
        }

        internal TextOnlyOutput(Processor processor, TextWriter writer) {
            if (writer == null) {
                throw new ArgumentNullException("writer");
            }

            this.processor  = processor;
            this.writer     = writer;
        }

        //
        // RecordOutput interface method implementation
        //

        public Processor.OutputResult RecordDone(RecordBuilder record) {
            BuilderInfo mainNode       = record.MainNode;

            switch (mainNode.NodeType) {
            case XmlNodeType.Text:
            case XmlNodeType.Whitespace:
            case XmlNodeType.SignificantWhitespace:
                this.writer.Write(mainNode.Value);
                break;
            default:
                break;
            }

            record.Reset();
            return Processor.OutputResult.Continue;
        }

        public void TheEnd() {
            this.writer.Flush();
        }
    }
}
