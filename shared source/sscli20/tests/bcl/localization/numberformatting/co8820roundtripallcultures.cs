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
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Collections;
public class Co8820RoundTripAllCultures
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "NumberFormatting - RoundTrip in all cultures";
 public static String s_strTFName        = "Co8820RoundTripAllCultures.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
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
   String s;
   NumberStyles styles;
   Boolean parse;
   Int64 lValue;
   Decimal dcmValue;
   Double dValue;
   try
     {
     strLoc = "Loc_137sdgd";
     iCountTestcases++;
     foreach(CultureInfo culture in CultureInfo.GetCultures(CultureTypes.AllCultures)){
     if(culture.IsNeutralCulture)
       continue;
     foreach(Int64 value in numberTable){
     foreach(String format in numericalFormats){
     styles = GetNumberStyle(format[0], out parse);
     s = value.ToString(format, culture);
     if(parse){
     try{
     lValue = Int64.Parse(s, styles, culture);
     if(lValue != value){
     if(!AreTheseNumbersEqual(lValue, value, format, culture)){
     iCountErrors++;
     Console.WriteLine("Err_3947sg! Wrong value returned. Returned: <{0}>, Expected: <{1}>", lValue, value);
     }
     }
     }catch(OverflowException){
     }catch(Exception e){
     iCountErrors++;
     Console.WriteLine("Err_3248gsf! unexpected exception thrown, Culture: {0}, String: {1}, Format: {3}, ExceptionType: {2}", culture, s, e.GetType().Name, format);
     }
     }
     }
     }
     foreach(Decimal value in decimalTable){
     foreach(String format in numericalFormats){
     if(format[0] == 'D' || format[0] == 'X')
       continue;
     styles = GetNumberStyle(format[0], out parse);
     s = value.ToString(format, culture);
     if(parse){
     try{
     dcmValue = Decimal.Parse(s, styles, culture);
     if(dcmValue != value){
     if(!AreTheseNumbersEqual((double)dcmValue, (double)value, format, culture)){
     iCountErrors++;
     Console.WriteLine("Err_3067sfg! Wrong value returned. Returned: <{0}>, Expected: <{1}>", dcmValue, value);
     }
     }
     }catch(OverflowException){
     }catch(Exception e){
     iCountErrors++;
     Console.WriteLine("Err_3248gsf! unexpected exception thrown, Culture: {0}, String: {1}, Format: {3}, ExceptionType: {2}", culture, s, e.GetType().Name, format);
     }
     }
     }
     }
     foreach(Double value in doubleTable){
     foreach(String format in numericalFormats){
     if(format[0] == 'D' || format[0] == 'X')
       continue;
     styles = GetNumberStyle(format[0], out parse);
     s = value.ToString(format, culture);
     if(parse){
     try{
     dValue = Double.Parse(s, styles, culture);
     if(dValue != value){
     if(!AreTheseNumbersEqual(dValue, value, format, culture)){
     iCountErrors++;
     Console.WriteLine("Err_347g! Wrong value returned. Returned: <{0}>, Expected: <{1}>, String: {2}, Culture: {3}, Format: {4}", dValue, value, s, culture, format);
     }
     }
     }catch(OverflowException){
     }catch(Exception e){
     iCountErrors++;
     Console.WriteLine("Err_3248gsf! unexpected exception thrown, Culture: {0}, String: {1}, Format: {3}, ExceptionType: {2}", culture, s, e.GetType().Name, format);
     }
     }
     }
     }
     }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8820RoundTripAllCultures cbA = new Co8820RoundTripAllCultures();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
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
   if((culture.LCID==0x0411 || culture.LCID==0x0412 || culture.LCID==0x043E
       || culture.LCID==0x0443 || culture.LCID==0x083E || culture.LCID==0x421
       )
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
}
