// XpsFlatFile SDK Sample - ContentTypes.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;


namespace XpsFlatFile
{
    /// <summary>
    ///   Specifies the XPS file types.</summary>
    public enum XpsFileType
    {
        FixedDocumentSequence,
        FixedDocument,
        FixedPage,
        Relationship,
        ImagePng,
        ImageTif,
        ImageJpg,
        ImageWmp,
        Font,
        ObfuscatedFont,
        ResourceDictionary,
        ColorContext
    }


    // ======================== class ContentTypesFile ========================
    /// <summary>
    ///   Creates the ContentTypes file required for XPS Packaging.</summary>
    /// <remarks>
    ///   If you are creating your own XPS packaging format,
    ///   a content type file is not required.<remarks>
    public class ContentTypesFile
    {
        // ------------------ ContentTypesFile constructor --------------------
        /// <summary>
        ///   Creates the XmlTextWriter and writes the initial elements.</summary>
        /// <param name="path">
        ///   The folder in which to create the Content_Types.xml file.</param>
        public ContentTypesFile(String path)
        {
            String filename = path + "\\" + _contentTypesFileName;
            _writer = new XmlTextWriter(filename, null);

            _writer.WriteStartDocument();
            _writer.WriteStartElement("Types");
            _writer.WriteAttributeString("xmlns", _contentTypesNamespace);

            _contentTypeTable = new ArrayList(10);
        }


        // ----------------------------- AddType ------------------------------
        /// <summary>
        ///   Relates a file extension to a XpsFileType
        ///   in the Content Types file.</summary>
        /// <param name="fileType">
        ///   The type of XpsFile to add.</param>
        /// <param name="extension">
        ///   The file extension of the file type.</param>
        public void AddType(XpsFileType fileType, String extension)
        {
            //check for duplicates
            if (_contentTypeTable.Contains(fileType) == false)
            {
                _contentTypeTable.Add(fileType);

                _writer.WriteStartElement(_contentTypeDefault);
                _writer.WriteAttributeString(_contentTypeExtension, extension);

                String contentType;
                switch (fileType)
                {
                    case XpsFileType.FixedDocumentSequence:
                        contentType = _FixedDocumentSequenceContentType;
                        break;
                    case XpsFileType.FixedDocument:
                        contentType = _FixedDocumentContentType;
                        break;
                    case XpsFileType.FixedPage:
                        contentType = _FixedPageContentType;
                        break;
                    case XpsFileType.Relationship:
                        contentType = _RelationshipsContentType;
                        break;
                    case XpsFileType.ImagePng:
                        contentType = _ImagePngContentType;
                        break;
                    case XpsFileType.ImageTif:
                        contentType = _ImageTifContentType;
                        break;
                    case XpsFileType.ImageJpg:
                        contentType = _ImageJpgContentType;
                        break;
                    case XpsFileType.ImageWmp:
                        contentType = _ImageWmpContentType;
                        break;
                    case XpsFileType.Font:
                        contentType = _fontContentType;
                        break;
                    case XpsFileType.ObfuscatedFont:
                        contentType = _ObfuscatedFontContentType;
                        break;
                    case XpsFileType.ResourceDictionary:
                        contentType = _resourceDictionaryContentType;
                        break;
                    case XpsFileType.ColorContext:
                        contentType = _colorContextContentType;
                        break;
                    default:
                        throw new ArgumentException("Invalid XpsFileType");
                }

                _writer.WriteAttributeString(_contentTypeString, contentType);
                _writer.WriteEndElement();
            }
        }// end:AddType


        // ------------------------------ Close -------------------------------
        /// <summary>
        ///   Close sthe ContentTypes file.</summary>
        public void Close()
        {
            if (_writer != null)
            {
                _writer.WriteEndElement();
                _writer.Close();
                _writer = null;
            }
        }


        #region Private Memebers

        private XmlTextWriter _writer;
        private ArrayList     _contentTypeTable;

        //content types markup
        static private string _contentTypesNamespace =
            "http://schemas.microsoft.com/package/2005/06/content-types";

        static private string _contentTypesFileName = "[Content_Types].xml";
        static private string _contentTypeDefault   = "Default";
        static private string _contentTypeExtension = "Extension";
        static private string _contentTypeString    = "ContentType";

        static private string _FixedDocumentSequenceContentType =
            "application/vnd.ms-package.xps-fixeddocumentsequence+xml";

        static private string _FixedDocumentContentType =
            "application/vnd.ms-package.xps-fixeddocument+xml";

        static private string _FixedPageContentType =
            "application/vnd.ms-package.xps-fixedpage+xml";

        static private string _ObfuscatedFontContentType =
            "application/vnd.ms-package.obfuscated-opentype";

        static private string _fontContentType = "application/vnd.ms-opentype";

        static private string _RelationshipsContentType =
            "application/vnd.ms-package.relationships+xml";

        static private string _resourceDictionaryContentType =
            "application/vnd.ms-package.xps-resourcedictionary+xml";

        static private string _colorContextContentType =
            "application/vnd.ms-color.iccprofile";

        static private string _ImagePngContentType = "image/png";
        static private string _ImageJpgContentType = "image/jpeg";
        static private string _ImageTifContentType = "image/tiff";
        static private string _ImageWmpContentType = "image/vnd.ms-photo";

        #endregion Private Memebers

    }// end:class ContentTypesFile

}// end:namespace XpsFlatFile

