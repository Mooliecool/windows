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
public class Co5044Format_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Byte.ToString(Byte value, String format)";
 public static String s_strTFName        = "Co5044Format_str.cs";
 public static String s_strTFAbbrev      = "Co5044";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Byte byt1a = (Byte)0;
   Byte bytb = (Byte)0;
   String str1 = null;
   String str2 = null;
   Byte[] bytTestValues = {Byte.MinValue, 
			   -0,
			   0,
			   5,
			   13,
			   50,
			   101,
			   200,
			   Byte.MaxValue
   };
   String[] strResultCFormat1 = {"$0.00", 
                                 "$0.00",
                                 "$0.00",
                                 "$5.00",
                                 "$13.00",
                                 "$50.00",
                                 "$101.00",
                                 "$200.00",
                                 "$255.00",
   };
   String[] strResultCFormat2 = {"$0.0000", 
                                 "$0.0000",
                                 "$0.0000",
                                 "$5.0000",
                                 "$13.0000",
                                 "$50.0000",
                                 "$101.0000",
                                 "$200.0000",
                                 "$255.0000",
   };
   String[] strResultDFormat1 = {"0", 
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "200",
				 "255"
   };
   String[] strResultDFormat2 = {"0000", 
				 "0000",
				 "0000",
				 "0005",
				 "0013",
				 "0050",
				 "0101",
				 "0200",
				 "0255"
   };
   String[] strResultEFormat1 = {"0.000000E+000", 
                                 "0.000000E+000",
                                 "0.000000E+000",
                                 "5.000000E+000",
                                 "1.300000E+001",
                                 "5.000000E+001",
                                 "1.010000E+002",
                                 "2.000000E+002",
                                 "2.550000E+002",
   };
   String[] strResultEFormat2 = {"0.0000e+000", 
                                 "0.0000e+000",
                                 "0.0000e+000",
                                 "5.0000e+000",
                                 "1.3000e+001",
                                 "5.0000e+001",
                                 "1.0100e+002",
                                 "2.0000e+002",
                                 "2.5500e+002",
   };
   String[] strResultFFormat1 = {"0.00", 
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "50.00",
				 "101.00",
				 "200.00",
				 "255.00"
   };
   String[] strResultFFormat2 = {"0.0000", 
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "50.0000",
				 "101.0000",
				 "200.0000",
				 "255.0000"
   };
   String[] strResultGFormat1 = {"0", 
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "200",
				 "255"
   };
   String[] strResultGFormat2 = {"0", 
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "200",
				 "255"
   };
   String[] strResultNFormat1 = {"0.00", 
				 "0.00",
				 "0.00",
				 "5.00",
				 "13.00",
				 "50.00",
				 "101.00",
				 "200.00",
				 "255.00"
   };
   String[] strResultNFormat2 = {"0.0000", 
				 "0.0000",
				 "0.0000",
				 "5.0000",
				 "13.0000",
				 "50.0000",
				 "101.0000",
				 "200.0000",
				 "255.0000"
   };
   String[] strResultXFormat1 = {"0", 
                                 "0",
                                 "0",
                                 "5",
                                 "D",
                                 "32",
                                 "65",
                                 "C8",
                                 "FF"
   };
   String[] strResultXFormat2 = {"0000", 
                                 "0000",
                                 "0000",
                                 "0005",
                                 "000D",
                                 "0032",
                                 "0065",
                                 "00C8",
                                 "00FF"
   };
   String[] strResultXFormat3 = {"0000", 
                                 "0000",
                                 "0000",
                                 "0005",
                                 "000d",
                                 "0032",
                                 "0065",
                                 "00c8",
                                 "00ff"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "");
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "G");
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "G4");
       if(!str1.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "C");
       if(!str1.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "C4");
       if(!str1.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "D");
       if(!str1.Equals(strResultDFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "D4");
       if(!str1.Equals(strResultDFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "E");
       if(!str1.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "e4");
       if(!str1.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "N");
       if(!str1.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "N4");
       if(!str1.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2200aa_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "X");
       if(!str1.Equals(strResultXFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_384wi! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2300nr_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "X4");
       if(!str1.Equals(strResultXFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_837qo! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2400qi_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "x4");
       if(!str1.Equals(strResultXFormat3[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_833jd! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "F");
       if(!str1.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = bytTestValues[i].ToString( "F4");
       if(!str1.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" str1=="+str1);
	 }
       }
     strLoc = "Loc_248oj";
     byt1a = (Byte)(50);
     str1 = "humbug: 50";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "'humbug:' ###")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_125rt! ,format=="+byt1a.ToString( "'humbug:' ###"));
       }
     strLoc = "Loc_250ej";
     byt1a = (Byte)45;
     str1 = "00045";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "0000#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_279eo! ,format=="+byt1a.ToString( "0000#"));
       }
     strLoc = "Loc_140jj";
     byt1a = (Byte)23;
     str1 = "%2300";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "%#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_182nq! ,format=="+byt1a.ToString( "%#"));
       }
     strLoc = "Loc_145hw";
     byt1a = (Byte)34;
     str1 = "test 34 test";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "test #### test")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wj! ,format=="+byt1a.ToString( "test #### test"));
       }
     strLoc = "Loc_328au";
     byt1a = (Byte)33;
     str1 = "33E+000";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "##.#E+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_892wo! ,format=="+byt1a.ToString( "##.#E+000"));
       }
     strLoc = "Loc_329er";
     byt1a = (Byte)33;
     str1 = "33e+000";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "##.#e+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293nw! ,format=="+byt1a.ToString( "##.#e+000"));
       }
     strLoc = "Loc_347yu";
     byt1a = (Byte)123;
     str1 = "12\\3";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "##\\\\#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_087he! ,format=="+byt1a.ToString( "##\\\\#"));
       }
     strLoc = "Loc_378nq";
     byt1a = (Byte)123;
     str1 = "ABC#123";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "'ABC#'##")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128qx! ,strOut=="+byt1a.ToString( "'ABC#'##"));
       }
     strLoc = "Loc_400ne";
     byt1a = (Byte)(123);
     str1 = "00123";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "00000;00;00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_539tw! ,strOut=="+byt1a.ToString( "00000;000000;00"));
       }
     strLoc = "Loc_415ue";
     byt1a = (Byte)0;
     str1 = "000";
     iCountTestcases++;
     if(!str1.Equals(byt1a.ToString( "00;00;000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+byt1a.ToString( "00;00;000"));
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
   Co5044Format_str cbA = new Co5044Format_str();
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
