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
class Co6055ToChar_all  : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToChar", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToChar( all )";
 public static String s_strTFName        = "Cb6055ToChar_all";
 public static String s_strTFAbbrev      = "Cb6055";
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
 if ( verbose ) Console.WriteLine( "Testing Method: Int32" );
 try {
 String expected = null;
 Int32[]   testValues = { 
   (Int32)Char.MaxValue,
   (Int32)Char.MinValue,
 };
 Char[]   expectedValues = { 
   Char.MaxValue,
   Char.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
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
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( Int32.MaxValue );
 iCountErrors++;
 strLoc = "Err_EXint3ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( -1000  );
 iCountErrors++;
 strLoc = "Err_EXint3ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXint3ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_497dy! Uncaught Exception in Char Convert.ToChar( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String expected = null;
 String[]   testValues = { 
   "a",
   "T",
   "z",
   "a",
 };
 Char[]   expectedValues = { 
   'a' ,
   'T' ,
   'z' ,
   'a' ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
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
 expected = "System.ArgumentNullException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( null );
 iCountErrors++;
 strLoc = "Err_EXstriArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( ArgumentNullException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.FormatException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( "" );
 iCountErrors++;
 strLoc = "Err_EXstriArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_948fy! Uncaught Exception in Char Convert.ToChar( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 try {
 String expected = null;
 String[]   testValues = { 
   "a",
   "T",
   "z",
   "a",
 };
 Char[]   expectedValues = { 
   'a' ,
   'T' ,
   'z' ,
   'a' ,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i], new NumberFormatInfo() );
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
 expected = "System.ArgumentNullException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( null , new NumberFormatInfo());
 iCountErrors++;
 strLoc = "Err_EXstriArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( ArgumentNullException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.FormatException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( "" , new NumberFormatInfo());
 iCountErrors++;
 strLoc = "Err_EXstriArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 try {
 iCountTestcases++;
 Char result = Convert.ToChar( "ab" , new NumberFormatInfo());
 iCountErrors++;
 strLoc = "Err_EXstriArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( FormatException) {
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_948fy! Uncaught Exception in Char Convert.ToChar( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Int16" );
 try {
 String expected = null;
 Int16[]   testValues = { 
   Int16.MaxValue ,
   0 ,
 };
 Char[]   expectedValues = { 
   (Char)Int16.MaxValue,
   (Char)0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_vint16Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint16Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( Int16.MinValue );
 iCountErrors++;
 strLoc = "Err_EXint16ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXint16ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXint16ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( (Int16)(-1000)  );
 iCountErrors++;
 strLoc = "Err_EXint16ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXint16ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXint16ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_03479! Uncaught Exception in Char Convert.ToChar( Int16 )");
 Console.WriteLine( "Exception->" + e.ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Byte" );
 try {
 Byte[]   testValues = { 
   Byte.MaxValue ,
   Byte.MinValue ,
 };
 Char[]   expectedValues = { 
   (Char)Byte.MaxValue,
   (Char)Byte.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
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
 Console.WriteLine( "Error_30848! Uncaught Exception in Char Convert.ToChar( Byte )");
 Console.WriteLine( "Exception->" + e.ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: SByte" );
 try {
 String expected = null;
 SByte[]   testValues = { 
   SByte.MaxValue ,
   0 ,
 };
 Char[]   expectedValues = { 
   (Char)SByte.MaxValue,
   (Char)0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
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
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( SByte.MinValue );
 iCountErrors++;
 strLoc = "Err_EXSByteArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXSByteArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXSByteArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( (SByte)( -100 ) );
 iCountErrors++;
 strLoc = "Err_EXSByteArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXSByteArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXSByteArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( (SByte)(-1) );
 iCountErrors++;
 strLoc = "Err_EXUInt32ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_85787! Uncaught Exception in Char Convert.ToChar( SByte )");
 Console.WriteLine( "Exception->" + e.ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt32" );
 try {
 String expected = null;
 UInt32[]   testValues = { 
   UInt16.MaxValue ,
   0 ,
 };
 Char[]   expectedValues = { 
   (Char)UInt16.MaxValue,
   (Char)0,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Char result = Convert.ToChar( testValues[i] );
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
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( UInt32.MaxValue );
 iCountErrors++;
 strLoc = "Err_EXUInt32ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.OverflowException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 Char result = Convert.ToChar( UInt32.MaxValue );
 iCountErrors++;
 strLoc = "Err_EXUInt32ArA1";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXUInt32ArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
 Console.WriteLine( "Exception->" + e.ToString() );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try 
   {
   Char[]   testValues = {Char.MaxValue, Char.MinValue, 'b'};
   Char[]   expectedValues = { Char.MaxValue,Char.MinValue,'b'};
   for( int i = 0; i < testValues.Length; i++ ) 
     {	
     iCountTestcases++;
     if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
     try 
       {
       Char result = Convert.ToChar( testValues[i] );
       if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
       if ( !result.Equals( expectedValues[i] ) ) 
	 {
	 iCountErrors++;
	 strLoc = "Err_vUInt32Ar," + i;
	 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
				  "' ... Received = '" +  result + "'." );
	 }
       }
     catch( Exception e ) 
       {
       iCountErrors++;
       strLoc = "Err_xUInt32Ar," + i;
       Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
       }
     }
   } 
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16" );
 try 
   {
   UInt16[]   testValues = {0, 98, UInt16.MaxValue};
   Char[]   expectedValues = { (Char) 0,'b',Char.MaxValue};
   for( int i = 0; i < testValues.Length; i++ ) 
     {	
     iCountTestcases++;
     if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
     try 
       {
       Char result = Convert.ToChar( testValues[i] );
       if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
       if ( !result.Equals( expectedValues[i] ) ) 
	 {
	 iCountErrors++;
	 strLoc = "Err_vUInt32Ar," + i;
	 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
				  "' ... Received = '" +  result + "'." );
	 }
       }
     catch( Exception e ) 
       {
       iCountErrors++;
       strLoc = "Err_xUInt32Ar," + i;
       Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
       }
     }
   } 
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: Int64" );
 try 
   {
   String expected = null;	
   Int64[]   testValues = {0, 98, UInt16.MaxValue};
   Char[]   expectedValues = { (Char) 0,'b',Char.MaxValue};
   for( int i = 0; i < testValues.Length; i++ ) 
     {	
     iCountTestcases++;
     if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
     try 
       {
       Char result = Convert.ToChar( testValues[i] );
       if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
       if ( !result.Equals( expectedValues[i] ) ) 
	 {
	 iCountErrors++;
	 strLoc = "Err_vUInt32Ar," + i;
	 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
				  "' ... Received = '" +  result + "'." );
	 }
       }
     catch( Exception e ) 
       {
       iCountErrors++;
       strLoc = "Err_xUInt32Ar," + i;
       Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
       }
     }
   expected = "System.OverflowException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (Int64) (UInt16.MaxValue  + 1));
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( OverflowException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.OverflowException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (Int64) (-1) );
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( OverflowException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   }
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: UInt64" );
 try 
   {
   String expected = null;	
   UInt64[]   testValues = {0, 98, UInt16.MaxValue};
   Char[]   expectedValues = { (Char) 0,'b',Char.MaxValue};
   for( int i = 0; i < testValues.Length; i++ ) 
     {	
     iCountTestcases++;
     if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
     try 
       {
       Char result = Convert.ToChar( testValues[i] );
       if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
       if ( !result.Equals( expectedValues[i] ) ) 
	 {
	 iCountErrors++;
	 strLoc = "Err_vUInt32Ar," + i;
	 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
				  "' ... Received = '" +  result + "'." );
	 }
       }
     catch( Exception e ) 
       {
       iCountErrors++;
       strLoc = "Err_xUInt32Ar," + i;
       Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
       }
     }
   expected = "System.OverflowException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (UInt64) (UInt16.MaxValue  + 1));
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( OverflowException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   }
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: Single" );
 try 
   {
   String expected = null;	
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (Single) (0));
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Single.MinValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Single.MaxValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }  	  	
   }
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: Double" );
 try 
   {
   String expected = null;	
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (Double) (0));
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Double.MinValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Double.MaxValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }  	  	
   }
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 if ( verbose ) Console.WriteLine( "Testing Method: Decimal" );
 try 
   {
   String expected = null;	
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( (Decimal) (0));
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Decimal.MinValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }
   expected = "System.InvalidCastException";
   try 
     {
     iCountTestcases++;
     if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
     Char result = Convert.ToChar( Decimal.MaxValue);
     iCountErrors++;
     strLoc = "Err_EXUInt32ArA1";
     Console.WriteLine( strLoc + " Exception not Thrown!" );
     } 
   catch( InvalidCastException e ) 
     {
     if ( !e.GetType().FullName.Equals( expected ) ) 
       {
       iCountErrors++;
       strLoc = "Err_EXUInt32ArB1";
       Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
       }
     } 
   catch( Exception e ) 
     {
     iCountErrors++;
     strLoc = "Err_EXUInt32ArC1";
     Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
     }  	  	
   }
 catch( Exception e ) 
   {
   iCountErrors++;
   Console.WriteLine( "Error_r9h49! Uncaught Exception in Char Convert.ToChar( UInt32 )");
   Console.WriteLine( "Exception->" + e.ToString() );
   }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Char bTest = Convert.ToChar (new Object());	
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
   Char bTest = Convert.ToChar (new Object(), new NumberFormatInfo());	
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
   Char bTest = Convert.ToChar (DateTime.Now);	
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
   Co6055ToChar_all cbX = new Co6055ToChar_all();
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
     Console.Error.WriteLine( "Try 'cb6054ToByte_all.exe -v' to see tests..." );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
