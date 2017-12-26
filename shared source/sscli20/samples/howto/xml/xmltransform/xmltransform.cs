//------------------------------------------------------------------------------
// <copyright file="xmltransform.cs" company="Microsoft">
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
using System.Xml;
using System.Xml.Xsl;
using System.Xml.XPath;

/// <summary>
/// Demonstrates the use of the XslTranform class.
/// </summary>
public class TransformXMLSample
{
  private const string BOOKSXML = "books.xml";
  private const string PARAMSXML1 = "processparameters1.xml";
  private const string PARAMSXML2 = "processparameters2.xml";
  private const string BOOKSTYLESHEET = "books.xsl";
  private const string PARAMSTYLESHEET = "stylesheetgenerator.xsl";

  private static string nl = Environment.NewLine;

  /// <summary>
  /// Main entry point for the sample.
  /// </summary>
  /// <param name="args"></param>
  public static void Main(string[] args)
  {
    string marker = 
      nl + nl + "===============================================================================";
    Console.WriteLine(marker);
    ReadTransformWrite(BOOKSXML, BOOKSTYLESHEET);
    Console.WriteLine(marker);
    ReadTransformWrite(PARAMSXML1, PARAMSTYLESHEET);
    Console.WriteLine(marker);
    ReadTransformWrite(PARAMSXML2, PARAMSTYLESHEET);
    Console.WriteLine(marker);
  } //Main()

  /// <summary>
  /// Demonstrates the XslTransform class using two different overloads.
  /// One returns an XmlReader containing the transform.
  /// The other stores the transform in an XmlTextWriter.
  /// </summary>
  /// <param name="document"></param>
  /// <param name="stylesheet"></param>
  public static void ReadTransformWrite(string document, string stylesheet)
  {
    StreamReader stream = null;

    try
    {
      string outputFile = Path.GetFileNameWithoutExtension(document) + "_transform.xml";
      // XPathDocument stores the target XML in an efficient way for transformation.
      XPathDocument myXPathDocument = new XPathDocument(document);
      XslTransform myXslTransform = new XslTransform();
      myXslTransform.Load(stylesheet);

      // Get back the transform results in an XMLReader and output them to the console.
      XmlReader reader = myXslTransform.Transform(myXPathDocument, null);

      Console.WriteLine("Input XML file: {0} \nXSL stylesheet: {1}" + nl, document, stylesheet);
      Console.WriteLine(nl + "Layout of transformed XML content from XML reader:" + nl);
      OutputXml(reader);

      // Create an output file to hold the tranform.
      // Using an intermediate XmlTextWriter instead of the overload of the Transform method
      // that writes directly to a file allows more control of the formatting.
      XmlTextWriter writer = new XmlTextWriter(outputFile, null);
      writer.Formatting = Formatting.Indented;
      writer.Indentation = 2;
      myXslTransform.Transform(myXPathDocument, null, writer);
      writer.Close();

      //Output the contents of the tranformed file to the console.
      Console.WriteLine(nl + "Transformed XML content from file:" + nl);
      stream = new StreamReader (outputFile);
      Console.Write(stream.ReadToEnd());
    } //try

    catch (Exception e)
    {
      Console.WriteLine ("Exception: {0}", e.ToString());
    } //catch

    finally
    {
      if (stream != null)
        stream.Close();
    } //finally
  } //ReadTransformWrite()

  /// <summary>
  /// Output the tranformed XML in annotated form to the console.
  /// Calls the Output method.
  /// </summary>
  /// <param name="reader"></param>
  private static void OutputXml (XmlReader reader)
  {
    while (reader.Read())
    {
      switch (reader.NodeType)
      {
        case XmlNodeType.ProcessingInstruction:
          Output(reader, "ProcessingInstruction");
          break;

        case XmlNodeType.DocumentType:
          Output(reader, "DocumentType");
          break;

        case XmlNodeType.Document:
          Output(reader, "Document");
          break;

        case XmlNodeType.Comment:
          Output(reader, "Comment");
          break;

        case XmlNodeType.Element:
          Output(reader, "Element");
          while(reader.MoveToNextAttribute())
          {
            Output(reader, "Attribute");
          } //while
          break;

        case XmlNodeType.Text:
          Boolean flag = false;
          // Do not display whitespace text nodes
          for (int i=0; i < reader.Value.Length; i++)
          {
            if (!System.Char.IsWhiteSpace(reader.Value[i]))
              flag = true;
          } //for
          if(flag)
            Output (reader, "Text");
          break;
      } //switch
    } //while
    Console.WriteLine();
  } //OutputXML()

  /// <summary>
  /// Output the XML content to the console.
  /// </summary>
  /// <param name="reader"></param>
  /// <param name="nodeType"></param>
  private static void Output(XmlReader reader, string nodeType)
  {
    // Format the output
    for (int i=0; i < reader.Depth; i++)
    {
      Console.Write("  ");
    } //for

    Console.Write(nodeType + "<" + reader.Name + ">" + reader.Value);
    Console.WriteLine();
  } //Output()

} //class TransformXMLSample

