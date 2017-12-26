//------------------------------------------------------------------------------
// <copyright file="QilStrConcatenator.cs" company="Microsoft">
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
using System.Diagnostics;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Xsl.XPath;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Xslt {

    internal class QilStrConcatenator {
        private XPathQilFactory f;
        private BufferBuilder   builder;
        private QilList         concat;
        private bool inUse = false;

        public QilStrConcatenator(XPathQilFactory f) {
            this.f  = f;
            builder = new BufferBuilder();
        }

        public void Reset() {
            Debug.Assert(! inUse);
            inUse = true;
            builder.Clear();
            concat = null;
        }

        private void FlushBuilder() {
            if (concat == null) {
                concat = f.BaseFactory.Sequence();
            }
            if (builder.Length != 0) {
                concat.Add(f.String(builder.ToString()));
                builder.Length = 0;
            }
        }

        public void Append(string value) {
            Debug.Assert(inUse, "Reset() wasn't called");
            builder.Append(value);
        }

        public void Append(char value) {
            Debug.Assert(inUse, "Reset() wasn't called");
            builder.Append(value);
        }

        public void Append(QilNode value) {
            Debug.Assert(inUse, "Reset() wasn't called");
            if (value != null) {
                Debug.Assert(value.XmlType.TypeCode == XmlTypeCode.String);
                if (value.NodeType == QilNodeType.LiteralString) {
                    builder.Append((string)(QilLiteral)value);
                } else {
                    FlushBuilder();
                    concat.Add(value);
                }
            }
        }

        public QilNode ToQil() {
            Debug.Assert(inUse); // If we want allow multiple calls to ToQil() this logic should be changed
            inUse = false;
            if (concat == null) {
                return f.String(builder.ToString());
            } else {
                FlushBuilder();
                return f.StrConcat(concat);
            }
        }
    }
}
