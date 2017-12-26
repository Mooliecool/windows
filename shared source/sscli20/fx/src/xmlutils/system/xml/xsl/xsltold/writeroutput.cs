//------------------------------------------------------------------------------
// <copyright file="WriterOutput.cs" company="Microsoft">
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
    using System.Collections;

    internal class WriterOutput : RecordOutput {
        private XmlWriter writer;
        private Processor processor;

        internal WriterOutput(Processor processor, XmlWriter writer) {
            if (writer == null) {
                throw new ArgumentNullException("writer");
            }

            this.writer    = writer;
            this.processor = processor;
        }

        // RecordOutput interface method implementation
        //
        public Processor.OutputResult RecordDone(RecordBuilder record) {
            BuilderInfo mainNode       = record.MainNode;

            switch (mainNode.NodeType) {
            case XmlNodeType.Element:
                this.writer.WriteStartElement(mainNode.Prefix, mainNode.LocalName, mainNode.NamespaceURI);

                WriteAttributes(record.AttributeList, record.AttributeCount);

                if (mainNode.IsEmptyTag) {
                    this.writer.WriteEndElement();
                }
                break;
            case XmlNodeType.Text:
            case XmlNodeType.Whitespace:
            case XmlNodeType.SignificantWhitespace:
                this.writer.WriteString(mainNode.Value);
                break;
            case XmlNodeType.CDATA:
                Debug.Assert(false, "XSLT never gives us CDATA");
                this.writer.WriteCData(mainNode.Value);
                break;
            case XmlNodeType.EntityReference:
                this.writer.WriteEntityRef(mainNode.LocalName);
                break;
            case XmlNodeType.ProcessingInstruction:
                this.writer.WriteProcessingInstruction(mainNode.LocalName, mainNode.Value);
                break;
            case XmlNodeType.Comment:
                this.writer.WriteComment(mainNode.Value);
                break;
            case XmlNodeType.Document:
                break;
            case XmlNodeType.DocumentType:
                this.writer.WriteRaw(mainNode.Value);
                break;
            case XmlNodeType.EndElement:
                this.writer.WriteFullEndElement();
                break;

            case XmlNodeType.None:
            case XmlNodeType.Attribute:
            case XmlNodeType.Entity:
            case XmlNodeType.Notation:
            case XmlNodeType.DocumentFragment:
            case XmlNodeType.EndEntity:
                break;
            default:
                Debug.Fail("Invalid NodeType on output: " + mainNode.NodeType);
                break;
            }

            record.Reset();
            return Processor.OutputResult.Continue;
        }

        public void TheEnd() {
            this.writer.Flush();
            this.writer = null;
        }

        private void WriteAttributes(ArrayList list, int count) {
            Debug.Assert(list.Count >= count);
            for (int attrib = 0; attrib < count; attrib ++) {
                Debug.Assert(list[attrib] is BuilderInfo);
                BuilderInfo attribute = (BuilderInfo) list[attrib];
                this.writer.WriteAttributeString(attribute.Prefix, attribute.LocalName, attribute.NamespaceURI, attribute.Value);
            }
        }
    }
}

