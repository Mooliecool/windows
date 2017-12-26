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
public class Co3712Format_StrObjArrIFP
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Format(String format, Object obj[], IFormatProvider fp)"; 
 public static String s_strTFName        = "Co3712Format_StrObjArrIFP.";
 public static String s_strTFAbbrev      = "Co3712";
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
		     (Int32)20000
   };
   Object[] objs2 = {(Int64)4999, 
		     (Single)(-123.388),
		     (Double)3499.298
   };
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
   String[] strArr = {"String1", 
		      "String2",
		      "String3",
		      "String4",
		      "String5",
		      "String6"
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.CurrencySymbol = "&";  
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   try {
   do
     {
     strLoc = "Loc_100aa";
     iCountTestcases++;
     try {
     String.Format(   nfi1 ,null ,objs1);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_372wu! , result=="+String.Format( null, objs1 ));
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_987wu! , exc=="+exc);
     }
     strLoc = "Loc_100bb";
     strFormat = ""; 
     iCountTestcases++;
     try {
     String.Format(nfi1 ,strFormat, null);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_820iq! , result=="+String.Format ( strFormat, null ));
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_893ch! , exc=="+exc);
     }
     strBaseLoc = "Loc_1100aa_";
     strFormat = " ByteT: {0:C} SByteT: {1:C} Int2: {2:C} Int4: {3:c} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strLoc = strBaseLoc + ii.ToString();
       strExpectedResult = " ByteT: "+bytArr[ii].ToString("C", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString("C", nfi1)+
	 " Int2: "+in2Arr[ii].ToString("C", nfi1)+
	 " Int4: "+in4Arr[ii].ToString("C", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110aa! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100bb_";
     strFormat = " ByteT: {0:d} SByteT: {1:D} Int2: {2:d} Int4: {3:D} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString("d", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "D", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "D", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "d", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110bb! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100cc_";
     strFormat = " ByteT: {0:e} SByteT: {1:E} Int2: {2:E} Int4: {3:e} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString("e", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "E", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "E", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "e", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110cc! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100dd_";
     strFormat = " ByteT: {0:f} SByteT: {1:F} Int2: {2:F} Int4: {3:f} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString( "F", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "F", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "f", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "f", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110dd! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ee_";
     strFormat = " ByteT: {0:g} SByteT: {1:G} Int2: {2:G} Int4: {3:g} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString( "G", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "G", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "g", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "g", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100ff_";
     strFormat = " ByteT: {0:n} SByteT: {1:N} Int2: {2:N} Int4: {3:n} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString( "N", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "N", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "n", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "n", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110ee! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1100gg_";
     strFormat = " ByteT: {0:x} SByteT: {1:X} Int2: {2:x} Int4: {3:X} done";
     for(int ii =0 ; ii < bytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs1[0] = ((Byte)bytArr[ii]);
       objs1[1] = ((SByte)sbytArr[ii]);
       objs1[2] = ((Int16)in2Arr[ii]);
       objs1[3] = ((Int32)in4Arr[ii]);
       strExpectedResult = " ByteT: "+bytArr[ii].ToString( "x", nfi1)+
	 " SByteT: "+sbytArr[ii].ToString( "X", nfi1)+
	 " Int2: "+in2Arr[ii].ToString( "x", nfi1)+
	 " Int4: "+in4Arr[ii].ToString( "X", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs1);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_110ff! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200aa_";
     strFormat = " Int8: {0:C} Single: {1:C} Double: {2:c} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = ((Int64)in8Arr[ii]);
       objs2[1] = ((Single)snglArr[ii]);
       objs2[2] = ((Double)dblArr[ii]);
       strExpectedResult = " Int8: "+in8Arr[ii].ToString( "C", nfi1)+
	 " Single: "+snglArr[ii].ToString( "c", nfi1)+
	 " Double: "+dblArr[ii].ToString( "C", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2837s! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200cc_";
     strFormat = " Int8: {0:e} Single: {1:E} Double: {2:e} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = ((Int64)in8Arr[ii]);
       objs2[1] = ((Single)snglArr[ii]);
       objs2[2] = ((Double)dblArr[ii]);
       strExpectedResult = " Int8: "+in8Arr[ii].ToString( "e", nfi1)+
	 " Single: "+snglArr[ii].ToString( "E", nfi1)+
	 " Double: "+dblArr[ii].ToString( "e", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_374aa! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200dd_";
     strFormat = " Int8: {0:f} Single: {1:F} Double: {2:f} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = ((Int64)in8Arr[ii]);
       objs2[1] = ((Single)snglArr[ii]);
       objs2[2] = ((Double)dblArr[ii]);
       strExpectedResult = " Int8: "+in8Arr[ii].ToString( "F", nfi1)+
	 " Single: "+snglArr[ii].ToString( "f", nfi1)+
	 " Double: "+dblArr[ii].ToString( "F", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_27sii! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ee_";
     strFormat = " Int8: {0:g} Single: {1:G} Double: {2:g} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = ((Int64)in8Arr[ii]);
       objs2[1] = ((Single)snglArr[ii]);
       objs2[2] = ((Double)dblArr[ii]);
       strExpectedResult = " Int8: "+in8Arr[ii].ToString( "G", nfi1)+
	 " Single: "+snglArr[ii].ToString( "G", nfi1)+
	 " Double: "+dblArr[ii].ToString( "g", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_11173! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strBaseLoc = "Loc_1200ff_";
     strFormat = " Int8: {0:n} Single: {1:N} Double: {2:n} done";
     for(int ii =0 ; ii < sbytArr.Length ; ii++ )
       {
       strLoc = strBaseLoc + ii.ToString();
       objs2[0] = ((Int64)in8Arr[ii]);
       objs2[1] = ((Single)snglArr[ii]);
       objs2[2] = ((Double)dblArr[ii]);
       strExpectedResult = " Int8: "+in8Arr[ii].ToString( "n", nfi1)+
	 " Single: "+snglArr[ii].ToString( "N", nfi1)+
	 " Double: "+dblArr[ii].ToString( "n", nfi1)+" done";
       strResult = String.Format(nfi1 ,strFormat, objs2);
       iCountTestcases++;
       if(!strResult.Equals(strExpectedResult))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_287dy! , strResult=="+strResult+" strExpected=="+strExpectedResult);
	 }
       }
     strLoc = "Loc_130aa";
     strFormat = "String1: {0:} String2: {1:} String3: {2:} String4: {3:} String5: {4:}";
     strExpectedResult = "String1: "+strArr[0]+" String2: "+strArr[1]+" String3: "+strArr[2]+" String4: "+strArr[3]+" String5: "+strArr[4];
     iCountTestcases++;
     try {
     strResult = String.Format(nfi1 ,strFormat, (Object[])strArr); 
     if(!strResult.Equals(strExpectedResult))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_381wi! strResult=="+strResult);
       }
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_482ue! , exc=="+exc);
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
   Co3712Format_StrObjArrIFP cbA = new Co3712Format_StrObjArrIFP();
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
