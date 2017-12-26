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
public class Co5100Format_str_obj
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Format(String format, Object obj)";
 public static String s_strTFName        = "Co5100Format_str_obj.";
 public static String s_strTFAbbrev      = "Cb50100";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strFormat = null;
   String strExpectedResult;
   String strResult;
   String str2 = null;
   Object[] objs = new Object[10]; 
   Int16 in2a, in2b, in2c, in2d;
   Int32 in4a, in4b, in4c, in4d;
   Int64 in8a, in8b, in8c, in8d;
   Byte byt1a, byt1b, byt1c, byt1d;
   Double dbl1a, dbl1b, dlb1c, dbl1d;
   Single sngl1a, sngl1b, sngl1c, dngl1d;
   SByte sbyt1a, sbyt1b, sbyt1c, sbyt1d;
   Byte[] bytArr = {Byte.MinValue, 
		    20,
		    50,
		    100,
		    120,
		    Byte.MaxValue
   };
   SByte[] sbytArr = {SByte.MinValue, 
		      -100,
		      -0,
		      0,
		      100,
		      SByte.MaxValue
   };
   Int16[] in2Arr = {Int16.MinValue, 
		     -1000,
		     -50,
		     0,
		     700,
		     Int16.MaxValue
   };
   Int32[] in4Arr = {Int32.MinValue, 
		     -1000,
		     -50,
		     0,
		     700,
		     Int32.MaxValue
   };
   Int64[] in8Arr = {Int64.MinValue, 
		     -1000,
		     -50,
		     0,
		     700,
		     Int64.MaxValue
   };
   Single[] snglArr = {Single.MinValue, 
		       (Single)(-1000.423),
		       (Single)(-50.2388),
		       (Single)(0.43),
		       (Single)(700.5),
		       Single.MaxValue
   };
   Double[] dblArr = {Double.MinValue, 
		      -1000.423,
		      -50.2388,
		      0.43,
		      700.5,
		      Double.MaxValue
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     Console.WriteLine(CultureInfo.CurrentCulture.Name);
     if(CultureInfo.CurrentCulture.Name != "ja" && CultureInfo.CurrentCulture.Name != "ja-JP" && CultureInfo.CurrentCulture.Name != "ar-SA" &&
	!CultureInfo.CurrentCulture.Name.StartsWith("de")) {
     strLoc = "Loc_100aa";
     iCountTestcases++;
     try {
     String.Format(null, objs);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_372wu! , result=="+String.Format( null, objs ));
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_987wu! , exc=="+exc);
     }
     strLoc = "Loc_100bb";
     strFormat = "";
     iCountTestcases++;
     try {
     String.Format(strFormat, null);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_820iq! , result=="+String.Format ( strFormat, null ));
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_893ch! , exc=="+exc);
     }
     strBaseLoc = "Loc_1100aa_";
     strFormat = "ByteTest: {0:C} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: $"+bytArr[ii].ToString()+".00 done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_421wh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100bb_";
     strFormat = "ByteTest: {0:D} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString()+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_422sd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100cc_";
     strFormat = "ByteTest: {0:E} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_327wu! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100dd_";
     strFormat = "ByteTest: {0:F} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_237wy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ee_";
     strFormat = "ByteTest: {0:G} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_828ao! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ff_";
     strFormat = "ByteTest: {0:N} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString()+".00 done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_237sy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100gg_";
     strFormat = "ByteTest: {0:X} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2394h! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100hh_";
     strFormat = "ByteTest: {0:x} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_12usy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100jj_";
     strFormat = "ByteTest: {0:0000000.000} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_4812! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100kk_";
     strFormat = "ByteTest: {0:##########} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_382wu! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ll_";
     strFormat = "ByteTest: {0:##########.###} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_237ay! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100mm_";
     strFormat = "ByteTest: {0:###,###,} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_38yaa! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100nn_";
     strFormat = "ByteTest: {0:%00000} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_417us! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100oo_";
     strFormat = "ByteTest: {0:%000E+00} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "2884w! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100pp_";
     strFormat = "ByteTest: {0:000E-00} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "4582y! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100qq_";
     strFormat = "ByteTest: {0:000e-00} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "47wu! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100rr_";
     strFormat = "ByteTest: {0:000e+00} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "482sy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ss_";
     strFormat = "ByteTest: {0:##\\\\##} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "38ysw! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ss_";
     strFormat = "ByteTest: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "ByteTest: "+bytArr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, bytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "38ysw! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200aa_";
     strFormat = "SByteTest: {0:C} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2837s! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200bb_";
     strFormat = "SByteTest: {0:D} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString()+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_382js! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200cc_";
     strFormat = "SByteTest: {0:E} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_374aa! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200dd_";
     strFormat = "SByteTest: {0:F} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_27sii! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ee_";
     strFormat = "SByteTest: {0:G} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_11173! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ff_";
     strFormat = "SByteTest: {0:N} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString()+".00 done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_287dy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200gg_";
     strFormat = "SByteTest: {0:X} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_27anl! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200hh_";
     strFormat = "SByteTest: {0:x} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_987dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200jj_";
     strFormat = "SByteTest: {0:0000000.000} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_287ao! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200kk_";
     strFormat = "SByteTest: {0:##########} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_48dau! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ll_";
     strFormat = "SByteTest: {0:##########.###} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_r873s! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200mm_";
     strFormat = "SByteTest: {0:###,###,} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_429su! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200nn_";
     strFormat = "SByteTest: {0:%00000} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_417us! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200oo_";
     strFormat = "SByteTest: {0:%000E+00} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "427ww! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200pp_";
     strFormat = "SByteTest: {0:000E-00} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "482se! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200qq_";
     strFormat = "SByteTest: {0:000e-00} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "489ya! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200rr_";
     strFormat = "SByteTest: {0:000e+00} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "127qw! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ss_";
     strFormat = "SByteTest: {0:##\\\\##} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "49say! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ss_";
     strFormat = "SByteTest: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SByteTest: "+sbytArr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, sbytArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "192ja! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300aa_";
     strFormat = "Integer2Test: {0:C} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130we! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300bb_";
     strFormat = "Integer2Test: {0:D} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString()+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300cc_";
     strFormat = "Integer2Test: {0:E} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300dd_";
     strFormat = "Integer2Test: {0:F} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300ee_";
     strFormat = "Integer2Test: {0:G} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300ff_";
     strFormat = "Integer2Test: {0:N} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "N")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300gg_";
     strFormat = "Integer2Test: {0:X} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130gg! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300hh_";
     strFormat = "Integer2Test: {0:x} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130hh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300jj_";
     strFormat = "Integer2Test: {0:0000000.000} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130jj! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300kk_";
     strFormat = "Integer2Test: {0:##########} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130kk! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300ll_";
     strFormat = "Integer2Test: {0:##########.###} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130ll! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300mm_";
     strFormat = "Integer2Test: {0:###,###,} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130mm! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300nn_";
     strFormat = "Integer2Test: {0:%00000} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130nn! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300oo_";
     strFormat = "Integer2Test: {0:%000E+00} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130oo! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300pp_";
     strFormat = "Integer2Test: {0:000E-00} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130pp! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300qq_";
     strFormat = "Integer2Test: {0:000e-00} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130qq! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300rr_";
     strFormat = "Integer2Test: {0:000e+00} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_130rr! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300ss_";
     strFormat = "Integer2Test: {0:##\\\\##} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "130ss! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1300tt_";
     strFormat = "Integer2Test: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < in2Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer2Test: "+in2Arr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, in2Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "130tt! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400aa_";
     strFormat = "Integer4Test: {0:C} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140we! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400bb_";
     strFormat = "Integer4Test: {0:D} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString()+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400cc_";
     strFormat = "Integer4Test: {0:E} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400dd_";
     strFormat = "Integer4Test: {0:F} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400ee_";
     strFormat = "Integer4Test: {0:G} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400ff_";
     strFormat = "Integer4Test: {0:N} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "N")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400gg_";
     strFormat = "Integer4Test: {0:X} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140gg! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400hh_";
     strFormat = "Integer4Test: {0:x} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140hh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400jj_";
     strFormat = "Integer4Test: {0:0000000.000} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140jj! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400kk_";
     strFormat = "Integer4Test: {0:##########} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140kk! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400ll_";
     strFormat = "Integer4Test: {0:##########.###} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140ll! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400mm_";
     strFormat = "Integer4Test: {0:###,###,} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140mm! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400nn_";
     strFormat = "Integer4Test: {0:%00000} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140nn! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400oo_";
     strFormat = "Integer4Test: {0:%000E+00} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140oo! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400pp_";
     strFormat = "Integer4Test: {0:000E-00} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140pp! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400qq_";
     strFormat = "Integer4Test: {0:000e-00} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140qq! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400rr_";
     strFormat = "Integer4Test: {0:000e+00} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_140rr! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400ss_";
     strFormat = "Integer4Test: {0:##\\\\##} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "140ss! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1400tt_";
     strFormat = "Integer4Test: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < in4Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer4Test: "+in4Arr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, in4Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "140tt! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500aa_";
     strFormat = "Integer8Test: {0:C} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150we! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500bb_";
     strFormat = "Integer8Test: {0:D} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString()+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500cc_";
     strFormat = "Integer8Test: {0:E} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500dd_";
     strFormat = "Integer8Test: {0:F} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500ee_";
     strFormat = "Integer8Test: {0:G} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500ff_";
     strFormat = "Integer8Test: {0:N} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "N")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500gg_";
     strFormat = "Integer8Test: {0:X} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150gg! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500hh_";
     strFormat = "Integer8Test: {0:x} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150hh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500jj_";
     strFormat = "Integer8Test: {0:0000000.000} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150jj! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500kk_";
     strFormat = "Integer8Test: {0:##########} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150kk! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500ll_";
     strFormat = "Integer8Test: {0:##########.###} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150ll! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500mm_";
     strFormat = "Integer8Test: {0:###,###,} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150mm! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500nn_";
     strFormat = "Integer8Test: {0:%00000} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150nn! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500oo_";
     strFormat = "Integer8Test: {0:%000E+00} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150oo! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500pp_";
     strFormat = "Integer8Test: {0:000E-00} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150pp! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500qq_";
     strFormat = "Integer8Test: {0:000e-00} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150qq! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500rr_";
     strFormat = "Integer8Test: {0:000e+00} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_150rr! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500ss_";
     strFormat = "Integer8Test: {0:##\\\\##} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "150ss! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1500tt_";
     strFormat = "Integer8Test: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < in8Arr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "Integer8Test: "+in8Arr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, in8Arr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "150tt! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600aa_";
     strFormat = "SingleTest: {0:C} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160we! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600bb_";
     try {
     strFormat = "SingleTest: {0:D} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString()+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     }catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_160bc! , exc=="+exc);
     }
     strBaseLoc = "Loc_1600cc_";
     strFormat = "SingleTest: {0:E} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600dd_";
     strFormat = "SingleTest: {0:F} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600ee_";
     strFormat = "SingleTest: {0:G} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600ff_";
     strFormat = "SingleTest: {0:N} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "N")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600gg_";
     try {
     strFormat = "SingleTest: {0:X} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160gg! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600hh_";
     strFormat = "SingleTest: {0:x} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160hh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_160ha! , exc=="+exc);
     }
     strBaseLoc = "Loc_1600jj_";
     strFormat = "SingleTest: {0:0000000.000} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160jj! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600kk_";
     strFormat = "SingleTest: {0:##########} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160kk! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600ll_";
     strFormat = "SingleTest: {0:##########.###} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160ll! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600mm_";
     strFormat = "SingleTest: {0:###,###,} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160mm! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600nn_";
     strFormat = "SingleTest: {0:%00000} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160nn! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600oo_";
     strFormat = "SingleTest: {0:%000E+00} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160oo! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600pp_";
     strFormat = "SingleTest: {0:000E-00} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160pp! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600qq_";
     strFormat = "SingleTest: {0:000e-00} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160qq! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600rr_";
     strFormat = "SingleTest: {0:000e+00} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_160rr! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600ss_";
     strFormat = "SingleTest: {0:##\\\\##} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "160ss! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1600tt_";
     strFormat = "SingleTest: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < snglArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "SingleTest: "+snglArr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, snglArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "160tt! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700aa_";
     strFormat = "DoubleTest: {0:C} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "C")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170we! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700bb_";
     try {
     strFormat = "DoubleTest: {0:D} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString()+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     }catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_170bc! , exc=="+exc);
     }
     strBaseLoc = "Loc_1700cc_";
     strFormat = "DoubleTest: {0:E} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "E")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700dd_";
     strFormat = "DoubleTest: {0:F} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "F")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700ee_";
     strFormat = "DoubleTest: {0:G} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "G")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700ff_";
     strFormat = "DoubleTest: {0:N} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "N")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700gg_";
     try {
     strFormat = "DoubleTest: {0:X} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "X")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170gg! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700hh_";
     strFormat = "DoubleTest: {0:x} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "x")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170hh! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_170ha! , exc=="+exc);
     }
     strBaseLoc = "Loc_1700jj_";
     strFormat = "DoubleTest: {0:0000000.000} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "0000000.000")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170jj! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700kk_";
     strFormat = "DoubleTest: {0:##########} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "##########")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170kk! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700ll_";
     strFormat = "DoubleTest: {0:##########.###} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "##########.###")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170ll! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700mm_";
     strFormat = "DoubleTest: {0:###,###,} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "###,###,")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170mm! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700nn_";
     strFormat = "DoubleTest: {0:%00000} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "%00000")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170nn! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700oo_";
     strFormat = "DoubleTest: {0:%000E+00} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "%000E+00")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170oo! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700pp_";
     strFormat = "DoubleTest: {0:000E-00} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "000E-00")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170pp! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700qq_";
     strFormat = "DoubleTest: {0:000e-00} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "000e-00")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170qq! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700rr_";
     strFormat = "DoubleTest: {0:000e+00} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "000e+00")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_170rr! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700ss_";
     strFormat = "DoubleTest: {0:##\\\\##} done";
     for(int ii =1 ; ii < dblArr.Length-1 ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "##\\\\##")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "170ss! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1700tt_";
     strFormat = "DoubleTest: {0:0000;-0000;0000} done";
     for(int ii =0 ; ii < dblArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = "DoubleTest: "+dblArr[ii].ToString( "0000;-0000;0000")+" done";
       strResult = String.Format(strFormat, dblArr[ii]);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "170tt! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5100Format_str_obj cbA = new Co5100Format_str_obj();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
