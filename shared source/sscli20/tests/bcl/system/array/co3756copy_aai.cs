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
using System.Reflection; 
using System.Threading;
using System.Globalization;
public class Co3756Copy_aai
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.Copy(Array, Array, int)";
 public static String s_strTFName        = "Co3756Copy_aai";
 public static String s_strTFAbbrev      = "Co3756";
 public static String s_strTFPath        = "";
 private bool fExceptionInfo;
 private bool fReflection;
 private bool fDebug;
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
   Array arrSource;
   Array arrSourceJagged;
   Array arrSourceDoubleJagged;
   Array arrTarget;
   int iArraySize = 10;
   Hashtable hshArrayValue;
   ArrayList alst1 = new ArrayList();
   Array arMultiDimSrc;
   Array arMultiDimTgt;
   try
     {
     do
       {
       hshArrayValue = new Hashtable();
       GoFillSomeTestValues(hshArrayValue);
       for(int ii=0; ii < TypeArray.Length; ii++)
	 {
	 strLoc="Loc_326fd_" + ii;
	 try
	   {
	   tpValue = TypeArray[ii];
	   arrSource  = Array.CreateInstance(tpValue, iArraySize);
	   if(fDebug)
	     {
	     Console.WriteLine();
	     Console.WriteLine("[][][] Test case 4 - Copy the original uninitialized array (created from Type array) to all the possible Array types (created from Type array)");
	     Console.WriteLine();
	     }
	   if(fDebug)
	     Console.WriteLine("ii=" + ii + ", tpValue=" + tpValue.Name);
	   for(int jj=0; jj < TypeArray.Length; jj++)
	     {
	     strLoc="Loc_036vd_" + ii + "_" + jj;
	     tpValue = TypeArray[jj];
	     arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	     try
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + tpValue.Name);
	       ++iCountTestcases;
	       Array.Copy(arrSource, arrTarget, iArraySize);
	       for(int l=0; l< arrSource.Length; l++)
		 {
		 ++iCountTestcases;
		 if(!arrSource.GetValue(l).Equals(arrTarget.GetValue(l)))
		   {
		   if (fExceptionInfo)
		     {
		     Console.Write( "Uninitialised values, Loc_7346sgf_" + ii + "_" + jj + "_" + l );
		     Console.WriteLine( ", values not the same, =="+ arrSource.GetValue(l) + ", " + arrTarget.GetValue(l));
		     }
		   }
		 }
	       }
	     catch(Exception ex)
	       {
	       if (fExceptionInfo)
		 Console.WriteLine(ex + ", Uninitialized Copy; ArrayType, Source=" + arrSource.GetType().Name + ", Target=" + arrTarget.GetType().Name);
	       }
	     }
	   if(fDebug)
	     {
	     Console.WriteLine();
	     Console.WriteLine("[][][] Test case 5 - Copy an initialised jagged array [][] to all data types");
	     Console.WriteLine();
	     }
	   strLoc="Loc_02456vdf" + ii;
	   if(fDebug)
	     Console.WriteLine("ii=" + ii + ", tpValue=" + tpValue.Name);
	   arrSourceJagged  = Array.CreateInstance(arrSource.GetType(), iArraySize);
	   for(int jj=0; jj < TypeArray.Length; jj++)
	     {
	     strLoc="Loc_6235csd_" + ii + "_" + jj;
	     tpValue = TypeArray[jj];
	     arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	     try
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + tpValue.Name);
	       ++iCountTestcases;
	       Array.Copy(arrSourceJagged, arrTarget, iArraySize);
	       for(int l=0; l< arrSourceJagged.Length; l++)
		 {
		 ++iCountTestcases;
		 if(!arrSourceJagged.GetValue(l).Equals(arrTarget.GetValue(l)))
		   {
		   if (fExceptionInfo)
		     {
		     Console.Write( "Uninitialised values, Loc_15gf3_" + ii + "_" + jj + "_" + l );
		     Console.WriteLine( ", values not the same, =="+ arrSourceJagged.GetValue(l) + ", " + arrTarget.GetValue(l));
		     }
		   }
		 }
	       }
	     catch(Exception ex)
	       {
	       if (fExceptionInfo)
		 Console.WriteLine(ex + ", Uninitialized Copy; ArrayType, Source=" + arrSourceJagged.GetType().Name + ", Target=" + arrTarget.GetType().Name);
	       }
	     }
	   if(fDebug)
	     {
	     Console.WriteLine();
	     Console.WriteLine("[][][] Test case 6 - Copy an initialised jagged array [][][] to all data types");
	     Console.WriteLine();
	     }
	   strLoc="Loc_03yvdf" + ii;
	   if(fDebug)
	     Console.WriteLine("ii=" + ii + ", tpValue=" + tpValue.Name);
	   arrSourceDoubleJagged  = Array.CreateInstance(arrSourceJagged.GetType(), iArraySize);
	   for(int jj=0; jj < TypeArray.Length; jj++)
	     {
	     strLoc="Loc_0134csd" + ii + "_" + jj;
	     tpValue = TypeArray[jj];
	     arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	     try
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + tpValue.Name);
	       ++iCountTestcases;
	       Array.Copy(arrSourceDoubleJagged, arrTarget, iArraySize);
	       for(int l=0; l< arrSourceDoubleJagged.Length; l++)
		 {
		 ++iCountTestcases;
		 if(arrSourceDoubleJagged.GetValue(l)!=arrTarget.GetValue(l))
		   {
		   if (fExceptionInfo)
		     {
		     Console.Write( "Uninitialised values, Loc_1204xca" + ii + "_" + jj + "_" + l );
		     Console.WriteLine( ", values not the same, =="+ arrSourceDoubleJagged.GetValue(l) + ", " + arrTarget.GetValue(l));
		     }
		   }
		 }
	       }
	     catch(Exception ex)
	       {
	       if (fExceptionInfo)
		 Console.WriteLine(ex + ", Uninitialized Copy; ArrayType, Source=" + arrSourceDoubleJagged.GetType().Name + ", Target=" + arrTarget.GetType().Name);
	       }
	     }
	   if(hshArrayValue.ContainsKey(TypeArray[ii].Name))
	     {
	     if(fDebug)
	       Console.WriteLine("ii=" + ii + ", tpValue=" + TypeArray[ii]);
	     if(fDebug)
	       {
	       Console.WriteLine();
	       Console.WriteLine("[][][] Fill initialized array with values");
	       Console.WriteLine();
	       }
	     strLoc = "E634sa_" + ii ;
	     alst1 = (ArrayList)hshArrayValue[TypeArray[ii].Name];
	     for(int kk = 0; kk < alst1.Count; kk++)
	       {
	       strLoc = "E742da_" + ii + "_" + kk;
	       arrSource.SetValue(alst1[kk], kk);
	       }
	     if(fDebug)
	       {
	       Console.WriteLine();
	       Console.WriteLine("[][][] Test case 1 - This value filled array is then copied to all the possible Array types (created from the Type Array)");
	       Console.WriteLine();
	       }
	     ++iCountTestcases;
	     for(int jj=0; jj < TypeArray.Length; jj++)
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + TypeArray[jj]);
	       tpValue = TypeArray[jj];
	       arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	       try
		 {
		 Array.Copy(arrSource, arrTarget, iArraySize);
		 if(fReflection)
		   {
		   if(ii==jj)
		     {
		     for(int l=0; l< arrSource.Length; l++)
		       {
		       if(!arrSource.GetValue(l).Equals(arrTarget.GetValue(l)))
			 {
			 ++iCountErrors;
			 Console.WriteLine( s_strTFAbbrev + " Err_475aq!  values not the same, =="+ arrSource.GetValue(l) + ", " + arrTarget.GetValue(l));
			 }
		       }
		     }
		   else
		     {
		     for(int l=0; l< arrSource.Length; l++)
		       {
		       if(ReflectThroughConvert(arrSource, arrTarget, l))
			 {
			 ++iCountErrors;
			 Console.WriteLine( s_strTFAbbrev + " Err_654sx!  Values not the same " + arrSource.GetType().Name + ", Target=" + arrTarget.GetType().Name);
			 }
		       }
		     }
		   }
		 else
		   {
		   if(ii==jj)
		     {
		     for(int l=0; l< arrSource.Length; l++)
		       {
		       ++iCountTestcases;
		       if(!arrSource.GetValue(l).Equals(arrTarget.GetValue(l)))
			 {
			 ++iCountErrors;
			 Console.WriteLine( s_strTFAbbrev + " Err_376sf_" + ii + "_" + jj + "_" + l + "! values not the same, =="+ arrSource.GetValue(l) + ", " + arrTarget.GetValue(l));
			 }
		       }
		     }
		   else
		     {
		     }
		   }
		 }
	       catch(ArrayTypeMismatchException)
		 {
		 if (fExceptionInfo)
		   Console.WriteLine("ArrayType, Source=" + arrSource.GetType().Name + ", Target=" + arrTarget.GetType().Name);
		 }
	       catch(InvalidCastException)
		 {
		 if (fExceptionInfo)
		   Console.WriteLine("Invalid, Source=" + arrSource.GetType().Name + ", Target=" + arrTarget.GetType().Name);
		 }
	       catch(Exception ex)
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev + "Err_743af!  wrong exception thrown=="+ ex);
		 }
	       }
	     if(fDebug)
	       {
	       Console.WriteLine();
	       Console.WriteLine("[][][] Test case 2 - An Object array is created, filled with the values of the original type array, and a copy is done to all possible array types");
	       Console.WriteLine();
	       }
	     if(fDebug)
	       Console.WriteLine("ii=" + ii + ", tpValue=" + TypeArray[ii].Name);
	     Array objArrayForBaseType = Array.CreateInstance(typeof(System.Object), iArraySize);
	     alst1 = (ArrayList)hshArrayValue[TypeArray[ii].Name];
	     for(int kk = 0; kk < alst1.Count; kk++)
	       {
	       strLoc = "Loc276sd_" + ii + "_" + kk;
	       objArrayForBaseType.SetValue(alst1[kk], kk);
	       }
	     for(int jj=0; jj < TypeArray.Length; jj++)
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + TypeArray[jj].Name);
	       tpValue = TypeArray[jj];
	       arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	       try
		 {
		 Array.Copy(objArrayForBaseType, arrTarget, iArraySize);
		 if(ii==jj)
		   {
		   for(int l=0; l< objArrayForBaseType.Length; l++)
		     {
		     if(!objArrayForBaseType.GetValue(l).Equals(arrTarget.GetValue(l)))
		       {
		       ++iCountErrors;
		       Console.WriteLine( s_strTFAbbrev + " Err_458edf!  values not the same, =="+ objArrayForBaseType.GetValue(l) + ", " + arrTarget.GetValue(l));
		       }
		     }
		   }
		 else
		   {
		   if (fExceptionInfo)
		     {
		     for(int l=0; l< objArrayForBaseType.Length; l++)
		       {
		       Console.WriteLine( "Uninitialised values, Loc_627wm_" + ii + "_" + jj + "_" + l + ", source/target not the same, =="+ arrSource.GetValue(l) + ", " + arrTarget.GetValue(l));
		       }
		     }
		   }
		 }
	       catch(Exception ex)
		 {
		 if (fExceptionInfo)
		   Console.WriteLine(ex + " ArrayType, Source=" + arrSource.GetType().Name + ", Target=" + arrTarget.GetType().Name);
		 }
	       }
	     if(fDebug)
	       {
	       Console.WriteLine();
	       Console.WriteLine("[][][] Test case 3 - A Object array is created, filled with the values of the original type array, and a copy is done to all possible array types");
	       Console.WriteLine();
	       }
	     if(fDebug)
	       Console.WriteLine("ii=" + ii + ", tpValue=" + TypeArray[ii].Name);
	     Array varArrayForBaseType = Array.CreateInstance(typeof(System.Object), iArraySize);
	     alst1 = (ArrayList)hshArrayValue[TypeArray[ii].Name];
	     for(int kk = 0; kk < alst1.Count; kk++)
	       {
	       strLoc = "Loc459sf_" + ii + "_" + kk;
	       varArrayForBaseType.SetValue(alst1[kk], kk);
	       }
	     for(int jj=0; jj < TypeArray.Length; jj++)
	       {
	       if(fDebug)
		 Console.WriteLine("jj=" + jj + ", tpValue=" + TypeArray[jj].Name);
	       tpValue = TypeArray[jj];
	       arrTarget  = Array.CreateInstance(tpValue, iArraySize);
	       try
		 {
		 Array.Copy(varArrayForBaseType, arrTarget, iArraySize);
		 if(ii==jj)
		   {
		   for(int l=0; l< varArrayForBaseType.Length; l++)
		     {
		     if(!varArrayForBaseType.GetValue(l).Equals(arrTarget.GetValue(l)))
		       {
		       ++iCountErrors;
		       Console.WriteLine( s_strTFAbbrev + " Err_857sf_" + ii + "!  values not the same, =="+ varArrayForBaseType.GetValue(l) + ", " + arrTarget.GetValue(l));
		       }
		     }
		   }
		 else
		   {
		   if (fExceptionInfo)
		     {
		     for(int l=0; l< varArrayForBaseType.Length; l++)
		       {
		       Console.WriteLine( "Uninitialised values, Loc_627wm_" + ii + "_" + jj + "_" + l + ", source/target not the same, =="+ arrSource.GetValue(l) + ", " + arrTarget.GetValue(l));
		       }
		     }
		   }
		 }
	       catch(Exception ex)
		 {
		 if (fExceptionInfo)
		   Console.WriteLine(ex + " Err_752sdf! ArrayType, Source=" + varArrayForBaseType.GetType().Name + ", Target=" + arrTarget.GetType().Name);
		 }
	       }
	     }
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( " Error E_972qr! Unexpected Exception thrown == " + ex.ToString() + " strLoc=" + strLoc);
	   }
	 }
       strLoc="Loc_734dccaw";
       arMultiDimSrc = Array.CreateInstance(typeof(System.Int32), 10, 10);
       for(int i=0; i<10; i++)
	 {
	 for(int j=0; j<10; j++)
	   {
	   arMultiDimSrc.SetValue((Object)(i*j), i, j);
	   }
	 }
       arMultiDimTgt = Array.CreateInstance(typeof(System.Int32), 10, 10);
       Array.Copy(arMultiDimSrc, arMultiDimTgt, 100);
       for(int i=0; i<10; i++)
	 {
	 for(int j=0; j<10; j++)
	   {
	   if(!arMultiDimTgt.GetValue(i,j).Equals(i*j))
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev + " Err_0264vsd_" + i + "_" + j + "!  values not the same, ==" + arMultiDimTgt.GetValue(i,j));
	     }
	   }
	 }
       try
	 {
	 Array.Copy(arMultiDimSrc, arMultiDimTgt, 110);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_09356gdf! Exception not thrown");
	 }
       catch(ArgumentException){}
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_09356gdf! Wrong Exception thrown, "  + ex.ToString());
	 }
       ++iCountTestcases;
       arMultiDimTgt = Array.CreateInstance(typeof(System.Int32), 10, 10, 10);
       try
	 {
	 Array.Copy(arMultiDimSrc, arMultiDimTgt, 100);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_015vdf! Exception not thrown");
	 }
       catch(RankException){}
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_246ds! Wrong Exception thrown, "  + ex.ToString());
	 }
       ++iCountTestcases;
       arMultiDimTgt = Array.CreateInstance(typeof(System.String), 10, 10);
       try
	 {
	 Array.Copy(arMultiDimSrc, arMultiDimTgt, 100);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_0156vs! Exception not thrown");
	 }
       catch(ArrayTypeMismatchException){}
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + " Err_1046vs! Wrong Exception thrown, "  + ex.ToString());
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
 static Type [] TypeArray = {
   typeof(System.Boolean),
   typeof(System.Char),
   typeof(System.SByte),
   typeof(System.Byte),
   typeof(System.Int16),
   typeof(System.Int32),
   typeof(System.Int64),
   typeof(System.UInt16),
   typeof(System.UInt32),
   typeof(System.UInt64),
   typeof(System.Single),
   typeof(System.Double),
   typeof(System.DateTime),
   typeof(System.TimeSpan),
   typeof(System.Decimal),
   typeof(System.Decimal),
   typeof(System.Guid),
   typeof(System.String),
   typeof(System.Object),
   Type.GetType("Simple"),
   typeof(System.IFormattable),
   typeof(System.Int32[]),
   typeof(System.Globalization.NumberStyles),
   Type.GetType("Enum_Int"),
   Type.GetType("Enum_Short"),
   Type.GetType("Enum_Long"),
   Type.GetType("Enum_Byte"),
   typeof(System.Int32[,]),
 };
 static Missing[] msArr = {Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value, Missing.Value};
 static DBNull[] nlArr = {DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value};
 static DBNull[] emArr = {DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value, DBNull.Value};
 static Boolean[]	bArr		= {true, true, true, true, true, false, false, false, false, false};
 static Char[]			cArr	  = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'd', 'c', 'b'};
 static SByte[]		sbtArr	= {SByte.MinValue, -100, -5, 0, 5, 100, SByte.MaxValue};
 static Byte[]			btArr		= {Byte.MinValue, 0, 5, 100, Byte.MaxValue};
 static Int16[]		i16Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int32[]		i32Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int64[]		i64Arr	= {-530, Int64.MinValue, Int32.MinValue, Int16.MinValue, -127, 0, Int64.MaxValue, Int32.MaxValue, Int16.MaxValue};
 static UInt16[]		ui16Arr	= {19, 238, 317, 6, 565, 0, 52, 60, (ushort)Int16.MaxValue+1, (ushort)Int16.MaxValue};
 static UInt32[]		ui32Arr	= {19, 238, 317, 6, 565, 0, 52, 60, (uint) (Object)(((UInt32)Int32.MaxValue)+1), (UInt32)Int32.MaxValue};
 static UInt64[]		ui64Arr	= {530, UInt64.MinValue, UInt32.MinValue, UInt16.MinValue, 127, 0, UInt64.MaxValue, Int32.MaxValue, UInt16.MaxValue};
 static Single[] 	fArr    = {-1.2e23f, 1.2e-32f, -1.23f, 0.0f, 1.23e23f, 1.23f, 0.0f, 2.45f, 1.2f, -5.6f};
 static Double[] 	dArr    = {-1.2e23, 1.2e-32, -1.23, 0.0, 3.4, -1.2e23, 1.2e-32, -1.23, 0.0, 3.4};
 static DateTime[] dtArr	  = {new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54),
				     new DateTime(1999, 5, 7), new DateTime(1999, 5, 7, 7, 36, 54)};
 static TimeSpan[] tsArr	  = {TimeSpan.MinValue, TimeSpan.MinValue,
				     TimeSpan.MinValue, TimeSpan.MinValue,
				     TimeSpan.MinValue, TimeSpan.MinValue,
				     TimeSpan.MinValue, TimeSpan.MinValue,
				     TimeSpan.MinValue, TimeSpan.MinValue};
 static Decimal[] dcmArr  = {Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue,
			     Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue};
 static Decimal[] curArr  = {Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue,
			     Decimal.MinValue, Decimal.MinusOne, Decimal.Zero, Decimal.One, Decimal.MaxValue};
 static Guid[] gdArr	  = {Guid.Empty, Guid.Empty,
			     Guid.Empty, Guid.Empty,
			     Guid.Empty, Guid.Empty,
			     Guid.Empty, Guid.Empty,
			     Guid.Empty, Guid.Empty};
 static String[]   strArr  = {"This", " ", "a", " ", "test", " ", "of", " ", "patience", "."};
 static Object[]	oArr      = {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 static Simple[] simArr	  = {new Simple(), new Simple(),
			     new Simple(), new Simple(),
			     new Simple(), new Simple(),
			     new Simple(), new Simple(),
			     new Simple(), new Simple()};
 IFormattable[] ifmtArr = {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 NumberStyles[] enumNumStyles = {NumberStyles.AllowCurrencySymbol, NumberStyles.AllowDecimalPoint,
				 NumberStyles.AllowExponent, NumberStyles.AllowLeadingSign,
				 NumberStyles.AllowLeadingWhite, NumberStyles.AllowParentheses,
				 NumberStyles.AllowThousands, NumberStyles.AllowTrailingSign,
				 NumberStyles.AllowTrailingWhite, NumberStyles.Any
 };
 Enum_Int[] enumInt32 = {Enum_Int.A, Enum_Int.B, Enum_Int.C, Enum_Int.D, Enum_Int.E, Enum_Int.F, Enum_Int.G, Enum_Int.H, Enum_Int.I, Enum_Int.J};
 Enum_Short[] enumInt16 = {Enum_Short.A, Enum_Short.B, Enum_Short.C, Enum_Short.D, Enum_Short.E, Enum_Short.F, Enum_Short.G, Enum_Short.H, Enum_Short.I, Enum_Short.J};
 Enum_Long[] enumInt64 = {Enum_Long.A, Enum_Long.B, Enum_Long.C, Enum_Long.D, Enum_Long.E, Enum_Long.F, Enum_Long.G, Enum_Long.H, Enum_Long.I, Enum_Long.J};
 Enum_Byte[] enumInt8 = {Enum_Byte.A, Enum_Byte.B, Enum_Byte.C, Enum_Byte.D, Enum_Byte.E, Enum_Byte.F, Enum_Byte.G, Enum_Byte.H, Enum_Byte.I, Enum_Byte.J};
 private void GoFillSomeTestValues(Hashtable hshArrayValue)
   {
   ArrayList alst = new ArrayList();
   for(int ii=0; ii < msArr.Length; ii++)
     {
     alst.Add(msArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Reflection.Missing).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < nlArr.Length; ii++)
     {
     alst.Add(nlArr[ii]);
     }
   hshArrayValue.Add(typeof(System.DBNull).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < bArr.Length; ii++)
     {
     alst.Add(bArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Boolean).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < cArr.Length; ii++)
     {
     alst.Add(cArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Char).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < sbtArr.Length; ii++)
     {
     alst.Add(sbtArr[ii]);
     }
   hshArrayValue.Add(typeof(System.SByte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < btArr.Length; ii++)
     {
     alst.Add(btArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Byte).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i16Arr.Length; ii++)
     {
     alst.Add(i16Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.Int16).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i32Arr.Length; ii++)
     {
     alst.Add(i32Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.Int32).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < i64Arr.Length; ii++)
     {
     alst.Add(i64Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.Int64).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < ui16Arr.Length; ii++)
     {
     alst.Add(ui16Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.UInt16).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < ui32Arr.Length; ii++)
     {
     alst.Add(ui32Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.UInt32).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < ui64Arr.Length; ii++)
     {
     alst.Add(ui64Arr[ii]);
     }
   hshArrayValue.Add(typeof(System.UInt64).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < fArr.Length; ii++)
     {
     alst.Add(fArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Single).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dArr.Length; ii++)
     {
     alst.Add(dArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Double).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dtArr.Length; ii++)
     {
     alst.Add(dtArr[ii]);
     }
   hshArrayValue.Add(typeof(System.DateTime).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < tsArr.Length; ii++)
     {
     alst.Add(tsArr[ii]);
     }
   hshArrayValue.Add(typeof(System.TimeSpan).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < dcmArr.Length; ii++)
     {
     alst.Add(dcmArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Decimal).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < gdArr.Length; ii++)
     {
     alst.Add(gdArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Guid).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < strArr.Length; ii++)
     {
     alst.Add(strArr[ii]);
     }
   hshArrayValue.Add(typeof(System.String).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < oArr.Length; ii++)
     {
     alst.Add(oArr[ii]);
     }
   hshArrayValue.Add(typeof(System.Object).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < simArr.Length; ii++)
     {
     alst.Add(simArr[ii]);
     }
   hshArrayValue.Add(Type.GetType("Simple").Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < ifmtArr.Length; ii++)
     {
     alst.Add(ifmtArr[ii]);
     }
   hshArrayValue.Add(typeof(System.IFormattable).Name, alst);
   alst = new ArrayList();
   Int32[] iAr1Jag;
   int nNumberOfArrays = 10;
   for(int j=10; j < (10 + nNumberOfArrays); j++)
     {
     iAr1Jag = new Int32[j];
     for(int i=0;i<j;i++)
       iAr1Jag[i]=i;
     alst.Add(iAr1Jag);
     }
   hshArrayValue.Add(typeof(System.Int32[]).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < enumNumStyles.Length; ii++)
     {
     alst.Add(enumNumStyles[ii]);
     }
   hshArrayValue.Add(typeof(System.Globalization.NumberStyles).Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < enumInt32.Length; ii++)
     {
     alst.Add(enumInt32[ii]);
     }
   hshArrayValue.Add(Type.GetType("Enum_Int").Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < enumInt16.Length; ii++)
     {
     alst.Add(enumInt16[ii]);
     }
   hshArrayValue.Add(Type.GetType("Enum_Short").Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < enumInt64.Length; ii++)
     {
     alst.Add(enumInt64[ii]);
     }
   hshArrayValue.Add(Type.GetType("Enum_Long").Name, alst);
   alst = new ArrayList();
   for(int ii=0; ii < enumInt8.Length; ii++)
     {
     alst.Add(enumInt8[ii]);
     }
   hshArrayValue.Add(Type.GetType("Enum_Byte").Name, alst);
   }
 private bool ReflectThroughConvert(Array source, Array target, int iArrayElement)
   {
   Type tpTargetArrayElement = target.GetType().GetElementType();
   String strToXXX = "To" + tpTargetArrayElement.Name.Trim();
   Type tpConvert = typeof(System.Convert);
   Object[] vntSourceElemValue = new Object[1];
   vntSourceElemValue[0] = source.GetValue(iArrayElement);
   Object vntConvertedSourceValue = null;
   try
     {
     vntConvertedSourceValue = tpConvert.InvokeMember(strToXXX, BindingFlags.InvokeMethod, null, null, vntSourceElemValue);
     }catch(MissingMethodException) {
     return false;
     }
   Object vntTargetValue = target.GetValue(iArrayElement);
   if(!vntConvertedSourceValue.Equals(vntTargetValue))
     {
     Console.WriteLine();
     Console.WriteLine("reflected target = " + vntConvertedSourceValue + ", type = " + vntConvertedSourceValue.GetType() + ", strToXXX = " + strToXXX);
     Console.WriteLine("real target = " + vntTargetValue + ", type = " + vntTargetValue.GetType());
     Console.WriteLine("source array type = " + source.GetType().GetElementType() + ", target array type = " + target.GetType().GetElementType());
     Console.WriteLine();
     }
   return false;
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3756Copy_aai oCbTest = new Co3756Copy_aai();
   try
     {
     Console.WriteLine();
     Console.WriteLine("CommandLine Arg Information:");
     Console.WriteLine("\t1 - For all Array Copy Exception information (Source and Target types)");
     Console.WriteLine("\t2 - Array copy to be confirmed through reflection (using Convert.ToXXX)");
     Console.WriteLine("\t3 - Debug Information; Point to where the error is occuring");
     Console.WriteLine();
     oCbTest.fExceptionInfo=false;
     oCbTest.fReflection=false;
     for ( int aa = 0 ; aa < args.Length ;aa++ )
       {
       if ( args[aa].Equals( "1" ) == true )
	 oCbTest.fExceptionInfo=true;
       else if ( args[aa].Equals( "2" ) == true )
	 oCbTest.fReflection=true;
       else if ( args[aa].Equals( "3" ) == true )
	 oCbTest.fDebug=true;
       }
     DateTime dtStartedTime = DateTime.Now;
     bResult = oCbTest.runTest();
     Console.WriteLine("Execution Time(mm:ss)=" + (new DateTime((DateTime.Now-dtStartedTime).Ticks)).Minute + ":" + (new DateTime((DateTime.Now-dtStartedTime).Ticks)).Second);
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
public class Simple
{
 public Simple() { m_oObject = "Hello World";}
 Object m_oObject;
}
enum Enum_Int {
  A = 1,
  B = 2,
  C = 3,
  D = 4,
  E = 5,
  F = 6,
  G = 7,
  H = 8,
  I = 9,
  J = 0,
}
enum Enum_Short {
  A = 1,
  B = 2,
  C = 3,
  D = 4,
  E = 5,
  F = 6,
  G = 7,
  H = 8,
  I = 9,
  J = 0,
}
enum Enum_Long {
  A = 1,
  B = 2,
  C = 3,
  D = 4,
  E = 5,
  F = 6,
  G = 7,
  H = 8,
  I = 9,
  J = 0,
}
enum Enum_Byte {
  A = 1,
  B = 2,
  C = 3,
  D = 4,
  E = 5,
  F = 6,
  G = 7,
  H = 8,
  I = 9,
  J = 0,
}
