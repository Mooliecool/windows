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
class Co6012CompareTo_Ob {
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer = "";
 public static String s_strComponentBeingTested = "UInt16.CompareTo( Object )";
 public static String s_strTFName = "Co6012CompareTo_Ob.cs";
 public static String s_strTFAbbrev = "Co6012";
 public static String s_strTFPath = "";
 public Boolean verbose = false;
 public Boolean runTest() {
 Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
 String strLoc = "Loc_000ooo";
 int iCountTestcases = 0;
 int iCountErrors = 0;
 if ( verbose ) Console.WriteLine( "Testing Method: UInt16.CompareTo( Object )" );
 try {
 UInt16[]   baseValues = { 
   UInt16.MinValue,
   UInt16.MinValue,
   UInt16.MinValue,
   UInt16.MaxValue,
   UInt16.MaxValue,
   UInt16.MaxValue,
   UInt16.MinValue,      
   UInt16.MaxValue,      
 };
 Object[]   testValues = { 
   UInt16.MinValue,
   (UInt16) 100,
   UInt16.MaxValue,
   UInt16.MinValue,
   (UInt16) 100,
   UInt16.MaxValue,
   null,         
   null,         
 };
 Int32[]   expectedValues = { 
   0,
   -1,
   -1,
   1,
   1,
   0,
   1,                    
   1,                    
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 Int32 result = baseValues[i].CompareTo( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) &&
      !(result > 0 && expectedValues[i] >0) &&
      !(result < 0 && expectedValues[i] < 0)) {
 iCountErrors++;
 strLoc = "Err_VN1," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_VE1," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 UInt16   baseValue = UInt16.MinValue;
 String[] expectedExceptions = { 
   "System.ArgumentException",
   "System.ArgumentException",
   "System.ArgumentException",
   "System.ArgumentException",
 };
 Object[]   errorValues = { 
   UInt32.MinValue,
   UInt64.MinValue,
   Int16.MinValue,
   Int16.MaxValue,
 };
 for( int i = 0; i < expectedExceptions.Length; i++ ) {
 iCountTestcases++;
 try {
 if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
 Int32 result = baseValue.CompareTo( errorValues[i] );
 iCountErrors++;
 strLoc = "Err_NE1," + i;
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( Exception e ) {
 if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) {
 iCountErrors++;
 strLoc = "Err_WE1," + i;
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 }
 }
 } catch( Exception e ) {
 Console.WriteLine( "Uncaught Exception in Int32 UInt16.CompareTo( Object )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 Console.Error.Write( s_strTFName );
 Console.Error.Write( ": " );
 if ( iCountErrors == 0 ) {
 Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases + " paSs" );
 return true;
 } else {
 Console.Error.WriteLine( s_strTFPath + s_strTFName + ".cs" );
 Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases );
 Console.Error.WriteLine( "FAiL" );
 Console.Error.WriteLine( " iCountErrors==" + iCountErrors );
 return false;
 }
 }
 public static void Main( String[] args ) { 
 Boolean bResult = false; 
 Co6012CompareTo_Ob cbX = new Co6012CompareTo_Ob();
 try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } catch( Exception e ) {}
 try {
 bResult = cbX.runTest();
 } catch ( Exception exc_main ) {
 bResult = false;
 Console.WriteLine( "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
 }
 if ( ! bResult ) {
 Console.WriteLine( s_strTFPath + s_strTFName );
 Console.Error.WriteLine( " " );
 Console.Error.WriteLine( "Try '" + s_strTFName + ".exe -v' to see tests..." );
 Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
 Console.Error.WriteLine( " " );
 }
 if ( bResult == true ) Environment.ExitCode = 0;
 else Environment.ExitCode = 1; 
 }
}  
