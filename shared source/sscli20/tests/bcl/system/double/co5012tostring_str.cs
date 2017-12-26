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
public class Co5012Format_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Double.ToString(Double value, String format)";
 public static String s_strTFName        = "Co5012Format_str.cs";
 public static String s_strTFAbbrev      = "Co5012";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer +" ,Possible Bugs="+ s_strActiveBugNums );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Double dbl1a = (Double)0;
   String strOut = null;
   String format;
   Double[] dblTestValues = {Double.MinValue, 
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
   String[] strResultCFormat1 = {"($179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00)", 
                                 "($1,000.54)",
                                 "($99.34)",
                                 "($5.45)",
                                 "$0.00",
                                 "$0.00",
                                 "$5.62",
                                 "$13.42",
                                 "$101.81",
                                 "$1,001.00",
                                 "$179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00",
   };
   String[] strResultCFormat2 = {"($179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000)", 
                                 "($1,000.5400)",
                                 "($99.3390)",
                                 "($5.4500)",
                                 "$0.0000",
                                 "$0.0000",
                                 "$5.6200",
                                 "$13.4223",
                                 "$101.8100",
                                 "$1,000.9990",
                                 "$179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000",
   };
   String[] strResultDFormat1 = {"-1.79769313486232E308", 
				 "-1000",
				 "-99",
				 "-5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "101",
				 "1000",
				 "1.79769313486232E308"
   };
   String[] strResultDFormat2 = {"-1.79769313486232E308", 
				 "-1000",
				 "-0099",
				 "-0005",
				 "0000",
				 "0000",
				 "0005",
				 "0013",
				 "0101",
				 "1000",
				 "1.79769313486232E308"
   };
   String[] strResultEFormat1 = {"-1.797693E+308", 
                                 "-1.000540E+003",
                                 "-9.933900E+001",
                                 "-5.450000E+000",
                                 "0.000000E+000",
                                 "0.000000E+000",
                                 "5.620000E+000",
                                 "1.342230E+001",
                                 "1.018100E+002",
                                 "1.000999E+003",
                                 "1.797693E+308",
   };
   String[] strResultEFormat2 = {"-1.7977e+308", 
                                 "-1.0005e+003",
                                 "-9.9339e+001",
                                 "-5.4500e+000",
                                 "0.0000e+000",
                                 "0.0000e+000",
                                 "5.6200e+000",
                                 "1.3422e+001",
                                 "1.0181e+002",
                                 "1.0010e+003",
                                 "1.7977e+308",
   };
   String[] strResultFFormat1 = {"-179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.00", 
				 "-1000.54",
				 "-99.34",
				 "-5.45",
				 "0.00",
				 "0.00",
				 "5.62",
				 "13.42",
				 "101.81",
				 "1001.00",
				 "179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.00"
   };
   String[] strResultFFormat2 = {"-179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0000", 
				 "-1000.5400",
				 "-99.3390",
				 "-5.4500",
				 "0.0000",
				 "0.0000",
				 "5.6200",
				 "13.4223",
				 "101.8100",
				 "1000.9990",
				 "179769313486232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0000"
   };
   String[] strResultGFormat1 = {"-1.79769313486232E+308", 
				 "-1000.54",
				 "-99.339",
				 "-5.45",
				 "0",
				 "0",
				 "5.62",
				 "13.4223",
				 "101.81",
				 "1000.999",
				 "1.79769313486232E+308"
   };
   String[] strResultGFormat2 = {"-1.798E+308", 
				 "-1001",
				 "-99.34",
				 "-5.45",
				 "0",
				 "0",
				 "5.62",
				 "13.42",
				 "101.8",
				 "1001",
				 "1.798E+308"
   };
   String[] strResultNFormat1 = {"-179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00", 
				 "-1,000.54",
				 "-99.34",
				 "-5.45",
				 "0.00",
				 "0.00",
				 "5.62",
				 "13.42",
				 "101.81",
				 "1,001.00",
				 "179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00"
   };
   String[] strResultNFormat2 = {"-179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000", 
				 "-1,000.5400",
				 "-99.3390",
				 "-5.4500",
				 "0.0000",
				 "0.0000",
				 "5.6200",
				 "13.4223",
				 "101.8100",
				 "1,000.9990",
				 "179,769,313,486,232,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.0000"
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
   do
     {
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F");
       if(!strOut.Equals(strResultFFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87sdgs! {0} \r\n {1} \r\n {2}", i, strResultFFormat1[i], strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F4");
       if(!strOut.Equals(strResultFFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_7935rdg! {0} \r\n {1} \r\n {2}", i, strResultFFormat2[i], strOut);
	 }
       }
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "");
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
       strOut = dblTestValues[i].ToString( "G");
       String strOut2 = ((IConvertible)dblTestValues[i]).ToString(null );
       String strOut3 =  dblTestValues[i].ToString( "");
       if ( ! strOut.Equals( strOut2 ) ||
	    ! strOut.Equals( strOut3 )    )
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_333dt! , i=="+i+" strOut=="+strOut+" strOut2=="+strOut2+" strOut3=="+strOut3);
	 }
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
       strOut = dblTestValues[i].ToString( "G4");
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
       strOut = dblTestValues[i].ToString( "C");
       if(!strOut.Equals(strResultCFormat1[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_2947dg! {0} \r\n {1} \r\n {2}", i, strResultCFormat1[i], strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "C4");
       if(!strOut.Equals(strResultCFormat2[i]))
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" strOut=="+strOut);
	 }
       }
     strLoc = "Loc_160nd";
     iCountTestcases++;
     int ii = 1;
     try {
     strOut = dblTestValues[ii].ToString( "D");
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982ne! , strOut=="+strOut);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_899ye! ,exc=="+exc);
     }
     strLoc = "Loc_170wi";
     iCountTestcases++;
     ii = 1;
     try {
     strOut = dblTestValues[ii].ToString( "D4");
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , strOut=="+strOut);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_128nw! ,exc=="+exc);
     }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < dblTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "E");
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
       strOut = dblTestValues[i].ToString( "e4");
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
       strOut = dblTestValues[i].ToString( "N");
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
       strOut = dblTestValues[i].ToString( "N4");
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
     strOut = dblTestValues[ii].ToString( "X");
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_384wi! , strOut=="+strOut);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_873qq! ,exc=="+exc);
     }
     strLoc = "Loc_230nr";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "X4");
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_837qo! , strOut=="+strOut);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_547ow! ,exc=="+exc);
     }
     strLoc = "Loc_240qi";
     ii = 1;
     iCountTestcases++;
     try {
     strOut = dblTestValues[ii].ToString( "x4");
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_833jd! ,strOut=="+strOut);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_218pq! ,exc=="+exc);
     }
     strLoc = "Loc_248oj";
     dbl1a = (Double)(-50.396);
     strOut = "-humbug: 50.40";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "'humbug:' ###.00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_125tr! ,strOut=="+dbl1a.ToString( "'humbug:' ###.00"));
       }
     strLoc = "Loc_250ej";
     dbl1a = (Double)45.878;
     strOut = "00045.87800";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "0000#.##000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_279eo! ,strOut=="+dbl1a.ToString( "0000#"));
       }
     strLoc = "Loc_129nq";
     dbl1a = (Double)29145.344;
     strOut = "29,145.3";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "#,#.#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_931ee! ,strOut=="+dbl1a.ToString( "#,#.#"));
       }
     strLoc = "Loc_139qi";
     dbl1a = (Double)30000;
     strOut = "30";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "#,")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_289qi! ,strOut=="+dbl1a.ToString( "#,"));
       }
     strLoc = "Loc_140jj";
     dbl1a = (Double)23.899;
     strOut = "%2389.9";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "%#.##")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_182nq! ,strOut=="+dbl1a.ToString( "%#.##"));
       }
     strLoc = "Loc_145hw";
     dbl1a = (Double)(34.34);
     strOut = "test 34.340 test";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "test ####.##0 test")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wj! ,strOut=="+dbl1a.ToString( "test ####.##0 test"));
       }
     strLoc = "Loc_328au";
     dbl1a = (Double)3034.344;
     strOut = "30.3434E+002";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "##.#000E+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_892wo! ,strOut=="+dbl1a.ToString( "##.#000E+000"));
       }
     strLoc = "Loc_329er";
     dbl1a = (Double)3034.322;
     strOut = "30.343e+002";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "##.###e+000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293nw! ,strOut=="+dbl1a.ToString( "##.###e+000"));
       }
     strLoc = "Loc_347yu";
     dbl1a = (Double)1234.3;
     strOut = "12\\34.30";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "##\\\\##.#0")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_087he! ,strOut=="+dbl1a.ToString( "##\\\\##.#0"));
       }
     strLoc = "Loc_378nq";
     dbl1a = (Double)1234.45;
     strOut = "ABC#1234.5";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "'ABC#'##.#")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128qx! ,strOut=="+dbl1a.ToString( "'ABC#'##.#"));
       }
     strLoc = "Loc_400ne";
     dbl1a = (Double)(1234.399);
     strOut = "01234.3990";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00000.0000;00;00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_539tw! ,strOut=="+dbl1a.ToString( "00000.0000;00;00"));
       }
     strLoc = "Loc_412he";
     dbl1a = (Double)(-123.274);
     strOut = "-000123.27";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00;-000000.##;00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_739em! ,strOut=="+dbl1a.ToString( "00;-000000.##;00"));
       }
     strLoc = "Loc_415ue";
     dbl1a = (Double)0.00;
     strOut = "000.000";
     iCountTestcases++;
     if(!strOut.Equals(dbl1a.ToString( "00;00;000.000")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_638kl! ,strOut=="+dbl1a.ToString( "00;00;000.000"));
       }
     strLoc = "Loc_415ue";
     dbl1a = (Double)0.00;
     strOut = "000.000";
     iCountTestcases++;
     try
       {
       if(!strOut.Equals(dbl1a.ToString( "jfdajfkldajf;ajfldsajf;la8098r09809e8wq9e0w8q09ewq89kel;jklejw jfl; flkadsnf;lalkjal;ke ngnalkje; lhlahleth l;ewjl;a ldhflkaj lfk;ahl;fk hakel;jklejw jfl; flkadsnf;lalkjal;ke ngnalkje; lhlahle")))
	 {}
       }
     catch ( Exception  )
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_639kl!");
       }
     strLoc = "Loc_415ue";
     strOut = "0.00";
     iCountTestcases++;
     if(!strOut.Equals((1+(-1)).ToString( "0.00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( "0.00"));
       }
     strOut = "0.00";
     if(!strOut.Equals((1.0+(-1.000)).ToString( "0.00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( "0.00"));
       }
     strOut = "0.00";
     if(!strOut.Equals((2.0000+(-1.000)+(-1.000)).ToString( "0.00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( "0.00"));
       }
     iCountTestcases++;
     dbl1a = 0;
     strOut = "0.000e+00";
     if(!strOut.Equals(dbl1a.ToString( "0.000e+00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( "0.000e+00"));
       }
     dbl1a = 0;
     strOut = "0.000e00";
     if(!strOut.Equals(dbl1a.ToString( "0.000e-00")))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( "0.000e-00"));
       }
     iCountTestcases++;
     dbl1a = 7;
     strOut = "0,007";
     format = "#,##0,000";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_2435wf! ,strOut=="+dbl1a.ToString( format));
       }
     dbl1a = 7;
     strOut = "0,007.00";
     format = "#,##0,000.00";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( format));
       }
     dbl1a = 7;
     strOut = "0,000,007";
     format = "0,000,000";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_3294ursfg! ,strOut=="+dbl1a.ToString( format));
       }
     iCountTestcases++;
     dbl1a = 7;
     strOut = "007";
     format = "0,00";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_2435wf! ,strOut=="+dbl1a.ToString( format));
       }
     iCountTestcases++;
     dbl1a = -5.45e-5;
     strOut = "$0.00";
     format = "c";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = "0.00";
     format = "f";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = "0.00";
     format = "n";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = "-0.01 %";
     format = "p";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = "0.00";
     format = "0.00";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = "0,000.00";
     format = "0,000.00";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     strOut = String.Empty;
     format = "#.##";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347tfg! ,strOut=="+dbl1a.ToString( format));
       }
     iCountTestcases++;
     dbl1a = -45387.1685;
     strOut = "-$045,387.17";
     format = "$#,000,000.00";
     if(!strOut.Equals(dbl1a.ToString( format)))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_2435wf! ,strOut=="+dbl1a.ToString( format));
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
   Co5012Format_str cbA = new Co5012Format_str();
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
