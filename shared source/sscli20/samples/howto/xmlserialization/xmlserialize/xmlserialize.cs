//------------------------------------------------------------------------------
// <copyright file="xmlserialize.cs" company="Microsoft">
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
using System.Xml.Serialization;
using System.IO;
using XmlSerializationHowTo;

public class XmlSerialize 
{
  const string XMLDATAFILE = "inputdata.xml";
  const string XMLOUTPUTFILE = "purchaseorder_serialized.xml";


  public static void Main(string[] args) 
  {
    // Type PurchaseOrder is created from purchaseorder.xsd which generates the 
    // C# code which is then compiled and referenced from this application.
    Console.WriteLine("Creating new serializer from type PurchaseOrder\n");
    XmlSerializer serializer = new XmlSerializer(typeof(PurchaseOrder));
    
    Console.WriteLine("Opening TextReader on {0}\n", XMLDATAFILE);
    
    TextReader reader = new StreamReader(XMLDATAFILE);

    Console.WriteLine("Creating deserialized PurchaseOrder object from XmlTextReader\n");
    PurchaseOrder po = (PurchaseOrder)serializer.Deserialize(reader);
    reader.Close();

    //Changing some of the content of the XML
    po.orderDate = System.DateTime.Now;
    Console.WriteLine("Number of order items: {0}", po.Items.Length);
    foreach (OrderItem oi in po.Items)
    {
      Console.WriteLine("Item: {0}, quantity: {1}", oi.productName, oi.quantity);
      oi.shipDate = DateTime.Now;
      oi.shipDateSpecified = true;
    } //foreach

    //Add a new order item to serialize.
    OrderItem newOI = new OrderItem();
    newOI.comment = "Free bonus lawnmower cover at no extra charge.";
    newOI.partNum = "375-DF";
    newOI.productName = "Lawnmower cover";
    newOI.quantity = "1";
    newOI.shipDate = DateTime.Now;
    newOI.shipDateSpecified = true;
    newOI.price = Convert.ToDecimal("00.00");

    OrderItem [] orders = new OrderItem [po.Items.Length + 1];
    po.Items.CopyTo(orders, 0);
    orders[po.Items.Length] = newOI;
    po.Items = orders;

    Console.WriteLine("Opening XML output file {0}\n", XMLOUTPUTFILE);
    TextWriter writer = new StreamWriter(XMLOUTPUTFILE);

    Console.WriteLine("Serializing XML data to output file\n");
    serializer.Serialize(writer, po);
    writer.Close();
  } //Main()
} //class XmlSerialize
