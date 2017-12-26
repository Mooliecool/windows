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
public class Co5006Format_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Integer2.ToString(Integer2 value, String format)";
 public static String s_strTFName        = "Co5006Format_str.cs";
 public static String s_strTFAbbrev      = "Co5006";
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
   String str1 = null;
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
   String[] strResultCFormat1 = {"($32,768.00)", 
                                 "($1,000.00)",
                                 "($99.00)",
                                 "($5.00)",
                                 "$0.00",
                                 "$0.00",
                                 "$5.00",
                                 "$13.00",
                                 "$101.00",
                                 "$1,000.00",
                                 "$32,767.00",
   };
   String[] strResultCFormat2 = {"($32,768.0000)", 
                                 "($1,000.0000)",
                                 "($99.0000)",
                                 "($5.0000)",
                                 "$0.0000",
                                 "$0.0000",
                                 "$5.0000",
                                 "$13.0000",
                                 "$101.0000",
                                 "$1,000.0000",
                                 "$32,767.0000",
   };
   String[] strResultDFormat1 = {"-32768", 
				 "-1000",
				 "-99",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "32767"
   };
   String[] strResultDFormat2 = {"-32768", 
				 "-1000",
				 "-0099",
				 "-0005",
				 "0000",
				 "0000",
				 "0005",
				 "0013",
				 "0101",
				 "1000",
				 "32767"
   };
   String[] strResultEFormat1 = {"-3.276800E+004", 
                                 "-1.000000E+003",
                                 "-9.900000E+001",
                                 "-5.000000E+000",
                                 "0.000000E+000",
                                 "0.000000E+000",
                                 "5.000000E+000",
                                 "1.300000E+001",
                                 "1.010000E+002",
                                 "1.000000E+003",
                                 "3.276700E+004",
   };
   String[] strResultEFormat2 = {"-3.2768e+004", 
                                 "-1.0000e+003",
                                 "-9.9000e+001",
                                 "-5.0000e+000",
                                 "0.0000e+000",
                                 "0.0000e+000",
                                 "5.0000e+000",
                                 "1.3000e+001",
                                 "1.0100e+002",
                                 "1.0000e+003",
                                 "3.2767e+004",
   };
   String[] strResultFFormat1 = {"-32768.00", 
				 "-1000.00",
				 "-99.00",
				 "-5.00",
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "101.00",
				 "1000.00",
				 "32767.00"
   };
   String[] strResultFFormat2 = {"-32768.0000", 
				 "-1000.0000",
				 "-99.0000",
				 "-5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "101.0000",
				 "1000.0000",
				 "32767.0000"
   };
   String[] strResultGFormat1 = {"-32768", 
				 "-1000",
				 "-99",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "32767"
   };
   String[] strResultGFormat2 = {"-3.277E+04", 
				 "-1000",
				 "-99",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "3.277E+04"
   };
   String[] strResultNFormat1 = {"-32,768.00", 
				 "-1,000.00",
				 "-99.00",
				 "-5.00",
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "101.00",
				 "1,000.00",
				 "32,767.00"
   };
   String[] strResultNFormat2 = {"-32,768.0000", 
				 "-1,000.0000",
				 "-99.0000",
				 "-5.0000",
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "101.0000",
				 "1,000.0000",
				 "32,767.0000"
   };
   String[] strResultXFormat1 = {"8000", 
                                 "FC18",
                                 "FF9D",
                                 "FFFB",
                                 "0",
                                 "0",
                                 "5",
                                 "D",
                                 "65",
                                 "3E8",
                                 "7FFF"
   };
   String[] strResultXFormat2 = {"08000", 
                                 "0FC18",
                                 "0FF9D",
                                 "0FFFB",
                                 "00000",
                                 "00000",
                                 "00005",
                                 "0000D",
                                 "00065",
                                 "003E8",
                                 "07FFF"
   };
   String[] strResultXFormat3 = {"8000", 
                                 "fc18",
                                 "ff9d",
                                 "fffb",
                                 "000",
                                 "000",
                                 "005",
                                 "00d",
                                 "065",
                                 "3e8",
                                 "7fff"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "");
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "G");
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "G4");
       if(!str1.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "C");
       if(!str1.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "C4");
       if(!str1.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "D");
       if(!str1.Equals(strResultDFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "D4");
       if(!str1.Equals(strResultDFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "E");
       if(!str1.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "e4");
       if(!str1.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "N");
       if(!str1.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "N4");
       if(!str1.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2200aa_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "X");
       if(!str1.Equals(strResultXFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_384wi! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2300nr_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "X5");
       if(!str1.Equals(strResultXFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_837qo! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2400qi_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "x3");
       if(!str1.Equals(strResultXFormat3[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_833jd! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "F");
       if(!str1.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < in2TestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = in2TestValues[i].ToString( "F4");
       if(!str1.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" str1=="+str1);
	 }
       }
     strLoc = "Loc_248oj";
     in2a = (Int16)(-50);
     str1 = "-humbug: 50";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "'humbug:' ###")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_125rt! ,format=="+in2a.ToString( "'humbug:' ###"));
       }
     strLoc = "Loc_250ej";
     in2a = (Int16)45;
     str1 = "00045";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "0000#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_279eo! ,format=="+in2a.ToString( "0000#"));
       }
     strLoc = "Loc_129nq";
     in2a = (Int16)29145;
     str1 = "29,145";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "#,#.")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_931ee! ,format=="+in2a.ToString( "#,#."));
       }
     strLoc = "Loc_139qi";
     in2a = (Int16)30000;
     str1 = "30";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "#,")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_289qi! ,format=="+in2a.ToString( "#,"));
       }
     strLoc = "Loc_140jj";
     in2a = (Int16)23;
     str1 = "%2300";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "%#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_182nq! ,format=="+in2a.ToString( "%#"));
       }
     strLoc = "Loc_145hw";
     in2a = (Int16)34;
     str1 = "test 34 test";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "test #### test")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wj! ,format=="+in2a.ToString( "test #### test"));
       }
     strLoc = "Loc_328au";
     in2a = (Int16)3034;
     str1 = "30.3E+002";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "##.#E+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_892wo! ,format=="+in2a.ToString( "##.#E+000"));
       }
     strLoc = "Loc_329er";
     in2a = (Int16)3034;
     str1 = "30.3e+002";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "##.#e+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293nw! ,format=="+in2a.ToString( "##.#e+000"));
       }
     strLoc = "Loc_347yu";
     in2a = (Int16)1234;
     str1 = "12\\34";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "##\\\\##")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_087he! ,format=="+in2a.ToString( "##\\\\##"));
       }
     strLoc = "Loc_378nq";
     in2a = (Int16)1234;
     str1 = "ABC#1234";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "'ABC#'##")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128qx! ,strOut=="+in2a.ToString( "'ABC#'##"));
       }
     strLoc = "Loc_400ne";
     in2a = (Int16)(1234);
     str1 = "01234";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "00000;00;00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_539tw! ,strOut=="+in2a.ToString( "00000;000000;00"));
       }
     strLoc = "Loc_412he";
     in2a = (Int16)(-123);
     str1 = "-000123";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "00;-000000;00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_739em! ,strOut=="+in2a.ToString( "00;-000000;00"));
       }
     strLoc = "Loc_415ue";
     in2a = (Int16)0;
     str1 = "000";
     iCountTestcases++;
     if(!str1.Equals(in2a.ToString( "00;00;000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+in2a.ToString( "00;00;000"));
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
   Co5006Format_str cbA = new Co5006Format_str();
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
