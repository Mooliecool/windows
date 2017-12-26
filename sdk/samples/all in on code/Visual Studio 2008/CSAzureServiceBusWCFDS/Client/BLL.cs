/****************************** Module Header ******************************\
* Module Name:	BLL.cs
* Project:		Client
* Copyright (c) Microsoft Corporation.
* 
* This module is business logic layer of ASP.NET application that retrieves data
* from CSAzureServiceBusWCFDS that is exposed via Service Bus 
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
using System.Web;
using System.Collections;
using Client.ServiceReference1;
using System.Data.Services.Client;
using System.Net;
using System.Collections.Specialized;
using System.Text;
using System.IO;

namespace Client
{
    public class BLL
    {
        static Uri serviceUri = new Uri("https://[Your Service Namespace].servicebus.windows.net/DataService");
        static string _scope = "http://[Your Service Namespace].servicebus.windows.net/DataService";
        static string _servicenamespace = "[Your Service Namespace]";
        static string _issuerkey = "[Your Secret]";

        public static IEnumerable<Customers> Select()
        {
           NorthwindEntities _entities = new NorthwindEntities(serviceUri);
           _entities.SendingRequest += new EventHandler<SendingRequestEventArgs>(_entities_SendingRequest);
           List<Customers> _collection = new List<Customers>();
           // Query the first 10 records to test. You can do paging by Skip and Take but for the
           // simplicity purpose it's not shown here
           DataServiceQuery<Customers> query = (DataServiceQuery<Customers>)(
                from c in _entities.Customers
                select c).Skip(0).Take(10);
          var result= query.Execute();
          return result;
        }

        static void _entities_SendingRequest(object sender, SendingRequestEventArgs e)
        {   // Since service requires RelayAccessToken to authenticate client here we need to get token
            // from Access Control Service first before accessing our service.
            var token = GetTokenFromACS();
            string headerValue = string.Format("WRAP access_token=\"{0}\"", HttpUtility.UrlDecode(token)); 
            // We then attach the token to Authorization HTTP header. To learn more details please refer
            // to WRAP specifications.
            e.Request.Headers.Add("Authorization", headerValue);
        }

        public static void Insert(string CustomerID, string CompanyName) {
            NorthwindEntities _entities = new NorthwindEntities(serviceUri);
            _entities.SendingRequest+=new EventHandler<SendingRequestEventArgs>(_entities_SendingRequest);
            _entities.AddToCustomers(new Customers() { CustomerID = CustomerID, CompanyName = CompanyName });
            _entities.SaveChanges();
        }

        private static string GetTokenFromACS()
        {
            string s = string.Empty;
            try
            {
                // Request a token from ACS
                WebClient client = new WebClient();
                client.BaseAddress = string.Format("https://{0}-sb.accesscontrol.windows.net", _servicenamespace);

                NameValueCollection values = new NameValueCollection();
                values.Add("wrap_name", "owner");
                values.Add("wrap_password", _issuerkey);
                values.Add("wrap_scope", _scope);

                byte[] responseBytes = client.UploadValues("WRAPv0.9", "POST", values);
                string response = Encoding.UTF8.GetString(responseBytes);
                Console.WriteLine("\nreceived token from ACS: {0}\n", response);

                return response
                    .Split('&')
                    .Single(value => value.StartsWith("wrap_access_token=", StringComparison.OrdinalIgnoreCase))
                    .Split('=')[1];
            }
            catch (WebException ex)
            {
                // You can set a breakpoint here to check detailed exception from detailedexception
                StreamReader sr = new StreamReader(ex.Response.GetResponseStream());
                var detailedexception = sr.ReadToEnd();
            }
            return s;


        }
    }
}
