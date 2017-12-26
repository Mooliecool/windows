//------------------------------------------------------------------------------
// <copyright file="simpleserialize.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------



using System;
using System.IO;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

class SerializeTest
{
  const string FILENAME = "objectgraph.bin";

  public static void Main(String[] args) 
  {
    Console.WriteLine ("Fill MyList object with content\n");
    MyList l = new MyList();
    for (int x=0; x< 10; x++) 
    {
      Console.WriteLine (x);
      l.Add (x);
    } // end for
    Console.WriteLine("\nSerializing object graph to file {0}", FILENAME);
    Stream outFile = File.Open(FILENAME, FileMode.Create, FileAccess.ReadWrite);
    BinaryFormatter outFormat = new BinaryFormatter();
    outFormat.Serialize(outFile, l);
    outFile.Close();
    Console.WriteLine("Finished\n");

    Console.WriteLine("Deserializing object graph from file {0}", FILENAME);
    Stream inFile = File.Open(FILENAME, FileMode.Open, FileAccess.Read);
    BinaryFormatter inFormat = new BinaryFormatter();
    MyList templist = (MyList)inFormat.Deserialize(inFile);
    Console.WriteLine ("Finished\n");
    
    foreach (MyListItem mli in templist) 
    {
      Console.WriteLine ("List item number {0}, square root {1}", mli.Number, mli.Sqrt);
    } //foreach

    inFile.Close();
  } // end main
} // end class

/// <summary>
/// Class to serialize.
/// Note that class derives from a class that is serializable but
/// must itself be marked with the Serializable attribute.
/// </summary>
[Serializable]
class MyList : ArrayList
{
  public override int Add(object InValue)
  {
    MyListItem mli = new MyListItem(InValue);
    return base.Add(mli);
  } //Add()
  
} //class MyList

/// <summary>
/// Item class for MyList items.
/// </summary>
[Serializable]
struct MyListItem
{
  public int Number;
  public double Sqrt;

  public MyListItem(object InValue)
  {
    this.Number = (int) InValue;
    this.Sqrt = Math.Sqrt((double)this.Number);
  } //constructor
} //class MyListItem

