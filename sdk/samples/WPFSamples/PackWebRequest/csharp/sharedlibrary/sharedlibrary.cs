// PackWebRequest SDK Sample - SharedLibrary.cs
// Copyright (c) Microsoft Corporation. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.IO.Packaging;

namespace PackWebRequestSample
{
    /// <summary>
    /// This class provides some common definitions and utility methods
    /// shared by other projects in this solution.
    /// </summary>
    public static class SharedLibrary
    {
        public static string PackageExt
        {
            get
            {
                return _packageExt;
            }
        }

        public static string InfoXmlName
        {
            get
            {
                return _infoXmlName;
            }
        }

        public static string InfoRelationshipType
        {
            get
            {
                return _relationshipType;
            }
        }

        public static string InfoContentType
        {
            get
            {
                return _infoContentType;
            }
        }

        public static string Namespace
        {
            get
            {
                return _namespace;
            }
        }

        public static string ThumbnailPackName
        {
            get
            {
                return _thumbnailPackName;
            }
        }

        public static bool IsFolderExist(string folderPath)
        {
            if (folderPath == null)
                return false;

            DirectoryInfo dir;

            try
            {
                dir = new DirectoryInfo(folderPath);
            }
            catch (ArgumentException e)
            {
                Console.WriteLine("The folder path {0} causes the {1} exception:",
                    folderPath, e.GetType().ToString());
                Console.WriteLine("Exception Message: {0}", e.Message);
                return false;
            }

            if (!dir.Exists)
            {
                Console.WriteLine("Folder " + folderPath + " does not exist!");
                return false;
            }

            return true;
        }

        /// <summary>
        /// Get the information part in the package. The part is specified by 
        /// a specific relationship of the package. It has a specific 
        /// content type and namespace.
        /// </summary>
        /// <param name="pack">
        /// The package from which to retrieve the information part.
        /// </param>
        /// <returns>The XmlDocument of the information part.</returns>
        public static XmlDocument GetInfoXml(Package package)
        {
            if (package == null)
            {
                throw new ArgumentNullException("package");
            }

            PackagePart infoXmlPart = null;
            PackageRelationshipCollection prc = package.GetRelationships();

            foreach (PackageRelationship pr in prc)
            {
                if (string.CompareOrdinal(pr.RelationshipType,
                        _relationshipType) == 0)
                {
                    PackagePart part;
                    Uri partUri = pr.TargetUri;

                    if (package.PartExists(partUri))
                    {
                        part = package.GetPart(partUri);
                    }
                    else
                        break;

                    // Make sure the content type is expected.
                    if (string.CompareOrdinal(part.ContentType,
                            _infoContentType) == 0)
                    {
                        infoXmlPart = part;
                        break;
                    }
                }
            }

            if (infoXmlPart == null)
            {
                Console.WriteLine("This package does not contain a required part. ");
                return null;
            }

            XmlDocument xmlDoc = new XmlDocument();
            using (Stream partStream = infoXmlPart.GetStream())
            {
                xmlDoc.Load(partStream);
            } // end using partStream, it will be closed

            XmlElement packInfoNode = xmlDoc.DocumentElement;
            if (string.CompareOrdinal(packInfoNode.NamespaceURI,
                    _namespace) != 0)
            {
                Console.WriteLine(infoXmlPart.Uri +
                    " in the package does not have the correct XML namespace. ");
                return null;
            }

            return xmlDoc;
        }

        public static void CopyStream(Stream source, Stream target)
        {
            int bytesRead = 0;

            while ((bytesRead = source.Read(_copyBuffer, 0, _copyBufferSize)) > 0)
                target.Write(_copyBuffer, 0, bytesRead);
        }

        private const int _copyBufferSize = 4096;
        private static byte[] _copyBuffer = new byte[_copyBufferSize];

        // File extension of the package file
        // Append the ".zip" after ".package" to make the file openable by 
        // WinZip to encourage people using this sample to inspect and 
        // understand the inner structure of the package files in this sample. 
        // But please keep in mind that they are OPC files, not just ordinary 
        // zip files.
        private const string _packageExt = ".package.zip";

        private const string _infoXmlName = "Info.xml";

        private const string _relationshipType =
            "http://schemas.openxmlformats.org/package/2006/relationships/sdk-samples/packwebrequest/packageinfo";

        private const string _infoContentType =
            "application/vnd.openxmlformats-package.sdk-samples.packwebrequest.packageinfo";

        private const string _namespace =
            "http://schemas.openxmlformats.org/package/2006/sdk-samples/packwebrequest";

        private const string _thumbnailPackName =
                                "Thumbnails" + _packageExt;
    }
}
