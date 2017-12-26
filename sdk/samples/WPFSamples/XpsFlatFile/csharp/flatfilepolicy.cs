// XpsFlatFile SDK Sample - FlatFilePolicy.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Printing;
using System.IO;
using System.IO.Packaging;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Xps;

using System.Windows.Xps.Serialization;
using System.Windows.Xps.Packaging;
using System.Windows.Markup;
using System.Globalization;

namespace XpsFlatFile
{
    /// <summary>
    ///   Represents the XPS packaging policy for "flat file" storage.</summary>
    public class FlatPackagingPolicy :
                 BasePackagingPolicy
    {
        #region Constructor

        public FlatPackagingPolicy(String path) :
            base()
        {
            _path = path;

            if (Directory.Exists(path) != true)
            {
                Directory.CreateDirectory(path);
            }

            _contentTypeFile = new ContentTypesFile(path);

            //Font table to cache fonts
            _fontsCache = new Hashtable(11);
        }

        #endregion Constructor


        #region Public API (BasePackagingPolicy overrides)

        // ------------- AcquireXmlWriterForFixedDocumentSequence -------------
        /// <Summary>
        ///   Acquires an XmlWriter for a FixedDocumentSequence.</Summary>
        public override XmlWriter AcquireXmlWriterForFixedDocumentSequence()
        {
            XmlWriter xmlWriter = null;

            // Check to make sure that there is not already a Document Sequence writer created
            if (_currentDocumentSequenceHelper == null)
            {
                String filename = FileHelper.GenerateFileName(XpsFileType.FixedDocumentSequence, _path);
                _currentDocumentSequenceHelper = new XmlWriterHelper(_path, filename);
            }

            xmlWriter = _currentDocumentSequenceHelper.AcquireXmlWriter();

            //Adds file type for FixedDocumentSequence to Content Types
            _contentTypeFile.AddType(XpsFileType.FixedDocumentSequence,
                                     FileHelper.GetExtension(XpsFileType.FixedDocumentSequence));

            //Adds file type for Relationships to Content Types
            _contentTypeFile.AddType(XpsFileType.Relationship,
                                     FileHelper.GetExtension(XpsFileType.Relationship));

            return xmlWriter;
        }


        // ------------- ReleaseXmlWriterForFixedDocumentSequence -------------
        /// <Summary>
        ///   Releases a reference to the current FixedDocumentSequence.</Summary>
        public override void ReleaseXmlWriterForFixedDocumentSequence()
        {
            if (_currentDocumentSequenceHelper != null)
            {
                _currentDocumentSequenceHelper.ReleaseXmlWriter();

                if (_currentDocumentSequenceHelper.FileRef == 0)
                {
                    //Create relationship stream and add starting part relationship to the file
                    String relationshipsFileName = _path + "/_rels/.rels";
                    RelationshipStream relStream = new RelationshipStream(relationshipsFileName);
                    String startingPart = "/" + _currentDocumentSequenceHelper.FileName;
                    relStream.CreateStartingPart(startingPart);
                    relStream.Close();

                    _contentTypeFile.Close();

                    _currentDocumentSequenceHelper = null;

                    // Since the FixedDocumentSequence is being closed we need
                    // to also close the current FixedDocument and FixedPage
                    // if they are still open.

                    //Release Fixed Document Writer
                    if (_currentFixedDocumentHelper != null)
                    {
                        _currentFixedDocumentHelper.ReleaseAll();
                        _currentFixedDocumentHelper = null;
                    }

                    //Release Fixed Page Writer
                    if (_currentFixedPageHelper != null)
                    {
                        _currentFixedPageHelper.ReleaseAll();
                        _currentFixedPageHelper = null;
                    }
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }// end:ReleaseXmlWriterForFixedDocumentSequence


        // ----------------- AcquireXmlWriterForFixedDocument -----------------
        /// <Summary>
        ///   Acquires an XmlWriter for a FixedDocument.</Summary>
        public override XmlWriter AcquireXmlWriterForFixedDocument()
        {
            XmlWriter xmlWriter = null;

            //Create a FixedDocumentHelper if one isn't already created.
            if (_currentFixedDocumentHelper == null)
            {
                String fileName = FileHelper.GenerateFileName(XpsFileType.FixedDocument, _path);
                _currentFixedDocumentHelper = new XmlWriterHelper(_path, fileName);

                //Add FixedDocument content type to ConentTypes file
                _contentTypeFile.AddType(XpsFileType.FixedDocument,
                             FileHelper.GetExtension(XpsFileType.FixedDocument));

                //Need to figure out name for the relationship file for this FixedPage
                String path = _path + "/Documents/_rels/" + Path.GetFileName(fileName) + ".rels";
                _fixedDocumentRelationshipStream = new RelationshipStream(path);

                //Need to write DocumentReference markup to FixedDocument
                _currentDocumentSequenceHelper.WriteStartElement("DocumentReference");
                String relativePath = _currentFixedDocumentHelper.FileName;
                _currentDocumentSequenceHelper.WriteAttributeString("Source", relativePath);
                _currentDocumentSequenceHelper.WriteEndElement();
            }

            xmlWriter = _currentFixedDocumentHelper.AcquireXmlWriter();

            return xmlWriter;
        }


        // ----------------- ReleaseXmlWriterForFixedDocument -----------------
        /// <Summary>
        ///   Releases a reference to the current FixedDocument.</Summary>
        public override void ReleaseXmlWriterForFixedDocument()
        {
            if (_currentFixedDocumentHelper != null)
            {
                _currentFixedDocumentHelper.ReleaseXmlWriter();

                if (_currentFixedDocumentHelper.FileRef == 0)
                {
                    _currentFixedDocumentHelper = null;

                    //close current FixedDocument RelationshipStream
                    if (_fixedDocumentRelationshipStream != null)
                    {
                        _fixedDocumentRelationshipStream.Close();
                        _fixedDocumentRelationshipStream = null;
                    }

                    //When releasing the FixedDocument writer we must ensure the FixedPage writer is also released
                    if (_currentFixedPageHelper != null)
                    {
                        _currentFixedPageHelper.ReleaseAll();
                        _currentFixedPageHelper = null;
                    }
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }// end:ReleaseXmlWriterForFixedDocument


        // ------------------- AcquireXmlWriterForFixedPage -------------------
        /// <Summary>
        ///   Acquires an XmlWriter for a FixedPage.</Summary>
        public override XmlWriter AcquireXmlWriterForFixedPage()
        {
            XmlWriter xmlWriter = null;

            if (_currentFixedPageHelper == null)
            {
                String fileName = FileHelper.GenerateFileName(XpsFileType.FixedPage, _path);
                _currentFixedPageHelper = new XmlWriterHelper(_path, fileName);

                //Need to allocate helper streams and writers for FixedPage
                _linkTargetStream = new List<String>();
                _resourceDictionaryStream = new StringWriter(CultureInfo.InvariantCulture);
                _resourceDictionaryXmlWriter = new XmlTextWriter(_resourceDictionaryStream);
                _pageStream = new StringWriter(CultureInfo.InvariantCulture);
                _pageXmlWriter = new XmlTextWriter(_pageStream);

                //Need to figure out name for the relationship file for this FixedPage
                String path = _path + "/Pages/_rels/" + Path.GetFileName(fileName) + ".rels";
                _fixedPageRelationshipStream = new RelationshipStream(path);

                //Add FixedPage type to Content Type file
                _contentTypeFile.AddType(XpsFileType.FixedPage,
                         FileHelper.GetExtension(XpsFileType.FixedPage));
            }

            xmlWriter = _currentFixedPageHelper.AcquireXmlWriter();

            return xmlWriter;
        }


        // ------------------- ReleaseXmlWriterForFixedPage -------------------
        /// <Summary>
        ///   Releases the FixedPage reference to the XmlWriter.</Summary>
        public override void ReleaseXmlWriterForFixedPage()
        {
            if (_currentFixedPageHelper != null)
            {
                _currentFixedPageHelper.ReleaseXmlWriter();

                if (_currentFixedPageHelper.FileRef == 0)
                {
                    //Need to add some markup here to the FixedDocument to add the source of the FixedPage
                    //and also the LinkTargets (used for navigation)
                    //
                    // Write <PageContent Source="partUri"/>
                    //
                    _currentFixedDocumentHelper.WriteStartElement("PageContent");
                    String fixedPath = "/" + _currentFixedPageHelper.FileName;
                    _currentFixedDocumentHelper.WriteAttributeString("Source", fixedPath);

                    //
                    // Write out link targets if necessary
                    //
                    if (_linkTargetStream.Count != 0)
                    {
                        _currentFixedDocumentHelper.WriteRaw("<PageContent.LinkTargets>");
                        foreach (String nameElement in _linkTargetStream)
                        {
                            _currentFixedDocumentHelper.WriteRaw(String.Format(
                               System.Globalization.CultureInfo.InvariantCulture,
                               "<LinkTarget Name=\"{0}\" />",
                               nameElement)
                               );
                        }
                        _currentFixedDocumentHelper.WriteRaw("</PageContent.LinkTargets>");
                    }

                    _currentFixedDocumentHelper.WriteEndElement();

                    _currentFixedPageHelper = null;

                    //null out and close helper writers/streams
                    _linkTargetStream = null;
                    _resourceDictionaryStream = null;
                    _resourceDictionaryXmlWriter = null;
                    _pageStream = null;
                    _pageXmlWriter = null;
                    _fixedPageRelationshipStream.Close();
                    _fixedPageRelationshipStream = null;
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }// end:ReleaseXmlWriterForFixedPage


        // ------------------- AcquireResourceStreamForXpsFont -------------------
        /// <Summary>
        ///   Acquires a ResourceStream for an XpsFont.</Summary>
        public override XpsResourceStream AcquireResourceStreamForXpsFont()
        {
            throw new NotImplementedException(
                "AcquireResourceStreamForXpsFont() will soon be deprecated.  " +
                "Use AcquireResourceStreamForXpsFont(String resourceId).");
        }


        // ------------------- AcquireResourceStreamForXpsFont -------------------
        /// <Summary>
        ///  Acquires a ResourceStream for an XpsFont with a specified ID.</Summary>
        /// <param name="resourceId">
        ///   The ID of the Resource.</param>
        public override XpsResourceStream AcquireResourceStreamForXpsFont(String resourceId)
        {
            XpsResourceStream resourceStream = null;

            ResourceStreamCacheItem resourceStreamCacheItem =
                (ResourceStreamCacheItem)_fontsCache[resourceId];

            if (resourceStreamCacheItem == null)
            {
                resourceStreamCacheItem = new ResourceStreamCacheItem();

                //
                // We need to create the corresponding part in the Xps package
                // and then acquire the Stream
                //
                Stream fontStream = null;
                String fileName;
                if (_currentFixedPageHelper != null)
                {
                    // Create a new Font Stream
                    fileName = FileHelper.GenerateFileName(XpsFileType.Font, _path);
                    fontStream = FileHelper.CreateFile(fileName, _path);

                    if (fontStream != null)
                    {
                        resourceStreamCacheItem.XpsResource = null;
                        //
                        // retreive the appropriate stream and uri from the package API layer
                        //
                        String fileUri = "/" + fileName;
                        _fontResourceStream = new XpsResourceStream(
                            fontStream, new Uri(fileUri, UriKind.Relative));

                        resourceStreamCacheItem.XpsResourceStream = _fontResourceStream;

                        _fontsCache[resourceId] = resourceStreamCacheItem;

                        resourceStream = _fontResourceStream;

                        //Add Obfuscatedfont type to content type file
                        _contentTypeFile.AddType(XpsFileType.ObfuscatedFont,
                            FileHelper.GetExtension(XpsFileType.ObfuscatedFont));

                        fontStream = null;
                    }
                    else
                    {
                        throw new ArgumentNullException("Could not create FontStream");
                    }
                }
                else
                {
                    throw new XpsSerializationException("No FixedPage Writer");
                }
            }
            else
            {
                resourceStream = resourceStreamCacheItem.XpsResourceStream;
                resourceStreamCacheItem.IncRef();
            }

            //Add relationship to fixedpage for resource stream
            _fixedPageRelationshipStream.AddFixedRelationship(
                    _requiredResource, resourceStream.Uri.ToString());
            return resourceStream;
        }


        /// <Summary>
        ///   Releases a reference to the current XpsFont ResourceStream.</Summary>
        public override void ReleaseResourceStreamForXpsFont()
        {
            throw new NotImplementedException(
                "ReleaseResourceStreamForXpsFont() will soon be deprecated.  " +
                "Use ReleaseResourceStreamForXpsFont(String resourceId).");
        }


        /// <Summary>
        ///   Releases a reference to the current XpsFont ResourceStream.</Summary>
        /// <param name="resourceId">
        ///   ID of the Resource to release.</param>
        public override void ReleaseResourceStreamForXpsFont(String resourceId)
        {
            ResourceStreamCacheItem resourceStreamCacheItem = (ResourceStreamCacheItem)_fontsCache[resourceId];

            if (resourceStreamCacheItem != null)
            {
                if (resourceStreamCacheItem.Release() == 0)
                {
                    _fontsCache.Remove(resourceId);
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }


        /// <Summary>
        ///   Acquires a ResourceStream for an XpsImage.</Summary>
        /// <param name="resourceId">
        ///   The MIME type of the image - image/png, image/jpeg,
        ///   image/tiff, image/vnd.ms-photo.</param>
        public override XpsResourceStream AcquireResourceStreamForXpsImage(String resourceId)
        {
            XpsResourceStream resourceStream = null;

            if (_currentXpsImageRef == 0)
            {
                //
                // We need to create the corresponding part in
                // the Xps package and then acquire the Stream
                //
                if (_currentFixedPageHelper != null)
                {
                    // Create a new image Stream
                    XpsFileType imageType;

                    switch (resourceId)
                    {
                        case "image/png":
                            imageType = XpsFileType.ImagePng;
                            break;
                        case "image/jpeg":
                            imageType = XpsFileType.ImageJpg;
                            break;
                        case "image/tiff":
                            imageType = XpsFileType.ImageTif;
                            break;
                        case "image/vnd.ms-photo":
                            imageType = XpsFileType.ImageWmp;
                            break;
                        default:
                            throw new ArgumentException("Invalid image type");
                    }

                    String fileName = FileHelper.GenerateFileName(imageType, _path);
                    Stream imageStream = FileHelper.CreateFile(fileName, _path);

                    //Add image to the fixed page relationships
                    String imageUri = "/" + fileName;
                    _fixedPageRelationshipStream.AddFixedRelationship(_requiredResource, imageUri);

                    _imageResourceStream = new XpsResourceStream(imageStream, new Uri(imageUri, UriKind.Relative));

                    //Add content type to file
                    _contentTypeFile.AddType(imageType,
                                             FileHelper.GetExtension(imageType));
                }
                else
                {
                    throw new ArgumentNullException("Fixed Page Writer was not allocated before XpsImage was serialized");
                }
            }
            _currentXpsImageRef++;

            resourceStream = _imageResourceStream;

            return resourceStream;
        }


        /// <Summary>
        ///   Releases a reference of the current XpsImage ResourceStream.</Summary>
        public override void ReleaseResourceStreamForXpsImage()
        {
            if (_currentXpsImageRef > 0)
            {
                _currentXpsImageRef--;

                if (_currentXpsImageRef == 0)
                {
                    _imageResourceStream.Initialize();
                    _imageResourceStream = null;
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }


        /// <Summary>
        ///   Acquires a ResourceSTream for a XpsColorContext.</Summary>
        /// <param name="resourceId">
        ///   The ID for the ResourceStream.</param>
        public override XpsResourceStream
            AcquireResourceStreamForXpsColorContext(String resourceId)
        {
            XpsResourceStream resourceStream = null;

            if (_currentXpsColorContextRef == 0)
            {
                //
                // We need to create the corresponding part in
                // the XPS package and then acquire the stream.
                //
                if (_currentFixedPageHelper != null)
                {
                    String fileName = FileHelper.GenerateFileName(XpsFileType.ColorContext, _path);
                    Stream contextStream = FileHelper.CreateFile(fileName, _path);

                    //Add image to the fixed page relationships
                    String contextUri = "/" + fileName;
                    _fixedPageRelationshipStream.AddFixedRelationship(_requiredResource, contextUri);

                    _colorContextResourceStream = new XpsResourceStream(contextStream, new Uri(contextUri, UriKind.Relative));

                    //Add content type to file
                    _contentTypeFile.AddType(XpsFileType.ColorContext,
                                             FileHelper.GetExtension(XpsFileType.ColorContext));
                }
                else
                {
                    throw new ArgumentNullException("Fixed Page Writer was not allocated before XpsColorContext was serialized");
                }
            }

            _currentXpsColorContextRef++;

            resourceStream = _colorContextResourceStream;

            return resourceStream;
        }


        /// <Summary>
        ///   Releases a reference to the XpsColorContext ResourceStream.</Summary>
        public override void ReleaseResourceStreamForXpsColorContext()
        {
            if (_currentXpsColorContextRef > 0)
            {
                _currentXpsColorContextRef--;

                if (_currentXpsColorContextRef == 0)
                {
                    _colorContextResourceStream.Initialize();
                    _colorContextResourceStream = null;
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }


        /// <Summary>
        ///   Acquires a ResoureStream for a XpsResourceDictionary.</Summary>
        /// <param name="resourceId">
        ///   The resource Id for the ResourceStream.</param>
        public override XpsResourceStream
            AcquireResourceStreamForXpsResourceDictionary(String resourceId)
        {
            XpsResourceStream resourceStream = null;

            if (_currentXpsResourceDictionaryRef == 0)
            {
                //
                // We need to create the corresponding part in the Xps package
                // and then acquire the Stream
                //
                if (_currentFixedPageHelper != null)
                {
                    String fileName = FileHelper.GenerateFileName(XpsFileType.ResourceDictionary, _path);
                    Stream dictionaryStream = FileHelper.CreateFile(fileName, _path);

                    //Add image to the fixed page relationships
                    String dictionaryUri = "/" + fileName;
                    _fixedPageRelationshipStream.AddFixedRelationship(_requiredResource, dictionaryUri);

                    _resourceDictionaryResourceStream = new XpsResourceStream(dictionaryStream, new Uri(dictionaryUri, UriKind.Relative));

                    //Add content type to file
                    _contentTypeFile.AddType(XpsFileType.ResourceDictionary,
                                             FileHelper.GetExtension(XpsFileType.ResourceDictionary));
                }
                else
                {
                    throw new ArgumentNullException("Fixed Page Writer was not allocated before XpsResourceDictionary was serialized");
                }
            }

            _currentXpsResourceDictionaryRef++;

            resourceStream = _resourceDictionaryResourceStream;

            return resourceStream;
        }


        /// <Summary>
        ///   Releases a reference to the current XpsResourceDictionary.</Summary>
        public override void ReleaseResourceStreamForXpsResourceDictionary()
        {
            if (_currentXpsResourceDictionaryRef > 0)
            {
                _currentXpsResourceDictionaryRef--;

                if (_currentXpsResourceDictionaryRef == 0)
                {
                    _resourceDictionaryResourceStream.Initialize();
                    _resourceDictionaryResourceStream = null;
                }
            }
            else
            {
                throw new XpsSerializationException("Cannot release XmlWriter");
            }
        }


        /// <summary>
        ///   Adds a relationship with a specified target and
        //    relationship name to the current active page.</summary>
        /// <param name="targetUri">
        ///   The URI of the Target for the relationship.</param>
        /// <param name="relationshipName">
        ///   The name of the Relationship to add.</param>
        public override void
            RelateResourceToCurrentPage(Uri targetUri, string relationshipName)
        {
            if (_fixedPageRelationshipStream != null)
            {
                _fixedPageRelationshipStream.AddFixedRelationship(relationshipName, targetUri.ToString());
            }
        }


        /// <summary>
        ///   Adds a relationship with a specified target and
        //    relationship name to the current active document.</summary>
        /// <param name="targetUri">
        ///   The URI of the Target for the relationship.</param>
        public override void RelateRestrictedFontToCurrentDocument(Uri targetUri)
        {
            if (_currentFixedDocumentHelper != null &&
                _fixedDocumentRelationshipStream != null)
            {
                _fixedDocumentRelationshipStream.AddFixedRelationship(_restrictedFontRelationshipType, targetUri.ToString());
            }
        }


        /// <summary>
        /// Persists the PrintTicket to the packaging layer
        /// </summary>
        /// <exception cref="ArgumentNullException">printTicket is NULL.</exception>
        /// <param name="printTicket">
        /// Caller supplied PrintTicket.
        /// </param>
        public
        override
        void
        PersistPrintTicket(
            PrintTicket printTicket
            )
        {
            if (printTicket == null)
            {
                throw new ArgumentNullException("printTicket");
            }

            //Need to figure out which level in the document we are at and
            //assign the print ticket to the most specific level
            if (_currentFixedPageHelper != null)
            {
                _currentFixedPageHelper.PrintTicket = printTicket;
            }
            else if (_currentFixedDocumentHelper != null)
            {
                _currentFixedDocumentHelper.PrintTicket = printTicket;
            }
            else if (_currentDocumentSequenceHelper != null)
            {
                _currentDocumentSequenceHelper.PrintTicket = printTicket;
            }
        }

        /// <summary>
        /// Acquire a XmlWriter for the current page
        /// </summary>
        public
        override
        XmlWriter
        AcquireXmlWriterForPage(
            )
        {
            return _pageXmlWriter;
        }

        /// <summary>
        /// Prepare to commit the current page.  Need to flush current page helper streams, and write any additional
        /// data to the current fixed page.
        /// </summary>
        public
        override
        void
        PreCommitCurrentPage(
            )
        {
            _resourceDictionaryXmlWriter.Flush();
            _pageXmlWriter.Flush();

            if (_resourceDictionaryStream.ToString().Length > 0)
            {
                //Before committing the current page we need to serialize out the ResourceDictionary (if there are any)
                // to the current FixedPage Stream
                _currentFixedPageHelper.WriteStartElement("FixedPage.Resources");
                _currentFixedPageHelper.WriteStartElement("ResourceDictionary");

                _currentFixedPageHelper.WriteRaw(_resourceDictionaryStream.ToString());

                _currentFixedPageHelper.WriteEndElement();
                _currentFixedPageHelper.WriteEndElement();
            }

            //
            // Join page stream into main stream
            //
            _currentFixedPageHelper.WriteRaw(_pageStream.ToString());

        }

        /// <summary>
        /// Acquires the Resource Dictionary Xml Writer for the current FixedPage
        /// </summary>
        public
        override
        XmlWriter
        AcquireXmlWriterForResourceDictionary(
            )
        {
            return _resourceDictionaryXmlWriter;
        }

        /// <summary>
        /// Acquires the LinkTarget List for the current Fixed page
        /// </summary>
        public
        override
        IList<String>
        AcquireStreamForLinkTargets(
            )
        {
            return _linkTargetStream;
        }


        /// <summary>
        /// Get the Uri of the Current FixedDocumentWriter. Null is returned if there is
        /// not a current FixedDocumentWriter.
        /// </summary>
        public
        override
        Uri
        CurrentFixedDocumentUri
        {
            get
            {
                if (_currentFixedDocumentHelper == null)
                {
                    return null;
                }
                else
                {
                    return _currentFixedDocumentHelper.Uri;
                }
            }
        }

        /// <summary>
        /// Get the Uri of the Current FixedPageWriter.  Null is returned if there is
        /// not a current FixedPageWriter
        /// </summary>
        public
        override
        Uri
        CurrentFixedPageUri
        {
            get
            {
                if (_currentFixedPageHelper == null)
                {
                    return null;
                }
                else
                {
                    return _currentFixedPageHelper.Uri;
                }
            }
        }

        #endregion

        #region Private members

        private
        XmlWriterHelper _currentDocumentSequenceHelper;

        private
        XmlWriterHelper _currentFixedDocumentHelper;

        private
        XmlWriterHelper _currentFixedPageHelper;

        private
        XpsResourceStream _fontResourceStream;

        private
        ContentTypesFile _contentTypeFile;

        internal enum ResourceAcquireMode
        {
            NoneAcquired = 0,
            SingleAcquired = 1,
            MultipleAcquired = 2
        };

        private
        XpsResourceStream _imageResourceStream;

        private int _currentXpsColorContextRef;

        private
        XpsResourceStream _colorContextResourceStream;

        private
        IList<String> _linkTargetStream;

        private
        StringWriter _pageStream;

        private
        XmlWriter _pageXmlWriter;

        private
        RelationshipStream _fixedPageRelationshipStream;

        private
        RelationshipStream _fixedDocumentRelationshipStream;

        private
        XmlWriter _resourceDictionaryXmlWriter;

        private
        StringWriter _resourceDictionaryStream;

        private
        Hashtable   _fontsCache;

        private
        int _currentXpsImageRef;

        private
        int _currentXpsResourceDictionaryRef;

        private
        XpsResourceStream _resourceDictionaryResourceStream;

        private
        String _path;

        static private
        String _requiredResource = "http://schemas.microsoft.com/xps/2005/06/required-resource";

        static private
        string _restrictedFontRelationshipType = "http://schemas.microsoft.com/xps/2005/06/restricted-font";

        #endregion Private members
    }


    /// <summary>
    /// This class is created in order to allow sharing of resources
    /// across different parts of the package. It keeps an internal
    /// ref count of the resource usage.
    /// </summary>
    internal class ResourceStreamCacheItem
    {
        #region Constructor

        internal ResourceStreamCacheItem()
        {
            this._resourceStreamRefCount = 1;
        }

        #endregion Constructor


        #region Internal Properties

        internal XpsResourceStream XpsResourceStream
        {
            get
            {
                return _resourceStream;
            }

            set
            {
                _resourceStream = value;
            }
        }


        internal XpsResource XpsResource
        {
            get
            {
                return _reachResource;
            }

            set
            {
                _reachResource = value;
            }
        }

        #endregion Internal Properties


        #region Internal Methods

        internal void IncRef()
        {
            _resourceStreamRefCount++;
        }


        internal int Release()
        {
            if (_resourceStreamRefCount > 0)
            {
                _resourceStreamRefCount--;
            }

            return _resourceStreamRefCount;
        }

        #endregion Internal Methods


        #region Private Data

        private XpsResourceStream _resourceStream;

        private XpsResource _reachResource;

        private int _resourceStreamRefCount;

        #endregion Private Data
    }
}