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
using GenStrings;
public class Co5050Format_objs
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Format(String format, Object obj)";
 public static String s_strTFName        = "Co5050Format_objs.";
 public static String s_strTFAbbrev      = "Co5050";
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
   Object[] objs1 = {(Byte)5, 
		     (SByte)(-20),
		     (Int16)400,
		     (Int32)20000,
		     (Int64)4999,
   };
   Object[] objs2 = {(Single)(-123.388), 
		     (Double)3499.298
   };
   Object[] bytArr = {Byte.MinValue, 
		      (Byte)20,
		      (Byte)50,
		      (Byte)100,
		      (Byte)120,
		      (Byte.MaxValue)
   };
   String[] bytCExpected = {"$0.00", 
			    "$20.00",
			    "$50.00",
			    "$100.00",
			    "$120.00",
			    "$255.00",
   };
   String[] bytDExpected = {"0", 
			    "20",
			    "50",
			    "100",
			    "120",
			    "255",
   };
   String[] bytEExpected = {"0.000000e+000", 
			    "2.000000e+001",
			    "5.000000e+001",
			    "1.000000e+002",
			    "1.200000e+002",
			    "2.550000e+002",
   };
   String[] bytFExpected = {"0.00", 
			    "20.00",
			    "50.00",
			    "100.00",
			    "120.00",
			    "255.00",
   };
   String[] bytNExpected = {"0.00", 
			    "20.00",
			    "50.00",
			    "100.00",
			    "120.00",
			    "255.00",
   };
   String[] bytXExpected = {"0", 
			    "14",
			    "32",
			    "64",
			    "78",
			    "ff",
   };
   Object[] sbytArr = {SByte.MinValue, 
		       (SByte)(-100),
		       (SByte)(-0),
		       (SByte)0,
		       (SByte)100,
		       SByte.MaxValue
   };
   String[] sbytCExpected = {"($128.00)", 
			     "($100.00)",
			     "$0.00",
			     "$0.00",
			     "$100.00",
			     "$127.00",
   };
   String[] sbytDExpected = {"-128", 
			     "-100",
			     "0",
			     "0",
			     "100",
			     "127",
   };
   String[] sbytEExpected = {"-1.280000E+002", 
			     "-1.000000E+002",
			     "0.000000E+000",
			     "0.000000E+000",
			     "1.000000E+002",
			     "1.270000E+002",
   };
   String[] sbytFExpected = {"-128.00", 
			     "-100.00",
			     "0.00",
			     "0.00",
			     "100.00",
			     "127.00",
   };
   String[] sbytNExpected = {"-128.00", 
			     "-100.00",
			     "0.00",
			     "0.00",
			     "100.00",
			     "127.00",
   };
   String[] sbytXExpected = {"80", 
			     "9C",
			     "0",
			     "0",
			     "64",
			     "7F",
   };
   Object[] in2Arr = {Int16.MinValue, 
		      (Int16)(-1000),
		      (Int16)(-50),
		      (Int16)0,
		      (Int16)700,
		      (Int16)Int16.MaxValue
   };
   String[] in2CExpected = {"($32,768.00)", 
			    "($1,000.00)",
			    "($50.00)",
			    "$0.00",
			    "$700.00",
			    "$32,767.00",
   };
   String[] in2DExpected = {"-32768", 
			    "-1000",
			    "-50",
			    "0",
			    "700",
			    "32767",
   };
   String[] in2EExpected = {"-3.276800E+004", 
			    "-1.000000E+003",
			    "-5.000000E+001",
			    "0.000000E+000",
			    "7.000000E+002",
			    "3.276700E+004",
   };
   String[] in2FExpected = {"-32768.00", 
			    "-1000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "32767.00",
   };
   String[] in2NExpected = {"-32,768.00", 
			    "-1,000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "32,767.00",
   };
   String[] in2XExpected = {"8000", 
			    "fc18",
			    "ffce",
			    "0",
			    "2bc",
			    "7fff",
   };
   Object[] in4Arr = {Int32.MinValue, 
		      -1000,
		      -50,
		      0,
		      700,
		      Int32.MaxValue
   };
   String[] in4CExpected = {"($2,147,483,648.00)", 
			    "($1,000.00)",
			    "($50.00)",
			    "$0.00",
			    "$700.00",
			    "$2,147,483,647.00",
   };
   String[] in4DExpected = {"-2147483648", 
			    "-1000",
			    "-50",
			    "0",
			    "700",
			    "2147483647",
   };
   String[] in4EExpected = {"-2.147484e+009", 
			    "-1.000000e+003",
			    "-5.000000e+001",
			    "0.000000e+000",
			    "7.000000e+002",
			    "2.147484e+009",
   };
   String[] in4FExpected = {"-2147483648.00", 
			    "-1000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "2147483647.00",
   };
   String[] in4NExpected = {"-2,147,483,648.00", 
			    "-1,000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "2,147,483,647.00",
   };
   String[] in4XExpected = {"80000000", 
			    "FFFFFC18",
			    "FFFFFFCE",
			    "0",
			    "2BC",
			    "7FFFFFFF",
   };
   Object[] in8Arr = {Int64.MinValue, 
		      (Int64)(-1000),
		      (Int64)(-50),
		      (Int64)0,
		      (Int64)700,
		      (Int64)Int64.MaxValue
   };
   String[] in8CExpected = {"($9,223,372,036,854,775,808.00)", 
			    "($1,000.00)",
			    "($50.00)",
			    "$0.00",
			    "$700.00",
			    "$9,223,372,036,854,775,807.00",
   };
   String[] in8DExpected = {"-9223372036854775808", 
			    "-1000",
			    "-50",
			    "0",
			    "700",
			    "9223372036854775807",
   };
   String[] in8EExpected = {"-9.223372E+018", 
			    "-1.000000E+003",
			    "-5.000000E+001",
			    "0.000000E+000",
			    "7.000000E+002",
			    "9.223372E+018",
   };
   String[] in8FExpected = {"-9223372036854775808.00", 
			    "-1000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "9223372036854775807.00",
   };
   String[] in8NExpected = {"-9,223,372,036,854,775,808.00", 
			    "-1,000.00",
			    "-50.00",
			    "0.00",
			    "700.00",
			    "9,223,372,036,854,775,807.00",
   };
   String[] in8XExpected = {"8000000000000000", 
			    "FFFFFFFFFFFFFC18",
			    "FFFFFFFFFFFFFFCE",
			    "0",
			    "2BC",
			    "7FFFFFFFFFFFFFFF",
   };
   Object[] snglArr = {Single.MinValue, 
		       (Single)(-1000.423),
		       (Single)(-5.45E-5),
		       (Single)(0.43),
		       (Single)(700.5),
		       Single.MaxValue
   };
   String[] sglCExpected = {"($340,282,347,000,000,000,000,000,000,000,000,000,000.00)", 
			    "($1,000.42)",
			    "$0.00",
			    "$0.43",
			    "$700.50",
			    "$340,282,347,000,000,000,000,000,000,000,000,000,000.00",
   };
   String[] sglEExpected = {"-3.402823E+038", 
			    "-1.000423E+003",
			    "-5.450000E-005",
			    "4.300000E-001",
			    "7.005000E+002",
			    "3.402823E+038",
   };
   String[] sglFExpected = {"-340282347000000000000000000000000000000.00", 
			    "-1000.42",
			    "0.00",
			    "0.43",
			    "700.50",
			    "340282347000000000000000000000000000000.00",
   };
   Object[] dblArr = {Double.MinValue, 
		      -1000.423,
		      -5.45E-5,
		      0.43,
		      700.5,
		      Double.MaxValue
   };
   String[] dblCExpected = {"($179,769,313,486,231,570,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00)", 
			    "($1,000.42)",
			    "$0.00",
			    "$0.43",
			    "$700.50",
			    "$179,769,313,486,231,570,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000.00",
   };
   String[] dblEExpected = {"-1.797693e+308", 
			    "-1.000423e+003",
			    "-5.450000e-005",
			    "4.300000e-001",
			    "7.005000e+002",
			    "1.797693e+308",
   };
   String[] dblFExpected = {"-179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.00", 
			    "-1000.42",
			    "0.00",
			    "0.43",
			    "700.50",
			    "179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.00",
   };
   String[] strArr = {"String1", 
		      "String2",
		      "String3",
		      "String4",
		      "String5",
		      "String6"
   };
   try {
   do
     {
     Console.WriteLine(CultureInfo.CurrentCulture.EnglishName);
     Console.WriteLine(CultureInfo.CurrentCulture.Name);
     if(CultureInfo.CurrentCulture.Name != "ja" && CultureInfo.CurrentCulture.Name != "ja-JP" && CultureInfo.CurrentCulture.Name != "ar-SA" &&
	!CultureInfo.CurrentCulture.Name.StartsWith("de")) {
     strLoc = "Loc_100aa";
     Console.WriteLine("Inside.................");		
     iCountTestcases++;
     try {
     String.Format(null, objs1);
     iCountErrors++;
     printerr ( "Err_372wu! , result=="+String.Format( null, objs1 ));
     } catch (ArgumentException ){}
     catch (Exception exc) {
     iCountErrors++;
     printerr( "Err_987wu! , exc=="+exc);
     }
     strLoc = "Loc_100bb";
     strFormat = ""; 
     iCountTestcases++;
     try {
     String.Format(strFormat, null);
     iCountErrors++;
     printerr( "Err_820iq! , result=="+String.Format ( strFormat, null ));
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     printerr( "Err_893ch! , exc=="+exc);
     }
     strBaseLoc = "Loc_1100aa_";
     strFormat = " ByteT: {0:C} SByteT: {1:C} Int2: {2:C} Int4: {3:c} Int8: {4:c} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = " ByteT: "+bytCExpected[ii]+
	 " SByteT: "+sbytCExpected[ii]+
	 " Int2: "+in2CExpected[ii]+
	 " Int4: "+in4CExpected[ii]+
	 " Int8: "+in8CExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110aa_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100bb_";
     strFormat = " ByteT: {0:d} SByteT: {1:D} Int2: {2:d} Int4: {3:D} Int8: {4:D} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytDExpected[ii]+
	 " SByteT: "+sbytDExpected[ii]+
	 " Int2: "+in2DExpected[ii]+
	 " Int4: "+in4DExpected[ii]+
	 " Int8: "+in8DExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110bb_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100cc_";
     strFormat = " ByteT: {0:e} SByteT: {1:E} Int2: {2:E} Int4: {3:e} Int8: {4:E} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytEExpected[ii]+
	 " SByteT: "+sbytEExpected[ii]+
	 " Int2: "+in2EExpected[ii]+
	 " Int4: "+in4EExpected[ii]+
	 " Int8: "+in8EExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110cc_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100dd_";
     strFormat = " ByteT: {0:f} SByteT: {1:F} Int2: {2:F} Int4: {3:f} Int8: {4:F} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytFExpected[ii]+
	 " SByteT: "+sbytFExpected[ii]+
	 " Int2: "+in2FExpected[ii]+
	 " Int4: "+in4FExpected[ii]+
	 " Int8: "+in8FExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110dd_"+ii+"!");
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ee_";
     strFormat = " ByteT: {0:g} SByteT: {1:G} Int2: {2:G} Int4: {3:g} Int8: {4:g} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytArr[ii].ToString()+
	 " SByteT: "+sbytArr[ii].ToString()+
	 " Int2: "+in2Arr[ii].ToString()+
	 " Int4: "+in4Arr[ii].ToString()+
	 " Int8: "+in8Arr[ii].ToString()+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr("Err_110ee!");
	 printinfo( "strResult=="+strResult);
	 printinfo( "strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ff_";
     strFormat = " ByteT: {0:n} SByteT: {1:N} Int2: {2:N} Int4: {3:n} Int8: {4:N} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytNExpected[ii]+
	 " SByteT: "+sbytNExpected[ii]+
	 " Int2: "+in2NExpected[ii]+
	 " Int4: "+in4NExpected[ii]+
	 " Int8: "+in8NExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110ff_"+ii);
	 printinfo( "strResult=="+strResult);
	 printinfo( "strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100gg_";
     strFormat = " ByteT: {0:x} SByteT: {1:X} Int2: {2:x} Int4: {3:X} Int8: {4:X} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = bytArr[ii];
       objs1[1] = sbytArr[ii];
       objs1[2] = in2Arr[ii];
       objs1[3] = in4Arr[ii];
       objs1[4] = in8Arr[ii];
       strExpectedResult = " ByteT: "+bytXExpected[ii]+
	 " SByteT: "+sbytXExpected[ii]+
	 " Int2: "+in2XExpected[ii]+
	 " Int4: "+in4XExpected[ii]+
	 " Int8: "+in8XExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_110gg_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200cc_";
     strFormat = " Single: {0:E} Double: {1:e} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = snglArr[ii];
       objs2[1] = dblArr[ii];
       strExpectedResult = " Single: "+sglEExpected[ii]+
	 " Double: "+dblEExpected[ii]+
	 " done";
       strResult = String.Format(strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_374aa_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ee_";
     strFormat = " Single: {0:G} Double: {1:G} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = snglArr[ii];
       objs2[1] = dblArr[ii];
       strExpectedResult = " Single: "+snglArr[ii].ToString()+
	 " Double: "+dblArr[ii].ToString()+
	 " done";
       strResult = String.Format(strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_11173_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ff_";
     strFormat = " Single: {0:N} Double: {1:n} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = snglArr[ii];
       objs2[1] = dblArr[ii];
       strExpectedResult = " Single: "+((Single)snglArr[ii]).ToString( "N")+
	 " Double: "+((Double)dblArr[ii]).ToString("n")+
	 " done";
       strResult = String.Format(strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 printerr( "Err_287dy_"+ii);
	 printinfo("strResult=="+strResult);
	 printinfo("strExpect=="+strExpectedResult);
	 }
       }
     strLoc = "Loc_130aa";
     strFormat = "String1: {0:} String2: {1:} String3: {2:} String4: {3:} String5: {4:}";
     strExpectedResult = "String1: "+strArr[0]+" String2: "+strArr[1]+" String3: "+strArr[2]+" String4: "+strArr[3]+" String5: "+strArr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, (Object[])strArr); 
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_381wi! strResult=="+strResult);
       }
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Err_482ue! , exc=="+exc.ToString());
     }
     strLoc = "Loc_472yd";
     strFormat = "1: {0:} 2: {1:} 3: {2:} 4: {3:} 5: {4:}";
     strExpectedResult = "1: "+bytArr[0]+" 2: "+bytArr[1]+" 3: "+bytArr[2]+" 4: "+bytArr[3]+" 5: "+bytArr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, bytArr);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_982uq! strResult=="+strResult);
       }
     } catch (Exception exc) {iCountErrors++;printerr( "Err_27hjy, exc=="+exc);}
     strLoc = "Loc_472yd";
     strFormat = "1: {0:G} 2: {1:G} 3: {2:G} 4: {3:G} 5: {4:G}";
     strExpectedResult = "1: "+in2Arr[0]+" 2: "+in2Arr[1]+" 3: "+in2Arr[2]+" 4: "+in2Arr[3]+" 5: "+in2Arr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, in2Arr);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_371jd! strResult=="+strResult);
       }
     } catch (Exception exc) {iCountErrors++;printerr( "Err_287sy, exc=="+exc);}
     strLoc = "Loc_472yd";
     strFormat = "1: {0:G} 2: {1:G} 3: {2:G} 4: {3:G} 5: {4:G}";
     strExpectedResult = "1: "+in4Arr[0]+" 2: "+in4Arr[1]+" 3: "+in4Arr[2]+" 4: "+in4Arr[3]+" 5: "+in4Arr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, in4Arr);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_982uq! strResult=="+strResult);
       }
     } catch (Exception exc) {iCountErrors++;printerr( "Err_27iuo, exc=="+exc);}
     strLoc = "Loc_472yd";
     strFormat = "1: {0:G} 2: {1:G} 3: {2:G} 4: {3:G} 5: {4:G}";
     strExpectedResult = "1: "+in8Arr[0]+" 2: "+in8Arr[1]+" 3: "+in8Arr[2]+" 4: "+in8Arr[3]+" 5: "+in8Arr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, in8Arr);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_982uq! strResult=="+strResult);
       }
     } catch (Exception exc) {iCountErrors++;printerr( "Err_2787h, exc=="+exc);}
     strLoc = "Loc_472yd";
     strFormat = "1: {0:G} 2: {1:G} 3: {2:G} 4: {3:G} 5: {4:G}";
     strExpectedResult = "1: "+dblArr[0]+" 2: "+dblArr[1]+" 3: "+dblArr[2]+" 4: "+dblArr[3]+" 5: "+dblArr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(strFormat, dblArr);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_982uq! strResult=="+strResult);
       }
     } catch (Exception exc) {iCountErrors++;printerr( "Err_27kji, exc=="+exc);}
     Single sl = (Single)5e-4f;
     strFormat = "{0:G} Single";
     strExpectedResult = "0.0005 Single";
     iCountTestcases++;
     strResult = String.Format(strFormat, sl);
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       printerr( "Err_3847sy! ,  strResult=="+strResult);
       }
     }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   printerr(" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void printerr ( String err )
   {
   Console.Error.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public static void printinfo (String info)
   {
   Console.Error.WriteLine("EXTENDEDINFO: "+info);
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5050Format_objs cbA = new Co5050Format_objs();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   printerr( "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.Error.WriteLine (s_strTFPath+s_strTFName);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
