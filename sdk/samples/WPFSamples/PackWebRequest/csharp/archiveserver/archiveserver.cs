// PackWebRequest SDK Sample - ArchiveServer.cs
// Copyright (c) Microsoft Corporation. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Packaging;
using System.Xml;
using System.Windows.Media.Imaging;
using System.Drawing;
using System.Drawing.Imaging;

namespace PackWebRequestSample
{
    class ArchiveServer
    {
        private const string _keywordFileName = "Keywords.txt";
        private static string _archiveSnapshotName =
                    "Snapshot" + SharedLibrary.PackageExt;
                         
        // A counter that is used to give each thumbnail a unique ID 
        // and name. 
        private static int _currentId;

        /// <summary>
        /// This application simulates the photo archive server in the sample 
        /// scenario.
        /// 1. It creates a photo package of the photos in the photo directory.
        /// 2. It creates a snapshot package with thumbnails and information 
        ///    xml of the photos.
        /// 3. The snapshot package will be created in the output directory.
        /// </summary>
        /// <remarks>
        /// Photo archive server holds photo archives. It is simulated by 
        /// creating photo package and storing it in the archive server local 
        /// folder. The snapshot package will be sent to retail server. It is 
        /// simulated by creating the package in a local folder of the retail 
        /// server, which will be the OutputDirectory parameter passed in to 
        /// the application.
        /// </remarks>
        /// <param name="PhotoDirectory">The directory of photos.</param>
        /// <param name="OutputDirectory">
        /// The output directory for snapshot package.
        /// </param>
        private static int Main(string[] args)
        {
            string photoPath;
            string outputPath;

            if (!ValidateArguments(args, out photoPath, out outputPath))
            {
                Console.WriteLine("Invalid arguments!");
                return -1;
            }

            CreateArchive(photoPath, outputPath);

            return 0;
        }

        private static bool ValidateArguments(string[] args,
                            out string photoPath, out string outputPath)
        {
            photoPath = null;
            outputPath = null;

            if (args.Length != 2)
            {
                ShowUsage();
                return false;
            }

            photoPath = args[0];
            outputPath = args[1];

            if (!SharedLibrary.IsFolderExist(photoPath))
                return false;

            if (!SharedLibrary.IsFolderExist(outputPath))
                return false;

            return true;
        }

        private static void ShowUsage()
        {
            Console.WriteLine("Usage:");
            Console.WriteLine("ArchiveServer PhotoDirectory OutputDirectory");
            Console.WriteLine("This appliacation simulates the photo archive server in the sample scenario.");
            Console.WriteLine("1. It creates a photo package of the photos in the photo directory.");
            Console.WriteLine("2. It creates a snapshot package with thumbnails and information xml of the photos.");
            Console.WriteLine("3. The snapshot package is created in the output directory.");
        }

        /// <summary>
        /// Creates a photo package and a snapshot package.
        /// </summary>
        private static void CreateArchive(string photoPath, string outputPath)
        {
            DirectoryInfo photoDir = new DirectoryInfo(photoPath);
            DirectoryInfo serverDir = photoDir.Parent;

            // Suppose the path of photo directory is in this format:
            // ...\Server1\Photos
            // Then the name of photo package is: Photos.package.zip
            // The name of snapshot package is:   Server1-Snapshot.package.zip
            string photoPackagePath =
                serverDir.FullName + "\\" + photoDir.Name
                + SharedLibrary.PackageExt;

            string archiveSnapshotPath =
                outputPath + "\\" + serverDir.Name + "-" + _archiveSnapshotName;

            Package photoPackage =
                Package.Open(photoPackagePath, FileMode.Create);

            Package snapshotPackage =
                Package.Open(archiveSnapshotPath, FileMode.Create);

            // The snapshot package has an Info.xml part, which contains the 
            // information of photos and thumbnails.
            Uri infoXmlUri =
                PackUriHelper.CreatePartUri(
                    new Uri(SharedLibrary.InfoXmlName, UriKind.Relative));

            PackagePart InfoXmlPart =
                snapshotPackage.CreatePart(infoXmlUri,
                                           SharedLibrary.InfoContentType,
                                           CompressionOption.Normal);

            // Creates a relationship for the snapshot package, which points 
            // to the Info.xml part.
            snapshotPackage.CreateRelationship(infoXmlUri, TargetMode.Internal,
                                          SharedLibrary.InfoRelationshipType);

            using (XmlTextWriter infoXmlWriter =
                new XmlTextWriter(InfoXmlPart.GetStream(), Encoding.UTF8))
            {
                infoXmlWriter.WriteStartDocument();

                Uri packUri = new Uri(photoPackagePath);
                infoXmlWriter.WriteStartElement("PackInfo", SharedLibrary.Namespace);

                // Writes the pack Uri of the photo package to the SourcePackUri 
                // attribute.
                infoXmlWriter.WriteAttributeString("SourcePackUri",
                        packUri.ToString());
                infoXmlWriter.WriteStartElement("Photos");

                // A counter that is used to give each thumbnail a unique ID 
                // and name. Start the counter from 0.
                _currentId = 0;

                // Photos are stored in subfolders based on categories.
                // For example, photos of dogs are stored in Photos\Dogs\
                foreach (DirectoryInfo subFolder in photoDir.GetDirectories())
                {
                    ArchiveSubFolderPhotos(photoPackage, snapshotPackage,
                                  infoXmlWriter, subFolder);
                }

                infoXmlWriter.WriteEndDocument();
            }   //end:using(infoXmlWriter) - Close and dispose infoXmlWriter.

            photoPackage.Close();
            snapshotPackage.Close();

            Console.WriteLine("Successfully created archive package for photo folder on Photo Archive Server.");
            Console.WriteLine("Successfully sent snapshot package to Retail Server.");
            Console.WriteLine("------------------------------------------------------------------------------");
            Console.WriteLine("<Photo Folder>:\n\t" + photoPath);
            Console.WriteLine("<Archive package>:\n\t" + photoPackagePath);
            Console.WriteLine("<Snapshot Package>:\n\t" + archiveSnapshotPath);
            Console.WriteLine("------------------------------------------------------------------------------");

        }

