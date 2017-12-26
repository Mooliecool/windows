// PackWebRequest SDK Sample - RetailServer.cs
// Copyright (c) Microsoft Corporation. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Packaging;
using System.Windows.Media.Imaging;
using System.Windows.Xps.Packaging;
using PackWebRequestSample;
using System.Xml;
using System.Net;

namespace PackWebRequestSample
{
    class RetailServer
    {
        private const string _photoXpsFileName = "Photos.xps";

        private static int _currentId;     // used to create a unique name of a thumbnail

        private static Dictionary<string, XmlDocument> _packInfoDict =
            new Dictionary<string, XmlDocument>();

        private static Dictionary<string, Uri> _packUriDict =
            new Dictionary<string, Uri>();

        // new a PackWebRequestFactory object and cast it to a 
        // IWebRequestCreate, so we can create a PackWebRequest later.
        private static IWebRequestCreate _webRequestCreate =
                (IWebRequestCreate)(new PackWebRequestFactory());

        /// <summary>
        /// This application simulates the retail server in the sample scenario.
        /// It processes the query from client.
        /// 1.  If the query is asking for thumbnails with a specified keyword, 
        ///     then it extracts matching thumbnails from snapshot packages and 
        ///     creates a thumbnail package and sends it to client.
        /// 2.  If the query is asking for selected photos, then it retrieves 
        ///     (using PackWebRequest) the selected photos from photo archive 
        ///     servers and sends them to the client in an XPS document.
        /// </summary>
        /// <remarks>
        /// The server-client communication is simulated by reading/writing 
        /// files in specific folders. The retail server looks at the 
        /// QueryDirectory (passed in as a parameter) for any query XML from 
        /// the client. The query XML has a <ReturnPath> element that tells 
        /// the server where to write response files to. 
        /// </remarks>
        /// <param name="QueryDirectory">
        /// The directory where the retail server looks for query XML files.
        /// </param>
        /// <param name="ArchiveInfoDirectory">
        /// The directory that holds snapshot packages from photo archive servers.
        /// </param>
        private static int Main(string[] args)
        {
            string queryBufferPath;
            string archiveInfoPath;

            if (!ValidateArguments(
                    args, out queryBufferPath, out archiveInfoPath))
            {
                Console.WriteLine("Invalid arguments!");
                return -1;
            }

            DirectoryInfo queryFolder = new DirectoryInfo(queryBufferPath);

            foreach (FileInfo fi in queryFolder.GetFiles("*.xml"))
            {
                ProcessQuery(fi, archiveInfoPath);
            }

            return 0;
        }

        private static bool ValidateArguments(string[] args,
                    out string queryBufferPath, out string archiveInfoPath)
        {
            queryBufferPath = null;
            archiveInfoPath = null;

            if (args.Length != 2)
            {
                ShowUsage();
                return false;
            }

            queryBufferPath = args[0];
            archiveInfoPath = args[1];

            if (!SharedLibrary.IsFolderExist(queryBufferPath))
                return false;

            if (!SharedLibrary.IsFolderExist(archiveInfoPath))
                return false;

            return true;
        }

        private static void ShowUsage()
        {
            Console.WriteLine("Handle queries from clients. ");
            Console.WriteLine("Usage:");
            Console.WriteLine("RetailServer QueryDirectory ArchiveInfoDirectory");
        }

        private static void ProcessQuery(FileInfo queryFileInfo, string archiveInfoPath)
        {
            Console.WriteLine("Received a query from the client.");
            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<Query Xml>:\n\t" + queryFileInfo.FullName);
            Console.WriteLine("------------------------------------------------------------------------------");

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(queryFileInfo.FullName);

            try
            {
                ParseQueryXml(xmlDoc, archiveInfoPath);
            }
            catch (XmlException e)
            {
                Console.WriteLine("The format of {0} is incorrect: {1}",
                    queryFileInfo.Name, e.Message);
            }
        }

