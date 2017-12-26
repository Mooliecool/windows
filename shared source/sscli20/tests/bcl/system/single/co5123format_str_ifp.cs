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
using System.Globalization;
using System;
public class Co5123Format_str_ifp
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Single.ToString(Single value, String format, NumberFormatInfo info)";
 public static readonly String s_strTFName        = "Co5123Format_str_ifp.cs";
 public static readonly String s_strTFAbbrev      = "Co5123";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Single sngl1a = (Single)0;
   String strOut = null;
   String str2 = null;
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencySymbol = "&";  
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   Single [] singTestValues = {Single.MinValue,
			       (Single)(-1000.54),
			       (Single)(-99.339),
			       (Single)(-5.45),
			       (Single)(-0.0),
			       (Single)(0.0),
			       (Single)(5.621),
			       (Single)(13.4223),
			       (Single)(101.81),
			       (Single)(1000.999),
			       Single.MaxValue
   };
   String [] strResultCFormat1 = {"(&340,282,300,000,000,000,000,000,000,000,000,000,000.000)",
				  "(&1,000.540)",
				  "(&99.339)",
				  "(&5.450)",
				  "&0.000",
				  "&0.000",
				  "&5.621",
				  "&13.422",
				  "&101.810",
				  "&1,000.999",
				  "&340,282,300,000,000,000,000,000,000,000,000,000,000.000",
   };
   String [] strResultCFormat2 = {"(&340,282,300,000,000,000,000,000,000,000,000,000,000.0000)",
				  "(&1,000.5400)",
				  "(&99.3390)",
				  "(&5.4500)",
				  "&0.0000",
				  "&0.0000",
				  "&5.6210",
				  "&13.4223",
				  "&101.8100",
				  "&1,000.9990",
				  "&340,282,300,000,000,000,000,000,000,000,000,000,000.0000",
   };
   String [] strResultDFormat1 = {"^3.40283E+38",
				  "^1000",
				  "^99",
				  "^5",
				  "0",
				  "0",
				  "5",
				  "13",
				  "101",
				  "1000",
				  "3.40283E+38"
   };
   String [] strResultDFormat2 = {"^3.40282347E+38",
				  "^1000",
				  "^0099",
				  "^0005",
				  "0000",
				  "0000",
				  "0005",
				  "0013",
				  "0101",
				  "1000",
				  "3.40282347E+38"
   };
   String [] strResultEFormat1 = {"^3.402823E+038",
				  "^1.000540E+003",
				  "^9.933900E+001",
				  "^5.450000E+000",
				  "0.000000E+000",
				  "0.000000E+000",
				  "5.621000E+000",
				  "1.342230E+001",
				  "1.018100E+002",
				  "1.000999E+003",
				  "3.402823E+038",
   };
   String [] strResultEFormat2 = {"^3.4028e+038",
				  "^1.0005e+003",
				  "^9.9339e+001",
				  "^5.4500e+000",
				  "0.0000e+000",
				  "0.0000e+000",
				  "5.6210e+000",
				  "1.3422e+001",
				  "1.0181e+002",
				  "1.0010e+003",
				  "3.4028e+038",
   };
   String [] strResultFFormat1 = {"^340282300000000000000000000000000000000.000",
				  "^1000.540",
				  "^99.339",
				  "^5.450",
				  "0.000",
				  "0.000",
				  "5.621",
				  "13.422",
				  "101.810",
				  "1000.999",
				  "340282300000000000000000000000000000000.000"
   };
   String [] strResultFFormat2 = {"^340282300000000000000000000000000000000.0000",
				  "^1000.5400",
				  "^99.3390",
				  "^5.4500",
				  "0.0000",
				  "0.0000",
				  "5.6210",
				  "13.4223",
				  "101.8100",
				  "1000.9990",
				  "340282300000000000000000000000000000000.0000"
   };
   String [] strResultGFormat1 = {"^3.402823E+38",
				  "^1000.54",
				  "^99.339",
				  "^5.45",
				  "0",
				  "0",
				  "5.621",
				  "13.4223",
				  "101.81",
				  "1000.999",
				  "3.402823E+38"
   };
   String [] strResultGFormat2 = {"^3.403E+38",
				  "^1001",
				  "^99.34",
				  "^5.45",
				  "0",
				  "0",
				  "5.621",
				  "13.42",
				  "101.8",
				  "1001",
				  "3.403E+38"
   };
   String [] strResultNFormat1 = {"^340,282,300,000,000,000,000,000,000,000,000,000,000.000",
				  "^1,000.540",
				  "^99.339",
				  "^5.450",
				  "0.000",
				  "0.000",
				  "5.621",
				  "13.422",
				  "101.810",
				  "1,000.999",
				  "340,282,300,000,000,000,000,000,000,000,000,000,000.000"
   };
   String [] strResultNFormat2 = {"^340,282,300,000,000,000,000,000,000,000,000,000,000.0000",
				  "^1,000.5400",
				  "^99.3390",
				  "^5.4500",
				  "0.0000",
				  "0.0000",
				  "5.6210",
				  "13.4223",
				  "101.8100",
				  "1,000.9990",
				  "340,282,300,000,000,000,000,000,000,000,000,000,000.0000"
   };
   String [] strResultXFormat1 = {"8000000000000000",
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
   String [] strResultXFormat2 = {"8000000000000000",
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
   String [] strResultXFormat3 = {"8000000000000000",
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
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("F",nfi1);
       if(!strOut.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("F4",nfi1);
       if(!strOut.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("G", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("G4", nfi1);
       if(!strOut.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("C", nfi1);
       if(!strOut.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("C4", nfi1);
       if(!strOut.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("E", nfi1);
       if(!strOut.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("e4", nfi1);
       if(!strOut.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("N", nfi1);
       if(!strOut.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < singTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString("N4", nfi1);
       if(!strOut.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" strOut=="+strOut);
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
   bool bResult = false;
   Co5123Format_str_ifp cbA = new Co5123Format_str_ifp();
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
