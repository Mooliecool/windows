//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.ObjectModel;

namespace Microsoft.Rules.Samples
{
    // The container class of the extension method.
    public static class Extensions
    {
        // The definition of the extension method for the class OrderErrorCollection.
        public static void PrintOrderErrors(this OrderErrorCollection orderErrorCollection)
        {
            Console.WriteLine();

            foreach (OrderError orderError in orderErrorCollection)
            {
                Console.WriteLine(orderError.ErrorText);
            }
        }
    }

    // The error object that gets created when an invalid input is entered.
    public class OrderError
    {
        private string errorText;       // that is, Error: Zip code is invalid.
        private string customerName;    // that is, "John Customer".
        private int itemNum;            // that is, 1 => for Vista Ultimate DVD.
        private string zipCode;         // that is, "00999".

        public OrderError()
        {
        }

        public OrderError(int invalidItemNum)
        {
            this.ItemNum = invalidItemNum;
        }

        public OrderError(string invalidZipCode)
        {
            this.ZipCode = invalidZipCode;
        }

        public string ErrorText
        {
            get
            {
                return this.errorText;
            }
            set
            {
                this.errorText = value;
            }
        }

        public string CustomerName
        {
            get
            {
                return this.customerName;
            }
            set
            {
                this.customerName = value;
            }
        }

        public int ItemNum
        {
            get
            {
                return this.itemNum;
            }
            set
            {
                this.itemNum = value;
            }
        }

        public string ZipCode
        {
            get
            {
                return this.zipCode;
            }
            set
            {
                this.zipCode = value;
            }
        }
    }

    public class OrderErrorCollection : Collection<OrderError>
    {
        public OrderErrorCollection()
        {
        }

        public void AddError(OrderError orderError)
        {
            this.Add(orderError);
        }

        // Overload the operator + for two OrderErrorCollection objects.
        public static OrderErrorCollection operator +(OrderErrorCollection orderErrorCollection1, OrderErrorCollection orderErrorCollection2)
        {
            OrderErrorCollection orderErrorCollection = new OrderErrorCollection();
            if (null != orderErrorCollection1)
            {
                foreach (OrderError orderError in orderErrorCollection1)
                    orderErrorCollection.Add(orderError);
            }

            if (null != orderErrorCollection2)
            {
                foreach (OrderError orderError in orderErrorCollection2)
                    orderErrorCollection.Add(orderError);
            }

            return orderErrorCollection;
        }
    }
}
