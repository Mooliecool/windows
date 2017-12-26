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
using System.Collections; 
public class Co3742CreateInstance_type_i
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.CreateInstance(Type, int)";
 public static String s_strTFName        = "Co3742CreateInstance_type_i";
 public static String s_strTFAbbrev      = "Co3742";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Type tpValue;
   int[] iArr;
   Array objArr;
   Hashtable hshArrayValue;
   ArrayList alst1 = new ArrayList();
   Array arComplex;
   int[] iAr1 = null;
   Object o1;
   int[] iAr2;
   int nNumberOfArrays = 10;
   try
     {
     do
       {
       Console.WriteLine ("p1");
       tpValue = typeof(int);
       iArr = (int[])Array.CreateInstance(tpValue, 10);
       ++iCountTestcases;
       if ( iArr.Rank != 1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_643ag!  iArr.GetRank()=="+ iArr.Rank);
	 }
       ++iCountTestcases;
       if ( iArr.GetLength(0) != 10 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_320ye!  iArr.GetLength(0)=="+ iArr.GetLength(0) );
	 }
       ++iCountTestcases;
       if ( iArr.GetLowerBound(0) != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sf!  iArr.GetLowerBound(0)=="+ iArr.GetLowerBound(0) );
	 }
       ++iCountTestcases;
       if ( iArr.GetUpperBound(0) != 9 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_268aq!  iArr.GetUpperBound(0)=="+ iArr.GetUpperBound(0) );
	 }
       for(int ii=iArr.GetLowerBound(0); ii <= iArr.GetUpperBound(0); ii++)
	 iArr.SetValue(ii, ii);
       ++iCountTestcases;
       for(int ii=0; ii < 10; ii++)
	 {
	 if(!iArr.GetValue(ii).Equals(ii))
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_630jm_" + ii + ", ii==" + iArr.GetValue(ii));
	   }
	 }
       Console.WriteLine ("p2");
       tpValue = typeof(int[]);
       arComplex = Array.CreateInstance(tpValue, nNumberOfArrays);
       for(int j=10; j < (10 + nNumberOfArrays); j++)
	 {
	 iAr1 = new int[j];
	 for(int i=0;i<j;i++)
	   iAr1[i]=i;
	 arComplex.SetValue(iAr1, 0);
	 o1 = arComplex.GetValue(0);
	 iAr2 = (int[])o1;
	 for(int i=0; i < iAr2.Length; i++)
	   {
	   ++iCountTestcases;
	   if(!iAr1.GetValue(i).Equals(iAr2.GetValue(i)))
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + " Err_786sl_" + i + ", iAr1.GetValue(i)=="+ iAr1.GetValue(i) + ", iAr2.GetValue(i)=="+ iAr2.GetValue(i));
	     }
	   }
	 }
       hshArrayValue = new Hashtable();
       GoFillSomeTestValues(hshArrayValue);
       Console.WriteLine ("p3");
       for(int ii=0; ii < ClassTypes.Length; ii++)
	 {
	 strLoc = "Loc_653fdf_" + ii;
	 try
	   {
	   tpValue = ClassTypes[ii];
	   objArr  = Array.CreateInstance(tpValue, 10);
	   strLoc = "Loc_105sv_" + ii;
	   ++iCountTestcases;
	   if ( objArr.Rank != 1 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_762sw_" + ii + ", objArr.GetRank()=="+ objArr.Rank);
	     }
	   ++iCountTestcases;
	   if ( objArr.GetLength(0) != 10 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_874sg_" + ii + ",  objArr.GetLength(0)=="+ objArr.GetLength(0) );
	     }
	   ++iCountTestcases;
	   if ( objArr.GetLowerBound(0) != 0 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_763sr_" + ii + ",  objArr.GetLowerBound(0)=="+ objArr.GetLowerBound(0) );
	     }
	   ++iCountTestcases;
	   if ( objArr.GetUpperBound(0) != 9 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_623ov_" + ii + ", objArr.GetUpperBound(0)=="+ objArr.GetUpperBound(0) );
	     }
	   if(hshArrayValue.ContainsKey(ClassTypes[ii].Name))
	     {
	     strLoc = "E634sa_" + ii ;
	     alst1 = (ArrayList)hshArrayValue[ClassTypes[ii].Name];
	     for(int kk = 0; kk < alst1.Count; kk++)
	       {
	       strLoc = "E742da_" + ii + "_" + kk;
	       objArr.SetValue(alst1[kk], kk);
	       }
	     for(int kk = 0; kk < alst1.Count; kk++)
	       {
	       strLoc = "E124se_" + ii + "_" + kk;
	       ++iCountTestcases;
	       if (!objArr.GetValue(kk).Equals(alst1[kk]))
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev + ", Err_673sa_" + ii + "_" + kk + ", objArr.GetValue(kk)==" + objArr.GetValue(kk) + ", alst1[kk]==" + alst1[kk]);
		 }
	       }
	     }
	   strLoc = "Loc_738fdfsd";
	   try
	     {
	     tpValue = ClassTypes[ii];
	     ++iCountTestcases;
	     iArr = (int[])Array.CreateInstance(tpValue, -1);
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	     }
	   catch(ArgumentOutOfRangeException)
	     {
	     }
	   catch(Exception ex)
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
	     }
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( " Error E_972qr! Unexpected Exception thrown == " + ex.ToString() + " strLoc=" + strLoc);
	   }
	 }
       strLoc = "Loc_452as!";
       Console.WriteLine ("p4");
       try
	 {
	 ++iCountTestcases;
	 iArr = (int[])Array.CreateInstance(null, 10);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_357sy!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_833af!  wrong exception thrown=="+ ex);
	 }
       Console.WriteLine ("p5");
       try
	 {
	 ++iCountTestcases;
	 iArr = (int[])Array.CreateInstance(typeof(int), Int32.MaxValue);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_364sd!  exzception not thrown");
	 }
       catch(OutOfMemoryException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_645dy!  wrong exception thrown=="+ ex);
	 }
       Console.WriteLine ("p6");
       {
       Array arrJaggedTest = Array.CreateInstance(typeof(int), nNumberOfArrays);
       for (int i = 0; i<255; i++) {
       try
	 {
	 ++iCountTestcases;
	 arrJaggedTest = Array.CreateInstance(arrJaggedTest.GetType(), ++nNumberOfArrays);
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_635df_" + i + ",  wrong exception thrown=="+ ex);
	 }
       }
       }
       Console.WriteLine ("p7");
       Array ArrJagged = Array.CreateInstance(typeof(Int32[]), 4);
       Int32[] iArr1 = new Int32[2];
       for(int i=0; i < 2; i++)
	 {
	 iArr1[i] = i+10;
	 }
       Object vntValue = iArr1;
       ArrJagged.SetValue(vntValue, 0);
       iArr1 = new Int32[3];
       for(int i=0; i < 3; i++)
	 {
	 iArr1[i] = i+20;
	 }
       vntValue = iArr1;
       ArrJagged.SetValue(vntValue, 1);
       iArr1 = new Int32[4];
       for(int i=0; i < 4; i++)
	 {
	 iArr1[i] = i+30;
	 }
       vntValue = iArr1;
       ArrJagged.SetValue(vntValue, 2);
       iArr1 = new Int32[5];
       for(int i=0; i < 5; i++)
	 {
	 iArr1[i] = i+40;
	 }
       vntValue = iArr1;
       ArrJagged.SetValue(vntValue, 3);
       Object objValue = (Object)ArrJagged.GetValue(0);
       Int32[] iArr2 = (Int32[])objValue;
       for(int i=0; i<iArr2.Length; i++)
	 {
	 ++iCountTestcases;
	 if ( iArr2[i] != (i+10) )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_745sd_" + i + ",  iArr2[i]==" + iArr2[i]);
	   }
	 }
       objValue = (Object)ArrJagged.GetValue(1);
       iArr2 = (Int32[])objValue;
       for(int i=0; i<iArr2.Length; i++)
	 {
	 ++iCountTestcases;
	 if ( iArr2[i] != (i+20) )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_106cs_" + i + ",  iArr2[i]==" + iArr2[i]);
	   }
	 }
       Console.WriteLine ("p8");
       objValue = (Object)ArrJagged.GetValue(2);
       iArr2 = (Int32[])objValue;
       for(int i=0; i<iArr2.Length; i++)
	 {
	 ++iCountTestcases;
	 if ( iArr2[i] != (i+30) )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_634ad_" + i + ",  iArr2[i]==" + iArr2[i]);
	   }
	 }
       objValue = (Object)ArrJagged.GetValue(3);
       iArr2 = (Int32[])objValue;
       for(int i=0; i<iArr2.Length; i++)
	 {
	 ++iCountTestcases;
	 if ( iArr2[i] != (i+40) )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_725fa_" + i + ",  iArr2[i]==" + iArr2[i]);
	   }
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev + "Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 static Type [] ClassTypes = {
   typeof(System.DBNull),
   typeof(System.Boolean),
   typeof(System.Char),
   typeof(System.SByte),
   typeof(System.Byte),
   typeof(System.Int16),
   typeof(System.UInt16),
   typeof(System.Int32),
   typeof(System.UInt32),
   typeof(System.Int64),
   typeof(System.UInt64),
   typeof(System.Single),
   typeof(System.Double),
   typeof(System.String),
   typeof(System.DateTime),
   typeof(System.TimeSpan),
   typeof(System.Decimal),
   typeof(System.Decimal),
   typeof(System.Object),
   Type.GetType("Simple"),
   typeof(System.DBNull[]),
   typeof(System.Boolean[]),
   typeof(System.Char[]),
   typeof(System.SByte[]),
   typeof(System.Byte[]),
   typeof(System.Int16[]),
   typeof(System.UInt16[]),
   typeof(System.Int32[]),
   typeof(System.UInt32[]),
   typeof(System.Int64[]),
   typeof(System.UInt64[]),
   typeof(System.Single[]),
   typeof(System.Double[]),
   typeof(System.String[]),
   typeof(System.DateTime[]),
   typeof(System.TimeSpan[]),
   typeof(System.Decimal[]),
   typeof(System.Decimal[]),
   typeof(System.Object[]),
   Type.GetType("Simple[]"),
   typeof(System.DBNull[][]),
   typeof(System.Boolean[][]),
   typeof(System.Char[][]),
   typeof(System.SByte[][]),
   typeof(System.Byte[][]),
   typeof(System.Int16[][]),
   typeof(System.UInt16[][]),
   typeof(System.Int32[][]),
   typeof(System.UInt32[][]),
   typeof(System.Int64[][]),
   typeof(System.UInt64[][]),
   typeof(System.Single[][]),
   typeof(System.Double[][]),
   typeof(System.String[][]),
   typeof(System.DateTime[][]),
   typeof(System.TimeSpan[][]),
   typeof(System.Decimal[][]),
   typeof(System.Decimal[][]),
   typeof(System.Object[][]),
   Type.GetType("Simple[][]"),
   typeof(System.DBNull[][][]),
   typeof(System.Boolean[][][]),
   typeof(System.Char[][][]),
   typeof(System.SByte[][][]),
   typeof(System.Byte[][][]),
   typeof(System.Int16[][][]),
   typeof(System.UInt16[][][]),
   typeof(System.Int32[][][]),
   typeof(System.UInt32[][][]),
   typeof(System.Int64[][][]),
   typeof(System.UInt64[][][]),
   typeof(System.Single[][][]),
   typeof(System.Double[][][]),
   typeof(System.String[][][]),
   typeof(System.DateTime[][][]),
   typeof(System.TimeSpan[][][]),
   typeof(System.Decimal[][][]),
   typeof(System.Decimal[][][]),
   typeof(System.Object[][][]),
   Type.GetType("Simple[][][]"),
 };
 static Boolean[]	bArr		= {true, true, true, true, true, false, false, false, false, false};
 static Char[]			cArr	  = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'd', 'c', 'b'};
 static SByte[]		sbtArr	= {SByte.MinValue, -100, -5, 0, 5, 100, SByte.MaxValue};
 static Byte[]			btArr		= {Byte.MinValue, 0, 5, 100, Byte.MaxValue};
 static Int16[]		i16Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int32[]		i32Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int64[]		i64Arr	= {-530, Int64.MinValue, Int32.MinValue, Int16.MinValue, -127, 0, Int64.MaxValue, Int32.MaxValue, Int16.MaxValue};
 static Single[] 	fArr    = {-1.2e23f, 1.2e-32f, -1.23f, 0.0f, 1.23e23f, 1.23f, 0.0f, 2.45f, 1.2f, -5.6f};
 static Double[] 	dArr    = {-1.2e23, 1.2e-32, -1.23, 0.0, 3.4, -1.2e23, 1.2e-32, -1.23, 0.0, 3.4};
 static String[]   strArr  = {"This", " ", "a", " ", "test", " ", "of", " ", "patience", "."};
 static DateTime[] dtArr	  = {new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54)};
 static Decimal[] curArr  = {Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue,
			     Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue};
 static Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 private void GoFillSomeTestValues(Hashtable hshArrayValue)
   {
   ArrayList alst = new ArrayList();
   for(int ii=0; ii < bArr.Length; ii++)
     {
     alst.Add(bArr[ii]);
     }
   hshArrayValue.Add(typeof(Boolean).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < cArr.Length; ii++)
     {
     alst.Add(cArr[ii]);
     }
   hshArrayValue.Add(typeof(Char).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < sbtArr.Length; ii++)
     {
     alst.Add(sbtArr[ii]);
     }
   hshArrayValue.Add(typeof(SByte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < btArr.Length; ii++)
     {
     alst.Add(btArr[ii]);
     }
   hshArrayValue.Add(typeof(Byte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i16Arr.Length; ii++)
     {
     alst.Add(i16Arr[ii]);
     }
   hshArrayValue.Add(typeof(Int16).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i32Arr.Length; ii++)
     {
     alst.Add(i32Arr[ii]);
     }
   hshArrayValue.Add(typeof(Int32).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i64Arr.Length; ii++)
     {
     alst.Add(i64Arr[ii]);
     }
   hshArrayValue.Add(typeof(Int64).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < fArr.Length; ii++)
     {
     alst.Add(fArr[ii]);
     }
   hshArrayValue.Add(typeof(Single).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dArr.Length; ii++)
     {
     alst.Add(dArr[ii]);
     }
   hshArrayValue.Add(typeof(Double).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < strArr.Length; ii++)
     {
     alst.Add(strArr[ii]);
     }
   hshArrayValue.Add(typeof(String).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dtArr.Length; ii++)
     {
     alst.Add(dtArr[ii]);
     }
   hshArrayValue.Add(typeof(DateTime).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < curArr.Length; ii++)
     {
     alst.Add(curArr[ii]);
     }
   hshArrayValue.Add(typeof(Decimal).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < oArr.Length; ii++)
     {
     alst.Add(oArr[ii]);
     }
   hshArrayValue.Add(typeof(Object).Name, alst);
   }
 public static void Main( String[] args )
   {
   Boolean bResult = false;	
   Co3742CreateInstance_type_i oCbTest = new Co3742CreateInstance_type_i();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev + "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFAbbrev + s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode=0; else Environment.ExitCode=11; 
   }
}
class Simple
{
 Simple() { m_oObject = "Hello World";}
 Object m_oObject;
}
