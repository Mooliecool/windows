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
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co6067ToUInt16_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(String), typeof(Int32)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToUInt16", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToUInt16( all )";
 public static String s_strTFName        = "Cb6067ToUInt16_all";
 public static String s_strTFAbbrev      = "Cb6067";
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
 UInt16[]   expectedValues = { 
   1 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Boolean )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Double" );
 try {
 Double[]   testValues = { 
   1000.0,
   0.0,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
   -100.0 ,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Double )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Single" );
 try {
 Single[]   testValues = { 
   1000.0f ,
   0.0f ,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
   -100.0f ,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Single )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int32" );
 try {
 Int32[]   testValues = { 
   1000 ,
   0 ,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
 };
 Int32[]   errorValues = { 
   Int32.MinValue ,
   Int32.MaxValue ,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXint3ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int64" );
 try {
 Int64[]   testValues = { 
   1000 ,
   0 ,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
   "System.OverflowException",
 };
 Int64[]   errorValues = { 
   Int64.MaxValue ,
   Int64.MinValue ,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXint6ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXint6ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 Int16[]   testValues = { 
   1000 ,
   0 ,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
 };
 Int16[]   errorValues = { 
   Int16.MinValue ,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXint1ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXint1ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Decimal" );
 try {
 Decimal[]   testValues = { 
   new Decimal( 1000 ),
   new Decimal( 0 ),
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 UInt16 result = Convert.ToUInt16( errorValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Decimal )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "1000",
   "0",
   UInt16.MaxValue.ToString(),
   null,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
   UInt16.MaxValue,
   0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
   "-1",
   Decimal.MaxValue.ToString(),
   "abba",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "1000",
   "0",
   UInt16.MaxValue.ToString(),
   null,
 };
 UInt16[]   expectedValues = { 
   1000 ,
   0 ,
   UInt16.MaxValue,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i], new NumberFormatInfo() );
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
   "-1",
   Decimal.MaxValue.ToString(),
   "abba",
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i], new NumberFormatInfo() );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Byte" );
 try {
 Byte[]   testValues = { 
   Byte.MaxValue,
   Byte.MinValue,
 };
 UInt16[]   expectedValues = { 
   255 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 Console.WriteLine( "Uncaught Exception in UInt16 Convert.ToUInt16( Byte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String, Int32" );
 try {
 String [] dummy = { null, }; 
 UInt16 result = Convert.ToUInt16( dummy[0], 10 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }
 result = Convert.ToUInt16( dummy[0], 2 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToUInt16( dummy[0], 8 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToUInt16( dummy[0], 16 );
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
   "ffff",
   "65535",
   "177777",
   "1111111111111111",
   "0",
   "0",
   "0",
   "0",
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
 UInt16[]   expectedValues = { 
   UInt16.MaxValue,
   UInt16.MaxValue,
   UInt16.MaxValue,
   UInt16.MaxValue,
   UInt16.MinValue,
   UInt16.MinValue,
   UInt16.MinValue,
   UInt16.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i], testBases[i] );
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
 UInt16 result = Convert.ToUInt16( errorValues[i], errorBases[i] );
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
 Console.WriteLine( "Error_98934! Uncaught Exception in UInt16 Convert.ToUInt16( str2int )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 SByte[]   testValues = { 
   100,
   0,
 };
 UInt16[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
 };
 SByte[]   errorValues = { 
   SByte.MinValue
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXint3ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_433jk! Uncaught Exception in UInt16 Convert.ToUInt16( SByte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt32" );
 try {
 UInt32[]   testValues = { 
   100,
   0,
 };
 UInt16[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 String[] expectedExceptions = { 
   "System.OverflowException",
 };
 UInt32[]   errorValues = { 
   UInt32.MaxValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt16 result = Convert.ToUInt16( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_EXUInt32ArNoE," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArWrE," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_984yy! Uncaught Exception in UInt16 Convert.ToUInt16( UInt32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt64" );
 try {
 UInt64[]   testValues = { 
   100,
   0,
 };
 UInt16[]   expectedValues = { 
   100 ,
   0 ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 UInt16 result = Convert.ToUInt16( errorValues[i] );
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
 Console.WriteLine( "Error_848ey! Uncaught Exception in UInt16 Convert.ToUInt16( UInt64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16" );
 try {
 UInt16[]   testValues = { 
   UInt16.MaxValue,UInt16.MinValue
 };
 UInt16[]   expectedValues = { 
   UInt16.MaxValue,UInt16.MinValue
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vUInt16Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] + "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt16Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4789y! Uncaught Exception in Byte Convert.ToUInt16( UInt16 )");
 Console.WriteLine( "Exception->" + e.GetType().ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try {
 Char[]   testValues = { 
   Char.MaxValue,
   Char.MinValue,
   'b'
 };
 UInt16[]   expectedValues = { 
   (UInt16)Char.MaxValue,
   (UInt16)Char.MinValue,
   98
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt16 result = Convert.ToUInt16( testValues[i] );
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
 Console.WriteLine( "Error_4789y! Uncaught Exception in UInt16 Convert.ToUInt16( Char )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   UInt16 bTest = Convert.ToUInt16 (new Object());	
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
   UInt16 bTest = Convert.ToUInt16 (new Object(), new NumberFormatInfo());	
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
   UInt16 bTest = Convert.ToUInt16 (DateTime.Now);	
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
   Co6067ToUInt16_all cbX = new Co6067ToUInt16_all();
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
