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
using System.Reflection;
using System.Collections;
using System.Threading;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
class Co6059ToDouble_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToDouble", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Convert.ToDecimal( all )";
 public static readonly String s_strTFName        = "Cb6059ToDouble_all";
 public static readonly String s_strTFAbbrev      = "Cb6059";
 public static readonly String s_strTFPath        = "";
 public bool verbose = false;
 public void printoutCoveredMethods()
   {
   Console.Error.WriteLine( "" );
   MemberInfo[] mmm  = GetTestedMethods();
   Console.Error.WriteLine( "Method_Count== ("+ mmm.Length +"==confirm) !!" );
   Console.Error.WriteLine( "" );
   foreach(MemberInfo mm in mmm)
     Console.WriteLine(mm);
   Console.Error.WriteLine( "" );
   }
 public virtual Boolean runTest() {
 Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
 String strLoc="Loc_000oo";
 int iCountTestcases = 0;
 int iCountErrors = 0;
 printoutCoveredMethods();
 if ( verbose ) Console.WriteLine( "Testing Method: Single" );
 try {
 Single[]   testValues = {
   Single.MaxValue,
   Single.MinValue,
   ((Single) 0.0f ),
 };
 Double[]   expectedValues = {
   ((Double) Single.MaxValue ),
   ((Double) Single.MinValue ),
   ((Double) 0.0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vsingAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xsingAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_498hx! Uncaught Exception in Double Convert.ToDouble( Single )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int32" );
 try {
 Int32[]   testValues = {
   Int32.MaxValue,
   Int32.MinValue,
   0,
 };
 Double[]   expectedValues = {
   ((Double) Int32.MaxValue ),
   ((Double) Int32.MinValue ),
   ((Double) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint3Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint3Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_498ha! Uncaught Exception in Double Convert.ToDouble( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int64" );
 try {
 Int64[]   testValues = {
   Int64.MaxValue,
   Int64.MinValue,
   (long) 0,
 };
 Double[]   expectedValues = {
   (Double)Int64.MaxValue,
   (Double)Int64.MinValue,
   ((Double) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint6Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint6Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4098x! Uncaught Exception in Double Convert.ToDouble( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 Int16[]   testValues = {
   Int16.MaxValue,
   Int16.MinValue,
   (short) 0,
 };
 Double[]   expectedValues = {
   (Double)Int16.MaxValue,
   (Double)Int16.MinValue,
   ((Double) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_498cy! Uncaught Exception in Double Convert.ToDouble( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 Boolean[]   testValues = {
   true,
   false,
 };
 Double[]   expectedValues = {
   1.0,
   0.0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_498cy! Uncaught Exception in Double Convert.ToDouble( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Decimal" );
 try {
 Decimal[]   testValues = {
   Decimal.MaxValue,
   Decimal.MinValue,
   new Decimal( 0 ),
 };
 Double[]   expectedValues = {
   (Double)Decimal.MaxValue ,
   (Double)Decimal.MinValue ,
   (Double)(new Decimal( 0 )),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vdeciAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdeciAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_409ja! Uncaught Exception in Double Convert.ToDouble( Decimal )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = {
   0.0.ToString(),
   10.0.ToString(),
   (-10.0).ToString(),
   null,
 };
 Double[]   expectedValues = {
   0.0,
   10.0,
   -10.0,
   0.0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_3497tsfg," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String [] expectedExceptions = {
   "System.FormatException",
   "System.OverflowException",
   "System.OverflowException",
   "System.OverflowException",
   "System.OverflowException",
 };
 String[]   errorValues = {
   "123xyz",
   Double.MaxValue.ToString(),
   Double.MinValue.ToString(),
   "1.7976931348623155E309",
   "-1.7976931348623155E309",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + errorValues[i] + " should throw " + expectedExceptions[i] );
 Double result = Convert.ToDouble( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXstriArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_908hs! Uncaught Exception in Double Convert.ToDouble( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = {
   "0.0",
   "10.0",
   "-10.0",
   null,
 };
 Double[]   expectedValues = {
   0.0,
   10.0,
   -10.0,
   0.0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i], CultureInfo.InvariantCulture );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_234907sg," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String [] expectedExceptions = {
   "System.FormatException",
   "System.OverflowException",       
   "System.OverflowException",       
   "System.OverflowException",       
   "System.OverflowException",       
 };
 String[]   errorValues = {
   "123xyz",
   Double.MaxValue.ToString(),
   Double.MinValue.ToString(),
   "1.7976931348623155E309",
   "-1.7976931348623155E309",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + errorValues[i] + " should throw " + expectedExceptions[i] );
 Double result = Convert.ToDouble( errorValues[i], new NumberFormatInfo() );
 iCountErrors++;
 strLoc = "Err_EXstriArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_908hs! Uncaught Exception in Double Convert.ToDouble( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Byte" );
 try {
 Byte[]   testValues = {
   Byte.MaxValue,
   Byte.MinValue,
 };
 Double[]   expectedValues = {
   (Double)Byte.MaxValue,
   (Double)Byte.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vbyteAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xbyteAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_39a83! Uncaught Exception in Double Convert.ToDouble( Byte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt64" );
 try {
 UInt64[]   testValues = {
   UInt64.MaxValue,
   UInt64.MinValue,
 };
 Double[]   expectedValues = {
   (Double)UInt64.MaxValue,
   (Double)UInt64.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vUInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_908uq! Uncaught Exception in Double Convert.ToDouble( UInt64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt32" );
 try {
 UInt32[]   testValues = {
   UInt32.MaxValue,
   UInt32.MinValue,
 };
 Double[]   expectedValues = {
   (Double)UInt32.MaxValue,
   (Double)UInt32.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vUInt32Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt32Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_09zy3! Uncaught Exception in Double Convert.ToDouble( UInt32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16" );
 try {
 UInt16[]   testValues = {
   UInt16.MaxValue,
   UInt16.MinValue,
 };
 Double[]   expectedValues = {
   (Double)UInt16.MaxValue,
   (Double)UInt16.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vUInt16Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt16Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_09j49! Uncaught Exception in Double Convert.ToDouble( UInt16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 SByte[]   testValues = {
   SByte.MaxValue,
   SByte.MinValue,
 };
 Double[]   expectedValues = {
   (Double)SByte.MaxValue,
   (Double)SByte.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_09j49! Uncaught Exception in Double Convert.ToDouble( SByte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try {
 String[] expectedExceptions = { 
   "System.InvalidCastException",
   "System.InvalidCastException",
   "System.InvalidCastException",
 };
 Char[]   errorValues = { 
   Char.MinValue,
   Char.MaxValue,
   'b'
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Double result = Convert.ToDouble( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXstriArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4098x! Uncaught Exception in Double Convert.ToDouble( Char )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 Double[]   testValues = {
   Double.MaxValue,Double.MinValue, Double.NegativeInfinity, Double.PositiveInfinity, Double.Epsilon
 };
 Double[]   expectedValues = {
   Double.MaxValue,Double.MinValue, Double.NegativeInfinity, Double.PositiveInfinity, Double.Epsilon
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Double result = Convert.ToDouble( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vSByteAr," + i;
 Console.Error.WriteLine( strLoc +" Sent In = '" + testValues[i] +  "' Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_09j49! Uncaught Exception in Double Convert.ToDouble( SByte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Double bTest = Convert.ToDouble (new Object());	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Double bTest = Convert.ToDouble (new Object(), new NumberFormatInfo());	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Double bTest = Convert.ToDouble (DateTime.Now);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 Console.Error.Write( s_strTFName );
 Console.Error.Write( ": " );
 if ( iCountErrors == 0 )
   {
   Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases + " paSs" );
   return true;
   }
 else
   {
   Console.Error.WriteLine( s_strTFPath + s_strTFName + ".cs" );
   Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases );
   Console.Error.WriteLine( "FAiL" );
   Console.Error.WriteLine( " iCountErrors==" + iCountErrors );
   return false;
   }
 }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co6059ToDouble_all cbX = new Co6059ToDouble_all();
   try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } catch( Exception) {}
   try
     {
     bResult = cbX.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFPath + s_strTFName );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "Try '" + s_strTFName + ".exe -v' to see tests..." );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
