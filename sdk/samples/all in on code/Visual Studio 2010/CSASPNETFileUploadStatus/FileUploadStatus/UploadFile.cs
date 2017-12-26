/****************************** Module Header ******************************\
* Module Name:    UploadFile.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* This is a class for the uploading file in the request entity. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.IO;
using System.Web;


namespace CSASPNETFileUploadStatus
{
    public class UploadFile
    {
        private string cachePath = null;
        private int cacheLength = 1024 * 1024 * 5;

        public string FileName { get; private set; }
        public string ContentType { get; private set; }
        public string ClientPath { get; private set; }
        private byte[] Data { get; set; }

        private readonly string _defaultUploadFolder = "UploadedFiles";

        public UploadFile(string clientPath, string contentType)
        {
            Data = new byte[0];
            ClientPath = clientPath;
            ContentType = contentType;
            cachePath = HttpContext.Current.Server.MapPath("uploadcaching") +
                        "\\" + Guid.NewGuid().ToString();
            FileName = new FileInfo(clientPath).Name;
            FileInfo cache_file = new FileInfo(cachePath);
            if (!cache_file.Directory.Exists)
            {
                cache_file.Directory.Create();
            }
        }

        // For large files we need to read and store the data partially.
        // And this method is used to combine the partial data blocks.
        internal void AppendData(byte[] data)
        {
            this.Data = BinaryHelper.Combine(this.Data, data);
            if (this.Data.Length > cacheLength)
            {
                CacheData();
            }
        }

        // We could store the data which has already read into 
        // disk to release the data in memory.
        private void CacheData()
        {
            if (this.Data != null && this.Data.Length > 0)
            {

                using (FileStream fs = new FileStream(
                     cachePath, FileMode.Append, FileAccess.Write))
                {
                    fs.Write(Data, 0, Data.Length);
                    this.Data = new byte[0];
                }
            }
        }

        // Clear the template file
        internal void ClearCache()
        {
            if (File.Exists(cachePath))
            {
                File.Delete(cachePath);
            }
        }


        // Save the uploaded file to the correct path.
        public void Save(string path)
        {
            if (this.Data.Length > 0)
            {
                CacheData();
            }

            if (String.IsNullOrEmpty(path))
            {
                path = HttpContext.Current.Server.MapPath(
                    _defaultUploadFolder + "\\" + FileName);
            }

            // Move the cached file to the correct path.
            if (File.Exists(path))
            {
                File.Delete(path);
            }
            if (!new FileInfo(path).Directory.Exists)
            {
                new FileInfo(path).Directory.Create();
            }
            File.Move(cachePath, path);
        }

    }
}
