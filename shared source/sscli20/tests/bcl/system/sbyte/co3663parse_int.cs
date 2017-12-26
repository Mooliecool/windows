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
public class Co3663Parse_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "SignedByte.Parse(String s, int style)";
 public static String s_strTFName        = "Co3663Parse_int.cs";
 public static String s_strTFAbbrev      = "Co3663";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   SByte sbyt1a = 0;
   String str1 = null;
   String str2 = null;
   SByte[] sbytExpValues = {SByte.MinValue, 
			    -13,
			    -5,
			    -0,
			    0,
			    5,
			    13,
			    50,
			    101,
			    SByte.MaxValue
   };
   String[] strCFormat1Values = {"($128.00)", 
                                 "($13.00)",
                                 "($5.00)",
                                 "$0.00",
                                 "$0.00",
                                 "$5.00",
                                 "$13.00",
                                 "$50.00",
                                 "$101.00",
                                 "$127.00",
   };
   String[] strResultCFormat2 = {"($128.0000)", 
                                 "($13.0000)",
                                 "($5.0000)",
                                 "$0.0000",
                                 "$0.0000",
                                 "$5.0000",
                                 "$13.0000",
                                 "$50.0000",
                                 "$101.0000",
                                 "$127.0000",
   };
   String[] strDFormat1Values = {"-128", 
				 "-13",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "127"
   };
   String[] strDFormat2Values = {"-0128", 
				 "-0013",
				 "-0005",
				 "0000",
				 "0000",
				 "0005",
				 "0013",
				 "0050",
				 "0101",
				 "0127"
   };
   String[] strEFormat1Values = {"-1.280000E+002", 
                                 "-1.300000E+001",
                                 "-5.000000E+000",
                                 "0.000000E+000",
                                 "0.000000E+000",
                                 "5.000000E+000",
                                 "1.300000E+001",
                                 "5.000000E+001",
                                 "1.010000E+002",
                                 "1.270000E+002",
   };
   String[] strEFormat2Values = {"-1.2800e+002", 
                                 "-1.3000e+001",
                                 "-5.0000e+000",
                                 "0.0000e+000",
                                 "0.0000e+000",
                                 "5.0000e+000",
                                 "1.3000e+001",
                                 "5.0000e+001",
                                 "1.0100e+002",
                                 "1.2700e+002",
   };
   String[] strFFormat1Values = {"-128.00", 
				 "-13.00",
				 "-5.00",
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "50.00",
				 "101.00",
				 "127.00"
   };
   String[] strFFormat2Values = {"-128.0000", 
				 "-13.0000",
				 "-5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "50.0000",
				 "101.0000",
				 "127.0000"
   };
   String[] strGFormat1Values = {"-128", 
				 "-13",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "127"
   };
   String[] strGFormat2Values = {"-128", 
				 "-13",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "127"
   };
   String[] strNFormat1Values = {"-128.00", 
				 "-13.00",
				 "-5.00",
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "50.00",
				 "101.00",
				 "127.00"
   };
   String[] strNFormat2Values = {"-128.0000", 
				 "-13.0000",
				 "-5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "50.0000",
				 "101.0000",
				 "127.0000"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strGFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       sbyt1a = SByte.Parse(strGFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strCFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strResultCFormat2[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strDFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strDFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strEFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strEFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strNFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strNFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       sbyt1a = SByte.Parse(strFFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < sbytExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       sbyt1a = SByte.Parse(strFFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(sbyt1a != sbytExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i==" + i + " sbyt1a==" + sbyt1a);
	 }
       }
     try
       {
       str1 = null;
       sbyt1a = SByte.Parse(str1, NumberStyles.Any);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_903xr! Exception not thrown");
       }
     catch (ArgumentException ex)
       {
       }
     catch (Exception ex)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_683dr! wrong exception thrown " + ex);
       }
     try
       {
       str1 = "128";
       sbyt1a = SByte.Parse(str1, NumberStyles.Any);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_867sq! Exception not thrown");
       }
     catch (OverflowException ex)
       {
       }
     catch (Exception ex)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_106ep! wrong exception thrown " + ex);
       }
     try
       {
       str1 = "-129";
       sbyt1a = SByte.Parse(str1, NumberStyles.Any);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_024xw! Exception not thrown");
       }
     catch (OverflowException ex)
       {
       }
     catch (Exception ex)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_105xp! wrong exception thrown " + ex);
       }
     try
       {
       str1 = "gibberish-129";
       sbyt1a = SByte.Parse(str1, NumberStyles.Any);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_204es! Exception not thrown");
       }
     catch (FormatException ex)
       {
       }
     catch (Exception ex)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_104nu! wrong exception thrown " + ex);
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
   Co3663Parse_int cbA = new Co3663Parse_int();
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
