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
using System.Threading;
using System.Globalization;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
class Co6053ToBoolean_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(SByte)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToBoolean", new Type[]{typeof(Decimal)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToBoolean( all )";
 public static String s_strTFName        = "Cb6053ToBoolean_all";
 public static String s_strTFAbbrev      = "Cb6053";
 public static String s_strTFPath        = "";
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
 Int32[]  int3Array = { 
   Int32.MinValue,
   Int32.MaxValue,
   ((Int32) 0 ),
 };
 Boolean[]  int3Results = { 
   true,
   true,
   false,
 };
 for( int i=0; i < int3Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( int3Array[i] );
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
 Int64[]  int6Array = { 
   Int64.MinValue,
   Int64.MaxValue,
   ((Int64) 0 ),
 };
 Boolean[]  int6Results = { 
   true,
   true,
   false,
 };
 for( int i=0; i < int6Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( int6Array[i] );
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
 Int16[]  int1Array = { 
   Int16.MinValue,
   Int16.MaxValue,
   ((Int16)0),
 };
 Boolean[]  int1Results = { 
   true,
   true,
   false,
 };
 for( int i=0; i < int1Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( int1Array[i] );
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
 String[]  striArray = { 
   "True",
   "true ",
   " true",
   " true ",
   " false ",
   " false",
   "false ",
   "False",
 };
 Boolean[]  striResults = { 
   true,
   true,
   true,
   true,
   false,
   false,
   false,
   false,
 };
 for( int i=0; i < striArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( striArray[i] );
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
 try {
 iCountTestcases++;
 Boolean result = Convert.ToBoolean( null );
 if(result){
 iCountErrors++;
 strLoc = "Err_xstriA1";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 }
 }catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB1";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 try {
 iCountTestcases++;
 String[] tsA = {null}; 
 Boolean result = Convert.ToBoolean( tsA[0] );
 if(result){
 iCountErrors++;
 strLoc = "Err_xstriA2";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 }
 }catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB2";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 try {
 iCountTestcases++;
 Boolean result = Convert.ToBoolean( "Blah" );
 iCountErrors++;
 strLoc = "Err_xstriA3";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 } catch( FormatException) {}
 catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB3";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 for( int i=0; i < striArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( striArray[i], new NumberFormatInfo() );
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
 try {
 iCountTestcases++;
 Boolean result = Convert.ToBoolean( null, new DateTimeFormatInfo() );
 if(result){
 iCountErrors++;
 strLoc = "Err_xstriA1";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 }
 }catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB1";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 try {
 iCountTestcases++;
 String[] tsA = {null}; 
 Boolean result = Convert.ToBoolean( tsA[0], new DateTimeFormatInfo() );
 if(result){
 iCountErrors++;
 strLoc = "Err_xstriA2";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 }
 }catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB2";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 try {
 iCountTestcases++;
 Boolean result = Convert.ToBoolean( "Blah", new DateTimeFormatInfo() );
 iCountErrors++;
 strLoc = "Err_xstriA3";
 Console.Error.WriteLine( strLoc + " Exception not thrown." );
 } catch( FormatException) {}
 catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xstriB3";
 Console.Error.WriteLine( strLoc + " Wrong Exception thrown." + e.GetType().FullName );
 }
 Byte[]  SByteArray = { 
   Byte.MinValue,
   Byte.MaxValue,
 };
 Boolean[]  SByteResults = { 
   false,
   true,
 };
 for( int i=0; i < SByteArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( SByteArray[i] );
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
 SByte[]  sSByteArray = { 
   0,
   SByte.MaxValue,
 };
 Boolean[]  sSByteResults = { 
   false,
   true,
 };
 for( int i=0; i < sSByteArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( sSByteArray[i] );
 if ( !result.Equals( sSByteResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_sSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + sSByteResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xSByteAr," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 UInt16[]  UInt3216Array = { 
   UInt16.MinValue,
   UInt16.MaxValue,
   UInt16.MinValue,
   UInt16.MaxValue,
   ((UInt16)0),
 };
 Boolean[]  UInt3216Results = { 
   false,
   true,
   false,
   true,
   false,
 };
 for( int i=0; i < UInt3216Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( UInt3216Array[i] );
 if ( !result.Equals( UInt3216Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint16Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3216Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint16Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 UInt32[]  UInt3232Array = { 
   UInt32.MinValue,
   UInt32.MaxValue
 };
 Boolean[]  UInt3232Results = { 
   false,
   true,
   false,
 };
 for( int i=0; i < UInt3232Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( UInt3232Array[i] );
 if ( !result.Equals( UInt3232Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint32Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3232Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint32Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 UInt64[]  UInt3264Array = { 
   UInt64.MinValue,
   UInt64.MaxValue,
   new UInt64(),
 };
 Boolean[]  UInt3264Results = { 
   false,
   true,
   false,
 };
 for( int i=0; i < UInt3264Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( UInt3264Array[i] );
 if ( !result.Equals( UInt3264Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint64Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + UInt3264Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint64Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 Boolean[]  Boolean3312Array = { 
   true,
   false
 };
 Boolean[]  Boolean3312Results = { 
   true,
   false,
   false
 };
 for( int i=0; i < Boolean3312Array.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( Boolean3312Array[i] );
 if ( !result.Equals( Boolean3312Results[i] ) ) {
 iCountErrors++;
 strLoc = "Err_rint634Ar," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + Boolean3312Results[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_xint634Ar," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 Char[]  Char3352Array = { 
   Char.MinValue,
   Char.MaxValue,
   'b'
 };
 for( int i=0; i < Char3352Array.Length; i++ ) {
 iCountTestcases++;
 try 
   {
   Boolean result = Convert.ToBoolean( Char3352Array[i] );
   iCountErrors++;
   strLoc = "Err_cij394," + i;
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   } 
 catch( InvalidCastException ) 
   {
   }            
 catch( Exception e ) 
   {
   iCountErrors++;
   strLoc = "Err_xint635Ar," + i;
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 } 
 Single[]  sglArray = { 
   Single.Epsilon,
   Single.MaxValue,
   Single.MinValue,
   Single.NaN,
   Single.NegativeInfinity,
   Single.PositiveInfinity,
   0f,
   0.0f,
   1.5f,
   -1.5f,
   1.5e30f,
   -1.7e-100f,
   -1.7e30f,
   -1.7e-40f,
 };
 Boolean[]  sglResults = { 
   true,
   true,
   true,
   true,
   true,
   true,
   false,
   false,
   true,
   true,
   true,
   false,
   true,
   true,
 };
 for( int i=0; i < sglArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( sglArray[i] );
 if ( !result.Equals( sglResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_34sfdgs," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + sglResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_453ewgs," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 Double[]  dblArray = { 
   Double.Epsilon,
   Double.MaxValue,
   Double.MinValue,
   Double.NaN,
   Double.NegativeInfinity,
   Double.PositiveInfinity,
   0d,
   0.0,
   1.5,
   -1.5,
   1.5e300,
   -1.7e-500,
   -1.7e300,
   -1.7e-320,
 };
 Boolean[]  dblResults = { 
   true,
   true,
   true,
   true,
   true,
   true,
   false,
   false,
   true,
   true,
   true,
   false,
   true,
   true,
 };
 for( int i=0; i < dblArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( dblArray[i] );
 if ( !result.Equals( dblResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_5324," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + dblResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_gdf," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 Decimal[]  decArray = { 
   Decimal.MaxValue,
   Decimal.MinValue,
   Decimal.One,
   Decimal.Zero,
   0m,
   0.0m,
   1.5m,
   -1.5m,
   500.00m,
 };
 Boolean[]  dcmResults = { 
   true,
   true,
   true,
   false,
   false,
   false,
   true,
   true,
   true,
 };
 for( int i=0; i < decArray.Length; i++ ) {
 iCountTestcases++;
 try {
 Boolean result = Convert.ToBoolean( decArray[i] );
 if ( !result.Equals( dcmResults[i] ) ) {
 iCountErrors++;
 strLoc = "Err_2533245gfsdg," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + dcmResults[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_435dfg," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 strLoc = "Err_34sdg,";
 try
   {
   iCountTestcases++;
   Boolean bTest = Convert.ToBoolean (DateTime.Now);	
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
   Boolean bTest = Convert.ToBoolean (new Object());	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_98734tsg,";
 try
   {
   iCountTestcases++;
   Boolean bTest = Convert.ToBoolean (new Object(), new NumberFormatInfo());	
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
   Co6053ToBoolean_all cbX = new Co6053ToBoolean_all();
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
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