        /// <summary>
        /// Following is a sample xml we are going to parse:
        /// <?xml version="1.0" encoding="utf-8" ?> 
        /// <Query xmlns="http://schemas.openxmlformats.org/package/2006/sdk-samples/packwebrequest">
        /// <ReturnPath>C:\Projects\SDKSample\PackWebRequestSample\Client\Download</ReturnPath> 
        /// <Selected>
        /// <Thumbnail Id="Server1-Snapshot.package.zip_6" /> 
        /// </Selected>
        /// </Query>
        /// </summary>
        private static void ParseQueryXml(XmlDocument xmlDoc, string archiveInfoPath)
        {
            XmlNode queryNode = xmlDoc.DocumentElement;
            if (string.CompareOrdinal(queryNode.NamespaceURI, SharedLibrary.Namespace) != 0)
            {
                throw new XmlException(
                    "The Query XML does not have the correct namespace.");
            }
            if (string.CompareOrdinal(queryNode.Name, "Query") != 0)
            {
                throw new XmlException(
                    "The Query XML should have <Query> node at the beginning.");
            }

            XmlNode returnPathNode = queryNode.FirstChild;

            if (returnPathNode == null ||
                string.CompareOrdinal(returnPathNode.Name, "ReturnPath") != 0)
            {
                throw new XmlException(
                    "The first node of <Query> should be <ReturnPath>.");
            }

            string returnPath = returnPathNode.InnerText;

            XmlNode queryInfoNode = returnPathNode.NextSibling;
            if (queryInfoNode == null)
            {
                throw new XmlException(
                    "Either node <Keyword> or <Selected> must present in the XML.");
            }

            if (string.CompareOrdinal(queryInfoNode.Name, "Keyword") == 0)
            {
                string keyword = queryInfoNode.InnerText;

                Console.WriteLine(
                    "This query requests for thumbnails with keyword '{0}'.",
                    keyword);
                SendThumbnailsToClient(keyword, returnPath, archiveInfoPath);
            }
            else if (string.CompareOrdinal(queryInfoNode.Name, "Selected") == 0)
            {
                XmlNodeList selectedThumbnails = queryInfoNode.ChildNodes;

                Console.WriteLine("This query requests for selected photos.");
                SendSelectedPhotosToClient(selectedThumbnails, returnPath,
                                            archiveInfoPath);
            }
            else
            {
                throw new XmlException(
                    "Either node <Keyword> or <Selected> must present in the XML.");
            }
        }

        private static void SendThumbnailsToClient(string keyword, string returnPath,
                                            string archiveInfoPath)
        {
            _currentId = 0;    // used to create a unique name of a thumbnail

            string thumbnailPackPath =
                returnPath + "\\" + SharedLibrary.ThumbnailPackName;

            using (Package thumbnailPack =
                Package.Open(thumbnailPackPath, FileMode.Create))
            {

                // The thumbnail package has an Info.xml part, which contains the 
                // ID of thumbnails.
                Uri infoXmlUri =
                    PackUriHelper.CreatePartUri(
                        new Uri(SharedLibrary.InfoXmlName, UriKind.Relative));

                PackagePart InfoXmlPart =
                    thumbnailPack.CreatePart(infoXmlUri,
                        SharedLibrary.InfoContentType,
                        CompressionOption.Normal);

                // Creates a relationship for the thumbnail package, which points 
                // to the Info.xml part.
                thumbnailPack.CreateRelationship(infoXmlUri, TargetMode.Internal,
                        SharedLibrary.InfoRelationshipType);

                using (XmlTextWriter infoXmlWriter =
                    new XmlTextWriter(InfoXmlPart.GetStream(), Encoding.UTF8))
                {
                    infoXmlWriter.WriteStartDocument();
                    infoXmlWriter.WriteStartElement("Thumbnails",
                        SharedLibrary.Namespace);

                    DirectoryInfo archiveInfoDir =
                        new DirectoryInfo(archiveInfoPath);

                    Console.WriteLine(
                        "Retrieving thumbnails with keyword '{0}' from snapshot package(s).",
                        keyword);
                    Console.WriteLine("------------------------------------------------------------------------------");

                    foreach (FileInfo fi in
                        archiveInfoDir.GetFiles("*" + SharedLibrary.PackageExt))
                    {
                        Console.WriteLine("<Snapshot Package>:\n\t" + fi.FullName);
                        ExtractThumbnailsByKeyword(fi, thumbnailPack, infoXmlWriter,
                                keyword);
                    }
                    Console.WriteLine("------------------------------------------------------------------------------");

                    infoXmlWriter.WriteEndDocument();
                }
            }

            Console.WriteLine("Successfully created a thumbnail package with retrieved thumbnails and sent it to the client.");
            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<Thumbnail Package>:\n\t" + thumbnailPackPath);
            Console.WriteLine("------------------------------------------------------------------------------");
        }

