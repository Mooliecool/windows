// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

/*=====================================================================
  File:      gserialize.cs

  Summary:   Demonstrates runtime serialization on generic types.
*/


using System;
using System.Text;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Remoting;
using System.Runtime.Serialization.Formatters.Soap;
using System.Security.Permissions;

delegate T Converter<T>(int x);

public class App {
   private static void Usage() {
      Console.WriteLine(
         "Usage:\n" +
         "   -sb [filename] [nodecount]\tSerialize a linked list with "+
         "[nodecount]\n\t\t\t\tnodes to [filename] using binary formatter\n\n"+
         "   -sx [filename] [nodecount]\tSerialize a linked list with "+
         "[nodecount]\n\t\t\t\tnodes to [filename] using XML formatter\n\n"+
         "   -db [filename] \t\tDe-Serialize and display a linked list "+
         "from\n\t\t\t\t[filename] using binary formatter\n\n" +
         "   -dx [filename] \t\tDe-Serialize and display a linked list "+
         "from\n\t\t\t\t[filename] using XML formatter\n");
   }
 

   public static void Main(String[] args) {
      SerializeGraph(true, "tmp.soap", 10);
      DeserializeGraph(true, "tmp.soap");
      SerializeGraph(false, "tmp.dat", 10);
      DeserializeGraph(false, "tmp.dat");
   }

   private static Country[] countries = new Country[4]; 

   public static Country conv(int x) { return countries[x % 4]; }

   private static void SerializeGraph(bool useBinary, 
      string fileName, int nodeCount){
	   // Create a filestream object
	   Console.WriteLine("\nSerializing LinkedList to file: {0} ..\n", fileName);
	   Stream file = File.Open(fileName, FileMode.Create);

	   // Create a linked list object and populate it with random nodes
      LinkedList<Country> list= new LinkedList<Country>();
      list.PopulateRandom(nodeCount, new Converter<Country>(conv));
      Console.WriteLine(list); 

      // Create a formatter object based on command line arguments
      IFormatter formatter;
      if (useBinary)
          formatter = (IFormatter)new BinaryFormatter();
      else
          formatter = (IFormatter)new SoapFormatter();

      // Serialize the object graph to stream
      formatter.Serialize(file, list);
      
      // All done
      file.Close();    
   }

   private static void DeserializeGraph(bool useBinary, string fileName){
      // Verify that the input file exists
      if (!File.Exists(fileName)){
          Console.WriteLine("Input file not found: {0}\n", fileName);
          Usage();
          return;
      }

      // Open the requested file to a stream object
      Console.WriteLine("\nDeserializing LinkedList from file: {0} ..\n", 
         fileName);
      Stream file = File.Open(fileName, FileMode.Open);

      // Create a formatter object based on command line arguments
      IFormatter formatter;
      if (useBinary)
          formatter = (IFormatter)new BinaryFormatter();
      else
          formatter = (IFormatter)new SoapFormatter();
          
      // Deserialize the object graph from stream
      Object obj = formatter.Deserialize(file);
      Console.WriteLine("type of input is " + obj.GetType());
      LinkedList<Country> list = obj as LinkedList<Country>;

      // Tests like this are relevant in projects which 
      // serialize multiple object types
      if(list != null){
         Console.WriteLine(list);
      }else{
         Console.WriteLine("The deserialized object graph"+
            " is not a LinkedList.");
      }

      // All done
      file.Close();

   }
}

[Serializable]
public class Country {
    public String name;
    public String capital;
    public Country (String n, String c) {
        name = n;
        capital = c;
    }
}
    
// Note that this type is attributed as serializable
[Serializable]
class LinkedList<T>{

   // Construct an empty LinkedList
   public LinkedList(){
      nodeHead = new Node<T>();
   }

   // Represent the LinkedList as a string
   public override string ToString(){
      StringBuilder list = new StringBuilder("List:\n");

      int index = 0;
      Node<T> iter = nodeHead.Next;

      while(iter != null){
         list.Append("Node #"+(index++)+"\n"+iter+"\n");         
         iter = iter.Next;
      }      
      return list.ToString();
   }

   // Populate the list with an arbitrary number of nodes with random data
   public void PopulateRandom(int numNodes, Converter<T> c){
      Random rand = new Random();

      while(numNodes-- != 0){
         Add(c(rand.Next(1001)));
      }   
   }    
   
   // Add a node
   public void Add(T d){
      Node<T> node = new Node<T>();

      node.Data = d;

      node.Add(nodeHead);
   }   

   // Reference to the empty head node
   private Node<T> nodeHead;

   // This nested type is also attributed as serializable
   [Serializable]
   public class Node<T>{
      // Construct a Node object
      public Node(){
         next = null;   
      }

      // Add a node object to a list
      public void Add(Node<T> nodeHead){
         Node<T> iter = nodeHead;
         
         while(iter.next != null)
            iter = iter.next;

         iter.next = this;
         next = null;
      }

      // Accessor property for data private field
      public T Data{
         get{return data;}
         set{data = value;}
      }

      // Read-only property for next private field
      public Node<T> Next{
         get{return next;}
      }

      // Represent the node as a string
      public override string ToString(){
         return "\tData   = "+data;
      }
   
      // Private field referencing the next node in the list
      private Node<T> next;         

      // Private fields containing node data
      private T data;
   }
}


