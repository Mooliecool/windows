//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.Serialization;
using System.ServiceModel.Web;

namespace Microsoft.Samples.ConditionalGetAndPut
{
    class Program
    {
        static readonly DataContractSerializer customerSerializer = new DataContractSerializer(typeof(Customer));
        static readonly DataContractSerializer listOfCustomersSerializer = new DataContractSerializer(typeof(List<Customer>));

        static void Main(string[] args)
        {
            using (WebServiceHost host = new WebServiceHost(typeof(Service), new Uri("http://localhost:8000/Customers")))
            {
                //WebServiceHost will automatically create a default endpoint at the base address using the WebHttpBinding
                //and the WebHttpBehavior, so there's no need to set it up explicitly
                host.Open();

                Uri baseAddress = new Uri("http://localhost:8000/Customers");
                Console.WriteLine("Service is hosted at: " + baseAddress.AbsoluteUri);
                Console.WriteLine("Service help page is at: " + baseAddress.AbsoluteUri + "/help");
                Console.WriteLine("");

                Console.WriteLine("Adding some customers with POST:");
                Customer alice = new Customer("Alice", "123 Pike Place", null);
                Uri aliceLocation = PostCustomer(baseAddress, alice);

                Customer bob = new Customer("Bob", "2323 Lake Shore Drive", null);
                Uri bobLocation = PostCustomer(baseAddress, bob);

                Console.WriteLine("");
               
                Console.WriteLine("Using PUT to update a customer without specifying an Etag in the request. This will fail with error PreconditionFailed");
                alice.Name = "Charlie";
                alice.Uri = aliceLocation;
                string aliceEtag = null;
                try
                {
                    PutCustomer(aliceLocation, alice, aliceEtag);
                }
                catch (WebException ex)
                {
                    HttpWebResponse response = (HttpWebResponse)ex.Response;
                    Console.WriteLine("Request failed with error '{0}'", response.StatusCode);
                    aliceEtag = response.Headers[HttpResponseHeader.ETag];
                }
                Console.WriteLine("Retrying PUT to update a customer after specifying the latest Etag in the request. This will succeed");
                PutCustomer(aliceLocation, alice, aliceEtag);

                Console.WriteLine("");
                Console.WriteLine("Using GET to retrieve the list of customers");
                string customerListEtag = null;
                List<Customer> customers = GetCustomers(baseAddress, ref customerListEtag);
                foreach (Customer c in customers)
                {
                    Console.WriteLine(c.ToString());
                }
                Console.WriteLine("Doing a conditional GET to retrieve the list of customers again. This will return NotMofified");
                try
                {
                    GetCustomers(baseAddress, ref customerListEtag);
                }
                catch (WebException ex)
                {
                    HttpWebResponse response = (HttpWebResponse)ex.Response;
                    Console.WriteLine("Request failed with error '{0}'", response.StatusCode);
                }
                Console.WriteLine("");
                Console.WriteLine("Using DELETE to delete a customer without specifying an Etag in the request. This will fail with error PreconditionFailed");
                string bobEtag = null;
                try
                {
                    DeleteCustomer(bobLocation, bobEtag);
                }
                catch (WebException ex)
                {
                    HttpWebResponse response = (HttpWebResponse)ex.Response;
                    Console.WriteLine("Request failed with error '{0}'", response.StatusCode);
                    bobEtag = response.Headers[HttpResponseHeader.ETag];
                }
                Console.WriteLine("Retrying DELETE with the latest Etag in the request. This will succeed");
                DeleteCustomer(bobLocation, bobEtag);
                Console.WriteLine("");
                Console.WriteLine("Doing a conditional GET for final list of customers. This will not return NotModified since the etag for the customer list is stale");
                customers = GetCustomers(baseAddress, ref customerListEtag);
                foreach (Customer c in customers)
                {
                    Console.WriteLine(c.ToString());
                }

                Console.WriteLine("");

                Console.WriteLine("Press any key to terminate");
                Console.ReadLine();
            }
        }

        static Uri PostCustomer(Uri uri, Customer customer)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.Method = "POST";
            request.ContentType = "application/xml";
            using (Stream requestStream = request.GetRequestStream())
            {
                customerSerializer.WriteObject(requestStream, customer);
            }
            HttpWebResponse response = (HttpWebResponse) request.GetResponse();
            using (Stream responseStream = response.GetResponseStream())
            {
                Customer createdItem = (Customer) customerSerializer.ReadObject(responseStream);
                Console.WriteLine(createdItem.ToString());
            }
            response.Close();
            return new Uri(response.Headers[HttpResponseHeader.Location]);
        }

        static void PutCustomer(Uri uri, Customer customer, string etag)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.Method = "PUT";
            request.ContentType = "application/xml";
            request.Headers[HttpRequestHeader.IfMatch] = etag;
            using (Stream requestStream = request.GetRequestStream())
            {
                customerSerializer.WriteObject(requestStream, customer);
            }
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            using (Stream responseStream = response.GetResponseStream())
            {
                Customer updatedItem = (Customer)customerSerializer.ReadObject(responseStream);
                Console.WriteLine(updatedItem.ToString());
            }
            response.Close();
        }

        static List<Customer> GetCustomers(Uri uri, ref string etag)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.Method = "GET";
            request.Headers[HttpRequestHeader.IfNoneMatch] = etag;
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            List<Customer> customers;
            using (Stream responseStream = response.GetResponseStream())
            {
                customers = (List<Customer>)listOfCustomersSerializer.ReadObject(responseStream);
            }
            etag = response.Headers[HttpResponseHeader.ETag];
            response.Close();
            return customers;
        }

        static void DeleteCustomer(Uri uri, string etag)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.Method = "DELETE";
            request.Headers[HttpRequestHeader.IfMatch] = etag;
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            response.Close();
        }
    }
}
