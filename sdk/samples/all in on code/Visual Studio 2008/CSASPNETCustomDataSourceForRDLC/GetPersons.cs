/****************************** Module Header ******************************\
* Module Name:    GetPersons.cs
* Project:        CSASPNETCustomDataSourceForRDLC
* Copyright (c) Microsoft Corporation.
*
* Use a generic list to store all the records of the customers, and use that
* list as the datasource for the RDLC.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 11/23/2009 11:00 AM Zong-Qing Li Created
\***************************************************************************/
using System;
using System.Data;
using System.Configuration;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;
using System.Collections.Generic;

namespace CSASPNETCustomDataSourceForRDLC
{
    public class GetPersons
    {
      
        List<Customer> customers = new List<Customer>();

        // Create a generic List as the datasource for RDLC.
        public List<Customer> GetCustomers()
        {
            Customer ct1 = new Customer("1", "Rosenblinker, Inc.", "USA");
            Customer ct2 = new Customer("2", "Bolimite, Mfg", "USA");
            Customer ct3 = new Customer("3", "Sanders & Son", "USA");
            Customer ct4 = new Customer("4", "Landmark, Inc.", "USA");
            Customer ct5 = new Customer("5", "Juniper, Inc", "UK");
            Customer ct6 = new Customer("6", "Twillig Companies", "Canada");
            customers.Add(ct1);
            customers.Add(ct2);
            customers.Add(ct3);
            customers.Add(ct4);
            customers.Add(ct5);
            customers.Add(ct6);
            return customers;
        }   

    }
}
