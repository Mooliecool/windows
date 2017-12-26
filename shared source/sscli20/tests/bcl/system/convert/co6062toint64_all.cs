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
public class Co6062ToInt64_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(String), typeof(Int32)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToInt64", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToInt64( all )";
 public static String s_strTFName        = "Cb6062ToInt64_all";
 public static String s_strTFAbbrev      = "Cb6062";
 public static String s_strTFPath        = "";
 public Boolean verbose = false;
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
 public Boolean runTest() {
 Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
 String strLoc="Loc_000oo";
 int iCountTestcases = 0;
 int iCountErrors = 0;
 printoutCoveredMethods();
 if ( verbose ) Console.WriteLine( "Testing Method: Boolean" );
 try {
 Boolean[]   testValues = { 
   true,
   false,
 };
 Int64[]   expectedValues = { 
   (Int64)1,
   (Int64)0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vboolAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xboolAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountTestcases++;
 Console.WriteLine( "Error_49ygf! Uncaught Exception in Int64 Convert.ToInt64( Boolean )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Double" );
 try {
 Double[]   testValues = { 
   100.0,
   -100.0,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vdoubAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdoubAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
 };
 Double[]   errorValues = { 
   Double.MaxValue,
   Double.MinValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXdoubArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXdoubArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_98hcy! Uncaught Exception in Int64 Convert.ToInt64( Double )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Single" );
 try {
 Single[]   testValues = { 
   100.0f,
   -100.0f,
   0.0f,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
 };
 Single[]   errorValues = { 
   Single.MaxValue,
   Single.MinValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXsingArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXsingArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_98dym! Uncaught Exception in Int64 Convert.ToInt64( Single )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 Int16[]   testValues = { 
   100 ,
   -100 ,
   0 ,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 Console.WriteLine( "Error_9xs77! Uncaught Exception in Int64 Convert.ToInt64( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int32" );
 try {
 Int32[]   testValues = { 
   100,
   -100,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 Console.WriteLine( "Error_9509v! Uncaught Exception in Int64 Convert.ToInt64( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Decimal" );
 try {
 Decimal[]   testValues = { 
   new Decimal( 100 ),
   new Decimal( -100 ),
   new Decimal( 0.0 ),
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
 };
 Decimal[]   errorValues = { 
   Decimal.MaxValue,
   Decimal.MinValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXdeciArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXdeciArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_97x9o! Uncaught Exception in Int64 Convert.ToInt64( Decimal )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "100",
   "-100",
   "0",
   null,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
   0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vstriAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
   "System.FormatException",
 };
 String[]   errorValues = { 
   "1" + Int64.MaxValue.ToString(),
   Int64.MinValue.ToString()+ "1",
   "abba",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i] );
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
 Console.WriteLine( "Error_490ay! Uncaught Exception in Int64 Convert.ToInt64( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "100",
   "-100",
   "0",
   null,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i], new NumberFormatInfo() );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vstriAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
   "System.FormatException",
 };
 String[]   errorValues = { 
   "1" + Int64.MaxValue.ToString(),
   Int64.MinValue.ToString()+ "1",
   "abba",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i], new NumberFormatInfo() );
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
 Console.WriteLine( "Error_490ay! Uncaught Exception in Int64 Convert.ToInt64( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Byte" );
 try {
 Byte[]   testValues = { 
   Byte.MaxValue,
   Byte.MinValue,
 };
 Int64[]   expectedValues = { 
   255 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 Console.WriteLine( "ERror_698fj! Uncaught Exception in Int64 Convert.ToInt64( Byte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String, Int32" );
 try {
 String [] dummy = { null, }; 
 Int64 result = Convert.ToInt64( dummy[0], 10 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }
 result = Convert.ToInt64( dummy[0], 2 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToInt64( dummy[0], 8 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToInt64( dummy[0], 16 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 try {
 String[]   testValues = { 
   "7FFFFFFFFFFFFFFF",
   "9223372036854775807",
   "777777777777777777777",
   "111111111111111111111111111111111111111111111111111111111111111",
   "8000000000000000",
   "-9223372036854775808",
   "1000000000000000000000",
   "1000000000000000000000000000000000000000000000000000000000000000",
 };
 Int32[]    testBases = { 
   16,
   10,
   8,
   2,
   16,
   10,
   8,
   2,
 };
 Int64[]   expectedValues = { 
   Int64.MaxValue,
   Int64.MaxValue,
   Int64.MaxValue,
   Int64.MaxValue,
   Int64.MinValue,
   Int64.MinValue,
   Int64.MinValue,
   Int64.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i], testBases[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vstr2Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstr2Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 String[] expectedExceptions = { 
   "System.FormatException",
   "System.ArgumentException",
   "System.ArgumentException",
   "System.FormatException",
 };
 String[]   errorValues = { 
   "12",
   "11",
   "abba",
   "ffffffffffffffffffff"
 };
 Int32[]   errorBases = { 
   2,
   3,
   5,
   8,
   16,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i], errorBases[i] );
 iCountErrors++;
 strLoc = "Err_EXstr2ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXstr2ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "ERror_047ch! Uncaught Exception in Int64 Convert.ToInt64( str2int )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 SByte[]   testValues = { 
   100,
   -100,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   -100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint8Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint8Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_433jk! Uncaught Exception in Int64 Convert.ToInt64( SByte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16" );
 try {
 UInt16[]   testValues = { 
   100,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vuInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xuInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_98987! Uncaught Exception in Int64 Convert.ToInt64( UInt16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt32" );
 try {
 UInt32[]   testValues = { 
   100,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 Console.WriteLine( "Error_984yy! Uncaught Exception in Int64 Convert.ToInt64( UInt32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt64" );
 try {
 UInt64[]   testValues = { 
   100,
   0,
 };
 Int64[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
 };
 UInt64[]   errorValues = { 
   UInt64.MaxValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int64 result = Convert.ToInt64( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXUInt64ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXUInt64ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_848ey! Uncaught Exception in Int64 Convert.ToInt64( UInt64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int64" );
 try {
 Int64[]   testValues = { 
   Int64.MaxValue,Int64.MinValue, 0
 };
 Int64[]   expectedValues = { 
   Int64.MaxValue,Int64.MinValue, 0
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] + "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xInt64Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4789y! Uncaught Exception in Byte Convert.ToInt64( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try {
 Char[]   testValues = { 
   Char.MaxValue,
   Char.MinValue,
   'b'
 };
 Int64[]   expectedValues = { 
   (Int64)Char.MaxValue,
   (Int64)Char.MinValue,
   98
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int64 result = Convert.ToInt64( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vcharAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xcharAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4789y! Uncaught Exception in Int64 Convert.ToInt64( Char )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Int64 bTest = Convert.ToInt64 (new Object());	
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
   Int64 bTest = Convert.ToInt64 (new Object(), new NumberFormatInfo());	
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
   Int64 bTest = Convert.ToInt64 (DateTime.Now);	
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
   Boolean bResult = false; 
   Co6062ToInt64_all cbX = new Co6062ToInt64_all();
   try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } catch( Exception ) {}
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
