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
public class Co3748GetValue_ii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.GetValue(int, int)";
 public static String s_strTFName        = "Co3748GetValue_ii";
 public static String s_strTFAbbrev      = "Co3748";
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
   Int32[,] iArr;
   Array objArr;
   Hashtable hshArrayValue;
   ArrayList alst1 = new ArrayList();
   try
     {
     do
       {
       tpValue = typeof(Int32);
       iArr = (Int32[,])Array.CreateInstance(tpValue, 10, 10);
       for(int ii = iArr.GetLowerBound(0); ii <= iArr.GetUpperBound(0); ii++)
	 for(int jj=iArr.GetLowerBound(1); jj <= iArr.GetUpperBound(1); jj++)
	   iArr.SetValue(ii*jj, ii, jj);
       for(int ii = iArr.GetLowerBound(0); ii <= iArr.GetUpperBound(0); ii++)
	 {
	 for(int jj=iArr.GetLowerBound(1); jj <= iArr.GetUpperBound(1); jj++)
	   {
	   ++iCountTestcases;
	   if(!iArr.GetValue(ii,jj).Equals(ii*jj))
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_736qm_" + ii + "_" + jj + ", ii*jj==" + iArr.GetValue(ii,jj));
	     }
	   }
	 }
       int[] iTempArr = new int[2];
       for(int ii = iArr.GetLowerBound(0); ii <= iArr.GetUpperBound(0); ii++)
	 {
	 for(int jj=iArr.GetLowerBound(1); jj <= iArr.GetUpperBound(1); jj++)
	   {
	   iTempArr[0]=ii;
	   iTempArr[1]=jj;
	   ++iCountTestcases;
	   if(!iArr.GetValue(iTempArr).Equals(ii*jj))
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + "Err_630jm_" + ii + "_" + jj + ", ii*jj==" + iArr.GetValue(ii,jj));
	     }
	   }
	 }
       hshArrayValue = new Hashtable();
       GoFillSomeTestValues(hshArrayValue);
       for(int ii=0; ii < ClassTypes.Length; ii++)
	 {
	 try
	   {
	   strLoc = "E632af_" + ii;
	   tpValue = ClassTypes[ii];
	   objArr  = Array.CreateInstance(tpValue, 10, 10);
	   if(hshArrayValue.ContainsKey(ClassTypes[ii].Name))
	     {
	     alst1 = (ArrayList)hshArrayValue[ClassTypes[ii].Name];
	     for(int jj = 0, x = 0, y=0, z = objArr.GetLength(0); jj < alst1.Count; jj++)
	       {
	       objArr.SetValue(alst1[jj], x, y);
	       y++;
	       if((y%z==0))
		 {
		 x++;
		 y = 0;
		 }
	       }
	     for(int jj = 0, x = 0, y=0, z = objArr.GetLength(0); jj < alst1.Count; jj++)
	       {
	       strLoc="E_672ad_" + ii + "_" + x + "_" + y;
	       ++iCountTestcases;
	       if (!objArr.GetValue(x, y).Equals(alst1[jj]))
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev + ", Err_673sa_" + ii + "_" + x + "_" + y + ", objArr.GetValue(x, y)==" + objArr.GetValue(x, y) + ", alst1[jj]==" + alst1[jj]);
		 }
	       y++;
	       if((y%z==0))
		 {
		 x++;
		 y = 0;
		 }
	       }
	     ++iCountTestcases;
	     try
	       {
	       objArr.GetValue(objArr.GetLowerBound(0)-1, objArr.GetLowerBound(1));
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_763sr! Exception not thrown");
	       }
	     catch(IndexOutOfRangeException )
	       {
	       }
	     catch(Exception ex)
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_874sq! wrong exception thrown == " + ex);
	       }
	     ++iCountTestcases;
	     try
	       {
	       objArr.GetValue(objArr.GetUpperBound(0)+1, objArr.GetLowerBound(1));
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_739dw! Exception not thrown");
	       }
	     catch(IndexOutOfRangeException )
	       {
	       }
	     catch(Exception ex)
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_974sp! wrong exception thrown == " + ex);
	       }
	     ++iCountTestcases;
	     try
	       {
	       objArr.GetValue(objArr.GetUpperBound(0), objArr.GetLowerBound(1)-1);
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_874st! Exception not thrown");
	       }
	     catch(IndexOutOfRangeException )
	       {
	       }
	     catch(Exception ex)
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_684sp! wrong exception thrown == " + ex);
	       }
	     ++iCountTestcases;
	     try
	       {
	       objArr.GetValue(objArr.GetUpperBound(0), objArr.GetUpperBound(1)+1);
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_846pl! Exception not thrown");
	       }
	     catch(IndexOutOfRangeException )
	       {
	       }
	     catch(Exception ex)
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_760sm! wrong exception thrown == " + ex);
	       }
	     ++iCountTestcases;
	     if(strArrayTypes[ii].Equals(objArr.GetType().Name) != true)
	       {
	       ++iCountErrors;
	       Console.WriteLine( s_strTFAbbrev + ", Err_752sa! Look at this, Unexpected type==" + objArr.GetType().Name + "!=" + strArrayTypes[ii]);
	       }
	     }
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( " Error E_972qr! Unexpected Exception thrown == " + ex.ToString() + "strLoc = " + strLoc + " Type ="+ ClassTypes[ii].Name);
	   }
	 }
       tpValue = typeof(Int32);
       iArr = (Int32[,])Array.CreateInstance(tpValue, 10, 10);
       for(int ii = 0; ii < iArr.Rank; ii++)
	 {
	 for(int jj=iArr.GetLowerBound(ii); jj <= iArr.GetUpperBound(ii); jj++)
	   iArr.SetValue(ii*jj, ii, jj);
	 }
       try
	 {
	 ++iCountTestcases;
	 iArr.GetValue(iArr.GetUpperBound(0));
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
	 ++iCountTestcases;
	 iArr.GetValue(iArr.GetUpperBound(0), iArr.GetUpperBound(1), iArr.GetUpperBound(0));
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
 };
 static String [] strArrayTypes = {
   "DBNull[,]",
   "Boolean[,]",
   "Char[,]",
   "SByte[,]",
   "Byte[,]",
   "Int16[,]",
   "UInt16[,]",
   "Int32[,]",
   "UInt32[,]",
   "Int64[,]",
   "UInt64[,]",
   "Single[,]",
   "Double[,]",
   "String[,]",
   "DateTime[,]",
   "TimeSpan[,]",
   "Decimal[,]",
   "Decimal[,]",
   "Object[,]",
   "Missing[,]",
   "DBNull[,]",
   "Simple[,]",
 };
 static Boolean[,]	bArr		= {{true, true, true, true, true, false, false, false, false, false},
					   {true, true, true, true, true, false, false, false, false, false}};
 static Char[,]		cArr	  = {{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'},
				     {'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'}};
 static SByte[,]		sbtArr	= {{SByte.MinValue, -100, -5, 0, 5, 100, SByte.MaxValue},
					   {SByte.MinValue, -100, -5, 0, 5, 100, SByte.MaxValue}};
 static Byte[,]			btArr		= {{Byte.MinValue, 0, 5, 100, Byte.MaxValue},
						   {Byte.MinValue, 0, 5, 100, Byte.MaxValue}};
 static Int16[,]		i16Arr	= {{19, 238, 317, 6, 565, 0, -52, 60, -563, 753},
					   {19, 238, 317, 6, 565, 0, -52, 60, -563, 753}};
 static Int32[,]		i32Arr	= {{19, 238, 317, 6, 565, 0, -52, 60, -563, 753},
					   {19, 238, 317, 6, 565, 0, -52, 60, -563, 753}};
 static Int64[,]		i64Arr	= {{-530, Int64.MinValue, Int32.MinValue, Int16.MinValue, -127, 0, Int64.MaxValue, Int32.MaxValue, Int16.MaxValue, 0},
					   {-530, Int64.MinValue, Int32.MinValue, Int16.MinValue, -127, 0, Int64.MaxValue, Int32.MaxValue, Int16.MaxValue, 0}};
 static Single[,]	fArr    = {{-1.2e23f, 1.2e-32f, -1.23f, 0.0f, -1.0f, -1.2e23f, 1.2e-32f, -1.23f, 0.0f, -1.0f},
				   {1.23e23f, 1.23f, 0.0f, 2.45f, 35.0f, 1.23e23f, 1.23f, 0.0f, 2.45f, 35.0f}};
 static Double[,]	dArr    = {{-1.2e23, 1.2e-32, -1.23, 0.0, 56.0, -1.2e23, 1.2e-32, -1.23, 0.0, 56.0,},
				   {1.23e23, 1.23, 0.0, 2.45, 635.0, -1.2e23, 1.2e-32, -1.23, 0.0, 56.0}};
 static String[,]  strArr  = {{"This", " ", "a", " ", "test", " ", "of", " ", "patience", "."},
			      {"This", " ", "a", " ", "test", " ", "of", " ", "patience", "."}};
 private void GoFillSomeTestValues(Hashtable hshArrayValue)
   {
   ArrayList alst = new ArrayList();
   for(int ii=0; ii < bArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < bArr.GetLength(1); jj++)
       {
       alst.Add(bArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Boolean).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < cArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < cArr.GetLength(1); jj++)
       {
       alst.Add(cArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Char).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < sbtArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < sbtArr.GetLength(1); jj++)
       {
       alst.Add(sbtArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(SByte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < btArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < btArr.GetLength(1); jj++)
       {
       alst.Add(btArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Byte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i16Arr.GetLength(0); ii++)
     {
     for(int jj=0; jj < i16Arr.GetLength(1); jj++)
       {
       alst.Add(i16Arr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Int16).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i32Arr.GetLength(0); ii++)
     {
     for(int jj=0; jj < i32Arr.GetLength(1); jj++)
       {
       alst.Add(i32Arr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Int32).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i64Arr.GetLength(0); ii++)
     {
     for(int jj=0; jj < i64Arr.GetLength(1); jj++)
       {
       alst.Add(i64Arr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Int64).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < fArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < fArr.GetLength(1); jj++)
       {
       alst.Add(fArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Single).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < dArr.GetLength(1); jj++)
       {
       alst.Add(dArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(Double).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < strArr.GetLength(0); ii++)
     {
     for(int jj=0; jj < strArr.GetLength(1); jj++)
       {
       alst.Add(strArr[ii, jj]);
       }
     }
   hshArrayValue.Add(typeof(String).Name, alst);
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3748GetValue_ii oCbTest = new Co3748GetValue_ii();
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
