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
using System.Reflection;
using System.Collections;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
class Co6054ToByte_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Decimal)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(String), typeof(Int32)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Object)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToByte( all )";
 public static String s_strTFName        = "Cb6054ToByte_all";
 public static String s_strTFAbbrev      = "Cb6054";
 public static String s_strTFPath        = "";
 public Boolean verbose = false;
 public static String[] s_strMethodsCovered = 
 {
   "Method_Covered:  Convert.ToByte( Boolean )"           
   ,"Method_Covered:  Convert.ToByte( SByte )"              
   ,"Method_Covered:  Convert.ToByte( Double )"            
   ,"Method_Covered:  Convert.ToByte( Single )"            
   ,"Method_Covered:  Convert.ToByte( Int32 )"             
   ,"Method_Covered:  Convert.ToByte( Int64 )"             
   ,"Method_Covered:  Convert.ToByte( Int16 )"             
   ,"Method_Covered:  Convert.ToByte( Decimal )"          
   ,"Method_Covered:  Convert.ToByte( Decimal )"           
   ,"Method_Covered:  Convert.ToByte( String )"            
   ,"Method_Covered:  Convert.ToByte( String, Int32 )"     
   ,"Method_Covered: Convert.ToByte( UInt16 )"
   ,"Method_Covered: Convert.ToByte( UInt32 )"
   ,"Method_Covered: Convert.ToByte( UInt64 )"
   ,"Method_Covered: Convert.ToByte( Char )"
   ,"Method_Covered: Convert.ToByte( Byte )"
   ,"Method_Covered: Convert.ToByte( Object )"
 };
 public static void printoutCoveredMethods()
   {
   Console.Error.WriteLine( "" );
   Console.Error.WriteLine( "Method_Count==16 ("+ s_strMethodsCovered.Length +"==confirm) !!" );
   Console.Error.WriteLine( "" );
   for ( int ia = 0 ;ia < s_strMethodsCovered.Length ;ia++ )
     {
     Console.Error.WriteLine( s_strMethodsCovered[ia] );
     }
   Console.Error.WriteLine( "" );
   }
 public Boolean runTest() {
 Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
 String strLoc="Loc_000oo";
 int iCountTestcases = 0;
 int iCountErrors = 0;
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Boolean )" );
 try {   Boolean[]  boolArray = { 
   true,
   false,
 };
 Byte[]  boolResults = { 
   1,
   0,
   0,
 };
 for( int i=0; i < boolArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + boolArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( boolArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + boolResults[i] );
 if ( !result.Equals( boolResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rboolAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + boolResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xboolAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Boolean )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( SByte )" );
 try {   SByte[]  SByteArray = { 
   ((SByte) 10 ),
   ((SByte) 0 ),
 };
 Byte[]  SByteResults = { 
   (Byte)10,
   (Byte)0,
 };
 for( int i=0; i < SByteArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + SByteArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( SByteArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + SByteResults[i] );
 if ( !result.Equals( SByteResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + SByteResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (SByte)( -100 ));
 iCountErrors++;
 strLoc = "Err_xSByteA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xSByteB2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xSByteC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Byte )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Double )" );
 try {   Double[]  doubArray = { 
   (Double)Byte.MinValue,
   (Double)Byte.MaxValue,
   0.0,
   100.0,
   254.9,
 };
 Byte[]  doubResults = { 
   Byte.MinValue,
   Byte.MaxValue,
   0,
   100,
   255
 };
 for( int i=0; i < doubArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + doubArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( doubArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + doubResults[i] );
 if ( !result.Equals( doubResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rdoubAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + doubResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdoubAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (Double)(-100) );
 iCountErrors++;
 strLoc = "Err_xdoubA1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xdoubB1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdoubC1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (Double)1000);
 iCountErrors++;
 strLoc = "Err_xdoubA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xdoubB2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdoubC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Double )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Single )" );
 try {   Single[]  singArray = { 
   (Single)Byte.MaxValue,
   (Single)Byte.MinValue,
   254.01f,
 };
 Byte[]  singResults = { 
   Byte.MaxValue,
   Byte.MinValue,
   254,
 };
 for( int i=0; i < singArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + singArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( singArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + singResults[i] );
 if ( !result.Equals( singResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rsingAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + singResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xsingAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (Single)(-100 ) );
 iCountErrors++;
 strLoc = "Err_xsingA1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xsingB1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xsingC1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (Single) 1000 );
 iCountErrors++;
 strLoc = "Err_xsingA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xsingB2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xsingC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Single )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Int32 )" );
 try {   Int32[]  int3Array = { 
   10,
   0,
   100,
   255,
 };
 Byte[]  int3Results = { 
   10,
   0,
   100,
   255,
 };
 for( int i=0; i < int3Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + int3Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( int3Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + int3Results[i] );
 if ( !result.Equals( int3Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint3Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + int3Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint3Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( -100 );
 iCountErrors++;
 strLoc = "Err_xint3A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xint3B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint3C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( 1000 );
 iCountErrors++;
 strLoc = "Err_xint3A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xint3B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint3C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Int64 )" );
 try {   Int64[]  int6Array = { 
   10,
   100,
 };
 Byte[]  int6Results = { 
   (Byte)10,
   (Byte)100,
 };
 for( int i=0; i < int6Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + int6Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( int6Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + int6Results[i] );
 if ( !result.Equals( int6Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint6Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + int6Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint6Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (Int64)(-100 ));
 iCountErrors++;
 strLoc = "Err_xInt6A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xInt6B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xInt6C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (Int64) 1000  );
 iCountErrors++;
 strLoc = "Err_xInt6A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xInt6B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xInt6C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Int64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Int16 )" );
 try {   Int16[]  int1Array = { 
   0,
   255,
   100,
   2,
 };
 Byte[]  int1Results = { 
   0,
   255,
   100,
   2,
 };
 for( int i=0; i < int1Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + int1Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( int1Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + int1Results[i] );
 if ( !result.Equals( int1Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + int1Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint1Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (Int16)( -100 ) );
 iCountErrors++;
 strLoc = "Err_xint1A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xint1B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint1C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (Int16) 1000 );
 iCountErrors++;
 strLoc = "Err_xint1A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xint1B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint1C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Int16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( Decimal )" );
 try {   Decimal[]  deciArray = { 
   (Decimal)Byte.MaxValue,
   (Decimal)Byte.MinValue,
   new Decimal( 254.01 ),
   (Decimal)(Byte.MaxValue/2 ),
 };
 Byte[]  deciResults = { 
   Byte.MaxValue,
   Byte.MinValue,
   254,
   Byte.MaxValue/2,
 };
 for( int i=0; i < deciArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + deciArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( deciArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + deciResults[i] );
 if ( !result.Equals( deciResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rdeciAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + deciResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdeciAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( (Decimal) (-100 ) );
 iCountErrors++;
 strLoc = "Err_xdeciA1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xdeciB1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdeciC1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (Decimal)( 1000 ) );
 iCountErrors++;
 strLoc = "Err_xdeciA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xdeciB2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xdeciC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( Decimal )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( String )" );
 try {   String[]  striArray = { 
   Byte.MaxValue.ToString(),
   Byte.MinValue.ToString(),
 };
 Byte[]  striResults = { 
   Byte.MaxValue,
   Byte.MinValue,
 };
 for( int i=0; i < striArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + striArray[i] + "..."  );
 try {
 Byte result = Convert.ToByte( striArray[i] );
 if ( verbose ) Console.WriteLine( result + "==" + striResults[i] );
 if ( !result.Equals( striResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rstriAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + striResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Argument null"  );
 try {
 String [] dummy = { null, }; 
 Byte result = Convert.ToByte( dummy[0] );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstriA1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }
 } catch( ArgumentNullException e ) {
 if ( !e.GetType().FullName.Equals( "System.ArgumentNullException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstriB1";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriC1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : UpperBound"  );
 try {
 Byte result = Convert.ToByte( "256" );
 iCountErrors++;
 strLoc = "Err_xstriA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstriB2";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : LowerBound"  );
 try {
 Byte result = Convert.ToByte( "-1" );
 iCountErrors++;
 strLoc = "Err_xstriA4";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstriB4";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriC4";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Format"  );
 try {
 Byte result = Convert.ToByte( "!56" );
 iCountErrors++;
 strLoc = "Err_xstriA3";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstriB3";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriC3";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( String, Int32 )" );
 try {   String[]  striArray = { 
   "10",
   "100",
   "1011",
   "ff",
   "0xff",
   "77",
   "11",
   "11111111",
 };
 Int32[] striBase = { 
   10,
   10,
   2,
   16,
   16,
   8,
   2,
   2,
 };
 Byte[]  striResults = { 
   10,
   100,
   11,
   255,
   255,
   63,
   3,
   255,
 };
 for( int i=0; i < striArray.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + striArray[i] + "," + striBase[i] + "..."  );
 try {
 Byte result = Convert.ToByte( striArray[i], striBase[i] );
 if ( verbose ) Console.WriteLine( result + "==" + striResults[i] );
 if ( !result.Equals( striResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rstri2Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + striResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Argument (bad base)"  );
 try {
 String [] dummy = { null, }; 
 Byte result = Convert.ToByte( dummy[0], 11 );
 iCountErrors++;
 strLoc = "Err_xstri2A5";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( ArgumentException e ) {
 if ( !e.GetType().FullName.Equals( "System.ArgumentException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B5";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C5";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Argument null"  );
 try {
 String [] dummy = { null, }; 
 Byte result = Convert.ToByte( dummy[0], 10 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }
 result = Convert.ToByte( dummy[0], 2 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToByte( dummy[0], 8 );
 if(result!=0){
 iCountErrors++;
 strLoc = "Err_xstri2A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 }                      
 result = Convert.ToByte( dummy[0], 16 );
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
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : UpperBound"  );
 try {
 Byte result = Convert.ToByte( "256", 10 );
 iCountErrors++;
 strLoc = "Err_xstri2A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B2";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : UpperBound (binary)"  );
 try {
 Byte result = Convert.ToByte( "111111111", 2 );
 iCountErrors++;
 strLoc = "Err_xstri2A5";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B5";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C5";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : UpperBound (hex)"  );
 try {
 Byte result = Convert.ToByte( "ffffe", 16 );
 iCountErrors++;
 strLoc = "Err_xstri2A6";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B6";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C6";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : UpperBound (octal)"  );
 try {
 Byte result = Convert.ToByte( "7777777", 8 );
 iCountErrors++;
 strLoc = "Err_xstri2A7";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B7";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C7";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : ToString(hex)"  );
 try {
 Byte result = Convert.ToByte( "fffg", 16 );
 iCountErrors++;
 strLoc = "Err_xstri2A8";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B8";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C8";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : ToString(hex)"  );
 try {
 Byte result = Convert.ToByte( "0xxfff", 16 );
 iCountErrors++;
 strLoc = "Err_xstri2A8";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B8";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C8";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : ToString(octal)"  );
 try {
 Byte result = Convert.ToByte( "8", 8 );
 iCountErrors++;
 strLoc = "Err_xstri2A0";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B0";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C0";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : ToString(bin)"  );
 try {
 Byte result = Convert.ToByte( "112", 2 );
 iCountErrors++;
 strLoc = "Err_xstri2A9";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B9";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C9";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : LowerBound"  );
 try {
 Byte result = Convert.ToByte( "-1",10 );
 iCountErrors++;
 strLoc = "Err_xstri2A4";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B4";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C4";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Format"  );
 try {
 Byte result = Convert.ToByte( "!56", 10 );
 iCountErrors++;
 strLoc = "Err_xstri2A3";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( "System.FormatException" ) ) {
 iCountErrors++;
 strLoc = "Err_xstri2B3";
 Console.Error.WriteLine( strLoc + " More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstri2C3";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( String, Int32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( UInt16 )" );
 try {   UInt16[]  UInt3216Array = { 
   Byte.MinValue,
   Byte.MaxValue,
   (Byte) 10,
   (Byte) 100,
 };
 Byte[]  UInt3216Results = { 
   0,
   255,
   10,
   100,
 };
 for( int i=0; i < UInt3216Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + UInt3216Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( UInt3216Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + UInt3216Results[i] );
 if ( !result.Equals( UInt3216Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rUInt3216Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3216Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3216Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( UInt16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( UInt16 )" );
 try {   UInt16[]  UInt3216Array = { 
   (UInt16)10,
   (UInt16)0,
   (UInt16)100,
   (UInt16)255,
 };
 Byte[]  UInt3216Results = { 
   10,
   0,
   100,
   255,
 };
 for( int i=0; i < UInt3216Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + UInt3216Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( UInt3216Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + UInt3216Results[i] );
 if ( !result.Equals( UInt3216Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rUInt3216Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3216Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3216Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( ( -100 ) );
 iCountErrors++;
 strLoc = "Err_xUInt3216A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_UInt3216B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_UInt3216C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (UInt16) 256  );
 iCountErrors++;
 strLoc = "Err_xUInt3216A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xUInt3216B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3216C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( UInt16 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( UInt32 )" );
 try {   UInt32[]  UInt3232Array = { 
   (UInt32)10,
   (UInt32)0,
   (UInt32)100,
   (UInt32)255,
 };
 Byte[]  UInt3232Results = { 
   10,
   0,
   100,
   255,
 };
 for( int i=0; i < UInt3232Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + UInt3232Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( UInt3232Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + UInt3232Results[i] );
 if ( !result.Equals( UInt3232Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rUInt3232Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3232Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3232Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( 1000  );
 iCountErrors++;
 strLoc = "Err_xUInt3232A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_UInt3232B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_UInt3232C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (UInt32) 256 );
 iCountErrors++;
 strLoc = "Err_xUInt3232A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xUInt3232B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3232C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( UInt32 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Method: Byte Convert.ToByte( UInt64 )" );
 try {   UInt64[]  UInt3264Array = { 
   (UInt64)10,
   (UInt64)0,
   (UInt64)100,
   (UInt64)255,
 };
 Byte[]  UInt3264Results = { 
   10,
   0,
   100,
   255,
 };
 for( int i=0; i < UInt3264Array.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing : " + UInt3264Array[i] + "..."  );
 try {
 Byte result = Convert.ToByte( UInt3264Array[i] );
 if ( verbose ) Console.WriteLine( result + "==" + UInt3264Results[i] );
 if ( !result.Equals( UInt3264Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rUInt3264Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3264Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3264Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Lower Bound"  );
 try {
 Byte result = Convert.ToByte( 1000 );
 iCountErrors++;
 strLoc = "Err_xUInt3264A1";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_UInt3264B1";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_UInt3264C1";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( (UInt64)256 );
 iCountErrors++;
 strLoc = "Err_xUInt3264A2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xUInt3264B2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xUInt3264C2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {   Console.WriteLine( "Uncaught Exception in Byte Convert.ToByte( UInt64 )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: Char" );
 try {
 Char[]   testValues = { 
   'A',
   Char.MinValue,
 };
 Byte[]   expectedValues = { 
   (Byte)'A',
   (Byte)Char.MinValue,
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Byte result = Convert.ToByte( testValues[i] );
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
 iCountTestcases++;
 if ( verbose ) Console.WriteLine( "Testing : Upper Bound"  );
 try {
 Byte result = Convert.ToByte( Char.MaxValue );
 iCountErrors++;
 strLoc = "Err_xcharA2";
 Console.Error.WriteLine( strLoc + " No Exception Thrown." );
 } catch( OverflowException e ) {
 if ( !e.GetType().FullName.Equals( "System.OverflowException" ) ) {
 iCountErrors++;
 strLoc = "Err_xcharB2";
 Console.Error.WriteLine( strLoc + "More specific Exception thrown : " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xcharC2";
 Console.Error.WriteLine( strLoc + " Wrong Exception Thrown: " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_4789y! Uncaught Exception in Byte Convert.ToByte( Char )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   Byte bTest = Convert.ToByte (new Object());	
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
   System.String strFailMsg = null;
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
   Co6054ToByte_all cbX = new Co6054ToByte_all();
   try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } catch( Exception e ) {}
   try
     {
     printoutCoveredMethods();
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
     Console.Error.WriteLine( "Try 'Co6054ToByte_all.exe -v' to see tests..." );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