        private static void ArchiveSubFolderPhotos(Package photoPackage,
                                           Package snapshotPackage,
                                           XmlTextWriter infoXmlWriter,
                                           DirectoryInfo subFolder)
        {
            string keywords = ReadKeywords(subFolder);

            foreach (FileInfo photoFile in subFolder.GetFiles("*.jpg"))
            {
                // Add a thumbnail of the photo to snapshot package.
                Uri thumbnailUri = AddThumbnail(snapshotPackage, photoFile);

                // Add the photo to photo package.
                Uri photoPartUri = AddPhoto(photoPackage, subFolder, photoFile);

                // Write snapshot information to the Info.xml file.
                infoXmlWriter.WriteStartElement("Photo");
                infoXmlWriter.WriteAttributeString("Id", _currentId.ToString());

                // PartUri is the part Uri of the photo in photo package.
                infoXmlWriter.WriteAttributeString("PartUri",
                                                   photoPartUri.ToString());

                // ThumbnailUri is the part Uri of the thumbnail in 
                // snapshot package.
                infoXmlWriter.WriteAttributeString("ThumbnailUri",
                                                   thumbnailUri.ToString());

                infoXmlWriter.WriteAttributeString("Keywords", keywords);
                infoXmlWriter.WriteEndElement();

                _currentId++;
            }
        }

        private static string ReadKeywords(DirectoryInfo subFolder)
        {
            // There is a keyword text file in each category folder. It 
            // contains the keywords of photos in that folder.
            using (StreamReader sr =
                new StreamReader(subFolder.FullName + "\\" + _keywordFileName))
            {
                // Multiple keywords are in one line, separated by comma.
                return sr.ReadLine();
            }
        }

        private static Uri AddThumbnail(Package snapshotPackage,
                                FileInfo photoFile)
        {
            // Create a thumbnail of the photo.
            using (Bitmap thumbnail = CreateThumbnail(photoFile.FullName, 200))
            {
                string thumbnailName =
                    "Thumbnail" + _currentId.ToString() + ".jpg";

                Uri thumbnailUri = PackUriHelper.CreatePartUri(
                      new Uri(thumbnailName, UriKind.Relative));

                // Add the thumbnail to the snapshotPackage.
                PackagePart thumbnailPart =
                    snapshotPackage.CreatePart(thumbnailUri,
                        System.Net.Mime.MediaTypeNames.Image.Jpeg);

                using (Stream thumbnailPartStream = thumbnailPart.GetStream())
                {
                    thumbnail.Save(thumbnailPartStream, ImageFormat.Jpeg);
                } // end:using(thumbnailPartStream) - Close and dispose it.

                return thumbnailUri;
            } // Dispose thumbnail when it leaves this scope
        }

        private static Uri AddPhoto(Package photoPackage,
                            DirectoryInfo subFolder, FileInfo photoFile)
        {
            string photoPartName = subFolder.Name + "/" + photoFile.Name;
            Uri photoPartUri = PackUriHelper.CreatePartUri(
                    new Uri(photoPartName, UriKind.Relative));

            // Add the photo to photo package.
            PackagePart photoPart =
                photoPackage.CreatePart(photoPartUri,
                    System.Net.Mime.MediaTypeNames.Image.Jpeg);

            // Copy the photo image data to the photo Part.
            using (FileStream fileStream =
                new FileStream(photoFile.FullName, FileMode.Open, FileAccess.Read))
            {
                using (Stream photoPartStream = photoPart.GetStream())
                {
                    SharedLibrary.CopyStream(fileStream, photoPartStream);
                }// end:using(photoPartStream) - Close and dispose it.
            }// end:using(fileStream) - Close and dispose fileStream.

            return photoPartUri;
        }

        private static Bitmap CreateThumbnail(string imagePath,
                                            int thumbnailWidth)
        {
            using (Bitmap originalBitmap = new Bitmap(imagePath))
            {
                double ratio = 1.0 * thumbnailWidth / originalBitmap.Width;
                int thumbnailHeight = (int)(ratio * originalBitmap.Height);

                // Create a scaled bitmap (thumbnail) from the original one.
                return new Bitmap(originalBitmap, thumbnailWidth, thumbnailHeight);
            }
        }
    }
}
