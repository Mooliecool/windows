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
 const String strInt = "Integer";
 const String strDec = "Decimal";
 const String strDbl = "Double";
 const String strPic = "Picture";
 const String resourceBaseName = "Co8643numberformat";
 private Boolean verbose = false;		
 const String separator = "~";
 String EOL = Environment.NewLine;
 private String baseFileName = "table";
 private String resCultures = "cultures";
 Int64[] numberTable;	
 Decimal[] decimalTable;							
 Double[] doubleTable;
 private Boolean DoThings()
   {
   Int64 numberValue;
   Int32 i32Value;
   Int32 i32Value_1;
   Int16 i16Value;
   Int16 i16Value_1;
   SByte i8Value;
   SByte i8Value_1;
   Byte ui8Value;
   Byte ui8Value_1;
   UInt16 ui16Value;
   UInt16 ui16Value_1;
   UInt32 ui32Value;
   UInt32 ui32Value_1;
   UInt64 ui64Value;
   UInt64 ui64Value_1;
   Decimal decimalValue;
   Double doubleValue;
   String temp;
   String line;
   StringReader table;
   String fileName;
   String[] values;
   Boolean firstLine;
   Boolean pass;
   Boolean parse;
   String format;
   String[] expectedValues;
   String expectedValue;
   CultureInfo[] cultures;
   NumberFormatInfo numInfo;
   ResourceManager manager;
   NumberStyles styles = NumberStyles.Number;
   pass = true;
   try{
   manager = new ResourceManager(resourceBaseName, this.GetType().Assembly, null);
   }catch(Exception){
   throw new Exception("ResourceFileNotFound");
   }
   if(manager == null)
     throw new Exception("ResourceFileNotFound");
   cultures = (CultureInfo[])manager.GetObject(resCultures); 
   foreach(CultureInfo culture in cultures){
   if(verbose)
     Console.WriteLine("Looking at culture, <{0}>", culture);
   numInfo = NumberFormatInfo.GetInstance(culture);
   fileName = baseFileName + strInt + culture.ToString();
   if(verbose)
     Console.WriteLine("Processing Integer Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
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
     numberTable = new Int64[values.Length-2];
   else
     numberTable = new Int64[values.Length-1];
   for(int i=0; i<numberTable.Length; i++){
   numberTable[i] = Int64.Parse(values[i+1], numInfo);
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
   styles = GetNumberStyle(format[0], out parse);
   for(int i=0;i<numberTable.Length; i++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", numberTable[i], format, expectedValues[i]);
   if(!numberTable[i].ToString(format, numInfo).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_#45232! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", numberTable[i], format, expectedValues[i], numberTable[i].ToString(format, numInfo));
   }
   if(parse){
   try{
   numberValue = Int64.Parse(expectedValues[i], styles, numInfo);
   if(numberValue != numberTable[i]){
   if(!AreTheseNumbersEqual(numberValue, numberTable[i], format, culture)){
   pass = false;
   Console.WriteLine("Err_32985sdg! Expected: {0}, Returned: {1}", numberTable[i], numberValue);
   }
   }
   }catch(OverflowException){
   }
   }
   try{
   i32Value = Convert.ToInt32(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.StartsWith("X")){
   if(expectedValues[i].Length>8)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-8), 8);
   }
   if(!i32Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_3497sfdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", i32Value, format, expectedValue, i32Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   i32Value_1 = Int32.Parse(expectedValue, styles, numInfo);
   if(i32Value_1 != i32Value){
   if(!AreTheseNumbersEqual(i32Value_1, i32Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_89345trsdgf! Expected: {0}, Returned: {1}", i32Value, i32Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   i16Value = Convert.ToInt16(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.StartsWith("X")){
   if(expectedValues[i].Length>4)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-4), 4);
   }
   if(!i16Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_3846tsdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", i16Value, format, expectedValue, i16Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   i16Value_1 = Int16.Parse(expectedValue, styles, numInfo);
   if(i16Value_1 != i16Value){
   if(!AreTheseNumbersEqual(i16Value_1, i16Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_89345trsdgf! Expected: {0}, Returned: {1}", i16Value, i16Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   i8Value = Convert.ToSByte(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.Equals("X")){
   if(expectedValues[i].Length>2)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-2), 2);
   }
   if(format.Equals("X4")){
   if(expectedValues[i].Length>4)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-4), 4);
   if(expectedValue.Length==4 && expectedValue.StartsWith("FF"))
     expectedValue = "00" + expectedValue.Substring(2);
   }
   if(!i8Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_2407tfsg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", i8Value, format, expectedValue, i8Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   i8Value_1 = SByte.Parse(expectedValue, styles, numInfo);
   if(i8Value_1 != i8Value){
   if(!AreTheseNumbersEqual(i8Value_1, i8Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_83745sg! Expected: {0}, Returned: {1}", i8Value, i8Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   ui64Value = Convert.ToUInt32(numberTable[i]);
   expectedValue = expectedValues[i];
   if(!ui64Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_93476g! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", ui64Value, format, expectedValue, ui64Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   ui64Value_1 = UInt64.Parse(expectedValue, styles, numInfo);
   if(ui64Value_1 != ui64Value){
   if(!AreTheseNumbersEqual(ui64Value_1, ui64Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_83745sg! Expected: {0}, Returned: {1}", ui64Value, ui64Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   ui32Value = Convert.ToUInt32(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.StartsWith("X")){
   if(expectedValues[i].Length>8)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-8), 8);
   }
   if(!ui32Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_239467tsdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", ui32Value, format, expectedValue, ui32Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   ui32Value_1 = UInt32.Parse(expectedValue, styles, numInfo);
   if(ui32Value_1 != ui32Value){
   if(!AreTheseNumbersEqual(ui32Value_1, ui32Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_83745sg! Expected: {0}, Returned: {1}", ui32Value, ui32Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   ui16Value = Convert.ToUInt16(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.StartsWith("X")){
   if(expectedValues[i].Length>4)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-4), 4);
   }
   if(!ui16Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_90736tsdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", ui16Value, format, expectedValue, ui16Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   ui16Value_1 = UInt16.Parse(expectedValue, styles, numInfo);
   if(ui16Value_1 != ui16Value){
   if(!AreTheseNumbersEqual(ui16Value_1, ui16Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_83745sg! Expected: {0}, Returned: {1}", ui16Value, ui16Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   try{
   ui8Value = Convert.ToByte(numberTable[i]);
   expectedValue = expectedValues[i];
   if(format.Equals("X")){
   if(expectedValues[i].Length>2)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-2), 2);
   }
   if(format.Equals("X4")){
   if(expectedValues[i].Length>4)
     expectedValue = expectedValues[i].Substring((expectedValues[i].Length-4), 4);
   }
   if(!ui8Value.ToString(format, numInfo).Equals(expectedValue)){
   pass = false;
   Console.WriteLine("Err_9823tsdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", ui8Value, format, expectedValue, ui8Value.ToString(format, numInfo));
   }
   if(parse){
   try{
   ui8Value_1 = Byte.Parse(expectedValue, styles, numInfo);
   if(ui8Value_1 != ui8Value){
   if(!AreTheseNumbersEqual(ui8Value_1, ui8Value, format, culture)){
   pass = false;
   Console.WriteLine("Err_83745sg! Expected: {0}, Returned: {1}", ui8Value, ui8Value_1);
   }
   }
   }catch(OverflowException){
   }
   }						
   }catch(OverflowException){
   }
   }
   }
   }
   table.Close();
   fileName = baseFileName + strDec + culture.ToString();
   if(verbose)
     Console.WriteLine("Processing Decimal Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
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
     decimalTable = new Decimal[values.Length-2];
   else
     decimalTable = new Decimal[values.Length-1];
   for(int i=0; i<decimalTable.Length; i++){
   decimalTable[i] = Decimal.Parse(values[i+1], numInfo);
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
   styles = GetNumberStyle(format[0], out parse);
   for(int i=0;i<decimalTable.Length; i++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", decimalTable[i], format, expectedValues[i]);
   if(!decimalTable[i].ToString(format, numInfo).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_9756erg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", decimalTable[i], format, expectedValues[i], numberTable[i].ToString(format, numInfo));
   }
   if(parse){
   try{
   decimalValue = Decimal.Parse(expectedValues[i], styles, numInfo);
   if(decimalValue != decimalTable[i]){
   if(!AreTheseNumbersEqual((double)decimalValue, (double)decimalTable[i], format, culture)){
   pass = false;
   Console.WriteLine("Err_83475sgd! Expected: {0}, Returned: {1}, String: {2}", decimalValue, decimalTable[i], expectedValues[i]);
   }
   }
   }catch(OverflowException){
   }
   }
   }
   }
   }
   table.Close();
   fileName = baseFileName + strDbl + culture.ToString();
   if(verbose)
     Console.WriteLine("Processing Double Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
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
     doubleTable = new Double[values.Length-2];
   else
     doubleTable = new Double[values.Length-1];
   for(int i=0; i<doubleTable.Length; i++){
   doubleTable[i] = Double.Parse(values[i+1], numInfo);
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
   styles = GetNumberStyle(format[0], out parse);
   for(int i=0;i<doubleTable.Length; i++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", doubleTable[i], format, expectedValues[i]);
   if(!doubleTable[i].ToString(format, numInfo).Equals(expectedValues[i])){
   pass = false;
   Console.WriteLine("Err_87345sdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", doubleTable[i], format, expectedValues[i], numberTable[i].ToString(format, numInfo));
   }
   if(parse){
   try{
   doubleValue = Double.Parse(expectedValues[i], styles, numInfo);
   if(doubleValue != doubleTable[i]){
   if(!AreTheseNumbersEqual(doubleValue, doubleTable[i], format, culture)){
   pass = false;
   Console.WriteLine("Err_32985sdg! Expected: {0}, Returned: {1}, String: {2}", doubleTable[i], doubleValue, expectedValues[i]);
   }
   }
   }catch(OverflowException){
   }
   }
   }
   }
   }
   table.Close();
   fileName = baseFileName + strPic + culture.ToString();
   if(verbose)
     Console.WriteLine("Processing Picture Formatting, FileName - <{0}>", fileName);
   temp = manager.GetString(fileName); 
   if(temp == null)
     throw new Exception(String.Format("Formatting table for {0} locale not found", culture.ToString()));
   table = new StringReader(temp);
   firstLine = false;
   while(true){
   line = table.ReadLine();
   if(line==null)
     break;
   if(line.Trim()==String.Empty)
     continue;
   values = line.Split(new Char[]{separator[0]});
   Int32 picNumbers;
   if(!firstLine){
   ArrayList numbersCol = new ArrayList();
   ArrayList decimalCol = new ArrayList();
   ArrayList doubleCol = new ArrayList();
   for(picNumbers=1; picNumbers<values.Length; picNumbers++){
   try{
   numbersCol.Add(Int64.Parse(values[picNumbers], numInfo));
   }catch(Exception){
   break;
   }
   }
   for(; picNumbers<values.Length; picNumbers++){
   try{
   decimalCol.Add(Decimal.Parse(values[picNumbers], numInfo));
   }catch(Exception){
   break;
   }
   }
   for(; picNumbers<values.Length; picNumbers++){
   try{
   doubleCol.Add(Double.Parse(values[picNumbers], numInfo));
   }catch(Exception){
   break;
   }
   }
   numberTable = new Int64[numbersCol.Count];
   for(int i=0; i<numberTable.Length; i++){
   numberTable[i] = (Int64)numbersCol[i];
   }
   decimalTable = new Decimal[decimalCol.Count];
   for(int i=0; i<decimalTable.Length; i++){
   decimalTable[i] = (Decimal)decimalCol[i];
   }
   doubleTable = new Double[doubleCol.Count];
   for(int i=0; i<doubleTable.Length; i++){
   doubleTable[i] = (Double)doubleCol[i];
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
   picNumbers = 0;
   for(int i=0;i<numberTable.Length; i++, picNumbers++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", numberTable[i], format, expectedValues[picNumbers]);
   if(!numberTable[i].ToString(format, numInfo).Equals(expectedValues[picNumbers])){
   pass = false;
   Console.WriteLine("Err_87345sdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", numberTable[i], format, expectedValues[i], numberTable[picNumbers].ToString(format, numInfo));
   }
   }
   for(int i=0;i<decimalTable.Length; i++, picNumbers++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", decimalTable[i], format, expectedValues[picNumbers]);
   if(!decimalTable[i].ToString(format, numInfo).Equals(expectedValues[picNumbers])){
   pass = false;
   Console.WriteLine("Err_87345sdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", decimalTable[i], format, expectedValues[i], decimalTable[picNumbers].ToString(format, numInfo));
   }
   }
   for(int i=0;i<doubleTable.Length; i++, picNumbers++){
   if(verbose)
     Console.WriteLine("Number: <{0}>, Format: <{1}>, Expected: <{2}>", doubleTable[i], format, expectedValues[picNumbers]);
   if(!doubleTable[i].ToString(format, numInfo).Equals(expectedValues[picNumbers])){
   pass = false;
   Console.WriteLine("Err_87345sdg! Number: <{0}>, Format: <{1}>, Expected: <{2}>, Returned: <{3}>", doubleTable[i], format, expectedValues[i], doubleTable[picNumbers].ToString(format, numInfo));
   }
   }				
   }
   }
   table.Close();
   }
   return pass;		
   }
 private void WriteToFile(TextWriter writer, NumberType numType)
   {
   }
 private Boolean AreTheseNumbersEqual(double val1, double val2, String format, CultureInfo culture)
   {
   if(Double.IsNaN(val1)){
   if(Double.IsNaN(val2))
     return true;
   else
     return false;
   }
   Double threshold = 0.03;
   if(val1==0.0 || val2==0.0){
   if((culture.LCID==0x0411 || culture.LCID==0x0412)
      && format.Equals("C")
      )
     return true;
   if((val1<threshold) && (val2<threshold))
     return true;
   else
     return false;
   }
   Double result = val1/val2;
   if(result>1.0)
     result = result-1.0;
   else
     result = 1.0-result;
   if(result < threshold)
     return true;
   else{
   Console.WriteLine("value1: {0} value2: {1}, Result: {2}", val1, val2, result);
   return false;
   }
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
   Console.WriteLine("fail, " + ex.Message + " " + ex.GetType().Name);
   Environment.ExitCode = 1;
   }
   }
 private void GoDoPictureFormatting(CultureInfo culture)
   {
   Int64 numberValue;
   Decimal decimalValue;
   Double doubleValue;
   String fileName = baseFileName + "_" + "Picture" + "_" + culture.ToString() + ".txt";
   FileStream stream = new FileStream(fileName, FileMode.Create, FileAccess.Write);
   StreamWriter writer = new StreamWriter(stream);
   String s_pic_1 = "##00";
   String s_pic_2 = "#,0#";
   String s_pic_3 = "%#,0#";
   String s_pic_4 = "%#,0#,,,";
   String s_pic_5 = "#.0#";
   String s_pic_6 = "#,.0#";
   String s_pic_7 = "#..0.#";
   String s_pic_8 = "00.00E+0";
   String s_pic_9 = "00.00e-0";
   String s_pic_10 = "00.00e0";
   String s_pic_11 = "0.0e+0;-00;%0";
   StringBuilder numbers = new StringBuilder("Number" + separator);
   StringBuilder pic_1 = new StringBuilder(s_pic_1 + separator);
   StringBuilder pic_2 = new StringBuilder(s_pic_2 + separator);
   StringBuilder pic_3 = new StringBuilder(s_pic_3 + separator);
   StringBuilder pic_4 = new StringBuilder(s_pic_4 + separator);
   StringBuilder pic_5 = new StringBuilder(s_pic_5 + separator);
   StringBuilder pic_6 = new StringBuilder(s_pic_6 + separator);
   StringBuilder pic_7 = new StringBuilder(s_pic_7 + separator);
   StringBuilder pic_8 = new StringBuilder(s_pic_8 + separator);
   StringBuilder pic_9 = new StringBuilder(s_pic_9 + separator);
   StringBuilder pic_10 = new StringBuilder(s_pic_10 + separator);
   StringBuilder pic_11 = new StringBuilder(s_pic_11 + separator);
   for(int i=0; i<numberTable.Length; i++){
   numberValue = numberTable[i];
   numbers.Append(numberValue.ToString() + separator);
   pic_1.Append(numberValue.ToString(s_pic_1, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_2.Append(numberValue.ToString(s_pic_2, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_3.Append(numberValue.ToString(s_pic_3, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_4.Append(numberValue.ToString(s_pic_4, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_5.Append(numberValue.ToString(s_pic_5, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_6.Append(numberValue.ToString(s_pic_6, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_7.Append(numberValue.ToString(s_pic_7, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_8.Append(numberValue.ToString(s_pic_8, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_9.Append(numberValue.ToString(s_pic_9, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_10.Append(numberValue.ToString(s_pic_10, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_11.Append(numberValue.ToString(s_pic_11, NumberFormatInfo.GetInstance(culture)) + separator);
   }
   for(int i=0; i<decimalTable.Length; i++){
   decimalValue = decimalTable[i];
   numbers.Append(decimalValue.ToString() + separator);
   pic_1.Append(decimalValue.ToString(s_pic_1, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_2.Append(decimalValue.ToString(s_pic_2, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_3.Append(decimalValue.ToString(s_pic_3, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_4.Append(decimalValue.ToString(s_pic_4, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_5.Append(decimalValue.ToString(s_pic_5, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_6.Append(decimalValue.ToString(s_pic_6, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_7.Append(decimalValue.ToString(s_pic_7, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_8.Append(decimalValue.ToString(s_pic_8, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_9.Append(decimalValue.ToString(s_pic_9, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_10.Append(decimalValue.ToString(s_pic_10, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_11.Append(decimalValue.ToString(s_pic_11, NumberFormatInfo.GetInstance(culture)) + separator);
   }
   for(int i=0; i<doubleTable.Length; i++){
   if(i==0 || i==7)
     continue;
   doubleValue = doubleTable[i];
   numbers.Append(doubleValue.ToString() + separator);
   pic_1.Append(doubleValue.ToString(s_pic_1, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_2.Append(doubleValue.ToString(s_pic_2, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_3.Append(doubleValue.ToString(s_pic_3, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_4.Append(doubleValue.ToString(s_pic_4, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_5.Append(doubleValue.ToString(s_pic_5, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_6.Append(doubleValue.ToString(s_pic_6, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_7.Append(doubleValue.ToString(s_pic_7, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_8.Append(doubleValue.ToString(s_pic_8, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_9.Append(doubleValue.ToString(s_pic_9, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_10.Append(doubleValue.ToString(s_pic_10, NumberFormatInfo.GetInstance(culture)) + separator);
   pic_11.Append(doubleValue.ToString(s_pic_11, NumberFormatInfo.GetInstance(culture)) + separator);
   }
   numbers.Append(EOL);
   pic_1.Append(EOL);
   pic_2.Append(EOL);
   pic_3.Append(EOL);
   pic_4.Append(EOL);
   pic_5.Append(EOL);
   pic_6.Append(EOL);
   pic_7.Append(EOL);
   pic_8.Append(EOL);
   pic_9.Append(EOL);
   pic_10.Append(EOL);
   pic_11.Append(EOL);
   writer.Write(numbers.ToString());
   writer.Write(pic_1.ToString());
   writer.Write(pic_2.ToString());
   writer.Write(pic_3.ToString());
   writer.Write(pic_4.ToString());
   writer.Write(pic_5.ToString());
   writer.Write(pic_6.ToString());
   writer.Write(pic_7.ToString());
   writer.Write(pic_8.ToString());
   writer.Write(pic_9.ToString());
   writer.Write(pic_10.ToString());
   writer.Write(pic_11.ToString());
   writer.Close();
   }
 private NumberStyles GetNumberStyle(Char value, out Boolean parseThis)
   {
   NumberStyles styles = NumberStyles.Number;
   parseThis = true;
   switch(value){
   case 'C':
     styles = NumberStyles.Currency;
     break;
   case 'D':
   case 'N':
     styles = NumberStyles.Number;
     break;
   case 'E':
   case 'G':
     styles = NumberStyles.Float;
     break;
   case 'P':
     parseThis = false;
     break;
   case 'X':
     styles = NumberStyles.HexNumber;
     break;
   }
   return styles;
   }
}
enum NumberType
{
  Integer,
  Decimal,
  Double
}
