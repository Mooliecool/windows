// PackWebRequest SDK Sample - Client.cs
// Copyright (c) Microsoft Corporation. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;
using System.IO.Packaging;
using PackWebRequestSample;

namespace PackWebRequestSample
{
    class Client
    {
        private const string _queryFileName = "Query.xml";

        /// <summary>
        /// This application simulates the client in the sample scenario.
        /// It sends query to retail server.
        /// 
        /// Usage:
        /// Cient -k keyword | -s OutputDir DownloadDir
        /// -k keyword:     Generate request for thumbnails of photos matching 
        ///                 the keyword.
        /// -s:             Randomly select thumbnails in the thumbnail package 
        ///                 from retail server, and generate request for the 
        ///                 selected photos.
        /// </summary>
        private static int Main(string[] args)
        {
            string keyword;
            string outputPath;
            string downloadBufferPath;

            if (!ValidateArguments(
                args, out keyword, out outputPath, out downloadBufferPath))
            {
                Console.WriteLine("Invalid arguments!");
                return -1;
            }

            if (keyword != null)
            {
                GenerateThumbnailRequest(keyword, outputPath, downloadBufferPath);
            }
            else
            {
                GenerateDownloadRequest(outputPath, downloadBufferPath);
            }
            return 0;
        }

        private static void ShowUsage()
        {
            Console.WriteLine("Generate query to the retail server. ");
            Console.WriteLine("Usage:");
            Console.WriteLine("Cient -k keyword | -s OutputDir DownloadDir");
            Console.WriteLine("-k keyword: \tGenerate request for thumbnails of photos matching the keyword.");
            Console.WriteLine("-s: \tGenerate request for the selected photos.");
            Console.WriteLine("OutputDir: \tThe directory to place the query xml file.");
            Console.WriteLine("DownloadDir: \tThe directory to place the query result file from retail server.");
        }

        private static bool ValidateArguments(string[] args,
                                            out string keyword,
                                            out string outputPath,
                                            out string downloadBufferPath)
        {
            keyword = null;
            outputPath = null;
            downloadBufferPath = null;

            switch (args.Length)
            {
                case 3:
                    if (string.CompareOrdinal(args[0], "-s") != 0)
                    {
                        ShowUsage();
                        return false;
                    }
                    outputPath = args[1];
                    downloadBufferPath = args[2];
                    break;
                case 4:
                    if (string.CompareOrdinal(args[0], "-k") != 0)
                    {
                        ShowUsage();
                        return false;
                    }
                    keyword = args[1];
                    outputPath = args[2];
                    downloadBufferPath = args[3];
                    break;
                default:
                    ShowUsage();
                    return false;
            }

            if (!SharedLibrary.IsFolderExist(outputPath))
                return false;

            if (!SharedLibrary.IsFolderExist(downloadBufferPath))
                return false;

            return true;
        }

        private static void GenerateThumbnailRequest(string keyword,
                                            string outputPath,
                                            string downloadBufferPath)
        {
            string queryFilePath = outputPath + "\\" + _queryFileName;

            using (XmlTextWriter xmlWriter =
                new XmlTextWriter(queryFilePath, Encoding.UTF8))
            {

                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("Query", SharedLibrary.Namespace);

                // <ReturnPath> tells where the retail server should write response 
                // file to. Using <ReturnPath> in the query xml enables simulation 
                // of scenarios that have multiple clients.
                xmlWriter.WriteElementString("ReturnPath", downloadBufferPath);
                xmlWriter.WriteElementString("Keyword", keyword);
                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndDocument();
            } // end using xmlWriter, close it

            Console.WriteLine("Successfully sent query to retail server "
                        + "asking for thumbnails with keyword: " + keyword);

            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<Query Xml>:\n\t" + queryFilePath);
            Console.WriteLine("------------------------------------------------------------------------------");
        }

        private static bool GenerateDownloadRequest(string outputPath,
                                            string downloadBufferPath)
        {
            XmlNodeList thumbnailNodes = ReadThumbnailPack(downloadBufferPath);

            if (thumbnailNodes == null)
                return false;

            string queryFilePath = outputPath + "\\" + _queryFileName;

            using (XmlTextWriter xmlWriter =
                new XmlTextWriter(queryFilePath, Encoding.UTF8))
            {

                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("Query", SharedLibrary.Namespace);
                xmlWriter.WriteElementString("ReturnPath", downloadBufferPath);
                xmlWriter.WriteStartElement("Selected");

                Random rand = new Random(unchecked((int)DateTime.Now.Ticks));
                foreach (XmlNode node in thumbnailNodes)
                {
                    // We randomly select 50% of the thumbnails.
                    if (rand.NextDouble() < 0.5)
                    {
                        // Select the thumbnail.
                        string Id = node.Attributes["Id"].InnerText;
                        xmlWriter.WriteStartElement("Thumbnail");
                        xmlWriter.WriteAttributeString("Id", Id);
                        xmlWriter.WriteEndElement();
                    }
                }

                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndDocument();
            } // end using xmlWriter, close it

            Console.WriteLine("Client selected thumbnails in the thumbnail package "
            + "and sent a query to retail server to download selected photos.");

            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<Query Xml>:\n\t" + queryFilePath);
            Console.WriteLine("------------------------------------------------------------------------------");

            return true;
        }

        private static XmlNodeList ReadThumbnailPack(string downloadBufferPath)
        {
            string thumbnailPath =
                downloadBufferPath + "\\" + SharedLibrary.ThumbnailPackName;

            using (Package thumbnailPack =
                Package.Open(thumbnailPath, FileMode.Open, FileAccess.Read))
            {
                Console.WriteLine("Got a thumbnail package from retail server. ");
                Console.WriteLine("------------------------------------------------------------------------------");
                Console.WriteLine("<Thumbnail package>:\n\t" + thumbnailPath);
                Console.WriteLine("------------------------------------------------------------------------------");

                XmlDocument xmlDoc = SharedLibrary.GetInfoXml(thumbnailPack);
                if (xmlDoc == null)
                    return null;

                return xmlDoc.GetElementsByTagName("Thumbnail");
            }
        }
    }
}
