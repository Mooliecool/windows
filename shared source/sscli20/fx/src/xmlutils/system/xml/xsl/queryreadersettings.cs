//------------------------------------------------------------------------------
// <copyright file="QueryReaderSettings.cs" company="Microsoft">
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

using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Xml;

namespace System.Xml.Xsl {
    internal class QueryReaderSettings {
        private bool               validatingReader;
        private XmlReaderSettings  xmlReaderSettings;
        private XmlNameTable       xmlNameTable;
        private EntityHandling     entityHandling;
        private bool               namespaces;
        private bool               normalization;
        private bool               prohibitDtd;
        private WhitespaceHandling whitespaceHandling;
        private XmlResolver        xmlResolver;

        public QueryReaderSettings(XmlNameTable xmlNameTable) {
            Debug.Assert(xmlNameTable != null);
            xmlReaderSettings = new XmlReaderSettings();
            xmlReaderSettings.NameTable = xmlNameTable;
            xmlReaderSettings.ConformanceLevel = ConformanceLevel.Document;
            xmlReaderSettings.XmlResolver = null;
            xmlReaderSettings.ProhibitDtd = true;
            xmlReaderSettings.CloseInput = true;
        }

        public QueryReaderSettings(XmlReader reader) {
#pragma warning disable 618
            XmlValidatingReader valReader = reader as XmlValidatingReader;
#pragma warning restore 618
            if (valReader != null) {
                // Unwrap validation reader
                validatingReader = true;
                reader = valReader.Impl.Reader;
            }
            xmlReaderSettings = reader.Settings;
            if (xmlReaderSettings != null) {
                xmlReaderSettings = xmlReaderSettings.Clone();
                xmlReaderSettings.NameTable = reader.NameTable;
                xmlReaderSettings.CloseInput = true;
                xmlReaderSettings.LineNumberOffset = 0;
                xmlReaderSettings.LinePositionOffset = 0;
                XmlTextReaderImpl impl = reader as XmlTextReaderImpl;
                if (impl != null) {
                    xmlReaderSettings.XmlResolver = impl.GetResolver();
                }
            } else {
                xmlNameTable = reader.NameTable;
                XmlTextReader xmlTextReader = reader as XmlTextReader;
                if (xmlTextReader != null) {
                    XmlTextReaderImpl impl = xmlTextReader.Impl;
                    entityHandling     = impl.EntityHandling;
                    namespaces         = impl.Namespaces;
                    normalization      = impl.Normalization;
                    prohibitDtd        = impl.ProhibitDtd;
                    whitespaceHandling = impl.WhitespaceHandling;
                    xmlResolver        = impl.GetResolver();
                } else {
                    entityHandling     = EntityHandling.ExpandEntities;
                    namespaces         = true;
                    normalization      = true;
                    prohibitDtd        = true;
                    whitespaceHandling = WhitespaceHandling.All;
                    xmlResolver        = null;
                }
            }
        }

        public XmlReader CreateReader(Stream stream, string baseUri) {
            XmlReader reader;
            if (xmlReaderSettings != null) {
                reader = XmlTextReader.Create(stream, xmlReaderSettings, baseUri);
            } else {
                XmlTextReaderImpl readerImpl = new XmlTextReaderImpl(baseUri, stream, xmlNameTable);
                readerImpl.EntityHandling = entityHandling;
                readerImpl.Namespaces = namespaces;
                readerImpl.Normalization = normalization;
                readerImpl.ProhibitDtd = prohibitDtd;
                readerImpl.WhitespaceHandling = whitespaceHandling;
                readerImpl.XmlResolver = xmlResolver;
                reader = readerImpl;
            }
            if (validatingReader) {
#pragma warning disable 618
                reader = new XmlValidatingReader(reader);
#pragma warning restore 618
            }
            return reader;
        }

        public XmlNameTable NameTable {
            get { return xmlReaderSettings != null ? xmlReaderSettings.NameTable : xmlNameTable; }
        }
    }
}