        private static void ExtractThumbnailsByKeyword(FileInfo packFile,
                                            Package thumbnailPack,
                                            XmlTextWriter infoXmlWriter,
                                            string keyword)
        {
            using (Package snapshotPack =
                    Package.Open(packFile.FullName, FileMode.Open, FileAccess.Read))
            {
                // Get the Info.xml part from the snapshot package.
                XmlDocument xmlDoc = SharedLibrary.GetInfoXml(snapshotPack);
                if (xmlDoc == null)
                {
                    Console.WriteLine(packFile.FullName + "is not in correct format!");
                    return; // invalid package, required part is missing
                }

                XmlNamespaceManager nsManager =
                    new XmlNamespaceManager(xmlDoc.NameTable);
                nsManager.AddNamespace("ns", SharedLibrary.Namespace);

                XmlNodeList nodelist =
                    xmlDoc.DocumentElement.SelectNodes("ns:Photos/ns:Photo",
                                                       nsManager);

                // Select thumbnails with matching keywords.
                foreach (XmlNode node in nodelist)
                {
                    string keywords = node.Attributes["Keywords"].InnerText;
                    if (MatchingKeyword(keywords, keyword))
                    {
                        string srcThumnailPartName =
                            node.Attributes["ThumbnailUri"].InnerText;

                        // Copy the thumbnail from snapshotPack to 
                        // thumbnailPack. destPartUri is the part Uri of the
                        // new thumbnail in thumbnailPack.
                        Uri destPartUri = CopyThumbnail(thumbnailPack,
                               snapshotPack, srcThumnailPartName);

                        if (destPartUri==null)
                            continue;   // CopyThumbnail() failed

                        string localId = node.Attributes["Id"].InnerText;

                        string thumbnailId =
                            GenerateThumbnailId(packFile.Name, localId);

                        infoXmlWriter.WriteStartElement("Thumbnail");
                        infoXmlWriter.WriteAttributeString("Id", thumbnailId);
                        infoXmlWriter.WriteAttributeString("PartUri",
                                destPartUri.ToString());
                        infoXmlWriter.WriteEndElement();
                    }
                }
            }
        }

        /// <summary>
        /// Check whether the string keywords contains the keyword.
        /// </summary>
        /// <param name="keywords">
        /// A line of keywords, separated by comma.
        /// </param>
        /// <param name="keyword">A keyword</param>
        /// <returns></returns>
        private static bool MatchingKeyword(string keywords, string keyword)
        {
            string[] keywordList = keywords.Split(new char[] { ',' });
            foreach (string word in keywordList)
            {
                if (string.CompareOrdinal(word, keyword) == 0)
                    return true;
            }

            return false;
        }

        private static Uri CopyThumbnail(Package destPackage, Package srcPackage,
                                    string srcThumnailPartName)
        {
            Uri srcThumbnailUri =
                new Uri(srcThumnailPartName, UriKind.Relative);

            if (!srcPackage.PartExists(srcThumbnailUri))
            {
                Console.WriteLine(srcThumnailPartName
                                + " doesn't exist!");
                return null;
            }

            PackagePart srcPart =
                srcPackage.GetPart(srcThumbnailUri);

            string extension = Path.GetExtension(srcThumnailPartName);

            Uri destPartUri =
                PackUriHelper.CreatePartUri(
                    new Uri(CreateThumbnailName(extension), UriKind.Relative));

            PackagePart destPart =
                destPackage.CreatePart(destPartUri, srcPart.ContentType);

            using (Stream srcStream = srcPart.GetStream(),
                    destStream = destPart.GetStream())
            {
                SharedLibrary.CopyStream(srcStream, destStream);
            } // end using (srcStream and destStream), close them.

            return destPartUri;
        }

        /// <summary>
        /// Generate a unique Id for the thumbnail in the thumbnail package. 
        /// </summary>
        private static string GenerateThumbnailId(string serverId, string thumbnailId)
        {
            return serverId + "_" + thumbnailId;
        }

        private static string CreateThumbnailName(string extension)
        {
            string name = "Thumbnail" + _currentId.ToString() + extension;
            _currentId++;
            return name;
        }

        private static void SendSelectedPhotosToClient(XmlNodeList selectedThumbnails,
                                                string returnPath,
                                                string archiveInfoPath)
        {
            if (selectedThumbnails.Count == 0)
                return;

            DirectoryInfo archiveInfoDir = new DirectoryInfo(archiveInfoPath);

            foreach (FileInfo fi in
                    archiveInfoDir.GetFiles("*" + SharedLibrary.PackageExt))
            {
                LoadArchiveInfo(fi);
            }

            string xpsFilePath = returnPath + "\\" + _photoXpsFileName;

            // Create an XPS document with selected photos.
            using (Package package = Package.Open(xpsFilePath, FileMode.Create))
            {
                XpsDocument xpsDocument = new XpsDocument(package);

                AddPackageContent(xpsDocument, selectedThumbnails);
                xpsDocument.Close();
            }

            Console.WriteLine("Successfully created an XPS document with client selected photos and sent it to the client.");
            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<XPS document>:\n\t" + xpsFilePath);
            Console.WriteLine("------------------------------------------------------------------------------");
        }

