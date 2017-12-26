/************************************* Module Header **************************************\
* Module Name:  Person.cs
* Project:      CSWinFormBindToNestedProperties
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to bind a DataGridView column to a nested property 
* in the data source.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace BindingNestedProperties
{
    // add a TypeDescriptionProviderAttribute on the Person class
    [TypeDescriptionProvider(typeof(MyTypeDescriptionProvider))]
    class Person
    {
        private string id;
        private string name;
        private Address homeAddr;
        public string ID
        {
            get { return id; }
            set { id = value; }
        }
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        public Address HomeAddr
        {
            get { return homeAddr; }
            set { homeAddr = value; }
        }
    }
    class Address
    {
        private string cityname;
        private string postcode;
        public string CityName
        {
            get { return cityname; }
            set { cityname = value; }
        }
        public string PostCode
        {
            get { return postcode; }
            set { postcode = value; }
        }
    }

}
