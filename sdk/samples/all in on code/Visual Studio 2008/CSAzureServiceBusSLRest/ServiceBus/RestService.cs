/****************************** Module Header ******************************\
* Module Name:	RestService.cs
* Project:		CSAzureServiceBusSLRest
* Copyright (c) Microsoft Corporation.
* 
* This is the service implementation of the WCF REST Service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.IO;
using System.Net;
using System.ServiceModel.Web;

namespace AzureServiceBusSLRest
{
	public class RestService : IRestService
	{
		public Stream DownloadFile(string fileName)
		{
			try
			{
				using (FileStream fs = File.Open(fileName, FileMode.Open, FileAccess.Read))
				{
					MemoryStream stream = new MemoryStream();
					byte[] buffer = new byte[fs.Length];
					fs.Read(buffer, 0, buffer.Length);
					stream.Write(buffer, 0, buffer.Length);
					stream.Position = 0;
					return stream;
				}
			}
			catch (IOException ex)
			{
				WebOperationContext.Current.OutgoingResponse.StatusCode = HttpStatusCode.NotFound;
				return new MemoryStream();
			}
		}

		public string UploadFile(string fileName, Stream content)
		{
			// We do not know the length of the HTTP request stream,
			// so we must read the stream using the following code:
			int bufferSize = 4096;
			int bytesRead = 1;
			int totalBytesRead = 0;
			try
			{
				using (FileStream fileStream = File.Create(fileName))
				{
					byte[] buffer = new byte[bufferSize];
					bytesRead = content.Read(buffer, 0, bufferSize);
					while (bytesRead > 0)
					{
						fileStream.Write(buffer, 0, bytesRead);
						bytesRead = content.Read(buffer, 0, bufferSize);
						totalBytesRead += bytesRead;
					}
				}

				// To follow the best practices of REST services, when a resource is created,
				// we return a 201 Created status code instead of the default 200.
				// This means Silverlight clients must use client HTTP stack,
				// instead of the default browser HTTP stack to work with the service.
				return WriteResponse(HttpStatusCode.Created, "Created.");
			}
			catch (Exception ex)
			{
				return WriteResponse(HttpStatusCode.InternalServerError, ex.Message);
			}
		}

		private static string WriteResponse(HttpStatusCode statusCode, string message)
		{
			WebOperationContext.Current.OutgoingResponse.StatusCode = statusCode;
			return message;
		}

		public Stream GetClientAccessPolicy()
		{
			return this.DownloadFile("clientaccesspolicy.xml");
		}
	}
}
