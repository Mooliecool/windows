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
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace Microsoft.Samples.CustomOrder
{
    public class OrderedClass
    {
        private int field1;
        private string field2;
        private string field3;

        [XmlElement(Order = 3)]
        public string Field3
        {
            get { return field3; }
            set { field3 = value; }
        }

        [XmlElement(Order = 1)]
        public int Field1
        {
            get { return field1; }
            set { field1 = value; }
        }

        [XmlElement(Order = 2)]
        public string Field2
        {
            get { return field2; }
            set { field2 = value; }
        }

        public OrderedClass()
        {
            field1 = 1;
            field2 = "String1";
            field3 = "String2";
        }
    }

    public class UnorderedClass
    {
        private int field1;
        private string field2;
        private string field3;

        public string Field3
        {
            get { return field3; }
            set { field3 = value; }
        }

        public int Field1
        {
            get { return field1; }
            set { field1 = value; }
        }

        public string Field2
        {
            get { return field2; }
            set { field2 = value; }
        }

        public UnorderedClass()
        {
            field1 = 1;
            field2 = "String1";
            field3 = "String2";
        }
    }

    static class Program
    {
        static void Main()
        {
            UnorderedClass unordered = new UnorderedClass();
            OrderedClass ordered = new OrderedClass();

            XmlSerializer unorderedSerializer = 
                new XmlSerializer(typeof(UnorderedClass));
            XmlSerializer orderedSerializer = 
                new XmlSerializer(typeof(OrderedClass));

            Console.WriteLine("Unordered serialization:\n");
            unorderedSerializer.Serialize(Console.Out, unordered);
            Console.WriteLine("\n\nOrdered serialization:\n");
            orderedSerializer.Serialize(Console.Out, ordered);

            Console.ReadLine();

        }
    }
}
