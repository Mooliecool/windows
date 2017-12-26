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
public class Co6026CompareTo_Object
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "UInt32.CompareTo( Object )";
 public static String s_strTFName        = "Co6026CompareTo_Object.cs";
 public static String s_strTFAbbrev      = "Co6026";
 public static String s_strTFPath        = "";
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = false;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   String strBaseLoc;
   try
     {
     m_strLoc = "Loc_normalTests";
     UInt32 primitiveUInt = 100;
     UInt32 testUIntOne = ((UInt32) primitiveUInt );
     UInt32 testUIntTwo = ((UInt32) 2 * primitiveUInt );
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to self";
     if( testUIntOne.CompareTo( testUIntOne ) != 0 ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to lesser";
     if( testUIntTwo.CompareTo( testUIntOne ) < 0 ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to lesser";
     if( testUIntTwo.CompareTo( testUIntOne ) < 0 ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "testUIntOne = " + testUIntOne );
     Console.WriteLine( "testUIntTwo = " + testUIntTwo );
     Console.WriteLine( "two.CompareTo( one ) = " + testUIntTwo.CompareTo( testUIntOne ) );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to greater";
     if( testUIntOne.CompareTo( testUIntTwo ) > 0 ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to greater";
     if( testUIntOne.CompareTo( testUIntTwo ) > 0 ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to String";
     testUIntOne.CompareTo(  "Throw Something"  );
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "No Exception thrown" );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     catch( ArgumentException e ) {}
     catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"] -- Wrong Exception!" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to Int16";
     testUIntOne.CompareTo( ((Int16) 12 ) );
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "No Exception thrown" );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     catch( ArgumentException e ) {}
     catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"] -- Wrong Exception!" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to primitive UInt16";
     UInt16 pInt = 100;
     if( testUIntOne.CompareTo( pInt ) < 0 ) ErrorCode();
     }
     catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"] -- Wrong Exception!" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to primitive UInt32";
     if ( testUIntOne.CompareTo( primitiveUInt ) != 0 ) ErrorCode();
     }
     catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"] -- Wrong Exception!" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Compare to primitive short";
     short pShort = 100;
     testUIntOne.CompareTo( pShort );
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "No Exception thrown" );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     catch( ArgumentException e ) {}
     catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"] -- Wrong Exception!" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( "Error Err_8888yyy ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after m_strLoc=="+ m_strLoc +" ,exc_general=="+ exc_general );
     }
   Console.Write(Environment.NewLine);
   Console.WriteLine( "Total Tests Ran: " + iCountTestcases + " Failed Tests: " + iCountErrors );
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public void ErrorCode() {
 iCountErrors++;
 throw new Exception( "Test failed." );
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode = 1;  
   Boolean bResult = false; 
   Co6026CompareTo_Object cbX= new Co6026CompareTo_Object();
   try
     {
     bResult = cbX.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( "FAiL!  Error Err_9999zzz ("+ s_strTFAbbrev +")!  Uncaught Exception caught fell to Main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFPath + s_strTFName );
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
