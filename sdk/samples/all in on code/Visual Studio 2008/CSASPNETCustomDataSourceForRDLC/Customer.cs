/****************************** Module Header ******************************\
* Module Name:    Customer.cs
* Project:        CSASPNETCustomDataSourceForRDLC
* Copyright (c) Microsoft Corporation.
*
* In order to describe a customer, we create a customer class and set some 
* properties for it.
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

namespace CSASPNETCustomDataSourceForRDLC
{
    public class Customer
    {
        protected string id;
        protected string name;
        protected string country;

        public Customer()
        { 
        
        }

        public Customer(string id,string name,string country)
        {
            this.id = id;
            this.name = name;
            this.country = country;
        }


        public string Id
        {
            set
            {
                id = value;
            }
            get
            {
                return id;
            }
        }
        public string Name
        {
            set
            {
                name = value;
            }
            get
            {
                return name;
            }
        }
        public string Country
        {
            set
            {
                country = value;
            }
            get
            {
                return country;
            }
        }
    }
}
