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
public class Co5122Format_str_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Integer8.ToString(Integer8 value, String format, NumberFormatInfo info)";
 public static String s_strTFName        = "Co5122Format_str_ifp.cs";
 public static String s_strTFAbbrev      = "Co5122";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Int64 in8a = (Int64)0;
   String strOut = null;
   String str2 = null;
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencySymbol = "&";  
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   Int64[] in8TestValues = {Int64.MinValue, 
			    -1000,
			    -99,
			    -5,
			    -0,
			    0,
			    5,
			    13,
			    101,
			    1000,
			    Int64.MaxValue
   };
   String[] strResultCFormat1 = {"(&9,223,372,036,854,775,808.000)", 
                                 "(&1,000.000)",
                                 "(&99.000)",
                                 "(&5.000)",
                                 "&0.000",
                                 "&0.000",
                                 "&5.000",
                                 "&13.000",
                                 "&101.000",
                                 "&1,000.000",
                                 "&9,223,372,036,854,775,807.000",
   };
   String[] strResultCFormat2 = {"(&9,223,372,036,854,775,808.0000)", 
                                 "(&1,000.0000)",
                                 "(&99.0000)",
                                 "(&5.0000)",
                                 "&0.0000",
                                 "&0.0000",
                                 "&5.0000",
                                 "&13.0000",
                                 "&101.0000",
                                 "&1,000.0000",
                                 "&9,223,372,036,854,775,807.0000",
   };
   String[] strResultDFormat1 = {"^9223372036854775808", 
				 "^1000",
				 "^99",
				 "^5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "9223372036854775807"
   };
   String[] strResultDFormat2 = {"^9223372036854775808", 
				 "^1000",
				 "^0099",
				 "^0005",
				 "0000",
				 "0000",
				 "0005",
				 "0013",
				 "0101",
				 "1000",
				 "9223372036854775807"
   };
   String[] strResultEFormat1 = {"^9.223372E+018", 
                                 "^1.000000E+003",
                                 "^9.900000E+001",
                                 "^5.000000E+000",
                                 "0.000000E+000",
                                 "0.000000E+000",
                                 "5.000000E+000",
                                 "1.300000E+001",
                                 "1.010000E+002",
                                 "1.000000E+003",
                                 "9.223372E+018",
   };
   String[] strResultEFormat2 = {"^9.2234e+018", 
                                 "^1.0000e+003",
                                 "^9.9000e+001",
                                 "^5.0000e+000",
                                 "0.0000e+000",
                                 "0.0000e+000",
                                 "5.0000e+000",
                                 "1.3000e+001",
                                 "1.0100e+002",
                                 "1.0000e+003",
                                 "9.2234e+018",
   };
   String[] strResultFFormat1 = {"^9223372036854775808.000", 
				 "^1000.000",
				 "^99.000",
				 "^5.000",
				 "0.000",
				 "0.000",
				 "5.000",
				 "13.000",
				 "101.000",
				 "1000.000",
				 "9223372036854775807.000"
   };
   String[] strResultFFormat2 = {"^9223372036854775808.0000", 
				 "^1000.0000",
				 "^99.0000",
				 "^5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "101.0000",
				 "1000.0000",
				 "9223372036854775807.0000"
   };
   String[] strResultGFormat1 = {"^9223372036854775808", 
				 "^1000",
				 "^99",
				 "^5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "9223372036854775807"
   };
   String[] strResultGFormat2 = {"^9.223E+18", 
				 "^1000",
				 "^99",
				 "^5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "9.223E+18"
   };
   String[] strResultNFormat1 = {"^9,223,372,036,854,775,808.000", 
				 "^1,000.000",
				 "^99.000",
				 "^5.000",
				 "0.000",
				 "0.000",
				 "5.000",
				 "13.000",
				 "101.000",
				 "1,000.000",
				 "9,223,372,036,854,775,807.000"
   };
   String[] strResultNFormat2 = {"^9,223,372,036,854,775,808.0000", 
				 "^1,000.0000",
				 "^99.0000",
				 "^5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "101.0000",
				 "1,000.0000",
				 "9,223,372,036,854,775,807.0000"
   };
   String[] strResultXFormat1 = {"8000000000000000", 
                                 "FFFFFFFFFFFFFC18",
                                 "FFFFFFFFFFFFFF9D",
                                 "FFFFFFFFFFFFFFFB",
                                 "0",
                                 "0",
                                 "5",
                                 "D",
                                 "65",
                                 "3E8",
                                 "7FFFFFFFFFFFFFFF"
   };
   String[] strResultXFormat2 = {"8000000000000000", 
                                 "FFFFFFFFFFFFFC18",
                                 "FFFFFFFFFFFFFF9D",
                                 "FFFFFFFFFFFFFFFB",
                                 "0000",
                                 "0000",
                                 "0005",
                                 "000D",
                                 "0065",
                                 "03E8",
                                 "7FFFFFFFFFFFFFFF"
   };
   String[] strResultXFormat3 = {"8000000000000000", 
                                 "fffffffffffffc18",
                                 "ffffffffffffff9d",
                                 "fffffffffffffffb",
                                 "0000",
                                 "0000",
                                 "0005",
                                 "000d",
                                 "0065",
                                 "03e8",
                                 "7fffffffffffffff"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("F",nfi1);
       if(!strOut.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("F4",nfi1);
       if(!strOut.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("G", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("G4", nfi1);
       if(!strOut.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("C", nfi1);
       if(!strOut.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("C4", nfi1);
       if(!strOut.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("D", nfi1);
       if(!strOut.Equals(strResultDFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("D4", nfi1);
       if(!strOut.Equals(strResultDFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("E", nfi1);
       if(!strOut.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("e4", nfi1);
       if(!strOut.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("N", nfi1);
       if(!strOut.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("N4", nfi1);
       if(!strOut.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2200aa_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("X", nfi1);
       if(!strOut.Equals(strResultXFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_384wi! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2300nr_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("X4", nfi1);
       if(!strOut.Equals(strResultXFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_837qo! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2400qi_";
     for (int i=0; i < in8TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = in8TestValues[i].ToString("x4", nfi1);
       if(!strOut.Equals(strResultXFormat3[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_833jd! , i=="+i+" strOut=="+strOut);
	 }
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
   Co5122Format_str_ifp cbA = new Co5122Format_str_ifp();
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
