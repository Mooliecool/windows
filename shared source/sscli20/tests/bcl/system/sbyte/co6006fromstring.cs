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
using System.Text;
public class Co6006FromString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "SByte.FromString(String)";
 public static String s_strTFName        = "Co6006FromString.cs";
 public static String s_strTFAbbrev      = "Co6006";
 public static String s_strTFPath        = "";
 public static Int32 DBGLEV = 0;  
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = false;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   StringBuilder sblMsg = new StringBuilder( 99 );
   m_verbose = false;
   try
     {
     m_strLoc = "Loc_0-255";
     for ( int i = SByte.MinValue; i <= SByte.MaxValue; i++ )  {
     TestValue( ((IConvertible)Convert.ToInt16(i)).ToString( null ) , Convert.ToSByte(i), "." );
     }
     TestValue( "001", (SByte)Convert.ToChar( 1 ), "_" );
     TestValue( "   1   ", (SByte)Convert.ToChar( 1 ), "_" );
     TestValue( "1    ", (SByte)Convert.ToChar( 1 ), "_" );
     TestValue( "   1", (SByte)Convert.ToChar( 1 ), "_" );
     m_strLoc = "Loc_exepTests";
     TestException("exception", "System.FormatException", "f" );
     TestException("-130", "System.OverflowException", "^" );
     TestException("256", "System.OverflowException", "v" );
     TestException("3.6", "System.FormatException", "f" );
     TestException(null, "System.ArgumentNullException", "A" );
     Console.Write(Environment.NewLine);
     Console.WriteLine( "Total Tests Ran: " + iCountTestcases + " Failed Tests: " + iCountErrors );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( "Error Err_8888yyy ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after m_strLoc=="+ m_strLoc +" ,exc_general=="+ exc_general );
     }
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
 private void TestValue( String test, SByte expected, String moniker ) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestValue'"+test+"'";
 try {
 SByte btest = SByte.Parse( test );
 if ( btest != expected ) {
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+btest+" expected "+expected );
 }
 } catch ( Exception e ) {
 Console.WriteLine( e +"->"+ m_strLoc );
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"' expected "+expected );
 }
 }
 private void TestException( String test, String expected, String moniker) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestException'"+test+"'";
 try {
 SByte.Parse( test );
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "No Exception Thrown " +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected. '"+ test +"' did not throw an exception!"); 
 }
 catch ( Exception e ) {
 if( e.GetType().FullName.Equals( expected ) ) return;
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + e +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected, '"+e.ToString()+"' received. "); 
 return;
 }
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode = 1;  
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co6006FromString cbX = new Co6006FromString();
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