        /// <summary>
        /// Loads the snapshot package fi and updates the dictionaries
        /// _packInfoDict and _packUriDict.
        /// </summary>
        /// <param name="fi">FileInfo of the snapshot package</param>
        private static void LoadArchiveInfo(FileInfo fi)
        {
            using (Package snapshotPack =
                    Package.Open(fi.FullName, FileMode.Open, FileAccess.Read))
            {
                // Get the Info.xml part from the snapshot package.
                XmlDocument infoXml = SharedLibrary.GetInfoXml(snapshotPack);
                if (infoXml == null)
                {
                    Console.WriteLine(fi.FullName + "is not in correct format!");
                    return; // invalid package, required part is missing
                }

                XmlElement packInfoNode = infoXml.DocumentElement;

                // A dictionary that maps snapshot package file name to its
                // Info.xml
                _packInfoDict.Add(fi.Name, infoXml);

                Uri photoPackUri =
                    new Uri(packInfoNode.Attributes["SourcePackUri"].InnerText);

                // A dictionary that maps snapshot package file name to the
                // pack Uri of the photo package on Photo Archive Server
                _packUriDict.Add(fi.Name, photoPackUri);
            }
        }

        private static void DecodeClientSideThumbnailId(string id,
                                                out string serverId,
                                                out string thumbnailId)
        {
            // The thumbnail ID sent to the client is in such format:
            // "serverId_thumbnailId".
            string[] split = id.Split(new char[] { '_' });

            if (split.Length != 2)
            {
                throw new ArgumentException("Client side thumbnail ID " + id
                    + " is not in correct format!", "id");
            }

            serverId = split[0];
            thumbnailId = split[1];
        }

        /// <summary>
        ///   Adds a predefined set of content to a given XPS document.
        /// </summary>
        /// <param name="xpsDocument">
        ///   The package to add the document content to.
        /// </param>
        private static void AddPackageContent(XpsDocument xpsDocument,
                                    XmlNodeList selectedThumbnails)
        {
            // Add a FixedDocumentSequence at the Package root.
            IXpsFixedDocumentSequenceWriter documentSequenceWriter =
                xpsDocument.AddFixedDocumentSequence();

            // Add a FixedDocument to the FixedDocumentSequence.
            IXpsFixedDocumentWriter fixedDocumentWriter =
                documentSequenceWriter.AddFixedDocument();

            // Add content to the document.
            AddDocumentContent(fixedDocumentWriter, selectedThumbnails);

            // Commit the Document.
            fixedDocumentWriter.Commit();

            // Commit the FixedDocumentSequence.
            documentSequenceWriter.Commit();
        }

        private static void AddDocumentContent(IXpsFixedDocumentWriter fixedDocumentWriter,
                                        XmlNodeList selectedThumbnails)
        {
            Console.WriteLine("Using PackWebRequest and PackWebResponse APIs to retrieve selected photos ...");
            Console.WriteLine("------------------------------------------------------------------------------");

            foreach (XmlNode node in selectedThumbnails)
            {
                Uri packageUri;
                Uri partUri;
                string selectedId = node.Attributes["Id"].InnerText;

                try
                {
                    GetSelectedPhotoUri(selectedId, out packageUri, out partUri);
                }
                catch (Exception e)
                {
                    // We catch all exceptions because we don't want any 
                    // exception caused by invalid input crash our server.
                    // But we record all exceptions, and try to continue.
                    Console.WriteLine("Hit exception while resolving photo Uri "
                    + "for selected thumbnail ID " + selectedId);
                    Console.WriteLine("Exception:");
                    Console.WriteLine(e.ToString());
                    continue;
                }

                // Using PackWebRequest to retrieve the photo.
                using (WebResponse response =
                    PackWebRequestForPhotoPart(packageUri, partUri))
                {

                    Stream s = response.GetResponseStream();

                    // Add a fixed page into the fixed document.
                    IXpsFixedPageWriter fixedPageWriter =
                        fixedDocumentWriter.AddFixedPage();

                    // Add the photo image resource to the page.
                    XpsImage xpsImage =
                        fixedPageWriter.AddImage(response.ContentType);

                    SharedLibrary.CopyStream(s, xpsImage.GetStream());

                    xpsImage.Commit();

                    s.Seek(0, SeekOrigin.Begin);

                    // Use BitmapFrame to get the dimension of the photo image.
                    BitmapFrame imgFrame = BitmapFrame.Create(s);

                    // Write the XML description of the fixed page.
                    WritePageContent(fixedPageWriter.XmlWriter, xpsImage.Uri,
                                imgFrame);

                    fixedPageWriter.Commit();
                }
            }
            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("All selected photos retrieved.");
        }

