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
public class Co6000Parse
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "Boolean.Parse(String)";
 public static String s_strTFName        = "Co6000Parse.cs";
 public static String s_strTFAbbrev      = "Co6000";
 public static String s_strTFPath        = "";
 public static Int32 DBGLEV = 0;  
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = true;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   StringBuilder sblMsg = new StringBuilder( 99 );
   m_verbose = true;
   try
     {
     m_strLoc = "Loc_trueTests";
     TestValue("true",true,"+");
     TestValue("truE",true,"+");
     TestValue("trUe",true,"+");
     TestValue("trUE",true,"+");
     TestValue("tRue",true,"+");
     TestValue("tRuE",true,"+");
     TestValue("tRUe",true,"+");
     TestValue("tRUE",true,"+");
     TestValue("True",true,"+");
     TestValue("TruE",true,"+");
     TestValue("TrUe",true,"+");
     TestValue("TrUE",true,"+");
     TestValue("TRue",true,"+");
     TestValue("TRuE",true,"+");
     TestValue("TRUe",true,"+");
     TestValue("TRUE",true,"+");
     TestValue("            TrUE",true,"+");
     TestValue("              TRuE",true,"+");
     TestValue("true ",true,"+");
     TestValue("TrUe           ",true,"+");
     TestValue("     tRue     ",true,"+");
     TestValue("            TrUE            ",true,"+");
     m_strLoc = "Loc_exepTests";
     TestException("truetrue", "System.FormatException", "f" );
     TestException("rue", "System.FormatException", "f" );
     TestException("false true", "System.FormatException", "f" );
     TestException("true false", "System.FormatException", "f" );
     TestException("", "System.FormatException", "f" );
     TestException( null , "System.ArgumentNullException", "A" );
     m_strLoc = "Loc_falseTests";
     TestValue("false", false, "-" );
     TestValue("FalsE", false, "-" );
     TestValue("fALSe", false, "-" );
     TestValue("falSE", false, "-" );
     TestValue("FAlse", false, "-" );
     TestValue("False", false, "-" );
     TestValue("fAlse", false, "-" );
     TestValue("faLse", false, "-" );
     TestValue("falSe", false, "-" );
     TestValue("falsE", false, "-" );
     TestValue("FALSE", false, "-" );
     TestValue("\n\n\t  FALSE \n \t", false, "-" );
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
 private void TestValue( String test, Boolean expected, String moniker ) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestValue'"+test+"'";
 try {
 if ( Boolean.Parse( test ) != expected ) {
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"' expected "+expected );
 }
 } catch ( Exception e ) {
 iCountErrors++;
 Console.WriteLine( e +"->"+ m_strLoc );
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"' expected "+expected );
 }
 }
 private void TestException( String test, String expected, String moniker) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestException'"+test+"'";
 try {
 Boolean.Parse( test );
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
   Co6000Parse cbX = new Co6000Parse();
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
