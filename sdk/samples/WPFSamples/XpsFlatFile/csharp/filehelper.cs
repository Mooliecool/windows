// XpsFlatFile SDK Sample - FileHelper.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;


namespace XpsFlatFile
{
    /// <summary>
    ///   This is a basic file helper class for the Packaging Policy example.
    ///   This class is used to generate file names based on type, creating the
    ///   files, and getting a file extension based on type.  This class is not
    ///   required in order to override the BasePackagingPolicy.  It is used as
    ///   a resource for the implementation of this sample.</summary>
    public class FileHelper
    {
        /// <summary>
        ///   Here we generate the file name based on the Xps File Type.  We also
        ///   create the directory where the file will end up being saved.</summary>
        /// <param name="fileType">Type of Xps part</param>
        /// <param name="outputDirectory">Directory where the file will be stored</param>
        /// <returns>Name of file</returns>
        public static String GenerateFileName(XpsFileType fileType, String outputDirectory)
        {
            string fileName;
            string path;
            switch (fileType)
            {
                case XpsFileType.FixedDocumentSequence:
                    //There will be only 1 FixedDocumentSequence and it will be at the root
                    path = null;
                    fileName = _documentSequenceName + "." + _documentSequenceExtension;
                    break;
                case XpsFileType.FixedDocument:
                    path = "Documents/";
                    fileName = path + _fixedDocumentName + "_" + _fixedDocumentCount + "." + _fixedDocumentExtension;
                    _fixedDocumentCount++;
                    break;
                case XpsFileType.FixedPage:
                    path = "Pages/";
                    fileName = path + _fixedPageName + "_" + _fixedPageCount + "." + _fixedPageExtension;
                    _fixedPageCount++;
                    break;
                case XpsFileType.ImagePng:
                    path = "Resources/Images/";
                    fileName = path + _imageName + "_" + _imageCount + "." + _imagePngExtension;
                    _imageCount++;
                    break;
                case XpsFileType.Font:
                    Guid fontguid = Guid.NewGuid();
                    String fontString = fontguid.ToString();

                    path = "Resources/Fonts/";
                    fileName = path + fontString + "." + _obfuscatedFontExtension;
                    break;
                case XpsFileType.ColorContext:
                    Guid colorContextGuid = Guid.NewGuid();
                    String colorContextString = colorContextGuid.ToString();

                    path = "Resources/ColorContexts/";
                    fileName = path + colorContextString + "." + _colorContextExtension;
                    break;
                case XpsFileType.ResourceDictionary:
                    Guid resourceDictionaryGuid = Guid.NewGuid();
                    String resourceDictionaryString = resourceDictionaryGuid.ToString();

                    path = "Resources/ResourceDictionaries/";
                    fileName = path + resourceDictionaryString + "." + _resourceDictionaryExtension;
                    break;
                case XpsFileType.Relationship:
                    fileName = null;
                    path = null;
                    break;
                default:
                    throw new ArgumentException("File Type is not valid");
            }

            if (path != null)
            {
                String fullPath = outputDirectory + "/" + path;
                if (Directory.Exists(fullPath) != true)
                {
                    Directory.CreateDirectory(fullPath);
                }
            }
            return fileName;
        }// end:GenerateFileName


        /// <summary>
        ///   Creates a file given the filename and path.</summary>
        /// <param name="fileName">File name of the file to be created</param>
        /// <param name="outputPath">Path of the file to be created</param>
        /// <returns></returns>
        public static FileStream CreateFile(String fileName, String outputPath)
        {
            int lastChar = outputPath.Length - 1;
            StringBuilder tmpPath = new StringBuilder(outputPath);
            if (outputPath[lastChar] != '/')
            {
                tmpPath.Append("/");
            }
            tmpPath.Append(fileName);
            string path = tmpPath.ToString();
            return File.Create(path);
        }


        /// <summary>
        /// Returns the extension of the XpsFileType
        /// </summary>
        /// <param name="fileType">The type of extension returned</param>
        /// <returns></returns>
        public static String GetExtension(XpsFileType fileType)
        {
            string extension;
            switch (fileType)
            {
                case XpsFileType.FixedDocumentSequence:
                    extension = _documentSequenceExtension;
                    break;
                case XpsFileType.FixedDocument:
                    extension = _fixedDocumentExtension;
                    break;
                case XpsFileType.FixedPage:
                    extension = _fixedPageExtension;
                    break;
                case XpsFileType.Relationship:
                    extension = _relationshipExtension;
                    break;
                case XpsFileType.ImagePng:
                    extension = _imagePngExtension;
                    break;
                case XpsFileType.ImageTif:
                    extension = _imageTifExtension;
                    break;
                case XpsFileType.ImageJpg:
                    extension = _imageJpgExtension;
                    break;
                case XpsFileType.ImageWmp:
                    extension = _imageWmpExtension;
                    break;
                case XpsFileType.Font:
                    extension = _fontExtension;
                    break;
                case XpsFileType.ObfuscatedFont:
                    extension = _obfuscatedFontExtension;
                    break;
                case XpsFileType.ResourceDictionary:
                    extension = _resourceDictionaryExtension;
                    break;
                case XpsFileType.ColorContext:
                    extension = _colorContextExtension;
                    break;
                default:
                    throw new ArgumentException("Invalid XpsFileType");
            }
            return extension;
        }


        #region Private Members

        //FileNames

        static private string _documentSequenceName = "FixedDocumentSequence";
        static private string _fixedDocumentName = "FixedDocument";
        static private string _fixedPageName = "FixedPage";
        static private string _imageName = "Image";

        //File Extensions

        static private string _documentSequenceExtension = "fdseq";
        static private string _fixedDocumentExtension = "fdoc";
        static private string _fixedPageExtension = "fpage";
        static private string _imagePngExtension = "png";
        static private string _imageJpgExtension = "jpg";
        static private string _imageWmpExtension = "wdp";
        static private string _imageTifExtension = "tif";
        static private string _fontExtension = "ttf";
        static private string _obfuscatedFontExtension = "ODTTF";
        static private string _resourceDictionaryExtension = "dict";
        static private string _colorContextExtension = "icc";
        static private string _relationshipExtension = "rels";

        private static int _fixedDocumentCount = 1;
        private static int _fixedPageCount = 1;
        private static int _imageCount = 1;

        #endregion Private Members

    }// end:class FileHelper

}// end:namespace XpsFlatFile
