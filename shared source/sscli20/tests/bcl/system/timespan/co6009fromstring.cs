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
using System.Threading;
public class Co6009FromString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "TimeSpan.Parse(String)";
 public static String s_strTFName        = "Co6009FromString.cs";
 public static String s_strTFAbbrev      = "Co6009";
 public static String s_strTFPath        = "";
 public static Int32 DBGLEV = 0;  
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = false;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   m_verbose = false;
   try
     {
     m_strLoc = "Loc_normalTests";
     int D = 3;      
     int h = 4;      
     int m = 5;      
     int s = 42;     
     TimeSpan test = new TimeSpan();
     String   teststr =  "" ;
     test = TimeSpan.MinValue;
     TestValue( test.ToString(  ), test, "r" );
     for ( D = 0; D < 100; D += 10 ) {
     for ( h = 0; h < 24; h+=5 ) {
     for ( m = 0; m < 60; m += 5 ) {
     for ( s = 0; s < 60; s += 15 ) {
     teststr =  D + "." + h +":"+m+":"+s ;
     test = new TimeSpan( D, h, m, s );
     TestValue( teststr, test, "." );
     } 
     }
     }
     }
     m_strLoc = "Loc_exepTests";
     TestException( null, "System.ArgumentNullException", "A" );
     TestException( "00.5555.12.09", "System.OverflowException", "F" );
     TestException( "", "System.FormatException", "F" );
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
 private void TestValue( String test, TimeSpan expected, String moniker ) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestValue'"+test+"'";
 try {
 TimeSpan dtest = TimeSpan.Parse( test );
 if ( dtest != expected ) {
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
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
 TimeSpan dtest = TimeSpan.Parse( test );
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
   Co6009FromString cbX = new Co6009FromString();
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