        private static void GetSelectedPhotoUri(string selectedId, out Uri packageUri,
                                        out Uri partUri)
        {
            string serverId;
            string thumbnailId;

            DecodeClientSideThumbnailId(selectedId, out serverId,
                                    out thumbnailId);

            XmlDocument infoXml = _packInfoDict[serverId];

            XmlNamespaceManager nsManager =
                new XmlNamespaceManager(infoXml.NameTable);
            nsManager.AddNamespace("ns", SharedLibrary.Namespace);

            XmlNodeList targetNodeList =
                infoXml.DocumentElement.SelectNodes(
                    "ns:Photos/ns:Photo[@Id='" + thumbnailId + "']",
                    nsManager);

            if (targetNodeList.Count != 1)
            {
                string message = 
                    string.Format("Thumbnail ID {0} is not unique in {1}!",
                        thumbnailId, serverId);

                throw new Exception(message);
            }

            string photoPartName =
                targetNodeList[0].Attributes["PartUri"].InnerText;

            // The absolute Uri of photo package
            packageUri = _packUriDict[serverId];

            // The part Uri of the photo in photo package
            partUri = new Uri(photoPartName, UriKind.Relative);
        }

        /// <summary>
        /// Using PackWebRequest to retrieve the photo from photo package on 
        /// archive server.
        /// </summary>
        /// <param name="packageUri">
        /// Absolute Uri of the photo package on archive server.
        /// </param>
        /// <param name="partUri">
        /// Part Uri of the photo in the photo package.
        /// </param>
        /// <returns>
        /// WebResponse that has the stream of requested photo image.
        /// </returns>
        private static WebResponse PackWebRequestForPhotoPart(Uri packageUri,
                                                      Uri partUri)
        {
            Uri fullUri = PackUriHelper.Create(packageUri, partUri);

            Console.WriteLine("Pack URI= " + fullUri);

            // Create PackWebRequest.
            WebRequest request = _webRequestCreate.Create(fullUri);

            return request.GetResponse();
        }

        /// <summary>
        /// Write the fixed page XML description to make it display the photo 
        /// on the whole page.
        /// </summary>
        private static void WritePageContent(XmlWriter xmlWriter, Uri imageUri,
                                     BitmapFrame imgFrame)
        {
            xmlWriter.WriteStartElement("FixedPage");

            string strPageWidth = imgFrame.PixelWidth.ToString();
            string strPageHeight = imgFrame.PixelHeight.ToString();
            string strImgWidth = imgFrame.Width.ToString();
            string strImgHeight = imgFrame.Height.ToString();

            xmlWriter.WriteAttributeString("Width", strPageWidth);
            xmlWriter.WriteAttributeString("Height", strPageHeight);
            xmlWriter.WriteAttributeString("xmlns",
                "http://schemas.microsoft.com/xps/2005/06");
            xmlWriter.WriteAttributeString("xml:lang", "en-US");

            xmlWriter.WriteStartElement("Path");
            xmlWriter.WriteAttributeString("Data", "M 0,0 L " + strPageWidth
                            + ",0 " + strPageWidth + "," + strPageHeight
                            + " 0," + strPageHeight + " z");
            xmlWriter.WriteStartElement("Path.Fill");
            xmlWriter.WriteStartElement("ImageBrush");
            xmlWriter.WriteAttributeString("ImageSource", imageUri.ToString());
            xmlWriter.WriteAttributeString("Viewbox", "0,0," + strImgWidth
                            + "," + strImgHeight);
            xmlWriter.WriteAttributeString("TileMode", "FlipX");
            xmlWriter.WriteAttributeString("ViewboxUnits", "Absolute");
            xmlWriter.WriteAttributeString("ViewportUnits", "Absolute");
            xmlWriter.WriteAttributeString("Viewport", "0,0," + strPageWidth
                            + "," + strPageHeight);

            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
        }
    }
}
