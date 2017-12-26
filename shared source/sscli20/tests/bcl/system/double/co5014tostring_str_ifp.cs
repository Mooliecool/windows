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
public class Co5014Format_str_nfi
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Double.ToString(Double value, String format, NumberFormatInfo info)";
 public static readonly String s_strTFName        = "Co5014Format_str_nfi.cs";
 public static readonly String s_strTFAbbrev      = "Co5014";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Double dbl1a = (Double)0;
   String strOut = null;
   String str2 = null;
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencySymbol = "&";  
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   nfi1.NumberNegativePattern = 1;		
   Double [] dblTestValues = {Double.MinValue,
			      -1000.54,
			      -99.339,
			      -5.45,
			      -0.0,
			      0.0,
			      5.62,
			      13.4223,
			      101.81,
			      1000.999,
			      Double.MaxValue
   };
   String [] strResultCFormat1 = {"(&179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.000)",
				  "(&1,000.540)",
				  "(&99.339)",
				  "(&5.450)",
				  "&0.000",
				  "&0.000",
				  "&5.620",
				  "&13.422",
				  "&101.810",
				  "&1,000.999",
				  "&179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.000",
   };
   String [] strResultCFormat2 = {"(&179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000)",
				  "(&1,000.5400)",
				  "(&99.3390)",
				  "(&5.4500)",
				  "&0.0000",
				  "&0.0000",
				  "&5.6200",
				  "&13.4223",
				  "&101.8100",
				  "&1,000.9990",
				  "&179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000",
   };
   String [] strResultDFormat1 = {"^9223372036854775808",
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
   String [] strResultDFormat2 = {"^1.7976931348623157E308",
				  "^1000",
				  "^0099",
				  "^0005",
				  "0000",
				  "0000",
				  "0005",
				  "0013",
				  "0101",
				  "1000",
				  "1.7976931348623157E308"
   };
   String [] strResultEFormat1 = {"^1.797693E+308",
				  "^1.000540E+003",
				  "^9.933900E+001",
				  "^5.450000E+000",
				  "0.000000E+000",
				  "0.000000E+000",
				  "5.620000E+000",
				  "1.342230E+001",
				  "1.018100E+002",
				  "1.000999E+003",
				  "1.797693E+308",
   };
   String [] strResultEFormat2 = {"^1.7977e+308",
				  "^1.0005e+003",
				  "^9.9339e+001",
				  "^5.4500e+000",
				  "0.0000e+000",
				  "0.0000e+000",
				  "5.6200e+000",
				  "1.3422e+001",
				  "1.0181e+002",
				  "1.0010e+003",
				  "1.7977e+308",
   };
   String [] strResultFFormat1 = {"^179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000",
				  "^1000.540",
				  "^99.339",
				  "^5.450",
				  "0.000",
				  "0.000",
				  "5.620",
				  "13.422",
				  "101.810",
				  "1000.999",
				  "179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000"
   };
   String [] strResultFFormat2 = {"^179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0000",
				  "^1000.5400",
				  "^99.3390",
				  "^5.4500",
				  "0.0000",
				  "0.0000",
				  "5.6200",
				  "13.4223",
				  "101.8100",
				  "1000.9990",
				  "179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0000"
   };
   String [] strResultGFormat1 = {"^1.79769313486232E+308",
				  "^1000.54",
				  "^99.339",
				  "^5.45",
				  "0",
				  "0",
				  "5.62",
				  "13.4223",
				  "101.81",
				  "1000.999",
				  "1.79769313486232E+308"
   };
   String [] strResultGFormat2 = {"^1.798E+308",
				  "^1001",
				  "^99.34",
				  "^5.45",
				  "0",
				  "0",
				  "5.62",
				  "13.42",
				  "101.8",
				  "1001",
				  "1.798E+308"
   };
   String [] strResultNFormat1 = {"^179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.000",
				  "^1,000.540",
				  "^99.339",
				  "^5.450",
				  "0.000",
				  "0.000",
				  "5.620",
				  "13.422",
				  "101.810",
				  "1,000.999",
				  "179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.000"
   };
   String [] strResultNFormat2 = {"^179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000",
				  "^1,000.5400",
				  "^99.3390",
				  "^5.4500",
				  "0.0000",
				  "0.0000",
				  "5.6200",
				  "13.4223",
				  "101.8100",
				  "1,000.9990",
				  "179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000"
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
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F",nfi1);
       if(!strOut.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F4",nfi1);
       if(!strOut.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "G", nfi1);
       if(!strOut.Equals(strResultGFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "G4", nfi1);
       if(!strOut.Equals(strResultGFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "C", nfi1);
       if(!strOut.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "C4", nfi1);
       if(!strOut.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_160nd";
     int ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "D", nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , strOut=="+strOut);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_182jq! ,exc=="+exc);
     }
     strLoc = "Loc_170eu";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "D4", nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , strOut=="+strOut);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_278nq!, exc=="+exc);
     }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "E", nfi1);
       if(!strOut.Equals(strResultEFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "e4", nfi1);
       if(!strOut.Equals(strResultEFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "N", nfi1);
       if(!strOut.Equals(strResultNFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "N4", nfi1);
       if(!strOut.Equals(strResultNFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" strOut=="+strOut);
	 }
       }
     strLoc = "Loc_220aa";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "X", nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_384wi! , strOut=="+strOut);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_238qy! ,exc=="+exc);
     }
     strLoc = "Loc_230nr";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "X4", nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_837qo! , strOut=="+strOut);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_349je! ,exc=="+exc);
     }
     strLoc = "Loc_240qi";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "x4", nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_833jd! , strOut=="+strOut);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_731ey! ,exc=="+exc);
     }
     strLoc = "Loc_248oj";
     dbl1a = (Double)(-50.34);
     strOut = "^humbug: 50.340";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "'humbug:' ###.000", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_125jw! ,strOut=="+dbl1a.ToString( "'humbug:' ###.000", nfi1));
       }
     strLoc = "Loc_250ej";
     dbl1a = (Double)45.623;
     strOut = "00045.62";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "0000#.##", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_279eo! ,strOut=="+dbl1a.ToString( "0000#.##", nfi1));
       }
     strLoc = "Loc_129nq";
     dbl1a = (Double)29145.123;
     strOut = "29,145.1230";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "#,#.0000", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_931ee! ,strOut=="+dbl1a.ToString( "#,#.0000", nfi1));
       }
     strLoc = "Loc_145hw";
     dbl1a = (Double)(34.5129);
     strOut = "test 34.513 test";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "test ####.### test", nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wj! ,strOut=="+dbl1a.ToString( "test ####.### test", nfi1));
       }
     strLoc = "Loc_328au";
     dbl1a = (Double)3034.34;
     strOut = "30.343E+002";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "##.###E+000",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_892wo! ,strOut=="+dbl1a.ToString( "##.###E+000",nfi1));
       }
     strLoc = "Loc_378nq";
     dbl1a = (Double)1234.668;
     strOut = "ABC#1234.67";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "'ABC#'##.##",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128qx! ,strOut=="+dbl1a.ToString( "'ABC#'##.##",nfi1));
       }
     strLoc = "Loc_400ne";
     dbl1a = (Double)(1234.2);
     strOut = "01234.200";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00000.000;00;00",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_539tw! ,strOut=="+dbl1a.ToString( "00000.000;00;00",nfi1));
       }
     strLoc = "Loc_412he";
     dbl1a = (Double)(-123.41);
     strOut = "^000123.4";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00;^000000.#;00",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_739em! ,strOut=="+dbl1a.ToString( "00;^000000.#;00", nfi1));
       }
     strLoc = "Loc_415ue";
     dbl1a = (Double)0;
     strOut = "000.00";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00;00;000.00",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "00;00;000.00", nfi1));
       }
     strLoc = "Loc_415ue";
     iCountTestcases++;
     dbl1a = 20719.0;
     strOut = "20719.00";
     nfi1 = new NumberFormatInfo();
     nfi1.NumberGroupSizes = new int[]{0};
     if(!strOut.Equals(dbl1a.ToString( "n",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "n", nfi1));
       }
     dbl1a = 20719.0;
     strOut = "20719.0000000000";
     nfi1 = new NumberFormatInfo();
     nfi1.NumberGroupSizes = new int[]{0};
     nfi1.NumberDecimalDigits =10;
     if(!strOut.Equals(dbl1a.ToString( "n",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "n", nfi1));
       }
     dbl1a = 20719.0;
     strOut = "2,0,7,1,9.0000000000";
     nfi1 = new NumberFormatInfo();
     nfi1.NumberGroupSizes = new int[]{1};
     nfi1.NumberDecimalDigits =10;
     if(!strOut.Equals(dbl1a.ToString( "n",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "n", nfi1));
       }
     strLoc = "Loc_415ue";
     iCountTestcases++;
     dbl1a = 0.736;
     strOut = "736A";
     nfi1 = new NumberFormatInfo();
     nfi1.PerMilleSymbol = "A";
     if(!strOut.Equals(dbl1a.ToString( "#00\u2030",nfi1)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "n", nfi1));
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
   Co5014Format_str_nfi cbA = new Co5014Format_str_nfi();
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
