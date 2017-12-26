/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSAzureStorageRESTAPI
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to call List Blob REST API to list the blobs of a
* specific container of Blob storage
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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Collections.Specialized;
using System.Collections;
using System.Web;

namespace CSAzureStorageRESTAPI
{
    class Program
    {
        const string _bloburi = @"http://127.0.0.1:10000/devstoreaccount1";
        const string _accountname = "devstoreaccount1";
        const string _key = "Eby8vdM02xNOcqFlqUwJPLlmEtlCDXJ1OUzFT50uSRZ6IFsuFq2UVErCz4I6tq/K1SZFPTOtr/KBHBeksoGMGw==";
        const string _method = "GET";
       
        static void Main(string[] args)
        {
            string AccountName = _accountname;
            string AccountSharedKey = _key;
            string Address = _bloburi;
            string MessageSignature = "";
            Console.WriteLine("Please input the name of the container press <ENTER>. Its blobs info will be listed:");
            // Get the container name
            string container = Console.ReadLine();
            // Set request URI
            string QueryString = "?restype=container&comp=list";
            Uri requesturi = new Uri(Address + "/" + container + QueryString);
            
            // Create HttpWebRequest object
            HttpWebRequest Request = (HttpWebRequest)HttpWebRequest.Create(requesturi.AbsoluteUri);
            Request.Method = _method;
            Request.ContentLength = 0;
            // Add HTTP headers
            Request.Headers.Add("x-ms-date", DateTime.UtcNow.ToString("R"));
            Request.Headers.Add("x-ms-version", "2009-09-19");

            // Create Signature
            // Verb
            MessageSignature += "GET\n";
            // Content-Encoding
            MessageSignature += "\n";
            // Content-Language
            MessageSignature += "\n";
            // Content-Length
            MessageSignature += "\n";
            // Content-MD5
            MessageSignature += "\n";
            // Content-Type
            MessageSignature += "\n";
            // Date
            MessageSignature += "\n";
            // If-Modified-Since
            MessageSignature += "\n";
            // If-Match
            MessageSignature += "\n";
            // If-None-Match 
            MessageSignature += "\n";
            // If-Unmodified-Since
            MessageSignature += "\n";
            // Range
            MessageSignature += "\n";
            // CanonicalizedHeaders
            MessageSignature += GetCanonicalizedHeaders(Request);
            // CanonicalizedResource
            MessageSignature += GetCanonicalizedResourceVersion2(requesturi, AccountName);
            // Use HMAC-SHA256 to sign the signature
            byte[] SignatureBytes = System.Text.Encoding.UTF8.GetBytes(MessageSignature);
            System.Security.Cryptography.HMACSHA256 SHA256 = new System.Security.Cryptography.HMACSHA256(Convert.FromBase64String(AccountSharedKey));
            // Create Authorization HTTP header value
            String AuthorizationHeader = "SharedKey " + AccountName + ":" + Convert.ToBase64String(SHA256.ComputeHash(SignatureBytes));
            // Add Authorization HTTP header
            Request.Headers.Add("Authorization", AuthorizationHeader);

			try
			{
				// Send Http request and get response
				using (HttpWebResponse response = (HttpWebResponse)Request.GetResponse())
				{
					if (response.StatusCode == HttpStatusCode.OK)
					{
						// If success
						using (Stream stream = response.GetResponseStream())
						{
							using (StreamReader sr = new StreamReader(stream))
							{

								var s = sr.ReadToEnd();
								// Output response
								Console.WriteLine(s);
							}
						}
					}
				}
			}
			catch (WebException ex)
			{
				Console.WriteLine("An error occured. Status code:" + ((HttpWebResponse)ex.Response).StatusCode);
				Console.WriteLine("Error information:");
				using (Stream stream = ex.Response.GetResponseStream())
				{
					using (StreamReader sr = new StreamReader(stream))
					{
						var s = sr.ReadToEnd();
						Console.WriteLine(s);
					}
				}
			}

            Console.ReadLine();

        }

#region Helper method/class

        static  ArrayList GetHeaderValues(NameValueCollection headers, string headerName)
        {
            ArrayList list = new ArrayList();
            string[] values = headers.GetValues(headerName);
            if (values != null)
            {
                foreach (string str in values)
                {
                    list.Add(str.TrimStart(new char[0]));
                }
            }
            return list;
        }

        static string GetCanonicalizedHeaders(HttpWebRequest request)
        {
            ArrayList list = new ArrayList();
            StringBuilder sb = new StringBuilder();
            foreach (string str in request.Headers.Keys)
            {
                if (str.ToLowerInvariant().StartsWith("x-ms-", StringComparison.Ordinal))
                {
                    list.Add(str.ToLowerInvariant());
                }
            }
            list.Sort();
            foreach (string str2 in list)
            {
                StringBuilder builder = new StringBuilder(str2);
                string str3 = ":";
                foreach (string str4 in GetHeaderValues(request.Headers, str2))
                {
                    string str5 = str4.Replace("\r\n", string.Empty);
                    builder.Append(str3);
                    builder.Append(str5);
                    str3 = ",";
                }
                sb.Append(builder.ToString());
                sb.Append("\n");
            }
            return sb.ToString();
        }

        static string GetCanonicalizedResourceVersion2(Uri address, string accountName)
        {
            StringBuilder builder = new StringBuilder("/");
            builder.Append(accountName);
            builder.Append(address.AbsolutePath);
            CanonicalizedString str = new CanonicalizedString(builder.ToString());
            NameValueCollection values = HttpUtility.ParseQueryString(address.Query);
            NameValueCollection values2 = new NameValueCollection();
            foreach (string str2 in values.Keys)
            {
                ArrayList list = new ArrayList(values.GetValues(str2));
                list.Sort();
                StringBuilder builder2 = new StringBuilder();
                foreach (object obj2 in list)
                {
                    if (builder2.Length > 0)
                    {
                        builder2.Append(",");
                    }
                    builder2.Append(obj2.ToString());
                }
                values2.Add((str2 == null) ? str2 : str2.ToLowerInvariant(), builder2.ToString());
            }
            ArrayList list2 = new ArrayList(values2.AllKeys);
            list2.Sort();
            foreach (string str3 in list2)
            {
                StringBuilder builder3 = new StringBuilder(string.Empty);
                builder3.Append(str3);
                builder3.Append(":");
                builder3.Append(values2[str3]);
                str.AppendCanonicalizedElement(builder3.ToString());
            }
            return str.Value;
        }

        internal class CanonicalizedString
         {
             // Fields
             private StringBuilder canonicalizedString = new StringBuilder();

             // Methods
             internal CanonicalizedString(string initialElement)
             {
                 this.canonicalizedString.Append(initialElement);
             }

             internal void AppendCanonicalizedElement(string element)
             {
                 this.canonicalizedString.Append("\n");
                 this.canonicalizedString.Append(element);
             }

             // Properties
             internal string Value
             {
                 get
                 {
                     return this.canonicalizedString.ToString();
                 }
             }
         }

#endregion

    }
}
