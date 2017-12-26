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
public class Co3686Format_str_nfi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.ToString(Decimal value, String format, NumberFormatInfo nfi)";
 public static String s_strTFName        = "Co3686Format_str_nfi.cs";
 public static String s_strTFAbbrev      = "Co3686";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Decimal dcml1;
   String str1 = null;
   String str2 = null;
   Decimal[] dcmTestValues = {Decimal.MinValue, 
			      Decimal.MinusOne,
			      0,
			      Decimal.One,
			      Decimal.Parse("1.234567890123456789012345678"),
			      Decimal.MaxValue
   };
   String[] strResultCFormat1 = {"(&79,228,162,514,264,337,593,543,950,335.000)", 
				 "(&1.000)",
				 "&0.000",
				 "&1.000",
				 "&1.235",
				 "&79,228,162,514,264,337,593,543,950,335.000"
   };
   String[] strResultCFormat2 = {"(&79,228,162,514,264,337,593,543,950,335.0000)", 
				 "(&1.0000)",
				 "&0.0000",
				 "&1.0000",
				 "&1.2346",
				 "&79,228,162,514,264,337,593,543,950,335.0000"
   };
   String[] strResultEFormat1 = {"^7.922816E+028", 
				 "^1.000000E+000",
				 "0.000000E+000",
				 "1.000000E+000",
				 "1.234568E+000",
				 "7.922816E+028"
   };
   String[] strResultEFormat2 = {"^7.9228e+028", 
				 "^1.0000e+000",
				 "0.0000e+000",
				 "1.0000e+000",
				 "1.2346e+000",
				 "7.9228e+028"
   };
   String[] strResultFFormat1 = {"^79228162514264337593543950335.000", 
				 "^1.000",
				 "0.000",
				 "1.000",
				 "1.235",
				 "79228162514264337593543950335.000"
   };
   String[] strResultFFormat2 = {"^79228162514264337593543950335.0000", 
				 "^1.0000",
				 "0.0000",
				 "1.0000",
				 "1.2346",
				 "79228162514264337593543950335.0000"
   };
   String[] strResultGFormat1 = {"^79228162514264337593543950335", 
				 "^1",
				 "0",
				 "1",
				 "1.234567890123456789012345678",
				 "79228162514264337593543950335"
   };
   String[] strResultGFormat2 = {"^7.923E+28", 
				 "^1",
				 "0",
				 "1",
				 "1.235",
				 "7.923E+28"
   };
   String[] strResultNFormat1 = {"^79,228,162,514,264,337,593,543,950,335.000", 
				 "^1.000",
				 "0.000",
				 "1.000",
				 "1.235",
				 "79,228,162,514,264,337,593,543,950,335.000"
   };
   String[] strResultNFormat2 = {"^79,228,162,514,264,337,593,543,950,335.0000", 
				 "^1.0000",
				 "0.0000",
				 "1.0000",
				 "1.2346",
				 "79,228,162,514,264,337,593,543,950,335.0000"
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.CurrencySymbol = "&";  
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   nfi1.NumberNegativePattern = 1;		
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "", nfi1);
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "G", nfi1);
       if(!str1.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "G4", nfi1);
       if(!str1.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "C", nfi1);
       if(!str1.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "C4", nfi1);
       if(!str1.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "E", nfi1);
       if(!str1.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "e4", nfi1);
       if(!str1.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "N", nfi1);
       if(!str1.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "N4", nfi1);
       if(!str1.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "F", nfi1);
       if(!str1.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" str1=="+str1);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < dcmTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       str1 = dcmTestValues[i].ToString( "F4", nfi1);
       if(!str1.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" str1=="+str1);
	 }
       }
     strLoc = "Loc_248oj";
     dcml1 = 50;
     str1 = "humbug: 50";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "'humbug:' ###", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_125rt! ,format=="+dcml1.ToString( "'humbug:' ###", nfi1));
       }
     strLoc = "Loc_250ej";
     dcml1 = 45;
     str1 = "00045";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "0000#", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_279eo! ,format=="+dcml1.ToString( "0000#", nfi1));
       }
     strLoc = "Loc_140jj";
     dcml1 = 23;
     str1 = "%2300";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "%#", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_182nq! ,format=="+dcml1.ToString( "%#", nfi1));
       }
     strLoc = "Loc_145hw";
     dcml1 = 34;
     str1 = "test 34 test";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "test #### test", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wj! ,format=="+dcml1.ToString( "test #### test", nfi1));
       }
     strLoc = "Loc_328au";
     dcml1 = 33;
     str1 = "33E+000";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "##.#E+000", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_892wo! ,format=="+dcml1.ToString( "##.#E+000", nfi1));
       }
     strLoc = "Loc_329er";
     dcml1 = 33;
     str1 = "33e+000";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "##.#e+000", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293nw! ,format=="+dcml1.ToString( "##.#e+000", nfi1));
       }
     strLoc = "Loc_347yu";
     dcml1 = 123;
     str1 = "12\\3";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "##\\\\#", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_087he! ,format=="+dcml1.ToString( "##\\\\#", nfi1));
       }
     strLoc = "Loc_378nq";
     dcml1 = 123;
     str1 = "ABC#123";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "'ABC#'##", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128qx! ,strOut=="+dcml1.ToString( "'ABC#'##", nfi1));
       }
     strLoc = "Loc_400ne";
     dcml1 = (123);
     str1 = "00123";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "00000;00;00", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_539tw! ,strOut=="+dcml1.ToString( "00000;000000;00", nfi1));
       }
     strLoc = "Loc_415ue";
     dcml1 = 0;
     str1 = "000";
     iCountTestcases++;
     if(!str1.Equals(dcml1.ToString( "00;00;000", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dcml1.ToString( "00;00;000", nfi1));
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
   Co3686Format_str_nfi cbA = new Co3686Format_str_nfi();
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
