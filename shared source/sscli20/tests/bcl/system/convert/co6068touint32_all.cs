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
class Co6068ToUInt32_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(String), typeof(Int32)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToUInt32", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToUInt32( all )";
 public static String s_strTFName        = "Cb6068ToUInt32_all";
 public static String s_strTFAbbrev      = "Cb6068";
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
 UInt32[]   expectedValues = { 
   ((UInt32) 1 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 iCountErrors++;
 Console.WriteLine( "Error_987gy! Uncaught Exception in UInt16 Convert.ToUInt32( Boolean )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try {
 Char[]   testValues = { 
   Char.MinValue,
   Char.MaxValue,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) Char.MinValue ),
   ((UInt32) Char.MaxValue ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 Console.WriteLine( "Error_98y5c! Uncaught Exception in UInt32 Convert.ToUInt32( Char )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Double" );
 try {
 Double[]   testValues = { 
   1000.0,
   0.0,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
   -100.0,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_9857c! Uncaught Exception in UInt16 Convert.ToUInt32( Double )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Single" );
 try {
 Single[]   testValues = { 
   ((Single) 1000.0f ),
   ((Single) 0.0f ),
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
   -100.0f,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_47fcq! Uncaught Exception in UInt16 Convert.ToUInt32( Single )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int32" );
 try {
 Int32[]   testValues = { 
   ((Int32) 1000 ),
   ((Int32) 0 ),
   Int32.MaxValue,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
   ((UInt32) Int32.MaxValue ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 };
 Int32[]   errorValues = { 
   ((Int32) Int32.MinValue ),
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_497gy! Uncaught Exception in UInt16 Convert.ToUInt32( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int64" );
 try {
 Int64[]   testValues = { 
   ((Int64) 1000 ),
   ((Int64) 0 ),
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
   ((Int64) Int64.MaxValue ),
   ((Int64) Int64.MinValue ),
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 iCountErrors++;
 Console.WriteLine( "Error_49876! Uncaught Exception in UInt16 Convert.ToUInt32( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 Int16[]   testValues = { 
   ((Int16) 1000 ),
   ((Int16) 0 ),
   Int16.MaxValue,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
   ((UInt32) (UInt32)Int16.MaxValue ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
   ((Int16) Int16.MinValue ),
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 iCountErrors++;
 Console.WriteLine( "Error_09fyw! Uncaught Exception in UInt16 Convert.ToUInt32( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Decimal" );
 try {
 Decimal[]   testValues = { 
   new Decimal( 1000 ),
   new Decimal( 0 ),
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_98g78! Uncaught Exception in UInt16 Convert.ToUInt32( Decimal )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "1000",
   "0",
   UInt16.MaxValue.ToString(),
   UInt32.MaxValue.ToString(),
   null,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
   UInt16.MaxValue,
   UInt32.MaxValue,
   0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_958cy! Uncaught Exception in UInt16 Convert.ToUInt32( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String[]   testValues = { 
   "1000",
   "0",
   UInt16.MaxValue.ToString(),
   UInt32.MaxValue.ToString(),
   null,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 1000 ),
   ((UInt32) 0 ),
   UInt16.MaxValue,
   UInt32.MaxValue,
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i], new NumberFormatInfo() );
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
 UInt32 result = Convert.ToUInt32( errorValues[i], new NumberFormatInfo() );
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
 Console.WriteLine( "Error_958cy! Uncaught Exception in UInt16 Convert.ToUInt32( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Byte" );
 try {
 Byte[]   testValues = { 
   Byte.MaxValue,
   Byte.MinValue,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 255 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 Console.WriteLine( "Error_598dh! Uncaught Exception in UInt16 Convert.ToUInt32( Byte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String, Int32" );
 try {
 String [] dummy = { null, }; 
 UInt32 result = Convert.ToUInt32( dummy[0], 10 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }
 result = Convert.ToUInt32( dummy[0], 2 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToUInt32( dummy[0], 8 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToUInt32( dummy[0], 16 );
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
   "ffffffff",
   "4294967295",
   "37777777777",
   "11111111111111111111111111111111",
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
 UInt32[]   expectedValues = { 
   UInt32.MaxValue,
   UInt32.MaxValue,
   UInt32.MaxValue,
   UInt32.MaxValue,
   UInt32.MinValue,
   UInt32.MinValue,
   UInt32.MinValue,
   UInt32.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i], testBases[i] );
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
 UInt32 result = Convert.ToUInt32( errorValues[i], errorBases[i] );
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
 Console.WriteLine( "Error_98934! Uncaught Exception in UInt32 Convert.ToUInt32( str2int )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 SByte[]   testValues = { 
   100,
   0,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 100 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_433jk! Uncaught Exception in UInt32 Convert.ToUInt32( SByte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16" );
 try {
 UInt16[]   testValues = { 
   (UInt16)100,
   (UInt16)0,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 100 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 Console.WriteLine( "Error_988yd! Uncaught Exception in UInt32 Convert.ToUInt32( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt64" );
 try {
 UInt64[]   testValues = { 
   100,
   0,
 };
 UInt32[]   expectedValues = { 
   ((UInt32) 100 ),
   ((UInt32) 0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
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
 UInt32 result = Convert.ToUInt32( errorValues[i] );
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
 Console.WriteLine( "Error_848ey! Uncaught Exception in UInt32 Convert.ToUInt32( UInt64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt32" );
 try {
 UInt32[]   testValues = { 
   UInt32.MaxValue,UInt32.MinValue
 };
 UInt32[]   expectedValues = { 
   UInt32.MaxValue,UInt32.MinValue
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 UInt32 result = Convert.ToUInt32( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vUInt32Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] + "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt32Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4789y! Uncaught Exception in Byte Convert.ToUInt32( UInt32 )");
 Console.WriteLine( "Exception->" + e.GetType().ToString() );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   UInt32 bTest = Convert.ToUInt32 (new Object());	
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
   UInt32 bTest = Convert.ToUInt32 (new Object(), new NumberFormatInfo());	
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
   UInt32 bTest = Convert.ToUInt32 (DateTime.Now);	
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
   Co6068ToUInt32_all cbX = new Co6068ToUInt32_all();
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
