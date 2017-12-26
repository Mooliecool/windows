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
public class Co8569ParseExact_str_strA_ifp_dts
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.ParseExact(String s, String[] formats, IFormatProvider provider, DateTimeStyle style)";
 public static String s_strTFName        = "Co8569ParseExact_str_strA_ifp_dts.cs";
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
   DateTimeStyles style;
   String s;
   String[] formats = {"d", "D", "f", "F", "g", "G", "m", "M", "r", "R", "s", "t", "T", "u", "U", "y", "Y"};
   String[] forms;
   try
     {
     strLoc = "Loc_384sdg";
     iCountTestcases++;
     try{
     dateReturned = DateTime.ParseExact(null, formats, CultureInfo.InvariantCulture.DateTimeFormat, DateTimeStyles.None);
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Exception not thrown");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Wrong Exception thrown, {0}", ex.GetType().Name);
     }
     iCountTestcases++;
     date = new DateTime(2001, 4, 5, 21, 3, 4, 980);
     s = date.ToString();
     forms = null;
     try{
     dateReturned = DateTime.ParseExact(s, forms, CultureInfo.InvariantCulture.DateTimeFormat, DateTimeStyles.None);
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Exception not thrown");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Wrong Exception thrown, {0}", ex.GetType().Name);
     }
     iCountTestcases++;
     date = new DateTime(2001, 4, 5, 21, 3, 4, 980);
     s = date.ToString();
     forms = new String[0];
     try{
     dateReturned = DateTime.ParseExact(s, formats, CultureInfo.InvariantCulture.DateTimeFormat, DateTimeStyles.None);
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Exception not thrown");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Wrong Exception thrown, {0}", ex.GetType().Name);
     }
     iCountTestcases++;
     s = String.Empty;
     try{
     dateReturned = DateTime.ParseExact(s, formats, CultureInfo.InvariantCulture.DateTimeFormat, DateTimeStyles.None);
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Exception not thrown");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Wrong Exception thrown, {0}", ex.GetType().Name);
     }
     date = new DateTime(2001, 4, 5, 21, 3, 4, 980);
     cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
     foreach(CultureInfo culture in cultures){
     strLoc = "Loc_37tsdg";
     if(culture.IsNeutralCulture)
       continue;
     iCountTestcases++;
     info = culture.DateTimeFormat;
     s = date.ToString(info);
     try{
     dateReturned = DateTime.ParseExact(s, formats, info, DateTimeStyles.None);
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_7453fg! Wrong Exception thrown, {0}", ex.GetType().Name);
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
   if(culture.Name.Equals("ar-SA") || culture.Name.Equals("div-MV")){
   return (returned.ToString("y", culture.DateTimeFormat)==expected.ToString("y", culture.DateTimeFormat));
   }else
     return ((returned.Month==expected.Month) && (returned.Year==expected.Year));
 case "u":
 case "U":
   if(culture.Name.Equals("div-MV")){
   return true;
   }else
     goto default;
 default:
   return (returned==expected);
 }
 }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8569ParseExact_str_strA_ifp_dts cbA = new Co8569ParseExact_str_strA_ifp_dts();
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
