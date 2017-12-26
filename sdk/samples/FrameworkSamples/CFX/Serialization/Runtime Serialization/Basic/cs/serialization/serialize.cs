//-----------------------------------------------------------------------
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
/*=====================================================================
  File:      Serialize.cs

  Summary:   Demonstrates how to use runtime serialization.

=====================================================================*/


using System;
using System.Text;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml;

namespace Microsoft.Samples
{
    public class App
    {
        private static void Usage()
        {
            Console.WriteLine("Usage:\n" + "   -sb [filename] [nodecount]\tSerialize a linked list with " + "[nodecount]\n\t\t\t\tnodes to [filename] using binary formatter\n\n" + "   -sx [filename] [nodecount]\tSerialize a linked list with " + "[nodecount]\n\t\t\t\tnodes to [filename] using XML formatter\n\n" + "   -db [filename] \t\tDe-Serialize and display a linked list " + "from\n\t\t\t\t[filename] using binary formatter\n\n" + "   -dx [filename] \t\tDe-Serialize and display a linked list " + "from\n\t\t\t\t[filename] using XML formatter\n");
        }
        public static void Main(String[] args)
        {
            try
            {
                // Parse command line args
                bool useBinary = false;
                bool serialize = false;
                int shiftArgs = 0;

                if (args[0][0] == '-' || args[0][0] == '/')
                {
                    if (args[0][1] == '?')
                    {
                        Usage();
                        return;
                    }

                    useBinary = (args[0][2] == 'b');
                    serialize = (args[0][1] == 's');
                    shiftArgs = 1;
                }

                // Serialize or Deserialize an object graph
                if (serialize)
                {
                    SerializeGraph(useBinary, args[0 + shiftArgs], int.Parse(args[1 + shiftArgs]));
                }
                else
                {
                    DeserializeGraph(useBinary, args[0 + shiftArgs]);
                }
            }
            catch (IndexOutOfRangeException)
            {
                Usage();
            }
        }
        private static void SerializeGraph(bool useBinary, string fileName, int nodeCount)
        {
            // Create a filestream object
            Console.WriteLine("\nSerializing LinkedList to file: {0} ..\n", fileName);

            Stream file = File.Open(fileName, FileMode.Create);

            // Create a linked list object and populate it with random nodes
            LinkedList list = new LinkedList();

            list.PopulateRandom(nodeCount);
            Console.WriteLine(list);

            // Create a formatter object based on command line arguments
            IFormatter formatter = useBinary ? (IFormatter)new BinaryFormatter() : (IFormatter)new SoapFormatter();

            // Serialize the object graph to stream
            formatter.Serialize(file, list);

            // All done
            file.Close();
        }
        private static void DeserializeGraph(bool useBinary, string fileName)
        {
            // Verify that the input file exists
            if (!File.Exists(fileName))
            {
                Console.WriteLine("Input file not found: {0}\n", fileName);
                Usage();
                return;
            }

            // Open the requested file to a stream object
            Console.WriteLine("\nDeserializing LinkedList from file: {0} ..\n", fileName);

            Stream file = File.Open(fileName, FileMode.Open);

            // Create a formatter object based on command line arguments
            IFormatter formatter = useBinary ? (IFormatter)new BinaryFormatter() : (IFormatter)new SoapFormatter();

            // Deserialize the object graph from stream
            try
            {
                LinkedList list = formatter.Deserialize(file) as LinkedList;

                // Tests like this are relevant in projects which 
                // serialize multiple object types
                if (list != null)
                {
                    Console.WriteLine(list);
                }
                else
                {
                    Console.WriteLine("The deserialized object graph" + " is not a LinkedList.");
                }
            }
                // Handler for BinaryFormatter exception
                catch (SerializationException)
            {
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.");
            }
                // Handler for SoapException
                catch (ArgumentException)
            {
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.");
            }
                // Handler for XmlFormatter exception
                catch (XmlException)
            {
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.");
            }

            // All done
            file.Close();
        }
    }
    // Note that this type is attributed as serializable
    [Serializable]
    class LinkedList
    {
        // Construct an empty LinkedList
        public LinkedList()
        {
            nodeHead = new Node();
        }
        // Represent the LinkedList as a string
        public override string ToString()
        {
            StringBuilder list = new StringBuilder("List:\n");
            int index = 0;
            Node nodeIterator = nodeHead.Next;

            while (nodeIterator != null)
            {
                list.Append("Node #" + (index++) + "\n" + nodeIterator + "\n");
                nodeIterator = nodeIterator.Next;
            }

            return list.ToString();
        }
        // Populate the list with an arbitrary number of nodes with random data
        public void PopulateRandom(int numNodes)
        {
            Random rand = new Random();

            while (numNodes-- != 0)
            {
                Add("Semi-Random String: " + rand.Next(1001), rand.Next(1001));
            }
        }
        // Add a node
        public void Add(String text, int number)
        {
            Node node = new Node();

            node.TextData = text;
            node.NumberData = number;
            node.Add(nodeHead);
        }
        // Reference to the empty head node
        private Node nodeHead;
        // This nested type is also attributed as serializable
        [Serializable]
        public class Node
        {
            // Construct a Node object
            public Node()
            {
                next = null;
            }
            // Add a node object to a list
            public void Add(Node nodeHead)
            {
                Node nodeIterator = nodeHead;

                while (nodeIterator.next != null)
                    nodeIterator = nodeIterator.next;

                nodeIterator.next = this;
                next = null;
            }
            // Accessor property for textData private field
            public string TextData
            {
                get { return textData; }
                set { textData = value; }
            }
            // Accessor property for numberData private field
            public int NumberData
            {
                get { return numberData; }
                set { numberData = value; }
            }
            // Read-only property for next private field
            public Node Next
            {
                get { return next; }
            }
            // Represent the node as a string
            public override string ToString()
            {
                return "\tTextData   = \"" + TextData + "\"\n\tNumberData = " + NumberData;
            }
            // Private field referencing the next node in the list
            private Node next;
            // Private fields containing node data
            private string textData;
            private int numberData;
        }
    }
}