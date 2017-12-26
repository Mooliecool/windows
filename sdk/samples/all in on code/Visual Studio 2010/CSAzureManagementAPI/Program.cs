/****************************** Module Header ******************************\
Module Name:  Program.cs
Project:      CSAzureManagementAPI
Copyright (c) Microsoft Corporation.
 
This file shows how to create a hosted service on Azure via management api.
 
This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.Net;
using System.IO;
using System;

namespace ManagementAPI
{
    public class RequestState
    {
        const int BufferSize = 1024;
        public StringBuilder RequestData;
        public byte[] BufferRead;
        public WebRequest Request;
        public Stream ResponseStream;

        public Decoder StreamDecode = Encoding.UTF8.GetDecoder();  // Create Decoder for appropriate enconding type.

        public RequestState()
        {
            BufferRead = new byte[BufferSize];
            RequestData = new StringBuilder(String.Empty);
            Request = null;
            ResponseStream = null;
        }
    }


    class Program
    {

        static void Main(string[] args)
        {

            string subscriptionId = "";   // Your subscription id.

            // This is the thumbprint of the management certificate
            // make sure the certificate exists in your personal/Certificates. Please verify it by certmgr.msc
            // this certificate must be uploaded to Azure Management Portal, in Management Certificates
            // http://msdn.microsoft.com/en-us/library/gg551726.aspx
            string certThumbprint = "";

            // This will be use as the DNS prefix, so if it exists(someone else might took it) 
            // then you will receive 409 conflict error.
            string hostServiceName = "";  
            
            string labelName = "";
           
            X509Store certificateStore = new X509Store(StoreName.My, StoreLocation.CurrentUser);
            certificateStore.Open(OpenFlags.ReadOnly);
            X509Certificate2Collection certs = certificateStore.Certificates.Find(X509FindType.FindByThumbprint, 
                certThumbprint, false);

            if (certs.Count == 0)
            {
                Console.WriteLine("Couldn't find the certificate with thumbprint:" + certThumbprint);
                return;
            }

            certificateStore.Close();

            HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create(
                new Uri("https://management.core.windows.net/" + subscriptionId + "/services/hostedservices"));

            request.Method = "POST";
            request.ClientCertificates.Add(certs[0]);
            request.ContentType = "application/xml";
            request.Headers.Add("x-ms-version", "2010-10-28");

            StringBuilder sbRequestXML = new StringBuilder("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
            sbRequestXML.Append("<CreateHostedService xmlns=\"http://schemas.microsoft.com/windowsazure\">");
            sbRequestXML.AppendFormat("<ServiceName>{0}</ServiceName>", hostServiceName);
            sbRequestXML.AppendFormat("<Label>{0}</Label>", EncodeToBase64String(labelName));
            sbRequestXML.Append("<Location>Anywhere US</Location>");
            sbRequestXML.Append("</CreateHostedService>");

            byte[] formData =
                   UTF8Encoding.UTF8.GetBytes(sbRequestXML.ToString());
            request.ContentLength = formData.Length;
        
            using (Stream post = request.GetRequestStream())
            {
                post.Write(formData, 0, formData.Length);
            }

            Console.WriteLine("Creating Hosted Service: " + hostServiceName);

            try
            {
                RequestState state = new RequestState();
                state.Request = request;
                IAsyncResult result = request.BeginGetResponse(new AsyncCallback(RespCallback), state);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.Message);
            }

            Console.ReadKey();
        }

        public static string EncodeToBase64String(string original)
        {
            return Convert.ToBase64String(Encoding.UTF8.GetBytes(original));
        }

        private static void RespCallback(IAsyncResult result)
        {
            RequestState state = (RequestState)result.AsyncState;   // Grab the custom state object
            WebRequest request = (WebRequest)state.Request;

            HttpWebResponse response =                              
              (HttpWebResponse)request.EndGetResponse(result);      // Get the Response
         
            string statusCode = response.StatusCode.ToString();

            // A value that uniquely identifies a request made against the Management service. 
            // For an asynchronous operation, 
            // you can call get operation status with the value of the header to determine whether 
            // the operation is complete, has failed, or is still in progress.
            string reqId = response.GetResponseHeader("x-ms-request-id");  

            Console.WriteLine("Creation Return Value: " + statusCode);
            Console.WriteLine("RequestId: " + reqId);    
        }
    }
}
