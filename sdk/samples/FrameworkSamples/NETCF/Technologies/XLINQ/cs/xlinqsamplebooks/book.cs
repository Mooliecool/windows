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

using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
namespace Microsoft.Samples.XLinq
{
 
    /// <summary>
    /// Book class that contains the details of a book
    /// </summary>
    public class Book
    {
        /// <summary>
        /// Title of the book (private member)
        /// </summary>
        private string title;

        /// <summary>
        /// Title of the book (public property)
        /// </summary>
        public string Title
        {
            get { return title; }
            set { title = value; }
        }


        /// <summary>
        /// Author of the book (private member)
        /// </summary>
        private Author author;

        /// <summary>
        /// Author of the book (public property)
        /// </summary>
        public Author Author
        {
            get { return author; }
            set { author = value; }
        }


        /// <summary>
        /// Price of the book (private member)
        /// </summary>
        private double price;

        /// <summary>
        /// Price of the book (public property) 
        /// </summary>
        public double Price
        {
            get { return price; }
            set { price = value; }
        }

        /// <summary>
        /// overriden ToString implementation
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "Title ={0}, Author ={1}, Price ={2:c}", Title, Author, Price);
        }
    }
}
