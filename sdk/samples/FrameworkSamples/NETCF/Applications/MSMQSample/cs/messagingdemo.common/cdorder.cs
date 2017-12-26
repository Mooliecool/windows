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
    /// CDOrder class that represents a Compact Disc Order
    /// </summary>
    public class CDOrder
    {
        //Default Constructor
        public CDOrder()
        {
        }
        // Book Ordered
        private CompactDisc productValue;
        // Amount of Books orders
        private int quantityValue;
        // Customer that placed the Order
        private Customer customerValue;
        // Date the order was placed
        private DateTime orderTimeValue;

        public CompactDisc Product
        {
            get
            {
                return productValue;
            }
            set
            {
                productValue = value;
            }
        }
        public int Quantity
        {
            get
            {
                return quantityValue;
            }
            set
            {
                quantityValue = value;
            }
        }
        public DateTime OrderTime
        {
            get
            {
                return orderTimeValue;
            }
            set
            {
                orderTimeValue = value;
            }
        }
        public Customer Customer
        {
            get
            {
                return customerValue;
            }
            set
            {
                customerValue = value;
            }
        }
        public override string ToString()
        {
            return productValue.Title;
        }
    }
}

