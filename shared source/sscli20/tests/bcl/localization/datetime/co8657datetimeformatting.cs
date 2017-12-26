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
using System.Text;
using System.Threading;
using System.Globalization;
using System.Resources;
using System.Collections;
class CreateTable
{
 const String resourceBaseName = "Co8657DateTimeFormatting";
 const String fileFormalFormats = "formalFormats";
 const String fileDateTimePatterns = "dateTimePatterns";
 const String fileCustomFormats = "customFormats";
 private Boolean verbose = false;
 const String separator = "^";
 private String baseFileName = "table";
 private String resCultures = "cultures";
 DateTime[] dateTable;
 private Boolean DoThings()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;		
   String temp;
   String line;
   StringReader table;
   String fileName;
   String[] values;
   Boolean firstLine;
   Boolean pass;
   String format;
   String[] expectedValues;
   CultureInfo[] cultures;
   ResourceManager manager;
   Calendar otherCalendar;
   Calendar gregorian;
   Hashtable calendars;
   calendars = new Hashtable();
   calendars.Add(0x0401, new HijriCalendar());
   calendars.Add(0x0404, new TaiwanCalendar());
   calendars.Add(0x040D, new HebrewCalendar());
   calendars.Add(0x0411, new JapaneseCalendar());
   calendars.Add(0x0412, new KoreanCalendar());
   calendars.Add(0x041E, new ThaiBuddhistCalendar());
   otherCalendar = null;
   gregorian = new GregorianCalendar();
   pass = true;
   try{
   manager = new ResourceManager(resourceBaseName, this.GetType().Assembly, null);
   }catch(Exception){
   throw new Exception("ResourceFileNotFound");
   }
   if(manager == null)
     throw new Exception("ResourceFileNotFound");
   iCountTestcases++;
   cultures = (CultureInfo[])manager.GetObject(resCultures); 
   foreach(CultureInfo culture in cultures){
   if(calendars.ContainsKey(culture.LCID))
     otherCalendar = (Calendar)calendars[culture.LCID];
   fileName = baseFileName + fileFormalFormats + culture.ToString() + false.ToString();
   if(verbose)
     Console.WriteLine("Processing formal Formatting with Gregorian, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
   culture.DateTimeFormat.Calendar = gregorian;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.ParseExact(values[i+1], "G", culture);
   }
   firstLine = true;
   }
   else{
   format = values[0].Trim();
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_#45232! Date: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }																		
   }
   }
   }
   table.Close();
   fileName = baseFileName + fileFormalFormats + culture.ToString() + true.ToString();
   temp = manager.GetString(fileName); 
   if(temp != null){
   if(culture.LCID == 0x040D)
     break;
   if(verbose)
     Console.WriteLine("Processing formal Formatting with other otherCalendar, FileName - <{0}>", fileName);
   culture.DateTimeFormat.Calendar = otherCalendar;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.ParseExact(values[i+1], "G", culture);
   }
   firstLine = true;
   }
   else{
   format = values[0].Trim();
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_20735rd! Date: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }
   }
   }
   }
   table.Close();
   }
   fileName = baseFileName + fileDateTimePatterns + culture.ToString() + false.ToString();
   if(verbose)
     Console.WriteLine("Processing patterned Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
   culture.DateTimeFormat.Calendar = gregorian;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   if(line.Trim()==String.Empty)
     continue;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.Parse(values[i+1], culture);
   }
   firstLine = true;
   }
   else{
   format = values[0];
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_385sdg! DateTime: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }
   }
   }
   }
   table.Close();
   fileName = baseFileName + fileDateTimePatterns + culture.ToString() + true.ToString();
   temp = manager.GetString(fileName); 
   if(temp != null){
   if(culture.LCID == 0x040D)
     break;
   if(verbose)
     Console.WriteLine("Processing formal Formatting with other otherCalendar, FileName - <{0}>", fileName);
   culture.DateTimeFormat.Calendar = otherCalendar;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.ParseExact(values[i+1], "G", culture);
   }
   firstLine = true;
   }
   else{
   format = values[0];
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_3275sg! Date: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }
   }
   }
   }
   table.Close();
   }
   fileName = baseFileName + fileCustomFormats + culture.ToString() + false.ToString();
   if(verbose)
     Console.WriteLine("Processing custom Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
   culture.DateTimeFormat.Calendar = gregorian;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.ParseExact(values[i+1], "G", culture);
   }
   firstLine = true;
   }
   else{
   format = values[0].Trim();
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_02375sdg! DateTime: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }
   }
   }
   }
   table.Close();		
   fileName = baseFileName + fileCustomFormats + culture.ToString() + true.ToString();
   temp = manager.GetString(fileName); 
   if(temp != null){
   if(culture.LCID == 0x040D)
     break;
   if(verbose)
     Console.WriteLine("Processing formal Formatting with other otherCalendar, FileName - <{0}>", fileName);
   culture.DateTimeFormat.Calendar = otherCalendar;
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   values = line.Split(new Char[]{separator[0]});
   if(!firstLine){
   if(values[values.Length-1] == String.Empty)
     dateTable = new DateTime[values.Length-2];
   else
     dateTable = new DateTime[values.Length-1];
   for(int i=0; i<dateTable.Length; i++){
   dateTable[i] = DateTime.ParseExact(values[i+1], "G", culture);
   }
   firstLine = true;
   }
   else{
   format = values[0].Trim();
   if(values[values.Length-1] == String.Empty)
     expectedValues = new String[values.Length-2];
   else
     expectedValues = new String[values.Length-1];
   for(int i=0; i<expectedValues.Length; i++)
     expectedValues[i] = values[i+1];
   for(int i=0;i<dateTable.Length; i++){
   if(verbose)
     Console.WriteLine("Date: <{0}>, Format: <{1}>, Expected: <{2}>", dateTable[i], format, expectedValues[i]);
   if(!dateTable[i].ToString(format, culture).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_3275sg! Date: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", dateTable[i], format, expectedValues[i], dateTable[i].ToString(format, culture));
   }
   }
   }
   }
   table.Close();
   }
   }
   return pass;		
   }
 static void Main()
   {
   try{
   if(new CreateTable().DoThings()){
   Console.WriteLine("pass");
   Environment.ExitCode = 0;
   }
   else{
   Console.WriteLine("fail");
   Environment.ExitCode = 1;
   }
   }catch(Exception ex){
   Console.WriteLine("fail, " + ex);
   Environment.ExitCode = 1;
   }
   }	
}
