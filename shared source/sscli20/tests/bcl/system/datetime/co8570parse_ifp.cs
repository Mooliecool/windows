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
using System.Collections;
using System.Globalization;
using System.IO;
public class Co8570Parse_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.Parse(String s, IFormatProvider provider)";
 public static String s_strTFName        = "Co8570Parse_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime date;
   DateTime dateReturned;
   DateTime dateExpected;
   CultureInfo[] cultures;
   DateTimeFormatInfo info;
   String s;
   Random random;
   Hashtable table;
   Char[] splittableChars = {' ', '/', ':'};
   String[] formats = {"d", "D", "f", "F", "g", "G", "m", "M", "s", "t", "T", "y", "Y"};
   try
     {
     strLoc = "Loc_384sdg";
     date = new DateTime(2001, 4, 5, 21, 3, 4, 980);
     table = new Hashtable();
     table.Add("d", new DateTime(date.Year, date.Month, date.Day));
     table.Add("D", new DateTime(date.Year, date.Month, date.Day));
     table.Add("f", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
     table.Add("F", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("g", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
     table.Add("G", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("m", new DateTime(date.Year, date.Month, date.Day));
     table.Add("M", new DateTime(date.Year, date.Month, date.Day));
     table.Add("s", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("t", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
     table.Add("T", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("u", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("U", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
     table.Add("y", new DateTime(date.Year, date.Month, date.Day));
     table.Add("Y", new DateTime(date.Year, date.Month, date.Day));
     random = new Random();
     cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
     foreach(CultureInfo culture in cultures){
     strLoc = "Loc_37tsdg";
     if(culture.IsNeutralCulture)
       continue;
     if(culture.Name.Equals("div-MV")
	|| culture.Name.Equals("af-ZA")
	)
       continue;
     if(culture.Name.StartsWith("ar-")
	)
       continue;
     iCountTestcases++;
     info = culture.DateTimeFormat;
     foreach(String format in formats){
     strLoc = "Loc_386tsg";
     s = date.ToString(format, info);
     strLoc = "Loc_3947tsg";
     dateExpected = (DateTime)table[format];
     try{
     strLoc = "Loc_9374tsdg!";
     dateReturned = DateTime.Parse(s, info);
     if(!ResultCorrect(dateReturned, dateExpected, format, culture)){
     iCountErrors++;
     Console.WriteLine("Err_93745sdg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
     }
     }catch(Exception ex){
     if((culture.Name.Equals("vi-VN") || (culture.Name=="sq-AL") || (culture.Name=="th-TH")  || (culture.Name=="mn-MN"))
	&& (Char.ToUpper(format[0]).Equals('Y') || Char.ToUpper(format[0]).Equals('R'))
	){
     ex=null;
     }else{
     iCountErrors++;
     Console.WriteLine("Err_75639rge! Unexpected exception thrown {0}, Loc: {3}, format: {4}, Culture: {1}, s:<{2}>", ex.GetType().Name, culture, s, strLoc, format);
     }
     }
     }
     }
     strLoc = "Loc_3947sdg!";
     iCountTestcases++;
     strValue = @"1753-01-01T00:00:00.100";
     dateReturned = DateTime.Parse(strValue, DateTimeFormatInfo.InvariantInfo);
     date = new DateTime(1753, 1, 1, 0, 0, 0, 100);
     if(dateReturned!=date){
     iCountErrors++;
     Console.WriteLine("Err_9347gs! wrong value returned, <{0}>, <{1}>", dateReturned, date);
     }
     strValue = @"10:10:10.100";
     dateReturned = DateTime.Parse(strValue, DateTimeFormatInfo.InvariantInfo);
     date = DateTime.Now;
     date = new DateTime(date.Year, date.Month, date.Day, 10, 10, 10, 100);
     if(dateReturned!=date){
     iCountErrors++;
     Console.WriteLine("Err_9347gs! wrong value returned, <{0}>, <{1}>", dateReturned, date);
     }
     strValue = @"2001, 9, 25, 10:10:10.100";
     dateReturned = DateTime.Parse(strValue, DateTimeFormatInfo.InvariantInfo);
     date = DateTime.Now;
     date = new DateTime(2001, 9, 25, 10, 10, 10, 100);
     if(dateReturned!=date){
     iCountErrors++;
     Console.WriteLine("Err_9347gs! wrong value returned, <{0}>, <{1}>", dateReturned, date);
     }
     strValue = @"10:10:10.99999";
     try{
     dateReturned = DateTime.Parse(strValue, DateTimeFormatInfo.InvariantInfo);
     }catch(Exception){
     iCountErrors++;
     Console.WriteLine("Err_3457sdg! Unexpected exception thrown, see  resolution");				
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
 private Boolean ResultCorrect(DateTime returned, DateTime expected, String format, CultureInfo culture){
 switch(format){
 case "m":
 case "M":
   return ((returned.Month==expected.Month) && (returned.Day==expected.Day));
 case "t":
   return ((returned.Hour==expected.Hour) && (returned.Minute==expected.Minute));
 case "T":
   return ((returned.Hour==expected.Hour) && (returned.Minute==expected.Minute) && (returned.Second==expected.Second));
 case "y":
 case "Y":
   if(culture.Name.Equals("ar-SA")){
   return (returned.ToString("y", culture.DateTimeFormat)==expected.ToString("y", culture.DateTimeFormat));
   }else
     return ((returned.Month==expected.Month) && (returned.Year==expected.Year));
 default:
   return (returned==expected);
 }
 }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8570Parse_ifp cbA = new Co8570Parse_ifp();
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
}
