//------------------------------------------------------------------------------
// <copyright file="xmlschema.cs" company="Microsoft">
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
  using System.Xml.Schema;

  /// <summary>
  /// Class that contains the Main entry point.
  /// </summary>
  public class XmlSchemaObjectModelSample
  {
    //Schemas to read
    private const string BOOKSCHEMA = "books.xsd";
    private const string POSCHEMA = "poschema.xsd";

    /// <summary>
    /// Main entry point for sample application.
    /// </summary>
    /// <param name="args"></param>
    public static void Main(string[] args)
    {
      XmlSchema myXmlSchema;
      XmlTextWriter myXmlTextWriter;
      try
      {
        Console.WriteLine ("Creating Schema in the Schema Object Model...");

        // Setup console output.
        myXmlTextWriter = new XmlTextWriter(Console.Out);
        myXmlTextWriter.Formatting = Formatting.Indented;
        myXmlTextWriter.Indentation = 2;

        //Create an XmlNameTable.
        XmlNameTable myXmlNameTable = new NameTable();

        //Add the nametable to the XmlSchemaCollection.
        XmlSchemaCollection myXmlSchemaCollection = new XmlSchemaCollection(myXmlNameTable);

        //Add some schemas to the XmlSchemaCollection.
        Console.WriteLine ("Reading and adding {0} schema.", BOOKSCHEMA);
        myXmlSchemaCollection.Add(null, BOOKSCHEMA);
        Console.WriteLine ("Reading and adding {0} schema.", POSCHEMA);
        myXmlSchemaCollection.Add(null, POSCHEMA);
        Console.WriteLine ("Added schemas successfully ...");

        Console.WriteLine ("Showing added schemas");

        foreach(XmlSchema myTempXmlSchema in myXmlSchemaCollection)
        {
          myXmlSchema = myTempXmlSchema;
          Console.WriteLine();
          string outfile = myTempXmlSchema.SourceUri.Replace("file:///", "");

          Console.WriteLine("Schema from: {0}", outfile);
          Console.WriteLine();
          Console.WriteLine("=== Start of Schema ===");
          Console.WriteLine();

          // Write out the various schema parts
          WriteXSDSchema(myXmlSchema, myXmlTextWriter);

          Console.WriteLine();
          Console.WriteLine();
          Console.WriteLine("=== End of Schema ===");
          Console.WriteLine();
          Console.WriteLine("Example of possible XML contents for: {0}", outfile);
          Console.WriteLine();
          Console.WriteLine("=== Start of Example ===");

          // Write out an example of the XML for the schema
          WriteExample(myXmlSchema, myXmlTextWriter);

          Console.WriteLine();
          Console.WriteLine();
          Console.WriteLine("=== End of Example ===");
        } //foreach
      } //try
      catch (Exception e)
      {
        Console.WriteLine ("Exception: {0}", e.ToString());
      } //catch
    } //Main()

    /// <summary>
    /// This method writes out the XSD object model.
    /// </summary>
    /// <param name="myXmlSchema"></param>
    /// <param name="myXmlTextWriter"></param>
    public static void WriteXSDSchema(XmlSchema myXmlSchema, XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("schema", XmlSchema.Namespace);
      myXmlTextWriter.WriteAttributeString("targetNamespace", myXmlSchema.TargetNamespace);
      foreach(XmlSchemaInclude include in myXmlSchema.Includes)
      {
        myXmlTextWriter.WriteStartElement("include", XmlSchema.Namespace);
        myXmlTextWriter.WriteAttributeString("schemaLocation", include.SchemaLocation);
        myXmlTextWriter.WriteEndElement();
      }

      foreach(object item in myXmlSchema.Items)
      {
        if (item is XmlSchemaAttribute)
          WriteXmlSchemaAttribute((XmlSchemaAttribute)item, myXmlTextWriter);      //attribute
        else if (item is XmlSchemaComplexType)
          WriteXmlSchemaComplexType((XmlSchemaComplexType)item, myXmlSchema, myXmlTextWriter);  //complexType
        else if (item is XmlSchemaSimpleType)
          WriteXmlSchemaSimpleType((XmlSchemaSimpleType)item, myXmlTextWriter);    //simpleType
        else if (item is XmlSchemaElement)
          WriteXmlSchemaElement((XmlSchemaElement)item, myXmlSchema, myXmlTextWriter);          //element
        else if (item is XmlSchemaAnnotation)
          WriteXmlSchemaAnnotation((XmlSchemaAnnotation)item, myXmlTextWriter);    //annotation
        else if (item is XmlSchemaAttributeGroup)
          WriteXmlSchemaAttributeGroup((XmlSchemaAttributeGroup)item, myXmlTextWriter); //attributeGroup
        else if (item is XmlSchemaNotation)
          WriteXmlSchemaNotation((XmlSchemaNotation)item, myXmlTextWriter);        //notation
        else if (item is XmlSchemaGroup)
          WriteXmlSchemaGroup((XmlSchemaGroup)item, myXmlSchema, myXmlTextWriter, null);              //group
        else
          Console.WriteLine("Not Implemented.");

      } //foreach
      myXmlTextWriter.WriteEndElement();
    } //WriteXSDSchema()

    /// <summary>
    /// This method writes out an XML schema attribute.
    /// </summary>
    /// <param name="attribute">XML schema attribute to be output.</param>
    /// <param name="myXmlTextWriter">XmlTextWriter to write output to.</param>
    public static void WriteXmlSchemaAttribute(XmlSchemaAttribute attribute, XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("attribute", XmlSchema.Namespace);
      if (attribute.Name != null)
      {
        myXmlTextWriter.WriteAttributeString("name", attribute.Name);
      } //if

      if (!attribute.RefName.IsEmpty)
      {
        myXmlTextWriter.WriteStartAttribute("ref", null);
        myXmlTextWriter.WriteQualifiedName(attribute.RefName.Name, attribute.RefName.Namespace);
        myXmlTextWriter.WriteEndAttribute();
      } //if

      if (!attribute.SchemaTypeName.IsEmpty)
      {
        myXmlTextWriter.WriteStartAttribute("type", null);
        myXmlTextWriter.WriteQualifiedName(attribute.SchemaTypeName.Name, attribute.SchemaTypeName.Namespace);
        myXmlTextWriter.WriteEndAttribute();
      } //if

      if (attribute.SchemaType != null)
      {
        WriteXmlSchemaSimpleType(attribute.SchemaType, myXmlTextWriter);
      } //if

      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaAttribute()

    /// <summary>
    /// 
    /// </summary>
    /// <param name="complexType"></param>
    /// <param name="myXmlSchema"></param>
    /// <param name="myXmlTextWriter"></param>
    public static void WriteXmlSchemaComplexType(XmlSchemaComplexType complexType, 
                                                 XmlSchema myXmlSchema, 
                                                 XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("complexType", XmlSchema.Namespace);
      if (complexType.Name != null)
      {
        myXmlTextWriter.WriteAttributeString("name", complexType.Name);
      } //if

      if (complexType.ContentModel != null)
      {
        Console.WriteLine("Not Implemented for this ContentModel.");
      } //if
      else
      {
        if (complexType.Particle != null)
          WriteXmlSchemaParticle(complexType.Particle, myXmlSchema, myXmlTextWriter);
        foreach(object o in complexType.Attributes)
        {
          if (o is XmlSchemaAttribute)
            WriteXmlSchemaAttribute((XmlSchemaAttribute)o, myXmlTextWriter);
        } //foreach
      } //else
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaComplexType()

    //XmlSchemaSimpleType
    public static void WriteXmlSchemaSimpleType(XmlSchemaSimpleType simpleType, 
                                                XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("simpleType", XmlSchema.Namespace);
      if (simpleType.Name != null)
        myXmlTextWriter.WriteAttributeString("name", simpleType.Name);

      if (simpleType.Content is XmlSchemaSimpleTypeRestriction)
        myXmlTextWriter.WriteStartElement("restriction", XmlSchema.Namespace);
      else if (simpleType.Content is XmlSchemaSimpleTypeList)
        myXmlTextWriter.WriteStartElement("list", XmlSchema.Namespace);
      else
        myXmlTextWriter.WriteStartElement("union", XmlSchema.Namespace);

      myXmlTextWriter.WriteEndElement();
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaSimpleType()

    //XmlSchemaParticle
    public static void WriteXmlSchemaParticle(XmlSchemaParticle particle, 
                                              XmlSchema myXmlSchema, 
                                              XmlTextWriter myXmlTextWriter)
    {
      if (particle is XmlSchemaElement)
      {
        WriteXmlSchemaElement((XmlSchemaElement)particle, myXmlSchema, myXmlTextWriter);
      } //if
      else if (particle is XmlSchemaSequence)
      {
        myXmlTextWriter.WriteStartElement("sequence", XmlSchema.Namespace);
        foreach(XmlSchemaParticle particle1 in ((XmlSchemaSequence)particle).Items)
          WriteXmlSchemaParticle(particle1, myXmlSchema, myXmlTextWriter);

        myXmlTextWriter.WriteEndElement();
      } //else if
      else if (particle is XmlSchemaGroupRef)
      {
        XmlSchemaGroupRef xsgr = (XmlSchemaGroupRef)particle;
        XmlSchemaGroup group = (XmlSchemaGroup)myXmlSchema.Groups[xsgr.RefName];
        WriteXmlSchemaGroup(group, myXmlSchema, myXmlTextWriter, xsgr.RefName.Name);
      } //else if
      else
      {
        Console.WriteLine("Not Implemented for this type: {0}", particle.ToString());
      } //else
    } //WriteXmlSchemaParticle()

    // XmlSchemaElement
    public static void WriteXmlSchemaElement(XmlSchemaElement element, XmlSchema myXmlSchema, XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("element", XmlSchema.Namespace);
      if (element.Name != null)
      {
        myXmlTextWriter.WriteAttributeString("name", element.Name);
      }//if

      if (!element.RefName.IsEmpty)
      {
        myXmlTextWriter.WriteStartAttribute("ref", null);
        myXmlTextWriter.WriteQualifiedName(element.RefName.Name, element.RefName.Namespace);
        myXmlTextWriter.WriteEndAttribute();
      } //if

      if (!element.SchemaTypeName.IsEmpty)
      {
        myXmlTextWriter.WriteStartAttribute("type", null);
        myXmlTextWriter.WriteQualifiedName(element.SchemaTypeName.Name, element.SchemaTypeName.Namespace);
        myXmlTextWriter.WriteEndAttribute();
      } //if

      if (element.SchemaType != null)
      {
        if (element.SchemaType is XmlSchemaComplexType)
          WriteXmlSchemaComplexType((XmlSchemaComplexType)element.SchemaType, myXmlSchema, myXmlTextWriter);
        else
          WriteXmlSchemaSimpleType((XmlSchemaSimpleType)element.SchemaType, myXmlTextWriter);
      } //if
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaElement()

    //XmlSchemaAnnotation
    public static void WriteXmlSchemaAnnotation(XmlSchemaAnnotation annotation,
                                                XmlTextWriter myXmlTextWriter)
    {
      // Not a complete implementation
      myXmlTextWriter.WriteStartElement("annotation", XmlSchema.Namespace);
      foreach (object o in annotation.Items)
      {
        if (o is XmlSchemaDocumentation)
        {
          myXmlTextWriter.WriteStartElement("documentation", XmlSchema.Namespace);
          XmlSchemaDocumentation xmlsd = (XmlSchemaDocumentation)o;
          foreach (XmlNode n in xmlsd.Markup)
          {
            myXmlTextWriter.WriteStartElement("documentation values", XmlSchema.Namespace);
            myXmlTextWriter.WriteString(n.Value);
            myXmlTextWriter.WriteEndElement();
          } //foreach
          myXmlTextWriter.WriteEndElement();
        } //if
        else
        {
          if (o is XmlSchemaAppInfo)
          {
            XmlSchemaAppInfo xsai = (XmlSchemaAppInfo)o;
            foreach (XmlNode n in xsai.Markup)
            {
              myXmlTextWriter.WriteStartElement("appinfo values", XmlSchema.Namespace);
              myXmlTextWriter.WriteString(n.Value);
              myXmlTextWriter.WriteEndElement();
            } //foreach
            
          } //if
        } //else

      } //foreach
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaAnnotation()

    //XmlSchemaAttributeGroup
    public static void WriteXmlSchemaAttributeGroup(XmlSchemaAttributeGroup attributeGroup, 
                                                    XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("attributeGroup", XmlSchema.Namespace);
      foreach (object o in attributeGroup.Attributes)
      {
        WriteXmlSchemaAttribute((XmlSchemaAttribute)o, myXmlTextWriter);
      } //foreach
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaAttributeGroup()

    //XmlSchemaGroup
    public static void WriteXmlSchemaGroup(XmlSchemaGroup group, 
                                           XmlSchema myXmlSchema,
                                           XmlTextWriter myXmlTextWriter, 
                                           string RefName)
    {
      myXmlTextWriter.WriteStartElement("group", XmlSchema.Namespace);
      
      if (RefName == null)
      {
        myXmlTextWriter.WriteAttributeString("name", XmlSchema.Namespace, group.Name);
      } //if
      else
      {
        myXmlTextWriter.WriteAttributeString("ref", XmlSchema.Namespace, RefName);
      } //else
      WriteXmlSchemaParticle(group.Particle, myXmlSchema, myXmlTextWriter);
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaGroup()

    //XmlSchemaNotation
    public static void WriteXmlSchemaNotation(XmlSchemaNotation notation, XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement("notation", XmlSchema.Namespace);
      myXmlTextWriter.WriteAttributeString(null, "name", XmlSchema.Namespace, notation.Name);
      myXmlTextWriter.WriteAttributeString(null, "public", XmlSchema.Namespace, notation.Public);
      myXmlTextWriter.WriteAttributeString(null, "system", XmlSchema.Namespace, notation.System);
      myXmlTextWriter.WriteEndElement();
    } //WriteXmlSchemaNotation()


    // Write out the example of the XSD usage
    public static void WriteExample(XmlSchema myXmlSchema, XmlTextWriter myXmlTextWriter)
    {
      foreach(XmlSchemaElement element in myXmlSchema.Elements.Values)
      {
        WriteExampleElement(element, myXmlSchema, myXmlTextWriter);
      } //foreach
    } //WriteExample()


    // Write some example elements
    public static void WriteExampleElement(XmlSchemaElement element, 
                                           XmlSchema myXmlSchema, 
                                           XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartElement(element.QualifiedName.Name, element.QualifiedName.Namespace);
      if (element.ElementType is XmlSchemaComplexType)
      {
        XmlSchemaComplexType type = (XmlSchemaComplexType)element.ElementType;
        if (type.ContentModel != null)
        {
          Console.WriteLine("Not Implemented for this ContentModel");
        } //if
        WriteExampleAttributes(type.Attributes, myXmlSchema, myXmlTextWriter);
        WriteExampleParticle(type.Particle, myXmlSchema, myXmlTextWriter);
      } //if
      else
      {
        WriteExampleValue(element.ElementType, myXmlTextWriter);
      } //else

      myXmlTextWriter.WriteEndElement();
    } //WriteExampleElement()

    // Write some example attributes
    public static void WriteExampleAttributes(XmlSchemaObjectCollection attributes, 
                                              XmlSchema myXmlSchema,
                                              XmlTextWriter myXmlTextWriter)
    {
      foreach(object o in attributes)
      {
        if (o is XmlSchemaAttribute)
        {
          WriteExampleAttribute((XmlSchemaAttribute)o, myXmlTextWriter);
        } //if
        else
        {
          XmlSchemaAttributeGroupRef xsagr = (XmlSchemaAttributeGroupRef)o;
          XmlSchemaAttributeGroup group = 
              (XmlSchemaAttributeGroup)myXmlSchema.AttributeGroups[xsagr.RefName];
          WriteExampleAttributes(group.Attributes, myXmlSchema, myXmlTextWriter);
        } //else
      } //foreach
    } //WriteExampleAttributes()

    // Write a single example attribute
    public static void WriteExampleAttribute(XmlSchemaAttribute attribute, XmlTextWriter myXmlTextWriter)
    {
      myXmlTextWriter.WriteStartAttribute(attribute.QualifiedName.Name, attribute.QualifiedName.Namespace);
      // The examples value
      WriteExampleValue(attribute.AttributeType, myXmlTextWriter);
      myXmlTextWriter.WriteEndAttribute();
    } //WriteExampleAttribute()

    // Write example particles
    public static void WriteExampleParticle(XmlSchemaParticle particle, 
                                            XmlSchema myXmlSchema,
                                            XmlTextWriter myXmlTextWriter)
    {
      Decimal max;

      if (particle.MaxOccurs == -1 || particle.MaxOccurs > 10000)
      {
        max = 5;
      } //if
      else
      {
        max = particle.MaxOccurs;
      } //else

      for (int i = 0; i < max; i ++)
      {
        if (particle is XmlSchemaElement)
        {
          WriteExampleElement((XmlSchemaElement)particle, myXmlSchema, myXmlTextWriter);
        } //if 
        else if (particle is XmlSchemaSequence)
        {
          foreach (XmlSchemaParticle particle1 in ((XmlSchemaSequence)particle).Items)
            WriteExampleParticle(particle1, myXmlSchema, myXmlTextWriter);
        } //else if
        else if (particle is XmlSchemaGroupRef)
        {
          XmlSchemaGroupRef xsgr = (XmlSchemaGroupRef)particle;
          XmlSchemaGroup group = (XmlSchemaGroup)myXmlSchema.Groups[xsgr.RefName];
          WriteExampleParticle(group.Particle, myXmlSchema, myXmlTextWriter);
        } //else if
        else
        {
          Console.WriteLine("Not Implemented for this type: {0}", particle.ToString());
        } //else
      } //for
    } //WriteExampleParticle()

    // Write the examples text values
    public static void WriteExampleValue(object schemaType, XmlTextWriter myXmlTextWriter)
    {
      Random random = new Random();
      XmlSchemaDatatype datatype = 
        (schemaType is XmlSchemaSimpleType) ? 
                     ((XmlSchemaSimpleType)schemaType).Datatype : (XmlSchemaDatatype)schemaType;

      // Consult the XSD to CLR conversion table for the correct type mappings
      Type type = datatype.ValueType;
      if (type == typeof(bool))
        myXmlTextWriter.WriteString("true");
      else if (type == typeof(int) || type == typeof(long))
        myXmlTextWriter.WriteString(random.Next(100).ToString());
      else if (type == typeof(float) || type == typeof(decimal))
        myXmlTextWriter.WriteString(((float)random.Next(100)/(float)random.Next(10, 20)).ToString("#.00"));
      else if (type == typeof(System.Xml.XmlQualifiedName))
        myXmlTextWriter.WriteString("qualified_name" + random.Next(100).ToString());
      else if (type == typeof(DateTime))
        myXmlTextWriter.WriteString("12-12-2002");
      else if (type == typeof(string))
        myXmlTextWriter.WriteString("ExampleString" + random.Next(100).ToString());
        // Handle the 'xsd:positiveInteger' XSD type in the SOMsample.xsd
      else if (type == typeof(System.UInt64))
        //positiveInteger
        myXmlTextWriter.WriteString(random.Next(100).ToString());
      else
        myXmlTextWriter.WriteString("Not Implemented for this datatype: " + datatype.ToString());
    } //WriteExampleValue()

  } //class XmlSchemaObjectModel
