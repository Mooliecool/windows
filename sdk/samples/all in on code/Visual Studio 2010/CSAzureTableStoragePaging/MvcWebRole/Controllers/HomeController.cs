/****************************** Module Header ******************************\
* Module Name:	TableStoragePagingUtility.cs
* Project:		AzureTableStoragePaging
* Copyright (c) Microsoft Corporation.
* 
* This module controls the paging behavior. 
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
using System.Web.Mvc;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;
using MvcWebRole.Utilities;
using MvcWebRole.Models;

namespace MvcWebRole.Controllers
{
    [HandleError(ExceptionType=typeof(Exception),View="Error")]
    public class HomeController : Controller
    {
        static Random r = new Random();
        CloudStorageAccount cloudStorageAccount = CloudStorageAccount.FromConfigurationSetting("SampleDataConnectionString");
        public ActionResult Index()
        {
            try
            {
                var cloudTableClient = cloudStorageAccount.CreateCloudTableClient();
                cloudTableClient.CreateTableIfNotExist("Customers");
                var context = new CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials);
                var list = context.Customers.ToList();
                // If there are entities in the table show UI that renders the table
                if (list.Count() > 0)
                {
                    MVCSessionCachedDataProvider<Customer> provider = new MVCSessionCachedDataProvider<Customer>(this, "provider1");
                    TableStoragePagingUtility<Customer> pagingUtility = new TableStoragePagingUtility<Customer>(provider, cloudStorageAccount,
                        context, 10, "Customers");
                    return View("Index", new CustomersSet() { Customers = pagingUtility.GetCurrentOrFirstPage().ToList(), ReadyToShowUI = true });
                }
                else
                {
                    //If there is no entity in the table show link to guide user to add data to table.
                    ViewResult vr = View("Index", new CustomersSet() { ReadyToShowUI = false });
                    return vr;
                }                
            }
            catch (Exception ex) 
            {
                return View("Error", new HandleErrorInfo(ex, "HomeController", "Index"));
            }
           
        }

        public ActionResult AddDataToTest()
        {
            
            var cloudTableClient = cloudStorageAccount.CreateCloudTableClient();
            var context = new CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials);
            try
            {
                // Add 65 entities to the table
                for (int i = 0; i < 65; i++)
                {
                    context.AddObject("Customers", new Customer() { Age = r.Next(16, 70), Name = "Customer" + i.ToString() });
                }
                context.SaveChanges();
                MVCSessionCachedDataProvider<Customer> provider = new MVCSessionCachedDataProvider<Customer>(this, "provider1");
                TableStoragePagingUtility<Customer> pagingUtility = new TableStoragePagingUtility<Customer>(provider, cloudStorageAccount,
                    context, 10, "Customers");
                return View("Index", new CustomersSet() { Customers = pagingUtility.GetNextPage().ToList(), ReadyToShowUI = true }); 
            }
            catch (Exception ex)
            {
                return View("Error",new HandleErrorInfo(ex,"HomeController","AddDataToTest"));
            }

  
        }
        public ActionResult Previous() 
        {            
            var cloudTableClient = cloudStorageAccount.CreateCloudTableClient();
            var context = new CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials);
            MVCSessionCachedDataProvider<Customer> provider = new MVCSessionCachedDataProvider<Customer>(this, "provider1");
            TableStoragePagingUtility<Customer> pagingUtility = new TableStoragePagingUtility<Customer>(provider, cloudStorageAccount,
                context, 10, "Customers");
            return View("Index", new CustomersSet() { Customers = pagingUtility.GetPreviousPage().ToList(), ReadyToShowUI=true }); 
        }
        public ActionResult Next()
        {
            var cloudTableClient = cloudStorageAccount.CreateCloudTableClient();
            var context = new CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials);
            MVCSessionCachedDataProvider<Customer> provider = new MVCSessionCachedDataProvider<Customer>(this, "provider1");
            TableStoragePagingUtility<Customer> pagingUtility = new TableStoragePagingUtility<Customer>(provider, cloudStorageAccount,
                context, 10, "Customers");
            return View("Index", new CustomersSet() { Customers = pagingUtility.GetNextPage().ToList(), ReadyToShowUI=true }); 
        }
    }
}
