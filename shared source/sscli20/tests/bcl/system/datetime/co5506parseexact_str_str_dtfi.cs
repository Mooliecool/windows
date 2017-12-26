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
using System.Globalization;
public class Co5506ParseExact_str_str_dtfi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.Parse(String, String, dtfi)";
 public static String s_strTFName        = "Co5506ParseExact_str_str_dtfi.cs";
 public static String s_strTFAbbrev      = "Co5506";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     DateTime dt1, dt2;
     String str1 = String.Empty;
     CultureInfo ci = CultureInfo.InvariantCulture;
     strLoc = "Loc_498hx";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact(null, "", null);
     iCountErrors++;
     printerr( "Error_98hvc! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (ArgumentNullException aexc) {
     printinfo( "Info_98hcy! Caught expected ArgumentNullException, aexc=="+aexc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_29h8x! Unexpected Exception: "+exc.ToString());
     }
     strLoc = "Loc_49hvh";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact("", null, null);			dt2 = new DateTime(2000, 2, 29, 17, 59, 01);
     iCountErrors++;
     printerr( "Error_458vh! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (ArgumentNullException aexc) {
     printinfo( "Info_29hce! Caught expected ArgumentNullException, aexc=="+aexc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_198zz! Unexpected exception: "+exc.ToString());
     }
     strLoc = "Loc_20jx9";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact("", "G", null);
     iCountErrors++;
     printerr( "Error_298hc! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (FormatException fExc) {
     printinfo( "Info_98v89! Caught expected exception, fexc=="+fExc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_209xq! Unexpected exception: "+exc.ToString());
     }
     strLoc = "Loc_298yg";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact(DateTime.Now.ToString(), "", null);
     iCountErrors++;
     printerr( "Error_2h8x9! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (FormatException ) {
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_209v9! unexpected exception: "+exc.ToString());
     }
     CultureInfo[] cultinfos = CultureInfo.GetCultures(CultureTypes.AllCultures);
     IEnumerator cultenum = new ArrayList(cultinfos).GetEnumerator();
     strLoc = "Loc_248ch";
     str1 = "-1/10/2000 17:44:22";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact(str1, "G", DateTimeFormatInfo.GetInstance(CultureInfo.InvariantCulture));
     iCountErrors++;
     printerr( "Error_2d333! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (FormatException fexc) {
     printinfo( "Info_98hcx! Caught expected FormatException, fexc=="+fexc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_89hf8! Unexpected exception: "+exc.ToString());
     }
     str1 = "11/31/2000 14:38:22";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact(str1, "G", DateTimeFormatInfo.GetInstance(CultureInfo.InvariantCulture));
     iCountErrors++;
     printerr( "Error_1908x! Expected exception not thrown, dt1=="+dt1.ToString());
     } catch (FormatException fexc) {
     printinfo( "Info_1979s! Caught expected Exception, fexc=="+fexc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_287vh! Unexpected exception: "+exc.ToString());
     }
     str1 = "31/11/2000 14:44:22";
     iCountTestcases++;
     try {
     dt1 = DateTime.ParseExact(str1, "G", DateTimeFormatInfo.GetInstance(CultureInfo.InvariantCulture));
     iCountErrors++;
     printerr( "Error_29299! Expected Exception not thrown, dt1=="+dt1.ToString());
     } catch (FormatException fexc) {
     printinfo( "Info_39duh! Caught expected Exception, fexc=="+fexc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr("Error_4h7vc! Unexpected exception: "+exc.ToString());
     }
     while(cultenum.MoveNext()) {
     ci = (CultureInfo)cultenum.Current;
     if(ci.IsNeutralCulture)
       continue;
     if(ci.Name.Equals("div-MV"))
       continue;
     strLoc = "Loc_98vhg";
     try {
     dt2 = new DateTime(2000, 5, 10, 17, 44, 22);
     str1 = dt2.ToString("G", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "G",DateTimeFormatInfo.GetInstance(ci) );
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_98hx1! Expected=="+dt2.ToString()+ " , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_298dx! Failed RoundTrip:" +ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_298hx";
     try {
     dt2 = new DateTime(2000, 5, 10, 17, 44, 0);
     str1 = dt2.ToString("g", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(dt2.ToString("g", DateTimeFormatInfo.GetInstance(ci)), "g", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_g98h4! Expected=="+dt2.ToString()+ " , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_298vn! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_t598h";
     try {
     dt2 = new DateTime(2000, 2, 29, 17, 59, 22);
     dt1 = DateTime.ParseExact(dt2.ToString("d", DateTimeFormatInfo.GetInstance(ci)), "d", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(new DateTime(2000, 2, 29))) {
     iCountErrors++;
     printerr( "Error_399s8! Expected=="+new DateTime(2000, 2, 29).ToString()+ " , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_48hfd! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_98hgy";
     str1 = "2/28/1999 13:00:01";
     try {
     dt2 = new DateTime(1999, 2, 28, 17, 00, 01);
     dt1 = DateTime.ParseExact(dt2.ToString("D", DateTimeFormatInfo.GetInstance(ci)), "D", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(new DateTime(1999, 2, 28))) {
     iCountErrors++;
     printerr( "Error_29hcy! Expected=="+new DateTime(1999, 2, 28).ToString() + " , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_28y84! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_4987v";
     try {
     dt2 = new DateTime(2000, 2, 29, 17, 59, 00);
     str1 = dt2.ToString("f", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "f", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.ToString("f", DateTimeFormatInfo.GetInstance(ci)).Equals(str1)) {
     iCountErrors++;
     printerr( "Error_4998x! Expected=="+dt2.Ticks +" , got=="+dt1.Ticks);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_298x7! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_47hcq";
     try {
     dt2 = new DateTime(2000, 2, 29, 17, 59, 01);
     str1 = dt2.ToString("F", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "F", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_3190s! Expected=="+dt2.ToString() +" , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_209us! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_2388c";
     try {
     dt2 = new DateTime(1999, 12, 31, 23, 23, 23);
     str1 = dt2.ToString("m", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "m", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(
	!ci.Equals(new CultureInfo("ar")) && 
	!ci.Equals(new CultureInfo("ar-SA"))
	) {
     if(dt1.Day != dt2.Day) {
     iCountErrors++;
     printerr( "Error_4985f! Expected=="+dt2.Day+" , got=="+dt1.Day+", ci=="+ci);
     } 
     iCountTestcases++;
     if(dt1.Month != dt2.Month) {
     iCountErrors++;
     printerr( "Error_28x8y! Expected=="+dt2.Month+", got=="+dt1.Month+", ci=="+ci);
     }
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_857yv! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_4f87x";
     try {
     dt2 = new DateTime(1999, 12, 31, 23, 23, 23);
     str1 = dt2.ToString("M", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "M", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(
	!ci.Equals(new CultureInfo("ar")) && 
	!ci.Equals(new CultureInfo("ar-SA"))
	) {
     if(dt1.Day != dt2.Day) {
     iCountErrors++;
     printerr( "Error_988s3! Expected=="+dt2.Day+" , got=="+dt1.Day+", ci=="+ci);
     } 
     if(dt1.Month != dt2.Month) {
     iCountErrors++;
     printerr( "Error_1998s! Expected=="+dt2.Month+", got=="+dt1.Month+", ci=="+ci);
     }
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_298xh! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_87g87";
     try {
     dt2 = new DateTime(1883, 1, 1, 1, 1, 1);
     str1 = dt2.ToString("s", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "s", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_928hd! Culture=="+ci+" , Expected=="+str1+" , got=="+dt1.ToString("s", DateTimeFormatInfo.GetInstance(ci))+" , Culture=="+ci.DisplayName);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_1298s! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_98409";
     try {
     dt2 = new DateTime(938, 12, 12, 12, 19, 32);
     str1 = dt2.ToString("t", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "t", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != dt2.Hour) {
     iCountErrors++;
     printerr( "Error_298hv! Expected=="+dt2.Hour+", got=="+dt1.Hour+", ci=="+ci.DisplayName);
     }
     iCountTestcases++;
     if(dt1.Minute != dt2.Minute) {
     iCountErrors++;
     printerr( "Error_298cy! Expected=="+dt2.Minute+", got=="+dt1.Minute+", ci=="+ci.DisplayName);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_9029x! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_298s7";
     try {
     dt2 = new DateTime(8389, 12, 12, 12, 47, 21);
     str1 = dt2.ToString("T", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "T", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != dt2.Hour) {
     iCountErrors++;
     printerr( "Error_98xhy! Expected=="+dt2.Hour+", got=="+dt1.Hour+", ci=="+ci.DisplayName);
     }
     iCountTestcases++;
     if(dt1.Minute != dt2.Minute) {
     iCountErrors++;
     printerr( "Error_199ay! Expected=="+dt2.Minute+", got=="+dt1.Minute+", ci=="+ci.DisplayName);
     }
     iCountTestcases++;
     if(dt1.Second != dt2.Second) {
     iCountErrors++;
     printerr( "Error_20x98! Expected=="+dt2.Second+", got=="+dt2.Second+", ci=="+ci.DisplayName);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_2019s! Failed Roundtrip: "+ci.DisplayName+" : "+ci+", ci=="+ci.DisplayName);
     }
     strLoc = "Loc_1s98j";
     try {
     dt2 = new DateTime(2000, 2, 29, 16, 59, 01);
     str1 = dt2.ToString("u", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "u", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_209zj! Expected=="+dt2.ToString()+" , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_209sj! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_fh888";
     try {
     dt2 = new DateTime(2000, 2, 29, 16, 34, 23);
     str1 = dt2.ToString("U", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "U", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(!dt1.Equals(dt2)) {
     iCountErrors++;
     printerr( "Error_209su! Expected=="+dt2.ToString()+" , got=="+dt1.ToString());
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_109us! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_2898b";
     try {
     dt2 = new DateTime(3233, 2, 21, 21, 32, 22);
     str1 = dt2.ToString("y", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "y", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Year != dt2.Year) {
     iCountErrors++;
     printerr( "Error_28hcx! Expected=="+dt2.Year+", got=="+dt1.Year+", ci=="+ci.DisplayName);
     }
     if(dt1.Month != dt2.Month) {
     iCountErrors++;
     printerr( "Error_87v8x! Expected=="+dt2.Month+", got=="+dt1.Month);
     } 
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_019de! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_9898v";
     try {
     dt2 = new DateTime(3233, 3, 21, 21, 32, 22);
     str1 = dt2.ToString("Y", DateTimeFormatInfo.GetInstance(ci));
     dt1 = DateTime.ParseExact(str1, "Y", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Year != dt2.Year) {
     iCountErrors++;
     printerr( "Error_208x9! Expected=="+dt2.Year+", got=="+dt1.Year+", ci=="+ci.DisplayName);
     }
     if(dt1.Month != dt2.Month) {
     iCountErrors++;
     printerr( "Error_8459b! Expected=="+dt2.Month+" ,got=="+dt1.Month+", ci=="+ci.DisplayName);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_099su! Failed Roundtrip: "+ci.DisplayName+" : "+ci);
     }
     }
     ci = CultureInfo.InvariantCulture;
     strLoc = "Loc_0g9u7";
     strLoc = "Loc_398hc";
     dt1 = new DateTime(2000, 2, 29, 23, 0, 0);
     str1 = dt1.ToString("h tt", DateTimeFormatInfo.GetInstance(ci));
     try {
     dt1 = DateTime.ParseExact(str1, "h tt", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 23) {
     iCountErrors++;
     printerr( "Error_298hd! Expected==11, got=="+dt1.Hour);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_28hc8! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_98hvy";
     dt1 = new DateTime(2000, 2, 29, 23, 0, 0);
     str1 = dt1.ToString("hh tt", DateTimeFormatInfo.GetInstance(ci));
     try {
     dt1 = DateTime.ParseExact(str1, "hh tt", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 23) {
     iCountErrors++;
     printerr( "Error_1989s! Expected==11, got=="+dt1.Hour);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_198au! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_2987x";
     dt1 = new DateTime(2000, 2, 29, 23, 0, 0);
     str1 = dt1.ToString("hh tt", DateTimeFormatInfo.GetInstance(ci));
     try {
     dt1 = DateTime.ParseExact(str1, "hhhhhhhhhhhhhhhhhhhhhhhh tttttttttttttttttttttttttt", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 23) {
     iCountErrors++;
     printerr( "Error_298xy! Expected==11, got=="+dt1.Hour);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_29885! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_120su";
     try {
     str1 = "0";
     dt1 = DateTime.ParseExact(str1, "H", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 0) {
     iCountErrors++;
     printerr( "Error_2c7hc! Exected==0, got=="+dt1.Hour);
     }
     } catch (FormatException) {
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_209ve! Unexpected exception, , exc=="+exc.ToString());
     }
     strLoc = "Loc_298vy";
     str1 = "14";
     try {
     dt1 = DateTime.ParseExact(str1, "HH", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 14) {
     iCountErrors++;
     printerr( "Error_98cz5! Expected==14, got value=="+dt1.Hour);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_0190a! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_27yvw";
     str1 = "00";
     try {
     dt1 = DateTime.ParseExact(str1, "HH", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Hour != 0) {
     iCountErrors++;
     printerr( "Error_78yg7! Expected==0, got value=="+dt1.Hour);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_2109s! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_t87cy";
     str1 = "1 ";
     try {
     dt1 = DateTime.ParseExact(str1, "m ", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Minute != 1) {
     iCountErrors++;
     printerr( "Error_29cc7! Expected==1, got value=="+dt1.Minute);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_298hd! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     } 
     strLoc = "Loc_87ygy";
     str1 = "59";
     try {
     dt1 = DateTime.ParseExact(str1, "mm", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Minute != 59) {
     iCountErrors++;
     printerr( "Error_38d87! Expected==59, got value=="+dt1.Minute);
     } 
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_010su! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_98yv";
     str1 = "13 ";
     try {
     dt1 = DateTime.ParseExact(str1, "s ", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Second != 13) {
     iCountErrors++;
     printerr( "Error_29x87! Expected==13, got value=="+dt1.Second);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_29yd8! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_287yc";
     str1 = "45";
     try {
     dt1 = DateTime.ParseExact(str1, "ss", DateTimeFormatInfo.GetInstance(ci));
     iCountTestcases++;
     if(dt1.Second != 45) {
     iCountErrors++;
     printerr( "Error_2x993! Expected==45, got value=="+dt1.Second);
     }
     } catch (Exception) {
     iCountErrors++;
     printerr( "Error_1980x! Failed RoundTrip: "+ci.DisplayName+" : "+ci);
     }
     strLoc = "Loc_928hx";
     strLoc = "Loc_209xj";
     Boolean invarfound = false;
     cultinfos = CultureInfo.GetCultures(CultureTypes.AllCultures);
     cultenum = new ArrayList(cultinfos).GetEnumerator();
     while(cultenum.MoveNext())
       if(((CultureInfo)cultenum.Current).Equals(CultureInfo.InvariantCulture)) 
	 invarfound = true;
     iCountTestcases++;
     if(!invarfound) {
     iCountErrors++;
     printerr( "Error_20a9e! Invariant Culture not in supported cultures, ");
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5506ParseExact_str_str_dtfi cbA = new Co5506ParseExact_str_str_dtfi();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
