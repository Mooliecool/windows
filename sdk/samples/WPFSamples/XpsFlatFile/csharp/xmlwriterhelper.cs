// XpsFlatFile SDK Sample - XmlWriterHelper.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Printing;
using System.Windows.Xps;


namespace XpsFlatFile
{
    // ========================= XmlWriterHelper class ========================
    /// <summary>
    ///   Creates an XmlWriterHelper which tracks how many references there are
    ///   to it and destroys it when there are no more references.</summary>
    /// <remarks>
    ///   The XmlWriterHelper classs also implements XmlWriter methods to write
    ///   serialized XPS data.  This class is created to illustrate how to use
    ///   XPS Packaging Policy, but is not required.</remarks>
    public class XmlWriterHelper
    {
        // ------------------- XmlWriterHelper constructor --------------------
        /// <summary>
        ///   Concatenates the path and filename to create the fullpath.</summary>
        /// <param name="path">
        ///   The path for where the XML file will be stored.</param>
        /// <param name="fileName">
        ///   The file name for XML writer.</param>
        public XmlWriterHelper(String path, String fileName)
        {
            _fileName = fileName;
            int lastChar = path.Length - 1;
            StringBuilder tmpPath = new StringBuilder(path);
            if (path[lastChar] != '/')
                tmpPath.Append("/");
            tmpPath.Append(fileName);
            _path = tmpPath.ToString();
            _fileRef = 0;
        }

        // ------------------------- AcquireXmlWriter -------------------------
        /// <summary>
        ///   Returns either the current or a new XmlWriter,</summary>
        /// </summary>
        /// <returns>
        ///   The current or a new XmlWriter.</returns>
        /// <remarks>
        ///   The AcquireXmlWriter method returns the current XmlWriter if one
        ///   exists; or creates and returns a new XmlWriter if one does not
        ///   exist when called the first time.</remarks>
        public XmlWriter AcquireXmlWriter()
        {
            XmlWriter xmlWriter = null;

            // If there is currently no XmlWriter, create one.
            if (_fileRef == 0)
                _xmlTextWriter = new XmlTextWriter(_path, null);

            // Increment the reference count to the XmlWriter.
            _fileRef++;

            xmlWriter = _xmlTextWriter as XmlWriter;
            return xmlWriter;
        }


        // ------------------------- ReleaseXmlWriter -------------------------
        /// <summary>
        ///   Releases one reference to the XmlWriter.</summary>
        /// <remarks>
        ///   When the reference count goes to zero, the XmlWriter is closed.</remarks>
        public void ReleaseXmlWriter()
        {
            if (_xmlTextWriter != null &&
                _fileRef > 0)
            {
                _fileRef--;

                if (_fileRef == 0)
                {
                    _xmlTextWriter.Close();
                    _xmlTextWriter = null;
                }
            }
            else
            {
                throw new XpsSerializationException("Attempting to release an XmlWriter that has already been released");
            }
        }


        // ---------------------------- ReleaseAll ----------------------------
        /// <summary>
        ///   Releases all references to the XmlWriter and closes it.</summary>
        public void ReleaseAll()
        {
            if (_xmlTextWriter != null)
            {
                _xmlTextWriter.Close();
                _xmlTextWriter = null;
                _fileRef = 0;
            }
        }


        // ----------------------------- WriteRaw -----------------------------
        /// <summary>
        ///   Writes a specified string to the writer.</summary>
        /// <param name="s">
        ///   The string to write.</param>
        public void WriteRaw(String s)
        {
            _xmlTextWriter.WriteRaw(s);
        }


        // ------------------------- WriteStartElement ------------------------
        /// <summary>
        ///   Writes the specified StartElement string to the writer.</summary>
        /// <param name="s">
        ///   The StartElement string to write.</param>
        public void WriteStartElement(String s)
        {
            _xmlTextWriter.WriteStartElement(s);
        }


        // -------------------------- WriteEndElement -------------------------
        /// <summary>
        ///   Writes the EndElement to the writer.</summary>
        public void WriteEndElement()
        {
            _xmlTextWriter.WriteEndElement();
        }


        // ----------------------- WriteAttributeString -----------------------
        /// <summary>
        ///   Writes a specified attribute name and value to the writer.</summary>
        /// <param name="localName">
        ///   The name of the attribute.</param>
        /// <param name="value">
        ///   The value of of the attribute.</param>
        public void WriteAttributeString(String localName, String value)
        {
            _xmlTextWriter.WriteAttributeString(localName, value);
        }


        // ----------------------------- FileRef ------------------------------
        /// <summary>
        ///   Gets the number file references on the XmlWriter.</summary>
        public int FileRef
        {
            get
            {
                return _fileRef;
            }
        }


        // ----------------------------- FileName -----------------------------
        /// <summary>
        ///   Gets the output filename for the XmlWriter.</summary>
        public String FileName
        {
            get
            {
                return _fileName;
            }
        }


        // ---------------------------- PrintTicket ---------------------------
        /// <summary>
        ///   Gets or sets the PrintTicket for the XPS document.</summary>
        /// <remarks>
        //    Note: PrintTicket is currently not supported.</remarks>
        public PrintTicket PrintTicket
        {
            get
            {
                return _printTicket;
            }
            set
            {
                _printTicket = value;
            }
        }


        // -------------------------------- Uri -------------------------------
        /// <summary>
        ///   Gets the URI of the XmlWriter.</summary>
        public Uri Uri
        {
            get
            {
                Uri uri = new Uri(_path);
                return uri;
            }
        }


        private String        _path;
        private String        _fileName;
        private XmlTextWriter _xmlTextWriter;
        private int           _fileRef;
        private PrintTicket   _printTicket;

    }// end:class XmlWriterHelper

}// end:namespace XpsFlatFile
