//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
#region Using directives

using System;
using System.Text;

#endregion

namespace Microsoft.Samples.MsmqSample
{
    /// <summary>
    /// Customer class that represents a Customer
    /// </summary>
    public class Customer
    {
        // Name of the customer's Company
        private string companyValue;
        // Name of the customer
        private string contactValue;
        //Default Constructor
        public Customer()
        {
        }
        //Constructor
        public Customer(string company, string contact)
        {
            companyValue = company;
            contactValue = contact;
        }
        public string Company
        {
            get
            {
                return companyValue;
            }
            set
            {
                companyValue = value;
            }
        }
        public string Contact
        {
            get
            {
                return contactValue;
            }
            set
            {
                contactValue = value;
            }
        }
    }
}
