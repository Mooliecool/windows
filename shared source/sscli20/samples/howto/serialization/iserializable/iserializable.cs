//------------------------------------------------------------------------------
// <copyright file="iserializable.cs" company="Microsoft">
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
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization.Formatters.Soap;
using System.Text;
using System.Collections;

public class SimpleGraph 
{
  public const string XMLFILENAME = "objectgraph.xml";

  public static void Main(string[] args) 
  {
    Console.WriteLine("Creating object graph");
    TreeNode initialNode = new TreeNode("");
    FillTree(initialNode);
    initialNode.PrintTree(Console.Out);

    Console.Write("Serializing object graph to file {0}", XMLFILENAME);
    Stream ToXml = (Stream)File.Open(XMLFILENAME, FileMode.Create, FileAccess.Write);
    SoapFormatter OutFormat = new SoapFormatter();
    OutFormat.Serialize(ToXml, initialNode);
    ToXml.Close();
    Console.WriteLine("Finished");

    Console.WriteLine("\nPress enter to deserialize and output results.");
    Console.Read();

    Console.Write("Deserializing object graph from file {0}", XMLFILENAME);
    Stream FromXML = (Stream)File.Open(XMLFILENAME, FileMode.Open, FileAccess.Read);
    SoapFormatter InFormat = new SoapFormatter();

    TreeNode restoredNode = (TreeNode)InFormat.Deserialize(FromXML);
    Console.WriteLine("Finished");
    FromXML.Close();
    restoredNode.PrintTree(Console.Out);

  } //Main()

  public static void FillTree(TreeNode InputNode) 
  {
    //Fill the tree with all the types in the BCL assembly in inheritence hierarchy.
    Type [] types = typeof(object).Module.Assembly.GetExportedTypes();
    InputNode.AddChild(new TreeNode(typeof(object).FullName));

    //Go through the types and for each base type add any new derived types.
    //Since Find returns a reference to a node within InputNode, the original
    //node is modified when n is modified.
    //AddChild checks for existence and only adds if new.
    foreach (Type t in types) 
    {
      if (t.BaseType != null && t.BaseType.FullName != null)
      {
        TreeNode n = InputNode.Find(t.BaseType.FullName);
        if (n != null) 
        {
          n.AddChild(new TreeNode(t));
        } //if
      } //if
    } //foreach

  } //FillTree()

} //class SimpleGraph

/// <summary>
/// This is the type that will be serialized.
/// </summary>
[Serializable] public class TreeNode : ISerializable 
{
  private Object m_value;
  private ArrayList m_children;

  /// <summary>
  /// Private constructor.
  /// The ISerializable interface implies a constructor with the signature:
  ///    Constructor(SerializationInfo info, StreamingContext context)
  ///    
  /// At deserialization time, the current constructor is called only after the data 
  /// in the SerializationInfo has been deserialized by the formatter. 
  /// In general, this constructor should be protected if the class is not sealed.
  /// </summary>
  /// <param name="info"></param>
  /// <param name="c"></param>
  private TreeNode(SerializationInfo info, StreamingContext c) 
  {
    m_value = info.GetValue("value", typeof(object));
    m_children = new ArrayList();
    Object o;

    for (int i = 1; i < info.MemberCount; i++) 
    {
      o = info.GetValue(i.ToString(), typeof(object));
      m_children.Add(o);
    } //for
  } //private constructor

  /// <summary>
  /// Public constructor.
  /// </summary>
  /// <param name="val"></param>
  public TreeNode(Object val) 
  {
    if (val == null) 
    {
      throw new Exception("Input value val must not be null");
    }//if
    m_value = val;
    m_children = new ArrayList();
  } //public constructor


  /// <summary>
  /// Explicitly implements the required method to support
  /// the ISerializable interface.
  /// </summary>
  /// <param name="info"></param>
  /// <param name="context"></param>
  void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) 
  {
    if (m_value == null) 
    {
      info.AddValue("value", "NULL");
    } //if
    else 
    {
      info.AddValue("value", m_value.ToString() + "(SAVED)");
    } //else
    int i = 1;
    foreach (object o in m_children) 
    {
      if (o != null)
      {
        info.AddValue(i.ToString(), o);
        i++;
      } //if
    } //foreach
  } //ISerializable.GetObjectData()

  /// <summary>
  /// Property Value
  /// </summary>
  public Object Value 
  {
    get 
    {
      return m_value;
    } //get
  } //property Value

  /// <summary>
  /// Adds a child node to the current node ArrayList
  /// </summary>
  /// <param name="child"></param>
  public void AddChild(TreeNode child) 
  {
    if (!m_children.Contains(child))
    {
      m_children.Add(child);
    } //if
  } //AddChild()

  /// <summary>
  /// Find an node in the ArrayList of the current node.
  /// Return a reference to the found node.
  /// </summary>
  /// <param name="val"></param>
  /// <returns></returns>
  public TreeNode Find(Object val) 
  {
    if (val == null) 
    {
      throw new Exception("Input value val must not be null");
    } //if
    if (m_value.Equals(val)) 
    {
      return this;
    } //if
    foreach (TreeNode t in m_children) 
    {
      TreeNode w = t.Find(val);
      if (w != null) return w;
    } //foreach
    return null;
  } //Find()

  /// <summary>
  /// Overriding Equals for comparision in adding to ArrayList.
  /// </summary>
  /// <param name="obj"></param>
  /// <returns></returns>
  override public bool Equals(Object obj) 
  {
    if (!(obj is TreeNode)) return false;
    TreeNode t = (TreeNode) obj;
    return (t.m_value.Equals(this.m_value));
  } //override Equals

  override public int GetHashCode()
  {
    return m_value.GetHashCode();
  } //override GetHashCode()
                  

  override public string ToString() 
  {
    return m_value.ToString();
  } //override ToString()

  /// <summary>
  /// Overload that starts the print process at position 0.
  /// </summary>
  /// <param name="output"></param>
  public void PrintTree(TextWriter  output) 
  {
    PrintTree(output, 0);
  } //PrintTree()

  /// <summary>
  /// Overload that uses the position within TextWriter.
  /// </summary>
  /// <param name="output"></param>
  /// <param name="offset"></param>
  private void PrintTree(TextWriter  output, int offset) 
  {
    StringBuilder sb1 = new StringBuilder();
    StringBuilder sb2 = new StringBuilder();
    for (int i =0; i < offset-1; i++) 
    {
      sb1.Append(" ");
      sb1.Append("|");
      sb2.Append(" ");
      sb2.Append("|");
    } //for
    if (offset >=1) 
    {
      sb1.Append(" ");
      sb1.Append("+-");
      sb2.Append(" ");
      sb2.Append("|");
    } //if
    output.WriteLine("{0}", sb2);
    output.WriteLine("{0}{1}", sb1, m_value);
    foreach (TreeNode t in m_children) 
    {
      t.PrintTree(output, offset+1);
    } //foreach
  } //PrintTree()
} //class TreeNode

