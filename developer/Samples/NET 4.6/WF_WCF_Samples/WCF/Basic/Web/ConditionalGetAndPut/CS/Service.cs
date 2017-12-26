//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.Samples.ConditionalGetAndPut
{
    /// <summary>
    /// This service maintains an etag for the customer list and an etag for each customer item.
    /// The etag for the customer list is used to support conditional GET of the customer list.
    /// The etag for each customer item is used to support conditional GET, PUT and DELETE of the item.
    /// </summary>
    [ServiceContract, ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class Service
    {
        int counter = 0;
        Hashtable customers = new Hashtable();
        long customerListEtag = 0;
        Hashtable customerEtags = new Hashtable();
        object writeLock = new Object();

        [WebInvoke(Method = "POST", UriTemplate = ""), Description("Adds a customer to customers collection. The response Location header contains a URL to the added item.")]
        public Customer AddCustomer(Customer customer)
        {
            lock (writeLock)
            {
                string id = (++counter).ToString();              
                // Set the Uri of the customer and add it to the colleciton
                UriTemplate itemTemplate = WebOperationContext.Current.GetUriTemplate("GetCustomer");
                customer.Uri = itemTemplate.BindByPosition(WebOperationContext.Current.IncomingRequest.UriTemplateMatch.BaseUri, id);
                customers[id] = customer;
                // initialize an etag for the newly added customer
                long newEtag = DateTime.UtcNow.Ticks;
                customerEtags[id] = newEtag;
                // set the new etag in the response
                WebOperationContext.Current.OutgoingResponse.SetETag(newEtag);               
                // update the etag for the customer collection since the list has changed (item added to it)
                this.customerListEtag = DateTime.UtcNow.Ticks;
                WebOperationContext.Current.OutgoingResponse.SetStatusAsCreated(customer.Uri);
            }

            return customer;
        }

        [WebInvoke(Method = "DELETE", UriTemplate = "{id}"), Description("Deletes the specified customer from customers collection. Returns NotFound if there is no such customer. Returns PreconditionFailed if the client copy is stale.")]
        public void DeleteCustomer(string id)
        {
            lock (writeLock)
            {
                // return NotFound if there is no item with the specified id.
                object itemEtag = customerEtags[id];
                if (itemEtag == null)
                {
                    throw new WebFaultException(HttpStatusCode.NotFound);
                }
                
                // Return PreconditionFailed if the client request does not have an etag or has an old etag for the item
                WebOperationContext.Current.IncomingRequest.CheckConditionalUpdate((long) itemEtag);
                customers.Remove(id);
                // remove the etag for the deleted item
                customerEtags.Remove(id);
                // update the customer list etag since the list has changed (item deleted from it)
                this.customerListEtag = DateTime.UtcNow.Ticks;
            }
        }

        [WebGet(UriTemplate = "{id}"), Description("Returns the specified customer from customers collection. Returns NotFound if there is no such customer. Supports conditional GET.")]
        public Customer GetCustomer(string id)
        {
            lock (writeLock)
            {
                // return NotFound if there is no item with the specified id.
                object itemEtag = customerEtags[id];
                if (itemEtag == null)
                {
                    throw new WebFaultException(HttpStatusCode.NotFound);
                }

                // return NotModified if the client did a conditional GET and the customer item has not changed
                // since when the client last retrieved it
                WebOperationContext.Current.IncomingRequest.CheckConditionalRetrieve((long)itemEtag);
                Customer result = this.customers[id] as Customer;
                // set the customer etag before returning the result
                WebOperationContext.Current.OutgoingResponse.SetETag((long)itemEtag);
                return result;
            }
        }


        [WebGet(UriTemplate = ""), Description("Returns all the customers in the customers collection. Supports conditional GET.")]
        public List<Customer> GetCustomers()
        {
            lock (writeLock)
            {
                // return NotModified if the client did a conditional GET and the customer list has not changed
                // since when the client last retrieved it
                WebOperationContext.Current.IncomingRequest.CheckConditionalRetrieve(this.customerListEtag);
                List<Customer> list = new List<Customer>();
                foreach (Customer c in this.customers.Values)
                {
                    list.Add(c);
                }
                // set the customer list etag in the response before returning the result
                WebOperationContext.Current.OutgoingResponse.SetETag(this.customerListEtag);
                return list;
            }
        }

        [WebInvoke(Method = "PUT", UriTemplate = "{id}"), Description("Updates the specified customer. Returns NotFound if there is no such customer. Returns PreconditionFailed if the client copy is stale.")]
        public Customer UpdateCustomer(string id, Customer newCustomer)
        {
            lock (writeLock)
            {
                // return NotFound if there is no item with the specified id.
                object itemEtag = customerEtags[id];
                if (itemEtag == null)
                {
                    throw new WebFaultException(HttpStatusCode.NotFound);
                }
                // Return PreconditionFailed if the client request does not have an etag or has an old etag for the item
                WebOperationContext.Current.IncomingRequest.CheckConditionalUpdate((long)itemEtag);
                customers[id] = newCustomer;
                // update the etag for the item since it has changed
                long newEtag = DateTime.UtcNow.Ticks;
                customerEtags[id] = newEtag;

                // update the customer list etag since the item in the list has changed
                this.customerListEtag = DateTime.UtcNow.Ticks;

                // set the updated etag in the response
                WebOperationContext.Current.OutgoingResponse.SetETag(newEtag);
                return newCustomer;
            }
        }
    }
}
