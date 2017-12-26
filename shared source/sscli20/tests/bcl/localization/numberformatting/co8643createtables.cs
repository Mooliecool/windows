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
using System;
using System.IO;
using System.Collections;
using System.Text;
using System.Globalization;
using System.Resources;
using System.Xml;  
using System.Xml.Schema;
class CreateTable
{
 private Boolean verbose = false;
 const String strInt = "Integer";
 const String strDec = "Decimal";
 const String strDbl = "Double";
 const String strPic = "Picture";
 const String xmlSchemaFile = "NumberSchema";
 const String xmlDataFile = "NumberData";
 const String separator = "~";
 private String EOL = Environment.NewLine;
 const String resourceBaseName = "Co8643createtables";
 private String baseFileName = "table";
 private String resCultures = "cultures";
 Int64[] numberTable = {Int64.MinValue, Int32.MinValue, Int16.MinValue, -10000, -1234, SByte.MinValue, -5, 0, 
			5, SByte.MaxValue, Byte.MaxValue, 100, 1259, Int16.MaxValue, UInt16.MaxValue, Int32.MaxValue, 
			UInt32.MaxValue, Int64.MaxValue
 };
 Decimal[] decimalTable = {Decimal.MinValue, 0.1234567890123456789012345678m, 12345678901234.56789012345678m,
			   0.1000000000000000000000000009m, 
			   Decimal.MaxValue,
 };
 Double[] doubleTable = 	{Double.MinValue, Single.MinValue, -1.23e50, 0.0, Double.Epsilon, Single.Epsilon,  
				 Single.MaxValue, Double.MaxValue, Double.PositiveInfinity, Double.NegativeInfinity, Double.NaN, 
 };
 String[] numericalFormats = {
   "C", "C4", "D", "D4", "E", "E4", "F", "F4", "G", "G4", "N", "N4", "P", "P4", "X", "X4",  
 };
 String[] decimalFormats = {
   "C", "C4",  "E", "E4", "F", "F4", "G", "G4", "N", "N4", "P", "P4",   
 };
 String[] doubleFormats = {
   "C", "C4",  "E", "E4", "F", "F4", "G", "G4", "N", "N4", "P", "P4",   
 };
 String[] pictureFormats = {
   "0", "0.00", "#,##0", "#,##0.00", "#,##0;(#,##0)", "#,##0.00;(#,##0.00)", "0%", "0.00%", "0.00E+00", 
   "##00", "%#,0#", "%#,0#,,,", "#.0#", "#,.0#", "#..0.#", "00.00E+0", "00.00e-0", "00.00e0", "0.0e+0;-00;%0", 
 };
 CultureInfo[] cultures = {
   new CultureInfo("en-US"), 
   new CultureInfo("ja-JP"), 
   new CultureInfo("de-DE"),
   new CultureInfo("zh-CN"),
   new CultureInfo("zh-TW"),
   new CultureInfo("ko-KR"),
   new CultureInfo("es-ES"), 
   new CultureInfo("fr-FR"),
   new CultureInfo("it-IT"),
   new CultureInfo("ar-SA"),
   new CultureInfo("he-IL"),
   new CultureInfo("ru-RU"),
   new CultureInfo("el-GR"),
   new CultureInfo("th-TH"),
 };
 private void DoThings()
   {
   String[] bogusHeaders;
   ResourceWriter resWriter;
   CultureInfo culture;
   NumberFormatInfo numInfo;
   StreamWriter xmlwriter;
   XmlTextWriter myXmlTextWriter;
   resWriter = new ResourceWriter(resourceBaseName + ".resources");
   resWriter.AddResource(resCultures, cultures);
   for(int iCul = 0; iCul<cultures.Length; iCul++){
   culture = cultures[iCul];			
   numInfo = NumberFormatInfo.GetInstance(culture);
   if(verbose)
     Console.WriteLine(culture);
   if(verbose)
     Console.WriteLine("Numerical values and its formats");
   bogusHeaders = new String[numericalFormats.Length];
   for(int i=0; i<bogusHeaders.Length; i++){
   bogusHeaders[i] = "AA_" + i;
   }
   if(iCul==0){
   GenerateXmlSchema(bogusHeaders, strInt);
   }
   xmlwriter = File.CreateText(xmlDataFile  + "_" + strInt + "_" + culture.ToString() + ".xml");
   myXmlTextWriter = new XmlTextWriter(xmlwriter);
   myXmlTextWriter.Formatting = Formatting.Indented;
   myXmlTextWriter.WriteStartElement("NewDataSet");
   WriteRealHeaders(myXmlTextWriter, bogusHeaders, numericalFormats);
   WriteValues(myXmlTextWriter, NumberType.Int, bogusHeaders, numericalFormats, numInfo);
   WriteDataToResourceFile(strInt, resWriter, culture);
   if(verbose)
     Console.WriteLine("Decimal values and its formats");
   bogusHeaders = new String[decimalFormats.Length];
   for(int i=0; i<bogusHeaders.Length; i++){
   bogusHeaders[i] = "AA_" + i;
   }
   if(iCul==0){
   GenerateXmlSchema(bogusHeaders, strDec);
   }
   xmlwriter = File.CreateText(xmlDataFile  + "_" + strDec + "_" + culture.ToString() + ".xml");
   myXmlTextWriter = new XmlTextWriter(xmlwriter);
   myXmlTextWriter.Formatting = Formatting.Indented;
   myXmlTextWriter.WriteStartElement("NewDataSet");
   WriteRealHeaders(myXmlTextWriter, bogusHeaders, decimalFormats);
   WriteValues(myXmlTextWriter, NumberType.Decimal, bogusHeaders, decimalFormats, numInfo);
   WriteDataToResourceFile(strDec, resWriter, culture);
   if(verbose)
     Console.WriteLine("Double values and its formats");
   bogusHeaders = new String[doubleFormats.Length];
   for(int i=0; i<bogusHeaders.Length; i++){
   bogusHeaders[i] = "AA_" + i;
   }
   if(iCul==0){
   GenerateXmlSchema(bogusHeaders, strDbl);
   }
   xmlwriter = File.CreateText(xmlDataFile  + "_" + strDbl + "_" + culture.ToString() + ".xml");
   myXmlTextWriter = new XmlTextWriter(xmlwriter);
   myXmlTextWriter.Formatting = Formatting.Indented;
   myXmlTextWriter.WriteStartElement("NewDataSet");
   WriteRealHeaders(myXmlTextWriter, bogusHeaders, doubleFormats);
   WriteValues(myXmlTextWriter, NumberType.Double, bogusHeaders, doubleFormats, numInfo);
   WriteDataToResourceFile(strDbl, resWriter, culture);
   if(verbose)
     Console.WriteLine("Picture values and its formats");
   GoDoPictureFormatting(culture, resWriter);						
   }
   resWriter.Generate();
   resWriter.Close();
   }
 static void Main()
   {
   new CreateTable().DoThings();
   }	
 void GenerateXmlSchema(String[] formats, String fileFormatType)
   {
   StreamWriter writer = File.CreateText(xmlSchemaFile + "_" + fileFormatType + ".xml");		
   XmlSchema schema;
   XmlSchemaElement element;
   XmlSchemaComplexType complexType;
   XmlSchemaSequence sequence;
   schema = new XmlSchema();
   element = new XmlSchemaElement();
   schema.Items.Add(element);
   element.Name = "Table";
   complexType = new XmlSchemaComplexType();
   element.SchemaType = complexType;
   sequence = new XmlSchemaSequence();
   complexType.Particle = sequence;
   element = new XmlSchemaElement();
   element.Name = "Number";
   element.SchemaTypeName = new XmlQualifiedName("string", "http://www.w3.org/2001/XMLSchema");
   sequence.Items.Add(element);
   for(int i=0; i<formats.Length; i++){
   element = new XmlSchemaElement();
   element.Name = formats[i];
   element.SchemaTypeName = new XmlQualifiedName("string", "http://www.w3.org/2001/XMLSchema");
   sequence.Items.Add(element);
   }
   schema.Compile(new ValidationEventHandler(ValidationCallbackOne));
   schema.Write(writer);
   writer.Close();
   }
 public static void ValidationCallbackOne(object sender, ValidationEventArgs args) {
 Console.WriteLine(args.Message);
 }
 private void WriteRealHeaders(XmlTextWriter writer, String[] elements, String[] values){
 writer.WriteStartElement("Table");
 writer.WriteElementString("Number", "Number");
 for(int j=0; j<elements.Length; j++){
 writer.WriteElementString(elements[j], values[j]);
 }
 writer.WriteEndElement();
 }
 private void WriteValues(XmlTextWriter writer, NumberType numType, String[] elements, String[] values, NumberFormatInfo info){
 switch(numType){
 case NumberType.Int:
   {
   Int64 value;
   for(int i=0; i<numberTable.Length; i++){
   value = numberTable[i];
   writer.WriteStartElement("Table");
   writer.WriteElementString("Number", value.ToString(info));
   for(int j=0; j<elements.Length; j++){
   writer.WriteElementString(elements[j], value.ToString(values[j], info));
   }
   writer.WriteEndElement();
   }
   break;
   }
 case NumberType.Decimal:
   {
   Decimal value;
   for(int i=0; i<decimalTable.Length; i++){
   value = decimalTable[i];
   writer.WriteStartElement("Table");
   writer.WriteElementString("Number", value.ToString(info));
   for(int j=0; j<elements.Length; j++){
   writer.WriteElementString(elements[j], value.ToString(values[j], info));
   }
   writer.WriteEndElement();
   }
   break;
   }
 case NumberType.Double:
   {
   Double value;
   for(int i=0; i<doubleTable.Length; i++){
   value = doubleTable[i];
   writer.WriteStartElement("Table");
   writer.WriteElementString("Number", value.ToString("R", info));
   for(int j=0; j<elements.Length; j++){
   writer.WriteElementString(elements[j], value.ToString(values[j], info));
   }
   writer.WriteEndElement();
   }
   break;
   }
 }
 writer.WriteEndElement();
 writer.Flush();
 writer.Close();
 }
 private void WriteDataToResourceFile(String fileFormat, ResourceWriter resWriter, CultureInfo culture){
 ArrayList xmlTable;
 ArrayList xmlList;
 Int32 ielementCount;
 String elementName;
 StringBuilder resourceHolder;
 String resourceName;
 XmlTextReader reader;
 xmlTable = new ArrayList();
 xmlList = new ArrayList();
 reader = new XmlTextReader(xmlSchemaFile + "_" + fileFormat + ".xml");
 ielementCount=0;
 while (reader.Read())
   {
   switch (reader.NodeType)
     {
     case XmlNodeType.Element:
       if (reader.HasAttributes)
	 {
	 if(++ielementCount>2){
	 xmlTable.Add(String.Empty);
	 xmlList.Add(reader[0]);
	 }
	 }
       break;
     }
   }
 reader.Close();
 reader = new XmlTextReader(xmlDataFile  + "_" + fileFormat + "_" + culture.ToString() + ".xml");
 elementName = String.Empty;
 while (reader.Read())
   {
   switch (reader.NodeType)
     {
     case XmlNodeType.Element:
       elementName = reader.Name;
       break;
     case XmlNodeType.Text:
       if(xmlList.Contains(elementName)){
       xmlTable[xmlList.IndexOf(elementName)] = (String)xmlTable[xmlList.IndexOf(elementName)] + reader.Value + separator;
       }
       break;
     }
   }
 reader.Close();
 resourceHolder = new StringBuilder();
 foreach(String str111 in xmlList){
 resourceHolder.Append((String)xmlTable[xmlList.IndexOf(str111)] + EOL);
 }
 resourceName = baseFileName + fileFormat + culture.ToString();
 resWriter.AddResource(resourceName, resourceHolder.ToString());
 }
 private void GoDoPictureFormatting(CultureInfo culture, ResourceWriter resWriter)
   {
   NumberFormatInfo numInfo = NumberFormatInfo.GetInstance(culture);
   String[] bogusHeaders = new String[pictureFormats.Length];
   for(int i=0; i<bogusHeaders.Length; i++){
   bogusHeaders[i] = "AA_" + i;
   }
   GenerateXmlSchema(bogusHeaders, strPic);
   StreamWriter xmlwriter = File.CreateText(xmlDataFile  + "_" + strPic + "_" + culture.ToString() + ".xml");
   XmlTextWriter myXmlTextWriter = new XmlTextWriter(xmlwriter);
   myXmlTextWriter.Formatting = Formatting.Indented;
   myXmlTextWriter.WriteStartElement("NewDataSet");
   WriteRealHeaders(myXmlTextWriter, bogusHeaders, pictureFormats);
   {
   Int64 value;
   for(int i=0; i<numberTable.Length; i++){
   value = numberTable[i];
   myXmlTextWriter.WriteStartElement("Table");
   myXmlTextWriter.WriteElementString("Number", value.ToString(numInfo));
   for(int j=0; j<bogusHeaders.Length; j++){
   myXmlTextWriter.WriteElementString(bogusHeaders[j], value.ToString(pictureFormats[j], numInfo));
   }
   myXmlTextWriter.WriteEndElement();
   }
   }
   {
   Decimal value;
   for(int i=0; i<decimalTable.Length; i++){
   value = decimalTable[i];
   myXmlTextWriter.WriteStartElement("Table");
   myXmlTextWriter.WriteElementString("Number", value.ToString(numInfo));
   for(int j=0; j<bogusHeaders.Length; j++){
   myXmlTextWriter.WriteElementString(bogusHeaders[j], value.ToString(pictureFormats[j], numInfo));
   }
   myXmlTextWriter.WriteEndElement();
   }
   }
   {
   Double value;
   for(int i=0; i<doubleTable.Length; i++){
   value = doubleTable[i];
   myXmlTextWriter.WriteStartElement("Table");
   myXmlTextWriter.WriteElementString("Number", value.ToString("R", numInfo));
   for(int j=0; j<bogusHeaders.Length; j++){
   myXmlTextWriter.WriteElementString(bogusHeaders[j], value.ToString(pictureFormats[j], numInfo));
   }
   myXmlTextWriter.WriteEndElement();
   }
   }
   myXmlTextWriter.WriteEndElement();
   myXmlTextWriter.Flush();
   myXmlTextWriter.Close();
   WriteDataToResourceFile(strPic, resWriter, culture);
   }
}
enum NumberType
{
  Int,
  Decimal,
  Double,
}
