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
using System.Threading;
using System.Reflection;
public class Co3744CreateInstance_type_iii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.CreateInstance(Type, int, int, int)";
 public static String s_strTFName        = "Co3744CreateInstance_type_iii";
 public static String s_strTFAbbrev      = "Co3744";
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
   Int32[,,] iArr;
   Array objArr;
   Hashtable hshArrayValue;
   ArrayList alst1 = new ArrayList();
   int iFirstIndex;
   int iSecIndex;
   int iThirdIndex;
   try
     {
     do
       {
       tpValue = typeof(Int32);
       iArr = (Int32[,,])Array.CreateInstance(tpValue, 10, 10, 10);
       ++iCountTestcases;
       if ( iArr.Rank != 3 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_643ag!  iArr.Rank=="+ iArr.Rank);
	 }
       ++iCountTestcases;
       if ( iArr.Length != 1000 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_357sw!  iArr.Length=="+ iArr.Length);
	 }
       for(int ii = 0; ii < iArr.Rank; ii++)
	 {
	 ++iCountTestcases;
	 if ( iArr.GetLength(ii) != 10 )
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
	 if ( iArr.GetUpperBound(ii) != 9 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_268aq!  iArr.GetUpperBound(ii)=="+ iArr.GetUpperBound(ii) );
	   }
	 }
       for(int i = 0; i <= iArr.GetUpperBound(0); i++)
	 for(int j=0; j <= iArr.GetUpperBound(1); j++)
	   for(int k=0; k<= iArr.GetUpperBound(2); k++)
	     iArr.SetValue(i*j*k, i, j, k);
       for(int i = 0; i <= iArr.GetUpperBound(0); i++)
	 {
	 for(int j=0; j <= iArr.GetUpperBound(1); j++)
	   {
	   for(int k=0; k<= iArr.GetUpperBound(2); k++)
	     {
	     ++iCountTestcases;
	     if(!iArr.GetValue(i, j, k).Equals(i*j*k))
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_630jm_" + i + "_" + j + "_" + k + ", i*j*k==" + iArr.GetValue(i,j,k));
	       }
	     }
	   }
	 }
       strLoc="loc_246sd";
       hshArrayValue = new Hashtable();
       GoFillSomeTestValues(hshArrayValue);
       iFirstIndex = 3;
       iSecIndex = 3;
       iThirdIndex = 3;
       for(int ii=0; ii < ClassTypes.Length; ii++)
	 {
	 try
	   {
	   strLoc="loc_346sd_" + ii;
	   tpValue = ClassTypes[ii];
	   objArr  = Array.CreateInstance(tpValue, iFirstIndex, iSecIndex, iThirdIndex);
	   ++iCountTestcases;
	   if ( objArr.Rank != 3 )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_762sw_" + ii + ", objArr.Rank=="+ objArr.Rank);
	     }
	   ++iCountTestcases;
	   if ( objArr.Length != iFirstIndex * iSecIndex * iThirdIndex )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_357sw!  objArr.Length=="+ objArr.Length);
	     }
	   for(int jj = 0; jj < iArr.Rank; jj++)
	     {
	     ++iCountTestcases;
	     if ( objArr.GetLength(jj) != iFirstIndex )	
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_874sg_" + ii + ",  objArr.GetLength(jj)=="+ objArr.GetLength(jj) );
	       }
	     ++iCountTestcases;
	     if ( objArr.GetLowerBound(jj) != 0 )
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_763sr_" + ii + ",  objArr.GetLowerBound(jj)=="+ objArr.GetLowerBound(jj) );
	       }
	     ++iCountTestcases;
	     if ( objArr.GetUpperBound(jj) != iFirstIndex-1 )
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + "Err_623ov_" + ii + ", objArr.GetUpperBound(jj)=="+ objArr.GetUpperBound(jj) );
	       }
	     }
	   if(hshArrayValue.ContainsKey(ClassTypes[ii].Name))
	     {
	     alst1 = (ArrayList)hshArrayValue[ClassTypes[ii].Name];
	     for(int jj = 0, x = 0, y=0, z = 0, dim1max = objArr.GetLength(0), dim2max = objArr.GetLength(1); jj < alst1.Count; jj++)
	       {
	       strLoc="loc_649fs_" + ii + "_" + jj + "_" + x + "_" + y + "_" + z;
	       objArr.SetValue(alst1[jj], x, y, z);
	       z++;
	       if((z%dim1max==0))
		 {
		 z=0;
		 y++;
		 if((y%dim2max==0))
		   {
		   x++;
		   y = 0;
		   }
		 }
	       }
	     for(int jj = 0, x = 0, y=0, z = 0, dim1max = objArr.GetLength(0), dim2max = objArr.GetLength(1); jj < alst1.Count; jj++)
	       {
	       strLoc="E_672ad_" + ii + "_" + x + "_" + y;
	       ++iCountTestcases;
	       if (!objArr.GetValue(x, y, z).Equals(alst1[jj]))
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev + ", Err_673sa_" + ii + "_" + x + "_" + y + ", objArr.GetValue(x, y)==" + objArr.GetValue(x, y) + ", alst1[jj]==" + alst1[jj]);
		 }
	       z++;
	       if((z%dim1max==0))
		 {
		 z=0;
		 y++;
		 if((y%dim2max==0))
		   {
		   x++;
		   y = 0;
		   }
		 }
	       }
	     }
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( " Error E_972qr! Unexpected Exception thrown == " + ex.ToString() + ", strLoc = " + strLoc);
	   }
	 }
       try
	 {
	 tpValue = typeof(Int32);
	 ++iCountTestcases;
	 iArr = (Int32[,,])Array.CreateInstance(tpValue, -1, 5, 5);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException )
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
	 ++iCountTestcases;
	 iArr = (Int32[,,])Array.CreateInstance(tpValue, 1, -5, 5);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException )
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
	 ++iCountTestcases;
	 iArr = (Int32[,,])Array.CreateInstance(tpValue, 1, 5, -5);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_763sy!  exzception not thrown");
	 }
       catch(ArgumentException )
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
	 iArr = (Int32[,,])Array.CreateInstance(tpValue, 1, 5, 5);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_672sf!  exzception not thrown");
	 }
       catch(ArgumentException )
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
   typeof(System.Reflection.Missing),
   typeof(System.DBNull),
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
   typeof(System.Reflection.Missing[]),
   typeof(System.DBNull[]),
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
   typeof(System.Reflection.Missing[][]),
   typeof(System.DBNull[][]),
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
   typeof(System.Reflection.Missing[][][]),
   typeof(System.DBNull[][][]),
   Type.GetType("Simple[][][]"),
 };
 static Boolean[,,]	bArr		= {{{true, false, true}, {false, true, true}, {true, true, false}},
					   {{false, true, false}, {true, false, false}, {false, true, false}},
					   {{true, false, true}, {false, false, false}, {true, true, true}}};
 static Char[,,]		cArr		  = {{{'a', 'b', 'c'}, {'d', 'e', 'f'}, {'d', 'e', 'f'}},
						     {{'f', 'g', 'h'}, {'d', 'e', 'f'}, {'d', 'e', 'f'}},
						     {{'k', 'l', 'm'}, {'d', 'e', 'f'}, {'d', 'e', 'f'}}};
 static SByte[,,]		sbtArr	= {{{SByte.MinValue, -5, 0}, {5, SByte.MaxValue, -10}, {5, SByte.MaxValue, -10}},
					   {{SByte.MinValue, -5, 0}, {5, SByte.MaxValue, -10}, {5, SByte.MaxValue, -10}},
					   {{SByte.MinValue, -5, 0}, {5, SByte.MaxValue, -10}, {5, SByte.MaxValue, -10}}};
 static Byte[,,]			btArr		= {{{Byte.MinValue, 0, 5}, {100, Byte.MaxValue, 10}, {100, Byte.MaxValue, 10}},
							   {{Byte.MinValue, 0, 5}, {100, Byte.MaxValue, 10}, {100, Byte.MaxValue, 10}},
							   {{Byte.MinValue, 0, 5}, {100, Byte.MaxValue, 10}, {100, Byte.MaxValue, 10}}};
 static Int16[,,]		i16Arr	= {{{19, 565, 0}, {-52, 60, 64}, {-52, 60, 64}},
					   {{19, 238, 317}, {-52, 60, 64}, {-52, 60, 64}},
					   {{0, -52, 60}, {-52, 60, 64}, {-52, 60, 64}}};
 static Int32[,,]		i32Arr	= {{{19, 238, 317}, {6, 565, -563}, {19, 238, 317}},
					   {{0, -52, 60}, {6, 565, -563}, {19, 238, 317}},
					   {{19, 238, 317}, {6, 565, -563}, {19, 238, 317}}};
 static Int64[,,]		i64Arr	= {{{-530, Int64.MinValue, Int32.MinValue}, {Int16.MinValue, -127, 486}, {Int16.MinValue, -127, 486}},
					   {{0, Int64.MaxValue, Int32.MaxValue}, {Int16.MinValue, -127, 486}, {Int16.MinValue, -127, 486}},
					   {{-530, Int64.MinValue, Int32.MinValue}, {Int16.MinValue, -127, 486}, {Int16.MinValue, -127, 486}}};
 static Single[,,]	fArr		  = {{{-1.2e23f, 1.2e-32f, -1.23f}, {0.0f, 45.463f, Single.MaxValue}, {0.0f, 45.463f, Single.MinValue}},
					     {{1.23e23f, 1.23f, 0.0f}, {0.0f, 45.463f, Single.MaxValue}, {0.0f, 45.463f, Single.MinValue}},
					     {{1.23e23f, 1.23f, 0.0f}, {0.0f, 45.463f, Single.MaxValue}, {0.0f, 45.463f, Single.MinValue}}};
 static Double[,,]	dArr			= {{{-1.2e23, 1.2e-32, -1.23}, {0.0, 3.4, Double.MaxValue}, {0.0, 3.4, Double.MinValue}},
						   {{-1.2e23, 1.2e-32, -1.23}, {0.0, 3.4, Double.MaxValue}, {0.0, 3.4, Double.MinValue}},
						   {{1.23e23, 1.23, 0.0}, {0.0, 3.4, Double.MaxValue}, {0.0, 3.4, Double.MinValue}}};
 static Object[,,]	oArr			= {{{true, 'k', SByte.MinValue}, {Byte.MinValue, (short)2, "Hello World"}, {1.23, 2.45, 3.4}},
						   {{634, (long)436, (float)1.1}, {Byte.MinValue, (short)2, "Hello World"}, {1.23, 2.45, 3.4}},
						   {{false, 1.23, 0.0}, {Byte.MinValue, (short)2, "Hello World"}, {1.23, 2.45, 3.4}}};
 private void GoFillSomeTestValues(Hashtable hshArrayValue)
   {
   ArrayList alst = new ArrayList();
   for(int ii=0; ii < bArr.GetLength(0); ii++)
     for(int jj=0; jj < bArr.GetLength(1); jj++)
       for(int kk=0; kk < bArr.GetLength(2); kk++)
	 alst.Add(bArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Boolean).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < cArr.GetLength(0); ii++)
     for(int jj=0; jj < cArr.GetLength(1); jj++)
       for(int kk=0; kk < cArr.GetLength(2); kk++)
	 alst.Add(cArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Char).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < sbtArr.GetLength(0); ii++)
     for(int jj=0; jj < sbtArr.GetLength(1); jj++)
       for(int kk=0; kk < sbtArr.GetLength(2); kk++)
	 alst.Add(sbtArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(SByte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < btArr.GetLength(0); ii++)
     for(int jj=0; jj < btArr.GetLength(1); jj++)
       for(int kk=0; kk < btArr.GetLength(2); kk++)
	 alst.Add(btArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Byte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i16Arr.GetLength(0); ii++)
     for(int jj=0; jj < i16Arr.GetLength(1); jj++)
       for(int kk=0; kk < i16Arr.GetLength(2); kk++)
	 alst.Add(i16Arr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Int16).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i32Arr.GetLength(0); ii++)
     for(int jj=0; jj < i32Arr.GetLength(1); jj++)
       for(int kk=0; kk < i32Arr.GetLength(2); kk++)
	 alst.Add(i32Arr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Int32).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i64Arr.GetLength(0); ii++)
     for(int jj=0; jj < i64Arr.GetLength(1); jj++)
       for(int kk=0; kk < i64Arr.GetLength(2); kk++)
	 alst.Add(i64Arr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Int64).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < fArr.GetLength(0); ii++)
     for(int jj=0; jj < fArr.GetLength(1); jj++)
       for(int kk=0; kk < fArr.GetLength(2); kk++)
	 alst.Add(fArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Single).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dArr.GetLength(0); ii++)
     for(int jj=0; jj < dArr.GetLength(1); jj++)
       for(int kk=0; kk < dArr.GetLength(2); kk++)
	 alst.Add(dArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Double).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < oArr.GetLength(0); ii++)
     for(int jj=0; jj < oArr.GetLength(1); jj++)
       for(int kk=0; kk < oArr.GetLength(2); kk++)
	 alst.Add(oArr[ii, jj, kk]);
   hshArrayValue.Add(typeof(Object).Name, alst);
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3744CreateInstance_type_iii oCbTest = new Co3744CreateInstance_type_iii();
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
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
class Simple
{
 Simple() { m_oObject = "Hello World";}
 Object m_oObject;
}
