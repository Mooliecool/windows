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
using System.IO;
using System;
using System.Globalization;
public class Co5021Parse_int_nfi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Integer2.Parse(String s, int style, NumberFormatInfo nfi)";
 public static String s_strTFName        = "Co5021Parse_int_nfi.cs";
 public static String s_strTFAbbrev      = "Co5021";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Int16 in2a = (Int16)0;
   Int16 in2b = (Int16)0;
   String strOut = null;
   String str2 = null;
   Int16[] in2TestValues = {Int16.MinValue, 
			    -1000,
			    -99,
			    -5,
			    -0,
			    0,
			    5,
			    13,
			    101,
			    1000,
			    Int16.MaxValue
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencySymbol = "&";  
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" in2a=="+in2a);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "G", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "G5", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "C", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "C4", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "D", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "D4", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "E", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "e4", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "N", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "N4", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "F", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in2TestValues[i].ToString( "F4", nfi1);
       in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
       if(in2a != in2TestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" in2a=="+in2a+" strOut=="+strOut);
	 }
       }
     strOut = null;
     iCountTestcases++;
     try {
     in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , in2a=="+in2a);
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
     }
     strOut = "33000";
     iCountTestcases++;
     try {
     in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , in2a=="+in2a);
     } catch (OverflowException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_523eu! ,exc=="+exc);
     }
     strOut ="-33000";
     iCountTestcases++;
     try {
     in2a = Int16.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,in2a=="+in2a);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_371jy! ,exc=="+exc);
     }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5021Parse_int_nfi cbA = new Co5021Parse_int_nfi();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
