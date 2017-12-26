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
    const String fileFormalFormats = "formalFormats";
    const String fileDateTimePatterns = "dateTimePatterns";
    const String fileCustomFormats = "customFormats";
    const String otherCalendarString = "otherCalendar";
    const String xmlSchemaFile = "DateSchema";
    const String xmlDataFile = "DateData";
    const String separator = "^";
    private String EOL = Environment.NewLine;
    const String resourceBaseName = "Co8657FormatTables";
    private String baseFileName = "table";
    private String resCultures = "cultures";
    DateTime[] interestingDates = {
                                      new DateTime(2001, 4, 18, 9, 41, 26, 980),
                                      new DateTime(2001, 10, 8, 19, 5, 3, 1),
                                      new DateTime(2000, 2, 29, 23, 50, 59, 61),
                                      new DateTime(2001, 4, 18),		
    };
    String[] formalFormats = {"d",		
                                 "D",	
                                 "f",	
                                 "F",	
                                 "g",	
                                 "G",	
                                 "M",	
                                 "R",	
                                 "s",	
                                 "t",	
                                 "T",	
                                 "u",	
                                 "U",	
                                 "Y",	
    };
    String[] ourOwnCustomFormats = {
                                       "dddd",
                                       "ddd",
                                       "dd",
                                       "ddd, dd",
                                       "dddd, ddd, dd",
                                       "dddd ddd dd",
                                       "dddd~ddd~dd",
                                       "DD",	
                                       "ddddDDdddDDddD",	
                                       "dddddddddddd",	
                                       "MMMM",
                                       "MMM",
                                       "MM",
                                       "MMMM~MMM~MM",
                                       "MMMMddddMMMdddMMdd",
                                       "yyyy",
                                       "yyy",
                                       "yy",
                                       "yyyyyyyyyy",
                                       "HHHH",
                                       "HHH",
                                       "HH",
                                       "mmmm",
                                       "mmm",
                                       "mm",
                                       "ssss",
                                       "sss",
                                       "ss",
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
                                 new CultureInfo("hi-IN"),
                                 new CultureInfo("es-MX"),	
    };
    private void DoThings()
    {
        String[] bogusHeaders;
        ResourceWriter resWriter;
        String[] dateInfoCustomFormats;
        Hashtable dupTable;
        CultureInfo culture;
        DateTimeFormatInfo dateInfo;
        StreamWriter xmlwriter;
        XmlTextWriter myXmlTextWriter;
        Calendar otherCalendar;
        Calendar gregorian;
        Hashtable calendars;
        String[] tempFormats;
        calendars = new Hashtable();
        calendars.Add(0x0401, new HijriCalendar());
        calendars.Add(0x0404, new TaiwanCalendar());
        calendars.Add(0x040D, new HebrewCalendar());
        calendars.Add(0x0411, new JapaneseCalendar());
        calendars.Add(0x0412, new KoreanCalendar());
        calendars.Add(0x041E, new ThaiBuddhistCalendar());
        otherCalendar = null;
        gregorian = new GregorianCalendar();
        resWriter = new ResourceWriter(resourceBaseName + ".resources");
        resWriter.AddResource(resCultures, cultures);
        WriteXMLForCultures(cultures);
        for(int iCul = 0; iCul<cultures.Length; iCul++)
        {
            culture = cultures[iCul];			
            dateInfo = DateTimeFormatInfo.GetInstance(culture);
            try
            {
                otherCalendar = (Calendar)calendars[culture.LCID];
            }
            catch
            {
            }
            bogusHeaders = new String[formalFormats.Length];
            for(int i=0; i<bogusHeaders.Length; i++)
            {
                bogusHeaders[i] = "AA_" + i;
            }
            if(iCul==0)
            {
                GenerateXmlSchema(bogusHeaders, fileFormalFormats, false, culture);
            }
            if(calendars.ContainsKey(culture.LCID))
            {
                xmlwriter = File.CreateText(xmlDataFile  + "_" + fileFormalFormats + "_" + culture.ToString() + "_" + otherCalendarString + ".xml");
                myXmlTextWriter = new XmlTextWriter(xmlwriter);
                myXmlTextWriter.Formatting = Formatting.Indented;
                myXmlTextWriter.WriteStartElement("NewDataSet");
                tempFormats = new String[formalFormats.Length];
                formalFormats.CopyTo(tempFormats, 0);
                for(int i=0; i<tempFormats.Length; i++)
                {
                    if(Char.IsUpper(tempFormats[i][0]))
                        tempFormats[i] = tempFormats[i] + " ";
                }
                WriteRealHeaders(myXmlTextWriter, bogusHeaders, tempFormats);
                WriteValues(myXmlTextWriter, bogusHeaders, formalFormats, culture, otherCalendar);
                WriteDataToResourceFile(fileFormalFormats, resWriter, culture, false, true);
            }
            xmlwriter = File.CreateText(xmlDataFile  + "_" + fileFormalFormats + "_" + culture.ToString() + ".xml");
            myXmlTextWriter = new XmlTextWriter(xmlwriter);
            myXmlTextWriter.Formatting = Formatting.Indented;
            myXmlTextWriter.WriteStartElement("NewDataSet");
            tempFormats = new String[formalFormats.Length];
            formalFormats.CopyTo(tempFormats, 0);
            for(int i=0; i<tempFormats.Length; i++)
            {
                if(Char.IsUpper(tempFormats[i][0]))
                    tempFormats[i] = tempFormats[i] + " ";
            }
            WriteRealHeaders(myXmlTextWriter, bogusHeaders, tempFormats);
            WriteValues(myXmlTextWriter, bogusHeaders, formalFormats, culture, gregorian);
            WriteDataToResourceFile(fileFormalFormats, resWriter, culture, false, false);
            dateInfoCustomFormats = DateTimeFormatInfo.GetInstance(culture).GetAllDateTimePatterns();
            dupTable = new Hashtable();
            for(int i=0; i<dateInfoCustomFormats.Length; i++)
            {
                try
                {
                    dupTable.Add(dateInfoCustomFormats[i], null);
                }
                catch
                {
                }
            }
            dateInfoCustomFormats = new String[dupTable.Count];
            Int32 iTemp1=0;
            foreach(String strN in dupTable.Keys)
                dateInfoCustomFormats[iTemp1++] = strN;
            bogusHeaders = new String[dateInfoCustomFormats.Length];
            for(int i=0; i<bogusHeaders.Length; i++)
            {
                bogusHeaders[i] = "AA_" + i;
            }
            GenerateXmlSchema(bogusHeaders, fileDateTimePatterns, true, culture);
            if(calendars.ContainsKey(culture.LCID))
            {
                xmlwriter = File.CreateText(xmlDataFile  + "_" + fileDateTimePatterns + "_" + culture.ToString() + "_" + otherCalendarString + ".xml");
                myXmlTextWriter = new XmlTextWriter(xmlwriter);
                myXmlTextWriter.Formatting = Formatting.Indented;
                myXmlTextWriter.WriteStartElement("NewDataSet");
                WriteRealHeaders(myXmlTextWriter, bogusHeaders, dateInfoCustomFormats);
                WriteValues(myXmlTextWriter, bogusHeaders, dateInfoCustomFormats, culture, otherCalendar);
                WriteDataToResourceFile(fileDateTimePatterns, resWriter, culture, true, true);
            }
            xmlwriter = File.CreateText(xmlDataFile  + "_" + fileDateTimePatterns + "_" + culture.ToString() + ".xml");
            myXmlTextWriter = new XmlTextWriter(xmlwriter);
            myXmlTextWriter.Formatting = Formatting.Indented;
            myXmlTextWriter.WriteStartElement("NewDataSet");
            WriteRealHeaders(myXmlTextWriter, bogusHeaders, dateInfoCustomFormats);
            WriteValues(myXmlTextWriter, bogusHeaders, dateInfoCustomFormats, culture, gregorian);
            WriteDataToResourceFile(fileDateTimePatterns, resWriter, culture, true, false);
            bogusHeaders = new String[ourOwnCustomFormats.Length];
            for(int i=0; i<bogusHeaders.Length; i++)
            {
                bogusHeaders[i] = "AA_" + i;
            }
            if(iCul==0)
            {
                GenerateXmlSchema(bogusHeaders, fileCustomFormats, false, culture);
            }
            if(calendars.ContainsKey(culture.LCID))
            {
                xmlwriter = File.CreateText(xmlDataFile  + "_" + fileCustomFormats + "_" + culture.ToString() + "_" + otherCalendarString + ".xml");
                myXmlTextWriter = new XmlTextWriter(xmlwriter);
                myXmlTextWriter.Formatting = Formatting.Indented;
                myXmlTextWriter.WriteStartElement("NewDataSet");
                tempFormats = new String[ourOwnCustomFormats.Length];
                ourOwnCustomFormats.CopyTo(tempFormats, 0);
                for(int i=0; i<tempFormats.Length; i++)
                {
                    if(ourOwnCustomFormats[i]=="DD")
                        tempFormats[i] = tempFormats[i] + " ";
                }
                WriteRealHeaders(myXmlTextWriter, bogusHeaders, tempFormats);
                WriteValues(myXmlTextWriter, bogusHeaders, ourOwnCustomFormats, culture, otherCalendar);
                WriteDataToResourceFile(fileCustomFormats, resWriter, culture, false, true);
            }
            xmlwriter = File.CreateText(xmlDataFile  + "_" + fileCustomFormats + "_" + culture.ToString() + ".xml");
            myXmlTextWriter = new XmlTextWriter(xmlwriter);
            myXmlTextWriter.Formatting = Formatting.Indented;
            myXmlTextWriter.WriteStartElement("NewDataSet");
            tempFormats = new String[ourOwnCustomFormats.Length];
            ourOwnCustomFormats.CopyTo(tempFormats, 0);
            for(int i=0; i<tempFormats.Length; i++)
            {
                if(ourOwnCustomFormats[i]=="DD")
                    tempFormats[i] = tempFormats[i] + " ";
            }
            WriteRealHeaders(myXmlTextWriter, bogusHeaders, tempFormats);
            WriteValues(myXmlTextWriter, bogusHeaders, ourOwnCustomFormats, culture, gregorian);
            WriteDataToResourceFile(fileCustomFormats, resWriter, culture, false, false);
        }
        resWriter.Generate();
        resWriter.Close();
    }
    static void Main()
    {
        new CreateTable().DoThings();
    }	
    void GenerateXmlSchema(String[] formats, String fileFormatType, Boolean useCulture, CultureInfo culture)
    {
        StreamWriter writer;
        if(useCulture)
            writer = File.CreateText(xmlSchemaFile + "_" + fileFormatType + "_" + culture.ToString() + ".xml");		
        else
            writer = File.CreateText(xmlSchemaFile + "_" + fileFormatType + ".xml");		
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
        element.Name = "DateTime";
        element.SchemaTypeName = new XmlQualifiedName("string", "http://www.w3.org/2001/XMLSchema");
        sequence.Items.Add(element);
        for(int i=0; i<formats.Length; i++)
        {
            element = new XmlSchemaElement();
            element.Name = formats[i];
            element.SchemaTypeName = new XmlQualifiedName("string", "http://www.w3.org/2001/XMLSchema");
            sequence.Items.Add(element);
        }
        schema.Compile(new ValidationEventHandler(ValidationCallbackOne));
        schema.Write(writer);
        writer.Close();
    }
    public static void ValidationCallbackOne(object sender, ValidationEventArgs args) 
    {
        Console.WriteLine(args.Message);
    }
    private void WriteRealHeaders(XmlTextWriter writer, String[] elements, String[] values)
    {
        writer.WriteStartElement("Table");
        writer.WriteElementString("DateTime", "DateTime");
        for(int j=0; j<elements.Length; j++)
        {
            writer.WriteElementString(elements[j], values[j]);
        }
        writer.WriteEndElement();
    }
    private void WriteValues(XmlTextWriter writer, String[] elements, String[] values, CultureInfo culture, Calendar calendar)
    {
        DateTime dateValue;
        CultureInfo info = new CultureInfo(culture.LCID);
        info.DateTimeFormat.Calendar = calendar;
        for(int i=0; i<interestingDates.Length; i++)
        {
            dateValue = interestingDates[i];
            writer.WriteStartElement("Table");
            writer.WriteElementString("DateTime", dateValue.ToString(info));
            for(int j=0; j<elements.Length; j++)
            {
                writer.WriteElementString(elements[j], dateValue.ToString(values[j], info));
            }
            writer.WriteEndElement();
        }
        writer.WriteEndElement();
        writer.Flush();
        writer.Close();
    }
    private void WriteDataToResourceFile(String fileFormat, ResourceWriter resWriter, CultureInfo culture, Boolean useCulture, Boolean otherCalendar)
    {
        ArrayList xmlTable;
        ArrayList xmlList;
        Int32 ielementCount;
        String elementName;
        StringBuilder resourceHolder;
        String resourceName;
        XmlTextReader reader;
        xmlTable = new ArrayList();
        xmlList = new ArrayList();
        if(useCulture)
            reader = new XmlTextReader(xmlSchemaFile + "_" + fileFormat + "_" + culture.ToString() + ".xml");
        else
            reader = new XmlTextReader(xmlSchemaFile + "_" + fileFormat + ".xml");
        ielementCount=0;
        while (reader.Read())
        {
            switch (reader.NodeType)
            {
                case XmlNodeType.Element:
                    if (reader.HasAttributes)
                    {
                        if(++ielementCount>2)
                        {
                            xmlTable.Add(String.Empty);
                            xmlList.Add(reader[0]);
                        }
                    }
                    break;
            }
        }
        reader.Close();
        if(otherCalendar)
            reader = new XmlTextReader(xmlDataFile  + "_" + fileFormat + "_" + culture.ToString() + "_" + otherCalendarString + ".xml");
        else
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
                    if(xmlList.Contains(elementName))
                    {
                        xmlTable[xmlList.IndexOf(elementName)] = (String)xmlTable[xmlList.IndexOf(elementName)] + reader.Value + separator;
                    }
                    break;
            }
        }
        reader.Close();
        resourceHolder = new StringBuilder();
        foreach(String str111 in xmlList)
        {
            resourceHolder.Append((String)xmlTable[xmlList.IndexOf(str111)] + EOL);
        }
        resourceName = baseFileName + fileFormat + culture.ToString() + otherCalendar.ToString();
        resWriter.AddResource(resourceName, resourceHolder.ToString());
    }
    private void WriteXMLForCultures(CultureInfo[] cultures)
    {
        StreamWriter writer = File.CreateText("CultureSchema.xml");		
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
        element.Name = "CultureName";
        element.SchemaTypeName = new XmlQualifiedName("string", "http://www.w3.org/2001/XMLSchema");
        sequence.Items.Add(element);
        schema.Compile(new ValidationEventHandler(ValidationCallbackOne));
        schema.Write(writer);
        writer.Close();     
        writer = File.CreateText("CultureData.xml");
        XmlTextWriter myXmlTextWriter = new XmlTextWriter(writer);
        myXmlTextWriter.Formatting = Formatting.Indented;
        myXmlTextWriter.WriteStartElement("NewDataSet");
        for(int j=0; j<cultures.Length; j++)
        {
            myXmlTextWriter.WriteStartElement("Table");
            myXmlTextWriter.WriteElementString("CultureName", cultures[j].LCID.ToString());
            myXmlTextWriter.WriteEndElement();
        }
        myXmlTextWriter.WriteEndElement();
        myXmlTextWriter.Flush();
        myXmlTextWriter.Close();
    }
}
