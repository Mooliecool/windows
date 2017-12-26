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
public class Co3745CreateInstance_type_iArr
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.CreateInstance(Type, int[])";
 public static String s_strTFName        = "Co3745CreateInstance_type_iArr";
 public static String s_strTFAbbrev      = "Co3745";
 public static String s_strTFPath        = "";
 public bool runTest()
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
   Int32[,,,] iArr;
   int[] iIndexArr = {5,6,7,8};
   Array objArr;
   try
     {
     do
       {
       tpValue = typeof(Int32);
       iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr);
       ++iCountTestcases;
       if ( iArr.Rank != 4 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_643ag!  iArr.Rank=="+ iArr.Rank);
	 }
       ++iCountTestcases;
       if ( iArr.Length != 1680 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_357sw!  iArr.Length=="+ iArr.Length);
	 }
       for(int ii = 0; ii < iArr.Rank; ii++)
	 {
	 ++iCountTestcases;
	 if ( iArr.GetLength(ii) != iIndexArr[ii] )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_320ye!  iArr.GetLength(ii)=="+ iArr.GetLength(ii) );
	   }
	 ++iCountTestcases;
	 if ( iArr.GetLowerBound(ii) != 0 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_763sf!  iArr.GetLowerBound(ii)=="+ iArr.GetLowerBound(ii) );
	   }
	 ++iCountTestcases;
	 if ( iArr.GetUpperBound(ii) != iIndexArr[ii]-1 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_268aq!  iArr.GetUpperBound(ii)=="+ iArr.GetUpperBound(ii) );
	   }
	 }
       for(int i = 0; i <= iArr.GetUpperBound(0); i++)
	 for(int j=0; j <= iArr.GetUpperBound(1); j++)
	   for(int k=0; k<= iArr.GetUpperBound(2); k++)
	     for(int l=0; l<= iArr.GetUpperBound(3); l++)
	       {
	       iIndexArr[0]=i;
	       iIndexArr[1]=j;
	       iIndexArr[2]=k;
	       iIndexArr[3]=l;
	       iArr.SetValue(i*j*k*l, iIndexArr);
	       }
       for(int i = 0; i <= iArr.GetUpperBound(0); i++)
	 {
	 for(int j=0; j <= iArr.GetUpperBound(1); j++)
	   {
	   for(int k=0; k<= iArr.GetUpperBound(2); k++)
	     {
	     for(int l=0; l<= iArr.GetUpperBound(3); l++)
	       {
	       iIndexArr[0]=i;
	       iIndexArr[1]=j;
	       iIndexArr[2]=k;
	       iIndexArr[3]=l;
	       ++iCountTestcases;
	       if(!iArr.GetValue(iIndexArr).Equals(i*j*k*l))
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev + "Err_630jm_" + i + "_" + j + "_" + k + "_" + l + ", i*j*k*l==" + iArr.GetValue(iIndexArr));
		 }
	       }
	     }
	   }
	 }
       iIndexArr[0]=5;
       iIndexArr[1]=6;
       iIndexArr[2]=7;
       iIndexArr[3]=8;
       for(int ii=0; ii < ClassTypes.Length; ii++)
	 {
	 try
	   {
	   tpValue = ClassTypes[ii];
	   strLoc = "6537cdcf_" + ii;
	   objArr  = Array.CreateInstance(tpValue, iIndexArr);
	   ++iCountTestcases;
	   if ( objArr.Rank != 4 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_762sw_" + ii + ", objArr.Rank=="+ objArr.Rank);
	     }
	   ++iCountTestcases;
	   if ( objArr.Length != 1680 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_357sw!  objArr.Length=="+ objArr.Length);
	     }
	   for(int jj = 0; jj < iArr.Rank; jj++)
	     {
	     ++iCountTestcases;
	     if ( objArr.GetLength(jj) != iIndexArr[jj] )
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_874sg_" + ii + ",  objArr.GetLength(jj)=="+ objArr.GetLength(jj) + ", iIndexArr[jj]==" + iIndexArr[jj] );
	       }
	     ++iCountTestcases;
	     if ( objArr.GetLowerBound(jj) != 0 )
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_763sr_" + ii + ",  objArr.GetLowerBound(jj)=="+ objArr.GetLowerBound(jj) );
	       }
	     ++iCountTestcases;
	     if ( objArr.GetUpperBound(jj) != iIndexArr[jj]-1 )
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_623ov_" + ii + ", objArr.GetUpperBound(jj)=="+ objArr.GetUpperBound(jj) );
	       }
	     }
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( " Error E_972qr! Unexpected Exception thrown == " + ex.ToString() + ", strLoc = " + strLoc);
	   }
	 }
       tpValue = typeof(Int32);
       iIndexArr = new int[1000];
       try
	 {
	 ++iCountTestcases;
	 Array a = Array.CreateInstance(tpValue, iIndexArr);
	 Console.WriteLine(a.GetType ());
	 Console.WriteLine(a.Rank);
	 Console.WriteLine(a.GetType ().GetElementType());
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_635df!  No exception thrown");
	 }
       catch(TypeLoadException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_478dg!  wrong exception thrown=="+ ex);
	 }
       tpValue = typeof(Int32);
       iIndexArr = new int[] {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}; 
       try
	 {
	 ++iCountTestcases;
	 Array a = Array.CreateInstance(tpValue, iIndexArr);
	 Console.WriteLine(a.GetType ());
	 Console.WriteLine(a.Rank);
	 Console.WriteLine(a.GetType ().GetElementType());
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_90du3!  wrong exception thrown=="+ ex);
	 }
       try
	 {
	 tpValue = typeof(Int32);
	 iIndexArr = new int[4];
	 iIndexArr[0]=2;
	 iIndexArr[1]=2;
	 iIndexArr[2]=2;
	 iIndexArr[3]=-1;
	 ++iCountTestcases;
	 iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
	 }
       try
	 {
	 tpValue = typeof(Int32);
	 iIndexArr[0]=-2;
	 iIndexArr[1]=2;
	 iIndexArr[2]=2;
	 iIndexArr[3]=1;
	 ++iCountTestcases;
	 iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
	 }
       try
	 {
	 tpValue = typeof(Int32);
	 iIndexArr=null;
	 ++iCountTestcases;
	 iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
	 }
       try
	 {
	 tpValue = typeof(Int32);
	 int[] iIndexArr1=new int[0];
	 ++iCountTestcases;
	 iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
	 }
       try
	 {
	 tpValue = null;
	 ++iCountTestcases;
	 iArr = (Int32[,,,])Array.CreateInstance(tpValue, iIndexArr);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_672sf!  exzception not thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_926sp!  wrong exception thrown=="+ ex);
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
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3745CreateInstance_type_iArr oCbTest = new Co3745CreateInstance_type_iArr();
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
