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
    /// Class that holds a book's Author details
    /// </summary>
    public class Author
    {
        /// <summary>
        /// FirstName of the book (private member)
        /// </summary>
        private string firstName;

        /// <summary>
        /// FirstName of the book (public property)
        /// </summary>
        public string FirstName
        {
            get { return firstName; }
            set { firstName = value; }
        }

        /// <summary>
        /// LastName of the book (private member)
        /// </summary>
        private string lastName;

        /// <summary>
        /// LastName of the book (public property)
        /// </summary>
        public string LastName
        {
            get { return lastName; }
            set { lastName = value; }
        }


        /// <summary>
        /// Country of the book (private member)
        /// </summary>
        private string country;

        /// <summary>
        /// Country of the book (public property)
        /// </summary>
        public string Country
        {
            get { return country; }
            set { country = value; }
        }
        
        /// <summary>
        /// overriden ToString implementation
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture,"{0} {1}", FirstName, LastName);
        }
    }
}
