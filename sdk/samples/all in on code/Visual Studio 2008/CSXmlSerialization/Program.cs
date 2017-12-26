/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSXmlSerialization
* Copyright (c) Microsoft Corporation.
* 
* This sample shows how to serialize an in-memory object to a local xml file 
* and how to deserialize the xml file back to an in-memory object using 
* C#. The designed MySerializableType includes int, string, generic, as well
* as customized type field and property.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/29/2009 3:00 PM Colbert Zhou Created
* * 8/20/2009 12:01 AM Jialiang Ge Reviewed
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Collections;
#endregion


namespace CSXmlSerialization
{
    class Program
    {
        static void Main(string[] args)
        {
            /////////////////////////////////////////////////////////////////
            // Serialize the object to an XML file.
            // 

            // Create and initialize a MySerializableType instance.
            MySerializableType instance = new MySerializableType();
            instance.BoolValue = true;
            instance.IntValue = 1;
            instance.StringValue = "Test String";
            instance.ListValue.Add("List Item 1");
            instance.ListValue.Add("List Item 2");
            instance.ListValue.Add("List Item 3");
            instance.AnotherTypeValue = new AnotherType();
            instance.AnotherTypeValue.IntValue = 2;
            instance.AnotherTypeValue.StringValue = "Inner Test String";

            // Create the serializer
            XmlSerializer serializer = new XmlSerializer(typeof(MySerializableType));

            // Serialize the object to an XML file
            using (StreamWriter streamWriter = File.CreateText(
                "CSXmlSerialization.xml"))
            {
                serializer.Serialize(streamWriter, instance);
            }


            /////////////////////////////////////////////////////////////////
            // Deserialize from a XML file to an object instance.
            // 

            // Deserialize the object
            MySerializableType deserializedInstance;
            using (StreamReader streamReader = File.OpenText(
                "CSXmlSerialization.xml"))
            {
                deserializedInstance = serializer.Deserialize(streamReader) 
                    as MySerializableType;
            }

            // Dump the object
            Console.WriteLine("BoolValue: {0}", deserializedInstance.BoolValue);
            Console.WriteLine("IntValue: {0}", deserializedInstance.IntValue);
            Console.WriteLine("StringValue: {0}", deserializedInstance.StringValue);
            Console.WriteLine("AnotherTypeValue.IntValue: {0}", 
                deserializedInstance.AnotherTypeValue.IntValue);
            Console.WriteLine("AnotherTypeValue.StringValue: {0}", 
                deserializedInstance.AnotherTypeValue.StringValue);
            Console.WriteLine("ListValue: ");
            foreach (object obj in deserializedInstance.ListValue)
            {
                Console.WriteLine(obj.ToString());
            }
        }
    }


    /// <summary>
    /// Serializable Type Declaration
    /// </summary>
    [Serializable()]
    public class MySerializableType
    {
        // String field and property
        private string stringValue;
        public string StringValue
        {
            get { return stringValue; }
            set { stringValue = value; }
        }

        // Bool field and property
        private bool boolValue;
        public bool BoolValue
        {
            get { return boolValue; }
            set { boolValue = value; }
        }

        // Int field and property
        private int intValue;
        public int IntValue
        {
            get { return intValue; }
            set { intValue = value; }
        }

        // Another type field and property
        private AnotherType anotherTypeValue;
        public AnotherType AnotherTypeValue
        {
            get { return anotherTypeValue; }
            set { anotherTypeValue = value; }
        }

        // Generic type field and property
        private List<string> listValue = new List<string>();
        public List<string> ListValue
        {
            get { return listValue; }
            set { listValue = value; }
        }

        // Ignore a field using NonSerialized attribute
        [NonSerialized()]
        private int ignoredField = 1;
    }

    /// <summary>
    /// Another Type Declaration
    /// </summary>
    [Serializable()]
    public class AnotherType
    {
        private string stringValue;
        public string StringValue
        {
            get { return stringValue; }
            set { stringValue = value; }
        }

        private int intValue;
        public int IntValue
        {
            get { return intValue; }
            set { intValue = value; }
        }
    }
}
