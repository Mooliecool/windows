/****************************** Module Header ******************************\
* Module Name:    UploadProcessModule.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* In this sample, we can see the features listed below:
* 1. How to get the client's request entity body by HttpWorkerRequest.
* 2. How to control the server side to read the request data.
* 3. How to retrieve and store the upload status.
* 
* Based on this module, we can extend it to realize the features listed below:
* 1. Control the status of multiple files uploading.
* 2. Control big file upload to the server and 
*    store the files without the server cache.
* (Notice: in this sample I have not realized the features above.
*          I will add them in the near-future.)
* 
* The IIS restrict the request content length by default(About 28MB),
* if we want to make this module work for large files,
* please follow the readme file in the root directory.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Web;
using System.Text;
using System.Web.Caching;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace CSASPNETFileUploadStatus
{
    public class UploadProcessModule : IHttpModule
    {
        /// <summary>
        /// This is an ASP.NET HttpModule for showing
        ///   Non-Component File Upload Status  
        ///   based on .Net Framework 4.0.
        /// For details, please view the Readme file in the root directory.
        /// </summary>
        private string _cacheContainer = "fuFile";
        private string _uploadedFilesFolder = "UploadedFiles";
        private string _folderPath = "";

        public void Dispose()
        {

        }

        public void Init(HttpApplication context)
        {
            context.BeginRequest += new EventHandler(context_BeginRequest);

            // Check about the folder for the uploaded files.
            _folderPath = HttpContext.Current.Server.MapPath(_uploadedFilesFolder);
            if (!Directory.Exists(_folderPath))
            {
                Directory.CreateDirectory(_folderPath);
            }
        }

        private void context_BeginRequest(object sender, EventArgs e)
        {

            HttpApplication app = sender as HttpApplication;
            HttpContext context = app.Context;


            // We need the HttpWorkerRequest of the current context to
            // process the request data. For more details about HttpWorkerRequest,
            // please follow the Readme file in the root directory.
            IServiceProvider provider = (IServiceProvider)context;
            System.Web.HttpWorkerRequest request =
                (HttpWorkerRequest)provider.GetService(typeof(HttpWorkerRequest));

            // Get the content type of the current request.
            string contentType =
                request.GetKnownRequestHeader(
                System.Web.HttpWorkerRequest.HeaderContentType);
            // If we could not get the content type, then skip out the module
            if (contentType == null)
            {
                return;
            }
            // If the content type is not multipart/form-data,
            //   means that there is no file upload request
            //   then skip out the moudle
            if (contentType.IndexOf("multipart/form-data") == -1)
            {
                return;
            }
            string boundary = contentType.Substring(contentType.IndexOf("boundary=") + 9);
            // Get the content length of the current request
            long contentLength = Convert.ToInt64(
                request.GetKnownRequestHeader(
                HttpWorkerRequest.HeaderContentLength));

            // Get the data of the portion of the HTTP request body
            // that has currently been read.
            // This is the first step for us to store the upload file.
            byte[] data = request.GetPreloadedEntityBody();

            // Create an instance of the manager class which 
            // help to filter the request data.
            FileUploadDataManager storeManager =
                new FileUploadDataManager(boundary);
            // Append the preloaded data.
            storeManager.AppendData(data);

            UploadStatus status = null;
            if (context.Cache[_cacheContainer] == null)
            {
                //Initialize the UploadStatus which used to 
                //store the status for the client.
                status = new UploadStatus(
                    context,         // Send the current context to the status
                    // which will be used for the events.
                    contentLength    // Initialize the file length.
                    );
                // Bind a event when update the status.
                status.OnDataChanged +=
                    new UploadStatusEventHandler(status_OnDataChanged);

            }
            else
            {
                status = context.Cache[_cacheContainer] as UploadStatus;
                if (status.IsFinished)
                {
                    return;
                }
            }

            // Set the first read data length to the status class.
            if (data != null)
            {
                status.UpdateLoadedLength(data.Length);
            }

            // Get the length of the left request data. 
            long leftdata = status.ContentLength - status.LoadedLength;

            // Define a custom buffer length
            int customBufferLength = Convert.ToInt32(Math.Ceiling((double)contentLength / 16));
            if (customBufferLength < 1024)
            {
                customBufferLength = 1024;
            }
            while (!request.IsEntireEntityBodyIsPreloaded() && leftdata > 0)
            {
                // Check if user abort the upload, then close the connection
                if (status.Aborted)
                {
                    // Delete the cached files.
                    foreach (UploadFile file in storeManager.FilterResult)
                    {
                        file.ClearCache();
                    }
                    request.CloseConnection();
                    return;
                }

                // If the length the remained request data
                // is less than the buffer length,
                // then set the buffer length as the remained data length.
                if (leftdata < customBufferLength)
                {
                    customBufferLength = (int)leftdata;
                }

                // Read a custom buffer length of the request data
                data = new byte[customBufferLength];
                int redlen = request.ReadEntityBody(data, customBufferLength);
                if (customBufferLength > redlen)
                {
                    data = BinaryHelper.SubData(data, 0, redlen);
                }
                // Append the left data.
                storeManager.AppendData(data);

                // Add the buffer length to the status to update the upload status
                status.UpdateLoadedLength(redlen);

                leftdata -= redlen;
            }

            // After all the data has been read,
            // save the uploaded files.
            foreach (UploadFile file in storeManager.FilterResult)
            {
                file.Save(null);
            }
        }

        private void status_OnDataChanged(object sender, UploadStatusEventArgs e)
        {
            //Store the status class to the Cache of the current context.
            UploadStatus status = sender as UploadStatus;
            if (e.context.Cache[_cacheContainer] == null)
            {
                e.context.Cache.Add(_cacheContainer,
                    status,
                    null,
                    DateTime.Now.AddDays(1),
                    Cache.NoSlidingExpiration,
                    CacheItemPriority.High,
                    null);
            }
            else
            {
                e.context.Cache[_cacheContainer] = status;
            }

        }



    }
}
