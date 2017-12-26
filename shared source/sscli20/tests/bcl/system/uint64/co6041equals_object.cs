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
public class Co6041Equals_Object
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "UInt64.Equals( Object )";
 public static String s_strTFName        = "Co6041Equals_Object.cs";
 public static String s_strTFAbbrev      = "Co6041";
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
     UInt64 primativeULongA = 100;
     UInt64 primativeULongB = 200;
     UInt64 testUIntA = ((UInt64) primativeULongA );
     UInt64 testUIntAA = ((UInt64) primativeULongA );
     UInt64 testUIntB = ((UInt64) primativeULongB );
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals self";
     if( testUIntA.Equals( testUIntA ) != true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals same value";
     if( testUIntA.Equals( testUIntAA ) != true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals diff value";
     if( testUIntA.Equals( testUIntB ) == true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals primative UInt64";
     if( testUIntA.Equals( primativeULongA ) != true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals string 100";
     if( testUIntA.Equals(  "100" ) == true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     iCountTestcases++; 
     try {
     m_strLoc = "Starting testcase #" + iCountTestcases;
     m_strLoc = m_strLoc + "Equals new UInt64 same val";
     if( testUIntA.Equals( ((UInt64) primativeULongA ) ) != true ) ErrorCode();
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
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
 throw new Exception( "Test failed." );
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode = 1;  
   Boolean bResult = false; 
   Co6041Equals_Object cbX= new Co6041Equals_Object();
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
